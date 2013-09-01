
#include "navi.h"

#include <grayb/mprov.h>

// Native nodes tree model

NatnTreeMdl::NatnTreeMdl(MProvider* aNatnProv): Glib::ObjectBase(typeid(NatnTreeMdl)), Glib::Object(), Gtk::TreeModel(), iNatnProv(aNatnProv),
    iStamp(55)
{
    iNatnProv->AppendNodesInfo(iNodesInfo);
}

NatnTreeMdl::~NatnTreeMdl()
{
    // Remove glue items
    for(tGiList::iterator iter = iGiList.begin(); iter != iGiList.end(); ++iter)
    {
	GlueItem* pItem = *iter;
	delete pItem;
    }

}

Glib::RefPtr<NatnTreeMdl> NatnTreeMdl::create(MProvider* aNatnProv)
{
    NatnTreeMdl* nmdl = new NatnTreeMdl(aNatnProv);
    Gtk::TreeModel* mdl = reinterpret_cast<Gtk::TreeModel*> (nmdl);
    GtkTreeModel* treemdl = mdl->gobj();
    bool ist = GTK_IS_TREE_MODEL(treemdl);
    return Glib::RefPtr<NatnTreeMdl>(nmdl);
}

int NatnTreeMdl::get_n_columns_vfunc() const
{
    return iColRec.size();
}

GType NatnTreeMdl::get_column_type_vfunc(int index) const
{
    return iColRec.types()[index];
}

int NatnTreeMdl::iter_n_root_children_vfunc() const
{
    return iNodesInfo.size();
}

bool NatnTreeMdl::get_iter_vfunc(const Path& path, iterator& iter) const
{
    bool res = false;
    unsigned psize = path.size();
    iter = iterator(); // Set is as invalid as default
    if (psize > 0 && psize <= 1) {
	iter.set_stamp(iStamp);
	int row_index = path[0];
	//iter.gobj()->user_data = (gpointer) &(iNodesInfo.at(row_index));
	iter.gobj()->user_data = AddGlueItem(row_index);
	res = true;
    }
    return res;
}

bool NatnTreeMdl::IsIterValid(const iterator& iter) const
{
  return (iStamp == iter.get_stamp());
}

bool NatnTreeMdl::iter_is_valid(const iterator& iter) const
{
  // Anything that modifies the model's structure should change the model's stamp, so that old iters are ignored.
  return IsIterValid(iter) && Gtk::TreeModel::iter_is_valid(iter);
}

void NatnTreeMdl::get_value_vfunc(const TreeModel::iterator& iter, int column, Glib::ValueBase& value) const
{
    if (IsIterValid(iter)) {
	if (column < iColRec.size()) {
	    Glib::Value<Glib::ustring> sval;
	    string data = iNodesInfo.at(GetRowIndex(iter));
	    sval.set(data.c_str());
	    GType coltype = get_column_type_vfunc(column);
	    value.init(coltype);
	    value = sval;
	}
    }
}

bool NatnTreeMdl::iter_next_vfunc(const iterator& iter, iterator& iter_next) const
{
    bool res = false;
    iter_next = iterator();
    if (IsIterValid(iter)) {
	iter_next.set_stamp(iStamp);
	int ri = GetRowIndex(iter);
	if (ri < iNodesInfo.size()) {
	    iter_next.gobj()->user_data = AddGlueItem(++ri);
	}
    }
    return res;
}

int NatnTreeMdl::iter_n_children_vfunc(const iterator& iter) const
{
    return 0;
}

bool NatnTreeMdl::iter_children_vfunc(const iterator& parent, iterator& iter) const
{
}

bool NatnTreeMdl::iter_has_child_vfunc(const iterator& iter) const
{
}

int NatnTreeMdl::GetRowIndex(const iterator& iter) const
{
    GlueItem* gi = static_cast<GlueItem*>(iter.gobj()->user_data);
    return gi->iRowIndex;
}

NatnTreeMdl::GlueItem* NatnTreeMdl::AddGlueItem(int aRowIndex) const
{
    GlueItem* item = new GlueItem(aRowIndex);
    iGiList.push_back(item);
    return item;
}


// Native nodes navigation widget
NaviNatN::NaviNatN()
{
}

NaviNatN::~NaviNatN()
{
}

void NaviNatN::SetDesEnv(MEnv* aDesEnv)
{
    iDesEnv = aDesEnv;
    set_model(NatnTreeMdl::create(iDesEnv->Provider()));
}


// Navigation widget
Navi::Navi(): iNatn(NULL)
{
    iNatn = new NaviNatN();
    iNatn->show();
    append_page(*iNatn, "Native");
}

Navi::~Navi()
{
    delete iNatn;
}

void Navi::SetDesEnv(MEnv* aDesEnv)
{
    iDesEnv = aDesEnv;
    iNatn->SetDesEnv(iDesEnv);
}


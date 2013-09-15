
#include "navi.h"

#include <grayb/mprov.h>

static GtkTargetEntry targetentries[] =
{
    { "STRING",        0, 0 },
    { "text/plain",    0, 1 },
    { "text/uri-list", 0, 2 },
};


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

Gtk::TreeModelFlags NatnTreeMdl::get_flags_vfunc() const
{
    return Gtk::TreeModelFlags(0);
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

Gtk::TreeModel::Path NatnTreeMdl::get_path_vfunc(const iterator& iter) const
{
    // TODO
    return Path();
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
	    GType coltype = get_column_type_vfunc(column);
	    Glib::Value<Glib::ustring> sval;
	    sval.init(coltype);
	    string data = iNodesInfo.at(GetRowIndex(iter));
	    sval.set(data.c_str());
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
	if (++ri < iNodesInfo.size()) {
	    iter_next.gobj()->user_data = AddGlueItem(ri);
	    res = true;
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
  return iter_nth_child_vfunc(parent, 0, iter);
}

bool NatnTreeMdl::iter_has_child_vfunc(const iterator& iter) const
{
    return (iter_n_children_vfunc(iter) > 0);
}

bool NatnTreeMdl::iter_nth_child_vfunc(const iterator& parent, int n, iterator& iter) const
{
    bool res = false;
    iter = iterator();
    if (IsIterValid(parent)) {
	// To update further
    }
    return res;
}

bool NatnTreeMdl::iter_nth_root_child_vfunc(int n, iterator& iter) const
{
    bool res = false;
    iter = iterator();
    if (n < iNodesInfo.size()) {
	iter.set_stamp(iStamp);
	iter.gobj()->user_data = AddGlueItem(n);
	res = true;
    }
    return res;
}

bool NatnTreeMdl::iter_parent_vfunc(const iterator& child, iterator& iter) const
{
    bool res = false;
    iter = iterator();
    if (IsIterValid(child)) {
	// To update further
    }
    return res;
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
    set_headers_visible(false);
}

NaviNatN::~NaviNatN()
{
}

void NaviNatN::SetDesEnv(MEnv* aDesEnv)
{
    iDesEnv = aDesEnv;
    Glib::RefPtr<NatnTreeMdl> mdl = NatnTreeMdl::create(iDesEnv->Provider());
    set_model(mdl);
    append_column( "one", mdl->ColRec().name);
}

void NaviNatN::on_drag_begin(const Glib::RefPtr<Gdk::DragContext>& context)
{
    TreeView::on_drag_begin(context);
}

void NaviNatN::on_drag_data_get(const Glib::RefPtr<Gdk::DragContext >& context, Gtk::SelectionData& selection_data, guint info, guint time)
{
    TreeView::on_drag_data_get(context, selection_data, info, time);
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
    iNatn->enable_model_drag_source();
    iNatn->drag_source_set (Gtk::ArrayHandle_TargetEntry(targetentries));
}


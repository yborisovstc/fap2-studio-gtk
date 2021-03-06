
#include <iostream>
#include "navi.h"
#include "common.h"
#include <gtkmm/treerowreference.h>
#include <grayb/mprov.h>
#include <dirent.h>
#include <elem.h>
#include "msset.h"

static GtkTargetEntry targetentries[] =
{
    { (gchar*) "STRING",        0, 0 },
    { (gchar*) "text/plain",    0, 1 },
    { (gchar*) "text/uri-list", 0, 2 },
    { (gchar*) "GTK_TREE_MODEL_ROW", 0, 3 },
};

static GtkTargetEntry KModListTargets[] =
{
    { (gchar*) "STRING",        0, 0 },
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

bool NatnTreeMdl::row_draggable_vfunc(const TreeModel::Path& path) const
{
    return true;
}

bool NatnTreeMdl::drag_data_get_vfunc(const TreeModel::Path& path, Gtk::SelectionData& selection_data) const
{
    bool res = false;
    // Set selection. This will evolve DnD process 
    int row_index = path[0];
    string data = iNodesInfo.at(row_index);
    selection_data.set_text(data);
    res = true;
    return res;
}

bool NatnTreeMdl::drag_data_delete_vfunc(const TreeModel::Path& path)
{
    return true;
}


// Native nodes navigation widget
NaviNatN::NaviNatN(MMdlObserver* aDesObs): iDesObs(aDesObs)
{
    set_headers_visible(false);
    SetDesEnv(iDesObs->DesEnv());
    iDesObs->SignalDesEnvChanged().connect(sigc::mem_fun(*this, &NaviNatN::on_des_env_changed));
}

NaviNatN::~NaviNatN()
{
}

void NaviNatN::on_des_env_changed()
{
    SetDesEnv(iDesObs->DesEnv());
}

void NaviNatN::SetDesEnv(MEnv* aDesEnv)
{
    if (aDesEnv != iDesEnv) {
	unset_model();
	remove_all_columns();
	Glib::RefPtr<TreeModel> curmdl = get_model();
	curmdl.reset();
	iDesEnv = aDesEnv;
	if (iDesEnv != NULL) {
	    Glib::RefPtr<NatnTreeMdl> mdl = NatnTreeMdl::create(iDesEnv->Provider());
	    GtkTreeModel* model = mdl->Gtk::TreeModel::gobj();
	    bool isds = GTK_IS_TREE_DRAG_SOURCE(model);
	    set_model(mdl);
	    append_column( "one", mdl->ColRec().name);
	    enable_model_drag_source();
	    drag_source_set (Gtk::ArrayHandle_TargetEntry(targetentries));
	}
    }
}

bool NaviNatN::on_button_press_event(GdkEventButton* event)
{
    iPressX = event->x;
    iPressY = event->y;
    return TreeView::on_button_press_event(event);
}

void NaviNatN::on_drag_begin(const Glib::RefPtr<Gdk::DragContext>& context)
{
    TreeView::on_drag_begin(context);

    // Set source row. On the base GtkTreeView code it is done in gtk_tree_view_maybe_begin_dragging_row()
    // But this is not working for some reason. So fix it by implementing of setting source row here. 
    Gtk::TreeModel::Path path;
    Gtk::TreeViewColumn* col = NULL;
    GtkTreeView *tree_view = Gtk::TreeView::gobj();
    int cell_x, cell_y;

    bool row_exists = get_path_at_pos (iPressX, iPressY, path, col, cell_x, cell_y);
    Glib::RefPtr<Gtk::TreeModel> model = get_model();
    if (row_exists) {
	set_source_row(context, model, path);
    }
}

void NaviNatN::set_source_row(const Glib::RefPtr<Gdk::DragContext>& context, Glib::RefPtr<Gtk::TreeModel>& model, Gtk::TreePath& source_row)
{
    Gtk::TreeRowReference* rr = new Gtk::TreeRowReference(model, source_row);
    context->set_data(Glib::Quark("gtk-tree-view-source-row"), rr->gobj());
    GtkTreeRowReference *ref = (GtkTreeRowReference*) g_object_get_data (G_OBJECT (context->gobj()), "gtk-tree-view-source-row");
    GtkTreePath* res = NULL;
    if (ref) {
	res =  gtk_tree_row_reference_get_path (ref);
    }
 
}

void NaviNatN::on_drag_data_get(const Glib::RefPtr<Gdk::DragContext>& context, Gtk::SelectionData& selection_data, guint info, guint time)
{
    TreeView::on_drag_data_get(context, selection_data, info, time);
}

const ModulesTreeClrec NaviModules::iColRec;

// Modules tree model

const ModulesTreeClrec ModulesTreeMdl::iColRec;

ModulesTreeMdl::ModulesTreeMdl(MEnv* aDesEnv): Glib::ObjectBase(typeid(ModulesTreeMdl)), Glib::Object(), Gtk::TreeModel(), iDesEnv(aDesEnv),
    iStamp(55)
{
}

ModulesTreeMdl::~ModulesTreeMdl()
{
}

Glib::RefPtr<ModulesTreeMdl> ModulesTreeMdl::create(MEnv* aDesEnv)
{
    ModulesTreeMdl* nmdl = new ModulesTreeMdl(aDesEnv);
    Gtk::TreeModel* mdl = reinterpret_cast<Gtk::TreeModel*> (nmdl);
    GtkTreeModel* treemdl = mdl->gobj();
    bool ist = GTK_IS_TREE_MODEL(treemdl);
    return Glib::RefPtr<ModulesTreeMdl>(nmdl);
}

void ModulesTreeMdl::UpdateStamp()
{
    iStamp++;
}

Gtk::TreeModelFlags ModulesTreeMdl::get_flags_vfunc() const
{
    return Gtk::TreeModelFlags(0);
}

int ModulesTreeMdl::get_n_columns_vfunc() const
{
    return iColRec.size();
}

GType ModulesTreeMdl::get_column_type_vfunc(int index) const
{
    return iColRec.types()[index];
}

int ModulesTreeMdl::iter_n_root_children_vfunc() const
{
    vector<string> modules;
    iDesEnv->ImpsMgr()->GetModulesNames(modules);
    return modules.size();
}

bool ModulesTreeMdl::get_iter_vfunc(const Path& path, iterator& iter) const
{
    bool res = false;
    unsigned depth = path.size();
    vector<int> indcv = path.get_indices();
    int ind0 = indcv.at(0);
    // Get modules root first
    iterator iter_root;
    res =  iter_nth_root_child_vfunc(ind0, iter_root);
    ChromoNode node = create_node_from_iter(iter_root);
    for (int dc = 1; dc < depth; dc++) {
	int ind = indcv.at(dc);
	if (ind >= node.Count(ENt_Node)) {
	    return false;
	}
	node = node.At(ind, ENt_Node);
    }
    if (node.Handle() != NULL) {
	set_iter(iter, node, iter_root);
	res = true;
    }
    return res;
}

Gtk::TreeModel::Path ModulesTreeMdl::get_path_vfunc(const iterator& iter) const
{
    __ASSERT(false);
    Path path;
    ChromoNode node = create_node_from_iter(iter);
    Rank rank;
    node.GetRank(rank);
    for (Rank::const_iterator it = rank.begin(); it != rank.end(); it++) {
	path.push_back(*it);
    }
    return path;
}

bool ModulesTreeMdl::IsIterValid(const iterator& iter) const
{
    bool res = (iStamp == iter.get_stamp());
    return res;
}

bool ModulesTreeMdl::iter_is_valid(const iterator& iter) const
{
  // Anything that modifies the model's structure should change the model's stamp, so that old iters are ignored.
  return IsIterValid(iter) && Gtk::TreeModel::iter_is_valid(iter);
}

void ModulesTreeMdl::get_value_vfunc(const TreeModel::iterator& iter, int column, Glib::ValueBase& value) const
{
    if (IsIterValid(iter)) {
	if (column < iColRec.size()) {
	    GType coltype = get_column_type_vfunc(column);
	    if (column == ModulesTreeClrec::KCol_Name) {
		Glib::Value<Glib::ustring> sval;
		sval.init(coltype);
		ChromoNode node = create_node_from_iter(iter);
		string data = node.Name();
		sval.set(data.c_str());
		value.init(coltype);
		value = sval;
	    }
	    else if (column == ModulesTreeClrec::KCol_Uri) {
		Glib::Value<Glib::ustring> sval;
		sval.init(coltype);
		ChromoNode node = create_node_from_iter(iter);
		GUri uri;
		node.GetUri(uri);
		string data = uri.toString(true);
		sval.set(data);
		value.init(coltype);
		value = sval;
	    }
	}
    }
}

Chromo* ModulesTreeMdl::create_modules_chromo(const string& aModName) const
{
    string modpath = iDesEnv->ImpsMgr()->GetModulePath(aModName);
    Chromo* chromo = iDesEnv->Provider()->CreateChromo();
    bool res = chromo->Set(modpath);
    if (!res) {
	delete chromo;
	chromo = NULL;
    }
    return chromo; 
}

bool ModulesTreeMdl::iter_next_vfunc(const iterator& iter, iterator& iter_next) const
{
    bool res = false;
    Chromo* chromo = get_chromo_from_iter(iter);
    iter_next = iterator();
    if (IsIterValid(iter)) {
	if (!is_root_iter(iter)) {
	    ChromoNode node = create_node_from_iter(iter);
	    if (node.Parent() == node.End()) {
		// Module root
		int next_ind = get_next_module_ind(node.Name());
		if (next_ind != -1) {
		    res = iter_nth_root_child_vfunc(next_ind, iter_next);
		}
	    } else {
		// Modules node
		ChromoNode::TIter nit(node);
		nit++;
		if (nit != node.End()) {
		    set_iter(iter_next, *nit, chromo);
		    res = true;
		}
	    }
	}
    }
    if (!res) {
	res = false;
    }
    return res;
}

int ModulesTreeMdl::iter_n_children_vfunc(const iterator& iter) const
{
    int res = 0;
    if (IsIterValid(iter)) {
	ChromoNode node = create_node_from_iter(iter);
	res = node.Count(ENt_Node);
    }
    return res;
}

bool ModulesTreeMdl::iter_children_vfunc(const iterator& parent, iterator& iter) const
{
    return iter_nth_child_vfunc(parent, 0, iter);
}

bool ModulesTreeMdl::iter_has_child_vfunc(const iterator& iter) const
{
    bool res = false;
    if (IsIterValid(iter)) {
	ChromoNode node = create_node_from_iter(iter);
	res = (node.Count(ENt_Node) > 0);
    }
    return res;
}

ChromoNode ModulesTreeMdl::create_node_from_iter(const iterator& iter) const 
{
    void* handle = (void*) iter.gobj()->user_data;
    ChromoMdl& model = ((Chromo*) iter.gobj()->user_data2)->GetModel();
    return  ChromoNode(model, handle);
}

void ModulesTreeMdl::set_iter(iterator& iter, const ChromoNode& node, const Chromo* chromo) const
{
    iter.set_stamp(iStamp);
    iter.gobj()->user_data = (void*) node.Handle();
    iter.gobj()->user_data2 = (void*) chromo;
}

void ModulesTreeMdl::set_iter(iterator& iter, const ChromoNode& node, const iterator& chromo_src) const
{
    iter.set_stamp(iStamp);
    iter.gobj()->user_data = (void*) node.Handle();
    iter.gobj()->user_data2 = chromo_src.gobj()->user_data2;
}

Chromo* ModulesTreeMdl::get_chromo_from_iter(const iterator& iter) const
{
    return ((Chromo*) iter.gobj()->user_data2);
}

int ModulesTreeMdl::get_next_module_ind(const string& aModName) const
{
    int res = -1;
    vector<string> modules;
    iDesEnv->ImpsMgr()->GetModulesNames(modules);
    bool found = false;
    int ind = 0;
    for (; ind < modules.size() && !found; ind++) {
	found = (aModName == modules.at(ind));
    }
    if (found && ind < modules.size()) {
	res = ind;
    }
    return res;
}

bool ModulesTreeMdl::is_root_iter(const iterator& iter) const
{
    return (iter.gobj()->user_data2 == NULL);
}

bool ModulesTreeMdl::iter_nth_child_vfunc(const iterator& parent, int n, iterator& iter) const
{
    bool res = false;
    if (IsIterValid(parent)) {
	ChromoNode node = create_node_from_iter(parent);
	if (n < node.Count(ENt_Node)) {
	    set_iter(iter, node.At(n, ENt_Node), parent);
	    res = true;
	}
    }
    return res;
}

bool ModulesTreeMdl::iter_nth_root_child_vfunc(int n, iterator& iter) const
{
    bool res = false;
    vector<string> modules;
    iDesEnv->ImpsMgr()->GetModulesNames(modules);
    if (n < modules.size()) {
	Chromo* chromo = create_modules_chromo(modules.at(n));
	set_iter(iter, chromo->Root(), chromo);
	res = true;
    }
    return res;
}

bool ModulesTreeMdl::iter_parent_vfunc(const iterator& child, iterator& iter) const
{
    bool res = false;
    bool valid = IsIterValid(child);
    if (valid) {
	if (child.gobj()->user_data2 != NULL) {
	    ChromoNode node = create_node_from_iter(child);
	    if (node.Parent() != node.End()) {
		set_iter(iter, *(node.Parent()), child);
	    } else {
		// Root comp
		iter.set_stamp(iStamp);
		iter.gobj()->user_data = (void*) NULL;
		iter.gobj()->user_data2 = (void*) NULL;
	    }
	    res = true;
	}
    }
    return res;
}

bool ModulesTreeMdl::row_draggable_vfunc(const TreeModel::Path& path) const
{
    return true;
}

bool ModulesTreeMdl::drag_data_get_vfunc(const TreeModel::Path& path, Gtk::SelectionData& selection_data) const
{
    bool res = false;
    // Set selection. This will evolve DnD process 
    iterator iter((TreeModel*)this);
    bool ires = get_iter_vfunc(path, iter);
    string uris = (*iter).get_value(ColRec().uri);
    selection_data.set(KDnDTarg_Import, uris);
    res = true;
    return res;
}

bool ModulesTreeMdl::drag_data_delete_vfunc(const TreeModel::Path& path)
{
    return true;
}

static GtkTargetEntry sNaviModulesDnDTarg[] =
{
    { (gchar*) KDnDTarg_Import, 0, 4 },
};



// Modules navigation widget
NaviModules::NaviModules(MSEnv& aStEnv, MMdlObserver* aDesObs): iStEnv(aStEnv), iDesObs(aDesObs)
{
    set_headers_visible(false);
    SetDesEnv(iDesObs->DesEnv());
    iDesObs->SignalDesEnvChanged().connect(sigc::mem_fun(*this, &NaviModules::on_des_env_changed));
}

NaviModules::~NaviModules()
{
}

void NaviModules::SetDesEnv(MEnv* aDesEnv)
{
    assert(aDesEnv == NULL || aDesEnv != NULL && iDesEnv == NULL);
    if (aDesEnv != iDesEnv) {
	unset_model();
	remove_all_columns();
	iDesEnv = aDesEnv;
    }
    if (iDesEnv != NULL) {
	Glib::RefPtr<ModulesTreeMdl> mdl = ModulesTreeMdl::create(iDesEnv);
	set_model(mdl);
	append_column( "one", mdl->ColRec().name);
	enable_model_drag_source();
	drag_source_set (Gtk::ArrayHandle_TargetEntry(sNaviModulesDnDTarg, 1, Glib::OWNERSHIP_NONE), 
		Gdk::MODIFIER_MASK, Gdk::ACTION_COPY | Gdk::ACTION_MOVE);
    }
}

int NaviModules::FilterModulesDirEntries(const struct dirent *aEntry)
{
    string name = aEntry->d_name;
    size_t ppos = name.find_first_of(".");
    string suff = name.substr(ppos + 1);
    int res = suff.compare("xml"); 
    return (res == 0) ? 1 : 0;
}

bool NaviModules::on_button_press_event(GdkEventButton* event)
{
    iPressX = event->x;
    iPressY = event->y;
    return TreeView::on_button_press_event(event);
}

void NaviModules::on_drag_begin(const Glib::RefPtr<Gdk::DragContext>& context)
{
    TreeView::on_drag_begin(context);

    // Set source row. On the base GtkTreeView code it is done in gtk_tree_view_maybe_begin_dragging_row()
    // But this is not working for some reason. So fix it by implementing of setting source row here. 
    Gtk::TreeModel::Path path;
    Gtk::TreeViewColumn* col = NULL;
    GtkTreeView *tree_view = Gtk::TreeView::gobj();
    int cell_x, cell_y;

    bool row_exists = get_path_at_pos (iPressX, iPressY, path, col, cell_x, cell_y);
    Glib::RefPtr<Gtk::TreeModel> model = get_model();
    if (row_exists) {
	set_source_row(context, model, path);
    }
}

void NaviModules::set_source_row(const Glib::RefPtr<Gdk::DragContext>& context, Glib::RefPtr<Gtk::TreeModel>& model, Gtk::TreePath& source_row)
{
    Gtk::TreeRowReference* rr = new Gtk::TreeRowReference(model, source_row);
    context->set_data(Glib::Quark("gtk-tree-view-source-row"), rr->gobj());
    GtkTreeRowReference *ref = (GtkTreeRowReference*) g_object_get_data (G_OBJECT (context->gobj()), "gtk-tree-view-source-row");
    GtkTreePath* res = NULL;
    if (ref) {
	res =  gtk_tree_row_reference_get_path (ref);
    }

}

void NaviModules::on_drag_data_get(const Glib::RefPtr<Gdk::DragContext>& context, Gtk::SelectionData& selection_data, guint info, guint time)
{
    TreeView::on_drag_data_get(context, selection_data, info, time);
}

void NaviModules::on_des_env_changed()
{
    SetDesEnv(iDesObs->DesEnv());
}



// Current hier tree model

const HierTreeClrec HierTreeMdl::iColRec;

HierTreeMdl::HierTreeMdl(MEnv* aDesEnv): Glib::ObjectBase(typeid(HierTreeMdl)), Glib::Object(), Gtk::TreeModel(), iDesEnv(aDesEnv),
    iStamp(55)
{
    iRoot = iDesEnv->Root();
}

HierTreeMdl::~HierTreeMdl()
{
}

Glib::RefPtr<HierTreeMdl> HierTreeMdl::create(MEnv* aDesEnv)
{
    HierTreeMdl* nmdl = new HierTreeMdl(aDesEnv);
    Gtk::TreeModel* mdl = reinterpret_cast<Gtk::TreeModel*> (nmdl);
    GtkTreeModel* treemdl = mdl->gobj();
    bool ist = GTK_IS_TREE_MODEL(treemdl);
    return Glib::RefPtr<HierTreeMdl>(nmdl);
}

void HierTreeMdl::UpdateStamp()
{
    iStamp++;
}

Gtk::TreeModelFlags HierTreeMdl::get_flags_vfunc() const
{
    return Gtk::TreeModelFlags(0);
}

int HierTreeMdl::get_n_columns_vfunc() const
{
    return iColRec.size();
}

GType HierTreeMdl::get_column_type_vfunc(int index) const
{
    return iColRec.types()[index];
}

int HierTreeMdl::iter_n_root_children_vfunc() const
{
    return iRoot->CompsCount();
}

bool HierTreeMdl::get_iter_vfunc(const Path& path, iterator& iter) const
{
    bool res = false;
    MElem* comp = NULL;
    MElem* mgr = iRoot;
    unsigned depth = path.size();
    //Glib::ArrayHandle<int> indc = path.get_indices(); 
    vector<int> indcv = path.get_indices();
    for (int dc = 0; dc < depth; dc++) {
	int ind = indcv.at(dc);
	if (ind >= mgr->CompsCount()) {
	    return false;
	}
	comp = mgr->GetComp(ind);
	mgr = comp;
    }
    if (comp != NULL) {
	iter.set_stamp(iStamp);
	iter.gobj()->user_data = comp;
	res = true;
    }
    return res;
}

Gtk::TreeModel::Path HierTreeMdl::get_path_vfunc(const iterator& iter) const
{
    Path path;
    MElem* mgr = NULL;
    MElem* comp = (MElem*) iter.gobj()->user_data;
    // By depth
    do {
	mgr = comp->GetMan();
	if (mgr == NULL) {
	    break;
	}
	int pos;
	for (pos = 0; pos < mgr->CompsCount(); pos++) {
	    if (mgr->GetComp(pos) == comp) {
		break;
	    }
	}
	path.prepend_index(pos);
	comp = mgr;
    } while (comp != iRoot);
    return path;
}

bool HierTreeMdl::IsIterValid(const iterator& iter) const
{
    bool res = (iStamp == iter.get_stamp());
    return res;
}

bool HierTreeMdl::iter_is_valid(const iterator& iter) const
{
    // Anything that modifies the model's structure should change the model's stamp, so that old iters are ignored.
    return IsIterValid(iter) && Gtk::TreeModel::iter_is_valid(iter);
}

void HierTreeMdl::get_value_vfunc(const TreeModel::iterator& iter, int column, Glib::ValueBase& value) const
{
    if (IsIterValid(iter)) {
	if (column < iColRec.size()) {
	    GType coltype = get_column_type_vfunc(column);
	    if (column == HierTreeClrec::KCol_Name) {
		Glib::Value<Glib::ustring> sval;
		sval.init(coltype);
		MElem* node = (MElem*) iter.gobj()->user_data;
		//string data = node->EType() + ":" + node->Name();
		string data = node->Name();
		sval.set(data.c_str());
		value.init(coltype);
		value = sval;
	    }
	    else if (column == HierTreeClrec::KCol_Elem) {
		Glib::Value<MElem*> sval;
		sval.init(coltype);
		MElem* data = (MElem*) iter.gobj()->user_data;
		sval.set(data);
		value.init(coltype);
		value = sval;
	    }
	}
    }
}

MElem* HierTreeMdl::get_next_comp(MElem* aComp) 
{
    MElem* res = NULL;
    MElem* mgr = aComp->GetMan();
    if (mgr != NULL) {
	int ct = 0;
	for (; ct < mgr->CompsCount(); ct++) {
	    if (mgr->GetComp(ct) == aComp) {
		break;
	    }
	}
	if (ct < mgr->CompsCount() - 1) {
	    res = mgr->GetComp(++ct);
	}	
    }
    return res;
}

bool HierTreeMdl::iter_next_vfunc(const iterator& iter, iterator& iter_next) const
{
    bool res = false;
    iter_next = iterator();
    if (IsIterValid(iter)) {
	MElem* node = (MElem*) iter.gobj()->user_data;
	MElem* next = ((HierTreeMdl*) this)->get_next_comp(node);
	if (next != NULL) {
	    iter_next.set_stamp(iStamp);
	    iter_next.gobj()->user_data = next;
	    res = true;
	}
    }
    return res;
}

int HierTreeMdl::iter_n_children_vfunc(const iterator& iter) const
{
    int res = 0;
    if (IsIterValid(iter)) {
	MElem* node = (MElem*) iter.gobj()->user_data;
	res = node->CompsCount();
    }
    return res;
}

bool HierTreeMdl::iter_children_vfunc(const iterator& parent, iterator& iter) const
{
    return iter_nth_child_vfunc(parent, 0, iter);
}

bool HierTreeMdl::iter_has_child_vfunc(const iterator& iter) const
{
    bool res = false;
    if (IsIterValid(iter)) {
	MElem* node = (MElem*) iter.gobj()->user_data;
	res = (node->CompsCount() > 0);
    }
    return res;
}

bool HierTreeMdl::iter_nth_child_vfunc(const iterator& parent, int n, iterator& iter) const
{
    bool res = false;
    if (IsIterValid(parent)) {
	MElem* node = (MElem*) parent.gobj()->user_data;
	if (n < node->CompsCount()) {
	    iter.set_stamp(iStamp);
	    iter.gobj()->user_data = node->GetComp(n);
	    res = true;
	}
    }
    return res;
}

bool HierTreeMdl::iter_nth_root_child_vfunc(int n, iterator& iter) const
{
    bool res = false;
    if (n < iRoot->CompsCount()) {
	iter.set_stamp(iStamp);
	iter.gobj()->user_data = iRoot->GetComp(n);
	res = true;
    }
    return res;
}

bool HierTreeMdl::iter_parent_vfunc(const iterator& child, iterator& iter) const
{
    bool res = false;
    bool valid = IsIterValid(child);
    if (valid) {
	MElem* comp = (MElem*) child.gobj()->user_data;
	if (comp->GetMan() != NULL) {
	    iter.set_stamp(iStamp);
	    iter.gobj()->user_data = comp->GetMan();
	    res = true;
	}
    }
    return res;
}

bool HierTreeMdl::row_draggable_vfunc(const TreeModel::Path& path) const
{
    return true;
}

bool HierTreeMdl::drag_data_get_vfunc(const TreeModel::Path& path, Gtk::SelectionData& selection_data) const
{
    bool res = false;
    // Set selection. This will evolve DnD process 
    iterator iter((TreeModel*)this);
    bool ires = get_iter_vfunc(path, iter);
    MElem* node = (MElem*) (*iter).get_value(ColRec().elem);
    GUri uri;
    node->GetUri(uri);
    //selection_data.set_text(uri.GetUri());
    selection_data.set(KDnDTarg_Comp, uri);

    //
    /*
       int row_index = path[0];
       MElem* node = iRoot->Comps().at(row_index);
       string data = node->Name();
       selection_data.set_text(data);
       */
    res = true;
    return res;
}

bool HierTreeMdl::drag_data_delete_vfunc(const TreeModel::Path& path)
{
    return true;
}

void HierTreeMdl::OnCompDeleting(MElem& aComp, TBool aSoft, TBool aModif)
{
    //std::cout << "HierTreeMdl::OnCompDeleting: [" << aComp.Name() << "]" << std::endl;
    //UpdateStamp();
    iterator iter;
    iter.set_stamp(iStamp);
    iter.gobj()->user_data = &aComp;
    Path path = get_path_vfunc(iter);
    row_deleted(path);
}

void HierTreeMdl::OnCompAdding(MElem& aComp, TBool aModif)
{
    //std::cout << "HierTreeMdl::OnCompAdding: [" << aComp.Name() << "]" << std::endl;
    //UpdateStamp();
    // Nodify view of all the internal components. This is required because there is no
    // notif from internal comps - they are created before element gets inserted to the hier
    iterator iter;
    iter.set_stamp(iStamp);
    iter.gobj()->user_data = &aComp;
    // TODO [YB] There are still cases when null aComp is passes, for instance when early
    // construction of native agent happens, ref todo in Vert::Vert()
    // To consider solid solution
    // assert(&aComp != NULL);
    if (&aComp != NULL) {
	Path path = get_path_vfunc(iter);
	if (path.size() > 0) {
	    row_inserted(path, iter);
	    for (TInt ci = 0; ci < aComp.CompsCount(); ci++) {
		MElem* comp = aComp.GetComp(ci);
		OnCompAdding(*comp);
	    }
	} else {
	    //std::cout << "HierTreeMdl::OnCompAdding: Error on getting path" << std::endl;
	}
    }
}

TBool HierTreeMdl::OnCompChanged(MElem& aComp, const string& aContName, TBool aModif)
{
    //std::cout << "HierTreeMdl::OnCompChanged" << std::endl;
    UpdateStamp();
    return true;
}

void HierTreeMdl::on_comp_deleting(MElem* aComp)
{
    OnCompDeleting(*aComp);
}

void HierTreeMdl::on_comp_adding(MElem* aComp)
{
    OnCompAdding(*aComp);
}

void HierTreeMdl::on_comp_changed(MElem* aComp)
{
    OnCompChanged(*aComp);
}

static GtkTargetEntry sNaviHierDnDTarg[] =
{
    { (gchar*) KDnDTarg_Comp, 0, 0 },
};


// Current hier navigation widget
NaviHier::NaviHier(MMdlObserver* aDesObs): iDesEnv(NULL), iDesObs(aDesObs), iRootAdded(false)
{
    set_headers_visible(false);
    SetDesEnv(iDesObs->DesEnv());
    iDesObs->SignalDesEnvChanged().connect(sigc::mem_fun(*this, &NaviHier::on_des_env_changed));
    //iDesObs->SignalSystemChanged().connect(sigc::mem_fun(*this, &NaviHier::on_des_root_added));
    iDesObs->SignalSystemCreated().connect(sigc::mem_fun(*this, &NaviHier::on_des_root_added));
}

NaviHier::~NaviHier()
{
}

void NaviHier::on_des_env_changed()
{
    SetDesEnv(iDesObs->DesEnv());
}

void NaviHier::on_des_root_added()
{
    if (!iRootAdded && iDesEnv != NULL) {
	iRootAdded = true;
	Glib::RefPtr<HierTreeMdl> mdl = HierTreeMdl::create(iDesEnv);
	HierTreeMdl* hmdl = mdl.operator ->();
	GtkTreeModel* model = mdl->Gtk::TreeModel::gobj();
	bool isds = GTK_IS_TREE_DRAG_SOURCE(model);
	set_model(mdl);
	append_column( "one", mdl->ColRec().name);
	enable_model_drag_source();
	drag_source_set (Gtk::ArrayHandle_TargetEntry(sNaviHierDnDTarg, 1, Glib::OWNERSHIP_NONE), 
		Gdk::MODIFIER_MASK, Gdk::ACTION_COPY | Gdk::ACTION_MOVE);
	iDesObs->SignalCompDeleted().connect(sigc::mem_fun(*hmdl, &HierTreeMdl::on_comp_deleting));
	iDesObs->SignalCompAdded().connect(sigc::mem_fun(*hmdl, &HierTreeMdl::on_comp_adding));
	iDesObs->SignalCompChanged().connect(sigc::mem_fun(*hmdl, &HierTreeMdl::on_comp_changed));
    }
}

void NaviHier::SetDesEnv(MEnv* aDesEnv)
{
    assert(aDesEnv == NULL || aDesEnv != NULL && iDesEnv == NULL);
    if (aDesEnv != iDesEnv) {
	unset_model();
	remove_all_columns();
	/* No need to delete model explicitly, it is deleted at unset_model
	   Glib::RefPtr<TreeModel> curmdl = get_model();
	   TreeModel* curmdlp = curmdl.operator ->();
	   curmdl.reset();
	   delete curmdlp;
	   */
	iRootAdded = false;
	iDesEnv = aDesEnv;
	/*
	   if (iDesEnv != NULL) {
	   Glib::RefPtr<HierTreeMdl> mdl = HierTreeMdl::create(iDesEnv);
	   HierTreeMdl* hmdl = mdl.operator ->();
	   GtkTreeModel* model = mdl->Gtk::TreeModel::gobj();
	   bool isds = GTK_IS_TREE_DRAG_SOURCE(model);
	   set_model(mdl);
	   append_column( "one", mdl->ColRec().name);
	   enable_model_drag_source();
	   drag_source_set (Gtk::ArrayHandle_TargetEntry(sNaviHierDnDTarg, 1, Glib::OWNERSHIP_NONE), 
	   Gdk::MODIFIER_MASK, Gdk::ACTION_COPY | Gdk::ACTION_MOVE);
	   iDesObs->SignalCompDeleted().connect(sigc::mem_fun(*hmdl, &HierTreeMdl::on_comp_deleting));
	   iDesObs->SignalCompAdded().connect(sigc::mem_fun(*hmdl, &HierTreeMdl::on_comp_adding));
	   iDesObs->SignalCompChanged().connect(sigc::mem_fun(*hmdl, &HierTreeMdl::on_comp_changed));
	   }
	   */
    }
}

bool NaviHier::on_button_press_event(GdkEventButton* event)
{
    iPressX = event->x;
    iPressY = event->y;
    return TreeView::on_button_press_event(event);
}

void NaviHier::on_drag_begin(const Glib::RefPtr<Gdk::DragContext>& context)
{
    TreeView::on_drag_begin(context);

    // Set source row. On the base GtkTreeView code it is done in gtk_tree_view_maybe_begin_dragging_row()
    // But this is not working for some reason. So fix it by implementing of setting source row here. 
    Gtk::TreeModel::Path path;
    Gtk::TreeViewColumn* col = NULL;
    GtkTreeView *tree_view = Gtk::TreeView::gobj();
    int cell_x, cell_y;

    bool row_exists = get_path_at_pos (iPressX, iPressY, path, col, cell_x, cell_y);
    Glib::RefPtr<Gtk::TreeModel> model = get_model();
    if (row_exists) {
	set_source_row(context, model, path);
    }
}

void NaviHier::set_source_row(const Glib::RefPtr<Gdk::DragContext>& context, Glib::RefPtr<Gtk::TreeModel>& model, Gtk::TreePath& source_row)
{
    Gtk::TreeRowReference* rr = new Gtk::TreeRowReference(model, source_row);
    context->set_data(Glib::Quark("gtk-tree-view-source-row"), rr->gobj());
    GtkTreeRowReference *ref = (GtkTreeRowReference*) g_object_get_data (G_OBJECT (context->gobj()), "gtk-tree-view-source-row");
    GtkTreePath* res = NULL;
    if (ref) {
	res =  gtk_tree_row_reference_get_path (ref);
    }

}

void NaviHier::on_drag_data_get(const Glib::RefPtr<Gdk::DragContext>& context, Gtk::SelectionData& selection_data, guint info, guint time)
{
    TreeView::on_drag_data_get(context, selection_data, info, time);
}

void NaviHier::on_row_activated(const TreeModel::Path& path, TreeViewColumn* column)
{
    TreeModel::iterator iter = get_model()->get_iter(path);
    MElem* node = (MElem*) (*iter).get_value(HierTreeMdl::ColRec().elem);
    iSigCompActivated.emit(node);
}

NaviHier::tSigCompSelected NaviHier::SignalCompSelected()
{
    return iSigCompSelected;
}

NaviHier::tSigCompActivated NaviHier::SignalCompActivated()
{
    return iSigCompActivated;
}



// Navigation widget
Navi::Navi(MSEnv& aStEnv, MMdlObserver* aDesObs): iStEnv(aStEnv), iNatn(NULL), iDesObs(aDesObs)
{
    // Native nodes
    iNatn = new NaviNatN(iDesObs);
    iNatn->show();
    iNatnSw.add(*iNatn);
    iNatnSw.show();
    append_page(iNatnSw, "Native");
    // Modules
    iNatMod = new NaviModules(iStEnv, iDesObs);
    iNatMod->show();
    iNatModSw.add(*iNatMod);
    iNatModSw.show();
    append_page(iNatModSw, "Modules");
    // Current hier
    iNatHier = new NaviHier(iDesObs);
    iNatHier->show();
    iNatHierSw.add(*iNatHier);
    iNatHierSw.show();
    append_page(iNatHierSw, "Model");
    // Chromo
    iChromoTree = new ChromoTree(iDesObs);
    iChromoTree->show();
    iChromoSw.add(*iChromoTree);
    iChromoSw.show();
    append_page(iChromoSw, "Chromo");

}

Navi::~Navi()
{
    delete iNatn;
}

NaviHier& Navi::NatHier()
{
    return *iNatHier;
}

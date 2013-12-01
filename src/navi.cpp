
#include <iostream>
#include "navi.h"
#include <gtkmm/treerowreference.h>
#include <grayb/mprov.h>
#include <dirent.h>
#include <elem.h>

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
    //{ (gchar*) "GTK_TREE_MODEL_ROW", 0, 3 },
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
NaviNatN::NaviNatN()
{
    set_headers_visible(false);
}

NaviNatN::~NaviNatN()
{
}

void NaviNatN::SetDesEnv(MEnv* aDesEnv)
{
    if (aDesEnv != iDesEnv) {
	unset_model();
	remove_all_columns();
	Glib::RefPtr<TreeModel> curmdl = get_model();
	/*
	TreeModel* curmdlp = curmdl.operator ->();
	if (curmdlp != NULL) {
	    delete curmdlp;
	}
	*/
	curmdl.reset();
	iDesEnv = aDesEnv;
	if (iDesEnv != NULL) {
	    Glib::RefPtr<NatnTreeMdl> mdl = NatnTreeMdl::create(iDesEnv->Provider());
	    GtkTreeModel* model = mdl->Gtk::TreeModel::gobj();
	    bool isds = GTK_IS_TREE_DRAG_SOURCE(model);
	    set_model(mdl);
	    append_column( "one", mdl->ColRec().name);
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

// Modules navigation widget
NaviModules::NaviModules()
{
    set_headers_visible(false);
}

NaviModules::~NaviModules()
{
}

void NaviModules::SetDesEnv(MEnv* aDesEnv)
{
    if (aDesEnv != iDesEnv) {
	unset_model();
	remove_all_columns();
	Glib::RefPtr<TreeModel> curmdl = get_model();
	curmdl.reset();
	iDesEnv = aDesEnv;
	if (iDesEnv != NULL) {
	    Glib::RefPtr<Gtk::ListStore> mdl = Gtk::ListStore::create(iColRec);
	    GtkTreeModel* model = mdl->Gtk::TreeModel::gobj();
	    set_model(mdl);
	    append_column( "one", iColRec.name);
	    // Fill out the model
	    // List modules directory
	    struct dirent **entlist;
	    string modpath = iDesEnv->Provider()->ModulesPath();
	    int n = scandir (modpath.c_str(), &entlist, FilterModulesDirEntries, alphasort);
	    // Fill out the model
	    for (int cnt = 0; cnt < n; ++cnt) {
		Gtk::TreeIter it = mdl->append();
		Glib::ustring data = entlist[cnt]->d_name;
		(*it).set_value(iColRec.name, data);
	    }
	}
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
    // Combining full path to module
    GtkTreeRowReference* grr = (GtkTreeRowReference*) context->get_data(Glib::Quark("gtk-tree-view-source-row"));
    Gtk::TreeRowReference rr = Glib::wrap(grr);
    Gtk::TreePath path = rr.get_path();
    Glib::RefPtr<Gtk::TreeModel> mdl = get_model();
    Gtk::TreeIter it = mdl->get_iter(path);
    string modname = (*it).get_value(iColRec.name);
    string modpath = iDesEnv->Provider()->ModulesPath();
    string data = "file:" + modpath + modname + "#";
    selection_data.set_text(data);
}



// Current hier tree model

const HierTreeClrec HierTreeMdl::iColRec;

HierTreeMdl::HierTreeMdl(MEnv* aDesEnv): Glib::ObjectBase(typeid(HierTreeMdl)), Glib::Object(), Gtk::TreeModel(), iDesEnv(aDesEnv),
    iStamp(55)
{
    iRoot = iDesEnv->Root();
    iRoot->SetObserver(this);
}

HierTreeMdl::~HierTreeMdl()
{
    iRoot->SetObserver(NULL);
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
    return iRoot->Comps().size();
}

bool HierTreeMdl::get_iter_vfunc(const Path& path, iterator& iter) const
{
    bool res = false;
    Elem* comp = NULL;
    Elem* mgr = iRoot;
    unsigned depth = path.size();
    //Glib::ArrayHandle<int> indc = path.get_indices(); 
    vector<int> indcv = path.get_indices();
    for (int dc = 0; dc < depth; dc++) {
	int ind = indcv.at(dc);
	if (ind >= mgr->Comps().size()) {
	    return false;
	}
	comp = mgr->Comps().at(ind);
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
    Elem* mgr = NULL;
    Elem* comp = (Elem*) iter.gobj()->user_data;
    // By depth
    do {
	mgr = comp->GetMan();
	if (mgr == NULL) {
	    break;
	}
	int pos;
	for (pos = 0; pos < mgr->Comps().size(); pos++) {
	    if (mgr->Comps().at(pos) == comp) {
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
		Elem* node = (Elem*) iter.gobj()->user_data;
		//string data = node->EType() + ":" + node->Name();
		string data = node->Name();
		sval.set(data.c_str());
		value.init(coltype);
		value = sval;
	    }
	    else if (column == HierTreeClrec::KCol_Elem) {
		Glib::Value<Elem*> sval;
		sval.init(coltype);
		Elem* data = (Elem*) iter.gobj()->user_data;
		sval.set(data);
		value.init(coltype);
		value = sval;
	    }
	}
    }
}

Elem* HierTreeMdl::get_next_comp(Elem* aComp) 
{
    Elem* res = NULL;
    Elem* mgr = aComp->GetMan();
    if (mgr != NULL) {
	int ct = 0;
	for (; ct < mgr->Comps().size(); ct++) {
	    if (mgr->Comps().at(ct) == aComp) {
		break;
	    }
	}
	if (ct < mgr->Comps().size() - 1) {
	    res = mgr->Comps().at(++ct);
	}	
    }
    return res;
}

bool HierTreeMdl::iter_next_vfunc(const iterator& iter, iterator& iter_next) const
{
    bool res = false;
    iter_next = iterator();
    if (IsIterValid(iter)) {
	Elem* node = (Elem*) iter.gobj()->user_data;
	Elem* next = ((HierTreeMdl*) this)->get_next_comp(node);
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
	Elem* node = (Elem*) iter.gobj()->user_data;
	res = node->Comps().size();
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
	Elem* node = (Elem*) iter.gobj()->user_data;
	res = (node->Comps().size() > 0);
    }
    return res;
}

bool HierTreeMdl::iter_nth_child_vfunc(const iterator& parent, int n, iterator& iter) const
{
    bool res = false;
    if (IsIterValid(parent)) {
	Elem* node = (Elem*) parent.gobj()->user_data;
	if (n < node->Comps().size()) {
	    iter.set_stamp(iStamp);
	    iter.gobj()->user_data = node->Comps().at(n);
	    res = true;
	}
    }
    return res;
}

bool HierTreeMdl::iter_nth_root_child_vfunc(int n, iterator& iter) const
{
    bool res = false;
    if (n < iRoot->Comps().size()) {
	iter.set_stamp(iStamp);
	iter.gobj()->user_data = iRoot->Comps().at(n);
	res = true;
    }
    return res;
}

bool HierTreeMdl::iter_parent_vfunc(const iterator& child, iterator& iter) const
{
    bool res = false;
    bool valid = IsIterValid(child);
    if (valid) {
	Elem* comp = (Elem*) child.gobj()->user_data;
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
    Elem* node = (Elem*) (*iter).get_value(ColRec().elem);
    GUri uri;
    node->GetUri(uri);
    selection_data.set_text(uri.GetUri());
    //
    /*
    int row_index = path[0];
    Elem* node = iRoot->Comps().at(row_index);
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

void HierTreeMdl::OnCompDeleting(Elem& aComp)
{
    std::cout << "HierTreeMdl::OnCompDeleting: [" << aComp.Name() << "]" << std::endl;
    //UpdateStamp();
    iterator iter;
    iter.set_stamp(iStamp);
    iter.gobj()->user_data = &aComp;
    Path path = get_path_vfunc(iter);
    row_deleted(path);
}

void HierTreeMdl::OnCompAdding(Elem& aComp)
{
    std::cout << "HierTreeMdl::OnCompAdding: [" << aComp.Name() << "]" << std::endl;
    //UpdateStamp();
    // Nodify view of all the internal components. This is required because there is no
    // notif from internal comps - they are created before element gets inserted to the hier
    iterator iter;
    iter.set_stamp(iStamp);
    iter.gobj()->user_data = &aComp;
    Path path = get_path_vfunc(iter);
    row_inserted(path, iter);
    for (vector<Elem*>::iterator it = aComp.Comps().begin(); it != aComp.Comps().end(); it++) {
	OnCompAdding(*(*it));
    }
}

void HierTreeMdl::OnCompChanged(Elem& aComp)
{
    std::cout << "HierTreeMdl::OnCompChanged" << std::endl;
    UpdateStamp();
}



// Current hier navigation widget
NaviHier::NaviHier(): iDesEnv(NULL)
{
    set_headers_visible(false);
}

NaviHier::~NaviHier()
{
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
	iDesEnv = aDesEnv;
	if (iDesEnv != NULL) {
	    Glib::RefPtr<HierTreeMdl> mdl = HierTreeMdl::create(iDesEnv);
	    GtkTreeModel* model = mdl->Gtk::TreeModel::gobj();
	    bool isds = GTK_IS_TREE_DRAG_SOURCE(model);
	    set_model(mdl);
	    append_column( "one", mdl->ColRec().name);
	}
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
    Elem* node = (Elem*) (*iter).get_value(HierTreeMdl::ColRec().elem);
    iSigCompSelected.emit(node);
}

NaviHier::tSigCompSelected NaviHier::SignalCompSelected()
{
    return iSigCompSelected;
}



// Navigation widget
Navi::Navi(): iNatn(NULL), iDesEnv(NULL)
{
    // Native nodes
    iNatn = new NaviNatN();
    iNatn->show();
    iNatnSw.add(*iNatn);
    iNatnSw.show();
    append_page(iNatnSw, "Native");
    // Modules
    iNatMod = new NaviModules();
    iNatMod->show();
    append_page(*iNatMod, "Modules");
    // Current hier
    iNatHier = new NaviHier();
    iNatHier->show();
    iNatHierSw.add(*iNatHier);
    iNatHierSw.show();
    append_page(iNatHierSw, "Model");
}

Navi::~Navi()
{
    delete iNatn;
}

void Navi::SetDesEnv(MEnv* aDesEnv)
{
    assert(aDesEnv == NULL || aDesEnv != NULL && iDesEnv == NULL);
    iDesEnv = aDesEnv;
    iNatn->SetDesEnv(iDesEnv);
    iNatn->enable_model_drag_source();
    iNatn->drag_source_set (Gtk::ArrayHandle_TargetEntry(targetentries));
    iNatMod->SetDesEnv(iDesEnv);
    iNatMod->enable_model_drag_source();
    iNatMod->drag_source_set (Gtk::ArrayHandle_TargetEntry(KModListTargets, 1, Glib::OWNERSHIP_NONE), Gdk::MODIFIER_MASK, Gdk::ACTION_COPY);
    iNatHier->SetDesEnv(iDesEnv);
    iNatHier->enable_model_drag_source();
    iNatHier->drag_source_set (Gtk::ArrayHandle_TargetEntry(targetentries));
}

NaviHier& Navi::NatHier()
{
    return *iNatHier;
}

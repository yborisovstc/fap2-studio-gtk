
#include "common.h"
#include "chromoview.h"
#include <gtkmm/treerowreference.h>


// Current hier tree model

const ChromoTreeClrec ChromoTreeMdl::iColRec;

ChromoTreeMdl::ChromoTreeMdl(Elem* aRoot, MEnv* aDesEnv): Glib::ObjectBase(typeid(ChromoTreeMdl)), Glib::Object(), Gtk::TreeModel(), 
    iDesEnv(aDesEnv), iStamp(55), iRoot(aRoot), iRnode(iRoot->Chromos().Root())
{
}

ChromoTreeMdl::~ChromoTreeMdl()
{
}

Glib::RefPtr<ChromoTreeMdl> ChromoTreeMdl::create(Elem* aRoot, MEnv* aDesEnv)
{
    ChromoTreeMdl* nmdl = new ChromoTreeMdl(aRoot, aDesEnv);
    Gtk::TreeModel* mdl = reinterpret_cast<Gtk::TreeModel*> (nmdl);
    GtkTreeModel* treemdl = mdl->gobj();
    bool ist = GTK_IS_TREE_MODEL(treemdl);
    return Glib::RefPtr<ChromoTreeMdl>(nmdl);
}

void ChromoTreeMdl::UpdateStamp()
{
    iStamp++;
}

Gtk::TreeModelFlags ChromoTreeMdl::get_flags_vfunc() const
{
    return Gtk::TreeModelFlags(0);
}

int ChromoTreeMdl::get_n_columns_vfunc() const
{
    return iColRec.size();
}

GType ChromoTreeMdl::get_column_type_vfunc(int index) const
{
    return iColRec.types()[index];
}

int ChromoTreeMdl::iter_n_root_children_vfunc() const
{
    return iRnode.Count();
}

bool ChromoTreeMdl::get_iter_vfunc(const Path& path, iterator& iter) const
{
    bool res = false;
    ChromoNode node = iRnode;
    ChromoNode comp = iRnode;
    unsigned depth = path.size();
    vector<int> indcv = path.get_indices();
    for (int dc = 0; dc < depth; dc++) {
	int ind = indcv.at(dc);
	if (ind >= node.Count()) {
	    return false;
	}
	comp = node.At(ind);
	node = comp;
    }
    if (comp != *(iRnode.End())) {
	iter.set_stamp(iStamp);
	iter.gobj()->user_data = comp.Handle();
	res = true;
    }
    return res;
}

Gtk::TreeModel::Path ChromoTreeMdl::get_path_vfunc(const iterator& iter) const
{
    Path path;
    ChromoNode mgr = *iRnode.End();
    ChromoNode comp(mgr.Mdl(), iter.gobj()->user_data);
    // By depth
    do {
	mgr = *comp.Parent();
	if (mgr == *iRnode.End()) {
	    break;
	}
	int pos;
	for (pos = 0; pos < mgr.Count(); pos++) {
	    if (mgr.At(pos) == comp) {
		break;
	    }
	}
	path.prepend_index(pos);
	comp = mgr;
    } while (comp != iRnode);
    return path;
}

bool ChromoTreeMdl::IsIterValid(const iterator& iter) const
{
    bool res = (iStamp == iter.get_stamp());
    return res;
}

bool ChromoTreeMdl::iter_is_valid(const iterator& iter) const
{
  // Anything that modifies the model's structure should change the model's stamp, so that old iters are ignored.
  return IsIterValid(iter) && Gtk::TreeModel::iter_is_valid(iter);
}

void ChromoTreeMdl::get_value_vfunc(const TreeModel::iterator& iter, int column, Glib::ValueBase& value) const
{
    if (IsIterValid(iter)) {
	if (column < iColRec.size()) {
	    GType coltype = get_column_type_vfunc(column);
	    if (column == ChromoTreeClrec::KCol_Name) {
		Glib::Value<Glib::ustring> sval;
		sval.init(coltype);
		ChromoNode node(iRnode.Mdl(), iter.gobj()->user_data);
		//string data = node->EType() + ":" + node->Name();
		string data = GUriBase::NodeTypeName(node.Type());
		sval.set(data.c_str());
		value.init(coltype);
		value = sval;
	    }
	    else if (column == ChromoTreeClrec::KCol_Attr) {
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

ChromoNode ChromoTreeMdl::get_next_comp(const ChromoNode& aComp)
{
    ChromoNode::Iterator curr(aComp);
    curr++;
    ChromoNode res = *curr;
    return res;
}
    /*
    ChromoNode res = *iRnode.End();
    ChromoNode mgr = *aComp.Parent();
    if (mgr != *iRnode.End()) {
	int ct = 0;
	for (; ct < mgr.Count(); ct++) {
	    if (mgr.At(ct) == aComp) {
		break;
	    }
	}
	if (ct < mgr.Count() - 1) {
	    res = mgr.At(++ct);
	}	
    }
    return res;
    */

bool ChromoTreeMdl::iter_next_vfunc(const iterator& iter, iterator& iter_next) const
{
    bool res = false;
    iter_next = iterator();
    if (IsIterValid(iter)) {
	ChromoNode node(iRnode.Mdl(), iter.gobj()->user_data);
	ChromoNode next = ((ChromoTreeMdl*) this)->get_next_comp(node);
	if (next != *iRnode.End()) {
	    iter_next.set_stamp(iStamp);
	    iter_next.gobj()->user_data = next.Handle();
	    res = true;
	}
    }
    return res;
}

int ChromoTreeMdl::iter_n_children_vfunc(const iterator& iter) const
{
    int res = 0;
    if (IsIterValid(iter)) {
	ChromoNode node(iRnode.Mdl(), iter.gobj()->user_data);
	res = node.Count();
    }
    return res;
}

bool ChromoTreeMdl::iter_children_vfunc(const iterator& parent, iterator& iter) const
{
  return iter_nth_child_vfunc(parent, 0, iter);
}

bool ChromoTreeMdl::iter_has_child_vfunc(const iterator& iter) const
{
    bool res = false;
    if (IsIterValid(iter)) {
	ChromoNode node(iRnode.Mdl(), iter.gobj()->user_data);
	res = (node.Count() > 0);
    }
    return res;
}

bool ChromoTreeMdl::iter_nth_child_vfunc(const iterator& parent, int n, iterator& iter) const
{
    bool res = false;
    if (IsIterValid(parent)) {
	ChromoNode node(iRnode.Mdl(), parent.gobj()->user_data);
	if (n < node.Count()) {
	    iter.set_stamp(iStamp);
	    iter.gobj()->user_data = node.At(n).Handle();
	    res = true;
	}
    }
    return res;
}

bool ChromoTreeMdl::iter_nth_root_child_vfunc(int n, iterator& iter) const
{
    bool res = false;
    if (n < iRnode.Count()) {
	iter.set_stamp(iStamp);
	iter.gobj()->user_data = iRnode.At(n).Handle();
	res = true;
    }
    return res;
}

bool ChromoTreeMdl::iter_parent_vfunc(const iterator& child, iterator& iter) const
{
    bool res = false;
    bool valid = IsIterValid(child);
    if (valid) {
	ChromoNode comp(iRnode.Mdl(), child.gobj()->user_data);
	ChromoNode mgr = *comp.Parent();
	if (mgr != *iRnode.End()) {
	    iter.set_stamp(iStamp);
	    iter.gobj()->user_data = mgr.Handle();
	    res = true;
	}
    }
    return res;
}

bool ChromoTreeMdl::row_draggable_vfunc(const TreeModel::Path& path) const
{
    return true;
}

bool ChromoTreeMdl::drag_data_get_vfunc(const TreeModel::Path& path, Gtk::SelectionData& selection_data) const
{
    bool res = false;
    // Set selection. This will evolve DnD process 
    iterator iter((TreeModel*)this);
    bool ires = get_iter_vfunc(path, iter);
    /*
    Elem* node = (Elem*) (*iter).get_value(ColRec().elem);
    GUri uri;
    node->GetUri(uri);
    //selection_data.set_text(uri.GetUri());
    selection_data.set(KDnDTarg_Comp, uri.GetUri());
    */
    res = true;
    return res;
}

bool ChromoTreeMdl::drag_data_delete_vfunc(const TreeModel::Path& path)
{
    return true;
}

void ChromoTreeMdl::OnCompDeleting(Elem& aComp)
{
    //std::cout << "ChromoTreeMdl::OnCompDeleting: [" << aComp.Name() << "]" << std::endl;
    /*
    iterator iter;
    iter.set_stamp(iStamp);
    iter.gobj()->user_data = &aComp;
    Path path = get_path_vfunc(iter);
    row_deleted(path);
    */
}

void ChromoTreeMdl::OnCompAdding(Elem& aComp)
{
    //std::cout << "ChromoTreeMdl::OnCompAdding: [" << aComp.Name() << "]" << std::endl;
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

void ChromoTreeMdl::OnCompChanged(Elem& aComp)
{
    //std::cout << "ChromoTreeMdl::OnCompChanged" << std::endl;
    UpdateStamp();
}

void ChromoTreeMdl::on_comp_deleting(Elem* aComp)
{
    OnCompDeleting(*aComp);
}

void ChromoTreeMdl::on_comp_adding(Elem* aComp)
{
    OnCompAdding(*aComp);
}

void ChromoTreeMdl::on_comp_changed(Elem* aComp)
{
    OnCompChanged(*aComp);
}




static GtkTargetEntry sChromoTreeDnDTarg[] =
{
    { (gchar*) KDnDTarg_Comp, 0, 0 },
};

// Current chromo navigation widget
ChromoTree::ChromoTree(MMdlObserver* aDesObs): iDesEnv(NULL), iDesObs(aDesObs), iRootAdded(false)
{
    set_headers_visible(false);
    SetDesEnv(iDesObs->DesEnv());
    iDesObs->SignalDesEnvChanged().connect(sigc::mem_fun(*this, &ChromoTree::on_des_env_changed));
    iDesObs->SignalSystemCreated().connect(sigc::mem_fun(*this, &ChromoTree::on_des_root_added));
}

ChromoTree::~ChromoTree()
{
}

void ChromoTree::on_des_env_changed()
{
    SetDesEnv(iDesObs->DesEnv());
}

void ChromoTree::on_des_root_added()
{
    if (!iRootAdded && iDesEnv != NULL) {
	iRootAdded = true;
	Glib::RefPtr<ChromoTreeMdl> mdl = ChromoTreeMdl::create(iDesEnv->Root(), iDesEnv);
	ChromoTreeMdl* hmdl = mdl.operator ->();
	GtkTreeModel* model = mdl->Gtk::TreeModel::gobj();
	bool isds = GTK_IS_TREE_DRAG_SOURCE(model);
	set_model(mdl);
	append_column( "one", mdl->ColRec().name);
	enable_model_drag_source();
	drag_source_set (Gtk::ArrayHandle_TargetEntry(sChromoTreeDnDTarg, 1, Glib::OWNERSHIP_NONE), 
		Gdk::MODIFIER_MASK, Gdk::ACTION_COPY | Gdk::ACTION_MOVE);
	iDesObs->SignalCompDeleted().connect(sigc::mem_fun(*hmdl, &ChromoTreeMdl::on_comp_deleting));
	iDesObs->SignalCompAdded().connect(sigc::mem_fun(*hmdl, &ChromoTreeMdl::on_comp_adding));
	iDesObs->SignalCompChanged().connect(sigc::mem_fun(*hmdl, &ChromoTreeMdl::on_comp_changed));
    }
}

void ChromoTree::SetDesEnv(MEnv* aDesEnv)
{
    assert(aDesEnv == NULL || aDesEnv != NULL && iDesEnv == NULL);
    if (aDesEnv != iDesEnv) {
	unset_model();
	remove_all_columns();
	iRootAdded = false;
	iDesEnv = aDesEnv;
    }
}

bool ChromoTree::on_button_press_event(GdkEventButton* event)
{
    iPressX = event->x;
    iPressY = event->y;
    return TreeView::on_button_press_event(event);
}

void ChromoTree::on_drag_begin(const Glib::RefPtr<Gdk::DragContext>& context)
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

void ChromoTree::set_source_row(const Glib::RefPtr<Gdk::DragContext>& context, Glib::RefPtr<Gtk::TreeModel>& model, Gtk::TreePath& source_row)
{
    Gtk::TreeRowReference* rr = new Gtk::TreeRowReference(model, source_row);
    context->set_data(Glib::Quark("gtk-tree-view-source-row"), rr->gobj());
    GtkTreeRowReference *ref = (GtkTreeRowReference*) g_object_get_data (G_OBJECT (context->gobj()), "gtk-tree-view-source-row");
    GtkTreePath* res = NULL;
    if (ref) {
	res =  gtk_tree_row_reference_get_path (ref);
    }
 
}

void ChromoTree::on_drag_data_get(const Glib::RefPtr<Gdk::DragContext>& context, Gtk::SelectionData& selection_data, guint info, guint time)
{
    TreeView::on_drag_data_get(context, selection_data, info, time);
}

void ChromoTree::on_row_activated(const TreeModel::Path& path, TreeViewColumn* column)
{
    TreeModel::iterator iter = get_model()->get_iter(path);
}

ChromoTree::tSigCompSelected ChromoTree::SignalCompSelected()
{
    return iSigCompSelected;
}




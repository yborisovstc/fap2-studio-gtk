
#include "common.h"
#include "chromoview.h"
#include <gtkmm/treerowreference.h>
#include <glibmm/main.h>
#include <iostream>


// Current hier tree model

const ChromoTreeClrec ChromoTreeMdl::iColRec;

ChromoTreeMdl::ChromoTreeMdl(MElem* aRoot, MEnv* aDesEnv, MMdlObserver* aDesObs): Glib::ObjectBase(typeid(ChromoTreeMdl)), Glib::Object(), Gtk::TreeModel(), 
    iDesEnv(aDesEnv), iStamp(55), iRoot(aRoot), iRnode(iRoot->Chromos().Root()), iDesObs(aDesObs)
{
	iDesObs->SignalSystemChanged().connect(sigc::mem_fun(*this, &ChromoTreeMdl::on_system_changed));
}

ChromoTreeMdl::~ChromoTreeMdl()
{
}

Glib::RefPtr<ChromoTreeMdl> ChromoTreeMdl::create(MElem* aRoot, MEnv* aDesEnv, MMdlObserver* aDesObs)
{
    ChromoTreeMdl* nmdl = new ChromoTreeMdl(aRoot, aDesEnv, aDesObs);
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
	    ChromoNode node(iRnode.Mdl(), iter.gobj()->user_data);
	    GType coltype = get_column_type_vfunc(column);
	    if (column == ChromoTreeClrec::KCol_Name) {
		Glib::Value<Glib::ustring> sval;
		sval.init(coltype);
		string data; 
		if (node.Type() == ENt_Node) {
		    data = "node";
		    data += " id:" + node.Attr(ENa_Id);
		    data += " parent:" + node.Attr(ENa_Parent);
		    if (node.AttrExists(ENa_MutNode)) {
			data += " to:" + node.Attr(ENa_MutNode);
		    }
		}
		else if (node.Type() == ENt_Change) {
		    data = "rename";
		    data += " node:" + node.Attr(ENa_MutNode);
		    data += " val:" + node.Attr(ENa_MutVal);
		}
		else if (node.Type() == ENt_Move) { 
		    data = "move";
		    data += " node:" + node.Attr(ENa_Id);
		    if (node.AttrExists(ENa_MutNode)) {
			data += " to:" + node.Attr(ENa_MutNode);
		    }
		}
		else if (node.Type() == ENt_Cont) { 
		    data = "cont";
		    if (node.AttrExists(ENa_MutNode)) {
			data += " node:" + node.Attr(ENa_MutNode);
		    }
		    if (node.AttrExists(ENa_Id)) {
			data += " id:" + node.Attr(ENa_Id);
		    }
		    if (node.AttrExists(ENa_MutVal)) {
			data += " val:" + node.Attr(ENa_MutVal);
		    }
		    else if (node.AttrExists(ENa_Ref)) {
			data += " ref:" + node.Attr(ENa_Ref);
		    }
		}
		else if (node.Type() == ENt_Rm) {
		    data = "remove";
		    data += " node:" + node.Attr(ENa_MutNode);
		}
        else if (node.Type() == ENt_Import) {
		    data = "import";
		    data += " id:" + node.Attr(ENa_Id);
		}
		sval.set(data.c_str());
		value.init(coltype);
		value = sval;
	    }
	    else if (column == ChromoTreeClrec::KCol_Mutid) {
		Glib::Value<int> sval;
		sval.init(coltype);
		sval.set(node.LineId());
		value.init(coltype);
		value = sval;
	    }
	}
    }
}

void ChromoTreeMdl::GetNodeArg(const ChromoNode& aNode, int aArgInd, string& aArg) const
{
   TNodeType ntype = aNode.Type(); 
   string obj = aNode.Attr(ENa_MutNode);
   if (ntype == ENt_Node) {
       //if (obj.empty()) aArgInd++; /* Making 0 arg floating */
       if (aArgInd == 0) aArg = obj;
       else if (aArgInd == 1) aArg = string("id: ") + aNode.Attr(ENa_Id);
       else if (aArgInd == 2) aArg = string("parent: ") + aNode.Attr(ENa_Parent);
   }
   else if (ntype == ENt_Change) {
       if (aArgInd == 0) aArg = obj;
       else if (aArgInd == 1) aArg = string("val: ") + aNode.Attr(ENa_MutVal);
   }
   else if (ntype == ENt_Rm) {
       if (aArgInd == 0) aArg = obj;
   }
   else if (ntype == ENt_Cont) {
       if (aArgInd == 0) aArg = obj;
       else if (aArgInd == 1) {
	   if (!aNode.Attr(ENa_MutVal).empty()) aArg = string("val: ") + aNode.Attr(ENa_MutVal);
	   else if (!aNode.Attr(ENa_Ref).empty()) aArg = string("ref: ") + aNode.Attr(ENa_Ref);
       }
   }
   else if (ntype == ENt_Move) {
       if (aArgInd == 0) aArg = obj;
       else if (aArgInd == 1) aArg = string("node: ") + aNode.Attr(ENa_Id);
   }
}

ChromoNode ChromoTreeMdl::get_next_comp(const ChromoNode& aComp)
{
    ChromoNode::Iterator curr(aComp);
    curr++;
    ChromoNode res = *curr;
    return res;
}

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
    MElem* node = (MElem*) (*iter).get_value(ColRec().elem);
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

void ChromoTreeMdl::on_system_changed()
{
    cout << "ChromoTreeMdl::on_system_changed" << endl;
    // Chromo was appended, notify the view of row added
    //Path path;
    iterator iter;
    //get_iter_vfunc(path, iter);
    //while (iter_next_vfunc(iter, iter));

    ChromoNode mut = *(iRnode.Rbegin());
    iter.set_stamp(iStamp);
    iter.gobj()->user_data = mut.Handle();

    row_inserted(get_path_vfunc(iter), iter);
}




static GtkTargetEntry sChromoTreeDnDTarg[] =
{
    { (gchar*) KDnDTarg_Comp, 0, 0 },
};

// Current chromo navigation widget
ChromoTree::ChromoTree(MMdlObserver* aDesObs): iDesEnv(NULL), iDesObs(aDesObs), iRootAdded(false)
{
    set_headers_visible(false);
    set_enable_search(true);
    Glib::RefPtr<TreeSelection> sel = get_selection();
    sel->set_mode(SELECTION_MULTIPLE);
    //set_hover_selection(true);
    SetDesEnv(iDesObs->DesEnv());
    iDesObs->SignalDesEnvChanged().connect(sigc::mem_fun(*this, &ChromoTree::on_des_env_changed));
    iDesObs->SignalSystemCreated().connect(sigc::mem_fun(*this, &ChromoTree::on_des_root_added));
}

ChromoTree::~ChromoTree()
{
}

// Generic handler of comp change notification
void ChromoTree::on_comp_changed(MElem* aComp)
{
    Glib::signal_idle().connect_once(sigc::mem_fun(*this, &ChromoTree::on_refresh_model));
}

void ChromoTree::on_comp_renamed(MElem*, const std::string&)
{
    // Use idle one shot because of the mut caused the change is not in chromo yet and
    // will be added shortly
    Glib::signal_idle().connect_once(sigc::mem_fun(*this, &ChromoTree::on_refresh_model));
}

void ChromoTree::RefreshModel()
{
    // Simply reload model
    unset_model();
    remove_all_columns();
    Glib::RefPtr<ChromoTreeMdl> mdl = ChromoTreeMdl::create(iDesEnv->Root(), iDesEnv, iDesObs);
    set_model(mdl);
    append_column( "name", mdl->ColRec().name);
}

void ChromoTree::on_refresh_model()
{
    //RefreshModel();
}

void ChromoTree::on_des_env_changed()
{
    SetDesEnv(iDesObs->DesEnv());
}

void ChromoTree::on_des_root_added()
{
    if (!iRootAdded && iDesEnv != NULL) {
	iRootAdded = true;
	Glib::RefPtr<ChromoTreeMdl> mdl = ChromoTreeMdl::create(iDesEnv->Root(), iDesEnv, iDesObs);
	ChromoTreeMdl* hmdl = mdl.operator ->();
	GtkTreeModel* model = mdl->Gtk::TreeModel::gobj();
	set_model(mdl);
	append_column( "name", mdl->ColRec().name);
	enable_model_drag_source();
	drag_source_set (Gtk::ArrayHandle_TargetEntry(sChromoTreeDnDTarg, 1, Glib::OWNERSHIP_NONE), 
		Gdk::MODIFIER_MASK, Gdk::ACTION_COPY | Gdk::ACTION_MOVE);
	//iDesObs->SignalSystemChanged().connect(sigc::mem_fun(*hmdl, &ChromoTreeMdl::on_system_changed));
	iDesObs->SignalCompAdded().connect(sigc::mem_fun(*this, &ChromoTree::on_comp_changed));
	iDesObs->SignalCompDeleted().connect(sigc::mem_fun(*this, &ChromoTree::on_comp_changed));
	iDesObs->SignalCompChanged().connect(sigc::mem_fun(*this, &ChromoTree::on_comp_changed));
	iDesObs->SignalCompRenamed().connect(sigc::mem_fun(*this, &ChromoTree::on_comp_renamed));
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
    Glib::RefPtr<TreeSelection> sel = get_selection();
    //sel->select_all();

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

void ChromoTree::on_logrec_activated(const string& aNodeUri, int aMutId)
{
    Select(aMutId);
}

void ChromoTree::Select(int aMutId)
{
    get_selection()->unselect_all();
    TreeIter it = get_model()->get_iter(TreePath("0"));
    mSearchedMutId = aMutId;
    mFoundMutIdIter = get_model()->children().end();
    // We need to go thru the whole chromo hier, so use foreach
    get_model()->foreach_iter(sigc::mem_fun(*this, &ChromoTree::on_check_mutid));
    if (mFoundMutIdIter.operator bool()) {
	int mutid;
	mFoundMutIdIter->get_value(ChromoTreeClrec::KCol_Mutid, mutid);
	expand_to_path(TreePath(mFoundMutIdIter));
	scroll_to_row(TreePath(mFoundMutIdIter));
	get_selection()->select(mFoundMutIdIter);
    }
}

bool ChromoTree::on_check_mutid(const TreeModel::iterator& it) 
{
    bool res = false;
    int mutid;
    it->get_value(ChromoTreeClrec::KCol_Mutid, mutid);
    if (mutid == mSearchedMutId) {
	res = true;
	mFoundMutIdIter = it;
    }
    return res;
}



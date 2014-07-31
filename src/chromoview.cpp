
#include "chromoview.h"

#if 0

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
    return iRoot->Comps().size();
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

Elem* ChromoTreeMdl::get_next_comp(Elem* aComp) 
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

bool ChromoTreeMdl::iter_next_vfunc(const iterator& iter, iterator& iter_next) const
{
    bool res = false;
    iter_next = iterator();
    if (IsIterValid(iter)) {
	Elem* node = (Elem*) iter.gobj()->user_data;
	Elem* next = ((ChromoTreeMdl*) this)->get_next_comp(node);
	if (next != NULL) {
	    iter_next.set_stamp(iStamp);
	    iter_next.gobj()->user_data = next;
	    res = true;
	}
    }
    return res;
}

int ChromoTreeMdl::iter_n_children_vfunc(const iterator& iter) const
{
    int res = 0;
    if (IsIterValid(iter)) {
	Elem* node = (Elem*) iter.gobj()->user_data;
	res = node->Comps().size();
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
	Elem* node = (Elem*) iter.gobj()->user_data;
	res = (node->Comps().size() > 0);
    }
    return res;
}

bool ChromoTreeMdl::iter_nth_child_vfunc(const iterator& parent, int n, iterator& iter) const
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

bool ChromoTreeMdl::iter_nth_root_child_vfunc(int n, iterator& iter) const
{
    bool res = false;
    if (n < iRoot->Comps().size()) {
	iter.set_stamp(iStamp);
	iter.gobj()->user_data = iRoot->Comps().at(n);
	res = true;
    }
    return res;
}

bool ChromoTreeMdl::iter_parent_vfunc(const iterator& child, iterator& iter) const
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
    Elem* node = (Elem*) (*iter).get_value(ColRec().elem);
    GUri uri;
    node->GetUri(uri);
    //selection_data.set_text(uri.GetUri());
    selection_data.set(KDnDTarg_Comp, uri.GetUri());

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

bool ChromoTreeMdl::drag_data_delete_vfunc(const TreeModel::Path& path)
{
    return true;
}

void ChromoTreeMdl::OnCompDeleting(Elem& aComp)
{
    //std::cout << "ChromoTreeMdl::OnCompDeleting: [" << aComp.Name() << "]" << std::endl;
    //UpdateStamp();
    iterator iter;
    iter.set_stamp(iStamp);
    iter.gobj()->user_data = &aComp;
    Path path = get_path_vfunc(iter);
    row_deleted(path);
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

static GtkTargetEntry sNaviChromoDnDTarg[] =
{
    { (gchar*) KDnDTarg_Comp, 0, 0 },
};

#endif


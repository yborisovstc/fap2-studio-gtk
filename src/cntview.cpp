#include <gtkmm.h>
#include <gtkmm/treerowreference.h>
#include "cntview.h"

// Current hier tree model

const ContentTreeClrec ContentTreeMdl::iColRec;

ContentTreeMdl::ContentTreeMdl(const MElem& aAgent): Glib::ObjectBase(typeid(ContentTreeMdl)), Glib::Object(), Gtk::TreeModel(), mAgent(aAgent),
    iStamp(55)
{
}

ContentTreeMdl::~ContentTreeMdl()
{
    // Remove glue items
    for(tGiList::iterator iter = iGiList.begin(); iter != iGiList.end(); ++iter)
    {
	GlueItem* pItem = *iter;
	delete pItem;
    }
}

Glib::RefPtr<ContentTreeMdl> ContentTreeMdl::create(const MElem& aAgent)
{
    ContentTreeMdl* nmdl = new ContentTreeMdl(aAgent);
    Gtk::TreeModel* mdl = reinterpret_cast<Gtk::TreeModel*> (nmdl);
    GtkTreeModel* treemdl = mdl->gobj();
    bool ist = GTK_IS_TREE_MODEL(treemdl);
    return Glib::RefPtr<ContentTreeMdl>(nmdl);
}

ContentTreeMdl::GlueItem* ContentTreeMdl::AddGlueItem(const string& aContName) const
{
    GlueItem* item = new GlueItem(aContName);
    iGiList.push_back(item);
    return item;
}

void ContentTreeMdl::UpdateStamp()
{
    iStamp++;
}

int ContentTreeMdl::GetCompInd(const string& aComp) const
{
    int ind = 0;
    if (aComp.empty()) {
	ind = -1;
    } else {
	string owner = mAgent.GetContentOwner(aComp);
	for (; ind < mAgent.GetContCount(owner); ind++) {
	    string comp, cval;
	    mAgent.GetCont(ind, comp, cval, owner);
	    if (comp == aComp) {
		break;
	    }
	}
    }
    return ind;
}

Gtk::TreeModelFlags ContentTreeMdl::get_flags_vfunc() const
{
    return Gtk::TreeModelFlags(0);
}

int ContentTreeMdl::get_n_columns_vfunc() const
{
    return iColRec.size();
}

GType ContentTreeMdl::get_column_type_vfunc(int index) const
{
    return iColRec.types()[index];
}

int ContentTreeMdl::iter_n_root_children_vfunc() const
{
    return mAgent.GetContCount();
}

bool ContentTreeMdl::get_iter_vfunc(const Path& path, iterator& iter) const
{
    bool res = false;
    string comp, cval;
    string owner;
    unsigned depth = path.size();
    vector<int> indcv = path.get_indices();
    for (int dc = 0; dc < depth; dc++) {
	int ind = indcv.at(dc);
	if (ind >= mAgent.GetContCount(owner)) {
	    return false;
	}
	mAgent.GetCont(ind, comp, cval, owner);
	owner = comp;
    }
    iter.set_stamp(iStamp);
    iter.gobj()->user_data = AddGlueItem(owner);
    res = true;
    return res;
}

Gtk::TreeModel::Path ContentTreeMdl::get_path_vfunc(const iterator& iter) const
{
    Path path;
    GlueItem* gi = (GlueItem*) iter.gobj()->user_data;
    string comp = gi->mContName;
    string owner = MElem::GetContentOwner(comp);
    // By depth
    do {
	int pos;
	for (pos = 0; pos < mAgent.GetContCount(owner); pos++) {
	    string ccomp, cval;
	    mAgent.GetCont(pos, ccomp, cval, owner);
	    if (ccomp == comp) {
		break;
	    }
	}
	path.prepend_index(pos);
	comp = owner;
    } while (!comp.empty());
    return path;
}

bool ContentTreeMdl::IsIterValid(const iterator& iter) const
{
    bool res = (iStamp == iter.get_stamp());
    return res;
}

bool ContentTreeMdl::iter_is_valid(const iterator& iter) const
{
    // Anything that modifies the model's structure should change the model's stamp, so that old iters are ignored.
    return IsIterValid(iter) && Gtk::TreeModel::iter_is_valid(iter);
}

void ContentTreeMdl::get_value_vfunc(const TreeModel::iterator& iter, int column, Glib::ValueBase& value) const
{
    if (IsIterValid(iter)) {
	if (column < iColRec.size()) {
	    GType coltype = get_column_type_vfunc(column);
	    if (column == ContentTreeClrec::KCol_Name) {
		Glib::Value<Glib::ustring> sval;
		sval.init(coltype);
		GlueItem* gi = (GlueItem*) iter.gobj()->user_data;
		string data = MElem::GetContentLName(gi->mContName);
		sval.set(data.c_str());
		value.init(coltype);
		value = sval;
	    }
	    else if (column == ContentTreeClrec::KCol_Path) {
		Glib::Value<Glib::ustring> sval;
		sval.init(coltype);
		GlueItem* gi = (GlueItem*) iter.gobj()->user_data;
		string data = gi->mContName;
		sval.set(data.c_str());
		value.init(coltype);
		value = sval;
	    }
	    else if (column == ContentTreeClrec::KCol_Value) {
		Glib::Value<Glib::ustring> sval;
		sval.init(coltype);
		GlueItem* gi = (GlueItem*) iter.gobj()->user_data;
		string data;
		mAgent.GetCont(data, gi->mContName);
		sval.set(data.c_str());
		value.init(coltype);
		value = sval;
	    }
	}
    }
}

string ContentTreeMdl::get_next_comp(const string& aComp) 
{
    string res;
    string owner = mAgent.GetContentOwner(aComp);
    int count = mAgent.GetContCount();
    int ind = GetCompInd(aComp);
    if (ind < (count - 1)) {
	string cval;
	mAgent.GetCont(ind + 1, res, cval, owner);
    }
    return res;
}

bool ContentTreeMdl::iter_next_vfunc(const iterator& iter, iterator& iter_next) const
{
    bool res = false;
    iter_next = iterator();
    if (IsIterValid(iter)) {
	GlueItem* node = (GlueItem*) iter.gobj()->user_data;
	string next = ((ContentTreeMdl*) this)->get_next_comp(node->mContName);
	if (!next.empty()) {
	    iter_next.set_stamp(iStamp);
	    iter_next.gobj()->user_data = AddGlueItem(next);
	    res = true;
	}
    }
    return res;
}

int ContentTreeMdl::iter_n_children_vfunc(const iterator& iter) const
{
    int res = 0;
    if (IsIterValid(iter)) {
	GlueItem* node = (GlueItem*) iter.gobj()->user_data;
	res = mAgent.GetContCount(node->mContName);
    }
    return res;
}

bool ContentTreeMdl::iter_children_vfunc(const iterator& parent, iterator& iter) const
{
    return iter_nth_child_vfunc(parent, 0, iter);
}

bool ContentTreeMdl::iter_has_child_vfunc(const iterator& iter) const
{
    bool res = false;
    if (IsIterValid(iter)) {
	GlueItem* node = (GlueItem*) iter.gobj()->user_data;
	res = (mAgent.GetContCount(node->mContName) > 0);
    }
    return res;
}

bool ContentTreeMdl::iter_nth_child_vfunc(const iterator& parent, int n, iterator& iter) const
{
    bool res = false;
    if (IsIterValid(parent)) {
	GlueItem* owner = (GlueItem*) parent.gobj()->user_data;
	string name, val;
	if (n < mAgent.GetContCount(owner->mContName)) {
	    mAgent.GetCont(n, name, val, owner->mContName);
	    iter.set_stamp(iStamp);
	    iter.gobj()->user_data = AddGlueItem(name);
	    res = true;
	}
    }
    return res;
}

bool ContentTreeMdl::iter_nth_root_child_vfunc(int n, iterator& iter) const
{
    bool res = false;
    if (n < mAgent.GetContCount()) {
	iter.set_stamp(iStamp);
	string comp, cval;
	mAgent.GetCont(n, comp, cval);
	iter.gobj()->user_data = AddGlueItem(comp);
	res = true;
    }
    return res;
}

bool ContentTreeMdl::iter_parent_vfunc(const iterator& child, iterator& iter) const
{
    bool res = false;
    bool valid = IsIterValid(child);
    if (valid) {
	GlueItem* comp = (GlueItem*) child.gobj()->user_data;
	if (!comp->mContName.empty()) {
	    string owner = mAgent.GetContentOwner(comp->mContName);
	    iter.set_stamp(iStamp);
	    iter.gobj()->user_data = AddGlueItem(owner);
	    res = true;
	}
    }
    return res;
}

bool ContentTreeMdl::row_draggable_vfunc(const TreeModel::Path& path) const
{
    return true;
}

bool ContentTreeMdl::drag_data_get_vfunc(const TreeModel::Path& path, Gtk::SelectionData& selection_data) const
{
    bool res = false;
    return res;
}

bool ContentTreeMdl::drag_data_delete_vfunc(const TreeModel::Path& path)
{
    return true;
}

void ContentTreeMdl::on_comp_deleting(MElem* aComp)
{
}

void ContentTreeMdl::on_comp_adding(MElem* aComp)
{
}

void ContentTreeMdl::on_comp_changed(MElem* aComp)
{
}


// Content navigation widget

NaviContent::NaviContent(const MElem& aAgent): mAgent(aAgent)
{
    set_headers_visible(false);
    Glib::RefPtr<ContentTreeMdl> mdl = ContentTreeMdl::create(mAgent);
    set_model(mdl);
    append_column( "name", mdl->ColRec().name);
    append_column( "value", mdl->ColRec().value);
}

NaviContent::~NaviContent()
{
}

bool NaviContent::on_button_press_event(GdkEventButton* event)
{
    iPressX = event->x;
    iPressY = event->y;
    return TreeView::on_button_press_event(event);
}

void NaviContent::on_drag_begin(const Glib::RefPtr<Gdk::DragContext>& context)
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

void NaviContent::set_source_row(const Glib::RefPtr<Gdk::DragContext>& context, Glib::RefPtr<Gtk::TreeModel>& model, Gtk::TreePath& source_row)
{
    Gtk::TreeRowReference* rr = new Gtk::TreeRowReference(model, source_row);
    context->set_data(Glib::Quark("gtk-tree-view-source-row"), rr->gobj());
    GtkTreeRowReference *ref = (GtkTreeRowReference*) g_object_get_data (G_OBJECT (context->gobj()), "gtk-tree-view-source-row");
    GtkTreePath* res = NULL;
    if (ref) {
	res =  gtk_tree_row_reference_get_path (ref);
    }

}

void NaviContent::on_drag_data_get(const Glib::RefPtr<Gdk::DragContext>& context, Gtk::SelectionData& selection_data, guint info, guint time)
{
    TreeView::on_drag_data_get(context, selection_data, info, time);
}

void NaviContent::on_row_activated(const TreeModel::Path& path, TreeViewColumn* column)
{
    TreeModel::iterator iter = get_model()->get_iter(path);
    string node = (*iter).get_value(ContentTreeMdl::ColRec().name);
    iSigCompActivated.emit(node);
}

NaviContent::tSigCompSelected NaviContent::SignalCompSelected()
{
    return iSigCompSelected;
}

NaviContent::tSigCompActivated NaviContent::SignalCompActivated()
{
    return iSigCompActivated;
}


// Content select dialog

ContentSelectDlg::ContentSelectDlg(const string& aTitle, const MElem& aAgent): Gtk::Dialog(aTitle)
{
    add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
    add_button(Gtk::Stock::OK, Gtk::RESPONSE_OK);
    Gtk::VBox* cont_area = get_vbox();

    mNavi = new NaviContent(aAgent);
    mNavi->show();
    Glib::RefPtr<TreeSelection> sel = mNavi->get_selection();
    sel->set_mode(SELECTION_SINGLE);
//    mSw.add(*mNavi);
//    mSw.show();
//    cont_area->pack_start(mSw);
    cont_area->pack_start(*mNavi);
}

void ContentSelectDlg::GetData(string& aData)
{
    Glib::RefPtr<TreeSelection> sel = mNavi->get_selection();
    TreeModel::iterator slit = sel->get_selected();
    string cont_path;
    slit->get_value(ContentTreeClrec::KCol_Path, cont_path);
    aData = cont_path;
}



#include <gtkmm.h>
#include <gtkmm/treerowreference.h>
#include "cntview.h"
#include <iostream>

const string KMsgConfirmRm = "Remove the content selected?";

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
	    string comp =  mAgent.GetContComp(owner, ind);
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
//     return mAgent.GetContCount();
     return 1;
}

bool ContentTreeMdl::get_iter_vfunc(const Path& path, iterator& iter) const
{
    bool res = false;
    string comp;
    string owner;
    unsigned depth = path.size();
    vector<int> indcv = path.get_indices();
    for (int dc = 0; dc < depth; dc++) {
	int count = (dc == 0) ? 1 : mAgent.GetContCount(owner);
	int ind = indcv.at(dc);
	if (ind >= count) {
	    return false;
	}
	comp = (dc == 0) ? string() : mAgent.GetContComp(owner, ind);
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
    if (!comp.empty()) {
	// By depth
	do {
	    string owner = MElem::GetContentOwner(comp);
	    int pos;
	    for (pos = 0; pos < mAgent.GetContCount(owner); pos++) {
		string ccomp = mAgent.GetContComp(owner, pos);
		if (ccomp == comp) {
		    break;
		}
	    }
	    path.prepend_index(pos);
	    comp = owner;
	} while (!comp.empty());
    }
    path.prepend_index(0);
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
		string data = mAgent.GetContent(gi->mContName);
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
    if (!aComp.empty()) {
	string owner = mAgent.GetContentOwner(aComp);
	int count = mAgent.GetContCount(owner);
	int ind = GetCompInd(aComp);
	if (ind < (count - 1)) {
	    res = mAgent.GetContComp(owner, ind + 1);
	}
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
	if (n < mAgent.GetContCount(owner->mContName)) {
	    string name = mAgent.GetContComp(owner->mContName, n);
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
    /*
    if (n < mAgent.GetContCount()) {
	iter.set_stamp(iStamp);
	string comp = mAgent.GetContComp(string(), n);
	iter.gobj()->user_data = AddGlueItem(comp);
	res = true;
    }
    */
    __ASSERT(n==0);
    if (n == 0 ) {
	iter.set_stamp(iStamp);
	string comp = "";
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

void ContentTreeMdl::set_value_impl(const iterator& row, int column, const Glib::ValueBase& value)
{
    cout << "ContentTreeMdl::set_value_impl" << endl;
    const Glib::Value<Glib::ustring>& sval = (const Glib::Value<Glib::ustring>&) value;
    Glib::ustring val = sval.get();
    GlueItem* gi = (GlueItem*) row.gobj()->user_data;
    string strval(val);
    MElem& agt = (MElem&) mAgent;
    //((MElem&) mAgent).ChangeCont(val, ETrue, gi->mContName);

    agt.AppendMutation(TMut(ENt_Cont, ENa_Id, gi->mContName, ENa_MutVal, val));
    agt.Mutate(false, false, true, agt.GetRoot());
    // Notify model base of row changed (signal "row-changed") will be emitted to the view
    row_changed(get_path_vfunc(row), row);
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

void ContentTreeMdl::on_row_changed(const TreeModel::Path& path, const TreeModel::iterator& iter)
{
    cout << "ContentTreeMdl::on_row_changed" << endl;
    Gtk::TreeModel::on_row_changed(path, iter);
}

// Content navigation widget

NaviContent::NaviContent(MElem& aAgent): mAgent(aAgent)
{
    set_headers_visible(false);
    SetAgent();
}

NaviContent::~NaviContent()
{
    UnsetAgent();
}

void NaviContent::UnsetAgent()
{
    mAgent.SetObserver(NULL);
    remove_all_columns();
    unset_model();
    Glib::RefPtr<TreeModel> curmdl = get_model();
    curmdl.reset();
}

void NaviContent::SetAgent()
{
    mAgent.SetObserver(this);
    Glib::RefPtr<ContentTreeMdl> mdl = ContentTreeMdl::create(mAgent);
    GtkTreeModel* model = mdl->Gtk::TreeModel::gobj();
    set_model(mdl);
    append_column( "name", mdl->ColRec().name);
    append_column_editable( "value", mdl->ColRec().value);
    expand_all();
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

// From MAgentObserver

void NaviContent::OnCompDeleting(MElem& aComp, TBool aSoft, TBool aModif)
{
}

void NaviContent::OnCompAdding(MElem& aComp, TBool aModif)
{
}

TBool NaviContent::OnCompChanged(MElem& aComp, const string& aContName, TBool aModif)
{
//    UnsetAgent();
//    SetAgent();
}

TBool NaviContent::OnChanged(MElem& aComp)
{
//    UnsetAgent();
//    SetAgent();
}

TBool NaviContent::OnCompRenamed(MElem& aComp, const string& aOldName)
{
    return false;
}

void NaviContent::OnCompMutated(const MElem* aNode)
{
}

MIface* NaviContent::Call(const string& aSpec, string& aRes)
{
    __ASSERT(false);
}

string NaviContent::Mid() const
{
    __ASSERT(false);
}


// Content select dialog

ContentSelectDlg::ContentSelectDlg(const string& aTitle, MElem& aAgent): Gtk::Dialog(aTitle), mAgent(aAgent), mNavi(NULL)
{
    // Action area
    //    add_button(Gtk::Stock::ADD, ERsp_Add);
    add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
    add_button(Gtk::Stock::OK, Gtk::RESPONSE_OK);
    // Button "Add"
    mBtnAdd = new Button("Add");
    mBtnAdd->show();
    mBtnAdd->set_sensitive(false);
    get_action_area()->pack_end(*mBtnAdd);
    mBtnAdd->signal_pressed().connect(sigc::mem_fun(*this, &ContentSelectDlg::OnAddButtonPressed));
    // Button "Remove"
    mBtnRm = new Button("Remove");
    mBtnRm->show();
    mBtnRm->set_sensitive(false);
    get_action_area()->pack_end(*mBtnRm);
    mBtnRm->signal_pressed().connect(sigc::mem_fun(*this, &ContentSelectDlg::OnRmButtonPressed));

    // Data area
    Gtk::VBox* cont_area = get_vbox();
    // Content navigation tree view
    mNavi = new NaviContent(aAgent);
    mNavi->signal_row_activated().connect(sigc::mem_fun(*this, &ContentSelectDlg::OnModelRowActivated));
    mNavi->signal_columns_changed().connect(sigc::mem_fun(*this, &ContentSelectDlg::OnColumnChanged));
    mNavi->signal_cursor_changed().connect(sigc::mem_fun(*this, &ContentSelectDlg::OnCursorChanged));
    mNavi->expand_all();
    mNavi->show();
    Glib::RefPtr<TreeSelection> sel = mNavi->get_selection();
    sel->set_mode(SELECTION_SINGLE);
    set_response_sensitive(Gtk::RESPONSE_APPLY, false);
//    mSw.add(*mNavi);
//    mSw.show();
//    cont_area->pack_start(mSw);
    cont_area->pack_start(*mNavi);
}

ContentSelectDlg::~ContentSelectDlg()
{
    if (mNavi != NULL) {
	delete mNavi;
    }
}

void ContentSelectDlg::GetData(string& aData)
{
    Glib::RefPtr<TreeSelection> sel = mNavi->get_selection();
    TreeModel::iterator slit = sel->get_selected();
    string cont_path;
    slit->get_value(ContentTreeClrec::KCol_Path, cont_path);
    aData = cont_path;
}

void ContentSelectDlg::OnModelRowActivated(const TreeModel::Path& aPath, TreeViewColumn* aColumn)
{
    cout << "ContentSelectDlg::OnModelRowActivated" << endl;
} 

void ContentSelectDlg::OnColumnChanged() 
{
    cout << "ContentSelectDlg::OnColumnChanged" << endl;
} 

void ContentSelectDlg::OnCursorChanged() 
{
    cout << "ContentSelectDlg::OnCursorChanged" << endl;
    Glib::RefPtr<TreeSelection> sec = mNavi->get_selection();
    TreeModel::iterator it = sec->get_selected();
    bool isvalid = it.operator bool();
    if (isvalid) {
	mBtnAdd->set_sensitive(true);
	mBtnRm->set_sensitive(true);
    } else {
	mBtnAdd->set_sensitive(false);
	mBtnRm->set_sensitive(false);
    }
} 

void ContentSelectDlg::on_response(int response_id)
{
    cout << "ContentSelectDlg::on_response: " << response_id << endl;
    if (response_id == ERsp_Add) { // Adding content
	Glib::RefPtr<TreeSelection> sec = mNavi->get_selection();
	TreeModel::iterator it = sec->get_selected();
	Glib::ustring path;
	it->get_value(ContentTreeClrec::KCol_Path, path);
	cout << "Path: " << path << endl;
	string sName, sValue;
	ContentAddDlg* dlg = new ContentAddDlg("Add content");
	int res = dlg->run();
	if (res == Gtk::RESPONSE_OK) {
	    dlg->GetData(sName, sValue);
	    mAgent.AppendMutation(TMut(ENt_Cont, ENa_Id, sName, ENa_MutVal, sValue));
	    mAgent.Mutate(false, true, true, mAgent.GetRoot());
	//    mAgent.ChangeCont(sValue, EFalse, Elem::ContentCompId(path, sName));
	}
	delete dlg;
    }
}

void ContentSelectDlg::OnAddButtonPressed()
{
    Glib::RefPtr<TreeSelection> sec = mNavi->get_selection();
    TreeModel::iterator it = sec->get_selected();
    Glib::ustring path;
    it->get_value(ContentTreeClrec::KCol_Path, path);
    cout << "Path: " << path << endl;
    string sName, sValue;
    ContentAddDlg* dlg = new ContentAddDlg("Add content");
    int res = dlg->run();
    if (res == Gtk::RESPONSE_OK) {
	dlg->GetData(sName, sValue);
	//mAgent.ChangeCont(sValue, ETrue, Elem::ContentCompId(path, sName));
	mAgent.AppendMutation(TMut(ENt_Cont, ENa_Id, Elem::ContentCompId(path, sName), ENa_MutVal, sValue));
	mAgent.Mutate(false, true, true, mAgent.GetRoot());
    }
    delete dlg;
}

void ContentSelectDlg::OnRmButtonPressed()
{
    Glib::RefPtr<TreeSelection> sec = mNavi->get_selection();
    TreeModel::iterator it = sec->get_selected();
    Glib::ustring path;
    it->get_value(ContentTreeClrec::KCol_Path, path);
    cout << "Path: " << path << endl;
    // Confirmation dialog
    Gtk::MessageDialog cdlg(KMsgConfirmRm, false, MESSAGE_INFO, BUTTONS_OK_CANCEL, true);
    int result = cdlg.run();
    if (result == Gtk::RESPONSE_OK) {
	string val = "{" + MElem::GetContentLName(path) + ":-}";
	mAgent.ChangeCont(val, ETrue, MElem::GetContentOwner(path));
    }
}




ContentAddDlg::ContentAddDlg(const string& aTitle): Gtk::Dialog(aTitle)
{
    add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
    add_button(Gtk::Stock::OK, Gtk::RESPONSE_OK);
    Gtk::VBox* cont_area = get_vbox();
    // Name
    Gtk::HBox* mNameArea = new Gtk::HBox();
    cont_area->pack_start(*mNameArea);
    mNameArea->show();
    mNameLabel = new Gtk::Label();
    mNameLabel->set_text("Name: ");
    mNameLabel->show();
    mNameArea->pack_start(*mNameLabel, false, false, 4);
    Glib::RefPtr<Gtk::TextBuffer> namebuf = Gtk::TextBuffer::create();
    mName = new Gtk::TextView(namebuf);
    mName->show();
    mNameArea->pack_start(*mName, false, false, 4);
    // Value
    Gtk::HBox* mValueArea = new Gtk::HBox();
    cont_area->pack_start(*mValueArea);
    mValueArea->show();
    mValueLabel = new Gtk::Label();
    mValueLabel->set_text("Value: ");
    mValueLabel->show();
    mValueArea->pack_start(*mValueLabel, false, false, 4);
    Glib::RefPtr<Gtk::TextBuffer> valuebuf = Gtk::TextBuffer::create();
    mValue = new Gtk::TextView(valuebuf);
    mValue->show();
    mValueArea->pack_start(*mValue, false, false, 4);
}

void ContentAddDlg::GetData(string& aName, string &aValue)
{
    Glib::RefPtr<Gtk::TextBuffer> namebuf = mName->get_buffer();
    aName = namebuf->get_text();
    Glib::RefPtr<Gtk::TextBuffer> valuebuf = mValue->get_buffer();
    aValue = valuebuf->get_text();
}

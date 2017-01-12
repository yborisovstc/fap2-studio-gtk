
#include <iostream>
#include "common.h"
#include "elemdetrp.h"
#include "dlgbase.h"
#include "msset.h"
#include "mdeslog.h"
#include <edge.h>
#include <gtkmm.h>
#include <gtkmm/messagedialog.h>
#include <gtkmm/filechooserdialog.h>
#include "cntview.h"

// [YB] GTK_TREE_MODEL_ROW is the only target supported by tree view model
// All tree views in navigation panel sources are overwritted for now, so this target
// isn't required at the moment
static GtkTargetEntry targetentries[] =
{
    { (gchar*) KDnDTarg_Comp, 0, 0 },
    { (gchar*) KDnDTarg_Import, 0, 4 },
    { (gchar*) "STRING",        0, 0 },
    { (gchar*) "text/plain",    0, 1 },
    { (gchar*) "text/uri-list", 0, 2 },
    //    { (gchar*) "GTK_TREE_MODEL_ROW", 0, 3 },
};

static const Glib::ustring KDlgMsg_Rm_F1 = 
"Attemting to delete node which is referred from another node [%1].\n\nIn order to keep system consistency the mutation will be applied to [%1]. Perform mutation ?";

static const Glib::ustring KDlgMsg_Rm_F2 = 
"Attemting to delete node which has children.\n\nPerform mutation ?";

static const Glib::ustring KDlgMsg_Mut_F2 = 
"Changed node can affect other nodes. There are two options of applying the change:\n\
	1. Make mutation, please note that you'll need then to refresh the the model to actualize the changes in childs\n\
	2. Make phenotypic modification, the change will be for this node only, not affecting other nodes\n\n\
	Make mutation ? Oherwise phonotype modification";

static const Glib::ustring K_Att_Rld = 
"This change can affect other parts of the system, please reload system to apply the affecting changes.";

static const Glib::ustring KDlgMsg_CritDep = 
"There is critical dependency [%1] for this change in the system. So, only phenotypic modification can be done. OK to do the change ?";

static const Glib::ustring K_Att_CritDep = 
"There is critical dependency in node [%1] for this change in the system. Resolve dependency first, or select depending node for mutation.";

static const Glib::ustring K_Att_CritDep_1 = 
"There is critical dependency in node [%1] for this change in the system.\n\
	Please reload system, and resolve the problems if any. Otherwise undo the mutation";

static const Glib::ustring K_Att_WrongPinnedMnode = 
"Node pinned for mutation is not the owner of the current node. Select the correct node for mutation";

static const Glib::ustring K_Att_DeattachedNode = 
"Current node is deattached so cannot be mutated itself. Select some of its owners for mutation";

static const Glib::ustring K_Att_DeattachedParent = 
"Parent node is deattached so doesn't have chromosome and cannot be inherited.";

static const Glib::ustring K_Att_RefToBiggestRank = 
"Referenced node rank is higher than the rank of mutated node";


/*
   static const Gtk::TargetEntry targetentries[] =
   {
   Gtk::TargetEntry("STRING", Gtk::TARGET_SAME_APP, 0),
   Gtk::TargetEntry("text/plain", Gtk::TARGET_SAME_APP, 1),
   Gtk::TargetEntry("text/uri-list", Gtk::TARGET_SAME_APP, 2)
   };
   */

ElemDetRp::ElemDetRp(MElem* aElem, const MCrpProvider& aCrpProv, MSEnv& aStEnv): Gtk::Layout(), iElem(aElem), iCrpProv(aCrpProv),
    mStEnv(aStEnv), iDnDTarg(EDT_Unknown), iDropBaseCandidate(NULL), iReloadRequired(false)
{
    // Set dest with avoiding DestDefaults flags. These flags are only for some trivial DnD 
    // scenarious, but we need to implement requesting edges data during drop motion
    drag_dest_set(Gtk::ArrayHandle_TargetEntry(targetentries, 3, Glib::OWNERSHIP_NONE), Gtk::DestDefaults(0), Gdk::ACTION_COPY | Gdk::ACTION_MOVE);
    // Create comp menu elements
    // TODO [YB] To implement context menu in CRps but not DRp.
    Gtk::Menu_Helpers::MenuElem e_rename("_Rename", sigc::mem_fun(*this, &ElemDetRp::on_comp_menu_rename));
    Gtk::Menu_Helpers::MenuElem e_remove("_Remove", sigc::mem_fun(*this, &ElemDetRp::on_comp_menu_remove));
    Gtk::Menu_Helpers::MenuElem e_editcont("_Edit content", sigc::mem_fun(*this, &ElemDetRp::on_comp_menu_edit_content));
    // TODO [YB] Consider saving from app - saving current app context instead of using context menu
    Gtk::Menu_Helpers::MenuElem e_save_chromo("_Save chromo", sigc::mem_fun(*this, &ElemDetRp::on_comp_menu_save_chromo));
    iCompMenuElems.insert(pair<MCrp::Action, Gtk::Menu_Helpers::MenuElem>(MCrp::EA_Rename, e_rename));
    iCompMenuElems.insert(pair<MCrp::Action, Gtk::Menu_Helpers::MenuElem>(MCrp::EA_Remove, e_remove));
    iCompMenuElems.insert(pair<MCrp::Action, Gtk::Menu_Helpers::MenuElem>(MCrp::EA_Edit_Content, e_editcont));
    iCompMenuElems.insert(pair<MCrp::Action, Gtk::Menu_Helpers::MenuElem>(MCrp::EA_Save_Chromo, e_save_chromo));
    // Setup components context menu
    Gtk::Menu::MenuList& menulist = iCrpContextMenu.items();
    /*
       menulist.push_back(Gtk::Menu_Helpers::MenuElem("_Rename", sigc::mem_fun(*this, &ElemDetRp::on_comp_menu_rename) ) );
       menulist.push_back(Gtk::Menu_Helpers::MenuElem("_Remove", sigc::mem_fun(*this, &ElemDetRp::on_comp_menu_remove) ) );
       */
    /*
       menulist.push_back(iCompMenuElems.at(MCrp::EA_Rename));
       menulist.push_back(iCompMenuElems.at(MCrp::EA_Remove));
       */
    iCrpContextMenu.accelerate(*this);
    bool iserr =  mStEnv.DesLog().IsNodeLogged(iElem, EErr);
    set_name(iserr ? "ElemDrp~err" : "ElemDrp");
}

ElemDetRp::~ElemDetRp()
{
    Erase();
}

void ElemDetRp::Construct()
{
    // Add components
    for (TInt ci = 0; ci < iElem->CompsCount(); ci++) {
	MElem* comp = iElem->GetComp(ci);
	assert(comp != NULL);
	if (comp->IsRemoved()) continue;
	MCrp* rp = iCrpProv.CreateRp(*comp, this);
	if (rp != NULL) {
	    if (IsCrpLogged(rp, EErr)) {
		rp->SetErroneous(true);
	    }
	    Gtk::Widget& rpw = rp->Widget();
	    //rpw.signal_button_press_event().connect(sigc::bind<MElem*>(sigc::mem_fun(*this, &ElemDetRp::on_comp_button_press_ext), comp));
	    // Using specific signal for button press instead of standard because some Crps can have complex layout
	    rp->SignalButtonPress().connect(sigc::bind<MElem*>(sigc::mem_fun(*this, &ElemDetRp::on_comp_button_press), comp));
	    rp->SignalButtonPressName().connect(sigc::bind<MElem*>(sigc::mem_fun(*this, &ElemDetRp::on_comp_button_press_name), comp));
	    add(rpw);
	    iCompRps[comp] = rp;
	    rpw.show();
	} else {
	    rp = iCrpProv.CreateRp(*comp, this);
	    std::cout << "ElemDetRp::Construct, error - cannot create RP [" << comp->Name() << "]" << std::endl;
	}
    }
}

bool ElemDetRp::IsCrpLogged(MCrp* aCrp, TLogRecCtg aCtg) const
{
    return  mStEnv.DesLog().IsNodeLogged(aCrp->Model(), aCtg);
}

MElem* ElemDetRp::GetElem()
{
    return iElem;
}

void ElemDetRp::Erase()
{
    // Remove components
    for (tCrps::iterator it = iCompRps.begin(); it != iCompRps.end(); it++) {
	MCrp* rp = it->second;
	delete rp;
    }
    iCompRps.clear();

}

void ElemDetRp::Refresh()
{
    Erase();
    Construct();
    queue_resize();
}

bool ElemDetRp::IsTypeAllowed(const std::string& aType) const
{
    bool res = false;
    if (aType == ":Elem") {
	res = true;
    }
    return res;
}

void ElemDetRp::on_realize()
{
    Gtk::Layout::on_realize();
    // Base construct
    Construct();
}

void ElemDetRp::on_size_allocate(Gtk::Allocation& aAllc)
{
    //    Gtk::Layout::on_size_allocate(aAllc);
    //    Set allocation to itself and resize the windows. Refer gtk_layout_size_allocate 
    //    Gtk::Layout::on_size_allocate can be invoked instead but it does childs size allocation internally
    //    so it causes ovwrhead because we allocate child sizes here. 
    //    Probably better would be to set layout children requisition first and then run Gtk::Layout::on_size_allocate
    //    which does childs size allocation then. But this approach doesn't work for me - it causes continous resize

    set_allocation(aAllc);
    if (get_realized()) {
	get_window()->move_resize(aAllc.get_x(), aAllc.get_y(), aAllc.get_width(), aAllc.get_height());
	get_bin_window()->resize(aAllc.get_width(), aAllc.get_height());
    }

    // Allocate components
    int compb_x = aAllc.get_width()/2, compb_y = KViewCompGapHight;
    for (TInt ci = 0; ci < iElem->CompsCount(); ci++) {
	MElem* ecomp = iElem->GetComp(ci);
	if (ecomp->IsRemoved()) continue;
	MCrp* crp = iCompRps.at(ecomp);
	Gtk::Widget* comp = &(crp->Widget());
	Gtk::Requisition req = comp->size_request();
	int comp_body_center_x = req.width / 2;
	Gtk::Allocation allc = Gtk::Allocation(compb_x - comp_body_center_x, compb_y, req.width, req.height);
	comp->size_allocate(allc);
	compb_y += req.height + KViewCompGapHight;
    }
}

void ElemDetRp::on_size_request(Gtk::Requisition* aReq)
{
    // Calculate size of comps
    int comp_w = 0, comp_h = KViewCompGapHight;
    for (std::map<MElem*, MCrp*>::iterator it = iCompRps.begin(); it != iCompRps.end(); it++) {
	Gtk::Widget* cpw = &(it->second->Widget());
	Gtk::Requisition req = cpw->size_request();
	comp_w = max(comp_w, req.width);
	comp_h += req.height + KViewCompGapHight;
    }

    aReq->width = comp_w; 
    aReq->height = comp_h + KViewCompGapHight;
}

bool ElemDetRp::on_comp_button_press_ext(GdkEventButton* event, MElem* aComp)
{
    std::cout << "ElemDetRp::on_comp_button_press_ext, comp [" << aComp->Name() << "]" << std::endl;
    if (event->type == GDK_BUTTON_PRESS) {
	if (event->button == 3) {
	    ShowCrpCtxDlg(event, aComp);
	}
	if (event->button == 1) {
	    iSigCompSelected.emit(aComp);
	}
    }
    else if (event->type == GDK_2BUTTON_PRESS) {
	if (event->button != 3) {
	    iSigCompActivated.emit(aComp);
	}
    }
    return false;
}

void ElemDetRp::on_comp_button_press(GdkEventButton* event, MElem* aComp)
{
    std::cout << "ElemDetRp::on_comp_button_press, comp [" << aComp->Name() << "]" << std::endl;
    on_comp_button_press_ext(event, aComp);
}

void ElemDetRp::on_comp_button_press_name(GdkEventButton* event, MElem* aComp)
{
}

bool ElemDetRp::on_drag_motion (const Glib::RefPtr<Gdk::DragContext>& context, int x, int y, guint time)
{
    bool res = false;
    // Checking if source target is local component and action is Move
    Gdk::ListHandle_AtomString src_targets = context->get_targets();
    std::vector<std::string> vtargs = src_targets.operator std::vector<std::string>();
    std::string targ = vtargs.at(0);
    Gdk::DragAction action = context->get_action();
    Gdk::DragAction saction = context->get_suggested_action();
    //std::cout << "ElemDetRp on_drag_motion, iDnDTarg: " << iDnDTarg << ", action: " << action << ", s_action " << saction << std::endl;
    Gtk::Allocation alc = get_allocation();
    Adjustment* adj = get_vadjustment();
    //std::cout << "ElemDetRp on_drag_motion, vadj, low: " << adj->get_lower() << ", upp: " << adj->get_upper() << ", step: " << adj->get_step_increment() << ", val: " << adj->get_value() << ", pg: " << adj->get_page_size() << std::endl;
    iSigDragMotion.emit(*this, x, y);
    if (iDnDTarg == EDT_Unknown) {
	// Detect target type
	drag_get_data(context, targ, time);
    }
    else {
	//std::cout << "ElemDetRp on_drag_motion: targ detected: " << iDnDTarg << ", x: " << x << ", y: " << y << ", wy: " << alc.get_height() << std::endl;
	if (iDnDTarg == EDT_Node) {
	    // Find the nearest node and highligh it
	    MCrp* cand = NULL;
	    for (tCrps::const_iterator it = iCompRps.begin(); it != iCompRps.end() && cand == NULL; it++) {
		MCrp* crp = it->second;
		Widget& crpw = crp->Widget();
		Allocation alc = crpw.get_allocation();
		/*
		if (crp->IsDnDTargSupported(iDnDTarg) && 
			x >= alc.get_x() && x < alc.get_x() + alc.get_width() && y >= alc.get_y() && y < alc.get_y() + alc.get_height()) {
		    cand = crp;
		}
		*/
		if (crp->IsDnDTargSupported(iDnDTarg) && crp->IsIntersected(x, y)) {
		    cand = crp;
		}
	    }
	    if (cand != NULL && (cand != iDropBaseCandidate)) {
		if (iDropBaseCandidate != NULL) {
		    iDropBaseCandidate->SetHighlighted(false);
		}
		iDropBaseCandidate = cand;
		iDropBaseCandidate->SetHighlighted(true);
	    }
	    else if (cand == NULL && iDropBaseCandidate != NULL) {
		iDropBaseCandidate->SetHighlighted(false);
		iDropBaseCandidate = NULL;
	    }
	    queue_resize();
	    res = true;
	    //context->drag_status(Gdk::ACTION_COPY, time);
	    context->drag_status(saction, time);
	} else if (iDnDTarg == EDT_Import) {
	    context->drag_status(saction, time);
	    res = true;
	}
    }
    return res;
}

bool ElemDetRp::on_drag_drop(const Glib::RefPtr<Gdk::DragContext>& context, int x, int y, guint time)
{
    bool res = false;
    Gdk::DragAction action = context->get_action();
    std::cout << "ElemDetRp on_drag_drop, action: " << action << ", target: " << iDnDTarg << std::endl;
    if (iDnDTarg == EDT_Node) {
	context->drag_finish(true, false, time);
	GUri uri;
	if (iDropBaseCandidate != NULL) {
	    iDropBaseCandidate->SetHighlighted(false);
	    iDropBaseCandidate->Model()->GetUri(uri, iElem);
	}
	else {
	    // iElem->GetUri(uri, iElem); This will be empty uri anycase
	}
	if (action == Gdk::ACTION_COPY) {
	    add_node(iDndReceivedData, uri.GetUri());
	}
	else if (action == Gdk::ACTION_MOVE) {
	    move_node(iDndReceivedData, uri.GetUri());
	}
	iDnDTarg = EDT_Unknown;
	iDndReceivedData.clear();
	iDropBaseCandidate = NULL;
	res = true;
	queue_resize();
    } else if (iDnDTarg == EDT_Import) {
	context->drag_finish(true, false, time);
	if (action == Gdk::ACTION_COPY) {
	    import(iDndReceivedData);
	}
	iDnDTarg = EDT_Unknown;
	iDndReceivedData.clear();
	res = true;
	queue_resize();
    }
    return res;
}

void ElemDetRp::on_drag_data_received(const Glib::RefPtr<Gdk::DragContext>& context, int x, int y, const Gtk::SelectionData& selection_data, guint info, guint time)
{
    std::string targ = selection_data.get_target();
    Glib::ustring data = (const char*) selection_data.get_data();
    iDndReceivedData = data;
    std::cout << "ElemDetRp on_drag_data_received, target: " << targ << ", data: " << data << std::endl;
    if (iDnDTarg == EDT_Unknown) {
	// Classify DnD target
	if (targ == KDnDTarg_Comp) {
	    iDnDTarg = EDT_Node;
	    std::cout << "ElemDetRp on_drag_data_received, DnD targ: [EDT_Node], action - MOVE" << std::endl;
	    context->drag_status(Gdk::ACTION_MOVE, time);
	} else if (targ == KDnDTarg_Import) {
	    iDnDTarg = EDT_Import;
	    std::cout << "ElemDetRp on_drag_data_received, DnD targ: [EDT_Import], action - COPY" << std::endl;
	    context->drag_status(Gdk::ACTION_COPY, time);
	} else if (targ == "STRING") {
	    iDnDTarg = EDT_Node;
	    std::cout << "ElemDetRp on_drag_data_received, DnD targ: [EDT_Node], action - COPY" << std::endl;
	    context->drag_status(Gdk::ACTION_COPY, time);
	}
    }
    else {
	// Target type detected
	context->drop_finish(true, time);
    }
    // Finishing drag, not removing the original data
    /*
       context->drag_finish(true, false, time);
       on_node_dropped(data);
       */
}

void ElemDetRp::on_drag_leave(const Glib::RefPtr<Gdk::DragContext>& context, guint time)
{
    std::cout << "ElemDetRp on_drag_leave" << std::endl;
}

void ElemDetRp::on_node_dropped(const std::string& aUri)
{
    std::cout << "ElemDetRp on_node_dropped" << std::endl;
    add_node(aUri);
}

void ElemDetRp::rename_node(const std::string& aNodeUri, const std::string& aNewName)
{
    MElem* dnode = iElem->GetNode(aNodeUri);
    MElem* mutelem = dnode->GetMan();
    if (mutelem != NULL) {
	GUri nuri;
	dnode->GetUri(nuri, mutelem);
	mutelem->AppendMutation(TMut(ENt_Change, ENa_Comp, nuri.GetUri(),
		    ENa_MutAttr, TMut::NodeAttrName(ENa_Id), ENa_MutVal, aNewName));
	mutelem->Mutate(false, false, true, iElem->GetRoot());
	Refresh();
    }
}

void ElemDetRp::import(const std::string& aUri)
{
    MElem* mutelem = iElem;
    /*
    ChromoNode rmut = mutelem->AppendMutation(ENt_Import);
    rmut.SetAttr(ENa_Id, aUri);
    mutelem->Mutate();
    */
    mutelem->AppendMutation(TMut(ENt_Import, ENa_Id, aUri));
    mutelem->Mutate(false, false, true, iElem->GetRoot());
    Refresh();
}

void ElemDetRp::add_node(const std::string& aParentUri, const std::string& aTargetUri)
{
    // TODO [YB] Full type (PEType) needs to be used here. This relates to more serious problem
    // that grayb default provider gives us the types of native but not full type, ref ModulesPath()
    int pos = aParentUri.find(Edge::Type());
    bool use_default_name = pos != string::npos;
    std::string name;
    bool name_ok = true;
    if (use_default_name) {
    }
    else  {
	// Ask for name
	ParEditDlg* dlg = new ParEditDlg("Enter name", "");
	int res = dlg->run();
	if (res == Gtk::RESPONSE_OK) {
	    dlg->GetData(name);
	}
	else {
	    name_ok = false;
	}
	delete dlg;
    }
    if (name_ok) {
	do_add_node(name, aParentUri, aTargetUri);
	Refresh();
    }
}

bool ElemDetRp::IsMutOwnerBased(TNodeType aMut) const 
{
    return aMut == ENt_Move || aMut == ENt_Rm || aMut == ENt_Change;
}

void ElemDetRp::do_add_node(const std::string& aName, const std::string& aParentUri, const std::string& aTargetUri)
{
    // Mutate appending
    MElem* targ = iElem->GetNode(aTargetUri);
    __ASSERT(targ != NULL);
    __ASSERT(!aParentUri.empty());
    MElem* parent = iElem->GetNode(aParentUri);
    __ASSERT(parent != NULL);
    /*
    GUri prnturi;
    parent->GetUri(prnturi, targ);
    __ASSERT(!prnturi.IsErr());
    string prnturis = prnturi.GetUri(true);
    */
    // Using short Uri (owning hier only). It is OK because unique comps name approach applied.
    // Full URI causes problems with creating remote env root.
    string prnturis = parent->GetUri(targ, ETrue);
    string sname(aName);
    if (sname.empty()) {
	stringstream ss;
	ss << rand();
	sname = ss.str();
    }
    targ->AppendMutation(TMut(ENt_Node, ENa_Id, sname, ENa_Parent, prnturis));
    targ->Mutate(false, false, true, iElem->GetRoot());
}

void ElemDetRp::remove_node(const std::string& aNodeUri)
{
    MElem* dnode = iElem->GetNode(aNodeUri);
    __ASSERT(dnode != NULL);
    MElem* mutelem = dnode->GetMan();
    if (mutelem != NULL) {
	GUri nuri;
	dnode->GetUri(nuri, mutelem);
	mutelem->AppendMutation(TMut(ENt_Rm, ENa_Comp, nuri.GetUri()));
	mutelem->Mutate(false, false, true, iElem->GetRoot());
	Refresh();
    }
}

void ElemDetRp::change_content(const string& aNodeUri, const string& aCntPath, const string& aNewContent, bool aRef)
{
    MElem* node = iElem->GetNode(aNodeUri);
    MElem* mutelem = node;
    if (mutelem != NULL) {
	mutelem->AppendMutation(TMut(ENt_Cont, ENa_Id, aCntPath, aRef ? ENa_Ref : ENa_MutVal, aNewContent));
	mutelem->Mutate(false, true, true, iElem->GetRoot());
	Refresh();
    }
}

void ElemDetRp::move_node(const std::string& aNodeUri, const std::string& aDestUri)
{
    MElem* snode = iElem->GetNode(aNodeUri);
    MElem* dnode = iElem->GetNode(aDestUri);
    //if (snode != NULL && snode->GetMan() == iElem) {
    if (false) {
	// Src is comp = changing comps order
    }
    else {
	if (snode != NULL) {
	    MElem* cowner = iElem->GetCommonOwner(snode);
	    if (cowner != NULL) {
		ChromoNode change = cowner->AppendMutation(ENt_Move);
		change.SetAttr(ENa_Id, snode->GetUri(cowner));
		//	change.SetAttr(ENa_MutNode, iElem->GetUri(cowner));
		change.SetAttr(ENa_MutNode, dnode->GetUri(cowner));
		cowner->Mutate(false, true, true, iElem->GetRoot());
		Refresh();
	    }
	}
	else {
	    // Probably external node
	    // TODO [YB] To check explicitlly that it is external node but not wrong uri
	    ChromoNode change = iElem->AppendMutation(ENt_Move);
	    change.SetAttr(ENa_Id, aNodeUri);
	    iElem->Mutate(false, true, true, iElem->GetRoot());
	    Refresh();
	}
    }
}

TBool ElemDetRp::DoIsActionSupported(MElem* aComp, const MCrp::Action& aAction)
{
    TBool res = ETrue;
    if (aAction == MCrp::EA_GetParentsModifs) {
	res = EFalse;
    }
    return res;
}

void ElemDetRp::ShowCrpCtxDlg(GdkEventButton* event, MElem* aComp)
{
    iCompSelected = aComp;
    MCrp* crp = iCompRps.at(aComp);
    Gtk::Menu::MenuList& menulist = iCrpContextMenu.items();
    menulist.erase(menulist.begin(), menulist.end());
    if (crp->IsActionSupported(MCrp::EA_Rename)) {
	menulist.push_back(iCompMenuElems.at(MCrp::EA_Rename));
    }
    if (crp->IsActionSupported(MCrp::EA_Remove)) {
	menulist.push_back(iCompMenuElems.at(MCrp::EA_Remove));
    }
    if (crp->IsActionSupported(MCrp::EA_Edit_Content)) {
	menulist.push_back(iCompMenuElems.at(MCrp::EA_Edit_Content));
    }
    if (crp->IsActionSupported(MCrp::EA_Save_Chromo)) {
	menulist.push_back(iCompMenuElems.at(MCrp::EA_Save_Chromo));
    }
    if (DoIsActionSupported(aComp, MCrp::EA_GetParentsModifs)) {
	menulist.push_back(iCompMenuElems.at(MCrp::EA_GetParentsModifs));
    }
    iCrpContextMenu.popup(event->button, event->time);
}

void ElemDetRp::on_comp_menu_rename()
{
    assert(iCompSelected != NULL);
    ParEditDlg* dlg = new ParEditDlg("Enter new name", iCompSelected->Name());
    int res = dlg->run();
    if (res == Gtk::RESPONSE_OK) {
	std::string newname;
	dlg->GetData(newname);
	GUri duri;
	iCompSelected->GetUri(duri, iElem);
	rename_node(duri.GetUri(true), newname);
    }
    delete dlg;
    iCompSelected = NULL;
}

void ElemDetRp::on_comp_menu_remove()
{
    assert(iCompSelected != NULL);
    GUri nuri;
    iCompSelected->GetUri(nuri, iElem);
    remove_node(nuri.GetUri());
    iCompSelected = NULL;
}

void ElemDetRp::on_comp_menu_edit_content()
{
    assert(iCompSelected != NULL);
    string sCont;
    //iCompSelected->GetCont(sCont);
    MCrp* crp = iCompRps.at(iCompSelected);
    GUri uri;
    iCompSelected->GetUri(uri, iElem);
    GUri uri_cont;
    crp->GetContentUri(uri_cont);
    uri += uri_cont;
    MElem* cnode = iElem->GetNode(uri);
    ContentSelectDlg* sdlg = new ContentSelectDlg("Edit content", *cnode);
    int res = sdlg->run();
    if (res == Gtk::RESPONSE_OK) {
	/*
	string sCpath;
	sdlg->GetData(sCpath);
	string sValue = cnode->GetContent(sCpath);
	*/
	delete sdlg;
	/*
	TextEditDlg* edlg = new TextEditDlg("Edit content", sValue);
	res = edlg->run();
	if (res == Gtk::RESPONSE_OK) {
	    edlg->GetData(sValue);
	    change_content(uri.GetUri(), sCpath, sValue);
	}
	delete edlg;
	*/
    } else {
	delete sdlg;
    }
    iCompSelected = NULL;
}

void ElemDetRp::on_comp_menu_save_chromo()
{
    assert(iCompSelected != NULL);

    Gtk::FileChooserDialog dialog("Saving chromo - Please choose a file", Gtk::FILE_CHOOSER_ACTION_SAVE);
    //dialog.set_transient_for(*this);

    //Add response buttons the the dialog:
    dialog.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
    dialog.add_button(Gtk::Stock::SAVE_AS, Gtk::RESPONSE_OK);
    // Set filter
    Gtk::FileFilter filter;
    filter.set_name("DES model files");
    filter.add_mime_type("application/xml");
    dialog.add_filter(filter);

    int result = dialog.run();
    if (result == Gtk::RESPONSE_OK) {
	std::string filename = dialog.get_filename();
	iCompSelected->Chromos().Save(filename);
    }

    iCompSelected = NULL;
}

// Shifting of component to the latest rank
// Just moving mutation, it doesn't affect model at all, but required model refresh
void ElemDetRp::ShiftCompToEnd(MElem* aOwner, MElem* aComp) 
{
    ChromoNode oroot = aOwner->Chromos().Root();
    ChromoNode croot = aComp->Chromos().Root();
    __ASSERT((*croot.Parent()).Handle() == oroot.Handle());
    croot.MoveToEnd();
}

void ElemDetRp::DoUdno()
{
}

// Inserting remote chromo into the current one, i.e. mutating current node by remote chromo.
void ElemDetRp::DoOnActionInsert()
{
    Gtk::FileChooserDialog dialog("Adding node - Please choose a file", Gtk::FILE_CHOOSER_ACTION_OPEN);
    //Add response buttons the the dialog:
    dialog.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
    dialog.add_button(Gtk::Stock::OPEN, Gtk::RESPONSE_OK);
    // Set filter
    Gtk::FileFilter filter;
    filter.set_name("DES model files");
    filter.add_mime_type("application/xml");
    dialog.add_filter(filter);

    int result = dialog.run();
    if (result == Gtk::RESPONSE_OK) {
	std::string filename = dialog.get_filename();
	TBool res = iElem->AppendMutation(filename);
	if (res) {
	    iElem->Mutate(false, true, true, iElem->GetRoot());
	    Refresh();
	}
    }
}

const string sElemDrpType = "ElemDrp";

const string& ElemDrp::Type()
{
    return sElemDrpType;
}

string ElemDrp::EType()
{
    return Elem::PEType();
}

ElemDrp::ElemDrp(MElem* aElem, const MCrpProvider& aCrpProv, MSEnv& aStEnv)
{
    iRp = new ElemDetRp(aElem, aCrpProv, aStEnv);
}

ElemDrp::~ElemDrp()
{
    delete iRp;
}

void *ElemDrp::DoGetObj(const string& aName)
{
    void* res = NULL;
    if (aName ==  Type()) {
	res = this;
    }
    if (aName ==  MDrp::Type()) {
	res = (MDrp*) this;
    }
    return res;
}

Gtk::Widget& ElemDrp::Widget()
{
    return *iRp;
}

ElemDrp::tSigCompSelected ElemDrp::SignalCompSelected()
{
    return iRp->iSigCompSelected;
}

ElemDrp::tSigCompActivated ElemDrp::SignalCompActivated()
{
    return iRp->iSigCompActivated;
}

MElem* ElemDrp::Model()
{
    return iRp->iElem;
}

void ElemDrp::Udno()
{
    iRp->DoUdno();
}

MDrp::tSigDragMotion ElemDrp::SignalDragMotion()
{
    return iRp->iSigDragMotion;
}

void ElemDrp::OnActionInsert()
{
    iRp->DoOnActionInsert();
}

MDrp::tSigAttention ElemDrp::SignalAttention() 
{
    return iRp->mSignalAttention;
};

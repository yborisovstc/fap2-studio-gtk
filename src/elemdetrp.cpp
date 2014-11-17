
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

// [YB] GTK_TREE_MODEL_ROW is the only target supported by tree view model
// All tree views in navigation panel sources are overwritted for now, so this target
// isn't required at the moment
static GtkTargetEntry targetentries[] =
{
    { (gchar*) KDnDTarg_Comp, 0, 0 },
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

ElemDetRp::ElemDetRp(Elem* aElem, const MCrpProvider& aCrpProv, MSEnv& aStEnv): Gtk::Layout(), iElem(aElem), iCrpProv(aCrpProv),
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
    Gtk::Menu_Helpers::MenuElem e_transtomut("_Transform to mut", sigc::mem_fun(*this, &ElemDetRp::on_comp_menu_trans_to_mut));
    iCompMenuElems.insert(pair<MCrp::Action, Gtk::Menu_Helpers::MenuElem>(MCrp::EA_Rename, e_rename));
    iCompMenuElems.insert(pair<MCrp::Action, Gtk::Menu_Helpers::MenuElem>(MCrp::EA_Remove, e_remove));
    iCompMenuElems.insert(pair<MCrp::Action, Gtk::Menu_Helpers::MenuElem>(MCrp::EA_Edit_Content, e_editcont));
    iCompMenuElems.insert(pair<MCrp::Action, Gtk::Menu_Helpers::MenuElem>(MCrp::EA_Save_Chromo, e_save_chromo));
    iCompMenuElems.insert(pair<MCrp::Action, Gtk::Menu_Helpers::MenuElem>(MCrp::EA_TransToMut, e_transtomut));
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
    bool iserr =  mStEnv.DesLog().IsNodeLogged(iElem, MLogRec::EErr);
    set_name(iserr ? "ElemDrp~err" : "ElemDrp");
}

ElemDetRp::~ElemDetRp()
{
    Erase();
}

void ElemDetRp::Construct()
{
    // Add components
    for (std::vector<Elem*>::iterator it = iElem->Comps().begin(); it != iElem->Comps().end(); it++) {
	Elem* comp = *it;
	assert(comp != NULL);
	if (!comp->IsRemoved()) {
	    MCrp* rp = iCrpProv.CreateRp(*comp, this);
	    if (rp == NULL) {
		rp = iCrpProv.CreateRp(*comp, this);
	    }
	    if (IsCrpLogged(rp, MLogRec::EErr)) {
		rp->SetErroneous(true);
	    }
	    Gtk::Widget& rpw = rp->Widget();
	    //rpw.signal_button_press_event().connect(sigc::bind<Elem*>(sigc::mem_fun(*this, &ElemDetRp::on_comp_button_press_ext), comp));
	    // Using specific signal for button press instead of standard because some Crps can have complex layout
	    rp->SignalButtonPress().connect(sigc::bind<Elem*>(sigc::mem_fun(*this, &ElemDetRp::on_comp_button_press), comp));
	    rp->SignalButtonPressName().connect(sigc::bind<Elem*>(sigc::mem_fun(*this, &ElemDetRp::on_comp_button_press_name), comp));
	    add(rpw);
	    iCompRps[comp] = rp;
	    rpw.show();
	}
    }
}

bool ElemDetRp::IsCrpLogged(MCrp* aCrp, MLogRec::TLogRecCtg aCtg) const
{
    return  mStEnv.DesLog().IsNodeLogged(aCrp->Model(), aCtg);
}

Elem* ElemDetRp::GetElem()
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
    for (std::vector<Elem*>::iterator it = iElem->Comps().begin(); it != iElem->Comps().end(); it++) {
	Elem* comp = *it;
	if (!comp->IsRemoved()) {
	    MCrp* crp = iCompRps.at(*it);
	    Gtk::Widget* comp = &(crp->Widget());
	    Gtk::Requisition req = comp->size_request();
	    int comp_body_center_x = req.width / 2;
	    Gtk::Allocation allc = Gtk::Allocation(compb_x - comp_body_center_x, compb_y, req.width, req.height);
	    comp->size_allocate(allc);
	    compb_y += req.height + KViewCompGapHight;
	}
    }
}

void ElemDetRp::on_size_request(Gtk::Requisition* aReq)
{
    // Calculate size of comps
    int comp_w = 0, comp_h = KViewCompGapHight;
    for (std::map<Elem*, MCrp*>::iterator it = iCompRps.begin(); it != iCompRps.end(); it++) {
	Gtk::Widget* cpw = &(it->second->Widget());
	Gtk::Requisition req = cpw->size_request();
	comp_w = max(comp_w, req.width);
	comp_h += req.height + KViewCompGapHight;
    }

    aReq->width = comp_w; 
    aReq->height = comp_h + KViewCompGapHight;
}

bool ElemDetRp::on_comp_button_press_ext(GdkEventButton* event, Elem* aComp)
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

void ElemDetRp::on_comp_button_press(GdkEventButton* event, Elem* aComp)
{
    std::cout << "ElemDetRp::on_comp_button_press, comp [" << aComp->Name() << "]" << std::endl;
    on_comp_button_press_ext(event, aComp);
}

void ElemDetRp::on_comp_button_press_name(GdkEventButton* event, Elem* aComp)
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
	    iElem->GetUri(uri, iElem);
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
	    std::cout << "ElemDetRp on_drag_data_received, DnD targ: " << iDnDTarg << ", action - MOVE" << std::endl;
	    context->drag_status(Gdk::ACTION_MOVE, time);
	}
	if (targ == "STRING") {
	    iDnDTarg = EDT_Node;
	    std::cout << "ElemDetRp on_drag_data_received, DnD targ: " << iDnDTarg << ", action - COPY" << std::endl;
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
    // Get major dependency
    /*
       Elem* dnode = iElem->GetNode(aNodeUri);
       Elem::TMDep mdep = dnode->GetMajorDep();
       Elem* mnode = mdep.first.first;
       if (mnode == NULL) {
       mnode = iElem;
       }
       Elem* mutelem = mnode->GetAttachingMgr();
       */

    Elem* dnode = iElem->GetNode(aNodeUri);
    bool unsafe = false;
    Elem* mutelem = GetObjForSafeMut(iElem, dnode, ENt_Change, unsafe);
    if (mutelem != NULL) {
	ChromoNode smutr = mutelem->Mutation().Root();
	GUri nuri;
	dnode->GetRUri(nuri, mutelem);
	ChromoNode change = smutr.AddChild(ENt_Change);
	change.SetAttr(ENa_MutNode, nuri.GetUri());
	change.SetAttr(ENa_MutAttr, GUriBase::NodeAttrName(ENa_Id));
	change.SetAttr(ENa_MutVal, aNewName);
	mutelem->Mutate(false, !unsafe);
	Refresh();
    }
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

Elem* ElemDetRp::GetObjForSafeMut(Elem* aMnode, Elem* aNode, TNodeType aMutType, bool& aUnsafe) 
{
    aMnode = iElem;
    Elem* res = aNode;
    string att;
    aUnsafe = false;
    MStSetting<bool>& ena_pheno_s = mStEnv.Settings().GetSetting(MStSettings::ESts_EnablePhenoModif, ena_pheno_s);
    bool ena_pheno = ena_pheno_s.Get(ena_pheno);
    if (ena_pheno) {
	MStSetting<Glib::ustring>& pinned_mut_node_s = mStEnv.Settings().GetSetting(MStSettings::ESts_PinnedMutNode, pinned_mut_node_s);
	const Glib::ustring& pinned_mut_node = pinned_mut_node_s.Get(pinned_mut_node);
	if (!pinned_mut_node.empty()) {
	    Elem* pnode = iElem->GetNode(pinned_mut_node);
	    if (pnode == NULL || !pnode->IsComp(aMnode)) {
		res = NULL;
		att = K_Att_WrongPinnedMnode;
	    }
	    else {
		aMnode = pnode;
	    }
	}
    }
    if (res != NULL && ena_pheno && !aMnode->IsChromoAttached()) {
	// Request that when pheno is enabled that mutated node to be specified explicitly
	res = NULL;
	att = K_Att_DeattachedNode;
    }
    if (res != NULL) {
	Rank noderank;
	Rank mnoderank;
	res->GetRank(noderank, res->Chromos().Root());
	aMnode->GetRank(mnoderank, aMnode->Chromos().Root());
	Rank rank = noderank;
	// Checking critical deps
	Elem::TMDep dep = aNode->GetMajorDep(aMutType, MChromo::EDl_Critical);
	//Elem::TMDep dep = aNode->GetMajorDep();
	if (dep.first.first != NULL) {
	    Rank deprank;
	    Elem::GetDepRank(dep, deprank);
	    if (deprank > rank && !deprank.IsRankOf(rank)) {
		res = dep.first.first;
		rank = deprank;
	    }
	}
	/* Don't block mut even crit dep. 
	   if (res != aMnode && rank > mnoderank && !rank.IsRankOf(mnoderank)) {
	   att = Glib::ustring::compose(K_Att_CritDep, res->GetUri());
	   res = NULL;
	   }
	   */
	if (res != aMnode && rank > mnoderank && !rank.IsRankOf(mnoderank)) {
	    att = Glib::ustring::compose(K_Att_CritDep_1, res->GetUri());
	    res = aMnode;
	    aUnsafe = true;
	}
#if 0
	// [YB] Verbose handling of error is disabled at the moment, commented out
	if (res != aMnode && rank > mnoderank && !rank.IsRankOf(mnoderank) && !ena_pheno) {
	    // Safe mut point is out of scope, but pheno modif is not enabled, need to say to user
	    int dres = RESPONSE_OK;
	    MessageDialog* dlg = new MessageDialog(Glib::ustring::compose(KDlgMsg_CritDep, res->GetUri()), 
		    false, MESSAGE_INFO, BUTTONS_OK_CANCEL, true);
	    dres = dlg->run();
	    delete dlg;
	    if (dres == RESPONSE_CANCEL) {
		// User reject pheno mutation proposed, just cancel operation
		res = NULL;
	    }
	}
#endif
	// Checking affecting deps
	if (res != NULL) {
	    bool isaffdep = false;
	    Elem* affdep = NULL;
	    dep = aNode->GetMajorDep(aMutType, MChromo::EDl_Affecting);
	    if (dep.first.first != NULL) {
		Rank deprank;
		Elem::GetDepRank(dep, deprank);
		if (deprank > rank && !deprank.IsRankOf(rank)) {
		    affdep = dep.first.first;
		    rank = deprank;
		    isaffdep = true;
		}
		else if (deprank > noderank && !deprank.IsRankOf(noderank)) {
		    isaffdep = true;
		}
	    }
	    if (isaffdep && affdep != NULL && affdep != aMnode && !aMnode->IsComp(affdep)) {
		// There is affecting dep
		if (!ena_pheno) {
		    att = K_Att_Rld;
		    //iReloadRequired = true;
		    /*
		    // Pheno modif are disabled, notify the user of nececcity of reload
		    res = aNode;
		    MessageDialog* dlg = new MessageDialog(K_Att_Rld, false, MESSAGE_INFO, BUTTONS_OK, true);
		    dlg->run();
		    delete dlg;
		    */
		}
		else {
		    // Pheno modif are enabled, asking the user for the choice: mut or modif
		    int dres = RESPONSE_YES;
		    MessageDialog* dlg = new MessageDialog(Glib::ustring(KDlgMsg_Mut_F2), 
			    false, MESSAGE_INFO, BUTTONS_YES_NO, true);
		    dres = dlg->run();
		    delete dlg;
		    if (dres == RESPONSE_YES) {
			res = aNode;
		    }
		    else {
			res = res->GetCommonOwner(aNode);
		    }
		}
	    }
	}
	if (res != NULL) {
	    if (!res->IsChromoAttached()) {
		res = res->GetAttachingMgr(); 
	    }
#if 0
	    if (!ena_pheno && res != aNode && deptype == ENa_Parent) {
		// Taking into account options of change: geno or pheno, ref fap2 uc_038
		MessageDialog* dlg = new MessageDialog(Glib::ustring(KDlgMsg_Mut_F2), 
			false, MESSAGE_INFO, BUTTONS_YES_NO, true);
		int dres = dlg->run();
		delete dlg;
		if (dres == RESPONSE_YES) {
		    res = iElem;
		}
		else {
		    res = res->GetCommonOwner(aNode);
		}
	    } 
	    else {
		res = res->GetCommonOwner(aNode);
	    }
#endif
	    // Taking into account preferrend mutated node if pheno is enabled
	    if (ena_pheno) {
		res = res->GetCommonOwner(aMnode);
	    }
	    // Set mutated node as owner of changed node if mutation type is owner based and 
	    // the node selected for mutation is changed node itself
	    if (res == aNode && IsMutOwnerBased(aMutType)) {
		res = aMnode;
	    }
	}
    }
    mSignalAttention.emit(att);
    return res;
}

bool ElemDetRp::IsMutOwnerBased(TNodeType aMut) const 
{
    return aMut == ENt_Move || aMut == ENt_Rm || aMut == ENt_Change;
}

bool ElemDetRp::IsParentSafe(Elem* aTarg, const string& aParentUri)
{
    bool res = true;
    __ASSERT(!aParentUri.empty());
    // Checking if parent rank is correct
    Elem* parent = aTarg->GetNode(aParentUri);
    __ASSERT(parent != NULL);
    Rank targrank;
    Rank prntrank;
    aTarg->GetRank(prntrank, parent->Chromos().Root());
    aTarg->GetLRank(targrank);
    if (prntrank > targrank && !prntrank.IsRankOf(targrank)) {
	res = false;
    }
    return res;
}

void ElemDetRp::do_add_node(const std::string& aName, const std::string& aParentUri, const std::string& aTargetUri)
{
    bool err = false;
    // Mutate appending
    Elem* targ = iElem->GetNode(aTargetUri);
    bool unsafe = false;
    Elem* mutelem = GetObjForSafeMut(iElem, targ, ENt_Node, unsafe);
    if (mutelem != NULL) {
	ChromoNode mut = mutelem->Mutation().Root();
	ChromoNode rmut = mut.AddChild(ENt_Node);
	if (mutelem != targ) {
	    GUri nodeuri;
	    targ->GetUri(nodeuri, mutelem);
	    string snodeuri = nodeuri.GetUri(true);
	    rmut.SetAttr(ENa_MutNode, snodeuri);
	}
	__ASSERT(!aParentUri.empty());
	// Checking if parent rank is correct
	bool prntissafe = IsParentSafe(mutelem, aParentUri);
	if (!aParentUri.empty()) {
	    rmut.SetAttr(ENa_Parent, aParentUri);
	}
	if (!err) {
	    string sname(aName);
	    if (sname.empty()) {
		stringstream ss;
		ss << rand();
		sname = ss.str();
	    }
	    rmut.SetAttr(ENa_Id, sname);
	    // TODO [YB] To replace comps order with another mechanism
	    /*
	       if (!aNeighborUri.empty() && !aName.empty()) {
	    // Mutate moving
	    GUri duri;
	    iElem->GetUri(duri, mutelem);
	    GUri puri(aParentUri);
	    GUri suri = duri;
	    suri.AppendElem(puri.GetName(), GUri::KParentSep, aName);
	    GUri nuri = duri + GUri(aNeighborUri);
	    ChromoNode rmut = mutelem->Mutation().Root();
	    ChromoNode change = rmut.AddChild(ENt_Move);
	    change.SetAttr(ENa_Id, suri.GetUri());
	    change.SetAttr(ENa_MutNode, nuri.GetUri());
	    }
	    */
	    mutelem->Mutate();
	    /*
	       if (iReloadRequired) {
	       iReloadRequired = false;
	       mSigReloadRequired.emit();
	       };
	       */
	}
    }
}

void ElemDetRp::remove_node(const std::string& aNodeUri)
{
    MStSetting<bool>& ena_pheno_s = mStEnv.Settings().GetSetting(MStSettings::ESts_EnablePhenoModif, ena_pheno_s);
    bool ena_pheno = ena_pheno_s.Get(ena_pheno);
    // Node to be deleted
    Elem* dnode = iElem->GetNode(aNodeUri);
    __ASSERT(dnode != NULL);
    bool unsafe = false;
    Elem* mutelem = GetObjForSafeMut(iElem, dnode, ENt_Rm, unsafe);
#if 0
    // Checking mutation safety
    bool ismutsafe = iElem->IsMutSafe(dnode);
    Elem::TMDep mdep = dnode->GetMajorDep();
    Elem* mnode = mdep.first.first;
    if (mnode == NULL || ismutsafe) {
	mnode = iElem;
    }
    Elem* mutelem = mnode->GetAttachingMgr();
    int dres = RESPONSE_OK;
    if (!ismutsafe) {
	if (ena_pheno) {
	    MessageDialog* dlg = new MessageDialog(Glib::ustring::compose(KDlgMsg_Rm_F1, mnode->GetUri()), 
		    false, MESSAGE_INFO, BUTTONS_OK_CANCEL, true);
	    dres = dlg->run();
	    delete dlg;
	}
	    mSignalAttention.emit(K_Att_Rld);
	}
    }
#endif
    //if (dres == RESPONSE_OK) {
    if (true) {
	GUri nuri;
	dnode->GetUri(nuri, mutelem);
	ChromoNode mutn = mutelem->Mutation().Root().AddChild(ENt_Rm);
	mutn.SetAttr(ENa_MutNode, nuri.GetUri());
	mutelem->Mutate(false, !unsafe);
	Refresh();
    }
}

void ElemDetRp::change_content(const std::string& aNodeUri, const std::string& aNewContent, bool aRef )
{
    Elem* node = iElem->GetNode(aNodeUri);
    bool unsafe = false;
    Elem* mutelem = GetObjForSafeMut(iElem, node, ENt_Cont, unsafe);
    __ASSERT(mutelem != NULL);
    MStSetting<bool>& ena_pheno_s = mStEnv.Settings().GetSetting(MStSettings::ESts_EnablePhenoModif, ena_pheno_s);
    bool ena_pheno = ena_pheno_s.Get(ena_pheno);
    if (aRef) {
	Elem* rnode = node->GetNode(aNewContent);
	if (aRef && !mutelem->IsRefSafe(rnode)) {
	    Elem::TMDep rdep;
	    rnode->GetDep(rdep, ENa_Id);
	    Elem* depnode = rdep.first.first;
	    if (ena_pheno) {
		// Pheno enabled, select safe mutelem
		mutelem = depnode;
	    }
	    else {
		// Pheno disabled, try to shift comp to resolve dep
		if (iElem->IsComp(depnode) || iElem == depnode) {
		    ShiftCompToEnd(iElem, mutelem != iElem ? mutelem: node);
		}
		else {
		    mutelem = NULL;
		    mSignalAttention.emit(K_Att_RefToBiggestRank);
		}
	    }
	}
    }
    if (mutelem != NULL) {
	GUri nuri;
	node->GetUri(nuri, mutelem);
	ChromoNode change = mutelem->Mutation().Root().AddChild(ENt_Cont);
	change.SetAttr(ENa_MutNode, nuri.GetUri(true));
	change.SetAttr(aRef ? ENa_Ref : ENa_MutVal, aNewContent);
	mutelem->Mutate();
	Refresh();
    }
}

void ElemDetRp::move_node(const std::string& aNodeUri, const std::string& aDestUri)
{
    Elem* snode = iElem->GetNode(aNodeUri);
    Elem* dnode = iElem->GetNode(aDestUri);
    //if (snode != NULL && snode->GetMan() == iElem) {
    if (false) {
	// Src is comp = changing comps order
    }
    else {
	if (snode != NULL) {
	    Elem* cowner = iElem->GetCommonOwner(snode);
	    if (cowner != NULL) {
		ChromoNode rmut = cowner->Mutation().Root();
		ChromoNode change = rmut.AddChild(ENt_Move);
		change.SetAttr(ENa_Id, snode->GetUri(cowner));
		//	change.SetAttr(ENa_MutNode, iElem->GetUri(cowner));
		change.SetAttr(ENa_MutNode, dnode->GetUri(cowner));
		cowner->Mutate();
		Refresh();
	    }
	}
	else {
	    // Probably external node
	    // TODO [YB] To check explicitlly that it is external node but not wrong uri
	    ChromoNode rmut = iElem->Mutation().Root();
	    ChromoNode change = rmut.AddChild(ENt_Move);
	    change.SetAttr(ENa_Id, aNodeUri);
	    iElem->Mutate();
	    Refresh();
	}
    }
}

TBool ElemDetRp::DoIsActionSupported(Elem* aComp, const MCrp::Action& aAction)
{
    TBool res = ETrue;
    if (aAction == MCrp::EA_TransToMut) {
	res = aComp->IsPhenoModif();
    }
    return res;
}

void ElemDetRp::ShowCrpCtxDlg(GdkEventButton* event, Elem* aComp)
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
    if (DoIsActionSupported(aComp, MCrp::EA_TransToMut)) {
	menulist.push_back(iCompMenuElems.at(MCrp::EA_TransToMut));
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
	iCompSelected->GetRUri(duri, iElem);
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
    Elem* cnode = iElem->GetNode(uri);
    cnode->GetCont(sCont);
    TextEditDlg* dlg = new TextEditDlg("Edit content", sCont);
    int res = dlg->run();
    if (res == Gtk::RESPONSE_OK) {
	dlg->GetData(sCont);
	change_content(uri.GetUri(), sCont);
    }
    delete dlg;
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

// [YB] This is example of how to do "transformations" in chromo
// The approach is to avoid of editing chromo directly but instead to use
// combinatin of mutations and chromo squeezing
void ElemDetRp::on_comp_menu_trans_to_mut()
{
    // Add true mutation to model, using temp name
    // Temp name is required to avoid conflict with the current pheno modif
    ChromoNode mut = iElem->Mutation().Root();
    ChromoNode rmut = mut.AddChild(ENt_Node);
    Elem* parent = iCompSelected->GetParent();
    GUri puri;
    parent->GetUri(puri, iElem);
    rmut.SetAttr(ENa_Parent, puri.GetUri(true));
    rmut.SetAttr(ENa_Id, iCompSelected->Name() + "~tmp");
    iElem->Mutate();
    // Remove node originated by pheno modif
    Elem::TMDep dep;
    iCompSelected->GetDep(dep, ENa_Id, ETrue);
    Elem* depnode = dep.first.first;
    ChromoNode mutr = depnode->Mutation().Root();
    ChromoNode rmutr = mutr.AddChild(ENt_Rm);
    GUri nuri;
    iCompSelected->GetUri(nuri, depnode);
    rmutr.SetAttr(ENa_MutNode, nuri.GetUri(true));
    depnode->Mutate();
    // Squeeze chromo for mutations made to depnode: rm and rename
    ChromoNode lastmut = *(depnode->Chromos().Root().Rbegin());
    Elem* root = iElem->GetRoot();
    root->CompactChromo(lastmut);
    // Rename tmp name to correct
    ChromoNode rmutrn = mutr.AddChild(ENt_Change);
    rmutrn.SetAttr(ENa_MutNode, nuri.GetUri(true) + "~tmp");
    rmutrn.SetAttr(ENa_MutAttr, GUriBase::NodeAttrName(ENa_Id));
    rmutrn.SetAttr(ENa_MutVal, iCompSelected->Name());
    depnode->Mutate();
    // Squeeze chromo for mutations made to depnode: rm and rename
    lastmut = *(depnode->Chromos().Root().Rbegin());
    root->CompactChromo(lastmut);
}

// Shifting of component to the latest rank
// Just moving mutation, it doesn't affect model at all, but required model refresh
void ElemDetRp::ShiftCompToEnd(Elem* aOwner, Elem* aComp) 
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
	    iElem->Mutate();
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

ElemDrp::ElemDrp(Elem* aElem, const MCrpProvider& aCrpProv, MSEnv& aStEnv)
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

Elem* ElemDrp::Model()
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


#include <iostream>
#include <vert.h>
#include <edge.h>
#include <prop.h>
#include "common.h"
#include "vertdrp.h"
#include <gdkmm/types.h>


// Note that it is not possible just to use custom targets, like "text/edge-cp-uri" for purpose of resolving source. 
// The targets names are also used in DnD interprocess data exchange 
// Ref Selection. In case of custom targets the selection data transferring can face some trouble
static GtkTargetEntry targetentries[] =
{
    { (gchar*) KDnDTarg_EdgeCp, 0, KTei_EdgeCp },
};

// Widget of Vertex detailed representation
const string sVertDrpType = "VertDrp";

// Widget of Vertex detailed representation, version#1

VertDrpw_v1::ConnInfo::ConnInfo(): iCompOrder(0) { }

VertDrpw_v1::ConnInfo::ConnInfo(int aOrder): iCompOrder(aOrder) { }

VertDrpw_v1::ConnInfo::ConnInfo(const ConnInfo& aCInfo): iCompOrder(aCInfo.iCompOrder) { }


VertDrpw_v1::VertDrpw_v1(MElem* aElem, const MCrpProvider& aCrpProv, MSEnv& aStEnv): ElemDetRp(aElem, aCrpProv, aStEnv), iEdgeDropCandidate(NULL),
    iEdgeDropCpCandidate(NULL)
{
    // Set dest with avoiding DestDefaults flags. These flags are only for some trivial DnD 
    // scenarious, but we need to implement requesting edges data during drop motion
    //drag_dest_set(Gtk::ArrayHandle_TargetEntry(targetentries, 4, Glib::OWNERSHIP_NONE), Gtk::DestDefaults(0));
    // Add specific DnD targets
    Glib::RefPtr<TargetList> targets = drag_dest_get_target_list();
    targets->add(Gtk::ArrayHandle_TargetEntry(targetentries, 1, Glib::OWNERSHIP_NONE));
    // Enable receiving events by itself to handle events from non-windowed widgets (edges)
    add_events(Gdk::BUTTON_PRESS_MASK | Gdk::POINTER_MOTION_MASK);
}

VertDrpw_v1::~VertDrpw_v1()
{
}

string VertDrpw_v1::EType()
{
    return Vert::PEType();
}

void *VertDrpw_v1::DoGetObj(const string& aName)
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

Gtk::Widget& VertDrpw_v1::Widget()
{
    return *(static_cast<Gtk::Widget*>(this));
}

MElem* VertDrpw_v1::Model()
{
    return iElem;
}

MDrp::tSigCompSelected VertDrpw_v1::SignalCompSelected()
{
    return iSigCompSelected;
}

MDrp::tSigCompActivated VertDrpw_v1::SignalCompActivated()
{
    return iSigCompActivated;
}

bool VertDrpw_v1::IsTypeAllowed(const std::string& aType) const
{
    bool res = false;
    if (aType == Vert::PEType() || aType == Edge::PEType() || aType == Prop::PEType() || ElemDetRp::IsTypeAllowed(aType)) {
	res = true;
    }
    return res;
}

MElem* VertDrpw_v1::GetCompOwning(MElem* aElem)
{
    MElem* res = NULL;
    for (std::vector<MElem*>::iterator it = iElem->Comps().begin(); it != iElem->Comps().end() && res == NULL; it++) {
	MElem* comp = *it;
	if (comp->IsRemoved()) continue;
	if (aElem == comp || comp->IsComp(aElem)) {
	    res = comp;
	}
    }
    return res;
}

void VertDrpw_v1::on_comp_updated(MElem* aElem)
{
    std::cout << "on_comp_updated" << std::endl;
}

// Overloaded on-expose handler to prioritize highlighted childs (edges)
bool VertDrpw_v1::on_expose_event(GdkEventExpose* event)
{
    Glib::ListHandle<Gtk::Widget*> children = get_children();
    // Normal states first
    for (Glib::ListHandle<Gtk::Widget*>::iterator it = children.begin(); it != children.end(); it++) {
	Gtk::Widget* ww = (*it);
	if (ww->get_state() == Gtk::STATE_NORMAL) {
	    propagate_expose(*ww, event);
	}
    }
    // Then highlighted
    for (Glib::ListHandle<Gtk::Widget*>::iterator it = children.begin(); it != children.end(); it++) {
	Gtk::Widget* ww = (*it);
	if (ww->get_state() == Gtk::STATE_PRELIGHT) {
	    propagate_expose(*ww, event);
	}
    }
    // Then selected
    for (Glib::ListHandle<Gtk::Widget*>::iterator it = children.begin(); it != children.end(); it++) {
	Gtk::Widget* ww = (*it);
	if (ww->get_state() == Gtk::STATE_SELECTED) {
	    propagate_expose(*ww, event);
	}
    }
}

void VertDrpw_v1::on_size_allocate(Gtk::Allocation& aAllc)
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

    // Allocate components excluding edges
    // Keeping components order, so using elems register of comps
    int compb_x = aAllc.get_width()/2, compb_y = KViewCompGapHight;
    int comps_w_max = 0;
    for (std::vector<MElem*>::iterator it = iElem->Comps().begin(); it != iElem->Comps().end(); it++) {
	MElem* comp = *it;
	if (comp->IsRemoved()) continue;
	MCrp* crp = iCompRps.at(comp);
	MEdgeCrp* medgecrp = crp->GetObj(medgecrp);
	if (medgecrp == NULL) {
	    Gtk::Widget* comp = &(crp->Widget());
	    Gtk::Requisition req = comp->size_request();
	    int comp_body_center_x = req.width / 2;
	    comps_w_max = max(comps_w_max, req.width);
	    int hh = req.height;
	    Gtk::Allocation allc = Gtk::Allocation(compb_x - comp_body_center_x, compb_y, req.width, hh);
	    comp->size_allocate(allc);
	    compb_y += hh + KViewCompGapHight;
	}
    }
    // Allocate edges
    int edge_wd = 0;
}

void VertDrpw_v1::on_size_request(Gtk::Requisition* aReq)
{
    // Calculate size of regular comps and edges
    int comp_w = 0, comp_h = KViewCompGapHight;
    int edge_w = 0, edge_h = 0;
    for (std::map<MElem*, MCrp*>::iterator it = iCompRps.begin(); it != iCompRps.end(); it++) {
	MCrp* crp = it->second;
	MEdgeCrp* medgecrp = crp->GetObj(medgecrp);
	if (medgecrp == NULL) {
	    Gtk::Widget* cpw = &(crp->Widget());
	    Gtk::Requisition req = cpw->size_request();
	    comp_w = max(comp_w, req.width);
	    comp_h += req.height + KViewCompGapHight;
	}
	else {
	    Gtk::Widget* cpw = &(crp->Widget());
	    Gtk::Requisition req = cpw->size_request();
	    if (edge_w == 0) {
		edge_w = KConnHorizSpreadMin;
	    }
	    else {
		edge_w += KEdgeGridCell;
	    }
	    edge_h = max(edge_h, req.height);
	}
    }
    // Doubling edge_w - taking into account the left area
    aReq->width = comp_w + 2 * (edge_w + KDrpPadding); 
//    aReq->height = max(comp_h, edge_h);
    aReq->height = comp_h;
}

bool VertDrpw_v1::AreCpsCompatible(MElem* aCp1, MElem* aCp2)
{
    MVert* v1 = aCp1->GetObj(v1);
    MVert* v2 = aCp2->GetObj(v2);
    return (v1 != NULL && v2 != NULL);
}

bool VertDrpw_v1::on_drag_motion (const Glib::RefPtr<Gdk::DragContext>& context, int x, int y, guint time)
{
    bool res = false;
    /*
    // Checking if source target is Edge CP URI
    Gdk::ListHandle_AtomString src_targ = context->get_targets();
    std::vector<std::string> vv = src_targ.operator std::vector<std::string>();
    std::string targ = vv.at(0);
    if (iDnDTarg == EDT_Unknown) {
	// Detect target type
//	std::cout << "VertDrpw_v1 on_drag_motion: edge-cp-uri" << std::endl;
	drag_get_data(context, targ, time);
    }
    else {
    */
//    std::cout << "VertDrpw_v1 on_drag_motion, iDnDTarg: " << iDnDTarg  << std::endl;
    res = ElemDetRp::on_drag_motion(context, x, y, time);
    if (!res) {
	if (iDnDTarg == EDT_EdgeCp) {
	    // Stretch Edge to dragging CP
	    Gtk::Requisition coord = {x,y};
	    MElem* cp = iElem->GetNode(iDndReceivedData);
	    //std::cout << "VertDrpw_v1 on_drag_motion, edge CP:" << cp->Name() << std::endl;
	    MElem* edge = cp->GetMan();
	    MEdge* medge = edge->GetObj(medge);
	    assert(medge != NULL);
	    MCrp* crp = iCompRps.at(edge);
	    MEdgeCrp* medgecrp = crp->GetObj(medgecrp);
	    MVert* pair = NULL;
	    if (cp->Name() == "P1") {
		medgecrp->SetCp1Coord(coord);
		pair = medge->Point2();
	    }
	    else if (cp->Name() == "P2") {
		medgecrp->SetCp2Coord(coord);
		pair = medge->Point1();
	    }
	    MElem* epair = pair != NULL ? pair->GetObj(epair) : NULL;
	    MCompatChecker* mpaircc = pair != NULL ? pair->GetObj(mpaircc) : NULL;
	    // Find the nearest CP and highligh it
	    int dist = KDistThresholdEdge ;
	    MCrp* cand = NULL;
	    MElem* candcp = NULL;
	    for (tCrps::iterator it = iCompRps.begin(); it != iCompRps.end(); it++) {
		MCrp* crp = it->second;
		MCrpConnectable* conn = crp->GetObj(conn);
		if (conn != NULL) {
		    MElem* curcp = NULL;
		    int dd = conn->GetNearestCp(coord, curcp);
		    if (dd < dist) {
			dist = dd;
			cand = crp;
			candcp = curcp;
		    }
		}
	    }
	    if (cand != NULL && candcp != NULL && (cand != iEdgeDropCandidate || candcp != iEdgeDropCpCandidate)) {
		MCrpConnectable* conn = NULL;
		if (iEdgeDropCandidate != NULL) {
		    conn = iEdgeDropCandidate->GetObj(conn);
		    conn->HighlightCp(iEdgeDropCpCandidate, false);
		}
		//if (mpaircc == NULL || mpaircc->IsCompatible(candcp)) {
		if (pair == NULL || AreCpsCompatible(epair, candcp)) {
		    iEdgeDropCandidate = cand;
		    iEdgeDropCpCandidate = candcp;
		    conn = iEdgeDropCandidate->GetObj(conn);
		    conn->HighlightCp(iEdgeDropCpCandidate, true);
		}
		else {
		    iEdgeDropCandidate = NULL;
		    iEdgeDropCpCandidate = NULL;
		}
	    }
	    else if (cand == NULL && iEdgeDropCandidate != NULL) {
		MCrpConnectable* conn = iEdgeDropCandidate->GetObj(conn);
		conn->HighlightCp(iEdgeDropCpCandidate, false);
		iEdgeDropCandidate = NULL;
		iEdgeDropCpCandidate = NULL;
	    }
	    queue_resize();
	}
	//std::cout << "VertDrpw_v1 on_drag_motion: targ detected: " << iDnDTarg << ", x: " << x << ", y: " << y << std::endl;
	res = true;
	context->drag_status(Gdk::ACTION_COPY, time);
    }
    return res;
}

void VertDrpw_v1::on_drag_data_received(const Glib::RefPtr<Gdk::DragContext>& context, int x, int y, 
	const Gtk::SelectionData& sel_data, guint info, guint time)
{
    std::string target = sel_data.get_target();
    /*
    iDndReceivedData = sel_data.get_text();
    */
    //std::cout << "VertDrpw_v1 on_drag_data_received, target: " << target << ", info: " << info << " , text: " << iDndReceivedData << std::endl;
    ElemDetRp::on_drag_data_received(context, x, y, sel_data, info, time);
    if (iDnDTarg == EDT_Unknown) {
	// Classify DnD target
	if (target == KDnDTarg_EdgeCp) {
	    iDnDTarg = EDT_EdgeCp;
	}
	//std::cout << "VertDrpw_v1 on_drag_data_received, detecting target: " << iDnDTarg << std::endl;
    }
    /*
    else {
	// Target type detected
	context->drop_finish(true, time);
    }
    */
}

bool VertDrpw_v1::on_drag_drop(const Glib::RefPtr<Gdk::DragContext>& context, int x, int y, guint time)
{
    bool res = false;
    //std::cout << "VertDrpw_v1 on_drag_drop, detected target: " << iDnDTarg << std::endl;
    res = ElemDetRp::on_drag_drop(context, x, y, time);
    if (!res) {
    if (iDnDTarg == EDT_Node) {
	res = true;
	context->drag_finish(true, false, time);
	on_node_dropped(iDndReceivedData);
	queue_resize();
    }
    else if (iDnDTarg == EDT_EdgeCp) {
	if (iEdgeDropCandidate != NULL) {
	    MCrpConnectable* conn = iEdgeDropCandidate->GetObj(conn);
	    // Reset highlighting of drop candidate
	    conn->HighlightCp(iEdgeDropCpCandidate, false);
	    MElem* targ = iEdgeDropCpCandidate;
	    MElem* node = iElem->GetNode(iDndReceivedData);
	    GUri uri;
	    targ->GetRUri(uri, node);
	    res = true;
	    context->drag_finish(res, false, time);
	    //std::cout << "VertDrpw_v1, connectin edge [" << iDndReceivedData << "] to [" << uri.GetUri() << "]" << std::endl;
	    change_content(iDndReceivedData, uri.GetUri(true), true);
	}
	else {
	    // Disconnect edge if it is connected
	    res = true;
	    context->drag_finish(res, false, time);
	    MElem* cp = iElem->GetNode(iDndReceivedData);
	    MProp* prop = cp->GetObj(prop);
	    //std::cout << "VertDrpw_v1 on_drag_motion, disconnecting CP:" << prop->Value() << std::endl;
	    change_content(iDndReceivedData, std::string());
	    // Redraw
	    //queue_resize();
	}
	iEdgeDropCandidate = NULL;
    }
    iDnDTarg = EDT_Unknown;
    iDndReceivedData.clear();
    }
    return res;
}

bool VertDrpw_v1::on_button_press_event(GdkEventButton* aEvent)
{
    bool res = false;
    // Propagate event to edges, recalculating the event coord from originated window to the target
    int ox, oy;
    gdk_window_get_position(aEvent->window, &ox, &oy);
    int ex = aEvent->x, ey = aEvent->y;
    std::cout << "VertDrpw_v1 on_button_press_event, ewpos = (" << ox << " , " << oy << "), ecoord = (" 
	<< aEvent->x << " , " << aEvent->y << ")" << std::endl;
    // Prioritize selected edge to allow DnD for it
    for (tCrps::iterator it = iCompRps.begin(); it != iCompRps.end() && !res; it++) {
	MCrp* crp = it->second;
	MEdgeCrp* medgecrp = crp->GetObj(medgecrp);
	if (medgecrp != NULL) {
	    Gtk::Widget& wd = crp->Widget();
	    if (wd.get_state() == Gtk::STATE_SELECTED) {
		Gtk::Allocation talc = wd.get_allocation();
		aEvent->x = ex + ox - talc.get_x();
		aEvent->y = ey + oy - talc.get_y();
		res = wd.event((GdkEvent*) aEvent);
	    }
	}
    }
    if (!res) {
	for (tCrps::iterator it = iCompRps.begin(); it != iCompRps.end(); it++) {
	    MCrp* crp = it->second;
	    Gtk::Widget& wd = crp->Widget();
	    if (!wd.get_has_window()) {
		Gtk::Allocation talc = wd.get_allocation();
		MEdgeCrp* medgecrp = crp->GetObj(medgecrp);
		if (medgecrp != NULL) {
		    if (wd.get_state() != Gtk::STATE_SELECTED) {
			aEvent->x = ex + ox - talc.get_x();
			aEvent->y = ey + oy - talc.get_y();
			res = wd.event((GdkEvent*) aEvent);
		    }
		}
		// Also hangling other not-windowed widgets
		else {
		    aEvent->x = ex + ox - talc.get_x();
		    aEvent->y = ey + oy - talc.get_y();
		    res = wd.event((GdkEvent*) aEvent);
		}
	    }
	    // We need to immediatelly leave this method because it is possible that this widget
	    // gets deleted in case of redirected event is handled and resulted in context changing
	    if (res) break;
	}
    }
    return true;
}

bool VertDrpw_v1::on_motion_notify_event(GdkEventMotion* aEvent)
{
    bool res = false;
    // Propagate event to edges, recalculating the event coord from originated window to the target
    // Note that we need to use edges allocation to recalulation instead of edges window because edges
    // window is hidden (EventBox)
    // We also need to use only this DRP window event. This is because events from childs, e.g. CRPs can be
    // propagated to this handler, ref. S.Weiss Drawing in GTK+, sect. "More About Events in GTK+"
    //std::cout << "VertDrpw_v1 on_motion_notify_event, own_wnd: " << get_bin_window()->gobj() << " , event_wnd: " << aEvent->window << std::endl;
    if (get_bin_window()->gobj() == aEvent->window) {
	int ox, oy;
	gdk_window_get_position(aEvent->window, &ox, &oy);
	int ex = aEvent->x, ey = aEvent->y;
	//std::cout << "VertDrpw_v1 on_motion_notify_event, ewpos = (" << ox << " , " << oy << "), ecoord = (" 
	//	<< aEvent->x << " , " << aEvent->y << ")" << std::endl;
	for (tCrps::iterator it = iCompRps.begin(); it != iCompRps.end(); it++) {
	    MCrp* crp = it->second;
	    MEdgeCrp* medgecrp = crp->GetObj(medgecrp);
	    if (medgecrp != NULL) {
		Gtk::Widget& wd = crp->Widget();
		Gtk::Allocation talc = wd.get_allocation();
		//std::cout << "edge [" << wd.get_name() << "], orig = [" << talc.get_x() << " , " << talc.get_y() << ")" << std::endl;
		aEvent->x = ex + ox - talc.get_x();
		aEvent->y = ey + oy - talc.get_y();
		res = wd.event((GdkEvent*) aEvent);
	    }
	}
    }
    return true;
}

bool VertDrpw_v1::on_leave_notify_event(GdkEventCrossing* aEvent)
{
    bool res = false;
    std::cout << "VertDrpw_v1 on_leave_notify_event" << std::endl;
    // Propagate event to edges
    for (tCrps::iterator it = iCompRps.begin(); it != iCompRps.end(); it++) {
	MCrp* crp = it->second;
	MEdgeCrp* medgecrp = crp->GetObj(medgecrp);
	if (medgecrp != NULL) {
	    res = crp->Widget().event((GdkEvent*) aEvent);
	}
    }
    return true;

}

void VertDrpw_v1::Udno()
{
}

MDrp::tSigDragMotion VertDrpw_v1::SignalDragMotion()
{
    return iSigDragMotion;
}

void VertDrpw_v1::OnActionInsert()
{
    ElemDetRp::DoOnActionInsert();
}


const string& VertDrp::Type()
{
    return sVertDrpType;
}


string VertDrp::EType()
{
    return ":Elem:Vert";
}

VertDrp::VertDrp(MElem* aElem, const MCrpProvider& aCrpProv, MSEnv& aStEnv)
{
    iRp = new VertDrpw_v1(aElem, aCrpProv, aStEnv);
}

VertDrp::~VertDrp()
{
    delete iRp;
}

void *VertDrp::DoGetObj(const string& aName)
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

Gtk::Widget& VertDrp::Widget()
{
    return *iRp;
}

MDrp::tSigCompSelected VertDrp::SignalCompSelected()
{
    return iRp->iSigCompSelected;
}

MElem* VertDrp::Model()
{
    return iRp->iElem;
}

MDrp::tSigDragMotion VertDrp::SignalDragMotion()
{
    return iRp->iSigDragMotion;
}

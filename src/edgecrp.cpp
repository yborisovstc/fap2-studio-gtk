
#include <iostream>
#include <limits>
#include "edgecrp.h"
#include <edge.h>
#include <mvert.h>
#include "common.h"
#include <gdkmm/event.h>

const int KEdgeDragThreshold = 20;

static GtkTargetEntry targetentries[] =
{
//    { (gchar*) "STRING",        0, KTei_EdgeCp },
    { (gchar*) KDnDTarg_EdgeCp, 0, KTei_EdgeCp },
};

using namespace Gdk;

// Edge compact representation iface, ver 1

const string sMEdgeCrp_Type = "MEdgeCrp";

const string& MEdgeCrp::Type()
{
    return sMEdgeCrp_Type;
}

// Edge compact representation, version 1
const string sType = "EdgeCrp";

const string& EdgeCrp::Type()
{
    return sType;
}

string EdgeCrp::EType()
{
    return Edge::PEType();
}

EdgeCrp::EdgeCrp(MElem* aElem): iElem(aElem), iDraggedPart(EDp_None), iHoweredTerm(EDp_None), iDragging(false), iHighlighted(false), 
    iDnDSupp(EDT_None)
{
    // set no_window mode
    set_has_window(false);
    bool isatt = iElem->IsChromoAttached();
    // Set name
    set_name(isatt ? "Edge" : "Edge~da");
    //set_name("Edge~" + iElem->Name());
    // Set events mask
    add_events(Gdk::BUTTON_PRESS_MASK | Gdk::POINTER_MOTION_MASK);
}


EdgeCrp::~EdgeCrp()
{
}

Gtk::Widget& EdgeCrp::Widget()
{
    return *this;
}

void *EdgeCrp::DoGetObj(const string& aName)
{
    void* res = NULL;
    if (aName ==  MEdgeCrp::Type()) {
	res = (MEdgeCrp*) this;
    }
    return res;
}

MCrp::tSigButtonPressName EdgeCrp::SignalButtonPressName()
{
    return iSigButtonPressName;
}

MCrp::tSigButtonPress EdgeCrp::SignalButtonPress()
{
    return iSigButtonPress;
}

MCrp::tSigUpdated EdgeCrp::SignalUpdated()
{
    return iSigUpdated;
}

bool EdgeCrp::IsActionSupported(Action aAction)
{
    return (aAction == EA_Remove || aAction == EA_Rename);
}

void EdgeCrp::GetContentUri(GUri& aUri)
{
}

bool EdgeCrp::Dragging()
{
    return iDragging;
}

void EdgeCrp::SetHighlighted(bool aSet)
{
    if (aSet != iHighlighted) {
	iHighlighted = aSet;
	if (iHighlighted) {
	    set_state(Gtk::STATE_PRELIGHT);
	}
	else {
	    set_state(Gtk::STATE_NORMAL);
	}
    }
}

void EdgeCrp::SetErroneous(bool aSet)
{
    if (aSet != iErr) {
	iErr = aSet;
	if (iErr) {
	}
	else {
	}
    }
}

MElem* EdgeCrp::Model()
{
    return iElem;
}

void EdgeCrp::SetLArea(int aArea)
{
}

int EdgeCrp::GetLArea() const
{
    return MCrp::EOverlay;
}

void EdgeCrp::SetCp1Coord(const Gtk::Requisition& aReq)
{
    iCp1 = aReq;
}

void EdgeCrp::SetCp2Coord(const Gtk::Requisition& aReq)
{
    iCp2 = aReq;
}

const Gtk::Requisition& EdgeCrp::Cp1Coord()
{
    return iCp1;
}
const Gtk::Requisition& EdgeCrp::Cp2Coord()
{
    return iCp2;
}

MElem* EdgeCrp::Point1()
{
    MElem* res = NULL;
    MEdge* medge = iElem->GetObj(medge);
    MVert* p1 = medge->Point1();
    if (p1 != NULL) {
	res = p1->GetObj(res);
    }
    return res;
}

MElem* EdgeCrp::Point2()
{
    MElem* res = NULL;
    MEdge* medge = iElem->GetObj(medge);
    MVert* p1 = medge->Point2();
    if (p1 != NULL) {
	res = p1->GetObj(res);
    }
    return res;
}

MEdgeCrp::TVectEn& EdgeCrp::VectEn()
{
    return iNodes;
}

bool EdgeCrp::IsPointIn(int aX, int aY) const
{
    // Get region of drawable part of edge
    Gdk::Region* r = (Gdk::Region*) &iRegion;
    return r->point_in(aX, aY);
}

template <EdgeCrp::TTpoint pt> bool EdgeCrp::IsCpLeft() const
{
    __ASSERT(pt != EDp_None);
    Requisition beg = *(iNodes.begin());
    Requisition beg_next = *(++iNodes.begin());
    Requisition end = *(iNodes.rbegin());
    Requisition end_prev = *(++iNodes.rbegin());
    Requisition cp = (pt == EDp_Cp1) ? iCp1 : iCp2;
    bool res = (cp.width == beg.width && cp.height == beg.height) ||
	(cp.width == end.width && cp.height == end.height && end.width < end_prev.width); 
    return res;
}

EdgeCrp::TTpoint EdgeCrp::HoweredTerm(int aX, int aY) const
{
    TTpoint res = EDp_None;
    Gtk::Allocation alc = get_allocation();
    int cp1x = iCp1.width - alc.get_x();
    int cp1y = iCp1.height - alc.get_y();
    int cp2x = iCp2.width - alc.get_x();
    int cp2y = iCp2.height - alc.get_y();
    bool cp1l = IsCpLeft<EDp_Cp1>();
    if (cp1l && ((aX - cp1x) > 0 && (aX - cp1x) < KEdgeDragThreshold && abs(aY - cp1y) < KEdgeDragThreshold) ||
	    !cp1l && ((cp1x - aX) > 0 && (cp1x - aX) < KEdgeDragThreshold && abs(aY - cp1y) < KEdgeDragThreshold)) {
	    res = EDp_Cp1;
    }
    bool cp2l = IsCpLeft<EDp_Cp2>();
    if (cp2l && ((aX - cp2x) > 0 && (aX - cp2x) < KEdgeDragThreshold && abs(aY - cp2y) < KEdgeDragThreshold) ||
	    !cp2l && ((cp2x - aX) > 0 && (cp2x - aX) < KEdgeDragThreshold && abs(aY - cp2y) < KEdgeDragThreshold)) {
	    res = EDp_Cp2;
    }
    return res;
}

bool EdgeCrp::on_expose_event(GdkEventExpose* aEvent)
{
    Glib::RefPtr<Gdk::Window> drw = get_window();
    Glib::RefPtr<Gtk::Style> style = get_style(); 	
    Glib::RefPtr<Gdk::GC> gc = style->get_fg_gc(get_state());
    Gtk::Allocation alc = get_allocation();
    LineStyle ls = LINE_SOLID;
    if (iHoweredTerm != EDp_None) {
	ls = LINE_ON_OFF_DASH;
    }
    gc->set_line_attributes(1, ls, CAP_NOT_LAST, JOIN_MITER);

    iRegion = Gdk::Region();
    Requisition beg;
    Requisition end;
    if (iNodes.size() >= 2) {
	TVectEn::iterator it = iNodes.begin();
	end = *it;
	it++;
	for (; it != iNodes.end(); it++) {
	    beg = end;
	    end = *it;
	    drw->draw_line(gc, beg.width, beg.height, end.width, end.height);
	    Requisition tl = (beg.width <= end.width && beg.height <= end.height ) ? beg : end;
	    Requisition br = (beg.width <= end.width && beg.height <= end.height ) ? end : beg;
	    // Note: we are using coord relative to the wnd, because region will be used for event
	    // which coord are relative to wnd
	    int rx = tl.width - alc.get_x() - KEdgeBorderWidth;
	    int ry = tl.height - alc.get_y() - KEdgeBorderWidth;
	    int rw = br.width - tl.width + 2*KEdgeBorderWidth;
	    int rh = br.height - tl.height + 2*KEdgeBorderWidth;
	    iRegion.union_with_rect(Gdk::Rectangle(rx, ry, rw, rh));
	}
    }
    gc->set_line_attributes(1, LINE_SOLID, CAP_NOT_LAST, JOIN_MITER);
    /*
    if (iHoweredTerm != EDp_None) {
	int w = KEdgeDragThreshold;
	Requisition point = iCp1;
	if (iHoweredTerm == EDp_Cp2) {
	    point = iCp2;
	}
	Requisition lp = *(iNodes.begin());
	bool left = (point.width == lp.width && point.height == lp.height); 
	if (left) {
	    drw->draw_rectangle(gc, true, point.width - w, point.height - w, w*2, w*2);
	} else {
	    drw->draw_rectangle(gc, true, point.width - w, point.height - w, w*2, w*2);
	}
    }
    */
}

void EdgeCrp::on_size_request(Gtk::Requisition* aReq)
{
    int left = std::numeric_limits<int>::max(), top = left, right = 0, bottom = 0;
    if (iNodes.size() >= 2) {
	for (TVectEn::iterator it = iNodes.begin(); it != iNodes.end(); it++) {
	    Requisition rr = *it;
	    int x = rr.width;
	    int y = rr.height;
	    left = min(left, x);
	    right = max(right, x);
	    top = min(top, y);
	    bottom = max(bottom, y);
	}
	aReq->width = right - left; 
	aReq->height = bottom - top;
    }
    else {
	aReq->width = KEdgeGridCell + 2*KEdgeBorderWidth; 
	aReq->height = KViewCompGapHight;
    }
}

bool EdgeCrp::on_motion_notify_event(GdkEventMotion* aEvent)
{
    bool res = false;
    // std::cout << "EdgeCrp on_motion_notify_event [" << get_name() << "], coord (" << aEvent->x << "," << aEvent->y << "), ewnd " 
    //	<< aEvent->window << ", wnd " << get_window()->gobj() << std::endl;
    Gtk::Allocation alc = get_allocation();
     //std::cout << "alc = [ " << alc.get_x() << "," << alc.get_y() << "," << alc.get_width() << "," << alc.get_height() << "]" << std::endl;
    // std::cout << "cp1 = [ " << iCp1.iCoord.width << "," << iCp1.iCoord.height << "]" << std::endl;
    // Set highlighted state if point is in
    // Check if cursor is howered on terminal points and set howered term points attribute
    TTpoint iht = HoweredTerm(aEvent->x, aEvent->y);
    if (iht != iHoweredTerm) {
	iHoweredTerm = iht;
	queue_draw();
    }
    //std::cout << "EdgeCrp on_motion_notify_event, iHoweredTerm: " << iht << ", cp1left: " << IsCp1Left() << std::endl;
    bool isin = IsPointIn(aEvent->x, aEvent->y);
    if (isin) {
	//std::cout << "EdgeCrp on_motion_notify_event, IsPointIn" << std::endl;
	if (get_state() == Gtk::STATE_NORMAL) {
	    //std::cout << "EdgeCrp on_motion_notify_event [" << get_name() << "] : set prelight" << std::endl;
	    set_state(Gtk::STATE_PRELIGHT);
	}
	//res = true;
    }
    else { 
	if (get_state() == Gtk::STATE_PRELIGHT) {
	    // std::cout << "EdgeCrp on_motion_notify_event  [" << get_name() << "] : unset plelight" << std::endl;
	    set_state(Gtk::STATE_NORMAL);
	}
    }
    return res;
}

bool EdgeCrp::on_leave_notify_event(GdkEventCrossing* event)
{
    if (get_state() == Gtk::STATE_PRELIGHT) {
	set_state(Gtk::STATE_NORMAL);
    }
    return false;
}

bool EdgeCrp::on_button_press_event(GdkEventButton* aEvent)
{
    bool res = false;
    if (aEvent->type == GDK_BUTTON_PRESS) {
	std::cout << "EdgeCrp on_button_press_event [" << get_name() << "]"  << std::endl;
	Gtk::Allocation alc = get_allocation();
	int ex = aEvent->x;
	int ey = aEvent->y;
	int cp1x = iCp1.width - alc.get_x();
	int cp1y = iCp1.height - alc.get_y();
	int cp2x = iCp2.width - alc.get_x();
	int cp2y = iCp2.height - alc.get_y();
	if (abs(ex - cp1x) < KEdgeDragThreshold && abs(ey - cp1y) < KEdgeDragThreshold) {
	    iDraggedPart = EDp_Cp1;
	    iDragging = true;
	}
	else if (abs(ex - cp2x) < KEdgeDragThreshold && abs(ey - cp2y) < KEdgeDragThreshold) {
	    iDraggedPart = EDp_Cp2;
	    iDragging = true;
	}
	if (iDragging) {
	    // Needs to use this array handle constructor with length argument, otherwise not working
	    Glib::RefPtr<Gtk::TargetList> targ = Gtk::TargetList::create(Gtk::ArrayHandle_TargetEntry(targetentries, 1, Glib::OWNERSHIP_NONE));
	    GdkEvent* evt = (GdkEvent*) aEvent;
	    std::cout << "EdgeCrp begin dragging from " << iDraggedPart << std::endl;
	    //drag_source_set(Gtk::ArrayHandle_TargetEntry(targetentries, 1, Glib::OWNERSHIP_NONE));
	    // TODO [YB] Only ACTION_COPY works here, to analyse why
	    drag_begin(targ, Gdk::ACTION_COPY, aEvent->button, evt);
	    res = true;
	}
	else {
	    //std::cout << "EdgeCrp on_button_press_event [" << get_name() << "], coord (" << aEvent->x << "," << aEvent->y << "), ewnd " 
		//<< aEvent->window << ", wnd " << get_window()->gobj() << std::endl;
	    bool isin = IsPointIn(aEvent->x, aEvent->y);
	    if (isin) {
		std::cout << "EdgeCrp iSigButtonPress emit "  << std::endl;
		iSigButtonPress.emit(aEvent);
	    }
	    if (aEvent->button == 1) {
		// Select edge
		// Don't stop event processing in ordet to allow other edges to be selected
		//std::cout << "EdgeCompRp_v3 on_button_press_event [" << get_name() << "], set selected" << std::endl; 
		if (isin) {
		    if (get_state() != Gtk::STATE_SELECTED) {
			set_state(Gtk::STATE_SELECTED);
		    }
		    else {
			set_state(Gtk::STATE_NORMAL);
		    }
		    res = true;
		}
	    }
	}
    }
    return res;
}

void EdgeCrp::on_drag_data_get(const Glib::RefPtr<Gdk::DragContext>&, Gtk::SelectionData& data, guint info, guint time)
{
    //std::cout << "EdgeCrp on_drag_data_get, info: " << info << std::endl;
    if (info == KTei_EdgeCp && iDragging && iDraggedPart != EDp_None) {
	GUri uri;
	MElem* pte = NULL;
	if (iDraggedPart == EDp_Cp1) {
	    pte = iElem->GetNode("./P1");
	}
	else {
	    pte = iElem->GetNode("./P2");
	}
	//pte->GetUri(uri, iElem->GetMan());
	//std::string suri = uri.GetUri();
	std::string suri = iElem->GetUri();
	suri += (iDraggedPart == EDp_Cp1) ? ".P1" : ".P2";
	data.set(KDnDTarg_EdgeCp, suri);
	//std::cout << "EdgeCrp on_drag_data_get, data set: [" << suri << "]" << std::endl;
    }
}

void EdgeCrp::on_drag_begin(const Glib::RefPtr<Gdk::DragContext>& aContext)
{
    if (iDraggedPart != EDp_None) {
	iDragging = true;
	//std::cout << "Dragging begin, CP: " << iDraggedPart << std::endl;
    }
}

void EdgeCrp::on_drag_end(const Glib::RefPtr<Gdk::DragContext>& context)
{
    //std::cout << "EdgeCompRp_v3: on_drag_end "  << std::endl;
    iDraggedPart = EDp_None;
    iDragging = false;
}

void EdgeCrp::SetDnDTargSupported(int aTarg)
{
    iDnDSupp = aTarg;
}

bool EdgeCrp::IsDnDTargSupported(TDnDTarg aTarg) const
{
    return aTarg & iDnDSupp;
}

bool EdgeCrp::IsIntersected(int aX, int aY) const
{
    return IsPointIn(aX, aY);
}


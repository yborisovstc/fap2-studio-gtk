
#include <iostream>
#include "edgecrp.h"
#include <edge.h>
#include <mvert.h>
#include "common.h"
#include <gdkmm/event.h>

const int KEdgeDragThreshold = 8;

static GtkTargetEntry targetentries[] =
{
//    { (gchar*) "STRING",        0, KTei_EdgeCp },
    { (gchar*) KDnDTarg_EdgeCp, 0, KTei_EdgeCp },
};

const string sMEdgeCrpType = "MEdgeCrp";

const string& MEdgeCrp::Type()
{
    return sMEdgeCrpType;
}

// Edge Crp widget, version 3
EdgeCompRp_v3::EdgeCompRp_v3(Elem* aElem): iElem(aElem), iType(MEdgeCrp::EtLeft), iDraggedPart(EDp_None), 
    iDragging(false), iHighlighted(false)
{
    // set no_window mode
    set_has_window(false);
    // Set name
    set_name("Edge~" + iElem->Name());
    // Set events mask
    add_events(Gdk::BUTTON_PRESS_MASK | Gdk::POINTER_MOTION_MASK);
    // Setup DnD source
//    drag_source_set(Gtk::ArrayHandle_TargetEntry(targetentries, 1, Glib::OWNERSHIP_NONE));
//    drag_highlight();
//    signal_drag_data_get().connect(sigc::mem_fun(*this, &EdgeCompRp_v2::on_drag_data_get));
}

EdgeCompRp_v3::~EdgeCompRp_v3()
{
}

EdgeCompRp_v3::Cp::Cp(): iPos(0)
{
    iOffset.width = 0;
    iOffset.height = 0;
    iCoord.width = 0;
    iCoord.height = 0;
}

void EdgeCompRp_v3::on_drag_data_get(const Glib::RefPtr<Gdk::DragContext>&, Gtk::SelectionData& data, guint info, guint time)
{
    std::cout << "EdgeCompRp_v3 on_drag_data_get, info: " << info << std::endl;
    if (info == KTei_EdgeCp && iDragging && iDraggedPart != EDp_None) {
	GUri uri;
	Elem* pte = NULL;
	if (iDraggedPart == EDp_Cp1) {
	    pte = iElem->GetNode("Prop:P1");
	}
	else {
	    pte = iElem->GetNode("Prop:P2");
	}
	pte->GetUri(uri, iElem->GetMan());
	std::string suri = uri.GetUri();
	//data.set_text(suri);
	data.set(KDnDTarg_EdgeCp, suri);
	std::cout << "EdgeCompRp_v3 on_drag_data_get, data set: [" << suri << "]" << std::endl;
    }
}

void EdgeCompRp_v3::on_drag_begin(const Glib::RefPtr<Gdk::DragContext>& aContext)
{
    if (iDraggedPart != EDp_None) {
	iDragging = true;
	std::cout << "Dragging begin, CP: " << iDraggedPart << std::endl;
    }
}

void EdgeCompRp_v3::on_drag_end(const Glib::RefPtr<Gdk::DragContext>& context)
{
    std::cout << "EdgeCompRp_v3: on_drag_end "  << std::endl;
    iDraggedPart = EDp_None;
    iDragging = false;
}

bool EdgeCompRp_v3::on_button_press_event(GdkEventButton* aEvent)
{
    bool res = false;
    if (aEvent->type == GDK_BUTTON_PRESS) {
	std::cout << "EdgeCompRp_v3 on_button_press_event [" << get_name() << "]"  << std::endl;
	//if (iDraggedPart == EDp_None) {
	if (true) {
	    Gtk::Allocation alc = get_allocation();
	    int ex = aEvent->x;
	    int ey = aEvent->y;
	    int cp1x = iCp1.iCoord.width - alc.get_x();
	    int cp1y = iCp1.iCoord.height - alc.get_y();
	    int cp2x = iCp2.iCoord.width - alc.get_x();
	    int cp2y = iCp2.iCoord.height - alc.get_y();
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
		std::cout << "EdgeCompRp_v3 begin dragging from " << iDraggedPart << std::endl;
		//drag_source_set(Gtk::ArrayHandle_TargetEntry(targetentries, 1, Glib::OWNERSHIP_NONE));
		// TODO [YB] Only ACTION_COPY works here, to analyse why
		drag_begin(targ, Gdk::ACTION_COPY, aEvent->button, evt);
		res = true;
	    }
	    else {
		bool isin = IsPointIn(aEvent->x, aEvent->y);
		if (isin) {
		    std::cout << "EdgeCompRp_v3 iSigButtonPress emit "  << std::endl;
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
    }
    //res = Widget::on_button_press_event(aEvent);
    return res;
    }

    bool EdgeCompRp_v3::on_motion_notify_event(GdkEventMotion* aEvent)
    {
	bool res = false;
   // std::cout << "EdgeCompRp_v3 on_motion_notify_event [" << get_name() << "], coord (" << aEvent->x << "," << aEvent->y << "), ewnd " 
//	<< aEvent->window << ", wnd " << get_window()->gobj() << std::endl;
    Gtk::Allocation alc = get_allocation();
   // std::cout << "alc = [ " << alc.get_x() << "," << alc.get_y() << "," << alc.get_width() << "," << alc.get_height() << "]" << std::endl;
   // std::cout << "cp1 = [ " << iCp1.iCoord.width << "," << iCp1.iCoord.height << "]" << std::endl;
   // std::cout << "rect = [ " << iRect1.get_x() << "," << iRect1.get_y() << "," << iRect1.get_width() << "," << iRect1.get_height() << "]" << std::endl;
    // Set highlighted state if point is in
    bool isin = IsPointIn(aEvent->x, aEvent->y);
    if (isin) {
	if (get_state() == Gtk::STATE_NORMAL) {
	    //std::cout << "EdgeCompRp_v3 on_motion_notify_event [" << get_name() << "] : set prelight" << std::endl;
	    set_state(Gtk::STATE_PRELIGHT);
	    //queue_draw();
	}
	//res = true;
    }
    else { 
	if (get_state() == Gtk::STATE_PRELIGHT) {
	   // std::cout << "EdgeCompRp_v3 on_motion_notify_event  [" << get_name() << "] : unset plelight" << std::endl;
	    set_state(Gtk::STATE_NORMAL);
	    //queue_draw();
	}
    }
    return res;
}

bool EdgeCompRp_v3::on_leave_notify_event(GdkEventCrossing* event)
{
    if (get_state() == Gtk::STATE_PRELIGHT) {
	std::cout << "EdgeCompRp_v3 on_leave_notify_event  [" << get_name() << "] : unset plelight" << std::endl;
	set_state(Gtk::STATE_NORMAL);
    }
    return false;
}

bool EdgeCompRp_v3::IsPointIn(int aX, int aY)
{
    // Get region of drawable part of edge
    return iRegion.point_in(aX, aY);
}

bool EdgeCompRp_v3::on_expose_event(GdkEventExpose* aEvent)
{
    Glib::RefPtr<Gdk::Window> drw = get_window();
    Glib::RefPtr<Gtk::Style> style = get_style(); 	
    Glib::RefPtr<Gdk::GC> gc = style->get_fg_gc(get_state());
    Gtk::Allocation alc = get_allocation();
    iRegion = Gdk::Region();
    if (iType == MEdgeCrp::EtLeft) {
	Cp& ucp = (iCp1.iCoord.height <= iCp2.iCoord.height) ? iCp1 : iCp2;
	Cp& lcp = (iCp1.iCoord.height > iCp2.iCoord.height) ? iCp1 : iCp2;
	int rx = alc.get_x() + alc.get_width() - KEdgeBorderWidth;
	drw->draw_line(gc, iCp1.iCoord.width, iCp1.iCoord.height, rx, iCp1.iCoord.height);
	drw->draw_line(gc, rx, iCp1.iCoord.height, rx, iCp2.iCoord.height);
	drw->draw_line(gc, iCp2.iCoord.width, iCp2.iCoord.height, rx, iCp2.iCoord.height);
	iRegion.union_with_rect(Gdk::Rectangle(ucp.iCoord.width - alc.get_x() - KEdgeBorderWidth, 
		    ucp.iCoord.height - alc.get_y() - KEdgeBorderWidth, alc.get_width(), 2*KEdgeBorderWidth));
	iRegion.union_with_rect(Gdk::Rectangle(lcp.iCoord.width - alc.get_x() - KEdgeBorderWidth, 
		    lcp.iCoord.height - alc.get_y() - KEdgeBorderWidth, alc.get_width(), 2*KEdgeBorderWidth));
	iRegion.union_with_rect(Gdk::Rectangle(rx - alc.get_x() - KEdgeBorderWidth, 
		    ucp.iCoord.height - alc.get_y() - KEdgeBorderWidth, 2*KEdgeBorderWidth, alc.get_height()));
    }
    else if (iType == MEdgeCrp::EtRight) {
	int ux = alc.get_x() + alc.get_width() - 1 + iUcp.iOffset.width - KEdgeBorderWidth;
	int uy = alc.get_y() + KEdgeBorderWidth;
	int lx = alc.get_x() + alc.get_width() - 1 + iLcp.iOffset.width - KEdgeBorderWidth;
	int ly = alc.get_y() + alc.get_height() - 1 - KEdgeBorderWidth;
	drw->draw_line(gc, alc.get_x(), uy, ux, uy);
	drw->draw_line(gc, alc.get_x(), uy, alc.get_x(), ly);
	drw->draw_line(gc, alc.get_x(), ly, lx, ly);
	iRegion.union_with_rect(Gdk::Rectangle(0, 0, ux + KEdgeBorderWidth, 2*KEdgeBorderWidth));
	iRegion.union_with_rect(Gdk::Rectangle(0, 0, 2*KEdgeBorderWidth, alc.get_height()));
	iRegion.union_with_rect(Gdk::Rectangle(0, alc.get_height() - 2*KEdgeBorderWidth, alc.get_width(), 2*KEdgeBorderWidth));
    }
    else if (iType == MEdgeCrp::EtLtRb) {
	// Left top to right bottom
	int ux = alc.get_x() + KEdgeBorderWidth;
	int urx = alc.get_x() + iUcp.iOffset.width; // Upper right
	int uy = alc.get_y() + KEdgeBorderWidth;
	int lx = alc.get_x() + alc.get_width() - 1 - KEdgeBorderWidth;
	int ly = alc.get_y() + alc.get_height() - 1 - KEdgeBorderWidth;
	drw->draw_line(gc, urx, uy, urx, ly);
	drw->draw_line(gc, ux, uy, urx, uy);
	drw->draw_line(gc, urx, ly , lx, ly);
	iRegion.union_with_rect(Gdk::Rectangle(iUcp.iOffset.width - KEdgeBorderWidth, 0, 2*KEdgeBorderWidth, ly - uy + 2*KEdgeBorderWidth));
	iRegion.union_with_rect(Gdk::Rectangle(0, 0, urx - ux + 2*KEdgeBorderWidth, 2*KEdgeBorderWidth));
	iRegion.union_with_rect(Gdk::Rectangle(iUcp.iOffset.width - KEdgeBorderWidth, alc.get_height() - 2*KEdgeBorderWidth, 
		    lx - urx + KEdgeBorderWidth, 2*KEdgeBorderWidth));
    }
    else {
	// Left bottom to right top
	int ux = alc.get_x() + alc.get_width() - 1 - KEdgeBorderWidth;
	int urx = alc.get_x() + iUcp.iOffset.width;
	int uy = alc.get_y() + KEdgeBorderWidth;
	int lx = alc.get_x() + KEdgeBorderWidth;
	int ly = alc.get_y() + alc.get_height() - 1 - KEdgeBorderWidth;
	drw->draw_line(gc, urx, uy, urx, ly);
	drw->draw_line(gc, urx, uy, ux, uy);
	drw->draw_line(gc, lx, ly , urx, ly);
	iRegion.union_with_rect(Gdk::Rectangle(iUcp.iOffset.width - KEdgeBorderWidth, 0, 2*KEdgeBorderWidth, ly - uy + 2*KEdgeBorderWidth));
	iRegion.union_with_rect(Gdk::Rectangle(iUcp.iOffset.width - KEdgeBorderWidth, 0, ux - urx + 2*KEdgeBorderWidth, 2*KEdgeBorderWidth));
	iRegion.union_with_rect(Gdk::Rectangle(0, alc.get_height() - 2*KEdgeBorderWidth, iUcp.iOffset.width + KEdgeBorderWidth, 2*KEdgeBorderWidth));
    }
}

void EdgeCompRp_v3::on_size_request(Gtk::Requisition* aReq)
{
   // aReq->width = KConnHorizSpreadMin; 
    aReq->width = KEdgeGridCell + 2*KEdgeBorderWidth; 
    aReq->height = KViewCompGapHight;
}

void EdgeCompRp_v3::DoSetHighlighted(bool aSet)
{
    if (aSet != iHighlighted) {
	iHighlighted = aSet;
	if (iHighlighted) {
	    std::cout << "Elem [" << iElem->Name() << "]: higligted" << std::endl;
	    set_state(Gtk::STATE_PRELIGHT);
	}
	else {
	    set_state(Gtk::STATE_NORMAL);
	}
    }
}




// Edge compact representation
//
const string sType = "EdgeCrp";

const string& EdgeCrp::Type()
{
    return sType;
}

string EdgeCrp::EType()
{
    return Edge::PEType();
}

EdgeCrp::EdgeCrp(Elem* aElem)
{
    iRp = new EdgeCompRp_v3(aElem);
}

EdgeCrp::~EdgeCrp()
{
    delete iRp;
}

void *EdgeCrp::DoGetObj(const string& aName)
{
    void* res = NULL;
    if (aName ==  Type()) {
	res = this;
    }
    else if (aName ==  MEdgeCrp::Type()) {
	res = (MEdgeCrp*) this;
    }
    return res;
}

Gtk::Widget& EdgeCrp::Widget()
{
    return *iRp;
}


void EdgeCrp::SetUcpPos(int aPos)
{
    iRp->iUcp.iPos = aPos;
}

void EdgeCrp::SetLcpPos(int aPos)
{
    iRp->iLcp.iPos = aPos;
}

void EdgeCrp::SetUcpExt(int aX, int aY)
{
    iRp->iUcp.iOffset.width = aX;
    iRp->iUcp.iOffset.height = aY;
}

void EdgeCrp::SetLcpExt(int aX, int aY)
{
    iRp->iLcp.iOffset.width = aX;
    iRp->iLcp.iOffset.height = aY;
}

void EdgeCrp::SetCp1Coord(const Gtk::Requisition& aReq)
{
    iRp->iCp1.iCoord = aReq;
}

void EdgeCrp::SetCp2Coord(const Gtk::Requisition& aReq)
{
    iRp->iCp2.iCoord = aReq;
}

const Gtk::Requisition& EdgeCrp::Cp1Coord()
{
    return iRp->iCp1.iCoord;
}
const Gtk::Requisition& EdgeCrp::Cp2Coord()
{
    return iRp->iCp2.iCoord;
}

Elem* EdgeCrp::Point1()
{
    Elem* res = NULL;
    Elem* elem = iRp->iElem;
    MEdge* medge = elem->GetObj(medge);
    MVert* p1 = medge->Point1();
    if (p1 != NULL) {
	res = p1->EBase()->GetObj(res);
    }
    return res;
}

Elem* EdgeCrp::Point2()
{
    Elem* res = NULL;
    Elem* elem = iRp->iElem;
    MEdge* medge = elem->GetObj(medge);
    MVert* p1 = medge->Point2();
    if (p1 != NULL) {
	res = p1->EBase()->GetObj(res);
    }
    return res;

}

void EdgeCrp::SetType(EdgeType aType)
{
    iRp->iType = aType;
}

MCrp::tSigButtonPressName EdgeCrp::SignalButtonPressName()
{
    return iSigButtonPressName;
}

MCrp::tSigUpdated EdgeCrp::SignalUpdated()
{
    return iRp->iSigUpdated;
}

bool EdgeCrp::IsActionSupported(Action aAction)
{
    return (aAction == EA_Remove || aAction == EA_Rename);
}

bool EdgeCrp::Dragging()
{
    return iRp->iDragging;
}

void EdgeCrp::SetHighlighted(bool aSet)
{
    iRp->DoSetHighlighted(aSet);
}

Elem* EdgeCrp::Model()
{
    return iRp->iElem;
}

MCrp::tSigButtonPress EdgeCrp::SignalButtonPress()
{
    return iRp->iSigButtonPress;
}

void EdgeCrp::GetContentUri(GUri& aUri)
{
}

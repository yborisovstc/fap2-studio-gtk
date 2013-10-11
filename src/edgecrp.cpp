
#include <iostream>
#include "edgecrp.h"
#include <edge.h>
#include <mvert.h>
#include "common.h"
#include <gdkmm/event.h>

const int KEdgeDragThreshold = 5;

static GtkTargetEntry targetentries[] =
{
//    { (gchar*) "STRING",        0, KTei_EdgeCp },
    { (gchar*) "text/edge-cp-uri", 0, KTei_EdgeCp },
//    { (gchar*) "text/plain",    0, KTei_EdgeCp },
//    { (gchar*) "text/uri-list", 0, 2 },
};

const string sMEdgeCrpType = "MEdgeCrp";

const string& MEdgeCrp::Type()
{
    return sMEdgeCrpType;
}

EdgeCompRp::EdgeCompRp(Elem* aElem): iElem(aElem), iType(MEdgeCrp::EtLeft)
{
    // set no_window mode
    set_has_window(false);
    // Set name
    set_name(iElem->Name());
    // Set events mask
    add_events(Gdk::BUTTON_PRESS_MASK);
    // Setup DnD source
    iEboxP1.drag_source_set(Gtk::ArrayHandle_TargetEntry(targetentries, 1, Glib::OWNERSHIP_NONE));
    iEboxP1.drag_highlight();
    signal_drag_data_get().connect(sigc::mem_fun(*this, &EdgeCompRp::on_drag_data_get));
    signal_drag_begin().connect(sigc::mem_fun(*this, &EdgeCompRp::on_drag_begin));
}

EdgeCompRp::~EdgeCompRp()
{
}

EdgeCompRp::Cp::Cp(): iPos(0)
{
    iOffset.width = 0;
    iOffset.height = 0;
}

void EdgeCompRp::on_drag_data_get(const Glib::RefPtr<Gdk::DragContext>&, Gtk::SelectionData& data, guint, guint)
{
}

void EdgeCompRp::on_drag_begin(const Glib::RefPtr<Gdk::DragContext>&)
{
}

bool EdgeCompRp::on_expose_event(GdkEventExpose* aEvent)
{
    Glib::RefPtr<Gdk::Window> drw = get_window();
    Glib::RefPtr<Gtk::Style> style = get_style(); 	
    Glib::RefPtr<Gdk::GC> gc = style->get_fg_gc(get_state());
    Gtk::Allocation alc = get_allocation();
    if (iType == MEdgeCrp::EtLeft) {
	int ux = alc.get_x() + iUcp.iOffset.width;
	int uy = alc.get_y() + iUcp.iPos*KConnVertGap;
	int lx = alc.get_x() + iLcp.iOffset.width;
	int ly = alc.get_y() + alc.get_height() - 1 - iLcp.iPos*KConnVertGap;
	drw->draw_line(gc, ux, uy, alc.get_x() + alc.get_width() - 1, uy);
	drw->draw_line(gc, alc.get_x() + alc.get_width() - 1, uy, alc.get_x() + alc.get_width() - 1, ly);
	drw->draw_line(gc, lx, ly , alc.get_x() + alc.get_width() - 1, ly);
    }
    else if (iType == MEdgeCrp::EtRight) {
	int ux = alc.get_x() + alc.get_width() - 1 + iUcp.iOffset.width;
	int uy = alc.get_y();
	int lx = alc.get_x() + alc.get_width() - 1 + iLcp.iOffset.width;
	int ly = alc.get_y() + alc.get_height() - 1;
	drw->draw_line(gc, alc.get_x(), uy, ux, uy);
	drw->draw_line(gc, alc.get_x(), uy, alc.get_x(), ly);
	drw->draw_line(gc, alc.get_x(), ly, lx, ly);
    }
    else if (iType == MEdgeCrp::EtLtRb) {
	// Left top to right bottom
	int ux = alc.get_x();
	int urx = alc.get_x() + iUcp.iOffset.width; // Upper right
	int uy = alc.get_y();
	int lx = alc.get_x() + alc.get_width() - 1;
	int ly = alc.get_y() + alc.get_height() - 1;
	drw->draw_line(gc, urx, uy, urx, ly);
	drw->draw_line(gc, ux, uy, urx, uy);
	drw->draw_line(gc, urx, ly , lx, ly);
    }
    else {
	// Left bottom to right top
	int ux = alc.get_x() + alc.get_width() - 1;
	int urx = alc.get_x() + iUcp.iOffset.width; // Upper right
	int uy = alc.get_y();
	int lx = alc.get_x();
	int ly = alc.get_y() + alc.get_height() - 1;
	drw->draw_line(gc, urx, uy, urx, ly);
	drw->draw_line(gc, urx, uy, ux, uy);
	drw->draw_line(gc, lx, ly , urx, ly);
    }
}

void EdgeCompRp::on_size_request(Gtk::Requisition* aReq)
{
   // aReq->width = KConnHorizSpreadMin; 
    aReq->width = 1; 
    aReq->height = KViewCompGapHight;
}



// Edge Crp widget, version 1
EdgeCompRp_v1::EdgeCompRp_v1(Elem* aElem): iElem(aElem), iType(MEdgeCrp::EtLeft), iP1(NULL)
{
    // set no_window mode
    set_has_window(false);
    // Set name
    set_name(iElem->Name());
    // Set events mask
    add_events(Gdk::BUTTON_PRESS_MASK);
    // Setup DnD source
    Gtk::EventBox* p1 = new Gtk::EventBox();
    add(*p1);
    p1->add_events(Gdk::BUTTON_PRESS_MASK);
    p1->set_sensitive(true);
    p1->show();
    p1->signal_drag_begin().connect(sigc::mem_fun(*this, &EdgeCompRp_v1::on_cp_drag_begin));
    p1->drag_highlight();
    p1->drag_source_set(Gtk::ArrayHandle_TargetEntry(targetentries, 1, Glib::OWNERSHIP_NONE));
    /*
    //add(iEboxP1);
    iEboxP1.add_events(Gdk::BUTTON_PRESS_MASK);
    iEboxP1.set_sensitive(true);
    iEboxP1.show();
    iEboxP1.signal_drag_begin().connect(sigc::mem_fun(*this, &EdgeCompRp_v1::on_cp_drag_begin));
    iEboxP1.signal_button_press_event().connect(sigc::mem_fun(*this, &EdgeCompRp_v1::on_cp_button_press));
    iEboxP1.drag_source_set(Gtk::ArrayHandle_TargetEntry(targetentries));
    iEboxP1.drag_highlight();
    */
}

EdgeCompRp_v1::~EdgeCompRp_v1()
{
}

EdgeCompRp_v1::Cp::Cp(): iPos(0)
{
    iOffset.width = 0;
    iOffset.height = 0;
}

void EdgeCompRp_v1::add(Gtk::Widget& widget)
{
    widget.set_parent(*this);
    if (iP1 == NULL) {
	iP1 = &widget;
    }
}

void EdgeCompRp_v1::forall_vfunc(gboolean include_internals, GtkCallback callback, gpointer callback_data)
{
    if (iP1 != NULL) {
	(*callback) (iP1->gobj(), callback_data);
    }
}

void EdgeCompRp_v1::on_size_allocate(Gtk::Allocation& aAlloc)
{
    set_allocation(aAlloc);
    Gtk::Allocation& alc = aAlloc;
//    iEboxP1.size_allocate(aAlloc);
    // Allocate connection points 
        if (iType == MEdgeCrp::EtLeft) {
	int ux = alc.get_x() + iUcp.iOffset.width;
	int uy = alc.get_y() + iUcp.iPos*KConnVertGap;
	int lx = alc.get_x() + iLcp.iOffset.width;
	int ly = alc.get_y() + alc.get_height() - 1 - iLcp.iPos*KConnVertGap;
	Gtk::Allocation allc(ux, uy, KEdgePointWidth, KEdgePointWidth);
	iP1->size_allocate(allc);
    }
    else if (iType == MEdgeCrp::EtRight) {
	int ux = alc.get_x() + alc.get_width() - 1 + iUcp.iOffset.width;
	int uy = alc.get_y();
	int lx = alc.get_x() + alc.get_width() - 1 + iLcp.iOffset.width;
	int ly = alc.get_y() + alc.get_height() - 1;
    }
    else if (iType == MEdgeCrp::EtLtRb) {
	// Left top to right bottom
	int ux = alc.get_x();
	int urx = alc.get_x() + iUcp.iOffset.width; // Upper right
	int uy = alc.get_y();
	int lx = alc.get_x() + alc.get_width() - 1;
	int ly = alc.get_y() + alc.get_height() - 1;
    }
    else {
	// Left bottom to right top
	int ux = alc.get_x() + alc.get_width() - 1;
	int urx = alc.get_x() + iUcp.iOffset.width; // Upper right
	int uy = alc.get_y();
	int lx = alc.get_x();
	int ly = alc.get_y() + alc.get_height() - 1;
    }

}

bool EdgeCompRp_v1::on_expose_event(GdkEventExpose* aEvent)
{

//    iEboxP1.send_expose((GdkEvent*) aEvent);
    Glib::RefPtr<Gdk::Window> drw = get_window();
    Glib::RefPtr<Gtk::Style> style = get_style(); 	
    Glib::RefPtr<Gdk::GC> gc = style->get_fg_gc(get_state());
    Gtk::Allocation alc = get_allocation();
    if (iType == MEdgeCrp::EtLeft) {
	int ux = alc.get_x() + iUcp.iOffset.width;
	int uy = alc.get_y() + iUcp.iPos*KConnVertGap;
	int lx = alc.get_x() + iLcp.iOffset.width;
	int ly = alc.get_y() + alc.get_height() - 1 - iLcp.iPos*KConnVertGap;
	drw->draw_line(gc, ux, uy, alc.get_x() + alc.get_width() - 1, uy);
	drw->draw_line(gc, alc.get_x() + alc.get_width() - 1, uy, alc.get_x() + alc.get_width() - 1, ly);
	drw->draw_line(gc, lx, ly , alc.get_x() + alc.get_width() - 1, ly);
    }
    else if (iType == MEdgeCrp::EtRight) {
	int ux = alc.get_x() + alc.get_width() - 1 + iUcp.iOffset.width;
	int uy = alc.get_y();
	int lx = alc.get_x() + alc.get_width() - 1 + iLcp.iOffset.width;
	int ly = alc.get_y() + alc.get_height() - 1;
	drw->draw_line(gc, alc.get_x(), uy, ux, uy);
	drw->draw_line(gc, alc.get_x(), uy, alc.get_x(), ly);
	drw->draw_line(gc, alc.get_x(), ly, lx, ly);
    }
    else if (iType == MEdgeCrp::EtLtRb) {
	// Left top to right bottom
	int ux = alc.get_x();
	int urx = alc.get_x() + iUcp.iOffset.width; // Upper right
	int uy = alc.get_y();
	int lx = alc.get_x() + alc.get_width() - 1;
	int ly = alc.get_y() + alc.get_height() - 1;
	drw->draw_line(gc, urx, uy, urx, ly);
	drw->draw_line(gc, ux, uy, urx, uy);
	drw->draw_line(gc, urx, ly , lx, ly);
    }
    else {
	// Left bottom to right top
	int ux = alc.get_x() + alc.get_width() - 1;
	int urx = alc.get_x() + iUcp.iOffset.width; // Upper right
	int uy = alc.get_y();
	int lx = alc.get_x();
	int ly = alc.get_y() + alc.get_height() - 1;
	drw->draw_line(gc, urx, uy, urx, ly);
	drw->draw_line(gc, urx, uy, ux, uy);
	drw->draw_line(gc, lx, ly , urx, ly);
    }
}

void EdgeCompRp_v1::on_size_request(Gtk::Requisition* aReq)
{
   // aReq->width = KConnHorizSpreadMin; 
    aReq->width = 1; 
    aReq->height = KViewCompGapHight;
}

void EdgeCompRp_v1::on_cp_drag_begin(const Glib::RefPtr<Gdk::DragContext>&)
{
}

bool EdgeCompRp_v1::on_cp_button_press(GdkEventButton* event)
{
}



// Edge Crp widget, version 2
EdgeCompRp_v2::EdgeCompRp_v2(Elem* aElem): iElem(aElem), iType(MEdgeCrp::EtLeft), iDraggedPart(EDp_None), 
    iDragging(false)
{
    // set no_window mode
    set_visible_window(false);
    // Set name
    set_name(iElem->Name());
    // Set events mask
    add_events(Gdk::BUTTON_PRESS_MASK);
    // Setup DnD source
//    drag_source_set(Gtk::ArrayHandle_TargetEntry(targetentries, 1, Glib::OWNERSHIP_NONE));
//    drag_highlight();
//    signal_drag_data_get().connect(sigc::mem_fun(*this, &EdgeCompRp_v2::on_drag_data_get));
}

EdgeCompRp_v2::~EdgeCompRp_v2()
{
}

EdgeCompRp_v2::Cp::Cp(): iPos(0)
{
    iOffset.width = 0;
    iOffset.height = 0;
    iCoord.width = 0;
    iCoord.height = 0;
}

void EdgeCompRp_v2::on_drag_data_get(const Glib::RefPtr<Gdk::DragContext>&, Gtk::SelectionData& data, guint info, guint time)
{
    std::cout << "EdgeCompRp_v2 on_drag_data_get" << std::endl;
    //if (info == KTei_EdgeCp && iDragging && iDraggedPart != EDp_None) {
    if (true) {
	std::string uri("fdfd");
	if (iDraggedPart == EDp_Cp1) {
	}
	else {
	}
	data.set_text(uri);
    }
}

void EdgeCompRp_v2::on_drag_begin(const Glib::RefPtr<Gdk::DragContext>& aContext)
{
    if (iDraggedPart != EDp_None) {
	iDragging = true;
	std::cout << "Dragging begin" << std::endl;
    }
}

bool EdgeCompRp_v2::on_button_press_event(GdkEventButton* aEvent)
{
    bool res = true;
    if (aEvent->type == GDK_BUTTON_PRESS) {
	//if (iDraggedPart == EDp_None) {
	if (true) {
	    Gtk::Allocation alc = get_allocation();
	    int ex = aEvent->x;
	    int ey = aEvent->y;
	    int cp1x = iCp1.iCoord.width - alc.get_x();
	    int cp1y = iCp1.iCoord.height - alc.get_y();
	    int cp2x = iCp2.iCoord.width - alc.get_x();
	    int cp2y = iCp2.iCoord.height - alc.get_y();
	    if (abs(ex - cp1x) < KEdgeDragThreshold || abs(ey - cp1y) < KEdgeDragThreshold) {
		iDraggedPart = EDp_Cp1;
		iDragging = true;
	    }
	    else if (abs(ex - cp2x) < KEdgeDragThreshold || abs(ey - cp2x) < KEdgeDragThreshold) {
		iDraggedPart = EDp_Cp2;
		iDragging = true;
	    }
	    //if (iDragging) {
	    if (true) {
		// Needs to use this array handle constructor with length argument, otherwise not working
		Glib::RefPtr<Gtk::TargetList> targ = Gtk::TargetList::create(Gtk::ArrayHandle_TargetEntry(targetentries, 1, Glib::OWNERSHIP_NONE));
		GdkEvent* evt = (GdkEvent*) aEvent;
		std::cout << "EdgeCompRp_v2 begin dragging from " << iDraggedPart << std::endl;
		//drag_source_set(Gtk::ArrayHandle_TargetEntry(targetentries, 1, Glib::OWNERSHIP_NONE));
		// TODO [YB] Only ACTION_COPY works here, to analyse why
		drag_begin(targ, Gdk::ACTION_COPY, aEvent->button, evt);
	    }
	}
    }
    res = Widget::on_button_press_event(aEvent);
    return res;
}

/*
bool EdgeCompRp_v2::on_motion_notify_event(GdkEventMotion* aEvent)
{
    if (iDragging) {
	if (iDraggedPart == EDp_Cp1) {
	    iCp1.iCoord.height = aEvent->y;
	    std::cout << "Dragging Cp1" << std::endl;
	    iSigUpdated.emit(iElem);
	}
	else if (iDraggedPart == EDp_Cp2) {
	    iCp2.iCoord.height = aEvent->y;
	    std::cout << "Dragging Cp2" << std::endl;
	    iSigUpdated.emit(iElem);
	}
    }
}
*/

/*
   bool EdgeCompRp_v2::on_expose_event(GdkEventExpose* aEvent)
   {
    Glib::RefPtr<Gdk::Window> drw = get_window();
    Glib::RefPtr<Gtk::Style> style = get_style(); 	
    Glib::RefPtr<Gdk::GC> gc = style->get_fg_gc(get_state());
    Gtk::Allocation alc = get_allocation();
    if (iType == MEdgeCrp::EtLeft) {
	int ux = alc.get_x() + iUcp.iOffset.width;
	int uy = alc.get_y() + iUcp.iPos*KConnVertGap;
	int lx = alc.get_x() + iLcp.iOffset.width;
	int ly = alc.get_y() + alc.get_height() - 1 - iLcp.iPos*KConnVertGap;
	drw->draw_line(gc, ux, uy, alc.get_x() + alc.get_width() - 1, uy);
	drw->draw_line(gc, alc.get_x() + alc.get_width() - 1, uy, alc.get_x() + alc.get_width() - 1, ly);
	drw->draw_line(gc, lx, ly , alc.get_x() + alc.get_width() - 1, ly);
    }
    else if (iType == MEdgeCrp::EtRight) {
	int ux = alc.get_x() + alc.get_width() - 1 + iUcp.iOffset.width;
	int uy = alc.get_y();
	int lx = alc.get_x() + alc.get_width() - 1 + iLcp.iOffset.width;
	int ly = alc.get_y() + alc.get_height() - 1;
	drw->draw_line(gc, alc.get_x(), uy, ux, uy);
	drw->draw_line(gc, alc.get_x(), uy, alc.get_x(), ly);
	drw->draw_line(gc, alc.get_x(), ly, lx, ly);
    }
    else if (iType == MEdgeCrp::EtLtRb) {
	// Left top to right bottom
	int ux = alc.get_x();
	int urx = alc.get_x() + iUcp.iOffset.width; // Upper right
	int uy = alc.get_y();
	int lx = alc.get_x() + alc.get_width() - 1;
	int ly = alc.get_y() + alc.get_height() - 1;
	drw->draw_line(gc, urx, uy, urx, ly);
	drw->draw_line(gc, ux, uy, urx, uy);
	drw->draw_line(gc, urx, ly , lx, ly);
    }
    else {
	// Left bottom to right top
	int ux = alc.get_x() + alc.get_width() - 1;
	int urx = alc.get_x() + iUcp.iOffset.width; // Upper right
	int uy = alc.get_y();
	int lx = alc.get_x();
	int ly = alc.get_y() + alc.get_height() - 1;
	drw->draw_line(gc, urx, uy, urx, ly);
	drw->draw_line(gc, urx, uy, ux, uy);
	drw->draw_line(gc, lx, ly , urx, ly);
    }
}
*/

bool EdgeCompRp_v2::on_expose_event(GdkEventExpose* aEvent)
{
    Glib::RefPtr<Gdk::Window> drw = get_window();
    Glib::RefPtr<Gtk::Style> style = get_style(); 	
    Glib::RefPtr<Gdk::GC> gc = style->get_fg_gc(get_state());
    Gtk::Allocation alc = get_allocation();
    if (iType == MEdgeCrp::EtLeft) {
	int rx = alc.get_x() + alc.get_width() - 2;
	drw->draw_line(gc, iCp1.iCoord.width, iCp1.iCoord.height, rx, iCp1.iCoord.height);
	drw->draw_line(gc, rx, iCp1.iCoord.height, rx, iCp2.iCoord.height);
	drw->draw_line(gc, iCp2.iCoord.width, iCp2.iCoord.height, rx, iCp2.iCoord.height);
    }
    else if (iType == MEdgeCrp::EtRight) {
	int ux = alc.get_x() + alc.get_width() - 1 + iUcp.iOffset.width;
	int uy = alc.get_y();
	int lx = alc.get_x() + alc.get_width() - 1 + iLcp.iOffset.width;
	int ly = alc.get_y() + alc.get_height() - 1;
	drw->draw_line(gc, alc.get_x(), uy, ux, uy);
	drw->draw_line(gc, alc.get_x(), uy, alc.get_x(), ly);
	drw->draw_line(gc, alc.get_x(), ly, lx, ly);
    }
    else if (iType == MEdgeCrp::EtLtRb) {
	// Left top to right bottom
	int ux = alc.get_x();
	int urx = alc.get_x() + iUcp.iOffset.width; // Upper right
	int uy = alc.get_y();
	int lx = alc.get_x() + alc.get_width() - 1;
	int ly = alc.get_y() + alc.get_height() - 1;
	drw->draw_line(gc, urx, uy, urx, ly);
	drw->draw_line(gc, ux, uy, urx, uy);
	drw->draw_line(gc, urx, ly , lx, ly);
    }
    else {
	// Left bottom to right top
	int ux = alc.get_x() + alc.get_width() - 1;
	int urx = alc.get_x() + iUcp.iOffset.width; // Upper right
	int uy = alc.get_y();
	int lx = alc.get_x();
	int ly = alc.get_y() + alc.get_height() - 1;
	drw->draw_line(gc, urx, uy, urx, ly);
	drw->draw_line(gc, urx, uy, ux, uy);
	drw->draw_line(gc, lx, ly , urx, ly);
    }
}

void EdgeCompRp_v2::on_size_request(Gtk::Requisition* aReq)
{
   // aReq->width = KConnHorizSpreadMin; 
    aReq->width = 1; 
    aReq->height = KViewCompGapHight;
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
    iRp = new EdgeCompRp_v2(aElem);
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


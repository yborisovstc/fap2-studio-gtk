
#include "edgecrp.h"
#include <edge.h>
#include <mvert.h>
#include "common.h"
#include <gdkmm/event.h>

static GtkTargetEntry targetentries[] =
{
    { (gchar*) "STRING",        0, 0 },
    { (gchar*) "text/plain",    0, 1 },
    { (gchar*) "text/uri-list", 0, 2 },
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
    iEboxP1.drag_source_set(Gtk::ArrayHandle_TargetEntry(targetentries));
    iEboxP1.drag_highlight();
    signal_drag_data_get().connect(sigc::mem_fun(*this, &EdgeCompRp::on_drag_data_get));
    signal_drag_begin().connect(sigc::mem_fun(*this, &EdgeCompRp::on_drag_begin));
}

EdgeCompRp::~EdgeCompRp()
{
}

EdgeCompRp::Cp::Cp(): iPos(0)
{
    iCoord.width = 0;
    iCoord.height = 0;
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
	int ux = alc.get_x() + iUcp.iCoord.width;
	int uy = alc.get_y() + iUcp.iPos*KConnVertGap;
	int lx = alc.get_x() + iLcp.iCoord.width;
	int ly = alc.get_y() + alc.get_height() - 1 - iLcp.iPos*KConnVertGap;
	drw->draw_line(gc, ux, uy, alc.get_x() + alc.get_width() - 1, uy);
	drw->draw_line(gc, alc.get_x() + alc.get_width() - 1, uy, alc.get_x() + alc.get_width() - 1, ly);
	drw->draw_line(gc, lx, ly , alc.get_x() + alc.get_width() - 1, ly);
    }
    else if (iType == MEdgeCrp::EtRight) {
	int ux = alc.get_x() + alc.get_width() - 1 + iUcp.iCoord.width;
	int uy = alc.get_y();
	int lx = alc.get_x() + alc.get_width() - 1 + iLcp.iCoord.width;
	int ly = alc.get_y() + alc.get_height() - 1;
	drw->draw_line(gc, alc.get_x(), uy, ux, uy);
	drw->draw_line(gc, alc.get_x(), uy, alc.get_x(), ly);
	drw->draw_line(gc, alc.get_x(), ly, lx, ly);
    }
    else if (iType == MEdgeCrp::EtLtRb) {
	// Left top to right bottom
	int ux = alc.get_x();
	int urx = alc.get_x() + iUcp.iCoord.width; // Upper right
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
	int urx = alc.get_x() + iUcp.iCoord.width; // Upper right
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
    p1->drag_source_set(Gtk::ArrayHandle_TargetEntry(targetentries));
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
    iCoord.width = 0;
    iCoord.height = 0;
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
	int ux = alc.get_x() + iUcp.iCoord.width;
	int uy = alc.get_y() + iUcp.iPos*KConnVertGap;
	int lx = alc.get_x() + iLcp.iCoord.width;
	int ly = alc.get_y() + alc.get_height() - 1 - iLcp.iPos*KConnVertGap;
	Gtk::Allocation allc(ux, uy, KEdgePointWidth, KEdgePointWidth);
	iP1->size_allocate(allc);
    }
    else if (iType == MEdgeCrp::EtRight) {
	int ux = alc.get_x() + alc.get_width() - 1 + iUcp.iCoord.width;
	int uy = alc.get_y();
	int lx = alc.get_x() + alc.get_width() - 1 + iLcp.iCoord.width;
	int ly = alc.get_y() + alc.get_height() - 1;
    }
    else if (iType == MEdgeCrp::EtLtRb) {
	// Left top to right bottom
	int ux = alc.get_x();
	int urx = alc.get_x() + iUcp.iCoord.width; // Upper right
	int uy = alc.get_y();
	int lx = alc.get_x() + alc.get_width() - 1;
	int ly = alc.get_y() + alc.get_height() - 1;
    }
    else {
	// Left bottom to right top
	int ux = alc.get_x() + alc.get_width() - 1;
	int urx = alc.get_x() + iUcp.iCoord.width; // Upper right
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
	int ux = alc.get_x() + iUcp.iCoord.width;
	int uy = alc.get_y() + iUcp.iPos*KConnVertGap;
	int lx = alc.get_x() + iLcp.iCoord.width;
	int ly = alc.get_y() + alc.get_height() - 1 - iLcp.iPos*KConnVertGap;
	drw->draw_line(gc, ux, uy, alc.get_x() + alc.get_width() - 1, uy);
	drw->draw_line(gc, alc.get_x() + alc.get_width() - 1, uy, alc.get_x() + alc.get_width() - 1, ly);
	drw->draw_line(gc, lx, ly , alc.get_x() + alc.get_width() - 1, ly);
    }
    else if (iType == MEdgeCrp::EtRight) {
	int ux = alc.get_x() + alc.get_width() - 1 + iUcp.iCoord.width;
	int uy = alc.get_y();
	int lx = alc.get_x() + alc.get_width() - 1 + iLcp.iCoord.width;
	int ly = alc.get_y() + alc.get_height() - 1;
	drw->draw_line(gc, alc.get_x(), uy, ux, uy);
	drw->draw_line(gc, alc.get_x(), uy, alc.get_x(), ly);
	drw->draw_line(gc, alc.get_x(), ly, lx, ly);
    }
    else if (iType == MEdgeCrp::EtLtRb) {
	// Left top to right bottom
	int ux = alc.get_x();
	int urx = alc.get_x() + iUcp.iCoord.width; // Upper right
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
	int urx = alc.get_x() + iUcp.iCoord.width; // Upper right
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
    iRp = new EdgeCompRp_v1(aElem);
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


void EdgeCrp::SetUcp(int aPos)
{
    iRp->iUcp.iPos = aPos;
}

void EdgeCrp::SetLcp(int aPos)
{
    iRp->iLcp.iPos = aPos;
}

void EdgeCrp::SetUcpExt(int aX, int aY)
{
    iRp->iUcp.iCoord.width = aX;
    iRp->iUcp.iCoord.height = aY;
}

void EdgeCrp::SetLcpExt(int aX, int aY)
{
    iRp->iLcp.iCoord.width = aX;
    iRp->iLcp.iCoord.height = aY;
}

void EdgeCrp::SetExtent(Gtk::Requisition aReq, int aPos)
{
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

bool EdgeCrp::IsActionSupported(Action aAction)
{
    return (aAction == EA_Remove || aAction == EA_Rename);
}



#include "edgecrp.h"
#include <edge.h>
#include <mvert.h>
#include "common.h"

const string sMEdgeCrpType = "MEdgeCrp";

const string& MEdgeCrp::Type()
{
    return sMEdgeCrpType;
}

EdgeCompRp::EdgeCompRp(Elem* aElem): iElem(aElem)
{
    // set no_window mode
    set_has_window(false);
    // Set name
    set_name(iElem->Name());
    // Set events mask
    add_events(Gdk::BUTTON_PRESS_MASK);
}

EdgeCompRp::~EdgeCompRp()
{
}


bool EdgeCompRp::on_expose_event(GdkEventExpose* aEvent)
{
    // Draw body rect
    Glib::RefPtr<Gdk::Window> drw = get_window();
    Glib::RefPtr<Gtk::Style> style = get_style(); 	
    Glib::RefPtr<Gdk::GC> gc = style->get_fg_gc(get_state());
    Gtk::Allocation alc = get_allocation();
    drw->draw_line(gc, alc.get_x(), alc.get_y(), alc.get_x() + alc.get_width() - 1, alc.get_y());
    drw->draw_line(gc, alc.get_x() + alc.get_width() - 1, alc.get_y(), alc.get_x() + alc.get_width() - 1, alc.get_y() + alc.get_height() - 1);
    drw->draw_line(gc, alc.get_x(), alc.get_y() + alc.get_height() - 1 , alc.get_x() + alc.get_width() - 1, alc.get_y() + alc.get_height() - 1);
}

void EdgeCompRp::on_size_request(Gtk::Requisition* aRequisition)
{
    aRequisition->width = KConnHorizSpreadMin; 
    aRequisition->height = KViewCompGapHight;
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
    iRp = new EdgeCompRp(aElem);
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


void EdgeCrp::SetUcp(Gtk::Requisition aReq, int aPos)
{
    iRp->iUcp.iCoord = aReq;
    iRp->iUcp.iPos = aPos;
}

void EdgeCrp::SetLcp(Gtk::Requisition aReq, int aPos)
{
    iRp->iLcp.iCoord = aReq;
    iRp->iLcp.iPos = aPos;
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



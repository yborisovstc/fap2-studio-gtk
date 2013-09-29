
#include <prop.h>
#include "common.h"
#include "propcrp.h"

const string sType = "PropCrp";

const string& PropCrp::Type()
{
    return sType;
}

string PropCrp::EType()
{
    return Prop::PEType();
}

PropCrp::PropCrp(Elem* aElem): ElemCompRp(aElem)
{

    MProp* prop = iElem->GetObj(prop);
    assert(prop != NULL);
    iContent.set_text(prop->Value());
}

PropCrp::~PropCrp()
{
    delete iHead;
}

void *PropCrp::DoGetObj(const string& aName)
{
    void* res = NULL;
    if (aName ==  Type()) {
	res = this;
    }
    return res;
}

bool PropCrp::on_expose_event(GdkEventExpose* aEvent)
{
    Gtk::Layout::on_expose_event(aEvent);
    // Draw body rect
    Glib::RefPtr<Gdk::Window> drw = get_bin_window();
    Glib::RefPtr<Gtk::Style> style = get_style(); 	
    Glib::RefPtr<Gdk::GC> gc = style->get_fg_gc(get_state());
    drw->draw_rectangle(gc, false, iBodyAlc.get_x(), iBodyAlc.get_y(), iBodyAlc.get_width() - 1, iBodyAlc.get_height() - 1);
    // Head separator
    Gtk::Allocation head_alc = iHead->get_allocation();
    drw->draw_line(gc, iBodyAlc.get_x(), head_alc.get_height(), iBodyAlc.get_x() + iBodyAlc.get_width() - 1, head_alc.get_height());
}

void PropCrp::on_size_allocate(Gtk::Allocation& aAllc)
{
    Gtk::Layout::on_size_allocate(aAllc);
    Gtk::Requisition head_req = iHead->size_request();

    // Calculate allocation of comp body
    iBodyAlc = Gtk::Allocation(0, 0, aAllc.get_width(), aAllc.get_height());

    // Allocate header
    Gtk::Allocation head_alc = Gtk::Allocation(iBodyAlc.get_x(), iBodyAlc.get_y(), iBodyAlc.get_width(), head_req.height);
    iHead->size_allocate(head_alc);
}

void PropCrp::on_size_request(Gtk::Requisition* aRequisition)
{
    Gtk::Requisition head_req = iHead->size_request();
    // Body width
    gint body_w = head_req.width;
    TInt body_h = KViewCompEmptyBodyHight;
    aRequisition->width = body_w; 
    aRequisition->height = head_req.height + body_h;
}



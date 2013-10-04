
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
    Glib::RefPtr<Gtk::TextBuffer> buf = Gtk::TextBuffer::create();
    buf->set_text(prop->Value());
    //iContent.set_buffer(buf);
    iContent.set_text(prop->Value());
    add(iContent);
    iContent.show();
}

PropCrp::~PropCrp()
{
}

void *PropCrp::DoGetObj(const string& aName)
{
    void* res = NULL;
    if (aName ==  Type()) {
	res = this;
    }
    return res;
}

Gtk::Widget& PropCrp::Widget()
{
    return *this;
}

MCrp::tSigButtonPressName PropCrp::SignalButtonPressName()
{
    return iSigButtonPressName;
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
    // Allocate content
    Gtk::Allocation cont_alc = Gtk::Allocation(iBodyAlc.get_x(), iBodyAlc.get_y() + head_alc.get_height() + KViewCompEmptyBodyHight/2,
	    iBodyAlc.get_width(), aAllc.get_height() - head_req.height - KViewCompEmptyBodyHight);
    iContent.size_allocate(cont_alc);
}

void PropCrp::on_size_request(Gtk::Requisition* aReq)
{
    ElemCompRp::on_size_request(aReq);
    // Updating with size of content
    Gtk::Requisition cont_req = iContent.size_request();
    aReq->height += cont_req.height;
    aReq->width = max(aReq->width, cont_req.width + 2*KViewElemCrpInnerBorder); 
}

bool PropCrp::IsActionSupported(Action aAction)
{
    return (aAction == EA_Remove || aAction == EA_Rename || aAction == EA_Edit_Content);
}


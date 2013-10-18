#include "common.h"
#include "elemcomprp.h"

ElemCrpCtxMenu::ElemCrpCtxMenu(): Gtk::Menu()
{
}



	
ElemCompHead::ElemCompHead(const Elem& aElem): iElem(aElem)
{
    // Create Name
    iName = new Gtk::Label();
    iName->set_text(iElem.Name());
    iName->show();
    pack_start(*iName, false, false, KCompHeaderLabelsGap);
    // Create Parent
    iParent = new Gtk::Label();
    iParent->set_text(iElem.EType());
    iParent->show();
    pack_start(*iParent, false, false, 2);
}

ElemCompHead::~ElemCompHead()
{
    delete iName;
    delete iParent;
}

bool ElemCompHead::on_expose_event(GdkEventExpose* aEvent)
{
    Gtk::HBox::on_expose_event(aEvent);
    // Parent separator
    Gtk::Allocation alc = get_allocation();
    Gtk::Allocation p_alc = iParent->get_allocation();
    Glib::RefPtr<Gdk::Window> drw = get_window();
    Glib::RefPtr<Gtk::Style> style = get_style(); 	
    Glib::RefPtr<Gdk::GC> gc = style->get_fg_gc(get_state());
    gint x = p_alc.get_x() - KCompHeaderLabelsGap/2;
    drw->draw_line(gc, x, alc.get_y(), x, alc.get_height() - 1);
}


ElemCompRp::ElemCompRp(Elem* aElem): iElem(aElem), iHead(NULL)
{
    // Set name
    set_name(iElem->Name());
    // Add header
    iHead = new ElemCompHead(*iElem);
    add(*iHead);
    iHead->show();
    // Set events mask
    add_events(Gdk::BUTTON_PRESS_MASK);
    iHead->iName->signal_button_press_event().connect(sigc::mem_fun(*this, &ElemCompRp::on_name_button_press));
}

ElemCompRp::~ElemCompRp()
{
    delete iHead;
}


bool ElemCompRp::on_expose_event(GdkEventExpose* aEvent)
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

void ElemCompRp::on_size_allocate(Gtk::Allocation& aAllc)
{
    Gtk::Layout::on_size_allocate(aAllc);
    Gtk::Requisition head_req = iHead->size_request();

    // Calculate allocation of comp body
    iBodyAlc = Gtk::Allocation(0, 0, aAllc.get_width(), aAllc.get_height());

    // Allocate header
    Gtk::Allocation head_alc = Gtk::Allocation(iBodyAlc.get_x(), iBodyAlc.get_y(), iBodyAlc.get_width(), head_req.height);
    iHead->size_allocate(head_alc);
}

void ElemCompRp::on_size_request(Gtk::Requisition* aRequisition)
{
    Gtk::Requisition head_req = iHead->size_request();
    // Body width
    gint body_w = head_req.width;
    TInt body_h = KViewCompEmptyBodyHight;
    aRequisition->width = body_w; 
    aRequisition->height = head_req.height + body_h;
}

bool ElemCompRp::on_name_button_press(GdkEventButton* event)
{
    iSigButtonPressName.emit(event);
}




const string sType = "ElemCrp";

const string& ElemCrp::Type()
{
    return sType;
}

string ElemCrp::EType()
{
    return ":Elem";
}

ElemCrp::ElemCrp(Elem* aElem)
{
    iRp = new ElemCompRp(aElem);
}

ElemCrp::~ElemCrp()
{
    delete iRp;
}

void *ElemCrp::DoGetObj(const string& aName)
{
    void* res = NULL;
    if (aName ==  Type()) {
	res = this;
    }
    return res;
}

Gtk::Widget& ElemCrp::Widget()
{
    return *iRp;
}

MCrp::tSigButtonPressName ElemCrp::SignalButtonPressName()
{
    return iRp->iSigButtonPressName;
}

MCrp::tSigUpdated ElemCrp::SignalUpdated()
{
    return iRp->iSigUpdated;
}

bool ElemCrp::IsActionSupported(Action aAction)
{
    return (aAction == EA_Remove || aAction == EA_Rename);
}

bool ElemCrp::Dragging()
{
    return false;
}

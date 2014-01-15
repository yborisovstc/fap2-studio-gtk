#include <complex>
#include <vert.h>
#include "common.h"
#include "vertcrp.h"

VertCompHead::VertCompHead(const Elem& aElem): iElem(aElem)
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

VertCompHead::~VertCompHead()
{
    delete iName;
    delete iParent;
}

bool VertCompHead::on_expose_event(GdkEventExpose* aEvent)
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


const string sType = "VertCompRp";

const string& VertCompRp::Type()
{
    return sType;
}

string VertCompRp::EType()
{
    return Vert::PEType();
}

VertCompRp::VertCompRp(Elem* aElem): ElemCompRp(aElem), iIsInt(true)
{
}

VertCompRp::~VertCompRp()
{
}

void *VertCompRp::DoGetObj(const string& aName)
{
    void* res = NULL;
    if (aName ==  Type()) {
	res = this;
    }
    else if (aName ==  MCrpConnectable::Type()) {
	res = (MCrpConnectable*) this;
    }
    return res;
}

bool VertCompRp::on_expose_event(GdkEventExpose* aEvent)
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

void VertCompRp::on_size_allocate(Gtk::Allocation& aAllc)
{
    Gtk::Layout::on_size_allocate(aAllc);
    Gtk::Requisition head_req = iHead->size_request();

    // Calculate allocation of comp body
    iBodyAlc = Gtk::Allocation(0, 0, aAllc.get_width(), aAllc.get_height());

    // Allocate header
    Gtk::Allocation head_alc = Gtk::Allocation(iBodyAlc.get_x(), iBodyAlc.get_y(), iBodyAlc.get_width(), head_req.height);
    iHead->size_allocate(head_alc);
}

void VertCompRp::on_size_request(Gtk::Requisition* aRequisition)
{
    Gtk::Requisition head_req = iHead->size_request();
    // Body width
    gint body_w = head_req.width;
    TInt body_h = KViewCompEmptyBodyHight;
    aRequisition->width = body_w; 
    aRequisition->height = head_req.height + body_h;
}

Gtk::Requisition VertCompRp::GetCpCoord(Elem* aCp)
{
    Gtk::Allocation alc = get_allocation();
    Gtk::Requisition head_req = iHead->size_request();
    TInt body_h = KViewCompEmptyBodyHight;
    Gtk::Requisition res;
    res.width = alc.get_x() + (iIsInt ? alc.get_width() : 0);
    res.height = alc.get_y() + head_req.height + body_h / 2;
    return res;
}

Gtk::Widget& VertCompRp::Widget()
{
    return *this;
}

bool VertCompRp::GetIsInt() const
{
    return iIsInt;
}

void VertCompRp::SetIsInt(bool aIsInt)
{
    if (aIsInt != iIsInt) {
	iIsInt = aIsInt;
    }
}

static int dist(int x, int y)
{
    return sqrt(x*x + y*y);
}

int VertCompRp::GetNearestCp(Gtk::Requisition aCoord, Elem*& aCp)
{
    int res = -1;
    Gtk::Requisition cpcoord = GetCpCoord(NULL);
    /*
    std::complex<int> sub(cpcoord.width - aCoord.width, cpcoord.height - aCoord.height);
    res = std::abs(sub);
    */
    res = dist(cpcoord.width - aCoord.width, cpcoord.height - aCoord.height);
    aCp = iElem;
    return res;
}

void VertCompRp::HighlightCp(Elem* aCp, bool aSet)
{
    SetHighlighted(aSet);
}


MCrp::tSigButtonPressName VertCompRp::SignalButtonPressName()
{
    return iSigButtonPressName;
}

MCrp::tSigUpdated VertCompRp::SignalUpdated()
{
    return iSigUpdated;
}

bool VertCompRp::IsActionSupported(Action aAction)
{
    return (aAction == EA_Remove || aAction == EA_Rename);
}

bool VertCompRp::Dragging()
{
    return false;
}

void VertCompRp::SetHighlighted(bool aSet)
{
    DoSetHighlighted(aSet);
}

Elem* VertCompRp::Model()
{
    return iElem;
}

MCrp::tSigButtonPress VertCompRp::SignalButtonPress()
{
    return iSigButtonPress;
}

#if 0

const string sType = "VertCrp";

const string& VertCrp::Type()
{
    return sType;
}

string VertCrp::EType()
{
    return Vert::PEType();
}

VertCrp::VertCrp(Elem* aElem)
{
    iRp = new VertCompRp(aElem);
}

VertCrp::~VertCrp()
{
    delete iRp;
}

void *VertCrp::DoGetObj(const string& aName)
{
    void* res = NULL;
    if (aName ==  Type()) {
	res = this;
    }
    else if (aName ==  MCrpConnectable::Type()) {
	res = (MCrpConnectable*) this;
    }
    return res;
}

Gtk::Widget& VertCrp::Widget()
{
    return *iRp;
}

Gtk::Requisition VertCrp::GetCpCoord(Elem* aCp)
{
    return iRp->GetCpCoord(aCp);
}

#endif

void VertCompRp::GetContentUri(GUri& aUri)
{
}


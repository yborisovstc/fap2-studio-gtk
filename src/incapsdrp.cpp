
#include <incaps.h>

#include "common.h"
#include "incapsdrp.h"


// CRP for Capsule

const string sCapsCrpType = "CapsCrp";

const string& CapsCrp::Type()
{
    return sCapsCrpType;
}

string CapsCrp::EType()
{
    return Elem::PEType();
}

CapsCrp::CapsCrp(Elem* aElem): iElem(aElem)
{
    // set no_window mode
    set_has_window(false);
    // Set name
    set_name("Capsule");
    // Set events mask
    add_events(Gdk::BUTTON_PRESS_MASK | Gdk::POINTER_MOTION_MASK);
}

CapsCrp::~CapsCrp()
{
}

Gtk::Widget& CapsCrp::Widget()
{
    return *this;
}

void *CapsCrp::DoGetObj(const string& aName)
{
    void* res = NULL;
    return res;
}

MCrp::tSigButtonPressName CapsCrp::SignalButtonPressName()
{
    return iSigButtonPressName;
}

MCrp::tSigButtonPress CapsCrp::SignalButtonPress()
{
    return iSigButtonPress;
}

MCrp::tSigUpdated CapsCrp::SignalUpdated()
{
    return iSigUpdated;
}

bool CapsCrp::IsActionSupported(Action aAction)
{
    return false;
}

void CapsCrp::GetContentUri(GUri& aUri)
{
}

bool CapsCrp::Dragging()
{
    return false;
}

void CapsCrp::SetHighlighted(bool aSet)
{
}

void CapsCrp::SetErroneous(bool aSet)
{
}

Elem* CapsCrp::Model()
{
    return iElem;
}

void CapsCrp::SetLArea(int aArea)
{
}

int CapsCrp::GetLArea() const
{
    return MCrp::EOverlay;
}

void CapsCrp::on_size_request(Gtk::Requisition* aReq)
{
    aReq->width = 2*KBoundCompGapWidth; 
    aReq->height = KViewCompGapHight;
}

bool CapsCrp::on_expose_event(GdkEventExpose* aEvent)
{
    Glib::RefPtr<Gdk::Window> drw = get_window();
    Glib::RefPtr<Gtk::Style> style = get_style(); 	
    Glib::RefPtr<Gdk::GC> gc = style->get_fg_gc(get_state());
    Gtk::Allocation alc = get_allocation();

    drw->draw_rectangle(gc, false, alc.get_x(), alc.get_y(), alc.get_width() - 1, alc.get_height() - 1);
}




const string IncapsDrp::KCapsUri = "./Capsule";
const string IncapsDrp::KIncapsType = Syst::PEType() + ":Incaps";
const string IncapsDrp::KExtdType = "Elem:Vert:Extender";

string IncapsDrp::EType()
{
    return ":Elem:Vert:Syst:Incaps";
}

IncapsDrp::IncapsDrp(Elem* aElem, const MCrpProvider& aCrpProv, MSEnv& aStEnv): SysDrp(aElem, aCrpProv, aStEnv)
{
}

IncapsDrp::~IncapsDrp()
{
}

void IncapsDrp::Construct()
{
    Elem* caps = Model()->GetNode(KCapsUri);
    // Add Caps CRP
    mCapsLCrp = new CapsCrp(caps);
    Gtk::Widget& capscrpw = mCapsLCrp->Widget();
    add(capscrpw);
    //iCompRps[caps] = capscrp;
    capscrpw.show();
    // Add CRPs from body
    assert(caps != NULL);
    for (std::vector<Elem*>::iterator it = iElem->Comps().begin(); it != iElem->Comps().end(); it++) {
	Elem* comp = *it;
	assert(comp != NULL);
	if (!comp->IsRemoved() && comp != caps) {
	    MCrp* rp = iCrpProv.CreateRp(*comp, this);
	    Gtk::Widget& rpw = rp->Widget();
	    // Using specific signal for button press instead of standard because some Crps can have complex layout
	    rp->SignalButtonPress().connect(sigc::bind<Elem*>(sigc::mem_fun(*this, &ElemDetRp::on_comp_button_press), comp));
	    rp->SignalButtonPressName().connect(sigc::bind<Elem*>(sigc::mem_fun(*this, &ElemDetRp::on_comp_button_press_name), comp));
	    add(rpw);
	    iCompRps[comp] = rp;
	    rpw.show();
	}
    }
    // Adding CRPs from Capsule
    for (std::vector<Elem*>::iterator it = caps->Comps().begin(); it != caps->Comps().end(); it++) {
	Elem* comp = *it;
	assert(comp != NULL);
	MCrp* rp = iCrpProv.CreateRp(*comp, this);
	Gtk::Widget& rpw = rp->Widget();
	rp->SignalButtonPress().connect(sigc::bind<Elem*>(sigc::mem_fun(*this, &ElemDetRp::on_comp_button_press), comp));
	rp->SignalButtonPressName().connect(sigc::bind<Elem*>(sigc::mem_fun(*this, &ElemDetRp::on_comp_button_press_name), comp));
	add(rpw);
	iCompRps[comp] = rp;
	rpw.show();
    }
    UpdateRpsRelatios();
    PreLayoutRps();
}

void *IncapsDrp::DoGetObj(const string& aName)
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

TBool IncapsDrp::IsTypeOf(const string& aType, const string& aParent) const
{
    int pos = aType.find(aParent);
    return pos != string::npos;
}

bool IncapsDrp::IsTypeAllowed(const std::string& aType) const
{
    bool res = false;
    if (SysDrp::IsTypeAllowed(aType) || IsTypeOf(aType, KIncapsType) || 
	   IsTypeOf(aType, KExtdType))  {
	res = true;
    }
    return res;
}

void IncapsDrp::on_size_allocate(Gtk::Allocation& aAllc)
{
    SysDrp::on_size_allocate(aAllc);
    // Allocate Capsule RPs
    TLAreaPar& area = iLaPars.at(0);
    Allocation& allc = area.first;
    allc.set_x(allc.get_x() - 2);
    allc.set_y(allc.get_y() - 2);
    allc.set_width(allc.get_width() + 4);
    allc.set_height(allc.get_height() + 4);
    mCapsLCrp->Widget().size_allocate(allc);
}

void IncapsDrp::on_size_request(Gtk::Requisition* aRequisition)
{
    SysDrp::on_size_request(aRequisition);
}

Elem* IncapsDrp::GetCompOwning(Elem* aElem)
{
    Elem* res = NULL;
    Elem* caps = iElem->GetNode(KCapsUri);
    for (std::vector<Elem*>::iterator it = iElem->Comps().begin(); it != iElem->Comps().end() && res == NULL; it++) {
	Elem* comp = *it;
	if (aElem == comp || comp != caps && comp->IsComp(aElem)) {
	    res = comp;
	}
    }
    if (res == NULL) {
	for (std::vector<Elem*>::iterator it = caps->Comps().begin(); it != caps->Comps().end() && res == NULL; it++) {
	    Elem* comp = *it;
	    if (aElem == comp || comp->IsComp(aElem)) {
		res = comp;
	    }
	}
    }
    return res;
}

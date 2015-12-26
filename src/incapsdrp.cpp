
#include <incaps.h>
#include <iostream>

#include "common.h"
#include "incapsdrp.h"


// CRP for Capsule

const string sMCapsCrp_Type = "MCapsCrp";

const string& MCapsCrp::Type()
{
    return sMCapsCrp_Type;
} 

const string sCapsCrpType = "CapsCrp";

const string& CapsCrp::Type()
{
    return sCapsCrpType;
}

string CapsCrp::EType()
{
    return Elem::PEType();
}

CapsCrp::CapsCrp(MElem* aElem): iElem(aElem)
{
    // set no_window mode
    set_has_window(false);
    // Set name
    set_name("CapsCrp");
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
    if (aName ==  MCapsCrp::Type()) {
	res = (MCapsCrp*) this;
    }
    return res;
}

Allocation& CapsCrp::SectAlloc(TSectId aSetId)
{
    return aSetId == ESI_Left ? iLeftSectAlc : iRightSectAlc;
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
    if (aSet) {
	set_state(Gtk::STATE_PRELIGHT);
    }
    else {
	set_state(Gtk::STATE_NORMAL);
    }
}

void CapsCrp::SetErroneous(bool aSet)
{
}

MElem* CapsCrp::Model()
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
    aReq->width = KLAreaMinWidth + 2*KBoundCompGapWidth; 
    aReq->height = KViewCompGapHight;
}

bool CapsCrp::on_expose_event(GdkEventExpose* aEvent)
{
    Glib::RefPtr<Gdk::Window> drw = get_window();
    Glib::RefPtr<Gtk::Style> style = get_style(); 	
    Glib::RefPtr<Gdk::GC> bgc = style->get_bg_gc(get_state());
    Glib::RefPtr<Gdk::GC> gc = style->get_fg_gc(get_state());

    //Gtk::Allocation alc = get_allocation();
    //drw->draw_rectangle(bgc, true, alc.get_x(), alc.get_y(), alc.get_width() - 1, alc.get_height() - 1);
    //drw->draw_rectangle(gc, false, alc.get_x(), alc.get_y(), alc.get_width() - 1, alc.get_height() - 1);
    Gtk::Allocation alc = SectAlloc(ESI_Left);
    drw->draw_rectangle(bgc, true, alc.get_x(), alc.get_y(), alc.get_width() - 1, alc.get_height() - 1);
    drw->draw_rectangle(gc, false, alc.get_x(), alc.get_y(), alc.get_width() - 1, alc.get_height() - 1);
    alc = SectAlloc(ESI_Right);
    drw->draw_rectangle(bgc, true, alc.get_x(), alc.get_y(), alc.get_width() - 1, alc.get_height() - 1);
    drw->draw_rectangle(gc, false, alc.get_x(), alc.get_y(), alc.get_width() - 1, alc.get_height() - 1);
}

void CapsCrp::SetDnDTargSupported(int aTarg)
{
    iDnDSupp = aTarg;
}

bool CapsCrp::IsDnDTargSupported(TDnDTarg aTarg) const
{
    return aTarg & iDnDSupp;
}

bool CapsCrp::on_button_press_event(GdkEventButton* aEvent)
{
    bool res = false;
    //std::cout << "EdgeCrp on_button_press_event [" << get_name() << "]"  << std::endl;
    Gtk::Allocation alc = get_allocation();
    int ex = aEvent->x;
    int ey = aEvent->y;
    bool isin = IsPointIn(aEvent->x, aEvent->y);
    std::cout << "CapsCrp::on_button_press_event, isin: " << isin << std::endl;
    if (isin) {
	//std::cout << "EdgeCrp iSigButtonPress emit "  << std::endl;
	iSigButtonPress.emit(aEvent);
	res = true;
    }
    return res;
}

void CapsCrp::GetAlloc(Allocation& aAllc) const
{
    int x = iLeftSectAlc.get_x();
    int y = iLeftSectAlc.get_y();
    int w = iRightSectAlc.has_zero_area() ? iLeftSectAlc.get_width() : iRightSectAlc.get_x() + iRightSectAlc.get_width() - x;
    int h = max(iLeftSectAlc.get_height(), iRightSectAlc.get_height());
    aAllc.set_x(x);
    aAllc.set_y(y);
    aAllc.set_width(w);
    aAllc.set_height(h);
}

bool CapsCrp::IsIntersected(int aX, int aY) const
{
    return IsPointIn(aX, aY);
}

bool CapsCrp::IsPointIn(int aX, int aY) const
{
    // Get region of drawable part of edge
    return IsInRect(aX, aY, iLeftSectAlc) || IsInRect(aX, aY, iRightSectAlc);
}

bool CapsCrp::IsInRect(int aX, int aY, const Allocation& aAllc) 
{
    int x = aAllc.get_x(); int y = aAllc.get_y(); 
    return (aX >= x && aX < x + aAllc.get_width() && aY >= y && aY < y + aAllc.get_height());
}


const string IncapsDrp::KCapsUri = "./Capsule";
const string IncapsDrp::KIncapsType = Syst::PEType() + ":Incaps";
const string IncapsDrp::KExtdType = "Elem:Vert:Extender";

string IncapsDrp::EType()
{
    return ":Elem:Vert:Syst:Incaps";
}

IncapsDrp::IncapsDrp(MElem* aElem, const MCrpProvider& aCrpProv, MSEnv& aStEnv): SysDrp(aElem, aCrpProv, aStEnv)
{
}

IncapsDrp::~IncapsDrp()
{
}

void IncapsDrp::Construct()
{
    MElem* caps = Model()->GetNode(KCapsUri);
    // Add Caps CRP
    mCapsLCrp = new CapsCrp(caps);
    mCapsLCrp->SetLArea(MCrp::EOverlay);
    mCapsLCrp->SetDnDTargSupported(EDT_Node);
    mCapsLCrp->SignalButtonPress().connect(sigc::bind<MElem*>(sigc::mem_fun(*this, &ElemDetRp::on_comp_button_press), mCapsLCrp->Model()));
    Gtk::Widget& capscrpw = mCapsLCrp->Widget();
    add(capscrpw);
    iCompRps[caps] = mCapsLCrp;
    //capscrpw.set_receives_default(false);
    capscrpw.show();
    // Add CRPs from body
    assert(caps != NULL);
    for (std::vector<MElem*>::iterator it = iElem->Comps().begin(); it != iElem->Comps().end(); it++) {
	MElem* comp = *it;
	if (comp->IsRemoved()) continue;
	assert(comp != NULL);
	if (comp != caps) {
	    MCrp* rp = iCrpProv.CreateRp(*comp, this);
	    Gtk::Widget& rpw = rp->Widget();
	    // Using specific signal for button press instead of standard because some Crps can have complex layout
	    rp->SignalButtonPress().connect(sigc::bind<MElem*>(sigc::mem_fun(*this, &ElemDetRp::on_comp_button_press), comp));
	    rp->SignalButtonPressName().connect(sigc::bind<MElem*>(sigc::mem_fun(*this, &ElemDetRp::on_comp_button_press_name), comp));
	    add(rpw);
	    iCompRps[comp] = rp;
	    rpw.show();
	}
    }
    // Adding CRPs from Capsule
    // TODO Using Comps is not legal because it doesnt mask removed comps. To consider hide Comps and use uri based iterator here.
    for (std::vector<MElem*>::iterator it = caps->Comps().begin(); it != caps->Comps().end(); it++) {
	MElem* comp = *it;
	if (comp->IsRemoved()) continue;
	assert(comp != NULL);
	MCrp* rp = iCrpProv.CreateRp(*comp, this);
	Gtk::Widget& rpw = rp->Widget();
	rp->SignalButtonPress().connect(sigc::bind<MElem*>(sigc::mem_fun(*this, &ElemDetRp::on_comp_button_press), comp));
	rp->SignalButtonPressName().connect(sigc::bind<MElem*>(sigc::mem_fun(*this, &ElemDetRp::on_comp_button_press_name), comp));
	add(rpw);
	iCompRps[comp] = rp;
	rpw.show();
    }
    UpdateRpsRelatios();
    PreLayoutRps();
}

void IncapsDrp::PreLayoutRps()
{
    // Pre-layout the components
    // There are two phase of pre-layouting: allocating to areas, ordering in areas
    // Scheme of areas is selected by the user, the default one is 3-areas scheme: inp, body, out
    iLaPars.resize(iLaNum);
    for (TLAreasPars::iterator zit = iLaPars.begin(); zit != iLaPars.end(); zit++) {
	zit->first = Allocation();
	zit->second.clear(); 
    }
    // Phase 1: allocating RPs to areas
    // Inputs and outputs - only from Capsule
    MElem* caps = iElem->GetNode("./Capsule");
    for (vector<MElem*>::const_iterator it = caps->Comps().begin(); it != caps->Comps().end(); it++) {
	MElem* comp = *it;
	if (comp->IsRemoved()) continue;
	MCrp* crp = iCompRps.at(comp);
	MEdgeCrp* ecrp = crp->GetObj(ecrp);
	if (ecrp == NULL) {
	    MElem* mdl = crp->Model();
	    MCrp::TLArea larea = MCrp::EMain;
	    //MCompatChecker* cc = mdl->GetObj(cc);
	    MCompatChecker* cc = (MCompatChecker*) mdl->GetSIfiC(MCompatChecker::Type());
	    if (cc != NULL) {
		MCompatChecker::TDir dir =  cc->GetDir();
		larea = dir == MCompatChecker::EInp ? MCrp::ELeft : MCrp::ERight;
	    }
	    int narea = larea == MCrp::ELeft ? 0 : iLaNum - 1;
	    TLAreaPar& area = iLaPars.at(narea);
	    crp->SetLArea(larea);
	    area.second.push_back(crp);
	}
    }
    // Sorting inp/out area
    Cmp cmp(*this);
    TVectCrps& crps_out = iLaPars.at(iLaNum - 1).second;;
    crps_out.sort(cmp);
    // Body
    BodyPreLayoutAut();
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
    // Left part
    TLAreaPar& area = iLaPars.at(0);
    Allocation& allc = area.first;
    MCapsCrp* mccrp = mCapsLCrp->GetObj(mccrp);
    Allocation& lallc = mccrp->SectAlloc(MCapsCrp::ESI_Left);
    lallc.set_x(aAllc.get_x());
    lallc.set_y(aAllc.get_y());
    lallc.set_width(allc.get_x() + allc.get_width() + KCrpPadding);
    lallc.set_height(aAllc.get_height());
    // Right part
    area = iLaPars.at(iLaPars.size() - 1);
    allc = area.first;
    Allocation& rallc = mccrp->SectAlloc(MCapsCrp::ESI_Right);
    rallc.set_x(allc.get_x() - KCrpPadding);
    rallc.set_y(aAllc.get_y());
    rallc.set_width(allc.get_width() + KCrpPadding);
    rallc.set_height(aAllc.get_height());

    Allocation finalc;
    mccrp->GetAlloc(finalc);
    mCapsLCrp->Widget().size_allocate(finalc);
}

void IncapsDrp::on_size_request(Gtk::Requisition* aRequisition)
{
    SysDrp::on_size_request(aRequisition);
}

MElem* IncapsDrp::GetCompOwning(MElem* aElem)
{
    MElem* res = NULL;
    MElem* caps = iElem->GetNode(KCapsUri);
    for (std::vector<MElem*>::iterator it = iElem->Comps().begin(); it != iElem->Comps().end() && res == NULL; it++) {
	MElem* comp = *it;
	if (comp->IsRemoved()) continue;
	if (aElem == comp || comp != caps && comp->IsComp(aElem)) {
	    res = comp;
	}
    }
    if (res == NULL) {
	for (std::vector<MElem*>::iterator it = caps->Comps().begin(); it != caps->Comps().end() && res == NULL; it++) {
	    MElem* comp = *it;
	    if (comp->IsRemoved()) continue;
	    if (aElem == comp || comp->IsComp(aElem)) {
		res = comp;
	    }
	}
    }
    return res;
}

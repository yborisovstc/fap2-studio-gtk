
#include <incaps.h>

#include "common.h"
#include "incapsdrp.h"


const string IncapsDrp::KCapsUri = "Capsule";
const string IncapsDrp::KIncapsType = Syst::PEType() + ":Incaps";
const string IncapsDrp::KExtdType = "Elem:Vert:Extender";

string IncapsDrp::EType()
{
    return ":Elem:Vert:Syst:Incaps";
}

IncapsDrp::IncapsDrp(Elem* aElem, const MCrpProvider& aCrpProv): SysDrp(aElem, aCrpProv)
{
}

IncapsDrp::~IncapsDrp()
{
}

void IncapsDrp::Construct()
{
    // Add CRPs from body
    Elem* caps = Model()->GetNode(KCapsUri);
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

    // Pre-layout the components
    iLaPars.resize(3);
    for (TLAreasPars::iterator zit = iLaPars.begin(); zit != iLaPars.end(); zit++) {
	zit->first = Allocation();
	zit->second.clear(); 
    }
    for (tCrps::iterator it = iCompRps.begin(); it != iCompRps.end(); it++) {
	MCrp* crp = it->second;
	MEdgeCrp* ecrp = crp->GetObj(ecrp);
	if (ecrp == NULL) {
	    Elem* ecmp = crp->Model();
	    MCompatChecker* cc = ecmp->GetObj(cc);
	    int narea = 1;
	    MCrp::TLArea larea = MCrp::EMain;
	    if (cc != NULL) {
		MCompatChecker::TDir dir =  cc->GetDir();
		narea = dir == MCompatChecker::EInp ? 0 : 2;
		larea = dir == MCompatChecker::EInp ? MCrp::ELeft : MCrp::ERight;
	    }
	    TLAreaPar& area = iLaPars.at(narea);
	    crp->SetLArea(larea);
	    area.second.push_back(crp);
	}
    }
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
}

void IncapsDrp::on_size_request(Gtk::Requisition* aRequisition)
{
    SysDrp::on_size_request(aRequisition);
}

Elem* IncapsDrp::GetCompOwning(Elem* aElem)
{
    Elem* res = NULL;
    Elem* caps = iElem->GetNode("Capsule");
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

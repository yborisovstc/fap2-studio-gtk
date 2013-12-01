#include <syst.h>
#include "incapscrp.h"
#include "common.h"

const string IncapsCrp::KCapsUri = "Elem:Capsule";

const string sCrpType = "IncapsCrp";

const string& IncapsCrp::Type()
{
    return sCrpType;
}

string IncapsCrp::EType()
{
    return Syst::PEType() + ":Incaps";
}

IncapsCrp::IncapsCrp(Elem* aElem): SysCrp(aElem)
{
}

IncapsCrp::~IncapsCrp()
{
}

void IncapsCrp::Construct()
{
    // Add CPs
    Elem* caps = iElem->GetNode(KCapsUri);
    assert(caps != NULL);
    for (std::vector<Elem*>::iterator it = caps->Comps().begin(); it != caps->Comps().end(); it++) {
	Elem* comp = *it;
	if (comp->IsHeirOf(ConnPointBase::PEType()) || comp->IsHeirOf("Vert:Extender")) {
	    CpRp* rp = new CpRp(comp);
	    add(*rp);
	    rp->show();
	    iCpRps[comp] = rp;
	}
    }
}

void *IncapsCrp::DoGetObj(const string& aName)
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



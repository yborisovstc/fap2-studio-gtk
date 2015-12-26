#include <syst.h>
#include "incapscrp.h"
#include "common.h"

const string IncapsCrp::KCapsUri = "./Capsule";

const string sCrpType = "IncapsCrp";

const string& IncapsCrp::Type()
{
    return sCrpType;
}

string IncapsCrp::EType()
{
    return Syst::PEType() + ":Incaps";
}

IncapsCrp::IncapsCrp(MElem* aElem, MMdlObserver* aMdlObs, const string& aDataUri): SysCrp(aElem, aMdlObs, aDataUri)
{
}

IncapsCrp::~IncapsCrp()
{
}

void IncapsCrp::Construct()
{
    ElemCompRp::Construct();
    // Add CPs
    MElem* caps = iElem->GetNode(KCapsUri);
    assert(caps != NULL);
    for (std::vector<MElem*>::iterator it = caps->Comps().begin(); it != caps->Comps().end(); it++) {
	MElem* comp = *it;
	CpRp* rp = new CpRp(comp);
	add(*rp);
	rp->show();
	iCpRps[comp] = rp;
    }
    set_has_tooltip();
    //set_tooltip_text("IncapsCrp Tooltip test");
    // Add Data Crp
    AddDataRp();
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



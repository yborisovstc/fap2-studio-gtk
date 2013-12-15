#include <elem.h>
#include "provdef.h"
#include "elemdetrp.h"
#include "elemcomprp.h"
#include "edgecrp.h"
#include "vertdrp.h"
#include "vertcrp.h"
#include "sysdrp.h"
#include "incapsdrp.h"
#include "syscrp.h"
#include "incapscrp.h"
#include "datacrp.h"
#include "cpcrp.h"
#include "extdcrp.h"
#include "propcrp.h"
#include "mcrp.h"

DefDrpProv::DefDrpProv(): iSenv(NULL)
{
}

DefDrpProv::~DefDrpProv()
{
}

void DefDrpProv::SetSenv(MSEnv& aEnv)
{
    iSenv = &aEnv;
}

int DefDrpProv::GetConfidence(const Elem& aElem) const
{
    int res = 0;
    bool fit = false;
    fit |= aElem.IsHeirOf(ElemDrp::EType());
    res = fit ? 1 : 0;
    return res;
}

MDrp* DefDrpProv::CreateRp(Elem& aElem) const
{
    MDrp* res = NULL;
    if (aElem.IsHeirOf(IncapsDrp::EType())) {
	res = new IncapsDrp(&aElem, iSenv->CrpProvider());
    }
    else if (aElem.IsHeirOf(SysDrp::EType())) {
	res = new SysDrp(&aElem, iSenv->CrpProvider());
    }
    else if (aElem.IsHeirOf(VertDrpw_v1::EType())) {
	res = new VertDrpw_v1(&aElem, iSenv->CrpProvider());
    }
    else if (aElem.IsHeirOf(ElemDrp::EType())) {
	res = new ElemDrp(&aElem, iSenv->CrpProvider());
    }
    return res;
}


const string KStateEType = "Incaps:State";
const string KStateDataUri = "Confirmed/Value";

DefCrpProv::DefCrpProv(MMdlObserver* aMdlObs): iSenv(NULL), iMdlObs(aMdlObs)
{
}

DefCrpProv::~DefCrpProv()
{
}

void DefCrpProv::SetSenv(MSEnv& aEnv)
{
    iSenv = &aEnv;
}

int DefCrpProv::GetConfidence(const Elem& aElem) const
{
    int res = 0;
    bool fit = false;
    fit |= aElem.IsHeirOf(ElemDrp::EType());
    res = fit ? 1 : 0;
    return res;
}

MCrp* DefCrpProv::CreateRp(Elem& aElem, const MCrpMgr* aMgr) const
{
    MCrp* res = NULL;
    if (aElem.IsHeirOf(DataCrp::EType()) && aMgr->IsTypeAllowed(DataCrp::EType())) {
	res = new DataCrp(&aElem, iMdlObs);
    }
    else if (aElem.IsHeirOf(KStateEType) && aMgr->IsTypeAllowed(IncapsCrp::EType())) {
	res = new IncapsCrp(&aElem, iMdlObs, KStateDataUri);
    }
    else if (aElem.IsHeirOf(IncapsCrp::EType()) && aMgr->IsTypeAllowed(IncapsCrp::EType())) {
	res = new IncapsCrp(&aElem, iMdlObs);
    }
    else if (aElem.IsHeirOf(SysCrp::EType()) && aMgr->IsTypeAllowed(SysCrp::EType())) {
	res = new SysCrp(&aElem, iMdlObs);
    }
    else if (aElem.IsHeirOf(ExtdCrp::EType()) && aMgr->IsTypeAllowed(ExtdCrp::EType())) {
	res = new ExtdCrp(&aElem);
    }
    else if (aElem.IsHeirOf(CpCrp::EType()) && aMgr->IsTypeAllowed(CpCrp::EType())) {
	res = new CpCrp(&aElem);
    }
    else if (aElem.IsHeirOf(VertCompRp::EType()) && aMgr->IsTypeAllowed(VertCompRp::EType())) {
	res = new VertCompRp(&aElem);
    }
    else if (aElem.IsHeirOf(EdgeCrp::EType()) && aMgr->IsTypeAllowed(EdgeCrp::EType())) {
	res = new EdgeCrp(&aElem);
    }
    else if (aElem.IsHeirOf(PropCrp::EType()) && aMgr->IsTypeAllowed(PropCrp::EType())) {
	res = new PropCrp(&aElem);
    }
    else if (aElem.IsHeirOf(ElemCrp::EType()) && aMgr->IsTypeAllowed(ElemCrp::EType())) {
	res = new ElemCrp(&aElem);
    }
    return res;
}

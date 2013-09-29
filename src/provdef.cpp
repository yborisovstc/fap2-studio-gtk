#include <elem.h>
#include "provdef.h"
#include "elemdetrp.h"
#include "elemcomprp.h"
#include "edgecrp.h"
#include "vertdrp.h"
#include "vertcrp.h"
#include "sysdrp.h"
#include "syscrp.h"
#include "cpcrp.h"
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
    if (aElem.IsHeirOf(SysDrp::EType())) {
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



DefCrpProv::DefCrpProv(): iSenv(NULL)
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
    if (aElem.IsHeirOf(SysCrp::EType()) && aMgr->IsTypeAllowed(SysCrp::EType())) {
	res = new SysCrp(&aElem);
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
    else if (aElem.IsHeirOf(ElemCrp::EType()) && aMgr->IsTypeAllowed(ElemCrp::EType())) {
	res = new ElemCrp(&aElem);
    }
    return res;
}

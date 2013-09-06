#include <elem.h>
#include "provdef.h"
#include "elemdetrp.h"
#include "elemcomprp.h"
#include "edgecrp.h"
#include "vertdrp.h"
#include "vertcrp.h"
#include "sysdrp.h"
#include "syscrp.h"
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
    else if (aElem.IsHeirOf(VertDrp::EType())) {
	res = new VertDrp(&aElem, iSenv->CrpProvider());
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

MCrp* DefCrpProv::CreateRp(Elem& aElem) const
{
    MCrp* res = NULL;
    if (aElem.IsHeirOf(SysCrp::EType())) {
	res = new SysCrp(&aElem);
    }
    else if (aElem.IsHeirOf(VertCrp::EType())) {
	res = new VertCrp(&aElem);
    }
    else if (aElem.IsHeirOf(EdgeCrp::EType())) {
	res = new EdgeCrp(&aElem);
    }
    else if (aElem.IsHeirOf(ElemCrp::EType())) {
	res = new ElemCrp(&aElem);
    }
    return res;
}

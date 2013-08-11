#include <elem.h>
#include "provdef.h"
#include "elemdetrp.h"
#include "elemcomprp.h"
#include "vertdrp.h"
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
    if (aElem.IsHeirOf(VertDrp::EType())) {
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
    if (aElem.IsHeirOf(ElemCrp::EType())) {
	res = new ElemCrp(&aElem);
    }
    return res;
}

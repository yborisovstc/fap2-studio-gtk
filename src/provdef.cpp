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
#include "cperp.h"


const string KEType_Aedge = ":Elem:Aedge";
const string KEType_ExtenderMc = ":Elem:Vert:ExtenderMc";

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

int DefDrpProv::GetConfidence(const MElem& aElem) const
{
    int res = 0;
    bool fit = false;
    fit |= aElem.IsHeirOf(ElemDrp::EType());
    res = fit ? 1 : 0;
    return res;
}

MDrp* DefDrpProv::CreateRp(MElem& aElem) const
{
    MDrp* res = NULL;
    if (aElem.IsHeirOf(IncapsDrp::EType())) {
	res = new IncapsDrp(&aElem, iSenv->CrpProvider(), *iSenv);
    }
    else if (aElem.IsHeirOf(SysDrp::EType())) {
	res = new SysDrp(&aElem, iSenv->CrpProvider(), *iSenv);
    }
    else if (aElem.IsHeirOf(VertDrpw_v1::EType())) {
	res = new VertDrpw_v1(&aElem, iSenv->CrpProvider(), *iSenv);
    }
    else if (aElem.IsHeirOf(ElemDrp::EType()) || aElem.EType().empty() && aElem.Name() == Elem::Type()) {
	res = new ElemDrp(&aElem, iSenv->CrpProvider(), *iSenv);
    }
    else {
	res = new ElemDrp(&aElem, iSenv->CrpProvider(), *iSenv);
    }
    return res;
}


const string KStateEType = "Incaps:State";
const string KStateDataUri = "Confirmed/Value";
const string KTFuncIntEType = "Incaps:TFuncInt";
const string KTFuncIntDataUri = "./func_agt";
const string KDataSEType = "Incaps:DataS";
const string KDataSDataUri = "Value";

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

int DefCrpProv::GetConfidence(const MElem& aElem) const
{
    int res = 0;
    bool fit = false;
    fit |= aElem.IsHeirOf(ElemDrp::EType());
    res = fit ? 1 : 0;
    return res;
}

bool DefCrpProv::IsExtender(MElem& aModel)
{
    bool res = EFalse;
    MCompatChecker* mcc = aModel.GetObj(mcc);
    if (mcc != NULL) {
	res = (mcc->GetExtd() != NULL);
    }
    return res;
}

MCrp* DefCrpProv::CreateRp(MElem& aElem, const MCrpMgr* aMgr) const
{
    MCrp* res = NULL;
    if (aElem.IsHeirOf(DataCrp::EType()) && aMgr->IsTypeAllowed(DataCrp::EType())) {
	res = new DataCrp(&aElem, iMdlObs);
    }
    /*
    if (aElem.IsHeirOf(KDataSEType) && aMgr->IsTypeAllowed(IncapsCrp::EType())) {
	res = new IncapsCrp(&aElem, iMdlObs, KDataSDataUri);
    }
    */
    else if (aElem.IsHeirOf(KStateEType) && aMgr->IsTypeAllowed(IncapsCrp::EType())) {
	res = new IncapsCrp(&aElem, iMdlObs, KStateDataUri);
    }
    else if (aElem.IsHeirOf(KTFuncIntEType) && aMgr->IsTypeAllowed(IncapsCrp::EType())) {
	res = new IncapsCrp(&aElem, iMdlObs, KTFuncIntDataUri);
    }
    else if (aElem.IsHeirOf(IncapsCrp::EType()) && aMgr->IsTypeAllowed(IncapsCrp::EType())) {
	res = new IncapsCrp(&aElem, iMdlObs);
    }
    else if (aElem.IsHeirOf(SysCrp::EType()) && aMgr->IsTypeAllowed(SysCrp::EType())) {
	res = new SysCrp(&aElem, iMdlObs);
    }
    else if ((aElem.IsHeirOf(ExtdCrp::EType()) || IsExtender(aElem)) && aMgr->IsTypeAllowed(ExtdCrp::EType())) {
	res = new ExtdCrp(&aElem, iSenv->ErpProvider());
    }
    else if (aElem.IsHeirOf(CpCrp::EType()) && aMgr->IsTypeAllowed(CpCrp::EType())) {
	res = new CpCrp(&aElem);
    }
    else if (aElem.IsHeirOf(VertCompRp::EType()) && aMgr->IsTypeAllowed(VertCompRp::EType())) {
	res = new VertCompRp(&aElem);
    }
    else if ((aElem.IsHeirOf(EdgeCrp::EType()) || aElem.IsHeirOf(KEType_Aedge)) && aMgr->IsTypeAllowed(EdgeCrp::EType())) {
	res = new EdgeCrp(&aElem);
    }
    else if (aElem.IsHeirOf(PropCrp::EType()) && aMgr->IsTypeAllowed(PropCrp::EType())) {
	res = new PropCrp(&aElem);
    }
    else if (aElem.IsHeirOf(ElemCrp::EType()) && aMgr->IsTypeAllowed(ElemCrp::EType())) {
	res = new ElemCrp(&aElem);
    }
    else {
	res = new ElemCrp(&aElem);
    }
    return res;
}

// Default Erp provider

DefErpProv::DefErpProv(MMdlObserver* aMdlObs): iSenv(NULL), iMdlObs(aMdlObs)
{
}

DefErpProv::~DefErpProv()
{
}

void DefErpProv::SetSenv(MSEnv& aEnv)
{
    iSenv = &aEnv;
}

int DefErpProv::GetConfidence(const MElem& aElem) const
{
    int res = 0;
    return res;
}

MErp* DefErpProv::CreateRp(MElem& aElem, const MErpMgr* aMgr) const
{
    MErp* res = NULL;
    if ((aElem.GetObj(CpErp::IfaceSupported().c_str()) != NULL)  && aMgr->IsTypeAllowed(CpErp::EType())) {
    //if ((aElem.IsHeirOf(CpErp::EType()) || aElem.IsHeirOf("Elem:Vert:ConnPointMc")) && aMgr->IsTypeAllowed(CpErp::EType())) {
	res = new CpErp(&aElem);
    }
    //else if ((aElem.GetSIfi(SockErp::IfaceSupported().c_str()) != NULL)  && aMgr->IsTypeAllowed(SockErp::EType())) {
    else if ((aElem.GetObj(SockErp::IfaceSupported().c_str()) != NULL)  && aMgr->IsTypeAllowed(SockErp::EType())) {
	res = new SockErp(&aElem, *this);
    }
    return res;
}

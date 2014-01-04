#include "stenv.h"
#include "provdef.h"

StEnv::StEnv(MMdlObserver* aMdlObs): iMdlObs(aMdlObs)
{
    iCrpProv = new DefCrpProv(iMdlObs);
    iCrpProv->SetSenv(*this);
    iDrpProv = new DefDrpProv();
    iDrpProv->SetSenv(*this);
    iErpProv = new DefErpProv(iMdlObs);
    iErpProv->SetSenv(*this);
}

StEnv::~StEnv()
{
    delete iCrpProv;
}

MCrpProvider& StEnv::CrpProvider()
{
    return *iCrpProv;
}

MDrpProvider& StEnv::DrpProvider()
{
    return *iDrpProv;
}

MErpProvider& StEnv::ErpProvider()
{
    return *iErpProv;
}

StDesEnv::StDesEnv(const Glib::RefPtr<UIManager>& aUiMgr): iUiMgr(aUiMgr)
{
}

StDesEnv::~StDesEnv()
{
}

Glib::RefPtr<UIManager> StDesEnv::UiMgr()
{
    return iUiMgr;
}


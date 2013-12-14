#include "stenv.h"
#include "provdef.h"

StEnv::StEnv()
{
    iCrpProv = new DefCrpProv();
    iCrpProv->SetSenv(*this);
    iDrpProv = new DefDrpProv();
    iDrpProv->SetSenv(*this);
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


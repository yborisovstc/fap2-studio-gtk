#include "stenv.h"
#include "provdef.h"

// Setting

template <typename T> const T& StSetting<T>::Get(const T& aSetting) const
{
    return mData;
}

template <typename T> void StSetting<T>::Set(const T& aSetting)
{
    if (aSetting != mData) {
	mData = aSetting;
	mSigChanged.emit();
    }
}

StSettings::StSettings() 
{
    mChromoLim.Set(0);
}

StSettings::~StSettings()
{
}

void* StSettings::DoGetSetting(TStSett aSettId)
{
    void* res = NULL;
    switch (aSettId) {
	case ESts_EnablePhenoModif: res =  &mEnablePhenoModif;
	case ESts_ChromoLim: res =  &mChromoLim;
    }
    return res;
}

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

StDesEnv::StDesEnv(const Glib::RefPtr<UIManager>& aUiMgr, Container& aVisWindow): iUiMgr(aUiMgr), iVisWindow(aVisWindow)
{
}

StDesEnv::~StDesEnv()
{
}

Glib::RefPtr<UIManager> StDesEnv::UiMgr()
{
    return iUiMgr;
}

MSDesEnv::tSigActionRecreate StDesEnv::SigActionRecreate()
{
    return iSigActionRecreate;
}

Container& StDesEnv::VisWindow()
{
    return iVisWindow;
}


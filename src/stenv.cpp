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
    switch (aSettId) {
	case ESts_EnablePhenoModif: return  &mEnablePhenoModif;
	case ESts_PinnedMutNode: return  &mPinnedMutNode; 
	case ESts_ModulesPath: return  &mStModulesPath; 
	case ESts_ChromoLim: return  &mChromoLim;
	default: return NULL;
    }
}

StEnv::StEnv(MMdlObserver* aMdlObs, MDesLog* aDesLog): iMdlObs(aMdlObs), mDesLog(aDesLog)
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


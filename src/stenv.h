
#ifndef __FAP2STU_GTK_STENV_H
#define __FAP2STU_GTK_STENV_H

// Studio environment

#include "msenv.h"
#include "msdesenv.h"
#include "provdef.h"
#include "mdesobs.h"
#include "msset.h"
#include <gtkmm.h>

using namespace Gtk;

template <typename T> class StSetting: public MStSetting<T>
{
    public:
	StSetting() {};
	typedef sigc::signal<void> tSigChanged;
	virtual const T& Get(const T& aSetting) const;
	virtual void Set(const T& aSetting);
	virtual tSigChanged SigChanged() { return mSigChanged; };
    protected:
	T mData;
	tSigChanged mSigChanged;
};

class StSettings: public MStSettings
{
    public:
	StSettings();
	virtual ~StSettings();
    public:
	// From MStSettings
	virtual void* DoGetSetting(TStSett aSettId);
    protected:
	 StSetting<bool> mEnablePhenoModif;
	 // Enable mutation even if broken critical deps are detected
	 StSetting<bool> mEnableMutWithCritDep;
	 StSetting<Glib::ustring> mPinnedMutNode;
	 StSetting<int> mChromoLim;
	 StSetting<Glib::ustring> mStModulesPath;
	 // Disable processing of optimization data
	 StSetting<bool> mDisableOpt;
};

class StEnv: public MSEnv
{
    public:
	StEnv(MMdlObserver* aMdlObs, MDesLog* aDesLog);
	virtual ~StEnv();
	// From MSEnv
	virtual MCrpProvider& CrpProvider();
	virtual MDrpProvider& DrpProvider();
	virtual MErpProvider& ErpProvider();
	virtual MStSettings& Settings() { return mSettings;};
	virtual MDesLog& DesLog() { return *mDesLog;};
    private:
	MMdlObserver* iMdlObs;
	// Compact representaions provider
	// TODO [YB] To replace default provider with full provider
	DefCrpProv* iCrpProv;
	DefDrpProv* iDrpProv;
	DefErpProv* iErpProv;
	StSettings mSettings;
	MDesLog* mDesLog;
};

// Studio DES Environment

class StDesEnv: public MSDesEnv
{
    public:
	StDesEnv(const Glib::RefPtr<UIManager>& aUiMgr, Container& aVisWindow);
	virtual ~StDesEnv();
	// From MSDesEnv
	virtual Glib::RefPtr<UIManager> UiMgr();
	virtual Container& VisWindow();
	virtual tSigActionRecreate SigActionRecreate();
    private:
	Glib::RefPtr<UIManager> iUiMgr;
	tSigActionRecreate iSigActionRecreate;
	Container& iVisWindow;
};


#endif


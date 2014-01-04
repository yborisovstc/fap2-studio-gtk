
#ifndef __FAP2STU_GTK_STENV_H
#define __FAP2STU_GTK_STENV_H

// Studio environment

#include "msenv.h"
#include "msdesenv.h"
#include "provdef.h"
#include "mdesobs.h"

using namespace Gtk;

class StEnv: public MSEnv
{
    public:
	StEnv(MMdlObserver* aMdlObs);
	virtual ~StEnv();
	// From MSEnv
	virtual MCrpProvider& CrpProvider();
	virtual MDrpProvider& DrpProvider();
	virtual MErpProvider& ErpProvider();
    private:
	MMdlObserver* iMdlObs;
	// Compact representaions provider
	// TODO [YB] To replace default provider with full provider
	DefCrpProv* iCrpProv;
	DefDrpProv* iDrpProv;
	DefErpProv* iErpProv;
};

// Studio DES Environment

class StDesEnv: public MSDesEnv
{
    public:
	StDesEnv(const Glib::RefPtr<UIManager>& aUiMgr);
	virtual ~StDesEnv();
	// From MSDesEnv
	virtual Glib::RefPtr<UIManager> UiMgr();
    private:
	Glib::RefPtr<UIManager> iUiMgr;
};


#endif


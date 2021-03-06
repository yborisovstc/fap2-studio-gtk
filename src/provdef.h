
#ifndef __FAP2STU_GTK_PROVDEF_H
#define __FAP2STU_GTK_PROVDEF_H

// Default providers

#include "mprov.h"
#include "mdesobs.h"

class DefDrpProv: public MDrpProvider
{
    public:
	DefDrpProv();
	virtual ~DefDrpProv();
	//  From MDrpProvider
	virtual void SetSenv(MSEnv& aEnv);
	virtual MDrp* CreateRp(MElem& aElem) const;
	virtual int GetConfidence(const MElem& aElem) const;
    private:
	MSEnv* iSenv;
};


class DefCrpProv: public MCrpProvider
{
    public:
	DefCrpProv(MMdlObserver* aMdlObs);
	virtual ~DefCrpProv();
	//  From MCrpProvider
	virtual void SetSenv(MSEnv& aEnv);
	virtual MCrp* CreateRp(MElem& aElem, const MCrpMgr* aMgr) const;
	virtual int GetConfidence(const MElem& aElem) const;
    protected:
	static bool IsExtender(MElem& aModel);
    private:
	MMdlObserver* iMdlObs;
	MSEnv* iSenv;
};

class DefErpProv: public MErpProvider
{
    public:
	DefErpProv(MMdlObserver* aMdlObs);
	virtual ~DefErpProv();
	//  From MErpProvider
	virtual void SetSenv(MSEnv& aEnv);
	virtual MErp* CreateRp(MElem& aElem, const MErpMgr* aMgr) const;
	virtual int GetConfidence(const MElem& aElem) const;
    private:
	MMdlObserver* iMdlObs;
	MSEnv* iSenv;
};

#endif

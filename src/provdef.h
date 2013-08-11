
#ifndef __FAP2STU_GTK_PROVDEF_H
#define __FAP2STU_GTK_PROVDEF_H

// Default providers

#include "mprov.h"

class DefDrpProv: public MDrpProvider
{
    public:
	DefDrpProv();
	virtual ~DefDrpProv();
	//  From MDrpProvider
	virtual void SetSenv(MSEnv& aEnv);
	virtual MDrp* CreateRp(Elem& aElem) const;
	virtual int GetConfidence(const Elem& aElem) const;
    private:
	MSEnv* iSenv;
};


class DefCrpProv: public MCrpProvider
{
    public:
	DefCrpProv();
	virtual ~DefCrpProv();
	//  From MCrpProvider
	virtual void SetSenv(MSEnv& aEnv);
	virtual MCrp* CreateRp(Elem& aElem) const;
	virtual int GetConfidence(const Elem& aElem) const;
    private:
	MSEnv* iSenv;
};

#endif

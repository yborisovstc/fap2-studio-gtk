
#ifndef __FAP2STU_GTK_STENV_H
#define __FAP2STU_GTK_STENV_H

// Studio environment

#include "msenv.h"
#include "provdef.h"

class StEnv: public MSEnv
{
    public:
	StEnv();
	virtual ~StEnv();
	// From MSEnv
	virtual MCrpProvider& CrpProvider();
    private:
	// Compact representaions provider
	// TODO [YB] To replace default provider with full provider
	DefCrpProv* iCrpProv;
};


#endif


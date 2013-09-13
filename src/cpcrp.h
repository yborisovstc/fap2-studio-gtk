
#ifndef __FAP2STU_CPCRP_H
#define __FAP2STU_CPCRP_H

#include "vertcrp.h"

class CpCrp: public VertCompRp
{
    public:
	static const string& Type();
	static string EType();
    public:
	CpCrp(Elem* aElem);
	virtual ~CpCrp();
    protected:
	// From MCrp
	virtual void *DoGetObj(const string& aName);
};

#endif

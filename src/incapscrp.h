
#ifndef __FAP2STU_INCAPSCRP_H
#define __FAP2STU_INCAPSCRP_H

#include <elem.h>
#include "mcrp.h"
#include "syscrp.h"

// Incapsulated system compact representation
class IncapsCrp: public SysCrp
{
    public:
	static const string& Type();
	static string EType();
    public:
	IncapsCrp(Elem* aElem);
	virtual ~IncapsCrp();
    protected:
	virtual void Construct();
	// From MCrp
	virtual void *DoGetObj(const string& aName);
    protected:
	static const string KCapsUri;
};

#endif


#ifndef __FAP2STU_GTK_MPROV_H
#define __FAP2STU_GTK_MPROV_H

// Providers interfaces

#include "msenv.h"
#include "mcrp.h"
#include "mdrp.h"

class Elem;

class MRpProvider 
{
    public:
	// Set studio environment
	virtual void SetSenv(MSEnv& aEnv) = 0;
	// Returns confidence level: 0-9
	virtual int GetConfidence(const Elem& aElem) const = 0;
};

// Detail representation provider
class MDrpProvider: public MRpProvider
{
    public:
	// Creates representation
	virtual MDrp* CreateRp(Elem& aElem) const = 0;
};

// Manager of CRp
class MCrpMgr
{
    public:
	// Verifies if CRp of given type is allowed
	virtual bool IsTypeAllowed(const std::string& aType) const = 0;
};

// Compact representation provider
class MCrpProvider: public MRpProvider
{
    public:
	// Creates representation
	virtual MCrp* CreateRp(Elem& aElem, const MCrpMgr* aMgr) const = 0;
};

#endif

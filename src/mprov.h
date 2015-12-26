
#ifndef __FAP2STU_GTK_MPROV_H
#define __FAP2STU_GTK_MPROV_H

// Providers interfaces

#include "msenv.h"
#include "mcrp.h"
#include "mdrp.h"
#include "merp.h"

class MElem;

class MRpProvider 
{
    public:
	// Set studio environment
	virtual void SetSenv(MSEnv& aEnv) = 0;
	// Returns confidence level: 0-9
	virtual int GetConfidence(const MElem& aElem) const = 0;
};

// Detail representation provider
class MDrpProvider: public MRpProvider
{
    public:
	// Creates representation
	virtual MDrp* CreateRp(MElem& aElem) const = 0;
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
	virtual MCrp* CreateRp(MElem& aElem, const MCrpMgr* aMgr) const = 0;
};

// Manager of ERp
class MErpMgr
{
    public:
	// Verifies if Rp of given type is allowed
	virtual bool IsTypeAllowed(const std::string& aType) const = 0;
};


// Embedded representation provider
class MErpProvider: public MRpProvider
{
    public:
	// Creates representation
	virtual MErp* CreateRp(MElem& aElem, const MErpMgr* aMgr) const = 0;
};

#endif

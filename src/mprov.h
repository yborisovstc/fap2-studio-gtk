
#ifndef __FAP2STU_GTK_MPROV_H
#define __FAP2STU_GTK_MPROV_H

// Providers interfaces

#include "mcrp.h"

class Elem;

class MRpProvider 
{
    public:
	// Returns confidence level: 0-9
	virtual int GetConfidence(const Elem& aElem) const = 0;
};

// Detail representation provider
class MDrpProvider: public MRpProvider
{
    public:
};

// Compact representation provider
class MCrpProvider: public MRpProvider
{
    public:
	// Creates representation
	virtual MCrp& CreateRp(const Elem& aElem) = 0;
};

#endif

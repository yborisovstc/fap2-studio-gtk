
#ifndef __FAP2STU_GTK_PROVDEF_H
#define __FAP2STU_GTK_PROVDEF_H

// Default providers

#include "mprov.h"

class DefCrpProv: public MCrpProvider
{
    public:
	DefCrpProv();
	virtual ~DefCrpProv();
	//  From MCrpProvider
	// Creates representation
	virtual MCrp& CreateRp(const Elem& aElem);
	virtual int GetConfidence(const Elem& aElem) const;
};

#endif

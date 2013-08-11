
#ifndef __FAP2STU_EDGECRP_H
#define __FAP2STU_EDGECRP_H

#include "mcrp.h"
#include <elem.h>

class EdgeCrp: public MCrp
{
    public:
	static string EType();
    public:
	EdgeCrp(Elem* aElem);
	virtual ~EdgeCrp();
	// From MCrp
	virtual Gtk::Widget& Widget();
    private:
	ElemCompRp* iRp;
};

#endif

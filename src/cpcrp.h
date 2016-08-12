
#ifndef __FAP2STU_CPCRP_H
#define __FAP2STU_CPCRP_H

#include "vertcrp.h"

class CpCrp: public VertCompRp
{
    public:
	static const string& Type();
	static string EType();
	static string IfaceSupported();
    public:
	CpCrp(MElem* aElem);
	virtual ~CpCrp();
    protected:
	// From VertCompRp
	virtual void on_size_allocate(Gtk::Allocation& 	aAlloc);
	virtual void on_size_request(Gtk::Requisition* aRequisition);
	// From MCrp
	virtual void *DoGetObj(const string& aName);
    protected:
	Gtk::Label* iLabProv;
	Gtk::Label* iLabReq;
};

#endif


#ifndef __FAP2STU_SYSDRP_H
#define __FAP2STU_SYSDRP_H

#include "vertdrp.h"

class SysDrp: public VertDrpw, public MDrp
{
    public:
	static std::string Type() { return string("SysDrp");};
    public:
	static string EType();
    public:
	SysDrp(Elem* aElem, const MCrpProvider& aCrpProv);
	virtual ~SysDrp();
    protected:
	// From MDrp
	virtual void *DoGetObj(const string& aName);
	virtual Gtk::Widget& Widget();
	virtual Elem* Model();
	virtual tSigCompSelected SignalCompSelected();
	//  From VertDrpw
	virtual void on_size_allocate(Gtk::Allocation& aAlloc);
	virtual void on_size_request(Gtk::Requisition* aRequisition);
};

#endif

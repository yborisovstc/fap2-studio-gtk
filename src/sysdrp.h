
#ifndef __FAP2STU_SYSDRP_H
#define __FAP2STU_SYSDRP_H

#include "vertdrp.h"

class SysDrp: public VertDrpw_v1
{
    public:
	static std::string Type() { return string("SysDrp");};
    public:
	static string EType();
    public:
	SysDrp(Elem* aElem, const MCrpProvider& aCrpProv);
	virtual ~SysDrp();
    protected:
	virtual void Construct();
	virtual bool AreCpsCompatible(Elem* aCp1, Elem* aCp2);
	// From MCrpMgr
	virtual bool IsTypeAllowed(const std::string& aType) const;
	// From MDrp
	virtual void *DoGetObj(const string& aName);
	virtual Gtk::Widget& Widget();
	virtual Elem* Model();
	virtual tSigCompSelected SignalCompSelected();
	//  From VertDrpw
	virtual void on_size_allocate(Gtk::Allocation& aAlloc);
	virtual void on_size_request(Gtk::Requisition* aRequisition);
    protected:
	static const string KCpEType;
	static const string KExtdEType;
};

#endif

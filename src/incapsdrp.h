
#ifndef __FAP2STU_INCAPSDRP_H
#define __FAP2STU_INCAPSDRP_H

#include "sysdrp.h"

class IncapsDrp: public SysDrp
{
    public:
	static std::string Type() { return string("IncapsDrp");};
    public:
	static string EType();
    public:
	IncapsDrp(Elem* aElem, const MCrpProvider& aCrpProv, MSEnv& aStEnv);
	virtual ~IncapsDrp();
    protected:
	virtual void Construct();
	TBool IsTypeOf(const string& aType, const string& aParent) const;
	virtual Elem* GetCompOwning(Elem* aElem);
	// From MCrpMgr
	virtual bool IsTypeAllowed(const std::string& aType) const;
	// From MDrp
	virtual void *DoGetObj(const string& aName);
	//  From SysDrp
	virtual void on_size_allocate(Gtk::Allocation& aAlloc);
	virtual void on_size_request(Gtk::Requisition* aRequisition);
    protected:
	static const string KCapsUri;
	static const string KIncapsType;
	static const string KExtdType;
};

#endif

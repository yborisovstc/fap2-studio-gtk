
#ifndef __FAP2STU_EXTDCRP_H
#define __FAP2STU_EXTDCRP_H

#include "cpcrp.h"
#include "merp.h"
#include "mprov.h"
#include <gtkmm/widget.h>

using namespace Gtk;

// Extender Crp
class ExtdCrp: public VertCompRp, public MErpMgr
{
    public:
	static const string& Type();
	static string EType();
    public:
	ExtdCrp(Elem* aElem, MErpProvider& aErpProv);
	virtual ~ExtdCrp();
    protected:
	// From VertCompRp
	virtual void on_size_allocate(Gtk::Allocation& 	aAlloc);
	virtual void on_size_request(Gtk::Requisition* aRequisition);
	// From MCrp
	virtual void *DoGetObj(const string& aName);
	// From MCrpConnectable
	virtual Gtk::Requisition GetCpCoord(Elem* aCp = NULL);
	virtual int GetNearestCp(Gtk::Requisition aCoord, Elem*& aCp);
	virtual void HighlightCp(Elem* aCp, bool aSet);
	// From MErpMgr
	virtual bool IsTypeAllowed(const std::string& aType) const;
    protected:
	MErpProvider& iErpProv;
	MErp* iInt;
	Gtk::Widget* iIntw;
	//Gtk::Label* iLabInt;
	Gtk::Label* iLabExt;
};

#endif
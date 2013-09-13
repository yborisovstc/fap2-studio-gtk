
#ifndef __FAP2STU_SYSCRP_H
#define __FAP2STU_SYSCRP_H

#include <gtkmm/layout.h>
#include <gtkmm/box.h>
#include "gtkmm/label.h"
#include <elem.h>
#include "mcrp.h"
#include "vertcrp.h"

//  Connection point representation
class CpRp: public Gtk::Label
{
    public:
	CpRp(Elem* aCp);
	virtual ~CpRp();
    protected:
	// From Layout
	virtual bool on_expose_event(GdkEventExpose* event);
	virtual void on_size_allocate(Gtk::Allocation& 	aAlloc);
	virtual void on_size_request(Gtk::Requisition* aRequisition);
    protected:
	Elem* iElem;
};

// System representation
class SysCrp: public VertCompRp
{
    public:
	static const string& Type();
	static string EType();
    public:
	SysCrp(Elem* aElem);
	virtual ~SysCrp();
    protected:
	// From MCrp
	virtual Gtk::Widget& Widget();
	virtual void *DoGetObj(const string& aName);
	// From MCrpConnectable
	virtual Gtk::Requisition GetCpCoord(Elem* aCp = NULL);
	// From Layout
	virtual bool on_expose_event(GdkEventExpose* event);
	virtual void on_size_allocate(Gtk::Allocation& 	aAlloc);
	virtual void on_size_request(Gtk::Requisition* aRequisition);
    protected:
	typedef std::map<Elem*, CpRp*> tCpRps;
	tCpRps iCpRps; // CPs representations
};

#endif

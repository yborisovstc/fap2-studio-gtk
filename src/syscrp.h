
#ifndef __FAP2STU_SYSCRP_H
#define __FAP2STU_SYSCRP_H

#include <gtkmm/layout.h>
#include <gtkmm/box.h>
#include "gtkmm/label.h"
#include <elem.h>
#include "mcrp.h"
#include "vertcrp.h"

class SysCrp: public VertCompRp, public MCrp, public MCrpConnectable
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
	virtual Gtk::Requisition GetCpCoord(CpType aCpType);
	// From Layout
	virtual bool on_expose_event(GdkEventExpose* event);
	virtual void on_size_allocate(Gtk::Allocation& 	aAlloc);
	virtual void on_size_request(Gtk::Requisition* aRequisition);
};

#endif

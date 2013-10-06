
#ifndef __FAP2STU_PROPCRP_H
#define __FAP2STU_PROPCRP_H

#include <gtkmm/textview.h>
#include "elemcomprp.h"

class PropCrp: public ElemCompRp, public MCrp
{
    public:
	static const string& Type();
	static string EType();
    public:
	PropCrp(Elem* aElem);
	virtual ~PropCrp();
    protected:
	virtual bool on_expose_event(GdkEventExpose* event);
	virtual void on_size_allocate(Gtk::Allocation& 	aAlloc);
	virtual void on_size_request(Gtk::Requisition* aRequisition);
	// From MCrp
	virtual Gtk::Widget& Widget();
	virtual void *DoGetObj(const string& aName);
	virtual tSigButtonPressName SignalButtonPressName();
	virtual tSigUpdated SignalUpdated();
	virtual bool IsActionSupported(Action aAction);
    protected:
	//Gtk::TextView iContent;
	Gtk::Label iContent;
};

#endif



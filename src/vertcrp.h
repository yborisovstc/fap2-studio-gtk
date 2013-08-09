
#ifndef __FAP2STU_VERTCRP_H
#define __FAP2STU_VERTCRP_H

#include <gtkmm/layout.h>
#include <gtkmm/box.h>
#include "gtkmm/label.h"
#include "gtkmm/drawingarea.h"
#include <elem.h>

class VertCrpHead: public Gtk::HBox
{
    public:
	VertCrpHead(const Elem& aElem);
	virtual ~VertCrpHead();
    protected:
	virtual bool on_expose_event(GdkEventExpose* event);
    private:
	const Elem& iElem;
	Gtk::Label* iName;
	Gtk::Label* iParent;
};

class VertCrp: public Gtk::Layout
{
    public:
	VertCrp(Elem* aElem);
	virtual ~VertCrp();
    protected:
	virtual bool on_expose_event(GdkEventExpose* event);
	virtual void on_size_allocate(Gtk::Allocation& 	aAlloc);
	virtual void on_size_request(Gtk::Requisition* aRequisition);
    private:
	Elem* iElem;
	VertCrpHead* iHead;
	Gtk::Allocation iBodyAlc;
};

#endif

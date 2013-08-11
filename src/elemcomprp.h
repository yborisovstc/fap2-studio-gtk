
#ifndef __FAP2STU_ELEMCOMPRP_H
#define __FAP2STU_ELEMCOMPRP_H

#include <gtkmm/layout.h>
#include <gtkmm/box.h>
#include "gtkmm/label.h"
#include "gtkmm/drawingarea.h"
#include "mcrp.h"
#include <elem.h>

class ElemCompHead: public Gtk::HBox
{
    public:
	ElemCompHead(const Elem& aElem);
	virtual ~ElemCompHead();
    protected:
	virtual bool on_expose_event(GdkEventExpose* event);
    private:
	const Elem& iElem;
	Gtk::Label* iName;
	Gtk::Label* iParent;
};

class ElemCompRp: public Gtk::Layout
{
    public:
	ElemCompRp(Elem* aElem);
	virtual ~ElemCompRp();
    protected:
	virtual bool on_expose_event(GdkEventExpose* event);
	virtual void on_size_allocate(Gtk::Allocation& 	aAlloc);
	virtual void on_size_request(Gtk::Requisition* aRequisition);
    private:
	Elem* iElem;
	ElemCompHead* iHead;
	Gtk::Allocation iBodyAlc;
};

class ElemCrp: public MCrp
{
    public:
	static string EType();
    public:
	ElemCrp(Elem* aElem);
	virtual ~ElemCrp();
	// From MCrp
	virtual Gtk::Widget& Widget();
    private:
	ElemCompRp* iRp;
};

#endif

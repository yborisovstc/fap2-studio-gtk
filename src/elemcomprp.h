
#ifndef __FAP2STU_ELEMCOMPRP_H
#define __FAP2STU_ELEMCOMPRP_H

#include <gtkmm/layout.h>
#include <gtkmm/box.h>
#include <elem.h>

class ElemCompHead: public Gtk::HBox
{
    public:
	ElemCompHead(const Elem& aElem);
    private:
	const Elem& iElem;
};

class ElemCompRp: public Gtk::Layout
{
    public:
	ElemCompRp(Elem* aElem);
	virtual ~ElemCompRp();
    protected:
	bool on_expose_event(GdkEventExpose* aEvent);
	void on_size_allocate(Gtk::Allocation* aAllc);
	void on_size_request(Gtk::Requisition* aRequisition);
    private:
	Elem* iElem;
	ElemCompHead* iHead;
	Gtk::Allocation iBodyAlc;
};

#endif

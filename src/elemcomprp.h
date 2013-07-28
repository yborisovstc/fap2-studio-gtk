
#ifndef __FAP2STU_ELEMCOMPRP_H
#define __FAP2STU_ELEMCOMPRP_H

#include <gtkmm/drawingarea.h>
#include <elem.h>

class ElemCompHead: public Gtk::HBox
{
    public:
	ElemCompHead(const Elem& aElem);
    private:
	const Elem& iElem;
};

class ElemCompRp: public Gtk::DrawingArea
{
    public:
	ElemCompRp(Elem* aElem);
	virtual ~ElemCompRp();
    private:
	Elem* iElem;
	ElemCompHead* iHead;
};

#endif

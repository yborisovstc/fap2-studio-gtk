
#ifndef __FAP2STU_ELEMDETRP_H
#define __FAP2STU_ELEMDETRP_H

#include <map>
#include <gtkmm/layout.h>
#include <gtkmm/box.h>
#include <gtkmm/enums.h>
#include <elem.h>
#include "elemcomprp.h"

class ElemDetRp: public Gtk::Layout
{
    public:
	ElemDetRp(Elem* aElem);
	virtual ~ElemDetRp();
    protected:
	virtual void on_size_allocate(Gtk::Allocation& aAlloc);
	virtual void on_size_request(Gtk::Requisition* aRequisition);
	bool on_comp_button_press(GdkEventButton* event);
    private:
	Elem* iElem;
	std::map<Elem*, ElemCompRp*> iCompRps; // Components representations
	std::vector<Elem*> iComps; // Components
};

#endif

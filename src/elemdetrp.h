
#ifndef __FAP2STU_ELEMDETRP_H
#define __FAP2STU_ELEMDETRP_H

#include <map>
#include <gtkmm/layout.h>
#include <gtkmm/box.h>
#include <elem.h>
#include "elemcomprp.h"

class ElemDetRp: public Gtk::Layout
{
    public:
	ElemDetRp(Elem* aElem);
	virtual ~ElemDetRp();
    protected:
	bool on_expose_event(GdkEventExpose* aEvent);
    private:
	Elem* iElem;
	std::map<Elem*, ElemCompRp*> iCompRps; // Components representations
	std::vector<Elem*> iComps; // Components
	Gtk::Allocation iBodyAlc;
};

#endif

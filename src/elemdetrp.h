
#ifndef __FAP2STU_ELEMDETRP_H
#define __FAP2STU_ELEMDETRP_H

#include <map>
#include <gtkmm/layout.h>
#include <gtkmm/box.h>
#include <gtkmm/enums.h>
#include <elem.h>
#include "elemcomprp.h"
#include "mprov.h"

class ElemDetRp: public Gtk::Layout
{
    public:
	ElemDetRp(Elem* aElem, const MCrpProvider& aCrpProv);
	virtual ~ElemDetRp();
	Elem* Model();
	typedef sigc::signal<void, Elem*> tSigCompSelected;
	tSigCompSelected SignalCompSelected();
    protected:
	virtual void on_size_allocate(Gtk::Allocation& aAlloc);
	virtual void on_size_request(Gtk::Requisition* aRequisition);
	bool on_comp_button_press(GdkEventButton* event);
	bool on_comp_button_press_ext(GdkEventButton* event, Elem* aComp);
    private:
	// Compact representations  provider
	const MCrpProvider& iCrpProv;
	Elem* iElem;
	std::map<Elem*, ElemCompRp*> iCompRps; // Components representations
	std::vector<Elem*> iComps; // Components
	tSigCompSelected iSigCompSelected;
};

#endif

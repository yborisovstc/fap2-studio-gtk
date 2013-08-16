
#ifndef __FAP2STU_VERTDRP_H
#define __FAP2STU_VERTDRP_H

#include <map>
#include <gtkmm/layout.h>
#include <gtkmm/box.h>
#include <gtkmm/enums.h>
#include <elem.h>
#include "elemcomprp.h"
#include "edgecrp.h"
#include "mprov.h"
#include "mdrp.h"

class VertDrpw: public Gtk::Layout
{
    friend class VertDrp;
    public:
	VertDrpw(Elem* aElem, const MCrpProvider& aCrpProv);
	virtual ~VertDrpw();
    protected:
	virtual void on_size_allocate(Gtk::Allocation& aAlloc);
	virtual void on_size_request(Gtk::Requisition* aRequisition);
	bool on_comp_button_press(GdkEventButton* event);
	bool on_comp_button_press_ext(GdkEventButton* event, Elem* aComp);
    private:
	// Compact representations  provider
	const MCrpProvider& iCrpProv;
	Elem* iElem;
	std::map<Elem*, MCrp*> iCompRps; // Components representations
	MDrp::tSigCompSelected iSigCompSelected;
};

class VertDrp: public MDrp
{
    public:
	static string EType();
    public:
	VertDrp(Elem* aElem, const MCrpProvider& aCrpProv);
	virtual ~VertDrp();
	// From MDrp
	virtual Gtk::Widget& Widget();
	virtual Elem* Model();
	virtual tSigCompSelected SignalCompSelected();
    private:
	VertDrpw* iRp;
};


#endif
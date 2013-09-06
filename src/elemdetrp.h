
#ifndef __FAP2STU_ELEMDETRP_H
#define __FAP2STU_ELEMDETRP_H

#include <map>
#include <gtkmm/layout.h>
#include <gtkmm/box.h>
#include <gtkmm/enums.h>
#include <elem.h>
#include "elemcomprp.h"
#include "mprov.h"
#include "mcrp.h"

class ElemDetRp: public Gtk::Layout
{
    friend class ElemDrp;
    public:
	ElemDetRp(Elem* aElem, const MCrpProvider& aCrpProv);
	virtual ~ElemDetRp();
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
	std::vector<Elem*> iComps; // Components
	MDrp::tSigCompSelected iSigCompSelected;
};

class ElemDrp: public MDrp
{
    public:
	static const string& Type();
	static string EType();
    public:
	ElemDrp(Elem* aElem, const MCrpProvider& aCrpProv);
	virtual ~ElemDrp();
	// From MDrp
	virtual void *DoGetObj(const string& aName);
	virtual Gtk::Widget& Widget();
	virtual Elem* Model();
	virtual tSigCompSelected SignalCompSelected();
    private:
	ElemDetRp* iRp;
};

#endif

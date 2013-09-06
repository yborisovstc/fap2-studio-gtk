
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
    public:
	class ConnInfo {
	    public:
		ConnInfo();
		ConnInfo(const ConnInfo& aCInfo);
		ConnInfo(int aOrder, int aToTop = 0, int aToBottom = 0);
	    public:
		int iCompOrder;
		int iConnsToTop;
		int iConnsToBottom;
	};
    public:
	VertDrpw(Elem* aElem, const MCrpProvider& aCrpProv);
	virtual ~VertDrpw();
    protected:
	virtual void on_size_allocate(Gtk::Allocation& aAlloc);
	virtual void on_size_request(Gtk::Requisition* aRequisition);
	bool on_comp_button_press(GdkEventButton* event);
	bool on_comp_button_press_ext(GdkEventButton* event, Elem* aComp);
    private:
	Elem* GetCompOwning(Elem* aElem);
    protected:
	// Compact representations  provider
	const MCrpProvider& iCrpProv;
	Elem* iElem;
	std::map<Elem*, MCrp*> iCompRps; // Components representations
	std::map<Elem*, ConnInfo> iConnInfos; // Connection infos
	MDrp::tSigCompSelected iSigCompSelected;

	friend class VertDrp;
};

class VertDrp: public MDrp
{
    public:
	static const string& Type();
	static string EType();
    public:
	VertDrp(Elem* aElem, const MCrpProvider& aCrpProv);
	virtual ~VertDrp();
	// From MDrp
	virtual void *DoGetObj(const string& aName);
	virtual Gtk::Widget& Widget();
	virtual Elem* Model();
	virtual tSigCompSelected SignalCompSelected();
    private:
	VertDrpw* iRp;
};


#endif

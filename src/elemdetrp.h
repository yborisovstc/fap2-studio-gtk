
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
    typedef std::map<Elem*, MCrp*> tCrps;
    public:
	ElemDetRp(Elem* aElem, const MCrpProvider& aCrpProv);
	virtual ~ElemDetRp();
    protected:
	virtual void on_size_allocate(Gtk::Allocation& aAlloc);
	virtual void on_size_request(Gtk::Requisition* aRequisition);
	virtual bool on_drag_drop(const Glib::RefPtr<Gdk::DragContext>& context, int x, int y, guint time);
	virtual void on_drag_data_received(const Glib::RefPtr<Gdk::DragContext>& context, int x, int y, const Gtk::SelectionData& selection_data, guint info, guint time);
	// Signal handlers
	bool on_comp_button_press(GdkEventButton* event);
	bool on_comp_button_press_ext(GdkEventButton* event, Elem* aComp);
	void on_comp_button_press_name(GdkEventButton* event, Elem* aComp);
    protected:
	virtual void on_node_dropped(const std::string& aUri);
    protected:
	void Construct();
	void Erase();
	void add_node(const std::string& aParentUri);
    private:
	// Compact representations  provider
	const MCrpProvider& iCrpProv;
	Elem* iElem;
	tCrps iCompRps; // Components representations
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

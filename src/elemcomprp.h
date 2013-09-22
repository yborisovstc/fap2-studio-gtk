
#ifndef __FAP2STU_ELEMCOMPRP_H
#define __FAP2STU_ELEMCOMPRP_H

#include <gtkmm/layout.h>
#include <gtkmm/box.h>
#include "gtkmm/label.h"
#include "gtkmm/drawingarea.h"
#include "mcrp.h"
#include <elem.h>

class ElemCrpCtxMenu: public Gtk::Menu
{
    public:
	ElemCrpCtxMenu();
};

class ElemCompHead: public Gtk::HBox
{
    public:
	ElemCompHead(const Elem& aElem);
	virtual ~ElemCompHead();
    protected:
	virtual bool on_expose_event(GdkEventExpose* event);
    public:
	const Elem& iElem;
	Gtk::Label* iName;
	Gtk::Label* iParent;
};

class ElemCompRp: public Gtk::Layout
{
    friend class ElemCrp;
    public:
	ElemCompRp(Elem* aElem);
	virtual ~ElemCompRp();
    protected:
	virtual bool on_expose_event(GdkEventExpose* event);
	virtual void on_size_allocate(Gtk::Allocation& 	aAlloc);
	virtual void on_size_request(Gtk::Requisition* aRequisition);
	// Signal handlers
	bool on_name_button_press(GdkEventButton* event);
    protected:
	Elem* iElem;
	ElemCompHead* iHead;
	Gtk::Allocation iBodyAlc;
	MCrp::tSigButtonPressName iSigButtonPressName;
	Gtk::Menu iContextMenu; // Context menu
};

class ElemCrp: public MCrp
{
    public:
	static const string& Type();
	static string EType();
    public:
	ElemCrp(Elem* aElem);
	virtual ~ElemCrp();
	// From MCrp
	virtual Gtk::Widget& Widget();
	virtual void *DoGetObj(const string& aName);
	virtual tSigButtonPressName SignalButtonPressName();
    private:
	ElemCompRp* iRp;
};

#endif

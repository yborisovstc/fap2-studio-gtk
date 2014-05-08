
#ifndef __FAP2STU_ELEMCOMPRP_H
#define __FAP2STU_ELEMCOMPRP_H

#include <gtkmm/layout.h>
#include <gtkmm/box.h>
#include "gtkmm/label.h"
#include "gtkmm/drawingarea.h"
#include "gtkmm/menu.h"
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
	void DoSetHighlighted(bool aSet);
    protected:
	virtual void Construct();
	virtual void on_realize();
	virtual bool on_expose_event(GdkEventExpose* event);
	virtual void on_size_allocate(Gtk::Allocation& 	aAlloc);
	virtual void on_size_request(Gtk::Requisition* aRequisition);
	virtual bool on_button_press_event(GdkEventButton* aEvent);
	virtual void on_drag_data_get(const Glib::RefPtr<Gdk::DragContext>&, Gtk::SelectionData& data, guint info, guint time);
	// Signal handlers
	bool on_name_button_press(GdkEventButton* event);
    protected:
	Elem* iElem;
	ElemCompHead* iHead;
	Gtk::Allocation iBodyAlc;
	MCrp::tSigButtonPressName iSigButtonPressName;
	MCrp::tSigButtonPress iSigButtonPress;
	MCrp::tSigUpdated iSigUpdated;
	Gtk::Menu iContextMenu; // Context menu
	bool iHighlighted;
	Gtk::Label iContent;
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
	virtual tSigButtonPress SignalButtonPress();
	virtual tSigUpdated SignalUpdated();
	virtual bool IsActionSupported(Action aAction);
	virtual void GetContentUri(GUri& aUri);
	virtual bool Dragging();
	virtual void SetHighlighted(bool aSet);
	virtual Elem* Model();
    private:
	ElemCompRp* iRp;
};

#endif

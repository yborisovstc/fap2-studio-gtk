
#ifndef __FAP2STU_ELEMCOMPRP_H
#define __FAP2STU_ELEMCOMPRP_H

#include <gtkmm/layout.h>
#include <gtkmm/box.h>
#include "gtkmm/label.h"
#include "gtkmm/drawingarea.h"
#include "gtkmm/menu.h"
#include "gtkmm/tooltip.h"
#include "mcrp.h"
#include <elem.h>

using namespace Gtk;

class ElemCrpCtxMenu: public Gtk::Menu
{
    public:
	ElemCrpCtxMenu();
};

class ElemCompHead: public Gtk::HBox
{
    public:
	ElemCompHead(const MElem& aElem);
	virtual ~ElemCompHead();
    protected:
	virtual bool on_expose_event(GdkEventExpose* event);
    public:
	const MElem& iElem;
	Gtk::Label* iName;
	Gtk::Label* iParent;
};

class ElemCompRp: public Gtk::Layout
{
    friend class ElemCrp;
    public:
	ElemCompRp(MElem* aElem);
	virtual ~ElemCompRp();
    protected:
	void DoSetHighlighted(bool aSet);
	void DoSetErroneous(bool aSet);
    protected:
	virtual void Construct();
	void DoSetDnDTargSupported(int aTarg);
	bool DoIsDnDTargSupported(TDnDTarg aTarg) const;
	bool DoIsIntersected(int aX, int aY) const;
	virtual void GetModelDebugInfo(int x, int y, string& aData) const;
	virtual bool DoIsActionSupported(MCrp::Action aAction);
	virtual void on_realize();
	virtual bool on_expose_event(GdkEventExpose* event);
	virtual void on_size_allocate(Gtk::Allocation& 	aAlloc);
	virtual void on_size_request(Gtk::Requisition* aRequisition);
	virtual bool on_button_press_event(GdkEventButton* aEvent);
	virtual void on_drag_data_get(const Glib::RefPtr<Gdk::DragContext>&, Gtk::SelectionData& data, guint info, guint time);
	// Signal handlers
	bool on_name_button_press(GdkEventButton* event);
	bool on_query_tooltip(int x, int y, bool keyboard_tooltip, const Glib::RefPtr<Tooltip>& tooltip);
	// Helpers
	void GetFormattedContent(string& aContent) const;
    protected:
	MElem* iElem;
	ElemCompHead* iHead;
	Gtk::Allocation iBodyAlc;
	MCrp::tSigButtonPressName iSigButtonPressName;
	MCrp::tSigButtonPress iSigButtonPress;
	MCrp::tSigUpdated iSigUpdated;
	Gtk::Menu iContextMenu; // Context menu
	bool iHighlighted;
	bool iErr;
	Gtk::Label iContent;
	int iLArea;
	int iDnDSupp;
};

class ElemCrp: public MCrp
{
    public:
	static const string& Type();
	static string EType();
    public:
	ElemCrp(MElem* aElem);
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
	virtual void SetErroneous(bool aSet);
	virtual MElem* Model();
	virtual void SetLArea(int aArea);
	virtual int GetLArea() const;
	virtual void SetDnDTargSupported(int aTarg);
	virtual bool IsDnDTargSupported(TDnDTarg aTarg) const;
	virtual bool IsIntersected(int aX, int aY) const;
    private:
	ElemCompRp* iRp;
};

#endif

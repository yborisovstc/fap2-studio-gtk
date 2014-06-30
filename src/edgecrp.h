
#ifndef __FAP2STU_EDGECRP_H
#define __FAP2STU_EDGECRP_H

#include "mcrp.h"
#include <elem.h>
#include <gtkmm/eventbox.h>
#include <gtkmm/drawingarea.h>

using namespace Gtk;

class MEdgeCrp
{
    public:
	// Vector of edges nodes
	typedef vector<Requisition> TVectEn;
    public:
	static const string& Type();
    public:
	virtual void SetCp1Coord(const Gtk::Requisition& aReq) = 0;
	virtual void SetCp2Coord(const Gtk::Requisition& aReq) = 0;
	virtual const Gtk::Requisition& Cp1Coord() = 0;
	virtual const Gtk::Requisition& Cp2Coord() = 0;
	virtual Elem* Point1() = 0;
	virtual Elem* Point2() = 0;
	virtual TVectEn& VectEn() = 0;
};


// Edge compact representation, version 1
class EdgeCrp: public Gtk::Widget, public MCrp, public MEdgeCrp
{
    public:
    enum TDraggedPart { EDp_None, EDp_Cp1, EDp_Cp2 };
    public:
	static const string& Type();
	static string EType();
    public:
	EdgeCrp(Elem* aElem);
	virtual ~EdgeCrp();
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
	virtual void SetLArea(int aArea);
	virtual int GetLArea() const;
	// From MEdgeCrp
	virtual void SetCp1Coord(const Gtk::Requisition& aReq);
	virtual void SetCp2Coord(const Gtk::Requisition& aReq);
	virtual const Gtk::Requisition& Cp1Coord();
	virtual const Gtk::Requisition& Cp2Coord();
	virtual Elem* Point1();
	virtual Elem* Point2();
	virtual TVectEn& VectEn();
    protected:
	bool IsPointIn(int aX, int aY);
    protected:
	virtual bool on_expose_event(GdkEventExpose* event);
	virtual void on_size_request(Gtk::Requisition* aRequisition);
	virtual bool on_motion_notify_event(GdkEventMotion* aEvent);
	virtual bool on_leave_notify_event(GdkEventCrossing* event);
	virtual bool on_button_press_event(GdkEventButton* aEvent);
	virtual void on_drag_begin(const Glib::RefPtr<Gdk::DragContext>& aContext);
	virtual void on_drag_data_get(const Glib::RefPtr<Gdk::DragContext>&, Gtk::SelectionData& data, guint info, guint time);
	virtual void on_drag_end(const Glib::RefPtr<Gdk::DragContext>& context);
    private:
	Elem* iElem;
	TVectEn iNodes;
	Requisition iCp1;
	Requisition iCp2;
	TDraggedPart iDraggedPart;
	bool iDragging;
	bool iHighlighted;
	Gdk::Region iRegion;
	MCrp::tSigUpdated iSigUpdated;
	MCrp::tSigButtonPress iSigButtonPress;
	MCrp::tSigButtonPressName iSigButtonPressName;
};

#endif

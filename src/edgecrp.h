
#ifndef __FAP2STU_EDGECRP_H
#define __FAP2STU_EDGECRP_H

#include "mcrp.h"
#include <elem.h>
#include <gtkmm/eventbox.h>
#include <gtkmm/drawingarea.h>

class MEdgeCrp
{
    public:
	enum EdgeType {
	    EtRight = 1,
	    EtLeft,
	    EtLtRb, /* Left top to right bottom */
	    EtLbRt /* Left bottom to right top */
	};
    public:
	static const string& Type();
    public:
	virtual void SetType(EdgeType aType) = 0;
	virtual void SetUcp(int aPos) = 0;
	virtual void SetLcp(int aPos) = 0;
	virtual void SetUcpExt(int aX, int aY) = 0;
	virtual void SetLcpExt(int aX, int aY) = 0;
	virtual void SetExtent(Gtk::Requisition aReq, int aPos) = 0;
	virtual Elem* Point1() = 0;
	virtual Elem* Point2() = 0;
};

class EdgeCompRp: public Gtk::Widget
{
    friend class EdgeCrp;
    public:
	class Cp 
	{
	    public:
		Cp();
		Gtk::Requisition iCoord;
		// Position from Cp: positive for upper
		int iPos;
	};
    public:
	EdgeCompRp(Elem* aElem);
	virtual ~EdgeCompRp();
    protected:
	virtual bool on_expose_event(GdkEventExpose* event);
	virtual void on_size_request(Gtk::Requisition* aRequisition);
	// Signal handlers
	void on_drag_data_get(const Glib::RefPtr<Gdk::DragContext>&, Gtk::SelectionData& data, guint, guint);
	void on_drag_begin(const Glib::RefPtr<Gdk::DragContext>&);
    private:
	Elem* iElem;
	MEdgeCrp::EdgeType iType;
	Cp iUcp;
	Cp iLcp;
	Gtk::EventBox iEboxP1;
};

class EdgeCompRp_v1: public Gtk::Container
{
    friend class EdgeCrp;
    public:
	class Cp 
	{
	    public:
		Cp();
		Gtk::Requisition iCoord;
		// Position from Cp: positive for upper
		int iPos;
	};
    public:
	EdgeCompRp_v1(Elem* aElem);
	virtual ~EdgeCompRp_v1();
	// From Container
	virtual void add(Gtk::Widget& widget);
    protected:
	virtual void on_size_allocate(Gtk::Allocation& aAlloc);
	virtual bool on_expose_event(GdkEventExpose* event);
	virtual void on_size_request(Gtk::Requisition* aRequisition);
	// From Container
	virtual void forall_vfunc(gboolean include_internals, GtkCallback callback, gpointer callback_data);
	// Signal handlers
	void on_cp_drag_begin(const Glib::RefPtr<Gdk::DragContext>&);
	bool on_cp_button_press(GdkEventButton* event);
    private:
	Elem* iElem;
	MEdgeCrp::EdgeType iType;
	Cp iUcp;
	Cp iLcp;
	//Gtk::EventBox iEboxP1;
	//Gtk::DrawingArea iEboxP1;
	Gtk::Widget* iP1;
};

class EdgeCrp: public MCrp, public MEdgeCrp
{
    public:
	static const string& Type();
	static string EType();
    public:
	EdgeCrp(Elem* aElem);
	virtual ~EdgeCrp();
	// From MEdgeCrp
	virtual void SetType(EdgeType aType);
	virtual void SetUcp(int aPos);
	virtual void SetLcp(int aPos);
	virtual void SetUcpExt(int aX, int aY);
	virtual void SetLcpExt(int aX, int aY);
	virtual void SetExtent(Gtk::Requisition aReq, int aPos);
	virtual Elem* Point1();
	virtual Elem* Point2();
	// From MCrp
	virtual Gtk::Widget& Widget();
	virtual void *DoGetObj(const string& aName);
	virtual tSigButtonPressName SignalButtonPressName();
	virtual bool IsActionSupported(Action aAction);
    private:
	//EdgeCompRp* iRp;
	EdgeCompRp_v1* iRp;
	MCrp::tSigButtonPressName iSigButtonPressName;
};


#endif


#ifndef __FAP2STU_VERTDRP_H
#define __FAP2STU_VERTDRP_H

#include <map>
#include <gtkmm/layout.h>
#include <gtkmm/box.h>
#include <gtkmm/enums.h>
#include <elem.h>
#include "elemcomprp.h"
#include "elemdetrp.h"
#include "edgecrp.h"
#include "mprov.h"
#include "mdrp.h"

// Widget of Vertex detailed representation, version#1
class VertDrpw_v1: public ElemDetRp, public MDrp
{
    public:
	class ConnInfo {
	    public:
		ConnInfo();
		ConnInfo(const ConnInfo& aCInfo);
		ConnInfo(int aOrder);
	    public:
		int iCompOrder;
	};

    public:
	static std::string Type() { return string("VertDrpw");};
    public:
	static string EType();
    public:
	VertDrpw_v1(MElem* aElem, const MCrpProvider& aCrpProv, MSEnv& aStEnv);
	virtual ~VertDrpw_v1();
    public:
	// From MCrpMgr
	virtual bool IsTypeAllowed(const std::string& aType) const;
    protected:
	// From MDrp
	virtual void *DoGetObj(const string& aName);
	virtual Gtk::Widget& Widget();
	virtual MElem* Model();
	virtual tSigCompSelected SignalCompSelected();
	virtual tSigCompActivated SignalCompActivated();
	virtual tSigDragMotion SignalDragMotion();
	virtual tSigAttention SignalAttention() {return mSignalAttention;};
	virtual void Udno();
	virtual void OnActionInsert();
    protected:
	virtual bool on_expose_event(GdkEventExpose* event);
	virtual bool on_drag_motion (const Glib::RefPtr<Gdk::DragContext>& context, int x, int y, guint time);
	virtual void on_drag_data_received(const Glib::RefPtr<Gdk::DragContext>& context, int x, int y, 
		const Gtk::SelectionData& selection_data, guint info, guint time);
	virtual bool on_drag_drop(const Glib::RefPtr<Gdk::DragContext>& context, int x, int y, guint time);
	virtual void on_size_allocate(Gtk::Allocation& aAlloc);
	virtual void on_size_request(Gtk::Requisition* aRequisition);
	virtual bool on_button_press_event(GdkEventButton* aEvent);
	virtual bool on_motion_notify_event(GdkEventMotion* aEvent);
	virtual bool on_leave_notify_event(GdkEventCrossing* event);
	// Signal handlers
	void on_comp_updated(MElem* aElem);
    protected:
	virtual MElem* GetCompOwning(MElem* aElem);
	// TODO This is workaround for missing of model APIs of mutation trying, ref #uc_014. To implement API
	virtual bool AreCpsCompatible(MElem* aCp1, MElem* aCp2);
    protected:
	void ParseDndReceivedDataEdge(string& aEdgeUri, string& aEdgePoint);
    protected:
	MCrp* iEdgeDropCandidate;
	MElem* iEdgeDropCpCandidate;

	friend class VertDrp;
};


// Widget of Vertex detailed representation
class VertDrp: public MDrp
{
    public:
	static const string& Type();
	static string EType();
    public:
	VertDrp(MElem* aElem, const MCrpProvider& aCrpProv, MSEnv& aStEnv);
	virtual ~VertDrp();
	// From MDrp
	virtual void *DoGetObj(const string& aName);
	virtual Gtk::Widget& Widget();
	virtual MElem* Model();
	virtual tSigCompSelected SignalCompSelected();
	virtual tSigDragMotion SignalDragMotion();
    private:
	VertDrpw_v1* iRp;
};


#endif

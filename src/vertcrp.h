
#ifndef __FAP2STU_VERTCRP_H
#define __FAP2STU_VERTCRP_H

#include <gtkmm/layout.h>
#include <gtkmm/box.h>
#include "gtkmm/label.h"
#include "gtkmm/drawingarea.h"
#include <elem.h>
#include "elemcomprp.h"

class VertCompHead: public Gtk::HBox
{
    public:
	VertCompHead(const Elem& aElem);
	virtual ~VertCompHead();
    protected:
	virtual bool on_expose_event(GdkEventExpose* event);
    private:
	const Elem& iElem;
	Gtk::Label* iName;
	Gtk::Label* iParent;
};

class VertCompRp: public ElemCompRp, public MCrp, public MCrpConnectable
{
    public:
	static const string& Type();
	static string EType();
    public:
	VertCompRp(Elem* aElem);
	virtual ~VertCompRp();
    protected:
	virtual bool on_expose_event(GdkEventExpose* event);
	virtual void on_size_allocate(Gtk::Allocation& 	aAlloc);
	virtual void on_size_request(Gtk::Requisition* aRequisition);
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
	// From MCrpConnectable
	virtual Gtk::Requisition GetCpCoord(Elem* aCp = NULL);
	virtual bool GetIsInt() const;
	virtual void SetIsInt(bool aIsInt);
	virtual TCpType GetCpType() const;
	virtual void SetCpType(TCpType aType);
	virtual int GetNearestCp(Gtk::Requisition aCoord, Elem*& aCp);
	virtual void HighlightCp(Elem* aCp, bool aSet);
    protected:
	bool iIsInt;
	TCpType mCpType;
};

/*
class VertCrp: public MCrp, public MCrpConnectable
{
    public:
	static const string& Type();
	static string EType();
    public:
	VertCrp(Elem* aElem);
	virtual ~VertCrp();
	// From MCrp
	virtual Gtk::Widget& Widget();
	virtual void *DoGetObj(const string& aName);
	// From MCrpConnectable
	virtual Gtk::Requisition GetCpCoord(Elem* aCp = NULL);
    private:
	VertCompRp* iRp;
};
*/

#endif

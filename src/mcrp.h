
#ifndef __FAP2STU_MCRP_H
#define __FAP2STU_MCRP_H

#include <string>
#include <elem.h>
#include <gtkmm/widget.h>
#include "mbase.h"


// Compact representation interface

class MCrp: public MBase
{
    public:
	enum Action {
	    EA_Rename,
	    EA_Remove,
	    EA_Edit_Content
	};
    public:
	typedef sigc::signal<void, GdkEventButton*> tSigButtonPressName;
	typedef sigc::signal<void, Elem*> tSigUpdated;
    public:
	virtual Gtk::Widget& Widget() = 0;
	virtual tSigButtonPressName SignalButtonPressName() = 0;
	virtual tSigUpdated SignalUpdated() = 0;
	virtual bool IsActionSupported(Action aAction) = 0;
	virtual bool Dragging() = 0;
};

class MCrpConnectable
{
    public:
	enum CpType{
	    CpGeneric,
	    CpInp,
	    CpOutp
	};
    public:
	static std::string Type() { return "MCrpConnectable";};
    public:
	virtual Gtk::Requisition GetCpCoord(Elem* aCp = NULL) = 0;
	// Is internal
	virtual bool GetIsInt() const = 0;
	// Set internal/boundary
	virtual void SetIsInt(bool aIsInt) = 0;
};

#endif

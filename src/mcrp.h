
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
	    EA_Edit_Content,
	    EA_Save_Chromo
	};
	// Layout area, vertical based
	enum TLArea {
	    EOverlay = -1, // Overlay, e.g. edge
	    ELeft = 0, // Left boundary zone
	    ERight = 1, // Right boundary zone
	    EMain = 2, // Main vertical layout area
	};
    public:
	typedef sigc::signal<void, GdkEventButton*> tSigButtonPressName;
	typedef sigc::signal<void, GdkEventButton*> tSigButtonPress;
	typedef sigc::signal<void, Elem*> tSigUpdated;
    public:
	virtual Gtk::Widget& Widget() = 0;
	virtual tSigButtonPressName SignalButtonPressName() = 0;
	virtual tSigButtonPress SignalButtonPress() = 0;
	virtual tSigUpdated SignalUpdated() = 0;
	virtual bool IsActionSupported(Action aAction) = 0;
	virtual void GetContentUri(GUri& aUri) = 0;
	virtual bool Dragging() = 0;
	virtual void SetHighlighted(bool aSet) = 0;
	virtual Elem* Model() = 0;
	virtual void SetLArea(int aArea) = 0;
	virtual int GetLArea() const = 0;
};

class MCrpConnectable
{
    public:
	enum TCpDir {
	    ECpGeneric,
	    ECpInp,
	    ECpOutp
	};
    public:
	static std::string Type() { return "MCrpConnectable";};
    public:
	virtual Gtk::Requisition GetCpCoord(Elem* aCp = NULL) = 0;
	// Is internal
	virtual bool GetIsInt() const = 0;
	// Set internal/boundary
	virtual void SetIsInt(bool aIsInt) = 0;
	// Get Cp type
	virtual TCpDir GetCpDir() const = 0;
	// Set Cp type
	virtual void SetCpDir(TCpDir aType) = 0;
	// Get nearest CP, returns distance to checking positon
	virtual int GetNearestCp(Gtk::Requisition aCoord, Elem*& aCp) = 0;
	// Highlight CP
	virtual void HighlightCp(Elem* aCp, bool aSet) = 0;
};

#endif

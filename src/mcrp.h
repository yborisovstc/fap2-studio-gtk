
#ifndef __FAP2STU_MCRP_H
#define __FAP2STU_MCRP_H

#include <string>
#include <elem.h>
#include <mvert.h>
#include <gtkmm/widget.h>
#include "mbase.h"
#include "common.h"


// Compact representation interface

class MCrp: public MBase
{
    public:
	enum Action {
	    EA_Rename,
	    EA_Remove,
	    EA_Edit_Content,
	    EA_Save_Chromo,
	    EA_TransToMut,
	    EA_GetParentsModifs
	};
	// Layout area, vertical based
	enum TLArea {
	    EUnknown = -2,
	    EOverlay = -1, // Overlay, e.g. edge
	    ELeft = 0, // Left boundary zone
	    ERight = 1, // Right boundary zone
	    EMain = 2, // Main vertical layout area
	};
    public:
	typedef sigc::signal<void, GdkEventButton*> tSigButtonPressName;
	typedef sigc::signal<void, GdkEventButton*> tSigButtonPress;
	typedef sigc::signal<void, MElem*> tSigUpdated;
    public:
	virtual Gtk::Widget& Widget() = 0;
	virtual tSigButtonPressName SignalButtonPressName() = 0;
	virtual tSigButtonPress SignalButtonPress() = 0;
	virtual tSigUpdated SignalUpdated() = 0;
	virtual bool IsActionSupported(Action aAction) = 0;
	virtual void GetContentUri(GUri& aUri) = 0;
	virtual bool Dragging() = 0;
	virtual void SetHighlighted(bool aSet) = 0;
	virtual void SetErroneous(bool aSet) = 0;
	virtual MElem* Model() = 0;
	virtual void SetLArea(int aArea) = 0;
	virtual int GetLArea() const = 0;
	virtual void SetDnDTargSupported(int aTarg) = 0;
	virtual bool IsDnDTargSupported(TDnDTarg aTarg) const = 0;
	virtual bool IsIntersected(int aX, int aY) const = 0;
};

class MCrpConnectable
{
    public:
	static std::string Type() { return "MCrpConnectable";};
    public:
	virtual Gtk::Requisition GetCpCoord(MElem* aCp = NULL) = 0;
	// Get Cp type
	virtual MCompatChecker::TDir GetCpDir() const = 0;
	// Get nearest CP, returns distance to checking positon
	virtual int GetNearestCp(Gtk::Requisition aCoord, MElem*& aCp) = 0;
	// Highlight CP
	virtual void HighlightCp(MElem* aCp, bool aSet) = 0;
};

#endif

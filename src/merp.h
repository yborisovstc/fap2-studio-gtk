
#ifndef __FAP2STU_MERP_H
#define __FAP2STU_MERP_H

#include <string>
#include <elem.h>
#include <mvert.h>
#include <gtkmm/widget.h>
#include "mbase.h"


// Embedded representation interface

class MErp: public MBase
{
    public:
	enum TPos {
	    EPos_Left,
	    EPos_Center,
	    EPos_Right
	};
    public:
	virtual Gtk::Widget& Widget() = 0;
	virtual void SetHighlighted(bool aSet) = 0;
	virtual MElem* Model() = 0;
	virtual void SetPos(TPos aPos) = 0;
	virtual TPos GetPos() const = 0;
	virtual MCompatChecker::TDir GetMdlDir() const = 0;
};

// TODO To combine MErpConnectable and MCrpConnectable
class MErpConnectable
{
    public:
	static std::string Type() { return "ECrpConnectable";};
    public:
	virtual Gtk::Requisition GetCpCoord(MElem* aCp = NULL) = 0;
	// Get nearest CP, returns distance to checking positon
	virtual int GetNearestCp(Gtk::Requisition aCoord, MElem*& aCp) = 0;
	// Highlight CP
	virtual void HighlightCp(MElem* aCp, bool aSet) = 0;
};


#endif

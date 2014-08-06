
#ifndef __FAP2STU_MDRP_H
#define __FAP2STU_MDRP_H

#include "mbase.h"
#include "gtkmm/widget.h"

class Elem;
class MCrp;

// Detail representation interface

class MDrp: public MBase
{
    public:
	static std::string Type() { return "MDrp";};
    public:
	typedef sigc::signal<void, Elem*> tSigCompSelected;
	typedef sigc::signal<void, Elem*> tSigCompActivated;
	typedef sigc::signal<void, Gtk::Widget&, int, int> tSigDragMotion;
	typedef sigc::signal<void, const string&> tSigAttention;
	typedef sigc::signal<void> tSigReloadRequired;
    public:
	virtual Gtk::Widget& Widget() = 0;
	virtual Elem* Model() = 0;
	virtual tSigCompSelected SignalCompSelected() = 0;
	virtual tSigCompSelected SignalCompActivated() = 0;
	virtual tSigDragMotion SignalDragMotion() = 0;
	virtual tSigAttention SignalAttention() {return mSigAttention;};
	virtual tSigReloadRequired SignalReloadRequired() {return mSigReloadRequired;};
	virtual void Udno() = 0;
	virtual bool IsActionSupported(Elem* aComp, const MCrp::Action& aAction) {return false;};
    protected:
	tSigAttention mSigAttention;
	tSigReloadRequired mSigReloadRequired;
};

#endif

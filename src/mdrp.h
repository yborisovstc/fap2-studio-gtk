
#ifndef __FAP2STU_MDRP_H
#define __FAP2STU_MDRP_H

#include "mbase.h"
#include "gtkmm/widget.h"

class Elem;

// Detail representation interface

class MDrp: public MBase
{
    public:
	static std::string Type() { return "MDrp";};
    public:
	typedef sigc::signal<void, Elem*> tSigCompSelected;
    public:
	virtual Gtk::Widget& Widget() = 0;
	virtual Elem* Model() = 0;
	virtual tSigCompSelected SignalCompSelected() = 0;
};

#endif

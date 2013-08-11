
#ifndef __FAP2STU_MDRP_H
#define __FAP2STU_MDRP_H

#include "gtkmm/widget.h"

class Elem;

// Detail representation interface

class MDrp
{
    public:
	typedef sigc::signal<void, Elem*> tSigCompSelected;
    public:
	virtual Gtk::Widget& Widget() = 0;
	virtual Elem* Model() = 0;
	virtual tSigCompSelected SignalCompSelected() = 0;
};

#endif

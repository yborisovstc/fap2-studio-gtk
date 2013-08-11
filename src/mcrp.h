
#ifndef __FAP2STU_MCRP_H
#define __FAP2STU_MCRP_H

#include "gtkmm/widget.h"


// Compact representation interface

class MCrp
{
    public:
	virtual ~MCrp() {};
	virtual Gtk::Widget& Widget() = 0;
};

#endif

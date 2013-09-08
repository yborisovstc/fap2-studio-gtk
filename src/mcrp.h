
#ifndef __FAP2STU_MCRP_H
#define __FAP2STU_MCRP_H

#include <string>
#include <elem.h>
#include "gtkmm/widget.h"
#include "mbase.h"


// Compact representation interface

class MCrp: public MBase
{
    public:
	virtual Gtk::Widget& Widget() = 0;
};

class MCrpConnectable: public MBase
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
	//virtual void SetCpCoord(const Gtk::Requisition& aRec, Elem* aCp = NULL) = 0;
};


#endif

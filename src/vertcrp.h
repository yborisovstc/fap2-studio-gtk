
#ifndef __FAP2STU_VERTCRP_H
#define __FAP2STU_VERTCRP_H

#include <gtkmm/layout.h>
#include <gtkmm/box.h>
#include "gtkmm/label.h"
#include "gtkmm/drawingarea.h"
#include <elem.h>
#include "mcrp.h"

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

class VertCompRp: public Gtk::Layout, public MCrp, public MCrpConnectable
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
	// From MCrpConnectable
	virtual Gtk::Requisition GetCpCoord(Elem* aCp = NULL);
	virtual bool GetIsInt() const;
	virtual void SetIsInt(bool aIsInt);
    protected:
	Elem* iElem;
	VertCompHead* iHead;
	Gtk::Allocation iBodyAlc;
	bool iIsInt;
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

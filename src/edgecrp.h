
#ifndef __FAP2STU_EDGECRP_H
#define __FAP2STU_EDGECRP_H

#include "mcrp.h"
#include <elem.h>

class MEdgeCrp
{
    public:
	static const string& Type();
    public:
	virtual void SetUcp(Gtk::Requisition aReq, int aPos) = 0;
	virtual void SetLcp(Gtk::Requisition aReq, int aPos) = 0;
	virtual void SetExtent(Gtk::Requisition aReq, int aPos) = 0;
	virtual Elem* Point1() = 0;
	virtual Elem* Point2() = 0;
};

class EdgeCompRp: public Gtk::Widget
{
    friend class EdgeCrp;
    public:
	struct Cp 
	{
	    Gtk::Requisition iCoord;
	    // Position from Cp: positive for upper
	    int iPos;
	};
    public:
	EdgeCompRp(Elem* aElem);
	virtual ~EdgeCompRp();
    protected:
	virtual bool on_expose_event(GdkEventExpose* event);
	virtual void on_size_request(Gtk::Requisition* aRequisition);
    private:
	Elem* iElem;
	Cp iUcp;
	Cp iLcp;
};

class EdgeCrp: public MCrp, public MEdgeCrp
{
    public:
	static const string& Type();
	static string EType();
    public:
	EdgeCrp(Elem* aElem);
	virtual ~EdgeCrp();
	// From MEdgeCrp
	virtual void SetUcp(Gtk::Requisition aReq, int aPos);
	virtual void SetLcp(Gtk::Requisition aReq, int aPos);
	virtual void SetExtent(Gtk::Requisition aReq, int aPos);
	virtual Elem* Point1();
	virtual Elem* Point2();
	// From MCrp
	virtual Gtk::Widget& Widget();
	virtual void *DoGetObj(const string& aName);
    private:
	EdgeCompRp* iRp;
};


#endif

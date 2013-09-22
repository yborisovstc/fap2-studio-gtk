
#ifndef __FAP2STU_EDGECRP_H
#define __FAP2STU_EDGECRP_H

#include "mcrp.h"
#include <elem.h>

class MEdgeCrp
{
    public:
	enum EdgeType {
	    EtRight = 1,
	    EtLeft,
	    EtLtRb, /* Left top to right bottom */
	    EtLbRt /* Left bottom to right top */
	};
    public:
	static const string& Type();
    public:
	virtual void SetType(EdgeType aType) = 0;
	virtual void SetUcp(int aPos) = 0;
	virtual void SetLcp(int aPos) = 0;
	virtual void SetUcpExt(int aX, int aY) = 0;
	virtual void SetLcpExt(int aX, int aY) = 0;
	virtual void SetExtent(Gtk::Requisition aReq, int aPos) = 0;
	virtual Elem* Point1() = 0;
	virtual Elem* Point2() = 0;
};

class EdgeCompRp: public Gtk::Widget
{
    friend class EdgeCrp;
    public:
	class Cp 
	{
	    public:
		Cp();
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
	MEdgeCrp::EdgeType iType;
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
	virtual void SetType(EdgeType aType);
	virtual void SetUcp(int aPos);
	virtual void SetLcp(int aPos);
	virtual void SetUcpExt(int aX, int aY);
	virtual void SetLcpExt(int aX, int aY);
	virtual void SetExtent(Gtk::Requisition aReq, int aPos);
	virtual Elem* Point1();
	virtual Elem* Point2();
	// From MCrp
	virtual Gtk::Widget& Widget();
	virtual void *DoGetObj(const string& aName);
	virtual tSigButtonPressName SignalButtonPressName();
    private:
	EdgeCompRp* iRp;
	MCrp::tSigButtonPressName iSigButtonPressName;
};


#endif

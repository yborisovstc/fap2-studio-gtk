
#ifndef __FAP2STU_PROPCRP_H
#define __FAP2STU_PROPCRP_H

#include <gtkmm/textview.h>
#include "elemcomprp.h"

class PropCrp: public ElemCompRp, public MCrp
{
    public:
	static const string& Type();
	static string EType();
    public:
	PropCrp(MElem* aElem);
	virtual ~PropCrp();
    protected:
	// From MCrp
	virtual Gtk::Widget& Widget();
	virtual void *DoGetObj(const string& aName);
	virtual tSigButtonPressName SignalButtonPressName();
	virtual tSigButtonPress SignalButtonPress();
	virtual tSigUpdated SignalUpdated();
	virtual bool IsActionSupported(Action aAction);
	virtual void GetContentUri(GUri& aUri);
	virtual bool Dragging();
	virtual void SetHighlighted(bool aSet);
	virtual void SetErroneous(bool aSet);
	virtual MElem* Model();
	virtual void SetLArea(int aArea);
	virtual int GetLArea() const;
	virtual void SetDnDTargSupported(int aTarg);
	virtual bool IsDnDTargSupported(TDnDTarg aTarg) const;
	virtual bool IsIntersected(int aX, int aY) const;
    protected:
	//Gtk::TextView iContent;
//	Gtk::Label iContent;
	static int iMaxWidthChars;
};

#endif



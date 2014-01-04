
#ifndef __FAP2STU_CPERP_H
#define __FAP2STU_CPERP_H

#include "gtkmm/label.h"
#include "gtkmm/box.h"
#include "merp.h"
#include "mprov.h"

using namespace std;
using namespace Gtk;

// Connection Point Erp
class CpErp: public Label, public MErp, public MErpConnectable
{
    public:
	static const string& Type();
	static string EType();
    public:
	CpErp(Elem* aElem);
	virtual ~CpErp();
    protected:
	void DoSetHighlighted(bool aSet);
    protected:
	// From MErp
	virtual Gtk::Widget& Widget();
	virtual void *DoGetObj(const string& aName);
	virtual void SetHighlighted(bool aSet);
	virtual Elem* Model();
	virtual void SetPos(TPos aPos);
	// From MErpConnectable
	virtual Gtk::Requisition GetCpCoord(Elem* aCp = NULL);
	virtual int GetNearestCp(Gtk::Requisition aCoord, Elem*& aCp);
	virtual void HighlightCp(Elem* aCp, bool aSet);
    protected:
	Elem* iElem;
	TPos iPos;
	bool iHighlighted;
};


// Socket Erp
class SockErp: public VBox, public MErp, public MErpMgr, public MErpConnectable
{
    public:
	static const string& Type();
	static string EType();
    public:
	SockErp(Elem* aElem, const MErpProvider& aErpProv);
	virtual ~SockErp();
    protected:
	void DoSetHighlighted(bool aSet);
    protected:
	// From MErp
	virtual Gtk::Widget& Widget();
	virtual void *DoGetObj(const string& aName);
	virtual void SetHighlighted(bool aSet);
	virtual Elem* Model();
	virtual void SetPos(TPos aPos);
	// From MErpMgr
	virtual bool IsTypeAllowed(const std::string& aType) const;
	// From MErpConnectable
	virtual Gtk::Requisition GetCpCoord(Elem* aCp = NULL);
	virtual int GetNearestCp(Gtk::Requisition aCoord, Elem*& aCp);
	virtual void HighlightCp(Elem* aCp, bool aSet);
    protected:
	Elem* iElem;
	TPos iPos;
	bool iHighlighted;
	Label* iName;
	const MErpProvider& iErpProv;
	typedef std::map<Elem*, MErp*> tPinRps;
	tPinRps iPinRps; // Pins representations
};


#endif

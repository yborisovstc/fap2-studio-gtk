
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
	static string IfaceSupported();
    public:
	CpErp(MElem* aElem);
	virtual ~CpErp();
    protected:
	void DoSetHighlighted(bool aSet);
    protected:
	// From MErp
	virtual Gtk::Widget& Widget();
	virtual void *DoGetObj(const string& aName);
	virtual void SetHighlighted(bool aSet);
	virtual MElem* Model();
	virtual void SetPos(TPos aPos);
	virtual TPos GetPos() const;
	virtual MCompatChecker::TDir GetMdlDir() const;
	// From MErpConnectable
	virtual Gtk::Requisition GetCpCoord(MElem* aCp = NULL);
	virtual int GetNearestCp(Gtk::Requisition aCoord, MElem*& aCp);
	virtual void HighlightCp(MElem* aCp, bool aSet);
    protected:
	MElem* iElem;
	TPos iPos;
	bool iHighlighted;
};


// Socket Erp
class SockErp: public VBox, public MErp, public MErpMgr, public MErpConnectable
{
    public:
	static const string& Type();
	static string EType();
	static string IfaceSupported();
    public:
	SockErp(MElem* aElem, const MErpProvider& aErpProv);
	virtual ~SockErp();
    protected:
	void DoSetHighlighted(bool aSet);
    protected:
	// From MErp
	virtual Gtk::Widget& Widget();
	virtual void *DoGetObj(const string& aName);
	virtual void SetHighlighted(bool aSet);
	virtual MElem* Model();
	virtual void SetPos(TPos aPos);
	virtual TPos GetPos() const;
	virtual MCompatChecker::TDir GetMdlDir() const;
	// From MErpMgr
	virtual bool IsTypeAllowed(const std::string& aType) const;
	// From MErpConnectable
	virtual Gtk::Requisition GetCpCoord(MElem* aCp = NULL);
	virtual int GetNearestCp(Gtk::Requisition aCoord, MElem*& aCp);
	virtual void HighlightCp(MElem* aCp, bool aSet);
    protected:
	MElem* iElem;
	TPos iPos;
	bool iHighlighted;
	Label* iName;
	const MErpProvider& iErpProv;
	typedef std::map<MElem*, MErp*> tPinRps;
	tPinRps iPinRps; // Pins representations
};


#endif

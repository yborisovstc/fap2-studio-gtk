
#ifndef __FAP2STU_SYSCRP_H
#define __FAP2STU_SYSCRP_H

#include <gtkmm/layout.h>
#include <gtkmm/box.h>
#include "gtkmm/label.h"
#include <elem.h>
#include <mvert.h>
#include "mcrp.h"
#include "mdesobs.h"
#include "vertcrp.h"

using namespace std;
using namespace Gtk;

//  Connection point representation
class CpRp: public Gtk::Label
{
    public:
	CpRp(MElem* aCp);
	virtual ~CpRp();
	MCompatChecker::TDir GetDir() const;
    protected:
	// From Layout
	virtual bool on_expose_event(GdkEventExpose* event);
	virtual void on_size_allocate(Gtk::Allocation& 	aAlloc);
	virtual void on_size_request(Gtk::Requisition* aRequisition);
    protected:
	MElem* iElem;
};

//  Data representation.
class DataRp: public Gtk::Label
{
    public:
	DataRp(MElem* aModel, const string& aDataName, MMdlObserver* aMdlObs);
    protected:
	// Model events handlers
	void on_comp_changed(MElem* aComp);
    protected:
	MElem* iElem;
	string iDataName;
	MMdlObserver* iMdlObs;
};


// System representation
class SysCrp: public VertCompRp
{
    public:
	static const string& Type();
	static string EType();
    public:
	SysCrp(MElem* aElem, MMdlObserver* aMdlObs, const string& aDataUri = string());
	virtual ~SysCrp();
    protected:
	virtual void Construct();
	// From MCrp
	virtual Gtk::Widget& Widget();
	virtual void *DoGetObj(const string& aName);
	virtual void GetModelDebugInfo(int x, int y, string& aData) const;
	// From MCrpConnectable
	virtual Gtk::Requisition GetCpCoord(MElem* aCp = NULL);
	virtual int GetNearestCp(Gtk::Requisition aCoord, MElem*& aCp);
	virtual void HighlightCp(MElem* aCp, bool aSet);
	// From Layout
	virtual bool on_expose_event(GdkEventExpose* event);
	virtual void on_size_allocate(Gtk::Allocation& 	aAlloc);
	virtual void on_size_request(Gtk::Requisition* aRequisition);
    protected:
	void AddDataRp();
	Gtk::Allocation GetCpAlloc(MElem* aCp);
	MElem* GetHoweredCp(Gtk::Requisition aCoord) const;
    protected:
	MMdlObserver* iMdlObs;
	typedef std::map<MElem*, CpRp*> tCpRps;
	typedef std::map<MElem*, DataRp*> tDataRps;
	tCpRps iCpRps; // CPs representations
	string iDataUri;
	tDataRps iDataRps;
};

#endif


#ifndef __FAP2STU_SYSDRP_H
#define __FAP2STU_SYSDRP_H

#include "vertdrp.h"

using namespace Gtk;
#if 0
class SysDrp: public VertDrpw_v1
{
    public:
	static std::string Type() { return string("SysDrp");};
    public:
	static string EType();
    public:
	SysDrp(Elem* aElem, const MCrpProvider& aCrpProv);
	virtual ~SysDrp();
    protected:
	virtual void Construct();
	virtual bool AreCpsCompatible(Elem* aCp1, Elem* aCp2);
	// From MCrpMgr
	virtual bool IsTypeAllowed(const std::string& aType) const;
	// From MDrp
	virtual void *DoGetObj(const string& aName);
	virtual Gtk::Widget& Widget();
	virtual Elem* Model();
	virtual tSigCompSelected SignalCompSelected();
	//  From VertDrpw
	virtual void on_size_allocate(Gtk::Allocation& aAlloc);
	virtual void on_size_request(Gtk::Requisition* aRequisition);
    protected:
	static const string KCpEType;
	static const string KExtdEType;
};
#endif

// System DRP with boundary direction support
class SysDrp: public VertDrpw_v1
{
    private:
	// Edge vertical tunnel info: tunnel ind, left
	typedef pair<int, bool> TEvtInfo;
    public:
	// Parameters of layout of areas
	typedef vector<MCrp*> TVectCrps;
	typedef pair<Allocation, TVectCrps> TLAreaPar;
	typedef vector<TLAreaPar> TLAreasPars;
    public:
	static std::string Type() { return string("SysDrp");};
	static string EType();
	SysDrp(Elem* aElem, const MCrpProvider& aCrpProv);
	virtual ~SysDrp();
    protected:
	virtual void Construct();
	virtual bool AreCpsCompatible(Elem* aCp1, Elem* aCp2);
	TEvtInfo GetEvtInfo(Requisition aCoord);
	int GetEvtWidth() const;
	int GetEvtLineX(MCrp* aEdge, int aTunnel, int aP1, int aP2);
	int GetEvtX(int aTnlCnt) const;
	static bool AreIntervalsIntersecting(int aA1, int aA2, int aB1, int aB2);
	void GetEdgeAlloc(MEdgeCrp* aEdge, Allocation& aAlloc);
	int GetEhtLine(MCrp* aEdge, int aEvt, int aY, bool aUp) const;
	int GetEvtEnty(int aEvtId) const;
	// From MCrpMgr
	virtual bool IsTypeAllowed(const std::string& aType) const;
	// From MDrp
	virtual void *DoGetObj(const string& aName);
	//  From VertDrpw
	virtual void on_size_allocate(Gtk::Allocation& aAlloc);
	virtual void on_size_request(Gtk::Requisition* aRequisition);
    protected:
	static const string KCpEType;
	static const string KExtdEType;
	// Edges tunnel capacity (number separated edges allowed)
	static const int KETnlCap;
	TLAreasPars iLaPars;
};

#endif

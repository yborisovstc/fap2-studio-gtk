
#ifndef __FAP2STU_SYSDRP_H
#define __FAP2STU_SYSDRP_H

#include "vertdrp.h"
#include <map>

using namespace std;
using namespace Gtk;


// System DRP with boundary direction support
class SysDrp: public VertDrpw_v1
{
    public:
	// Layout area CRPs comparator 
	struct Cmp {
	    Cmp(SysDrp& aHost): mHost(aHost) {};
	    bool operator() (MCrp*& aA, MCrp*& aB) const;
	    SysDrp& mHost;
	};
	struct CmpMain {
	    CmpMain(SysDrp& aHost): mHost(aHost) {};
	    bool operator() (MCrp*& aA, MCrp*& aB) const;
	    SysDrp& mHost;
	};
    private:
	// Edge vertical tunnel info: tunnel ind, left
	typedef pair<int, bool> TEvtInfo;
    public:
	// Parameters of layout of areas
	typedef list<MCrp*> TVectCrps;
	typedef pair<Allocation, TVectCrps> TLAreaPar;
	typedef vector<TLAreaPar> TLAreasPars;
	// CRPs relation: related CRP
	typedef pair <MCrp*, MCrp*> TRpRel;
	// CRPs relation metric: num of relation
	typedef pair <TRpRel, int> TRpRelm;
	// CRPs relations
	typedef std::map<TRpRel, int> TRpRelms;
	typedef multimap<MCrp*, MCrp*> TRpRels;
    public:
	static std::string Type() { return string("SysDrp");};
	static string EType();
	SysDrp(Elem* aElem, const MCrpProvider& aCrpProv);
	virtual ~SysDrp();
    protected:
	virtual void Construct();
	virtual bool AreCpsCompatible(Elem* aCp1, Elem* aCp2);
	TEvtInfo GetEvtInfo(Requisition aCoord);
	static int GetEvtMinWidth();
	static int GetAvzMinGapWidth();
	int GetEvtLineX(MCrp* aEdge, int aTunnel, int aP1, int aP2, bool aFromRight = false);
	int GetEvtX(int aTnlCnt) const;
	int GetEvtEnd(int aTnlCnt) const;
	static bool AreIntervalsIntersecting(int aA1, int aA2, int aB1, int aB2);
	void GetEdgeAlloc(MEdgeCrp* aEdge, Allocation& aAlloc);
	int GetEhtLine(MCrp* aEdge, int aEvt, int aY, bool aUp) const;
	void UpdateRpsRelatios();
	int GetCrpRelsCount(MCrp* aCrp) const;
	void PreLayoutRps();
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
	// CRPs relations, used for layouting
	TRpRels iRpRels;
	TRpRelms iRpRelms;
};

#endif


#include <syst.h>

#include <limits>
#include <iostream>
#include "common.h"
#include "sysdrp.h"


// System DRP with boundary direction support

const string SysDrp::KCpEType = "Elem:Vert:ConnPointBase:ConnPoint";
const string SysDrp::KExtdEType = "Elem:Vert:Extender";
const int SysDrp::KETnlCap = 5;

bool SysDrp::Cmp::operator() (MCrp*& aA, MCrp*& aB) const
{
    bool res = false;
    res = mHost.GetCrpRelsCount(aA) > mHost.GetCrpRelsCount(aB);
    return res;
}

bool SysDrp::CmpMain::operator() (MCrp*& aA, MCrp*& aB) const
{
    bool res = false;
    res = mHost.GetCrpRelsCount(aA) > mHost.GetCrpRelsCount(aB);
    return res;
}

string SysDrp::EType()
{
    return ":Elem:Vert:Syst";
}

SysDrp::SysDrp(Elem* aElem, const MCrpProvider& aCrpProv): VertDrpw_v1(aElem, aCrpProv)
{
}

SysDrp::~SysDrp()
{
}

int SysDrp::GetCrpRelsCount(MCrp* aCrp) const
{
    return  iRpRels.count(aCrp);
}

void SysDrp::PreLayoutRps()
{
    // Pre-layout the components
    // There are two phase of pre-layouting: allocating to areas, ordering in areas
    // Scheme of areas is selected by the user, the default one is 3-areas scheme: inp, body, out
    iLaPars.resize(3);
    for (TLAreasPars::iterator zit = iLaPars.begin(); zit != iLaPars.end(); zit++) {
	zit->first = Allocation();
	zit->second.clear(); 
    }
    // Phase 1: allocating RPs to areas
    for (tCrps::const_iterator it = iCompRps.begin(); it != iCompRps.end(); it++) {
	MCrp* crp = it->second;
	MEdgeCrp* ecrp = crp->GetObj(ecrp);
	if (ecrp == NULL) {
	    Elem* mdl = crp->Model();
	    MCompatChecker* cc = mdl->GetObj(cc);
	    int narea = 1;
	    MCrp::TLArea larea = MCrp::EMain;
	    if (cc != NULL) {
		MCompatChecker::TDir dir =  cc->GetDir();
		narea = dir == MCompatChecker::EInp ? 0 : 2;
		larea = dir == MCompatChecker::EInp ? MCrp::ELeft : MCrp::ERight;
	    }
	    TLAreaPar& area = iLaPars.at(narea);
	    crp->SetLArea(larea);
	    area.second.push_back(crp);
	}
    }
    // Sorting out area
    Cmp cmp(*this);
    TVectCrps& crps_out = iLaPars.at(2).second;;
    crps_out.sort(cmp);
    CmpMain cmp_main(*this);
    TVectCrps& crps_main = iLaPars.at(1).second;;
    crps_main.sort(cmp_main);
}

void SysDrp::Construct()
{
    VertDrpw_v1::Construct();
    UpdateRpsRelatios();
    PreLayoutRps();
}

void SysDrp::UpdateRpsRelatios()
{
    iRpRels.clear();
    iRpRelms.clear();
    for (tCrps::const_iterator it = iCompRps.begin(); it != iCompRps.end(); it++) {
	MCrp* crp = it->second;
	MEdgeCrp* ecrp = crp->GetObj(ecrp);
	if (ecrp != NULL) {
	    Elem* p1 = ecrp->Point1();
	    Elem* p2 = ecrp->Point2();
	    p1 = p1 != NULL ? GetCompOwning(p1): NULL;
	    p2 = p2 != NULL ? GetCompOwning(p2): NULL;
	    if (p1 != NULL && p2 != NULL) {
		MCrp* rp1 = iCompRps.at(p1);
		MCrp* rp2 = iCompRps.at(p2);
		if (iRpRelms.count(TRpRel(rp1, rp2)) != 0) {
		    // Relation already registered, just update
		    int& cnt = iRpRelms.at(TRpRel(rp1, rp2));
		    cnt++;
		    int& cnti = iRpRelms.at(TRpRel(rp2, rp1));
		    cnti++;
		}
		else {
		    // Relation is not registered, add it
		    iRpRels.insert(TRpRel(rp1, rp2));
		    iRpRelms.insert(TRpRelm(TRpRel(rp1, rp2),1));
		    iRpRels.insert(TRpRel(rp2, rp1));
		    iRpRelms.insert(TRpRelm(TRpRel(rp2, rp1),1));
		}
	    }
	}
    }
}

void *SysDrp::DoGetObj(const string& aName)
{
    void* res = NULL;
    if (aName ==  Type()) {
	res = this;
    }
    if (aName ==  MDrp::Type()) {
	res = (MDrp*) this;
    }
    return res;
}

bool SysDrp::IsTypeAllowed(const std::string& aType) const
{
    bool res = false;
    if (aType == KExtdEType || aType == KCpEType || aType == Syst::PEType() || VertDrpw_v1::IsTypeAllowed(aType)) {
	res = true;
    }
    return res;
}

void SysDrp::on_size_allocate(Gtk::Allocation& aAllc)
{
    set_allocation(aAllc);
    if (get_realized()) {
	get_window()->move_resize(aAllc.get_x(), aAllc.get_y(), aAllc.get_width(), aAllc.get_height());
	get_bin_window()->resize(aAllc.get_width(), aAllc.get_height());
    }

    // Allocate components excluding edges
    int cur_x = KBoundCompGapWidth;
    for (TLAreasPars::iterator zit = iLaPars.begin(); zit != iLaPars.end(); zit++) {
	Allocation& zal = zit->first;
	zal.set_x(cur_x); zal.set_y(KViewCompGapHight);
	int compb_x = cur_x + zal.get_width()/2, compb_y = zal.get_y();
	TVectCrps& col = zit->second; 
	for (TVectCrps::iterator it = col.begin(); it != col.end(); it++) {
	    MCrp* crp = *it;
	    Gtk::Widget* comp = &(crp->Widget());
	    Gtk::Requisition req = comp->size_request();
	    int compb_half_w = req.width / 2;
	    Allocation allc = Gtk::Allocation(compb_x - compb_half_w, compb_y, req.width, req.height);
	    compb_y += req.height + KViewCompGapHight;
	    comp->size_allocate(allc);
	}
	cur_x += zal.get_width() + GetAvzMinGapWidth();
    }

    // Allocate edges
    bool relayout_required = false;
    Allocation& avz0_alc = iLaPars.begin()->first;
    int edge_park_x = avz0_alc.get_x() + avz0_alc.get_width();
    int edge_park_y = KViewCompGapHight/2;
    for (std::map<Elem*, MCrp*>::iterator it = iCompRps.begin(); it != iCompRps.end(); it++) {
	MCrp* crp = it->second;
	Gtk::Widget* comp = &(crp->Widget());
	MEdgeCrp* medgecrp = crp->GetObj(medgecrp);
	Allocation allc;
	if (medgecrp != NULL) {
	    // Set the points
	    MEdgeCrp* medgecrp1 = crp->GetObj(medgecrp1);
	    Gtk::Widget* comp = &(crp->Widget());
	    Gtk::Requisition req = comp->size_request();
	    Elem* p1 = medgecrp->Point1();
	    Elem* p2 = medgecrp->Point2();
	    Elem* op1 = p1 != NULL ? GetCompOwning(p1): NULL;
	    Elem* op2 = p2 != NULL ? GetCompOwning(p2): NULL;

	    Gtk::Requisition p1coord = medgecrp->Cp1Coord();
	    Gtk::Requisition p2coord = medgecrp->Cp2Coord();
	    if (p1 != NULL) {
		MCrp* pcrp = iCompRps.at(op1);
		MCrpConnectable* pcrpcbl = pcrp->GetObj(pcrpcbl);
		assert(pcrpcbl != NULL);
		p1coord = pcrpcbl->GetCpCoord(p1);
		medgecrp->SetCp1Coord(p1coord);
	    }
	    else if (!crp->Dragging()) {
		p1coord.width = edge_park_x;
		p1coord.height =  edge_park_y;
		medgecrp->SetCp1Coord(p1coord);
	    }

	    if (p2 != NULL) {
		MCrp* pcrp = iCompRps.at(op2);
		MCrpConnectable* pcrpcbl = pcrp->GetObj(pcrpcbl);
		assert(pcrpcbl != NULL);
		p2coord = pcrpcbl->GetCpCoord(p2);
		medgecrp->SetCp2Coord(p2coord);
	    }
	    else if (!crp->Dragging()) {
		p2coord.width = edge_park_x + KEdgeGridCell;
		p2coord.height =  edge_park_y;
		medgecrp->SetCp2Coord(p2coord);
	    }
	    // Trace the edge, calculating the edges nodes, from left to right
	    // Tracing step is from entry of the current vert tunnel till the entry to next one (Z form)
	    bool done = false;
	    Requisition start = p1coord.width < p2coord.width ? p1coord : p2coord;
	    Requisition end = p1coord.width < p2coord.width ? p2coord : p1coord;
	    MEdgeCrp::TVectEn& ven = medgecrp->VectEn();
	    ven.clear();
	    ven.push_back(start);
	    Requisition cur = start;
	    TEvtInfo evi_e = GetEvtInfo(end);
	    while (!done) {
		TEvtInfo evi_c = GetEvtInfo(cur);
		int fy, fx; // Tracing step final point
		if (evi_c.first == evi_e.first) {
		    fx = end.width;
		    fy = end.height;
		    done = true;
		}
		else {
		    // Final vertical tunnel hasn't been achieved yet
		    // Find nearest horisontal tunnel available line
		    fy = GetEhtLine(crp, evi_c.first, cur.height, end.height < cur.height);
		    if (fy < 0) {
			relayout_required = true;
			break;
		    }
		    // Get next vertical tunnel entry
		    fx = GetEvtEnty(evi_c.first + 1);
		}
		// Find available line within the vertical tunnel
		Requisition n3 = {fx, fy};
		TEvtInfo evi_n3 = GetEvtInfo(n3);
		bool from_right = (evi_c.first == evi_n3.first) && !evi_c.second && !evi_n3.second || 
		    (evi_c.first != evi_n3.first) && !evi_c.second;
		int lx = GetEvtLineX(crp, evi_c.first, cur.height, fy, from_right);
		Requisition n1 = {lx, cur.height};
		Requisition n2 = {lx, fy};
		ven.push_back(n1);
		ven.push_back(n2);
		ven.push_back(n3);
		cur.width = fx; cur.height = fy;
	    }
	    if (relayout_required) {
		break;
	    }
	    // Allocate the edges widget
	    GetEdgeAlloc(medgecrp, allc);
	    comp->size_allocate(allc);
	}
    }
}

void SysDrp::GetEdgeAlloc(MEdgeCrp* aEdge, Allocation& aAlloc)
{
    MEdgeCrp::TVectEn& nodes = aEdge->VectEn();
    int left = std::numeric_limits<int>::max(), top = left, right = 0, bottom = 0;
    if (nodes.size() >= 2) {
	for (MEdgeCrp::TVectEn::iterator it = nodes.begin(); it != nodes.end(); it++) {
	    Requisition rr = *it;
	    int x = rr.width;
	    int y = rr.height;
	    left = min(left, x);
	    right = max(right, x);
	    top = min(top, y);
	    bottom = max(bottom, y);
	}
	aAlloc.set_x(left); 
	aAlloc.set_y(top); 
	aAlloc.set_width(right - left + 1);
	aAlloc.set_height(bottom - top + 1);
    }
}

int SysDrp::GetEvtWidth()
{
    return KETnlCap*KEdgeGridCell;
}

int SysDrp::GetAvzMinGapWidth()
{
    return 2* KEvtGap + GetEvtWidth();
}

int SysDrp::GetEvtEnty(int aEvtId) const
{
    int res = 0;
    int tw = GetEvtWidth();
    for (int cnt = 0; cnt < aEvtId; cnt++) {
	const Allocation& zal = iLaPars.at(cnt).first;
	res += (cnt == 0 ? 0 : tw) + zal.get_width();
    }
    return res;
}

SysDrp::TEvtInfo SysDrp::GetEvtInfo(Requisition aCoord)
{
    TEvtInfo res(iLaPars.size() - 1, true);
    int tw = GetAvzMinGapWidth();
    int x = aCoord.width;
    int cx = 0;
    for (int cnt = 0; cnt < iLaPars.size(); cnt++) {
	Allocation& zal = iLaPars.at(cnt).first;
	int zw = zal.get_width();
	if (x < cx + zw/2) {
	    res.first = cnt;
	    res.second = x < (cx - tw/2);
	    break;
	}
	cx += zw + tw;
    }
    return res;
}

int SysDrp::GetEvtX(int aTnlCnt) const
{
    __ASSERT(aTnlCnt <= iLaPars.size());
    int res = 0;
    int tw = GetAvzMinGapWidth();
    for (int cnt = 0; cnt < aTnlCnt; cnt++) {
	const Allocation& zal = iLaPars.at(cnt).first;
	int zw = zal.get_width();
	res += zw + (cnt == 0 ? 0: tw);
    }
    return res + KEvtGap;
}

bool SysDrp::AreIntervalsIntersecting(int aA1, int aA2, int aB1, int aB2)
{
    int al = min(aA1, aA2);
    int am = max(aA1, aA2);
    int bl = min(aB1, aB2);
    int bm = max(aB1, aB2);
    return !(bm < al || bl > am);
}

int SysDrp::GetEvtLineX(MCrp* aEdge, int aTunnel, int aP1, int aP2, bool aFromRight) 
{
    int lx_base = GetEvtX(aTunnel) + (aFromRight ? GetEvtWidth() : 0);
    int lx = lx_base;
    bool isec = false;
    int ptop = min(aP1, aP2);
    int pbottom = max(aP1, aP2);
    for (int lidi = 0; lidi < KETnlCap; lidi++) {
	int lid = aFromRight ? KETnlCap-lidi-1 : lidi;
	Requisition top = {lx, ptop};
	Requisition bottom = {lx, pbottom};
	isec = false;
	for (std::map<Elem*, MCrp*>::iterator it = iCompRps.begin(); it != iCompRps.end() && !isec; it++) {
	    MCrp* crp = it->second;
	    MEdgeCrp* medgecrp = crp->GetObj(medgecrp);
	    if (medgecrp != NULL && crp != aEdge) {
		MEdgeCrp::TVectEn& ven = medgecrp->VectEn();
		Requisition p1 = {0, 0};
		Requisition p2 = {0, 0};
		for (MEdgeCrp::TVectEn::iterator nit = ven.begin(); nit != ven.end() && !isec; nit++) {
		    p1 = p2;
		    p2 = *nit;
		    isec = p1.width == lx && p2.width == lx && AreIntervalsIntersecting(ptop, pbottom, p1.height, p2.height);
		}
	    }
	}
	if (!isec) 
	    break;
	lx += aFromRight ? (-KEdgeGridCell) : KEdgeGridCell;
    }
    if (isec) {
	lx = lx_base + 1;
    }
    return lx;
}

int SysDrp::GetEhtLine(MCrp* aEdge, int aEvt, int aY, bool aUp) const
{
    int res = -1; 
    // Find base first
    int base = aUp ? 0: std::numeric_limits<int>::max();
    const TVectCrps& crps = iLaPars.at(aEvt).second;
    TVectCrps::const_iterator rpit = crps.begin();
    for (; rpit != crps.end(); rpit++) {
	MCrp* crp = *rpit;
	Allocation alc = crp->Widget().get_allocation();
	base = aUp ? alc.get_y(): alc.get_y() + alc.get_height();
	if (aUp && base + alc.get_height() >= aY || !aUp && base >= aY)
	    break;
    }
    // Find available line
    int ly = base + (aUp ? (-KEdgeGridCell) : KEdgeGridCell);
    bool isec = false;
    int tnlcap = KViewCompGapHight / KEdgeGridCell - 1;
    for (int lid = 0; lid < tnlcap; lid++) {
	isec = false;
	for (std::map<Elem*, MCrp*>::const_iterator it = iCompRps.begin(); it != iCompRps.end() && !isec; it++) {
	    MCrp* crp = it->second;
	    MEdgeCrp* medgecrp = crp->GetObj(medgecrp);
	    if (medgecrp != NULL && crp != aEdge) {
		MEdgeCrp::TVectEn& ven = medgecrp->VectEn();
		Requisition p1 = {0, 0};
		Requisition p2 = {0, 0};
		for (MEdgeCrp::TVectEn::iterator nit = ven.begin(); nit != ven.end() && !isec; nit++) {
		    p1 = p2;
		    p2 = *nit;
		    isec = p1.height == ly && p2.height == ly;
		}
	    }
	}
	if (!isec) 
	    break;
	ly += (aUp ? (-KEdgeGridCell) : KEdgeGridCell);
    }
    if (isec) {
	// TODO [YB] To implement dynamic increasing of horiz tunnel
	// Shift CRPs to provide the additional line
	/*
	for (; rpit != crps.end(); rpit++) {
	    MCrp* crp = *rpit;
	    Allocation alc = crp->Widget().get_allocation();
	    alc.set_y(alc.get_y() + KEdgeGridCell);
	    crp->Widget().size_allocate(alc);
	}
	res = -1;
	*/
	ly = base + (aUp ? (-KEdgeGridCell-1) : KEdgeGridCell+1);
	res = ly;
    }
    else {
	res = ly;
    }
    return res;
}

void SysDrp::on_size_request(Gtk::Requisition* aRequisition)
{
    aRequisition->width = iLaPars.size() * GetAvzMinGapWidth();
    aRequisition->height = 0;

    for (TLAreasPars::iterator ait = iLaPars.begin(); ait != iLaPars.end(); ait++) {
	Allocation& alc = ait->first;
	alc = Allocation();
	TVectCrps& rps = ait->second;
	for (TVectCrps::iterator it = rps.begin(); it != rps.end(); it++) {
	    MCrp* crp = *it;
	    Gtk::Widget* comp = &(crp->Widget());
	    Gtk::Requisition req = comp->size_request();
	    alc.set_width(max(alc.get_width(), req.width));
	    alc.set_height(alc.get_height() + req.height + KViewCompGapHight);
	}
	aRequisition->width += alc.get_width();
	aRequisition->height = max(aRequisition->height, alc.get_height());
    }
}

bool SysDrp::AreCpsCompatible(Elem* aCp1, Elem* aCp2)
{
    bool res = false;
    MVert* v1 = aCp1->GetObj(v1);
    MVert* v2 = aCp2->GetObj(v2);
    if (v1 != NULL && v2 != NULL) {
	MCompatChecker* c1 = aCp1->GetObj(c1);
	MCompatChecker* c2 = aCp2->GetObj(c2);
	res = (c1 == NULL || c1->IsCompatible(aCp2)) && (c2 == NULL || c2->IsCompatible(aCp1));
    }
    //std::cout << "SysDrp::AreCpsCompatible, aCp1: " << aCp1->Name() << ", aCp2: " << aCp2->Name() << ", res: " << res << std::endl;
    return res;
}



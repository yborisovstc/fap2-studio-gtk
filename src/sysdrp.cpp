
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

SysDrp::SysDrp(MElem* aElem, const MCrpProvider& aCrpProv, MSEnv& aStEnv): VertDrpw_v1(aElem, aCrpProv, aStEnv)
{
    iLaNum = 2;
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
    iLaPars.resize(iLaNum);
    for (TLAreasPars::iterator zit = iLaPars.begin(); zit != iLaPars.end(); zit++) {
	zit->first = Allocation();
	zit->second.clear(); 
    }
    // Phase 1: allocating RPs to areas
    // Inputs and outputs
    for (tCrps::const_iterator it = iCompRps.begin(); it != iCompRps.end(); it++) {
	MCrp* crp = it->second;
	MEdgeCrp* ecrp = crp->GetObj(ecrp);
	if (ecrp == NULL) {
	    MElem* mdl = crp->Model();
	    MCompatChecker* cc = mdl->GetObj(cc);
	    MCrp::TLArea larea = MCrp::EMain;
	    if (cc != NULL) {
		MCompatChecker::TDir dir =  cc->GetDir();
		int narea = dir == MCompatChecker::EInp ? 0 : iLaNum - 1;
		larea = dir == MCompatChecker::EInp ? MCrp::ELeft : MCrp::ERight;
		TLAreaPar& area = iLaPars.at(narea);
		crp->SetLArea(larea);
		area.second.push_back(crp);
	    }
	}
    }
    // Sorting inp/out area
    Cmp cmp(*this);
    TVectCrps& crps_out = iLaPars.at(iLaNum - 1).second;;
    crps_out.sort(cmp);
    // Body
    BodyPreLayoutAut();
}

void SysDrp::BodyPreLayoutAut() 
{
    // Areas
    CmpMain cmp_main(*this);
    int ia = 1; 
    while (AreUnallocRpsRelToArea(ia)) {
	TLAreaPar area;
	for (tCrps::const_iterator it = iCompRps.begin(); it != iCompRps.end(); it++) {
	    MCrp* crp = it->second;
	    MEdgeCrp* ecrp = crp->GetObj(ecrp);
	    if (ecrp == NULL && crp->GetLArea() == MCrp::EUnknown) {
		MCompatChecker* cc = crp->Model()->GetObj(cc);
		if (cc == NULL) {
		    if (HasRelToArea(crp, ia)) {
			crp->SetLArea(MCrp::EMain);
			area.second.push_back(crp);
		    }
		}
	    }
	}
	TVectCrps& crps = area.second;;
	crps.sort(cmp_main);
	TLAreasPars::iterator ait = iLaPars.begin();
	iLaPars.insert(++ait, area);
    }
    // Adding remaining (not connected) comps to first area
    if (iLaPars.size() == iLaNum) {
	// Main areas still not created - to create
	TLAreaPar area;
	TLAreasPars::iterator ait = iLaPars.begin();
	iLaPars.insert(++ait, area);
    }
    TLAreaPar& area = iLaPars.at(1);
    for (tCrps::const_iterator it = iCompRps.begin(); it != iCompRps.end(); it++) {
	MCrp* crp = it->second;
	MEdgeCrp* ecrp = crp->GetObj(ecrp);
	if (ecrp == NULL && crp->GetLArea() == MCrp::EUnknown) {
	    MCompatChecker* cc = crp->Model()->GetObj(cc);
	    if (cc == NULL) {
		crp->SetLArea(MCrp::EMain);
		area.second.push_back(crp);
	    }
	}
    }
    TVectCrps& crps = area.second;;
    crps.sort(cmp_main);
}

bool SysDrp::HasRelToArea(MCrp* aCrp, int aAreaId) 
{
    bool res = false;
    TVectCrps& crps = iLaPars.at(aAreaId).second;;
    for (TVectCrps::iterator it = crps.begin(); it != crps.end() && !res; it++) {
	MCrp* crp = *it;
	res = iRpRelms.count(TRpRel(aCrp, crp)) > 0;
    }
    return res;
}

bool SysDrp::AreUnallocRpsRelToArea(int aAreaId) 
{
    bool res = false;
    for (tCrps::const_iterator it = iCompRps.begin(); it != iCompRps.end() && !res; it++) {
	MCrp* crp = it->second;
	MEdgeCrp* ecrp = crp->GetObj(ecrp);
	if (ecrp == NULL && crp->GetLArea() == MCrp::EUnknown) {
	    MCompatChecker* cc = crp->Model()->GetObj(cc);
	    res = (cc == NULL && HasRelToArea(crp, aAreaId));
	}
    }
    return res;
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
	    MElem* p1 = ecrp->Point1();
	    MElem* p2 = ecrp->Point2();
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
    //std::cout << "SysDrp::on_size_allocate" << std::endl;
    set_allocation(aAllc);
    if (get_realized()) {
	get_window()->move_resize(aAllc.get_x(), aAllc.get_y(), aAllc.get_width(), aAllc.get_height());
	get_bin_window()->resize(aAllc.get_width(), aAllc.get_height());
    }

    // Calculate vertical tunnel width + gaps
    int areas_w = 0;
    for (TLAreasPars::iterator zit = iLaPars.begin(); zit != iLaPars.end(); zit++) {
	Allocation& zal = zit->first;
	areas_w += zal.get_width();
    }
    int evt_w = (aAllc.get_width() - areas_w - 2*KBoundCompGapWidth) / (iLaPars.size() - 1);

    // Allocate components excluding edges
    AllocateComps(evt_w);

    // Allocate edges
    bool relayout_required = false;
    do {
	relayout_required = false;
	Allocation& avz0_alc = iLaPars.begin()->first;
	int edge_park_x = avz0_alc.get_x() + avz0_alc.get_width();
	int edge_park_ex = edge_park_x + KEvtGap;
	int edge_park_y = KViewCompGapHight/2;
	for (std::map<MElem*, MCrp*>::iterator it = iCompRps.begin(); it != iCompRps.end(); it++) {
	    MCrp* crp = it->second;
	    Gtk::Widget* comp = &(crp->Widget());
	    MEdgeCrp* medgecrp = crp->GetObj(medgecrp);
	    Allocation allc;
	    if (medgecrp != NULL) {
		// Set the points
		MEdgeCrp* medgecrp1 = crp->GetObj(medgecrp1);
		Gtk::Widget* comp = &(crp->Widget());
		Gtk::Requisition req = comp->size_request();
		MElem* p1 = medgecrp->Point1();
		MElem* p2 = medgecrp->Point2();
		MElem* op1 = p1 != NULL ? GetCompOwning(p1): NULL;
		MElem* op2 = p2 != NULL ? GetCompOwning(p2): NULL;

		Gtk::Requisition p1coord = medgecrp->Cp1Coord();
		Gtk::Requisition p2coord = medgecrp->Cp2Coord();
		//std::cout << "SysDrp::on_size_allocate, p1coord: " << p1coord.width << "-" << p1coord.height << ", p2coord: " << p2coord.width << "-" << p2coord.height << std::endl;
		// If not dragging then park edges terminal points 
		if (!crp->Dragging()) {
		if (p1 != NULL && !p1->IsRemoved()) {
		    __ASSERT(iCompRps.count(op1) > 0);
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

		if (p2 != NULL && !p2->IsRemoved()) {
		    __ASSERT(iCompRps.count(op2) > 0);
		    MCrp* pcrp = iCompRps.at(op2);
		    MCrpConnectable* pcrpcbl = pcrp->GetObj(pcrpcbl);
		    assert(pcrpcbl != NULL);
		    p2coord = pcrpcbl->GetCpCoord(p2);
		    medgecrp->SetCp2Coord(p2coord);
		}
		else if (!crp->Dragging()) {
		    p2coord.width = edge_park_ex;
		    p2coord.height =  edge_park_y;
		    medgecrp->SetCp2Coord(p2coord);
		}
		}
		//std::cout << "SysDrp::on_size_allocate, p1coordf: " << p1coord.width << "-" << p1coord.height << ", p2coordf: " << p2coord.width << "-" << p2coord.height << std::endl;
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
			fx = GetEvtX(evi_c.first + 1);
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
    while (relayout_required);
}

void SysDrp::AllocateComps(int aEvtW) 
{
    int evt_w = aEvtW;
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
	cur_x += zal.get_width() + evt_w;
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

int SysDrp::GetEvtMinWidth()
{
    return KETnlCap*KEdgeGridCell;
}

int SysDrp::GetAvzMinGapWidth()
{
    return 2* KEvtGap + GetEvtMinWidth();
}

SysDrp::TEvtInfo SysDrp::GetEvtInfo(Requisition aCoord)
{
    TEvtInfo res(iLaPars.size() - 1, true);
    int x = aCoord.width;
    // Avoiding zone 0, starting from 1. This is because 0 tunnel is disabled
    for (int cnt = 1; cnt < iLaPars.size(); cnt++) {
	Allocation& zal = iLaPars.at(cnt).first;
	int zw = zal.get_width();
	int cx = zal.get_x();
	int tw = 0;
	if (cnt != 0) {
	    Allocation& zal_prev = iLaPars.at(cnt - 1).first;
	    tw = cx - zal_prev.get_x() - zal_prev.get_width();
	}
	if (x < cx + zw/2) {
	    res.first = cnt;
	    res.second = x < (cx - tw/2);
	    break;
	}
    }
    return res;
}

int SysDrp::GetEvtX(int aTnlCnt) const
{
    __ASSERT(aTnlCnt > 0 && aTnlCnt < iLaPars.size());
    const Allocation& zal = iLaPars.at(aTnlCnt - 1).first;
    return  zal.get_x() + zal.get_width() + KEvtGap;
}

int SysDrp::GetEvtEnd(int aTnlCnt) const
{
    __ASSERT(aTnlCnt >= 0 && aTnlCnt < iLaPars.size());
    const Allocation& zal = iLaPars.at(aTnlCnt).first;
    return  zal.get_x() - KEvtGap;
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
    int lx_base = aFromRight ? GetEvtEnd(aTunnel) : GetEvtX(aTunnel);
    int lx = lx_base;
    bool isec = false;
    int ptop = min(aP1, aP2);
    int pbottom = max(aP1, aP2);
    int tnlcap = (GetEvtEnd(aTunnel) - GetEvtX(aTunnel)) / KEdgeGridCell;
    for (int lidi = 0; lidi < tnlcap; lidi++) {
	int lid = aFromRight ? tnlcap-lidi-1 : lidi;
	Requisition top = {lx, ptop};
	Requisition bottom = {lx, pbottom};
	isec = false;
	for (std::map<MElem*, MCrp*>::iterator it = iCompRps.begin(); it != iCompRps.end() && !isec; it++) {
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

int SysDrp::GetEhtLine(MCrp* aEdge, int aEvt, int aY, bool aUp)
{
    int res = 0; 
    // Find base first
    int base = aUp ? 0: std::numeric_limits<int>::max();
    int tnlw = KViewCompGapHight;
    Allocation& aalc = iLaPars.at(aEvt).first;
    Requisition size = size_request();
    const TVectCrps& crps = iLaPars.at(aEvt).second;
    TVectCrps::const_iterator rpit = crps.begin();
    if (rpit == crps.end()) {
	// Empty area, just to stay the same line
	res = aY;
    }
    else {
	for (; rpit != crps.end(); rpit++) {
	    MCrp* crp = *rpit;
	    Allocation alc = crp->Widget().get_allocation();
	    Allocation alc_nxt = alc;
	    TVectCrps::const_iterator itnxt = rpit;
	    if (++itnxt != crps.end()) {
		MCrp* crpnxt = *itnxt;
		alc_nxt = crpnxt->Widget().get_allocation();
	    }
	    else {
		alc_nxt.set_y(aalc.get_y() + aalc.get_height());
		alc_nxt.set_height(size.height);
	    }
	    base = aUp ? alc_nxt.get_y(): alc.get_y() + alc.get_height();
	    if (aUp && base + alc_nxt.get_height() >= aY || !aUp && base >= aY) {
		tnlw = alc_nxt.get_y() - alc.get_y() - alc.get_height();
		break;
	    }
	}
	// Find available line
	int ly = base + (aUp ? (-KEdgeGridCell) : KEdgeGridCell);
	bool isec = false;
	int tnlcap = tnlw / KEdgeGridCell - 1;
	for (int lid = 0; lid < tnlcap; lid++) {
	    isec = false;
	    for (std::map<MElem*, MCrp*>::const_iterator it = iCompRps.begin(); it != iCompRps.end() && !isec; it++) {
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
	    if (tnlw < KViewCompGapHightMax) {
		// There is still possibility to extend the tunnel
		// Shift CRPs to provide the additional line
		for (++rpit; rpit != crps.end(); rpit++) {
		    MCrp* crp = *rpit;
		    Allocation alc = crp->Widget().get_allocation();
		    alc.set_y(alc.get_y() + KEdgeGridCell);
		    crp->Widget().size_allocate(alc);
		}
		aalc.set_height(aalc.get_height() + KEdgeGridCell);
		res = -1;
	    }
	    else {
		// Tunnel is of max size, to squeeze the line
		ly = base + (aUp ? (-KEdgeGridCell-1) : KEdgeGridCell+1);
		res = ly;
	    }
	}
	else {
	    res = ly;
	}
    }
    return res;
}

void SysDrp::on_size_request(Gtk::Requisition* aRequisition)
{
    aRequisition->width = (iLaPars.size() - 1) * GetAvzMinGapWidth() + 2*KBoundCompGapWidth;
    aRequisition->height = 0;

    // Calculate size of components
    for (TLAreasPars::iterator ait = iLaPars.begin(); ait != iLaPars.end(); ait++) {
	Allocation& alc = ait->first;
	alc = Allocation();
	TVectCrps& rps = ait->second;
	int area_w = 0, area_h = 0;
	for (TVectCrps::iterator it = rps.begin(); it != rps.end(); it++) {
	    MCrp* crp = *it;
	    Gtk::Widget* comp = &(crp->Widget());
	    Gtk::Requisition req = comp->size_request();
	    area_w = max(area_w, req.width);
	    area_h += req.height + KViewCompGapHight;
	}
	area_w = max(area_w, KLAreaMinWidth);
	alc.set_width(area_w);
	area_h = max(area_h, KLAreaMinHeight);
	alc.set_height(area_h);

	// Calculate the potential height of edges if they are layouting into the horiz tunnels
	// TODO [YB] This is temporarily solution only. To consider solid solution instead:
	// one variant is to do pre-allocation including edges (this will do correction in comp areas height)
	// in Construct and then calculate size basing on allocation data
	int edges_h = 0;
	for (std::map<MElem*, MCrp*>::iterator it = iCompRps.begin(); it != iCompRps.end(); it++) {
	    MCrp* crp = it->second;
	    Gtk::Widget* comp = &(crp->Widget());
	    MEdgeCrp* medgecrp = crp->GetObj(medgecrp);
	    if (medgecrp != NULL) {
		edges_h += KEdgeGridCell;
	    }
	}
	aRequisition->width += area_w;
	aRequisition->height = max(aRequisition->height, area_h + edges_h);
    }
}

bool SysDrp::AreCpsCompatible(MElem* aCp1, MElem* aCp2)
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



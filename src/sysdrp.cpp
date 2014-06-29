
#include <syst.h>

#include <limits>
#include <iostream>
#include "common.h"
#include "sysdrp.h"

#if 0
const string SysDrp::KCpEType = "Elem:Vert:ConnPointBase:ConnPoint";
const string SysDrp::KExtdEType = "Elem:Vert:Extender";

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

void SysDrp::Construct()
{
    VertDrpw_v1::Construct();
    // Mark all CPs as boundary
    for (std::vector<Elem*>::iterator it = iElem->Comps().begin(); it != iElem->Comps().end(); it++) {
	Elem* ecmp = *it;
	if (ecmp->IsHeirOf(KCpEType)) {
	    assert(iCompRps.count(ecmp) > 0);
	    MCrp* crp = iCompRps.at(ecmp);
	    MCrpConnectable* crpc = crp->GetObj(crpc);
	    if (crpc != NULL) {
		crpc->SetIsInt(false);
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

Gtk::Widget& SysDrp::Widget()
{
    return *(static_cast<Gtk::Widget*>(this));
}

Elem* SysDrp::Model()
{
    return iElem;
}

bool SysDrp::IsTypeAllowed(const std::string& aType) const
{
    bool res = false;
    if (aType == KExtdEType || aType == KCpEType || aType == Syst::PEType() || VertDrpw_v1::IsTypeAllowed(aType)) {
	res = true;
    }
    return res;
}

MDrp::tSigCompSelected SysDrp::SignalCompSelected()
{
    return iSigCompSelected;
}

void SysDrp::on_size_allocate(Gtk::Allocation& aAllc)
{
    set_allocation(aAllc);
    if (get_realized()) {
	get_window()->move_resize(aAllc.get_x(), aAllc.get_y(), aAllc.get_width(), aAllc.get_height());
	get_bin_window()->resize(aAllc.get_width(), aAllc.get_height());
    }

    // Allocate components excluding edges
    // Keeping components order, so using elems register of comps
    int compb_x = aAllc.get_width()/2, compb_y = KViewCompGapHight;
    int bcompb_x = aAllc.get_width() - KBoundCompGapWidth , bcompb_y = KViewCompGapHight;
    int comps_w_max = 0;
    int bcomps_w_max = 0;
    for (std::vector<Elem*>::iterator it = iElem->Comps().begin(); it != iElem->Comps().end(); it++) {
	Elem* ecmp = *it;
	MCrp* crp = iCompRps.at(ecmp);
	MEdgeCrp* medgecrp = crp->GetObj(medgecrp);
	if (medgecrp == NULL) {
	    Gtk::Widget* comp = &(crp->Widget());
	    Gtk::Requisition req = comp->size_request();
	    int hh = req.height;
	    Gtk::Allocation allc;
	    MCrpConnectable* crpc = crp->GetObj(crpc);
	    if (crpc == NULL || crpc != NULL && crpc->GetIsInt()) {
		// Internal component
		if (crpc != NULL) {
		    crpc->SetIsInt(true);
		}
		int comp_body_center_x = req.width / 2;
		comps_w_max = max(comps_w_max, req.width);
		allc = Gtk::Allocation(compb_x - comp_body_center_x, compb_y, req.width, hh);
		compb_y += hh + KViewCompGapHight;
	    }
	    else {
		// Boundary components
		crpc->SetIsInt(false);
		bcomps_w_max = max(bcomps_w_max, req.width);
		allc = Gtk::Allocation(bcompb_x - req.width, bcompb_y, req.width, hh);
		bcompb_y += hh + KViewCompGapHight;
	    }
	    comp->size_allocate(allc);
	}
    }

    // Allocating edges
    int edge_wd = 0;
    int redge_wd = 0; // Right edges x-coord
    int edge_mpb = (bcompb_x - bcomps_w_max - (compb_x + comps_w_max/2 + KConnHorizSpreadMin)) / 2; // Left-right edges mid-point base
    for (std::map<Elem*, MCrp*>::iterator it = iCompRps.begin(); it != iCompRps.end(); it++) {
	MCrp* crp = it->second;
	Gtk::Widget* comp = &(crp->Widget());
	MEdgeCrp* medgecrp = crp->GetObj(medgecrp);
	if (medgecrp != NULL) {
	    Gtk::Widget* comp = &(crp->Widget());
	    Gtk::Requisition req = comp->size_request();
	    Elem* p1 = medgecrp->Point1();
	    Elem* p2 = medgecrp->Point2();
	    Elem* op1 = p1 != NULL ? GetCompOwning(p1): NULL;
	    Elem* op2 = p2 != NULL ? GetCompOwning(p2): NULL;

	    int edge_xw = compb_x + comps_w_max/2 + KConnHorizSpreadMin + edge_wd; // X + W
	    Gtk::Requisition p1coord = medgecrp->Cp1Coord();
	    Gtk::Requisition p2coord = medgecrp->Cp2Coord();
	    bool p1int = true, p2int = true;
	    if (p1 != NULL) {
		MCrp* pcrp = iCompRps.at(op1);
		MCrpConnectable* pcrpcbl = pcrp->GetObj(pcrpcbl);
		assert(pcrpcbl != NULL);
		p1coord = pcrpcbl->GetCpCoord(p1);
		medgecrp->SetCp1Coord(p1coord);
		p1int = pcrpcbl->GetIsInt();
	    }
	    else if (!crp->Dragging()) {
		p1coord.width = edge_xw;
	       	p1coord.height =  KViewCompGapHight;
		medgecrp->SetCp1Coord(p1coord);
	    }

	    if (p2 != NULL) {
		MCrp* pcrp = iCompRps.at(op2);
		MCrpConnectable* pcrpcbl = pcrp->GetObj(pcrpcbl);
		assert(pcrpcbl != NULL);
		p2coord = pcrpcbl->GetCpCoord(p2);
		medgecrp->SetCp2Coord(p2coord);
		p2int = pcrpcbl->GetIsInt();
	    }
	    else if (!crp->Dragging()) {
		p2coord.width = edge_xw;
	       	p2coord.height =  p1coord.height + KViewCompGapHight;
		medgecrp->SetCp2Coord(p2coord);
	    }

	    bool puint = p1int, plint = p2int;
	    Elem *pu = p1, *pl = p2;
	    Gtk::Requisition ucoord = p1coord, lcoord = p2coord;
	    if (p2coord.height < p1coord.height) {
		pu = p2; pl = p1; ucoord = p2coord; lcoord = p1coord;
		puint = p2int; plint = p1int;
	    }

	    Gtk::Allocation allc;
	    if (puint && plint) {
		medgecrp->SetType(MEdgeCrp::EtLeft);
		int uextd = ucoord.width - lcoord.width;
		if (uextd > 0) {
		    medgecrp->SetUcpExt(uextd, 0);
		}
		else {
		    medgecrp->SetLcpExt(-uextd, 0);
		}
		int edge_x = min(ucoord.width, lcoord.width);
		int edge_w = compb_x + comps_w_max/2 + KConnHorizSpreadMin - edge_x + edge_wd;
		allc = Gtk::Allocation(edge_x - KEdgeBorderWidth, ucoord.height - KEdgeBorderWidth, 
			edge_w + 2*KEdgeBorderWidth, lcoord.height - ucoord.height + 1 + 2*KEdgeBorderWidth);
		edge_wd += KConnHorizGap;
	    }
	    else if (!puint && !plint) {
		medgecrp->SetType(MEdgeCrp::EtRight);
		int uextd = ucoord.width - lcoord.width;
		if (uextd < 0) {
		    medgecrp->SetUcpExt(uextd, 0);
		}
		else {
		    medgecrp->SetLcpExt(-uextd, 0);
		}
		int edge_x = min(ucoord.width, lcoord.width);
		int edge_w = edge_x - (bcompb_x - bcomps_w_max - KConnHorizSpreadMin) + redge_wd;
		allc = Gtk::Allocation(edge_x - edge_w - KEdgeBorderWidth, ucoord.height - KEdgeBorderWidth, 
			edge_w + 2*KEdgeBorderWidth, lcoord.height - ucoord.height + 1 + 2*KEdgeBorderWidth);
		redge_wd += KConnHorizGap;
	    }
	    else if (puint && !plint) {
		medgecrp->SetType(MEdgeCrp::EtLtRb);
		int edge_x = min(ucoord.width, lcoord.width);
		int edge_w = lcoord.width - ucoord.width;
		int edge_mp = compb_x + comps_w_max/2 + KConnHorizSpreadMin - edge_x + edge_wd;
		medgecrp->SetUcpExt(edge_mp, 0);
		allc = Gtk::Allocation(edge_x - KEdgeBorderWidth, ucoord.height - KEdgeBorderWidth, 
			edge_w + 2*KEdgeBorderWidth, lcoord.height - ucoord.height + 1 + 2*KEdgeBorderWidth);
		edge_wd += KConnHorizGap;
	    }
	    else {
		medgecrp->SetType(MEdgeCrp::EtLbRt);
		int edge_x = min(ucoord.width, lcoord.width);
		int edge_w = ucoord.width - lcoord.width;
		int edge_mp = compb_x + comps_w_max/2 + KConnHorizSpreadMin - edge_x + edge_wd;
		medgecrp->SetUcpExt(edge_mp, 0);
		allc = Gtk::Allocation(edge_x - KEdgeBorderWidth, ucoord.height - KEdgeBorderWidth, 
			edge_w + 2*KEdgeBorderWidth, lcoord.height - ucoord.height + 1 + 2*KEdgeBorderWidth);
		edge_wd += KConnHorizGap;
	    }
	    comp->size_allocate(allc);
	}
    }
}

void SysDrp::on_size_request(Gtk::Requisition* aRequisition)
{
    int comp_w = 0, comp_h = KViewCompGapHight;
    int edge_w = 0, edge_h = 0;
    int bnd_width = 0, bnd_heigth = KViewCompGapHight;

    for (tCrps::iterator it = iCompRps.begin(); it != iCompRps.end(); it++) {
	MCrp* crp = it->second;
	MEdgeCrp* medgecrp = crp->GetObj(medgecrp);
	if (medgecrp == NULL) {
	    Gtk::Widget* comp = &(crp->Widget());
	    Gtk::Requisition req = comp->size_request();
	    MCrpConnectable* crpc = crp->GetObj(crpc);
	    if (crpc != NULL && !crpc->GetIsInt()) {
		// Boundary
		bnd_width = max(bnd_width, req.width);
		bnd_heigth += req.height + KViewCompGapHight;
	    }
	    else {
		// Internal
		comp_w = max(comp_w, req.width);
		comp_h += req.height + KViewCompGapHight;
	    }
	}
	else {
	    Gtk::Widget* cpw = &(crp->Widget());
	    Gtk::Requisition req = cpw->size_request();
	    if (edge_w == 0) {
		edge_w = KConnHorizSpreadMin;
	    }
	    else {
		edge_w += KEdgeGridCell;
	    }
	    edge_h = max(edge_h, req.height);
	}
    }

    // Doubling of bnd_width is because internal comps to be centered
    aRequisition->width = comp_w + 2 * (edge_w + KDrpPadding + bnd_width);
    aRequisition->height = max(comp_h, bnd_heigth);
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

#endif


// System DRP with boundary direction support


const string SysDrp::KCpEType = "Elem:Vert:ConnPointBase:ConnPoint";
const string SysDrp::KExtdEType = "Elem:Vert:Extender";
const int SysDrp::KETnlCap = 5;

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

void SysDrp::Construct()
{
    VertDrpw_v1::Construct();
    // Pre-layout the components
    iLaPars.resize(3);
    for (TLAreasPars::iterator zit = iLaPars.begin(); zit != iLaPars.end(); zit++) {
	zit->first = Allocation();
	zit->second.clear(); 
    }
    for (std::vector<Elem*>::iterator it = iElem->Comps().begin(); it != iElem->Comps().end(); it++) {
	Elem* ecmp = *it;
	assert(iCompRps.count(ecmp) > 0);
	MCrp* crp = iCompRps.at(ecmp);
	MEdgeCrp* ecrp = crp->GetObj(ecrp);
	if (ecrp == NULL) {
	    MCompatChecker* cc = ecmp->GetObj(cc);
	    int narea = 1;
	    if (cc != NULL) {
		MCompatChecker::TDir dir =  cc->GetDir();
		narea = dir == MCompatChecker::EInp ? 0 : 2;
	    }
	    TLAreaPar& area = iLaPars.at(narea);
	    area.second.push_back(crp);
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
	int compb_x = cur_x + zal.get_width()/2, compb_y = KViewCompGapHight;
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
	cur_x += zal.get_width() + KETnlCap*KEdgeGridCell;
    }

    // Allocate edges
    int edge_park_x = iLaPars.begin()->first.get_width() + KEdgeGridCell;
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
	       	p1coord.height =  KViewCompGapHight;
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
		p2coord.width = edge_park_x;
	       	p2coord.height =  p1coord.height + KViewCompGapHight;
		medgecrp->SetCp2Coord(p2coord);
	    }
	    // Trace the edge, calculating the edges nodes, from left to right
	    // Tracing step is from entry of the current vert tunnel till the entry to next one (Z form)
	    if (true) {
	    //if (p1 != NULL && p2 != NULL) {
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
			// Get next vertical tunnel entry
			fx = GetEvtEnty(evi_c.first + 1);
		    }
		    // Find available line within the vertical tunnel
		    int lx = GetEvtLineX(crp, evi_c.first, cur.height, fy);
		    Requisition n1 = {lx, cur.height};
		    Requisition n2 = {lx, fy};
		    Requisition n3 = {fx, fy};
		    ven.push_back(n1);
		    ven.push_back(n2);
		    ven.push_back(n3);
		    cur.width = fx; cur.height = fy;
		}
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

int SysDrp::GetEvtWidth() const
{
    return KETnlCap*KEdgeGridCell;
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
    int tw = GetEvtWidth();
    int x = aCoord.width;
    int cx = 0;
    for (int cnt = 0; cnt < iLaPars.size(); cnt++) {
	Allocation& zal = iLaPars.at(cnt).first;
	int zw = zal.get_width();
	if (x < cx + zw/2) {
	    res.first = cnt;
	    res.second = x < (cx - (zw + tw)/2);
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
    int tw = GetEvtWidth();
    for (int cnt = 0; cnt < aTnlCnt; cnt++) {
	const Allocation& zal = iLaPars.at(cnt).first;
	int zw = zal.get_width();
	res += zw + (cnt == 0 ? 0: tw);
    }
    return res;
}

bool SysDrp::AreIntervalsIntersecting(int aA1, int aA2, int aB1, int aB2)
{
    int al = min(aA1, aA2);
    int am = max(aA1, aA2);
    int bl = min(aB1, aB2);
    int bm = max(aB1, aB2);
    return !(bm < al || bl > am);
}

int SysDrp::GetEvtLineX(MCrp* aEdge, int aTunnel, int aP1, int aP2) 
{
    int lx = GetEvtX(aTunnel) + KEdgeGridCell;
    bool isec = false;
    int ptop = min(aP1, aP2);
    int pbottom = max(aP1, aP2);
    for (int lid = 0; lid < KETnlCap; lid++) {
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
	lx += KEdgeGridCell;
    }
    if (isec) {
	lx = GetEvtX(aTunnel) + KEdgeGridCell + 1;
    }
    return lx;
}

int SysDrp::GetEhtLine(MCrp* aEdge, int aEvt, int aY, bool aUp) const
{
    int res = -1; 
    // Find base first
    int base = aUp ? 0: std::numeric_limits<int>::max();
    const TVectCrps& crps = iLaPars.at(aEvt).second;
    for (TVectCrps::const_iterator it = crps.begin(); it != crps.end(); it++) {
	MCrp* crp = *it;
	Allocation alc = crp->Widget().get_allocation();
	int y;
	if (aUp) {
	    y = alc.get_y();
	    if (y < aY) {
		base = max(base, y);
	    }
	}
	else {
	    y = alc.get_y() + alc.get_height();
	    if (y >= aY) {
		base = min(base, y);
	    }
	}
    }
    // Find available line
    int ly = base + (aUp ? (-KEdgeGridCell) : KEdgeGridCell);
    bool isec = false;
    int tnlcap = KViewCompGapHight / KEdgeGridCell;
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
	ly = base + (aUp ? (-KEdgeGridCell-1) : KEdgeGridCell+1);
    }
    res = ly;
    return res;
}

void SysDrp::on_size_request(Gtk::Requisition* aRequisition)
{
    aRequisition->width = iLaPars.size() * KETnlCap*KEdgeGridCell;
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



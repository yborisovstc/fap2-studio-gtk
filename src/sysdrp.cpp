
#include <syst.h>

#include <iostream>
#include "common.h"
#include "sysdrp.h"

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
    std::cout << "SysDrp::AreCpsCompatible, aCp1: " << aCp1->Name() << ", aCp2: " << aCp2->Name() << ", res: " << res << std::endl;
    return res;
}


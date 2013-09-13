
#include "common.h"
#include "sysdrp.h"

const string KCpEType = "Elem:Vert:ConnPointBase:ConnPoint";

string SysDrp::EType()
{
    return ":Elem:Vert:Syst";
}

SysDrp::SysDrp(Elem* aElem, const MCrpProvider& aCrpProv): VertDrpw_v1(aElem, aCrpProv)
{
    // Mark all CPs as boundary
    for (std::vector<Elem*>::iterator it = iElem->Comps().begin(); it != iElem->Comps().end(); it++) {
	Elem* ecmp = *it;
	if (ecmp->IsHeirOf(KCpEType)) {
	    MCrp* crp = iCompRps.at(ecmp);
	    MCrpConnectable* crpc = crp->GetObj(crpc);
	    if (crpc != NULL) {
		crpc->SetIsInt(false);
	    }
	}
    }
}

SysDrp::~SysDrp()
{
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

    // Allocate edges
    int edge_wd = 0;
    int redge_wd = 0; // Right edges x-coord
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
	    Elem *pu = p1, *pl = p2, *opu = op1, *opl = op2;
	    bool puint, plint;
	    Gtk::Requisition ucoord, lcoord;
	    if (p1 != NULL && p2 != NULL) {
		MCrp* crp1 = iCompRps.at(op1);
		MCrpConnectable* crp1c = crp1->GetObj(crp1c);
		Gtk::Requisition coord1 = crp1c->GetCpCoord(p1);
		bool p1int = crp1c->GetIsInt();
		MCrp* crp2 = iCompRps.at(op2);
		MCrpConnectable* crp2c = crp2->GetObj(crp2c);
		Gtk::Requisition coord2 = crp2c->GetCpCoord(p2);
		bool p2int = crp2c->GetIsInt();
		puint = p1int; plint = p2int;
		ucoord = coord1; lcoord = coord2;
		if (coord2.height < coord1.height) {
		    pu = p2; pl = p1; opu = op2; opl = op1;
		    puint = p2int; plint = p1int;
		    ucoord = coord2; lcoord = coord1;
		}
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
		allc = Gtk::Allocation(edge_x, ucoord.height, edge_w, lcoord.height - ucoord.height + 1);
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
		allc = Gtk::Allocation(edge_x, ucoord.height, edge_w, lcoord.height - ucoord.height + 1);
		redge_wd += KConnHorizGap;
	    }
	    else if (puint && !plint) {
		medgecrp->SetType(MEdgeCrp::EtLtRb);
		int edge_x = min(ucoord.width, lcoord.width);
		int edge_w = lcoord.width - ucoord.width;
		medgecrp->SetUcpExt(edge_wd, 0);
		allc = Gtk::Allocation(edge_x, ucoord.height, edge_w, lcoord.height - ucoord.height + 1);
		edge_wd += KConnHorizGap;
	    }
	    else {
		medgecrp->SetType(MEdgeCrp::EtLbRt);
		int edge_x = min(ucoord.width, lcoord.width);
		int edge_w = ucoord.width - lcoord.width;
		int edge_mp = compb_x + comps_w_max/2 + KConnHorizSpreadMin - edge_x + edge_wd;
		medgecrp->SetUcpExt(edge_mp, 0);
		allc = Gtk::Allocation(edge_x, ucoord.height, edge_w, lcoord.height - ucoord.height + 1);
		edge_wd += KConnHorizGap;
	    }
	    comp->size_allocate(allc);
	}
    }

    // Allocate edges
    /*
    int edge_wd = 0;
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
	    Elem *pu = p1, *pl = p2, *opu = op1, *opl = op2;
	    if (p1 != NULL && p2 != NULL) {
		ConnInfo& ci1 = iConnInfos.at(op1);
		ConnInfo& ci2 = iConnInfos.at(op2);
		if (ci1.iCompOrder > ci2.iCompOrder) {
		    pu = p2; pl = p1; opu = op2; opl = op1;
		}
	    }
	    Gtk::Requisition ucpcoord, lcpcoord;
	    if (pu != NULL) {
		MCrp* pcrp = iCompRps.at(opu);
		MCrpConnectable* pcrpcbl = pcrp->GetObj(pcrpcbl);
		assert(pcrpcbl != NULL);
		ucpcoord = pcrpcbl->GetCpCoord(pu);
	    }
	    if (pl != NULL) {
		MCrp* pcrp = iCompRps.at(opl);
		MCrpConnectable* pcrpcbl = pcrp->GetObj(pcrpcbl);
		assert(pcrpcbl != NULL);
		lcpcoord = pcrpcbl->GetCpCoord(pl);
	    }
	    int uextd = ucpcoord.width > lcpcoord.width;
	    if (uextd > 0) {
		medgecrp->SetUcpExt(uextd, 0);
	    }
	    else {
		medgecrp->SetLcpExt(-uextd, 0);
	    }
	    int edge_x = min(ucpcoord.width, lcpcoord.width);
	    int edge_w = compb_x + comps_w_max/2 + KConnHorizSpreadMin - edge_x + edge_wd;
	    Gtk::Allocation allc(edge_x, ucpcoord.height, edge_w, lcpcoord.height - ucpcoord.height + 1);
	    comp->size_allocate(allc);
	    edge_wd += KConnHorizGap;
	}
    }
    */
}

void SysDrp::on_size_request(Gtk::Requisition* aRequisition)
{
    VertDrpw_v1::on_size_request(aRequisition);
}


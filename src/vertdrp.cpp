
#include <iostream>
#include <edge.h>
#include "common.h"
#include "vertdrp.h"

VertDrpw::ConnInfo::ConnInfo(): iCompOrder(0), iConnsToTop(0), iConnsToBottom(0)
{
}

VertDrpw::ConnInfo::ConnInfo(int aOrder, int aToTop, int aToBottom): 
    iCompOrder(aOrder), iConnsToTop(aToTop), iConnsToBottom(aToBottom)
{
}

VertDrpw::ConnInfo::ConnInfo(const ConnInfo& aCInfo): 
    iCompOrder(aCInfo.iCompOrder), iConnsToTop(aCInfo.iConnsToTop), iConnsToBottom(aCInfo.iConnsToBottom)
{
}

VertDrpw::VertDrpw(Elem* aElem, const MCrpProvider& aCrpProv): Gtk::Layout(), iElem(aElem), iCrpProv(aCrpProv)
{
    // Add components
    int compord = 0;
    for (std::vector<Elem*>::iterator it = iElem->Comps().begin(); it != iElem->Comps().end(); it++) {
	Elem* comp = *it;
	assert(comp != NULL);
	MCrp* rp = iCrpProv.CreateRp(*comp);
	Gtk::Widget& rpw = rp->Widget();
	rpw.signal_button_press_event().connect(sigc::bind<Elem*>(sigc::mem_fun(*this, &VertDrpw::on_comp_button_press_ext), comp));
	add(rpw);
	iCompRps[comp] = rp;
	rpw.show();
	MEdgeCrp* medgecrp = rp->GetObj(medgecrp);
	if (medgecrp == NULL) {
	    ConnInfo cinfo(compord++);
	    iConnInfos.insert(pair<Elem*, ConnInfo>(comp, cinfo));
	}
    }
}

VertDrpw::~VertDrpw()
{
}

Elem* VertDrpw::GetCompOwning(Elem* aElem)
{
    Elem* res = NULL;
    for (std::vector<Elem*>::iterator it = iElem->Comps().begin(); it != iElem->Comps().end() && res == NULL; it++) {
	Elem* comp = *it;
	if (aElem == comp || comp->IsComp(aElem)) {
	    res = comp;
	}
    }
    return res;
}

void VertDrpw::on_size_allocate(Gtk::Allocation& aAllc)
{
    //    Gtk::Layout::on_size_allocate(aAllc);
    //    Set allocation to itself and resize the windows. Refer gtk_layout_size_allocate 
    //    Gtk::Layout::on_size_allocate can be invoked instead but it does childs size allocation internally
    //    so it causes ovwrhead because we allocate child sizes here. 
    //    Probably better would be to set layout children requisition first and then run Gtk::Layout::on_size_allocate
    //    which does childs size allocation then. But this approach doesn't work for me - it causes continous resize

    set_allocation(aAllc);
    if (get_realized()) {
	get_window()->move_resize(aAllc.get_x(), aAllc.get_y(), aAllc.get_width(), aAllc.get_height());
	get_bin_window()->resize(aAllc.get_width(), aAllc.get_height());
    }

    // Configure edges order, calculate number of connections for each component
    for (std::vector<Elem*>::iterator it = iElem->Comps().begin(); it != iElem->Comps().end(); it++) {
	MCrp* crp = iCompRps.at(*it);
	MEdgeCrp* medgecrp = crp->GetObj(medgecrp);
	if (medgecrp == NULL) {
	    ConnInfo& ci = iConnInfos.at(*it);
	    ci.iConnsToBottom = 0;
	    ci.iConnsToTop = 0;
	}
    }
    for (std::map<Elem*, MCrp*>::iterator it = iCompRps.begin(); it != iCompRps.end(); it++) {
	MCrp* crp = it->second;
	Gtk::Widget* comp = &(crp->Widget());
	MEdgeCrp* medgecrp = crp->GetObj(medgecrp);
	if (medgecrp != NULL) {
	    Gtk::Widget* comp = &(crp->Widget());
	    Gtk::Requisition req = comp->size_request();
	    Elem* rp1 = medgecrp->Point1();
	    Elem* rp2 = medgecrp->Point2();
	    Elem* p1 = rp1 != NULL ? GetCompOwning(rp1): NULL;
	    Elem* p2 = rp2 != NULL ? GetCompOwning(rp2): NULL;
	    Elem *pu = p1, *pl = p2;
	    if (p1 != NULL && p2 != NULL) {
		ConnInfo& ci1 = iConnInfos.at(p1);
		ConnInfo& ci2 = iConnInfos.at(p2);
		if (ci1.iCompOrder > ci2.iCompOrder) {
		    pu = p2; pl = p1;
		}
	    }
	    if (pu != NULL) {
		ConnInfo& ciu = iConnInfos.at(pu);
		ciu.iConnsToBottom++;
		medgecrp->SetUcp(ciu.iConnsToBottom);
	    }
	    if (pl != NULL) {
		ConnInfo& cil = iConnInfos.at(pl);
		cil.iConnsToTop++;
		medgecrp->SetLcp(cil.iConnsToTop - 1);
	    }
	}
    }

    // Allocate components excluding edges
    // Keeping components order, so using elems register of comps
    int compb_x = aAllc.get_width()/2, compb_y = KViewCompGapHight;
    int comps_w_max = 0;
    for (std::vector<Elem*>::iterator it = iElem->Comps().begin(); it != iElem->Comps().end(); it++) {
	MCrp* crp = iCompRps.at(*it);
	MEdgeCrp* medgecrp = crp->GetObj(medgecrp);
	if (medgecrp == NULL) {
	    ConnInfo& ci = iConnInfos.at(*it);
	    Gtk::Widget* comp = &(crp->Widget());
	    Gtk::Requisition req = comp->size_request();
	    int comp_body_center_x = req.width / 2;
	    comps_w_max = max(comps_w_max, req.width);
	    int hh = req.height + (ci.iConnsToTop + ci.iConnsToBottom - 1) * KConnVertGap;
	    Gtk::Allocation allc = Gtk::Allocation(compb_x - comp_body_center_x, compb_y, req.width, hh);
	    comp->size_allocate(allc);
	    compb_y += hh + KViewCompGapHight;
	    ci.iConnsToBottom = 0;
	    ci.iConnsToTop = 0;
	}
    }
    // Allocate edges
    int edge_wd = 0;
    for (std::map<Elem*, MCrp*>::iterator it = iCompRps.begin(); it != iCompRps.end(); it++) {
	MCrp* crp = it->second;
	Gtk::Widget* comp = &(crp->Widget());
	MEdgeCrp* medgecrp = crp->GetObj(medgecrp);
	if (medgecrp != NULL) {
	    Gtk::Widget* comp = &(crp->Widget());
	    Gtk::Requisition req = comp->size_request();
	    Elem* rp1 = medgecrp->Point1();
	    Elem* rp2 = medgecrp->Point2();
	    Elem* p1 = rp1 != NULL ? GetCompOwning(rp1): NULL;
	    Elem* p2 = rp2 != NULL ? GetCompOwning(rp2): NULL;
	    Elem *pu = p1, *pl = p2;
	    if (p1 != NULL && p2 != NULL) {
		ConnInfo& ci1 = iConnInfos.at(p1);
		ConnInfo& ci2 = iConnInfos.at(p2);
		if (ci1.iCompOrder > ci2.iCompOrder) {
		    pu = p2; pl = p1;
		}
	    }
	    Gtk::Requisition ucpcoord, lcpcoord;
	    if (pu != NULL) {
		ConnInfo& ciu = iConnInfos.at(pu);
		MCrp* pcrp = iCompRps.at(pu);
		MCrpConnectable* pcrpcbl = pcrp->GetObj(pcrpcbl);
		assert(pcrpcbl != NULL);
		ucpcoord = pcrpcbl->GetCpCoord(MCrpConnectable::CpGeneric);
		ucpcoord.height += ciu.iConnsToTop * KConnVertGap;
	    }
	    if (pl != NULL) {
		ConnInfo& cil = iConnInfos.at(pl);
		MCrp* pcrp = iCompRps.at(pl);
		MCrpConnectable* pcrpcbl = pcrp->GetObj(pcrpcbl);
		assert(pcrpcbl != NULL);
		lcpcoord = pcrpcbl->GetCpCoord(MCrpConnectable::CpGeneric);
		lcpcoord.height += cil.iConnsToTop * KConnVertGap;
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
	    Gtk::Allocation allc(edge_x, ucpcoord.height, edge_w, lcpcoord.height - ucpcoord.height);
	    comp->size_allocate(allc);
	    edge_wd += KConnHorizGap;
	}
    }

}

void VertDrpw::on_size_request(Gtk::Requisition* aReq)
{
    // Calculate size of regular comps and edges
    int comp_w = 0, comp_h = 2*KViewCompGapHight;
    int edge_w = 0, edge_h = 0;
    for (std::map<Elem*, MCrp*>::iterator it = iCompRps.begin(); it != iCompRps.end(); it++) {
	MCrp* crp = it->second;
	MEdgeCrp* medgecrp = crp->GetObj(medgecrp);
	if (medgecrp == NULL) {
	    Gtk::Widget* cpw = &(crp->Widget());
	    Gtk::Requisition req = cpw->size_request();
	    edge_w = max(edge_w, req.width);
	    comp_h += req.height + KViewCompGapHight;
	}
	else {
	    Gtk::Widget* cpw = &(crp->Widget());
	    Gtk::Requisition req = cpw->size_request();
	    if (edge_w == 0) {
		edge_w = KConnHorizSpreadMin;
	    }
	    else {
		edge_w += KConnHorizGap;
	    }
	    edge_h = max(edge_h, req.height);
	}
    }

    aReq->width = comp_w + edge_w; 
    aReq->height = max(comp_h, edge_h);
}

bool VertDrpw::on_comp_button_press(GdkEventButton* event)
{
    std::cout << "on_comp_button_press" << std::endl;
}

bool VertDrpw::on_comp_button_press_ext(GdkEventButton* event, Elem* aComp)
{
    std::cout << "on_comp_button_press, comp [" << aComp->Name() << "]" << std::endl;
    iSigCompSelected.emit(aComp);
}


const string sVertDrpType = "VertDrp";

const string& VertDrp::Type()
{
    return sVertDrpType;
}


string VertDrp::EType()
{
    return ":Elem:Vert";
}

VertDrp::VertDrp(Elem* aElem, const MCrpProvider& aCrpProv)
{
    iRp = new VertDrpw(aElem, aCrpProv);
}

VertDrp::~VertDrp()
{
    delete iRp;
}

void *VertDrp::DoGetObj(const string& aName)
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

Gtk::Widget& VertDrp::Widget()
{
    return *iRp;
}

MDrp::tSigCompSelected VertDrp::SignalCompSelected()
{
    return iRp->iSigCompSelected;
}

Elem* VertDrp::Model()
{
    return iRp->iElem;
}



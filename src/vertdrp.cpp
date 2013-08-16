
#include <iostream>
#include <edge.h>
#include "common.h"
#include "vertdrp.h"

VertDrpw::VertDrpw(Elem* aElem, const MCrpProvider& aCrpProv): Gtk::Layout(), iElem(aElem), iCrpProv(aCrpProv)
{
    // Add components
    for (std::vector<Elem*>::iterator it = iElem->Comps().begin(); it != iElem->Comps().end(); it++) {
	Elem* comp = *it;
	assert(comp != NULL);
	MCrp* rp = iCrpProv.CreateRp(*comp);
	Gtk::Widget& rpw = rp->Widget();
	rpw.signal_button_press_event().connect(sigc::bind<Elem*>(sigc::mem_fun(*this, &VertDrpw::on_comp_button_press_ext), comp));
	add(rpw);
	iCompRps[comp] = rp;
	rpw.show();
    }
}

VertDrpw::~VertDrpw()
{
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

    // Allocate components excluding edges
    // Keeping components order, so using elems reginster of comps
    int compb_x = aAllc.get_width()/2, compb_y = KViewCompGapHight;
    int comps_w_max = 0;
    for (std::vector<Elem*>::iterator it = iElem->Comps().begin(); it != iElem->Comps().end(); it++) {
	MCrp* crp = iCompRps.at(*it);
	MEdgeCrp* medgecrp = crp->GetObj(medgecrp);
	if (medgecrp == NULL) {
	    Gtk::Widget* comp = &(crp->Widget());
	    Gtk::Requisition req = comp->size_request();
	    int comp_body_center_x = req.width / 2;
	    comps_w_max = max(comps_w_max, req.width);
	    Gtk::Allocation allc = Gtk::Allocation(compb_x - comp_body_center_x, compb_y, req.width, req.height);
	    comp->size_allocate(allc);
	    compb_y += req.height + KViewCompGapHight;
	}
    }
    // Allocate edges
    for (std::map<Elem*, MCrp*>::iterator it = iCompRps.begin(); it != iCompRps.end(); it++) {
	MCrp* crp = it->second;
	Gtk::Widget* comp = &(crp->Widget());
	MEdgeCrp* medgecrp = crp->GetObj(medgecrp);
	if (medgecrp != NULL) {
	    Gtk::Widget* comp = &(crp->Widget());
	    Gtk::Requisition req = comp->size_request();
	    Elem* p1 = medgecrp->Point1();
	    Elem* p2 = medgecrp->Point2();
	    Gtk::Requisition ucpcoord, lcpcoord;
	    if (p1 != NULL) {
		MCrp* pcrp = iCompRps.at(p1);
		MCrpConnectable* pcrpcbl = pcrp->GetObj(pcrpcbl);
		assert(pcrpcbl != NULL);
		ucpcoord = pcrpcbl->GetCpCoord(MCrpConnectable::CpGeneric);
		medgecrp->SetUcp(ucpcoord, 0);
	    }
	    if (p2 != NULL) {
		MCrp* pcrp = iCompRps.at(p2);
		MCrpConnectable* pcrpcbl = pcrp->GetObj(pcrpcbl);
		assert(pcrpcbl != NULL);
		lcpcoord = pcrpcbl->GetCpCoord(MCrpConnectable::CpGeneric);
		medgecrp->SetLcp(lcpcoord, 0);
	    }
	    int edge_w = compb_x + comps_w_max/2 + KConnHorizSpreadMin - min(ucpcoord.width, lcpcoord.width);
	    Gtk::Allocation allc(ucpcoord.width, ucpcoord.height, edge_w, lcpcoord.height - ucpcoord.height);
	    comp->size_allocate(allc);
	    edge_w += KConnHorizGap;
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



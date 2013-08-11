
#include <iostream>
#include <edge.h>
#include "common.h"
#include "vertdrp.h"

// Edge compact representation
EdgeCrp::EdgeCrp(Elem* aElem): iElem(aElem)
{
}

EdgeCrp::~EdgeCrp()
{
}

void EdgeCrp::SetUcp(Gtk::Requisition* aReq, int aPos)
{
    iUcp.iCoord = *aReq;
    iUcp.iPos = aPos;
}


VertDrpw::VertDrpw(Elem* aElem, const MCrpProvider& aCrpProv): Gtk::Layout(), iElem(aElem), iCrpProv(aCrpProv)
{
    // Add components
    for (std::vector<Elem*>::iterator it = iElem->Comps().begin(); it != iElem->Comps().end(); it++) {
	Elem* comp = *it;
	assert(comp != NULL);
	if (comp->IsHeirOf(Edge::PEType())) {
	    // Edges
	    EdgeCrp* ecrp = new EdgeCrp(comp);
	}
	else {
	    // Vertexes etc.
	    MCrp* rp = iCrpProv.CreateRp(*comp);
	    Gtk::Widget& rpw = rp->Widget();
	    rpw.signal_button_press_event().connect(sigc::bind<Elem*>(sigc::mem_fun(*this, &VertDrpw::on_comp_button_press_ext), comp));
	    add(rpw);
	    iCompRps[comp] = rp;
	    rpw.show();
	}
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

    // Allocate components
    int compb_x = aAllc.get_width()/2, compb_y = KViewCompGapHight;
    for (std::map<Elem*, MCrp*>::iterator it = iCompRps.begin(); it != iCompRps.end(); it++) {
	MCrp* crp = it->second;
	Gtk::Widget* comp = &(crp->Widget());
	Gtk::Requisition req = comp->size_request();
	int comp_body_center_x = req.width / 2;
	Gtk::Allocation allc = Gtk::Allocation(compb_x - comp_body_center_x, compb_y, req.width, req.height);
	comp->size_allocate(allc);
	compb_y += req.height + KViewCompGapHight;
    }
}

void VertDrpw::on_size_request(Gtk::Requisition* aReq)
{
    // Calculate size of comps
    int comp_w = 0, comp_h = KViewCompGapHight;
    for (std::map<Elem*, MCrp*>::iterator it = iCompRps.begin(); it != iCompRps.end(); it++) {
	Gtk::Widget* cpw = &(it->second->Widget());
	Gtk::Requisition req = cpw->size_request();
	comp_w = max(comp_w, req.width);
	comp_h += req.height + KViewCompGapHight;
    }

    aReq->width = comp_w; 
    aReq->height = comp_h + KViewCompGapHight;
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



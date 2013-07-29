#include "common.h"
#include "elemdetrp.h"

ElemDetRp::ElemDetRp(Elem* aElem): iElem(aElem)
{
    // Add components
    for (std::vector<Elem*>::iterator it = iElem->Comps().begin(); it != iElem->Comps().end(); it++) {
	Elem* comp = *it;
	assert(comp != NULL);
	ElemCompRp* rp = new ElemCompRp(comp);
	add(*rp);
	iCompRps[comp] = rp;
	rp->show();
    }
}

ElemDetRp::~ElemDetRp()
{
}

void ElemDetRp::on_size_allocate(Gtk::Allocation* aAllc)
{
    Gtk::Allocation alc = get_allocation();
    // Allocate components
    int compb_x = aAllc->get_width()/2, compb_y = KViewCompGapHight;
    for (std::map<Elem*, ElemCompRp*>::iterator it = iCompRps.begin(); it != iCompRps.end(); it++) {
	ElemCompRp* comp = it->second;
	Gtk::Requisition req = comp->size_request();
	int comp_body_center_x = req.width / 2;
	Gtk::Allocation allc = Gtk::Allocation(compb_x - comp_body_center_x, compb_y, req.width, req.height);
	comp->size_allocate(allc);
	compb_y += req.height + KViewCompGapHight;
    }
}

void ElemDetRp::on_size_request(Gtk::Requisition* aReq)
{
    // Calculate size of comps
    int comp_w = 0, comp_h = 0;
    for (std::map<Elem*, ElemCompRp*>::iterator it = iCompRps.begin(); it != iCompRps.end(); it++) {
	ElemCompRp* cpw = it->second;
	Gtk::Requisition req = cpw->size_request();
	comp_w = max(comp_w, req.width);
	comp_h += req.height + KViewCompGapHight;
    }

    aReq->width = comp_w; 
    aReq->height = comp_h + 2*KViewExtCompGapWidth;
}





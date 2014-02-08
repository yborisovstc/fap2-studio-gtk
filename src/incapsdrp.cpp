
#include <incaps.h>

#include "common.h"
#include "incapsdrp.h"


const string IncapsDrp::KCapsUri = "Capsule";
const string IncapsDrp::KIncapsType = Syst::PEType() + ":Incaps";
const string IncapsDrp::KExtdType = "Elem:Vert:Extender";

string IncapsDrp::EType()
{
    return ":Elem:Vert:Syst:Incaps";
}

IncapsDrp::IncapsDrp(Elem* aElem, const MCrpProvider& aCrpProv): SysDrp(aElem, aCrpProv)
{
}

IncapsDrp::~IncapsDrp()
{
}

void IncapsDrp::Construct()
{
    SysDrp::Construct();
    // Capsule
    Elem* caps = iElem->GetNode(KCapsUri);
    assert(caps != NULL);
    for (std::vector<Elem*>::iterator it = caps->Comps().begin(); it != caps->Comps().end(); it++) {
	Elem* comp = *it;
	assert(comp != NULL);
	MCrp* rp = iCrpProv.CreateRp(*comp, this);
	Gtk::Widget& rpw = rp->Widget();
	rp->SignalButtonPress().connect(sigc::bind<Elem*>(sigc::mem_fun(*this, &ElemDetRp::on_comp_button_press), comp));
	rp->SignalButtonPressName().connect(sigc::bind<Elem*>(sigc::mem_fun(*this, &ElemDetRp::on_comp_button_press_name), comp));
	add(rpw);
	iCompRps[comp] = rp;
	rpw.show();
	MCrpConnectable* crpc = rp->GetObj(crpc);
	if (crpc != NULL) {
	    crpc->SetIsInt(false);
	}
    }
}

void *IncapsDrp::DoGetObj(const string& aName)
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

TBool IncapsDrp::IsTypeOf(const string& aType, const string& aParent) const
{
    int pos = aType.find(aParent);
    return pos != string::npos;
}

bool IncapsDrp::IsTypeAllowed(const std::string& aType) const
{
    bool res = false;
    if (SysDrp::IsTypeAllowed(aType) || IsTypeOf(aType, KIncapsType) || 
	   IsTypeOf(aType, KExtdType))  {
	res = true;
    }
    return res;
}

void IncapsDrp::on_size_allocate(Gtk::Allocation& aAllc)
{
    set_allocation(aAllc);
    if (get_realized()) {
	get_window()->move_resize(aAllc.get_x(), aAllc.get_y(), aAllc.get_width(), aAllc.get_height());
	get_bin_window()->resize(aAllc.get_width(), aAllc.get_height());
    }

    // Allocate components excluding edges and capsula
    // Keeping components order, so using elems register of comps
    int compb_x = aAllc.get_width()/2, compb_y = KViewCompGapHight;
    int bcompb_x = aAllc.get_width() - KBoundCompGapWidth , bcompb_y = KViewCompGapHight;
    int comps_w_max = 0;
    int bcomps_w_max = 0;
    Elem* caps = iElem->GetNode(KCapsUri);
    assert(caps != NULL);
    for (std::vector<Elem*>::iterator it = iElem->Comps().begin(); it != iElem->Comps().end(); it++) {
	Elem* ecmp = *it;
	MCrp* crp = iCompRps.at(ecmp);
	MEdgeCrp* medgecrp = crp->GetObj(medgecrp);
	bool is_edge = medgecrp != NULL;
	bool is_caps = ecmp == caps;
	if (!is_edge && !is_caps) {
	    Gtk::Widget* comp = &(crp->Widget());
	    Gtk::Requisition req = comp->size_request();
	    int hh = req.height;
	    Gtk::Allocation allc;
	    MCrpConnectable* crpc = crp->GetObj(crpc);
	    // Internal component
	    if (crpc != NULL) {
		crpc->SetIsInt(true);
	    }
	    int comp_body_center_x = req.width / 2;
	    comps_w_max = max(comps_w_max, req.width);
	    allc = Gtk::Allocation(compb_x - comp_body_center_x, compb_y, req.width, hh);
	    compb_y += hh + KViewCompGapHight;
	    comp->size_allocate(allc);
	}
    }

    // Capsule
    for (std::vector<Elem*>::iterator it = caps->Comps().begin(); it != caps->Comps().end(); it++) {
	Elem* ecmp = *it;
	MCrp* crp = iCompRps.at(ecmp);
	Gtk::Widget* comp = &(crp->Widget());
	Gtk::Requisition req = comp->size_request();
	int hh = req.height;
	Gtk::Allocation allc;
	MCrpConnectable* crpc = crp->GetObj(crpc);
	if (crpc != NULL) {
	    crpc->SetIsInt(false);
	}
	bcomps_w_max = max(bcomps_w_max, req.width);
	allc = Gtk::Allocation(bcompb_x - req.width, bcompb_y, req.width, hh);
	bcompb_y += hh + KViewCompGapHight;
	comp->size_allocate(allc);
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
	    assert(allc.get_width() > 0 && allc.get_height() > 0);
	    comp->size_allocate(allc);
	}
    }
}

void IncapsDrp::on_size_request(Gtk::Requisition* aRequisition)
{
    int comp_w = 0, comp_h = KViewCompGapHight;
    int edge_w = 0, edge_h = 0;
    int bnd_w = 0, bnd_h = KViewCompGapHight;
    Elem* caps = Model()->GetNode(KCapsUri);
    assert(caps != NULL);

    for (std::vector<Elem*>::iterator it = iElem->Comps().begin(); it != iElem->Comps().end(); it++) {
	Elem* ecmp = *it;
	MCrp* crp = iCompRps.at(ecmp);
	MEdgeCrp* medgecrp = crp->GetObj(medgecrp);
	if (medgecrp == NULL) {
	    // Comps excluding caplule
	    if (crp->Model() != caps) {
		Gtk::Widget* comp = &(crp->Widget());
		Gtk::Requisition req = comp->size_request();
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
    // Boundary - capsule comps
    for (std::vector<Elem*>::iterator it = caps->Comps().begin(); it != caps->Comps().end(); it++) {
	Elem* ecmp = *it;
	MCrp* crp = iCompRps.at(ecmp);
	Gtk::Widget* comp = &(crp->Widget());
	Gtk::Requisition req = comp->size_request();
	bnd_w = max(bnd_w, req.width);
	bnd_h += req.height + KViewCompGapHight;
    }
    // Doubling of bnd_width is because internal comps to be centered
    aRequisition->width = comp_w + 2 * (edge_w + KDrpPadding + bnd_w);
    aRequisition->height = max(comp_h, bnd_h);
}

Elem* IncapsDrp::GetCompOwning(Elem* aElem)
{
    Elem* res = NULL;
    Elem* caps = iElem->GetNode("Capsule");
    for (std::vector<Elem*>::iterator it = iElem->Comps().begin(); it != iElem->Comps().end() && res == NULL; it++) {
	Elem* comp = *it;
	if (aElem == comp || comp != caps && comp->IsComp(aElem)) {
	    res = comp;
	}
    }
    if (res == NULL) {
	for (std::vector<Elem*>::iterator it = caps->Comps().begin(); it != caps->Comps().end() && res == NULL; it++) {
	    Elem* comp = *it;
	    if (aElem == comp || comp->IsComp(aElem)) {
		res = comp;
	    }
	}
    }
    return res;
}

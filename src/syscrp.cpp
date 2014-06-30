#include <mprop.h>
#include <complex>
#include <syst.h>
#include "syscrp.h"
#include "common.h"
#include <iostream>

//  Connection point representation
CpRp::CpRp(Elem* aCp): iElem(aCp)
{
    // Set text from CP name
    set_text(iElem->Name());
}

CpRp::~CpRp()
{
}

bool CpRp::on_expose_event(GdkEventExpose* event)
{
    Gtk::Label::on_expose_event(event);
}

void CpRp::on_size_allocate(Gtk::Allocation& aAlloc)
{
    Gtk::Label::on_size_allocate(aAlloc);
}

void CpRp::on_size_request(Gtk::Requisition* aRequisition)
{
    Gtk::Label::on_size_request(aRequisition);
}

MCompatChecker::TDir CpRp::GetDir() const
{
    MCompatChecker::TDir res = MCompatChecker::ERegular;
    MCompatChecker* cc = iElem->GetObj(cc);
    if (cc != NULL) {
	res = cc->GetDir();
    }
    return res;
}

//  Data (property) representation
DataRp::DataRp(Elem* aModel, const string& aDataName, MMdlObserver* aMdlObs): iElem(aModel), iMdlObs(aMdlObs),
    iDataName(aDataName)
{
    // Set text from Value
    /*
    MProp* prop = iElem->GetObj(prop);
    assert(prop != NULL);
    set_text(prop->Value());
    */
    string cont;
    iElem->GetCont(cont);
    set_text(iDataName + ": " + cont);
    iMdlObs->SignalCompChanged().connect(sigc::mem_fun(*this, &DataRp::on_comp_changed));
    iMdlObs->SignalContentChanged().connect(sigc::mem_fun(*this, &DataRp::on_comp_changed));
}

void DataRp::on_comp_changed(Elem* aComp)
{
    if (aComp == iElem) {
	//std::cout << "DataRp::on_comp_changed" << std::endl;
	/*
	MProp* prop = iElem->GetObj(prop);
	assert(prop != NULL);
	set_text(prop->Value());
	*/
	string cont;
	iElem->GetCont(cont);
	set_text(iDataName + ": " + cont);
    }
}


// System representation
const string sSysCrpType = "SysCrp";

const string& SysCrp::Type()
{
    return sSysCrpType;
}

string SysCrp::EType()
{
    return Syst::PEType();
}

SysCrp::SysCrp(Elem* aElem, MMdlObserver* aMdlObs, const string& aDataUri): VertCompRp(aElem), iDataUri(aDataUri),
    iMdlObs(aMdlObs)
{
}

void SysCrp::Construct()
{
    // Add CPs
    for (std::vector<Elem*>::iterator it = iElem->Comps().begin(); it != iElem->Comps().end(); it++) {
	Elem* comp = *it;
	if (comp->IsHeirOf(ConnPointBase::PEType()) || comp->IsHeirOf("Vert:Extender")) {
	    CpRp* rp = new CpRp(comp);
	    add(*rp);
	    rp->show();
	    iCpRps[comp] = rp;
	}
    }
    // Add Data Crp
    AddDataRp();
}

SysCrp::~SysCrp()
{
}

void SysCrp::AddDataRp()
{
    if (!iDataUri.empty()) {
	Elem* data = iElem->GetNode(iDataUri);
	if (data != NULL) {
	    DataRp* rp = new DataRp(data, "Data", iMdlObs);
	    add(*rp);
	    rp->show();
	    iDataRps[data] = rp;
	}    
    }
    Elem* vdata = iElem->GetNode("ViewData");
    if (vdata != NULL) {
	for (vector<Elem*>::iterator it = vdata->Comps().begin(); it != vdata->Comps().end(); it++) {
	    Elem* comp = *it;
	    MProp* pcomp = comp->GetObj(pcomp);
	    if (pcomp != NULL) {
		Elem* data = comp->GetNode(pcomp->Value());
		if (data != NULL) {
		    DataRp* rp = new DataRp(data, comp->Name(), iMdlObs);
		    add(*rp);
		    rp->show();
		    iDataRps[data] = rp;
		}    
	    }
	}
    }
}

void *SysCrp::DoGetObj(const string& aName)
{
    void* res = NULL;
    if (aName ==  Type()) {
	res = this;
    }
    else if (aName ==  MCrpConnectable::Type()) {
	res = (MCrpConnectable*) this;
    }
    return res;
}


Gtk::Widget& SysCrp::Widget()
{
    return *this;
}

Gtk::Requisition SysCrp::GetCpCoord(Elem* aCp)
{
    Gtk::Allocation alc = get_allocation();
    Gtk::Requisition res;
    if (iCpRps.count(aCp) > 0) {
	CpRp* cprp = iCpRps.at(aCp);
	Gtk::Allocation cpalc = cprp->get_allocation();
	if (cprp->GetDir() == MCompatChecker::EInp) {
	    res.width = alc.get_x() + cpalc.get_x();
	}
	else {
	    res.width = alc.get_x() + cpalc.get_x() + cpalc.get_width();
	}
	res.height = alc.get_y() + cpalc.get_y() + cpalc.get_height()/2;
    }
    else {
	res = VertCompRp::GetCpCoord(aCp);
    }
    return res;
}

int SysCrp::GetNearestCp(Gtk::Requisition aCoord, Elem*& aCp)
{
    int res = -1;
    Elem* ncp = NULL; // Nearest CP
    for (tCpRps::iterator it = iCpRps.begin(); it != iCpRps.end(); it++) {
	Elem* cp = it->first;
	Gtk::Requisition cpcoord = GetCpCoord(cp);
	std::complex<float> sub(cpcoord.width - aCoord.width, cpcoord.height - aCoord.height);
	int dist = std::abs(sub);
	if (res == -1 || dist < res) {
	    res = dist;
	    ncp = cp;
	}
    }
    if (ncp != NULL) {
	aCp = ncp;
    }
    return res;
}

void SysCrp::HighlightCp(Elem* aCp, bool aSet)
{
    Elem* cp = aCp;
    assert(cp != NULL && iCpRps.count(cp) > 0);
    Gtk::Widget* cprp = iCpRps.at(cp);
    if (aSet) {
	cprp->set_state(Gtk::STATE_PRELIGHT);
    }
    else {
	cprp->set_state(Gtk::STATE_NORMAL);
    }
}

bool SysCrp::on_expose_event(GdkEventExpose* event)
{
    VertCompRp::on_expose_event(event);
}

void SysCrp::on_size_allocate(Gtk::Allocation& aAlloc)
{
    // Base allocation
    VertCompRp::on_size_allocate(aAlloc);
    Gtk::Requisition head_req = iHead->size_request();
    int c_x = iBodyAlc.get_x(), c_y = iBodyAlc.get_y() + head_req.height;
    // Allocating CPs
    //    CP allocation X base: X of right position
    int cpaxb = aAlloc.get_width() - KViewConnGapWidth;
    int cpalxb = KViewConnGapWidth;
    //    CP allocation Y base: mid of Y position
    int cpayb = c_y + KViewCompEmptyBodyHight/2;
    int cpalyb = c_y + KViewCompEmptyBodyHight/2;
    for (tCpRps::iterator it = iCpRps.begin(); it != iCpRps.end(); it++) {
	CpRp* cprp = it->second;
	Gtk::Requisition cpreq = cprp->size_request();
	Gtk::Allocation cpalc;
	if (cprp->GetDir() == MCompatChecker::EInp) {
	    cpalc = Allocation(cpalxb, cpalyb, cpreq.width, cpreq.height);
	    cpalyb += cpreq.height + KViewConnGapHeight;
	}
	else {
	    cpalc = Allocation(cpaxb - cpreq.width, cpayb, cpreq.width, cpreq.height);
	    cpayb += cpreq.height + KViewConnGapHeight;
	}
	cprp->size_allocate(cpalc);
    }
    c_y = cpayb;
    // Allocating data
    if (!iDataRps.empty()) {
	for (tDataRps::iterator it = iDataRps.begin(); it != iDataRps.end(); it++) {
	    DataRp* rp = it->second;
	    Requisition data_r = rp->size_request();
	    Allocation data_alc = Allocation(c_x, c_y, iBodyAlc.get_width(), data_r.height);
	    rp->size_allocate(data_alc);
	    c_y += data_r.height;
	}
    }
}

void SysCrp::on_size_request(Gtk::Requisition* aReq)
{
    // Base size
    VertCompRp::on_size_request(aReq);
    // Add size of CPs
    int cp_height = 0, cp_width = 0;
    int cpl_height = 0, cpl_width = 0;
    for (tCpRps::iterator it = iCpRps.begin(); it != iCpRps.end(); it++) {
	CpRp* cprp = it->second;
	Gtk::Requisition cpreq = cprp->size_request();
	if (cprp->GetDir() == MCompatChecker::EInp) {
	    cpl_height += cpreq.height + KViewConnGapHeight;
	    cpl_width = max(cpl_width, cpreq.width + KViewConnGapWidth);
	}
	else {
	    cp_height += cpreq.height + KViewConnGapHeight;
	    cp_width = max(cp_width, cpreq.width + KViewConnGapWidth);
	}
    }
    aReq->height += max(cpl_height, cp_height);
    aReq->width = max(aReq->width, cpl_width + cp_width + KCrpBodyMinWidth);
    // Add size of data rp
    int data_h = 0, data_w = 0;
    if (!iDataRps.empty()) {
	for (tDataRps::iterator it = iDataRps.begin(); it != iDataRps.end(); it++) {
	    DataRp* rp = it->second;
	    Requisition data_req = rp->size_request();
	    data_h = data_req.height;
	    data_w = data_req.width;
	    aReq->height +=  data_h;
	    aReq->width = max(aReq->width, data_w + 2*KViewElemCrpInnerBorder); 
	}
    }
}


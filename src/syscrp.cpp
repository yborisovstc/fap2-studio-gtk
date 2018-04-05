#include <mprop.h>
#include <complex>
#include <syst.h>
#include "syscrp.h"
#include "common.h"
#include <iostream>
#include <gtkmm/toolitem.h>

//  Connection point representation
CpRp::CpRp(MElem* aCp): iElem(aCp)
{
    // Set text from CP name
    set_text(iElem->Name());
    set_name("CpRp");
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
    //MCompatChecker* cc = iElem->GetObj(cc);
    MCompatChecker* cc = (MCompatChecker*) iElem->GetSIfiC(MCompatChecker::Type());
    if (cc != NULL) {
	res = cc->GetDir();
    }
    return res;
}

//  Data (property) representation
DataRp::DataRp(MElem* aModel, const string& aDataName, MMdlObserver* aMdlObs): iElem(aModel), iMdlObs(aMdlObs),
    iDataName(aDataName)
{
    // Set text from Value
    /*
    MProp* prop = iElem->GetObj(prop);
    assert(prop != NULL);
    set_text(prop->Value());
    */
    string cont = iElem->GetContent();
    set_text(iDataName.empty() ? cont : iDataName + ": " + cont);
    iMdlObs->SignalCompChanged().connect(sigc::mem_fun(*this, &DataRp::on_comp_changed));
    iMdlObs->SignalContentChanged().connect(sigc::mem_fun(*this, &DataRp::on_comp_changed));
}

void DataRp::on_comp_changed(MElem* aComp)
{
    if (aComp == iElem) {
	//std::cout << "DataRp::on_comp_changed" << std::endl;
	/*
	MProp* prop = iElem->GetObj(prop);
	assert(prop != NULL);
	set_text(prop->Value());
	*/
	string cont = iElem->GetContent();
	set_text(iDataName.empty() ? cont : iDataName + ": " + cont);
	set_has_tooltip();
	set_tooltip_text(iDataName + ": " + cont);
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

SysCrp::SysCrp(MElem* aElem, MMdlObserver* aMdlObs, const string& aDataUri): VertCompRp(aElem), iDataUri(aDataUri),
    iMdlObs(aMdlObs)
{
}

void SysCrp::Construct()
{
    VertCompRp::Construct();
    // Add CPs - only representatives (i.e. connpoints)
    for (TInt ci = 0; ci < iElem->CompsCount(); ci++) {
	MElem* comp = iElem->GetComp(ci);
	if (comp->IsRemoved()) continue;
	MCompatChecker* mcp = comp->GetObj(mcp);
	if (mcp == NULL) continue;
	CpRp* rp = new CpRp(comp);
	add(*rp);
	rp->show();
	iCpRps[comp] = rp;
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
	MElem* data = iElem->GetNode(iDataUri);
	if (data != NULL) {
	    DataRp* rp = new DataRp(data, "Data", iMdlObs);
	    add(*rp);
	    rp->show();
	    iDataRps[data] = rp;
	}    
    }
    MElem* vdata = iElem->GetNode("./ViewData");
    if (vdata != NULL) {
	for (TInt ci = 0; ci < vdata->CompsCount(); ci++) {
	    MElem* comp = vdata->GetComp(ci);
	    MProp* pcomp = comp->GetObj(pcomp);
	    if (pcomp != NULL) {
		MElem* data = comp->GetNode(pcomp->Value());
		if (data != NULL) {
		    DataRp* rp = new DataRp(data, comp->Name(), iMdlObs);
		    add(*rp);
		    rp->show();
		    iDataRps[data] = rp;
		}    
	    }
	}
    } else { // Multicontent
	TInt cnt = iElem->GetContCount("ViewData");
	for (TInt ci = 0; ci < cnt; ci++) {
	    string id = iElem->GetContComp("ViewData", ci);
	    string val = iElem->GetContent(id);
	    if (!val.empty()) {
		MElem* data = iElem->GetNode(val);
		if (data != NULL) {
		    DataRp* rp = new DataRp(data, MElem::GetContentLName(id), iMdlObs);
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

Gtk::Requisition SysCrp::GetCpCoord(MElem* aCp)
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

int SysCrp::GetNearestCp(Gtk::Requisition aCoord, MElem*& aCp)
{
    int res = -1;
    MElem* ncp = NULL; // Nearest CP
    for (tCpRps::iterator it = iCpRps.begin(); it != iCpRps.end(); it++) {
	MElem* cp = it->first;
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

/*
Gtk::Allocation SysCrp::GetCpAlloc(MElem* aCp)
{
    Gtk::Allocation alc = get_allocation();
    Gtk::Allocation res;
    if (iCpRps.count(aCp) > 0) {
	CpRp* cprp = iCpRps.at(aCp);
	Gtk::Allocation cpalc = cprp->get_allocation();
	res = Gtk::Allocation(alc.get_x() + cpalc.get_x(), alc.get_y() + cpalc.get_y(),
		cpalc.get_width(), cpalc.get_height()); 
    }
    return res;
}
*/

Gtk::Allocation SysCrp::GetCpAlloc(MElem* aCp)
{
    Gtk::Allocation res;
    if (iCpRps.count(aCp) > 0) {
	CpRp* cprp = iCpRps.at(aCp);
	res = cprp->get_allocation();
    }
    return res;
}

MElem* SysCrp::GetHoweredCp(Gtk::Requisition aCoord) const
{
    MElem* res = NULL;
    for (tCpRps::const_iterator it = iCpRps.begin(); it != iCpRps.end(); it++) {
	MElem* cp = it->first;
	Gtk::Allocation cpalloc = ((SysCrp*) this)->GetCpAlloc(cp);
	TBool in = aCoord.width > cpalloc.get_x() &&
	    aCoord.width < (cpalloc.get_x() + cpalloc.get_width()) &&
	    aCoord.height > cpalloc.get_y() &&
	    aCoord.height < (cpalloc.get_y() + cpalloc.get_height());
	if (in) {
	    res = cp; break;
	}
    }
    return res;
}

void SysCrp::HighlightCp(MElem* aCp, bool aSet)
{
    MElem* cp = aCp;
    assert(cp != NULL && iCpRps.count(cp) > 0);
    Gtk::Widget* cprp = iCpRps.at(cp);
    std::cout << "SysCrp::HighlightCp, aSet: " << aSet << ", aCp: " << aCp->Uid() << ", widget: " << cprp->get_name()  << std::endl;
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
    c_y = max(cpayb, cpalyb);
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
	    data_h += data_req.height;
	    data_w = max(data_w, data_req.width);
	}
    }
    aReq->height += data_h;
    aReq->width = max(aReq->width, data_w + 2*KViewElemCrpInnerBorder); 
}

void SysCrp::GetModelDebugInfo(int x, int y, string& aData) const
{
//    std::cout << "SysCrp::GetModelDebugInfo" << std::endl;
    Gtk::Requisition coord = {x, y};
    MElem* cp = GetHoweredCp(coord);
    if (cp != NULL) {
	//std::cout << "SysCrp::GetModelDebugInfo, howered cp: " << cp->Uid() << std::endl;
	if (iElem->ContentExists("SpAgent")) {
	    string agt_uri = iElem->GetContent("SpAgent");
	    MElem* agent = iElem->GetNode(agt_uri);
	    if (agent != NULL) {
		GUri uri;
		cp->GetRUri(uri, agent);
		aData = agent->GetAssociatedData(uri.toString(ETrue));
	    }
	} else {
	    //GetFormattedContent(cp, aData);
	    GetContentFormatted(cp, "", 0, aData);
	}
    } else {
	VertCompRp::GetModelDebugInfo(x, y, aData);
    }
}

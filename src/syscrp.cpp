#include <syst.h>
#include "syscrp.h"
#include "common.h"

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

SysCrp::SysCrp(Elem* aElem): VertCompRp(aElem)
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
}

SysCrp::~SysCrp()
{
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
    return VertCompRp::GetCpCoord(aCp);
}

bool SysCrp::on_expose_event(GdkEventExpose* event)
{
    VertCompRp::on_expose_event(event);
}

void SysCrp::on_size_allocate(Gtk::Allocation& 	aAlloc)
{
    // Base allocation
    VertCompRp::on_size_allocate(aAlloc);
    // Allocating CPs
    Gtk::Requisition head_req = iHead->size_request();
    //    CP allocation X base: X of right position
    int cpaxb = aAlloc.get_width();
    //    CP allocation Y base: mid of Y position
    int cpayb = (aAlloc.get_height() + head_req.height)/2;

    for (tCpRps::iterator it = iCpRps.begin(); it != iCpRps.end(); it++) {
	CpRp* cprp = it->second;
	Gtk::Requisition cpreq = cprp->size_request();
	Gtk::Allocation cpalc(cpaxb - cpreq.width, cpayb - cpreq.height/2, cpreq.width, cpreq.height);
	cpayb += cpreq.height + KViewConnGapHeight;
	cprp->size_allocate(cpalc);
    }
}

void SysCrp::on_size_request(Gtk::Requisition* aReq)
{
    // Base size
    VertCompRp::on_size_request(aReq);
    // Add height of CPs
    for (tCpRps::iterator it = iCpRps.begin(); it != iCpRps.end(); it++) {
	CpRp* cprp = it->second;
	Gtk::Requisition cpreq = cprp->size_request();
	aReq->height += cpreq.height + KViewConnGapHeight;
    }
}


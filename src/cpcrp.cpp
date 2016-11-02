
#include "cpcrp.h"
#include "common.h"
#include <mprop.h>


const string KProvUri = "./Provided";
const string KReqUri = "./Required";
const string sType = "CpCrp";

const string& CpCrp::Type()
{
    return sType;
}

string CpCrp::EType()
{
    return "Elem:Vert:ConnPointBase:ConnPoint";
}

string CpCrp::IfaceSupported()
{
    return MConnPoint::Type();
}

CpCrp::CpCrp(MElem* aElem): VertCompRp(aElem)
{
    // Prepare data of "provided"
    MElem* prov = iElem->GetNode(KProvUri);
    assert(prov != NULL);
    MProp* mprop = prov->GetObj(mprop);
    assert(mprop != NULL);
    string pval("P: ");
    pval += mprop->Value();
    iLabProv = new Gtk::Label(pval, Gtk::ALIGN_LEFT);
    add(*iLabProv);
    iLabProv->show();
    // Prepare data of "required"
    MElem* req = iElem->GetNode(KReqUri);
    assert(req != NULL);
    mprop = req->GetObj(mprop);
    assert(mprop != NULL);
    string rval("R: ");
    rval += mprop->Value();
    iLabReq = new Gtk::Label(rval, Gtk::ALIGN_LEFT);
    add(*iLabReq);
    iLabReq->show();
    set_name("CpCrp");
}

CpCrp::~CpCrp()
{
}

void *CpCrp::DoGetObj(const string& aName)
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

void CpCrp::on_size_allocate(Gtk::Allocation& aAlloc)
{
    VertCompRp::on_size_allocate(aAlloc);
    Gtk::Requisition head_req = iHead->size_request();
    // Allocate size of "provided"
    Gtk::Requisition lprov_size = iLabProv->size_request();
    int lprov_x = KViewElemCrpInnerBorder;
    int lprov_y = head_req.height + KViewCompEmptyBodyHight/2;
    int lprov_w = aAlloc.get_width() - 2* KViewElemCrpInnerBorder;
    int lprov_h = lprov_size.height;
    Gtk::Allocation lprov_alc(lprov_x, lprov_y, lprov_w, lprov_h);
    iLabProv->size_allocate(lprov_alc);
    // Allocate size of "required"
    Gtk::Requisition lreq_size = iLabReq->size_request();
    int lreq_x = KViewElemCrpInnerBorder;
    int lreq_y = lprov_y + lprov_h + KViewElemContLinesVGap;
    int lreq_w = aAlloc.get_width() - 2* KViewElemCrpInnerBorder;
    int lreq_h = lreq_size.height;
    Gtk::Allocation lreq_alc(lreq_x, lreq_y, lreq_w, lreq_h);
    iLabReq->size_allocate(lreq_alc);

}

void CpCrp::on_size_request(Gtk::Requisition* aReq)
{
    VertCompRp::on_size_request(aReq);
    // Updating with size of "provided"
    Gtk::Requisition labprov_size = iLabProv->size_request();
    aReq->height += labprov_size.height;
    aReq->width = max(aReq->width, labprov_size.width + 2*KViewElemCrpInnerBorder);
    // Updating with size of "required"
    Gtk::Requisition labreq = iLabReq->size_request();
    aReq->height += labreq.height + KViewElemContLinesVGap;
    aReq->width = max(aReq->width, labreq.width + 2*KViewElemCrpInnerBorder);
}

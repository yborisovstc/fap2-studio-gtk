
#include <iostream>
#include "extdcrp.h"
#include "common.h"
#include <mprop.h>
#include <complex>


const string KIntUri = "./Int";
const string sType = "ExtdCrp";

const string& ExtdCrp::Type()
{
    return sType;
}

string ExtdCrp::EType()
{
    return "Elem:Vert:Extender";
}

ExtdCrp::ExtdCrp(Elem* aElem, MErpProvider& aErpProv): VertCompRp(aElem), iErpProv(aErpProv), iInt(NULL),
    iExtPos(MErp::EPos_Left)
{
    // Prepare data of "Internal"
    Elem* intr = iElem->GetNode(KIntUri);
    assert(intr != NULL);
    string pval = intr->Name();
    iInt = iErpProv.CreateRp(*intr, this);
    iIntw = &(iInt->Widget());
    //iLabInt = new Gtk::Label(pval, Gtk::ALIGN_LEFT);
    //add(*iLabInt);
    //iLabInt->show();
    add(iInt->Widget());
    MErp::TPos pos = MErp::EPos_Right;
    if (iInt->GetMdlDir() == MCompatChecker::EInp) {
	pos = MErp::EPos_Left;
	iExtPos = MErp::EPos_Right;
    }
    else if (iInt->GetMdlDir() == MCompatChecker::ERegular) {
	if (GetLArea() == ERight) {
	    pos = MErp::EPos_Left;
	    iExtPos = MErp::EPos_Right;
	}
    }
    iInt->SetPos(pos);
    iInt->Widget().show();
    // Prepare data of "Extended"
    iLabExt = new Gtk::Label("Ext", Gtk::ALIGN_LEFT);
    add(*iLabExt);
    iLabExt->show();
}

ExtdCrp::~ExtdCrp()
{
}

void *ExtdCrp::DoGetObj(const string& aName)
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

void ExtdCrp::SetLArea(int aArea)
{
    if (iLArea != aArea) {
	iLArea = aArea;
	// Update position of ERPs
	iExtPos = MErp::EPos_Left;
	MErp::TPos pos = MErp::EPos_Right;
	if (iInt->GetMdlDir() == MCompatChecker::EInp) {
	    pos = MErp::EPos_Left;
	    iExtPos = MErp::EPos_Right;
	}
	else if (iInt->GetMdlDir() == MCompatChecker::ERegular) {
	    if (GetLArea() == ERight) {
		pos = MErp::EPos_Left;
		iExtPos = MErp::EPos_Left;
	    }
	    else {
		pos = MErp::EPos_Right;
		iExtPos = MErp::EPos_Right;
	    }
	}
	iInt->SetPos(pos);
    }
}

void ExtdCrp::on_size_allocate(Gtk::Allocation& aAlloc)
{
    VertCompRp::on_size_allocate(aAlloc);
    Gtk::Requisition head_req = iHead->size_request();
    // Allocate size of "Internal"
    Gtk::Requisition lint_size = iIntw->size_request();
    int lint_x = KViewElemCrpInnerBorder;
    if (iInt->GetPos() == MErp::EPos_Right) {
	lint_x = aAlloc.get_width() - KViewElemCrpInnerBorder - lint_size.width;
    }
    int lint_y = head_req.height + KViewCompEmptyBodyHight/2;
    int lint_w = lint_size.width;
    int lint_h = lint_size.height;
    Gtk::Allocation lint_alc(lint_x, lint_y, lint_w, lint_h);
    iIntw->size_allocate(lint_alc);
    // Allocate size of "Extended"
    Gtk::Requisition lext_size = iLabExt->size_request();
    int lext_x = KViewElemCrpInnerBorder;
    if (iExtPos == MErp::EPos_Right) {
	lext_x = aAlloc.get_width() - KViewElemCrpInnerBorder - lext_size.width;
    }
    int lext_y = lint_y + lint_h + KViewElemContLinesVGap;
    int lext_w = aAlloc.get_width() - 2* KViewElemCrpInnerBorder;
    int lext_h = lext_size.height;
    Gtk::Allocation lext_alc(lext_x, lext_y, lext_w, lext_h);
    iLabExt->size_allocate(lext_alc);

}

void ExtdCrp::on_size_request(Gtk::Requisition* aReq)
{
    VertCompRp::on_size_request(aReq);
    // Updating with size of "Internal"
    Gtk::Requisition lab_size = iIntw->size_request();
    aReq->height += lab_size.height;
    aReq->width = max(aReq->width, lab_size.width + 2*KViewElemCrpInnerBorder);
    // Updating with size of "Extended"
    Gtk::Requisition labe_size = iLabExt->size_request();
    aReq->height += labe_size.height;
    aReq->width = max(aReq->width, labe_size.width + 2*KViewElemCrpInnerBorder);
}

Gtk::Requisition ExtdCrp::GetCpCoord(Elem* aCp)
{
    Gtk::Allocation alc = get_allocation();
    Gtk::Requisition res;
    Elem* intcp = iElem->GetNode(KIntUri);
    assert(intcp != NULL);
    assert(aCp == iElem || aCp == intcp || intcp->IsComp(aCp));
    if (aCp == iElem) {
	Gtk::Allocation cpalc = iLabExt->get_allocation();
	res.width = alc.get_x();
	res.height = alc.get_y() + cpalc.get_y() + cpalc.get_height()/2;
    }
    else {
	MErpConnectable* erpc = iInt->GetObj(erpc);
	if (erpc != NULL) {
	    Requisition cpres = erpc->GetCpCoord(aCp);
	    res.width = iInt->GetPos() == MErp::EPos_Left ? alc.get_x() : alc.get_x() + alc.get_width();
	    res.height = alc.get_y() + cpres.height;
	}
    }
    return res;
}

int ExtdCrp::GetNearestCp(Gtk::Requisition aCoord, Elem*& aCp)
{
    int res = -1;
    Elem* ncp = NULL; // Nearest CP
    Gtk::Requisition cpcoord = GetCpCoord(iElem);
    std::complex<int> sub(cpcoord.width - aCoord.width, cpcoord.height - aCoord.height);
    int dist = std::abs(sub);
    if (res == -1 || dist < res) {
	res = dist;
	ncp = iElem;
    }
    Elem* cpcand = NULL;
    MErpConnectable* erpc = iInt->GetObj(erpc);
    if (erpc != NULL) {
	// We cannot just pass aCoord to ERp because aCoord is upper window relative, i.e. window of Drp
	// but ERp knows nothing of this window but uses window of Crp. So we need to transform coord
	// to make it relative to Crp window
	Allocation crpalc = get_allocation();
	Requisition rcoord = {aCoord.width - crpalc.get_x(), aCoord.height - crpalc.get_y()};
	dist = erpc->GetNearestCp(rcoord, cpcand);
	if (res == -1 || dist < res) {
	    res = dist;
	    ncp = cpcand;
	}
    }
    if (ncp != NULL) {
	aCp = ncp;
    }
    //std::cout << "ExtdCrp::GetNearestCp, cp: " << aCp->Name()  << std::endl;
    return res;
}

void ExtdCrp::HighlightCp(Elem* aCp, bool aSet)
{
    Elem* intcp = iElem->GetNode(KIntUri);
    if (aCp == iElem) {
	iLabExt->set_state(aSet ? STATE_PRELIGHT: STATE_NORMAL);
    }
    else {
	MErpConnectable* erpc = iInt->GetObj(erpc);
	if (erpc != NULL) {
	    erpc->HighlightCp(aCp, aSet);
	}
    }
}


bool ExtdCrp::IsTypeAllowed(const std::string& aType) const
{
    return true;
}


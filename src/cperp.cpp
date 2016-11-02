#include <complex>
#include "common.h"
#include "cperp.h"
#include "gtkmm/alignment.h"
#include <iostream>

const string sType = "CpErp";

const string& CpErp::Type()
{
    return sType;
}

string CpErp::EType()
{
    return "Elem:Vert:ConnPointBase:ConnPoint";
}

string CpErp::IfaceSupported()
{
    return MConnPoint::Type();
}

CpErp::CpErp(MElem* aElem): Label(), iElem(aElem), iPos(EPos_Right), iHighlighted(false)
{
    set_label(iElem->Name());
    AlignmentEnum xalign = ALIGN_LEFT;
    if (iPos == EPos_Right) {
	xalign = ALIGN_RIGHT;
    }
    set_alignment(xalign);
}

CpErp::~CpErp()
{
}

void *CpErp::DoGetObj(const string& aName)
{
    void* res = NULL;
    if (aName ==  Type()) {
	res = this;
    }
    else if (aName ==  MErpConnectable::Type()) {
	res = (MErpConnectable*) this;
    }
    return res;
}

Gtk::Widget& CpErp::Widget()
{
    return *this;
}

void CpErp::SetHighlighted(bool aSet)
{
    DoSetHighlighted(aSet);
}

MElem* CpErp::Model()
{
    return iElem;
}

void CpErp::DoSetHighlighted(bool aSet)
{
    if (aSet != iHighlighted) {
	iHighlighted = aSet;
	if (iHighlighted) {
	    set_state(Gtk::STATE_PRELIGHT);
	} else {
	    set_state(Gtk::STATE_NORMAL);
	}
    }
}

void CpErp::SetPos(MErp::TPos aPos)
{
    if (iPos != aPos) {
	iPos = aPos;
    }
}

MErp::TPos CpErp::GetPos() const
{
    return iPos;
}

MCompatChecker::TDir CpErp::GetMdlDir() const
{
    MCompatChecker::TDir res = MCompatChecker::ERegular;
    MCompatChecker* cc = iElem->GetObj(cc);
    if (cc != NULL) {
	res = cc->GetDir();
    }
    return res;
}

Requisition CpErp::GetCpCoord(MElem* aCp) 
{
    Gtk::Allocation alc = get_allocation();
    Gtk::Requisition res;
    assert(aCp == iElem);
    res.width = (GetPos() == MErp::EPos_Left) ? alc.get_x() : alc.get_x() + alc.get_width();
    res.height = alc.get_y() + alc.get_height()/2;
    return res;
}

int CpErp::GetNearestCp(Gtk::Requisition aCoord, MElem*& aCp)
{
    int res = 0;
    Gtk::Requisition cpcoord = GetCpCoord(iElem);
    std::complex<float> sub(cpcoord.width - aCoord.width, cpcoord.height - aCoord.height);
    res = std::abs(sub);
    aCp = iElem;
    return res;
}

void CpErp::HighlightCp(MElem* aCp, bool aSet)
{
    assert(aCp == iElem);
    std::cout << "CpErp::HighlightCp, aSet: " << aSet  << std::endl;
    set_state(aSet ? STATE_PRELIGHT : STATE_NORMAL);
}





// Socket Erp

const int kPinPadding = 9;

const string sSockErpType = "SockErp";
const string sCpEType = ":Elem:Vert:ConnPointBase";
const string sCpEType2 = ":Elem:Vert:ConnPointMc";
const string sSockEType = ":Elem:Vert:Socket";
const string sSockEType2 = ":Elem:Vert:SocketMc";

const string& SockErp::Type()
{
    return sSockErpType;
}

string SockErp::EType()
{
    return sSockEType;
}

string SockErp::IfaceSupported()
{
    return MSocket::Type();
}

SockErp::SockErp(MElem* aElem, const MErpProvider& aErpProv): VBox(), iElem(aElem), iPos(EPos_Left), iHighlighted(false),
    iErpProv(aErpProv)
{
    iName = new Label(iElem->Name());
    pack_start(*iName);
    iName->show();
    AlignmentEnum xalign = ALIGN_LEFT;
    if (iPos == EPos_Right) {
	xalign = ALIGN_RIGHT;
    }
    iName->set_alignment(xalign);
    // Pins
    for (TInt ci = 0; ci < iElem->CompsCount(); ci++) {
	MElem* comp = iElem->GetComp(ci);
	if (comp->IsHeirOf(sCpEType) || comp->IsHeirOf(sCpEType2) || comp->IsHeirOf(sSockEType) || comp->IsHeirOf(sSockEType2)) {
	    MErp* pinrp = iErpProv.CreateRp(*comp, this);
	    if (pinrp != NULL) {
		iPinRps[comp] = pinrp;
		pinrp->SetPos(GetPos());
		Alignment* alg = new Alignment(xalign, ALIGN_CENTER); 
		alg->set_padding(0, 0, kPinPadding, 0);
		alg->add(pinrp->Widget());
		pack_end(*alg);
		alg->show();
		pinrp->Widget().show();
	    }
	}
    }
}

SockErp::~SockErp()
{
}

void *SockErp::DoGetObj(const string& aName)
{
    void* res = NULL;
    if (aName ==  Type()) {
	res = this;
    }
    else if (aName ==  MErpConnectable::Type()) {
	res = (MErpConnectable*) this;
    }
    return res;
}

Gtk::Widget& SockErp::Widget()
{
    return *this;
}

void SockErp::SetHighlighted(bool aSet)
{
    DoSetHighlighted(aSet);
}

MElem* SockErp::Model()
{
    return iElem;
}

void SockErp::DoSetHighlighted(bool aSet)
{
    if (aSet != iHighlighted) {
	iHighlighted = aSet;
	if (iHighlighted) {
	    set_state(Gtk::STATE_PRELIGHT);
	} else {
	    set_state(Gtk::STATE_NORMAL);
	}
    }
}

void SockErp::SetPos(MErp::TPos aPos)
{
    if (iPos != aPos) {
	iPos = aPos;
	for (tPinRps::iterator it = iPinRps.begin(); it != iPinRps.end(); it++) {
	    MErp* rp = it->second;
	    rp->SetPos(iPos);
	}
    }

}

MErp::TPos SockErp::GetPos() const
{
    return iPos;
}

MCompatChecker::TDir SockErp::GetMdlDir() const
{
    MCompatChecker::TDir res = MCompatChecker::ERegular;
    MCompatChecker* cc = iElem->GetObj(cc);
    if (cc != NULL) {
	res = cc->GetDir();
    }
    return res;
}

bool SockErp::IsTypeAllowed(const std::string& aType) const
{
    return true;
}

Requisition SockErp::GetCpCoord(MElem* aCp) 
{
    Gtk::Allocation alc = get_allocation();
    Gtk::Requisition res;
    if (aCp == iElem) {
	Gtk::Allocation cpalc = iName->get_allocation();
	res.width = cpalc.get_x();
	res.height = cpalc.get_y() + cpalc.get_height()/2;
    } 
    else if (iPinRps.count(aCp) > 0) {
	MErp* erp = iPinRps.at(aCp);
	MErpConnectable* erpc = erp->GetObj(erpc);
	if (erpc != NULL) {
	    res = erpc->GetCpCoord(aCp);
	}
	else {
	    Gtk::Allocation cpalc = erp->Widget().get_allocation();
	    res.width = cpalc.get_x();
	    res.height = cpalc.get_y() + cpalc.get_height()/2;
	}
    }
    else {
	for (tPinRps::iterator it = iPinRps.begin(); it != iPinRps.end(); it++) {
	    MErp* rp = it->second;
	}
    }
    /*
    res.width += alc.get_x();
    res.height += alc.get_y();
    */
    return res;
}

int SockErp::GetNearestCp(Gtk::Requisition aCoord, MElem*& aCp)
{
    int res = -1;
    MElem* ncp = NULL; // Nearest CP
    Gtk::Requisition cpcoord = GetCpCoord(iElem);
    std::complex<float> sub(cpcoord.width - aCoord.width, cpcoord.height - aCoord.height);
    int dist = std::abs(sub);
    if (res == -1 || dist < res) {
	res = dist;
	ncp = iElem;
    }
    for (tPinRps::iterator it = iPinRps.begin(); it != iPinRps.end(); it++) {
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

void SockErp::HighlightCp(MElem* aCp, bool aSet)
{
    assert(aCp != NULL);
    if (aCp == iElem) {
	iName->set_state(aSet ? STATE_PRELIGHT : STATE_NORMAL);
    }
    else if (iPinRps.count(aCp) > 0) {
	MErp* erp = iPinRps.at(aCp);
	MErpConnectable* erpc = erp->GetObj(erpc);
	if (erpc != NULL) {
	    erpc->HighlightCp(aCp, aSet);
	}
	else {
	    erp->Widget().set_state(aSet ? STATE_PRELIGHT : STATE_NORMAL);
	}
    }
    else {
	for (tPinRps::iterator it = iPinRps.begin(); it != iPinRps.end(); it++) {
	    MErp* rp = it->second;
	}
    }
}



#include "sysdrp.h"


string SysDrp::EType()
{
    return ":Elem:Vert:Syst";
}

SysDrp::SysDrp(Elem* aElem, const MCrpProvider& aCrpProv): VertDrpw(aElem, aCrpProv)
{
}

SysDrp::~SysDrp()
{
}


void *SysDrp::DoGetObj(const string& aName)
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

Gtk::Widget& SysDrp::Widget()
{
    return *(static_cast<Gtk::Widget*>(this));
}

Elem* SysDrp::Model()
{
    return iElem;
}

MDrp::tSigCompSelected SysDrp::SignalCompSelected()
{
    return iSigCompSelected;
}

void SysDrp::on_size_allocate(Gtk::Allocation& aAlloc)
{
    VertDrpw::on_size_allocate(aAlloc);
}

void SysDrp::on_size_request(Gtk::Requisition* aRequisition)
{
    VertDrpw::on_size_request(aRequisition);
}


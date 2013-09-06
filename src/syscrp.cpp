#include <syst.h>
#include "syscrp.h"


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

Gtk::Requisition SysCrp::GetCpCoord(MCrpConnectable::CpType aCpType)
{
    return VertCompRp::GetCpCoord(aCpType);
}

bool SysCrp::on_expose_event(GdkEventExpose* event)
{
    VertCompRp::on_expose_event(event);
}

void SysCrp::on_size_allocate(Gtk::Allocation& 	aAlloc)
{
    VertCompRp::on_size_allocate(aAlloc);
}

void SysCrp::on_size_request(Gtk::Requisition* aRequisition)
{
    VertCompRp::on_size_request(aRequisition);
}


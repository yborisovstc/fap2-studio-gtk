
#include "cpcrp.h"


const string sType = "CpCrp";

const string& CpCrp::Type()
{
    return sType;
}

string CpCrp::EType()
{
    return "Elem:Vert:ConnPointBase:ConnPoint";
}

CpCrp::CpCrp(Elem* aElem): VertCompRp(aElem)
{
}

CpCrp::~CpCrp()
{
    delete iHead;
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


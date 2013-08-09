#include <elem.h>
#include "provdef.h"
#include "mcrp.h"


DefCrpProv::DefCrpProv()
{
}

DefCrpProv::~DefCrpProv()
{
}

int DefCrpProv::GetConfidence(const Elem& aElem) const
{
    int res = 0;
    return res;
}

MCrp& DefCrpProv::CreateRp(const Elem& aElem)
{
    MCrp* res = NULL;
    return *res;
}

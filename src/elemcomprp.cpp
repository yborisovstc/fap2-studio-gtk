#include "elemcomprp.h"

ElemCompHead::ElemCompHead(const Elem& aElem): iElem(aElem)
{
}


ElemCompRp::ElemCompRp(Elem* aElem): iElem(aElem), iHead(NULL)
{
    // Add header
    iHead = new ElemCompHead(*iElem);
    add(iHead);
    iHead->Show();
}

ElemCompHead::~ElemCompRp()
{
    delete iHead;
}

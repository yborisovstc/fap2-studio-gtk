
#include <prop.h>
#include "common.h"
#include "propcrp.h"

const string sType = "PropCrp";
int PropCrp::iMaxWidthChars = 40;

const string& PropCrp::Type()
{
    return sType;
}

string PropCrp::EType()
{
    return Prop::PEType();
}

PropCrp::PropCrp(Elem* aElem): ElemCompRp(aElem)
{

    MProp* prop = iElem->GetObj(prop);
    assert(prop != NULL);
    /*
    Glib::RefPtr<Gtk::TextBuffer> buf = Gtk::TextBuffer::create();
    string cont;
    iElem->GetCont(cont);
    buf->set_text(prop->Value());
//    iContent.set_single_line_mode(false);
//    iContent.set_width_chars(iMaxWidthChars);
    iContent.set_line_wrap(true);
    iContent.set_text(prop->Value());
    add(iContent);
    iContent.show();
    */
}

PropCrp::~PropCrp()
{
}

void *PropCrp::DoGetObj(const string& aName)
{
    void* res = NULL;
    if (aName ==  Type()) {
	res = this;
    }
    return res;
}

Gtk::Widget& PropCrp::Widget()
{
    return *this;
}

MCrp::tSigButtonPressName PropCrp::SignalButtonPressName()
{
    return iSigButtonPressName;
}

MCrp::tSigUpdated PropCrp::SignalUpdated()
{
    return iSigUpdated;
}

bool PropCrp::IsActionSupported(Action aAction)
{
    return (aAction == EA_Remove || aAction == EA_Rename || aAction == EA_Edit_Content);
}

bool PropCrp::Dragging()
{
    return false;
}

void PropCrp::SetHighlighted(bool aSet)
{
    DoSetHighlighted(aSet);
}

void PropCrp::SetErroneous(bool aSet)
{
    DoSetErroneous(aSet);
}

Elem* PropCrp::Model()
{
    return iElem;
}

MCrp::tSigButtonPress PropCrp::SignalButtonPress()
{
    return iSigButtonPress;
}

void PropCrp::GetContentUri(GUri& aUri)
{
}

void PropCrp::SetLArea(int aArea)
{
    iLArea = aArea;
}

int PropCrp::GetLArea() const
{
    return iLArea;
}

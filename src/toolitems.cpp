#include "toolitems.h"


TiLabel::TiLabel(const Glib::ustring& aText)
{
    iLabel = new Gtk::Label(aText);
    add(*iLabel);
    iLabel->show();
}

TiLabel::~TiLabel()
{
    delete iLabel;
}


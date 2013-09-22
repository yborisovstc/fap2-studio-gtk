
#include <gtkmm.h>
#include "dlgbase.h"


ParEditDlg::ParEditDlg(const string& aTitle, const string& aData): Gtk::Dialog(aTitle)
{
    add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
    add_button(Gtk::Stock::OPEN, Gtk::RESPONSE_OK);
    Gtk::VBox* cont_area = get_vbox();
    iEntry = new Gtk::Entry();
    iEntry->set_text(aData);
    iEntry->show();
    cont_area->pack_start(*iEntry);
}

void ParEditDlg::GetData(string& aData)
{
}


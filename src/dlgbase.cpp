
#include <gtkmm.h>
#include "dlgbase.h"


ParEditDlg::ParEditDlg(const string& aTitle, const string& aData): Gtk::Dialog(aTitle)
{
    add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
    add_button(Gtk::Stock::OK, Gtk::RESPONSE_OK);
    Gtk::VBox* cont_area = get_vbox();
    iEntry = new Gtk::Entry();
    iEntry->set_text(aData);
    iEntry->show();
    cont_area->pack_start(*iEntry);
}

void ParEditDlg::GetData(string& aData)
{
    aData = iEntry->get_text();
}




TextEditDlg::TextEditDlg(const string& aTitle, const string& aData): Gtk::Dialog(aTitle)
{
    add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
    add_button(Gtk::Stock::OK, Gtk::RESPONSE_OK);
    Glib::RefPtr<Gtk::TextBuffer> buf = Gtk::TextBuffer::create();
    buf->set_text(aData);
    Gtk::VBox* cont_area = get_vbox();
    iTextView = new Gtk::TextView(buf);
    iTextView->show();
    cont_area->pack_start(*iTextView);
}

void TextEditDlg::GetData(string& aData)
{
    Glib::RefPtr<Gtk::TextBuffer> buf = iTextView->get_buffer();
    aData = buf->get_text();
}


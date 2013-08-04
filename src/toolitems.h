
#ifndef __FAP2STU_TOOLITEMS_H
#define __FAP2STU_TOOLITEMS_H

#include <gtkmm/toolitem.h>
#include <gtkmm/label.h>

class TiLabel: public Gtk::ToolItem
{
    public:
	TiLabel(const Glib::ustring& aText);
	virtual ~TiLabel();
	Gtk::Label& Label() { return *iLabel;};
    private:
	Gtk::Label* iLabel;
};

#endif


#ifndef __FAP2STU_GTK_MSDESENV_H
#define __FAP2STU_GTK_MSDESENV_H

#include <gtkmm/uimanager.h>

// Studio DES Environmet interface

class MSDesEnv
{
    public:
	virtual Glib::RefPtr<Gtk::UIManager> UiMgr() = 0;
};

#endif

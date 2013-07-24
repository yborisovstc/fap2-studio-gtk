
#ifndef __FAPSTU_GTK_MAINWND_H
#define __FAPSTU_GTK_MAINWND_H

#include <gtkmm/button.h>
#include <gtkmm/window.h>
#include <gtkmm/box.h>
#include <gtkmm/actiongroup.h>
#include <gtkmm/uimanager.h>

class MainWnd : public Gtk::Window
{
    public:
	MainWnd();
	virtual ~MainWnd();

    protected:
	// Signal handlers:
	void on_button_clicked();
	void on_action_open();
	// Member widgets:
	Gtk::Button iButton;
	Gtk::Button iButton2;

    private:
	Gtk::VBox iVboxMain;
	Glib::RefPtr<Gtk::ActionGroup> irActionGroup;
	Glib::RefPtr<Gtk::UIManager> irUiMgr;
};

#endif 

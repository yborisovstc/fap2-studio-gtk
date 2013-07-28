
#ifndef __FAPSTU_GTK_MAINWND_H
#define __FAPSTU_GTK_MAINWND_H

#include <gtkmm/button.h>
#include <gtkmm/window.h>
#include <gtkmm/box.h>
#include <gtkmm/actiongroup.h>
#include <gtkmm/uimanager.h>
#include <gtkmm/scrolledwindow.h>

class MainWnd : public Gtk::Window
{
    public:
	MainWnd();
	virtual ~MainWnd();
	Glib::RefPtr<Gtk::UIManager> UIManager() const;
	Gtk::Container& ClientWnd();
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
	// Client window for main view - normally hier detail view
	Gtk::ScrolledWindow* irMainClientWnd;
};

#endif 

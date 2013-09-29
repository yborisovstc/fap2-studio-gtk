
#ifndef __FAPSTU_GTK_MAINWND_H
#define __FAPSTU_GTK_MAINWND_H

#include <gtkmm/button.h>
#include <gtkmm/window.h>
#include <gtkmm/box.h>
#include <gtkmm/actiongroup.h>
#include <gtkmm/uimanager.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/paned.h>

class MainWnd : public Gtk::Window
{
    public:
	MainWnd();
	virtual ~MainWnd();
	Glib::RefPtr<Gtk::UIManager> UIManager() const;
	Gtk::Container& ClientWnd();
	void SetNaviPane(Gtk::Widget& aWidget);
    protected:
	// Signal handlers:
	void on_button_clicked();
	void on_action_open();
	void on_action_saveas();
	// Member widgets:
	Gtk::Button iButton;
	Gtk::Button iButton2;

    private:
	// Generic vertical layout: menu, toolbar, client
	Gtk::VBox iVboxMain;
	// Base horisontal layout: left for tree/list, right for detail 
	Gtk::HPaned iHPanG;
	Glib::RefPtr<Gtk::ActionGroup> irActionGroup;
	Glib::RefPtr<Gtk::UIManager> irUiMgr;
	// Client window for main view - normally hier detail view
	Gtk::ScrolledWindow* irMainClientWnd;
};

#endif 

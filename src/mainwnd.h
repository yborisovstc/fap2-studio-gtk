
#ifndef __FAPSTU_GTK_MAINWND_H
#define __FAPSTU_GTK_MAINWND_H

#include <gtkmm/button.h>
#include <gtkmm/window.h>
#include <gtkmm/box.h>
#include <gtkmm/actiongroup.h>
#include <gtkmm/uimanager.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/paned.h>
#include "logview.h"

using namespace std;

class MainWnd : public Gtk::Window
{
    public:
	MainWnd();
	virtual ~MainWnd();
	Glib::RefPtr<Gtk::UIManager> UIManager() const;
	Gtk::ScrolledWindow& ClientWnd();
	void SetNaviPane(Gtk::Widget& aWidget);
	void SetEnvLog(const string& aLogFileName);
    protected:
	// Signal handlers:
	void on_action_open();
	void on_action_saveas();
    private:
	// Generic vertical layout: menu, toolbar, client
	Gtk::VBox iVboxMain;
	// Base vertical layour: iHPanG top and logview bottom
	Gtk::VPaned iVPanG;
	// Base horisontal layout: left for tree/list, right for detail 
	Gtk::HPaned iHPanG;
	Glib::RefPtr<Gtk::ActionGroup> irActionGroup;
	Glib::RefPtr<Gtk::UIManager> irUiMgr;
	// Client window for main view - normally hier detail view
	Gtk::ScrolledWindow* irMainClientWnd;
	Gtk::ScrolledWindow* iLogWnd;
	LogView* iLogView;
};

#endif 

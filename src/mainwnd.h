
#ifndef __FAPSTU_GTK_MAINWND_H
#define __FAPSTU_GTK_MAINWND_H

#include <gtkmm/button.h>
#include <gtkmm/window.h>
#include <gtkmm/box.h>
#include <gtkmm/actiongroup.h>
#include <gtkmm/uimanager.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/paned.h>
#include <gtkmm/notebook.h>
#include "logview.h"

using namespace std;
using namespace Gtk;

class MainWnd : public Gtk::Window
{
    public:
	MainWnd();
	virtual ~MainWnd();
	Glib::RefPtr<Gtk::UIManager> UIManager() const;
	Container& VisWindow();
	Gtk::ScrolledWindow& ClientWnd();
	void SetNaviPane(Gtk::Widget& aWidget);
	void SetLogView(Gtk::Widget& aWidget);
	void SetEnvLog(const string& aLogFileName);
    protected:
	// Signal handlers:
	void on_action_new();
	void on_action_open();
	void on_action_save();
	void on_action_saveas();
	void on_action_compactas();
    private:
	// Generic vertical layout: menu, toolbar, client
	Gtk::VBox iVboxMain;
	// Base vertical layour: iHPanG top and logview bottom
	Gtk::VPaned iVPanG;
	// Base horisontal layout: left for tree/list, right for detail 
	Gtk::HPaned iHPanG;
	// Base services window
	Notebook iServ;
	// Model visualization window
	ScrolledWindow iVis;
	Glib::RefPtr<Gtk::ActionGroup> irActionGroup;
	Glib::RefPtr<Gtk::UIManager> irUiMgr;
	// Client window for main view - normally hier detail view
	Gtk::ScrolledWindow* irMainClientWnd;
	Gtk::ScrolledWindow* iLogWnd;
	LogView* iLogView;
};

#endif 

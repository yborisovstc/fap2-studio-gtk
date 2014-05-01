#include "mainwnd.h"
#include <iostream>
#include <gtkmm/stock.h>

Glib::ustring sUiInfo = 
"<ui>"
"  <menubar name='MenuBar'>"
"    <menu action='MenuFile'>"
"      <menuitem action='Open'/>"
"      <menuitem action='Save_as'/>"
"      <menuitem action='Compact_as'/>"
"    </menu>"
"  </menubar>"
"  <toolbar  name='ToolBar'>"
"    <toolitem action='Open'/>"
"    <toolitem action='Save_as'/>"
"  </toolbar>"
"</ui>";


MainWnd::MainWnd()
{
    set_border_width(0);

    irActionGroup = Gtk::ActionGroup::create("ActionGroup");
    irActionGroup->add(Gtk::Action::create("MenuFile", "_File"));
    irActionGroup->add(Gtk::Action::create("Open", Gtk::Stock::OPEN), sigc::mem_fun(*this, &MainWnd::on_action_open));
    irActionGroup->add(Gtk::Action::create("Save_as", Gtk::Stock::SAVE_AS), sigc::mem_fun(*this, &MainWnd::on_action_saveas));
    irActionGroup->add(Gtk::Action::create("Compact_as", "Compact&Save"), sigc::mem_fun(*this, &MainWnd::on_action_compactas));
    irUiMgr = Gtk::UIManager::create();
    irUiMgr->insert_action_group(irActionGroup);
    irUiMgr->add_ui_from_string(sUiInfo);
    Gtk::Widget* pMenuBar = irUiMgr->get_widget("/MenuBar");
    Gtk::Widget* pToolBar = irUiMgr->get_widget("/ToolBar");

    add(iVboxMain);
    iVboxMain.show();

    // Base vertical layout: menu
    iVboxMain.pack_start(*pMenuBar, Gtk::PACK_SHRINK);
    // Base vertical layout: toolbar
    iVboxMain.pack_start(*pToolBar, Gtk::PACK_SHRINK);

    // Create main view client window
    irMainClientWnd = new Gtk::ScrolledWindow();
    // Log view
    iLogWnd = new Gtk::ScrolledWindow();
    iLogView = new LogView();
    iLogWnd->add(*iLogView);
    iLogWnd->show();
    // Visualization
    iVis.show();
    // Services window
    iServ.show();
    iServ.append_page(*iLogWnd, "Log");
    iServ.append_page(iVis, "Visualization");

    iVPanG.show();
    // Base vertical layout: main horisontal layout
    iHPanG.show();
    iVboxMain.pack_start(iVPanG, Gtk::PACK_EXPAND_WIDGET);
//    iVboxMain.pack_start(*irMainClientWnd, Gtk::PACK_EXPAND_WIDGET);
    // Detail view pane
    iHPanG.pack2(*irMainClientWnd, Gtk::EXPAND);
    iVPanG.pack1(iHPanG, Gtk::EXPAND);
    iVPanG.pack2(iServ, Gtk::EXPAND);

    pMenuBar->show();
    pToolBar->show();
    irMainClientWnd->show();
    iLogView->show();
}

MainWnd::~MainWnd()
{
}

void MainWnd::on_action_open()
{
    std::cout << "Action Open triggered" << std::endl;
}

void MainWnd::on_action_saveas()
{
}

void MainWnd::on_action_compactas()
{
}

Glib::RefPtr<Gtk::UIManager> MainWnd::UIManager() const
{
    return irUiMgr;
}

Gtk::ScrolledWindow& MainWnd::ClientWnd()
{
    return *irMainClientWnd;
}

void MainWnd::SetNaviPane(Gtk::Widget& aWidget)
{
    iHPanG.pack1(aWidget, Gtk::SHRINK);
}

void MainWnd::SetEnvLog(const string& aLogFileName)
{
    iLogView->SetLogFileName(aLogFileName);
}

Container& MainWnd::VisWindow()
{
    return iVis;
}


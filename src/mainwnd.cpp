#include "mainwnd.h"
#include <iostream>
#include <gtkmm/stock.h>

Glib::ustring sUiInfo = 
"<ui>"
"  <menubar name='MenuBar'>"
"    <menu action='MenuFile'>"
"      <menuitem action='Open'/>"
"    </menu>"
"  </menubar>"
"  <toolbar  name='ToolBar'>"
"    <toolitem action='Open'/>"
"  </toolbar>"
"</ui>";

MainWnd::MainWnd() : iButton("Hello World"), iButton2("Test")
{
    set_border_width(0);

    irActionGroup = Gtk::ActionGroup::create("ActionGroup");
    irActionGroup->add(Gtk::Action::create("MenuFile", "_File"));
    irActionGroup->add(Gtk::Action::create("Open", Gtk::Stock::OPEN), sigc::mem_fun(*this, &MainWnd::on_action_open));
    irUiMgr = Gtk::UIManager::create();
    irUiMgr->insert_action_group(irActionGroup);
    irUiMgr->add_ui_from_string(sUiInfo);
    Gtk::Widget* pMenuBar = irUiMgr->get_widget("/MenuBar");
    Gtk::Widget* pToolBar = irUiMgr->get_widget("/ToolBar");

    add(iVboxMain);
    iVboxMain.show();

    iButton.signal_clicked().connect(sigc::mem_fun(*this, &MainWnd::on_button_clicked));

    iVboxMain.pack_start(*pMenuBar, Gtk::PACK_SHRINK);
    iVboxMain.pack_start(*pToolBar, Gtk::PACK_SHRINK);

    // Create main view client window
    irMainClientWnd = new Gtk::ScrolledWindow();

    //iVboxMain.pack_start(iButton);
    iVboxMain.pack_start(*irMainClientWnd, Gtk::PACK_EXPAND_WIDGET);

    pMenuBar->show();
    pToolBar->show();
    iButton.show();
    irMainClientWnd->show();

}

MainWnd::~MainWnd()
{
}

void MainWnd::on_button_clicked()
{
    std::cout << "Hello World" << std::endl;
}

void MainWnd::on_action_open()
{
    std::cout << "Action Open triggered" << std::endl;
}

Glib::RefPtr<Gtk::UIManager> MainWnd::UIManager() const
{
    return irUiMgr;
}

Gtk::Container& MainWnd::ClientWnd()
{
    return *irMainClientWnd;
}


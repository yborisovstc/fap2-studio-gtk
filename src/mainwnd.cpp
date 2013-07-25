#include "mainwnd.h"
#include <iostream>

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
    irActionGroup->add(Gtk::Action::create("Open", "Open"), sigc::mem_fun(*this, &MainWnd::on_action_open));
    irUiMgr = Gtk::UIManager::create();
    irUiMgr->insert_action_group(irActionGroup);
    irUiMgr->add_ui_from_string(sUiInfo);
    Gtk::Widget* pMenuBar = irUiMgr->get_widget("/MenuBar");

    add(iVboxMain);
    iVboxMain.show();

    iButton.signal_clicked().connect(sigc::mem_fun(*this, &MainWnd::on_button_clicked));

    iVboxMain.pack_start(*pMenuBar, false, false, 0);

    iVboxMain.pack_start(iButton);

    pMenuBar->show();
    iButton.show();

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

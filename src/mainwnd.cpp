#include "msset.h"
#include "mainwnd.h"
#include <iostream>
#include <gtkmm/stock.h>
#include <gtkmm/toggleaction.h>

Glib::ustring sUiInfo = 
"<ui>"
"  <menubar name='MenuBar'>"
"    <menu action='MenuFile'>"
"      <menuitem action='New'/>"
"      <menuitem action='Open'/>"
"      <menuitem action='Save'/>"
"      <menuitem action='Save_as'/>"
"      <menuitem action='Reload'/>"
"      <menuitem action='Compact_as'/>"
"      <menuitem action='Optimize'/>"
"    </menu>"
"  </menubar>"
"  <toolbar  name='ToolBar'>"
"    <toolitem action='New'/>"
"    <toolitem action='Open'/>"
"    <toolitem action='Save'/>"
"    <toolitem action='Save_as'/>"
"    <toolitem action='Reload'/>"
"    <toolitem action='Undo'/>"
"    <toolitem action='Redo'/>"
"    <toolitem action='Repair'/>"
"    <toolitem action='Disable_opt'/>"
"  </toolbar>"
"</ui>";

const string KToolTip_New = "Create new model";
const string KToolTip_Open = "Open a model";
const string KToolTip_Save = "Save the model";
const string KToolTip_SaveAs = "Save as the model";
const string KToolTip_Reload = "Reload the model";
const string KToolTip_Compact = "Compact and save as the model";
const string KToolTip_Optimize = "Optimize and save the model";
const string KToolTip_Undo = "Undo last mutation";
const string KToolTip_Redo = "Redo last removed mutation";
const string KToolTip_Repair = "Repair the chromo";
const string KToolTip_DisableOpt = "Disable optimization processing";

MainWnd::MainWnd(MSEnv* aStEnv): iStEnv(aStEnv)
{
    set_border_width(0);

    irActionGroup = Gtk::ActionGroup::create("ActionGroup");
    irActionGroup->add(Gtk::Action::create("MenuFile", "_File"));
    irActionGroup->add(Gtk::Action::create("New", Gtk::Stock::NEW, "New", KToolTip_New));
    irActionGroup->add(Gtk::Action::create("Open", Gtk::Stock::OPEN, "Open", KToolTip_Open));
    irActionGroup->add(Gtk::Action::create("Save", Gtk::Stock::SAVE, "Save", KToolTip_Save));
    irActionGroup->add(Gtk::Action::create("Save_as", Gtk::Stock::SAVE_AS, "Save as", KToolTip_SaveAs));
    irActionGroup->add(Gtk::Action::create("Reload", Gtk::Stock::REFRESH, "Reload", KToolTip_Reload));
    irActionGroup->add(Gtk::Action::create("Compact_as", "Compact&Save", KToolTip_Compact));
    irActionGroup->add(Gtk::Action::create("Optimize", "Optimize", KToolTip_Optimize));
    irActionGroup->add(Gtk::Action::create("Undo", Gtk::Stock::UNDO, "Undo", KToolTip_Undo));
    irActionGroup->add(Gtk::Action::create("Redo", Gtk::Stock::REDO, "Redo", KToolTip_Redo));
    irActionGroup->add(Gtk::Action::create("Repair", Gtk::Stock::CONVERT, "Repair", KToolTip_Repair));
    MStSetting<bool>& disable_opt_s = iStEnv->Settings().GetSetting(MStSettings::ESts_DisableOpt, disable_opt_s);
    bool disable_opt = disable_opt_s.Get(disable_opt);
    irActionGroup->add(Gtk::ToggleAction::create("Disable_opt", Gtk::Stock::REMOVE, "DisableOpt", KToolTip_DisableOpt,
		disable_opt));
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
    //iLogView = new LogView();
    //iLogWnd->add(*iLogView);
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
    //iLogView->show();
}

MainWnd::~MainWnd()
{
}

Glib::RefPtr<Gtk::UIManager> MainWnd::UIManager() const
{
    return irUiMgr;
}

Glib::RefPtr<Gtk::ActionGroup> MainWnd::ActionGroup() const
{
    return irActionGroup;
}

Gtk::ScrolledWindow& MainWnd::ClientWnd()
{
    return *irMainClientWnd;
}

void MainWnd::SetNaviPane(Gtk::Widget& aWidget)
{
    iHPanG.pack1(aWidget, Gtk::SHRINK);
}

void MainWnd::SetLogView(Gtk::Widget& aWidget)
{
    iLogWnd->add(aWidget);
    aWidget.show();
}

void MainWnd::SetEnvLog(const string& aLogFileName)
{
    iLogView->SetLogFileName(aLogFileName);
}

Container& MainWnd::VisWindow()
{
    return iVis;
}

void MainWnd::ShowSpinner()
{
    mSpinner.show();
    mSpinner.start();
    remove();
    set_border_width(100);
    add(mSpinner);
    queue_draw();
}


void MainWnd::HideSpinner()
{
    remove();
    set_border_width(0);
    add(iVboxMain);
}

#include <iostream>
#include "hierdetailview.h"
#include "gtkmm/toolitem.h"
#include "gtkmm/toolbar.h"
#include "gtkmm/separatortoolitem.h"
#include <gtkmm/stock.h>

Glib::ustring sUiHierDview = 
"<ui>"
"  <toolbar  name='ToolBar'>"
"    <toolitem action='GoUp'/>"
"    <separator/>"
"    <placeholder name='NamePlaceholder'/>"
"    <separator/>"
"    <toolitem name='Name' action='Name'/>"
"  </toolbar>"
"</ui>";

HierDetailView::HierDetailView(MSEnv& aStEnv, Gtk::Container& aCont, const Glib::RefPtr<Gtk::UIManager>& aUiMgr): 
    iStEnv(aStEnv), iContWnd(aCont), iUiMgr(aUiMgr), iDetRp(NULL), iAlignent()
{
    // Addig toolbar
    irActionGroup = Gtk::ActionGroup::create("ElemDrpActGroup");
    irActionGroup->add(Gtk::Action::create("GoUp", Gtk::Stock::GOTO_TOP, "Go Up"), sigc::mem_fun(*this, &HierDetailView::on_action_up));
    iUiMgr->insert_action_group(irActionGroup);
    iUiMgr->add_ui_from_string(sUiHierDview);
    Gtk::ToolItem* pc = dynamic_cast<Gtk::ToolItem*>(iUiMgr->get_widget("/ToolBar/NamePlaceholder"));
    iTbNameHd = new TiLabel("Name: ");
    iTbNameHd->show();
    iTbName= new TiLabel("");
    iTbName->show();
    iTbParentHd = new TiLabel("Parent: ");
    iTbParentHd->show();
    iTbParent= new TiLabel("");
    iTbParent->show();
    Gtk::Toolbar* pToolBar = dynamic_cast<Gtk::Toolbar*>(iUiMgr->get_widget("/ToolBar"));
    int pos = pToolBar->get_item_index(*pc);
    pToolBar->insert(*iTbNameHd, pos++);
    pToolBar->insert(*iTbName, pos++);
    Gtk::SeparatorToolItem* sep = new Gtk::SeparatorToolItem();
    sep->show();
    pToolBar->insert(*sep, pos++);
    pToolBar->insert(*iTbParentHd, pos++);
    pToolBar->insert(*iTbParent, pos++);
    
    // Adding alignment
    iAlignent = new Gtk::Alignment(1.0, 1.0, 1.0, 1.0);
    iContWnd.add(*iAlignent);
    iAlignent->show();
}

HierDetailView::~HierDetailView()
{
    delete iDetRp;
    delete iAlignent;
}

void HierDetailView::SetRoot(Elem* aRoot)
{
    assert(aRoot != NULL);
    iRoot = aRoot;
}

void HierDetailView::SetCursor(Elem* aElem)
{
    assert(aElem != NULL);
    if (iDetRp != NULL) {
	iAlignent->remove();
	delete iDetRp;
	iDetRp = NULL;
    }
    MDrpProvider& prov = iStEnv.DrpProvider();
    iDetRp = prov.CreateRp(*aElem);
    iDetRp->SignalCompSelected().connect(sigc::mem_fun(*this, &HierDetailView::on_comp_selected));
    iAlignent->add(iDetRp->Widget());
    iDetRp->Widget().show();
    // Settng name and parent to the toolbar
    iTbName->Label().set_text(aElem->Name());
    iTbParent->Label().set_text(aElem->EType());
}

void HierDetailView::on_action_up()
{
    Elem* cursor = iDetRp->Model();
    if (cursor != iRoot) {
	SetCursor(cursor->GetMan());
    }
}

void HierDetailView::on_comp_selected(Elem* aComp)
{
    std::cout << "on_comp_selected, comp [" << aComp->Name() << "]" << std::endl;
    SetCursor(aComp);
}

#include "hierdetailview.h"
#include "gtkmm/toolitem.h"
#include "gtkmm/toolbar.h"
#include "gtkmm/separatortoolitem.h"

Glib::ustring sUiHierDview = 
"<ui>"
"  <toolbar  name='ToolBar'>"
"    <separator/>"
"    <placeholder name='NamePlaceholder'/>"
"    <separator/>"
"    <toolitem name='Name' action='Name'/>"
"  </toolbar>"
"</ui>";

HierDetailView::HierDetailView(Gtk::Container& aCont, const Glib::RefPtr<Gtk::UIManager>& aUiMgr): 
    iContWnd(aCont), iUiMgr(aUiMgr), iDetRp(NULL), iAlignent()
{
    // Addig toolbar
    irActionGroup = Gtk::ActionGroup::create("ElemDrpActGroup");
    irActionGroup->add(Gtk::Action::create("Name", "Lable Element Name", "Element Name"), sigc::mem_fun(*this, &HierDetailView::on_action_name));
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

void HierDetailView::SetFocuse(Elem* aElem)
{
    assert(aElem != NULL);
    if (iDetRp != NULL) {
	delete iDetRp;
	iDetRp = NULL;
    }
    iDetRp = new ElemDetRp(aElem);
    iAlignent->add(*iDetRp);
    iDetRp->show();
    // Settng name and parent to the toolbar
    iTbName->Label().set_text(aElem->Name());
    iTbParent->Label().set_text(aElem->EType());
    
}

void HierDetailView::on_action_name()
{
}

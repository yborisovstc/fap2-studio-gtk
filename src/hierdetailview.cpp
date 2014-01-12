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
"    <toolitem action='Undo'/>"
"    <toolitem action='Redo'/>"
"    <separator/>"
"    <placeholder name='NamePlaceholder'/>"
"    <separator/>"
"  </toolbar>"
"</ui>";


HierDetailView::HierDetailView(MSEnv& aStEnv, Gtk::ScrolledWindow& aCont, const Glib::RefPtr<Gtk::UIManager>& aUiMgr): 
    iStEnv(aStEnv), iContWnd(aCont), iUiMgr(aUiMgr), iDetRp(NULL), iAlignent(NULL)
{
    // Addig toolbar
    irActionGroup = Gtk::ActionGroup::create("ElemDrpActGroup");
    irActionGroup->add(Gtk::Action::create("GoUp", Gtk::Stock::GOTO_TOP, "Go Up"), sigc::mem_fun(*this, &HierDetailView::on_action_up));
    irActionGroup->add(Gtk::Action::create("Undo", Gtk::Stock::UNDO, "Undo"), sigc::mem_fun(*this, &HierDetailView::on_action_undo));
    irActionGroup->add(Gtk::Action::create("Redo", Gtk::Stock::REDO, "Redo"), sigc::mem_fun(*this, &HierDetailView::on_action_redo));
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
    // TODO [YB] To consider if we need the alignment here
    iAlignent = new Gtk::Alignment(0.0, 0.0, 1.0, 1.0);
    iContWnd.add(*iAlignent);
    iAlignent->show();

    iContWnd.signal_size_allocate().connect(sigc::mem_fun(*this, &HierDetailView::on_cont_size_alocated));
}

void HierDetailView::on_cont_size_alocated(Allocation& alloc) {
    if (iDetRp != NULL)  {
	Requisition rreq = iDetRp->Widget().size_request();
    }
}

void HierDetailView::on_drp_drag_motion(Gtk::Widget& widget, int x, int y)
{
    Widget& drpw = iDetRp->Widget();
    Allocation ralc = drpw.get_allocation();
    Allocation salc = iContWnd.get_allocation();
    Gtk::Adjustment* vadj = iContWnd.get_vadjustment();
    //std::cout << "HierDetailView: on_drp_dragging_over_border, y: " << y << ", va value: " << vadj->get_value() <<  ", va pg: " << vadj->get_page_size() << std::endl;
    if (y > (vadj->get_value() + vadj->get_page_size() - 1)) {
	double val = vadj->get_value();
	std::cout << "ADJUSTING" << std::endl;
	vadj->set_value(val + vadj->get_step_increment());
    }
    else if (y < (vadj->get_value() + 1)) {
	double val = vadj->get_value();
	std::cout << "ADJUSTING" << std::endl;
	vadj->set_value(val - vadj->get_step_increment());
    }
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

string HierDetailView::GetCursor() const
{
    return iCursor;
}

void HierDetailView::SetCursor(const string& aUri)
{
    Elem* node = iRoot->GetNode(aUri);
    assert(node != NULL);
    SetCursor(node);
}

void HierDetailView::SetCursor(Elem* aElem)
{
    assert(aElem != NULL);
    if (iDetRp != NULL) {
	iAlignent->remove();
	//iContWnd.remove(iDetRp->Widget());
	delete iDetRp;
	iDetRp = NULL;
    }
    MDrpProvider& prov = iStEnv.DrpProvider();
    iCursor = aElem->GetUri();
    iDetRp = prov.CreateRp(*aElem);
    iDetRp->SignalCompSelected().connect(sigc::mem_fun(*this, &HierDetailView::on_comp_selected));
    iDetRp->SignalDragMotion().connect(sigc::mem_fun(*this, &HierDetailView::on_drp_drag_motion));
    iAlignent->add(iDetRp->Widget());
    //iContWnd.add(iDetRp->Widget());
    iDetRp->Widget().show();
    Allocation calc = iContWnd.get_allocation();
    Allocation ralc = iDetRp->Widget().get_allocation();
    Requisition rreq = iDetRp->Widget().size_request();
    //std::cout << "HierDetailView, c_w: " << calc.get_width() << ", c_h: " << calc.get_height() << ", r_w: " << ralc.get_width() << ", r_h: " << ralc.get_height() << std::endl;
    std::cout << "HierDetailView, c_w: " << calc.get_width() << ", c_h: " << calc.get_height() << ", r_w: " << rreq.width << ", r_h: " << rreq.height << std::endl;
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

void HierDetailView::on_action_undo()
{
    Gtk::Adjustment* vadj = iContWnd.get_vadjustment();
    double val = vadj->get_value();
    vadj->set_value(val + vadj->get_step_increment());
}

void HierDetailView::on_action_redo()
{
}

void HierDetailView::on_comp_selected(Elem* aComp)
{
    std::cout << "on_comp_selected, comp [" << aComp->Name() << "]" << std::endl;
    SetCursor(aComp);
}

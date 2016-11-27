#include <iostream>
#include "hierdetailview.h"
#include "gtkmm/toolitem.h"
#include "gtkmm/toolbar.h"
#include "gtkmm/separatortoolitem.h"
#include <gtkmm/stock.h>
#include <gtkmm/toggleaction.h>
#include "msset.h"

Glib::ustring sUiHierDview = 
"<ui>"
"  <toolbar  name='ToolBar'>"
"    <toolitem action='Insert'/>"
"    <toolitem action='GoUp'/>"
"    <toolitem action='Back'/>"
"    <toolitem action='Forward'/>"
"    <separator/>"
"    <placeholder name='NamePlaceholder'/>"
"    <separator/>"
"    <placeholder name='ParentPlaceholder' />"
"    <toolitem action='GoToParent'/>"
"    <placeholder name='MutModePlaceholder' />"
"    <toolitem action='SpecifyMutNode'/>"
"    <toolitem action='Attention'/>"
"    <separator/>"
"  </toolbar>"
"</ui>";

Glib::ustring sUiPinMutBtn = 
"<ui>"
"  <toolbar  name='ToolBar'>"
"    <placeholder name='MutModePlaceholder'>"
"    <toolitem action='PinMutNode'/>"
"    </placeholder>"
"  </toolbar>"
"</ui>";

// Note: To change tooltip bg/fg color just edit gtk color scheme: /usr/share/themes/Ambiance/gtk-2.0/gtkrc

const string KToolTip_Insert = "Insert external chromo";
const string KToolTip_GoUp = "Go to owner";
const string KToolTip_GoBack = "Navigate history back";
const string KToolTip_GoForward = "Navigate history forward";
const string KToolTip_GoParent = "Navigate to parent";
const string KToolTip_EnablePhenoModif = "Enable phenotypic modifications";
const string KToolTip_PinMutNode = "Pin node for mutation";
const string KToolTip_PinMutNodeA = "Pinned node for mutation: ";
const string KToolTip_PinMutNode_Deatt = "Pin node for mutation - Disabled because the current node is deattached";
const string KToolTip_Att = "Attention indicator";

HierDetailView::HierDetailView(MSEnv& aStEnv, Gtk::ScrolledWindow& aCont, const Glib::RefPtr<Gtk::UIManager>& aUiMgr): 
    iStEnv(aStEnv), iContWnd(aCont), iUiMgr(aUiMgr), iDetRp(NULL), iAlignent(NULL)
{
    // Addig toolbar
    irActionGroup = Gtk::ActionGroup::create("ElemDrpActGroup");
    irActionGroup->add(Gtk::Action::create("Insert", Gtk::Stock::ADD, "Insert", KToolTip_Insert),
	    sigc::mem_fun(*this, &HierDetailView::on_action_insert));
    irActionGroup->add(Gtk::Action::create("GoUp", Gtk::Stock::GOTO_TOP, "Go Up", KToolTip_GoUp),
	    sigc::mem_fun(*this, &HierDetailView::on_action_up));
    irActionGroup->add(Gtk::Action::create("Back", Stock::GO_BACK, "Back", KToolTip_GoBack),
	    sigc::mem_fun(*this, &HierDetailView::on_action_goback));
    irActionGroup->add(Gtk::Action::create("Forward", Stock::GO_FORWARD, "Forward", KToolTip_GoForward),
	    sigc::mem_fun(*this, &HierDetailView::on_action_goforward));
    irActionGroup->add(Gtk::Action::create("GoToParent", Stock::GO_FORWARD, "Parent", KToolTip_GoParent), 
	    sigc::mem_fun(*this, &HierDetailView::on_action_goparent));
    irActionGroup->add(Gtk::Action::create("Attention", Stock::DIALOG_WARNING, "Attention"), 
	    sigc::mem_fun(*this, &HierDetailView::on_action_attention));
    // Use mut node
    MStSetting<bool>& ena_pheno_s = iStEnv.Settings().GetSetting(MStSettings::ESts_EnablePhenoModif, ena_pheno_s);
    bool ena_pheno = ena_pheno_s.Get(ena_pheno);
    irActionGroup->add(Gtk::ToggleAction::create("SpecifyMutNode", Stock::JUMP_TO, "Enable pheno modifications",
		KToolTip_EnablePhenoModif, ena_pheno), sigc::mem_fun(*this, &HierDetailView::on_action_spec_mut_node));
    iUiMgr->add_ui_from_string(sUiHierDview);

    if (ena_pheno) {
	// Pin mut node
	iUiMgr->add_ui_from_string(sUiPinMutBtn);
	MStSetting<Glib::ustring>& pinned_mn_s  = iStEnv.Settings().GetSetting(MStSettings::ESts_PinnedMutNode, pinned_mn_s);
	const Glib::ustring& pinned_mn = pinned_mn_s.Get(pinned_mn);
	irActionGroup->add(Gtk::ToggleAction::create("PinMutNode", Stock::INDEX, "PinMutNode", KToolTip_PinMutNode, !pinned_mn.empty()), 
		sigc::mem_fun(*this, &HierDetailView::on_action_pin_mut_node));
    }

    iUiMgr->insert_action_group(irActionGroup);

    Gtk::ToolItem* att = dynamic_cast<Gtk::ToolItem*>(iUiMgr->get_widget("/ToolBar/Attention"));
    att->set_sensitive(false);

    Gtk::ToolItem* pc = dynamic_cast<Gtk::ToolItem*>(iUiMgr->get_widget("/ToolBar/NamePlaceholder"));
    iTbNameHd = new TiLabel("Name: ");
    iTbNameHd->show();
    iTbName= new TiLabel("");
    iTbName->show();
    iTbParentHd = new TiLabel("Parent: ");
    iTbParentHd->show();
    iTbParent= new TiLabel("");
    iTbParent->show();
    iTbParentHd->set_can_focus(true);
    iTbParentHd->signal_key_press_event().connect(sigc::mem_fun(*this, &HierDetailView::on_parent_press_event));
    Gtk::Toolbar* pToolBar = dynamic_cast<Gtk::Toolbar*>(iUiMgr->get_widget("/ToolBar"));
    int pos = pToolBar->get_item_index(*pc);
    pToolBar->insert(*iTbNameHd, pos++);
    pToolBar->insert(*iTbName, pos++);
    Gtk::SeparatorToolItem* sep = new Gtk::SeparatorToolItem();
    sep->show();
    pToolBar->insert(*sep, pos++);
    pc = dynamic_cast<Gtk::ToolItem*>(iUiMgr->get_widget("/ToolBar/ParentPlaceholder"));
    pos = pToolBar->get_item_index(*pc);
    pToolBar->insert(*iTbParentHd, pos++);
    pToolBar->insert(*iTbParent, pos++);
    pc = dynamic_cast<Gtk::ToolItem*>(iUiMgr->get_widget("/ToolBar/GoToParent"));
    pos = pToolBar->get_item_index(*pc);
    pos++;
    sep = new Gtk::SeparatorToolItem();
    sep->show();
    pToolBar->insert(*sep, pos++);
    
    // Adding alignment
    // TODO [YB] To consider if we need the alignment here
    iAlignent = new Gtk::Alignment(0.0, 0.0, 1.0, 1.0);
    iContWnd.add(*iAlignent);
    iAlignent->show();

    iContWnd.signal_size_allocate().connect(sigc::mem_fun(*this, &HierDetailView::on_cont_size_alocated));
    iNavHist.clear(); 
    iNavHist.reserve(1);
    iNavHistIter = iNavHist.end();
}

void HierDetailView::UpdateBtnUp()
{
    Gtk::ToolItem* item = dynamic_cast<Gtk::ToolItem*>(iUiMgr->get_widget("/ToolBar/GoUp"));
    MElem* cursor = iRoot->GetNode(iCursor);
    item->set_sensitive(cursor != NULL && cursor->GetMan() != NULL);
}

void HierDetailView::UpdateBtnGoParent()
{
    Gtk::ToolItem* item = dynamic_cast<Gtk::ToolItem*>(iUiMgr->get_widget("/ToolBar/GoToParent"));
    MElem* cursor = iDetRp == NULL ? NULL : iDetRp->Model();
    MElem* parent = cursor->GetParent();
    //MElem* cursor = iRoot->GetNode(iCursor);
    item->set_sensitive(cursor != NULL && parent != NULL);
}

void HierDetailView::on_action_insert()
{
    iDetRp->OnActionInsert();
}

void HierDetailView::on_action_pin_mut_node()
{
    MStSetting<Glib::ustring>& pinned_mn_s  = iStEnv.Settings().GetSetting(MStSettings::ESts_PinnedMutNode, pinned_mn_s);
    const Glib::ustring& pinned_mn = pinned_mn_s.Get(pinned_mn);
    Gtk::ToggleToolButton* button = dynamic_cast<Gtk::ToggleToolButton*>(iUiMgr->get_widget("/ToolBar/PinMutNode"));
    if (iDetRp != NULL) {
	MElem* cursor = iDetRp->Model();
	string uris = cursor->GetUri(NULL);
	pinned_mn_s.Set(button->get_active() ? uris: ""); 
	Gtk::ToolItem* item = dynamic_cast<Gtk::ToolItem*>(button);
	item->set_tooltip_text(KToolTip_PinMutNodeA + uris);
    }
}

void HierDetailView::on_action_spec_mut_node()
{
    MStSetting<bool>& ena_pheno_s = iStEnv.Settings().GetSetting(MStSettings::ESts_EnablePhenoModif, ena_pheno_s);
    Gtk::ToggleToolButton* button = dynamic_cast<Gtk::ToggleToolButton*>(iUiMgr->get_widget("/ToolBar/SpecifyMutNode"));
    ena_pheno_s.Set(button->get_active()); 
}

void HierDetailView::on_action_goparent()
{
    MElem* cursor = iDetRp->Model();
    MElem* parent = cursor->GetParent();
    if (parent != NULL) {
	SetCursor(cursor->GetParent());
    }
}

void HierDetailView::UpdatePinMutNode()
{
    Gtk::ToolItem* item = dynamic_cast<Gtk::ToolItem*>(iUiMgr->get_widget("/ToolBar/PinMutNode"));
    if (item != NULL) {
	Gtk::ToggleToolButton* button = dynamic_cast<Gtk::ToggleToolButton*>(item);
	if (!button->get_active() && iDetRp != NULL) {
	    MElem* cursor = iDetRp->Model();
	    if (cursor->IsChromoAttached()) {
		item->set_sensitive(true);
		item->set_tooltip_text(KToolTip_PinMutNode);
	    }
	    else {
		item->set_sensitive(false);
		item->set_tooltip_text(KToolTip_PinMutNode_Deatt);
	    }
	}
    }
}

bool HierDetailView::on_parent_press_event(GdkEventKey* aEvent)
{
    MElem* cursor = iDetRp->Model();
    if (cursor->GetParent() != NULL) {
	SetCursor(cursor->GetParent());
    }
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

void HierDetailView::SetRoot(MElem* aRoot)
{
    assert(aRoot != NULL);
    iRoot = aRoot;
    // Reset history
    iNavHist.clear(); 
    iNavHist.reserve(1);
    iNavHistIter = iNavHist.end();
    ResetAttention();
}

string HierDetailView::GetCursor() const
{
    return iCursor;
}

void HierDetailView::SetCursor(const string& aUri)
{
    MElem* node = iRoot->GetNode(aUri);
    if (node == NULL) {
	node = iRoot;
    }
    SetCursor(node);
}

void HierDetailView::UpdataHistNavUI()
{
    Gtk::ToolItem* back = dynamic_cast<Gtk::ToolItem*>(iUiMgr->get_widget("/ToolBar/Back"));
    Gtk::ToolItem* frwd = dynamic_cast<Gtk::ToolItem*>(iUiMgr->get_widget("/ToolBar/Forward"));
    back->set_sensitive(iNavHistIter != iNavHist.begin());
    frwd->set_sensitive(iNavHistIter != (iNavHist.end() - 1));
}

void HierDetailView::Reset()
{
    if (iDetRp != NULL) {
	iAlignent->remove();
	delete iDetRp;
	iDetRp = NULL;
    }
    iRoot = NULL;
    iCursor.clear();
}

void HierDetailView::SetCursor(MElem* aElem, bool FromHist)
{
    assert(aElem != NULL);
    // Adding to nav history
    if (!FromHist) {
	if (iNavHistIter != iNavHist.end()) {
	    iNavHist.erase(iNavHistIter + 1, iNavHist.end());
	}
	iNavHist.push_back(aElem);
	iNavHistIter = iNavHist.end() - 1;
    }
    UpdataHistNavUI();
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
    iDetRp->SignalCompActivated().connect(sigc::mem_fun(*this, &HierDetailView::on_comp_activated));
    iDetRp->SignalDragMotion().connect(sigc::mem_fun(*this, &HierDetailView::on_drp_drag_motion));
    iDetRp->SignalAttention().connect(sigc::mem_fun(*this, &HierDetailView::on_drp_attention));
    iDetRp->SignalReloadRequired().connect(sigc::mem_fun(*this, &HierDetailView::on_drp_reload_required));
    iAlignent->add(iDetRp->Widget());
    //iContWnd.add(iDetRp->Widget());
    iDetRp->Widget().show();
    Allocation calc = iContWnd.get_allocation();
    Allocation ralc = iDetRp->Widget().get_allocation();
    Requisition rreq = iDetRp->Widget().size_request();
    //std::cout << "HierDetailView, c_w: " << calc.get_width() << ", c_h: " << calc.get_height() << ", r_w: " << 
    //ralc.get_width() << ", r_h: " << ralc.get_height() << std::endl;
    //std::cout << "HierDetailView, c_w: " << calc.get_width() << ", c_h: " << calc.get_height() << ", r_w: " << rreq.width 
    //<< ", r_h: " << rreq.height << std::endl;
    // Settng name and parent to the toolbar
    iTbName->Label().set_text(aElem->Name());
    iTbParent->Label().set_text(aElem->EType());
    UpdatePinMutNode();
    UpdateBtnUp();
    UpdateBtnGoParent();
}

void HierDetailView::on_drp_reload_required()
{
    mSigRecreateRequested.emit();
}

void HierDetailView::on_drp_attention(const string& aInfo)
{
    mAttention = aInfo;
    Gtk::ToolItem* att = dynamic_cast<Gtk::ToolItem*>(iUiMgr->get_widget("/ToolBar/Attention"));
    att->set_sensitive(!aInfo.empty());
    att->set_tooltip_text(aInfo);
}

void HierDetailView::ResetAttention()
{
    Gtk::ToolItem* att = dynamic_cast<Gtk::ToolItem*>(iUiMgr->get_widget("/ToolBar/Attention"));
    att->set_sensitive(false);
    att->set_tooltip_text(KToolTip_Att);
}

void HierDetailView::on_action_attention()
{
    MessageDialog* dlg = new MessageDialog(mAttention, false, MESSAGE_INFO, BUTTONS_OK, true);
    dlg->run();
    delete dlg;
    ResetAttention();
}

void HierDetailView::on_action_up()
{
    MElem* cursor = iDetRp->Model();
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

void HierDetailView::on_comp_selected(MElem* aComp)
{
    std::cout << "HierDetailView::on_comp_selected, comp [" << aComp->Name() << "]" << std::endl;
    mSigCompSelected.emit(aComp);
}

void HierDetailView::on_comp_activated(MElem* aComp)
{
    std::cout << "on_comp_activated, comp [" << aComp->Name() << "]" << std::endl;
    SetCursor(aComp);
}

void HierDetailView::on_action_goback()
{
    MElem* cursor = iDetRp->Model();
    if (iNavHistIter != iNavHist.begin()) {
	MElem* comp = (MElem*) *(--iNavHistIter);
	SetCursor(comp, true);
    }
}

void HierDetailView::on_action_goforward()
{
    MElem* cursor = iDetRp->Model();
    if (iNavHistIter != iNavHist.end() - 1) {
	MElem* comp = (MElem*) *(++iNavHistIter);
	SetCursor(comp, true);
    }

}

void HierDetailView::on_logrec_activated(const string& aNodeUri, int aMutId)
{
    // Check if node exists, if so move to it
    MElem* node = iRoot->GetNode(aNodeUri);
    if (node != NULL) {
	SetCursor(aNodeUri);
    }
}

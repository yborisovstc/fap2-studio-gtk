
#include <gtkmm/uimanager.h>
#include <gtkmm/actiongroup.h>
#include <gtkmm/stock.h>
#include <iostream>
#include <mdes.h>

#include "dessync.h"

using namespace Gtk;

Glib::ustring sUi = 
"<ui>"
"  <toolbar  name='ToolBar'>"
"    <toolitem action='Run'/>"
"    <toolitem action='Stop'/>"
"    <toolitem action='Pause'/>"
"    <toolitem action='Next'/>"
"  </toolbar>"
"</ui>";


string ADesSync::PEType()
{
    return Elem::PEType() + GUri::KParentSep + Type();
}

ADesSync::ADesSync(const string& aName, Elem* aMan, MEnv* aEnv, MSDesEnv* aSDesEnv): Elem(aName, aMan, aEnv), iSDesEnv(aSDesEnv)
{
    SetEType(Type(), Elem::PEType());
    SetParent(Type());
    // Addig toolbar
    Glib::RefPtr<Gtk::ActionGroup> actionGroup = ActionGroup::create("DesSyncActGroup");
    actionGroup->add(Gtk::Action::create("Run", Stock::MEDIA_PLAY, "Run"), sigc::mem_fun(*this, &ADesSync::on_action_run));
    actionGroup->add(Gtk::Action::create("Stop", Stock::MEDIA_STOP, "Stop"), sigc::mem_fun(*this, &ADesSync::on_action_stop));
    actionGroup->add(Gtk::Action::create("Pause", Stock::MEDIA_PAUSE, "Pause"), sigc::mem_fun(*this, &ADesSync::on_action_pause));
    actionGroup->add(Gtk::Action::create("Next", Stock::MEDIA_NEXT, "Next"), sigc::mem_fun(*this, &ADesSync::on_action_next));
    iSDesEnv->UiMgr()->insert_action_group(actionGroup);
    UIManager::ui_merge_id id = iSDesEnv->UiMgr()->add_ui_from_string(sUi);
}

void *ADesSync::DoGetObj(const char *aName, TBool aIncUpHier, const RqContext* aCtx)
{
    void* res = NULL;
    if (strcmp(aName, Type()) == 0) {
	res = this;
    }
    else {
	res = Elem::DoGetObj(aName, aIncUpHier);
    }
    return res;
}

void ADesSync::on_action_run()
{
}

void ADesSync::on_action_stop()
{
}

void ADesSync::on_action_pause()
{
}

void ADesSync::on_action_next()
{
    DoStep();
}

void ADesSync::DoStep() {
    Elem* eout = GetNode("../../Elem:Capsule/ConnPoint:Out");
    MDesSyncable* out = eout->GetObj(out);
    if (eout != NULL) {
	if (out->IsActive()) {
	    out->Update();
	}
	if (out->IsUpdated()) {
	    out->Confirm();
	}
    }
}


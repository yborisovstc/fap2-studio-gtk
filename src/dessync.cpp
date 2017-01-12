
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

const gint KFapeTimeSlice = 50;

string ADesSync::PEType()
{
    return Elem::PEType() + GUri::KParentSep + Type();
}

ADesSync::ADesSync(const string& aName, MElem* aMan, MEnv* aEnv, MSDesEnv* aSDesEnv): 
    Elem(aName, aMan, aEnv), iSDesEnv(aSDesEnv), iCount(0), iRunning(false), iStopped(true)
{
    SetParent(Type());
    // Addig toolbar
    iActionGroup = ActionGroup::create("DesSyncActGroup");
    iActionGroup->add(Gtk::Action::create("Run", Stock::MEDIA_PLAY, "Run"), sigc::mem_fun(*this, &ADesSync::on_action_run));
    iActionGroup->add(Gtk::Action::create("Stop", Stock::MEDIA_STOP, "Stop"), sigc::mem_fun(*this, &ADesSync::on_action_stop));
    iActionGroup->add(Gtk::Action::create("Pause", Stock::MEDIA_PAUSE, "Pause"), sigc::mem_fun(*this, &ADesSync::on_action_pause));
    iActionGroup->add(Gtk::Action::create("Next", Stock::MEDIA_NEXT, "Next"), sigc::mem_fun(*this, &ADesSync::on_action_next));
    iSDesEnv->UiMgr()->insert_action_group(iActionGroup);
    iUiId = iSDesEnv->UiMgr()->add_ui_from_string(sUi);
}

ADesSync::ADesSync(MElem* aMan, MEnv* aEnv, MSDesEnv* aSDesEnv): 
    Elem(Type(), aMan, aEnv), iSDesEnv(aSDesEnv), iCount(0), iRunning(false), iStopped(true)
{
    SetParent(Elem::PEType());
    // Addig toolbar
    iActionGroup = ActionGroup::create("DesSyncActGroup");
    iActionGroup->add(Gtk::Action::create("Run", Stock::MEDIA_PLAY, "Run"), sigc::mem_fun(*this, &ADesSync::on_action_run));
    iActionGroup->add(Gtk::Action::create("Stop", Stock::MEDIA_STOP, "Stop"), sigc::mem_fun(*this, &ADesSync::on_action_stop));
    iActionGroup->add(Gtk::Action::create("Pause", Stock::MEDIA_PAUSE, "Pause"), sigc::mem_fun(*this, &ADesSync::on_action_pause));
    iActionGroup->add(Gtk::Action::create("Next", Stock::MEDIA_NEXT, "Next"), sigc::mem_fun(*this, &ADesSync::on_action_next));
    iSDesEnv->UiMgr()->insert_action_group(iActionGroup);
    iUiId = iSDesEnv->UiMgr()->add_ui_from_string(sUi);
}

ADesSync::~ADesSync()
{
    iSDesEnv->UiMgr()->remove_ui(iUiId);
    iSDesEnv->UiMgr()->remove_action_group(iActionGroup);
    iActionGroup.reset();
}

void *ADesSync::DoGetObj(const char *aName)
{
    void* res = NULL;
    if (strcmp(aName, Type()) == 0) {
	res = this;
    }
    else {
	res = Elem::DoGetObj(aName);
    }
    return res;
}

gboolean ADesSync::OnTimerEvent()
{
    if (iRunning) {
	DoStep();
    }
    return iRunning;
}

void ADesSync::on_action_run()
{
    if (!iRunning) {
	iTimer = Glib::TimeoutSource::create(KFapeTimeSlice);
	iTimer->connect(sigc::mem_fun(*this, &ADesSync::OnTimerEvent));
	iTimer->attach();
	iRunning = true;
	iStopped = false;
    }
}

void ADesSync::on_action_stop()
{
    if (!iStopped) {
	if (iRunning) {
	    iTimer->destroy();
	    iTimer.reset();
	    iRunning = false;
	}
	iSDesEnv->SigActionRecreate().emit();
	iStopped = true;
    }
}

void ADesSync::on_action_pause()
{
     if (iRunning) {
	iRunning = false;
	iTimer->destroy();
	iTimer.reset();
    }
}

void ADesSync::on_action_next()
{
    DoStep();
}

void ADesSync::DoStep() {
    MElem* eout = GetNode("./../../Capsule/Out");
    MDesSyncable* out = (MDesSyncable*) eout->GetSIfiC(MDesSyncable::Type(), this);
    if (eout != NULL) {
	Logger()->Write(EDbg, this, "Step %d", iCount++);
	try {
	    if (out->IsActive()) {
		out->Update();
	    }
	    if (out->IsUpdated()) {
		out->Confirm();
	    }
	} catch (const std::exception& e) {
	    Logger()->Write(EErr, this, "Unspecified error on update");
	}
    }
}


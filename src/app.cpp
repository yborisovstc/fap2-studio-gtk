#include "app.h"
#include <gtkmm.h>
#include "gtkmm/object.h"
#include <iostream>

const char* KLogFileName = "fap2-studio.log";
const char* KTmpFileName = ".fap2-studio-tmp.xml";
const char* KSpecFileName = "/usr/share/fap2-studio-gtk/templ/empty.xml";
const char* KTemplDirName = "/usr/share/fap2-studio-gtk/templ";
const char* KAppName = "fap2-studio";
const char* KRcFileName = "/usr/share/fap2-studio-gtk/conf/reg.rc";

const string KTitleUnsaved = "unsaved";
const string KMsgUnsaved = "The model has been modified. Do you want to save it?";
const string KBtnTxtDiscard = "Discard";

/* Time slice of FAP environment, in milliseconds */
const gint KFapeTimeSlice = 50;


const string sType = "DesObserver";

const string& DesObserver::Type()
{
    return sType;
}

DesObserver::DesObserver(): iDesEnv(NULL), iChanged(false), iLogRec(NULL)
{
}

DesObserver::~DesObserver()
{
    if (iLogRec != NULL) {
	iLogRec->RemoveLogObserver(this);
    }
}

void DesObserver::SetDes(MEnv* aDesEnv)
{
    assert(aDesEnv != NULL && iDesEnv == NULL || aDesEnv == NULL);
    if (iDesEnv != NULL) {
	iDesEnv->Logger()->RemoveLogObserver(this);
	Elem* root = iDesEnv->Root();
	root->SetObserver(NULL);
	iDesEnv = NULL;
    }
    else {
	iDesEnv = aDesEnv;
	iDesEnv->Logger()->AddLogObserver(this);
	Elem* root = iDesEnv->Root();
	if (root != NULL) {
	    root->SetObserver(this);
	}
    }
    iSigDesEnvChanged.emit();
}

// TODO [YB] This is required because on the first stage of des env creation, the root elem is not created yet, 
// but the logger only. So we need to have also two stage observer setup: logger first and then root observer.
// This is just workaround. To use more considered solution, e.g notif from env of root creation
void DesObserver::UpdateDesRootObserver()
{
    if (iDesEnv != NULL) {
	Elem* root = iDesEnv->Root();
	if (root != NULL) {
	    root->SetObserver(this);
	    //iSigSystemChanged.emit();
	}
    }
}

bool DesObserver::IsModelChanged() const
{
    return iChanged;
}

void DesObserver::SetModelChanged(bool aChanged)
{
    if (!iChanged && aChanged) {
	iSigSystemChanged.emit();
    }
    iChanged = aChanged;
}

void *DesObserver::DoGetObj(const string& aName)
{
    void* res = NULL;
    if (aName ==  Type()) {
	res = this;
    }
    else if (aName ==  MMdlObserver::Type()) {
	res = (MMdlObserver*) this;
    }
    else if (aName ==  MCompsObserver::Type()) {
	res = (MCompsObserver*) this;
    }
    return res;
}

MEnv* DesObserver::DesEnv()
{
    return iDesEnv;
}

MMdlObserver::tSigDesEnvChanged DesObserver::SignalDesEnvChanged()
{
    return iSigDesEnvChanged;
}

MMdlObserver::tSigCompDeleted DesObserver::SignalCompDeleted()
{
    return iSigCompDeleted;
}

MMdlObserver::tSigCompAdded DesObserver::SignalCompAdded()
{
    return iSigCompAdded;
}

MMdlObserver::tSigCompChanged DesObserver::SignalCompChanged()
{
    return iSigCompChanged;
}

MMdlObserver::tSigCompRenamed DesObserver::SignalCompRenamed()
{
    return iSigCompRenamed;
}

MMdlObserver::tSigContentChanged DesObserver::SignalContentChanged()
{
    return iSigContentChanged;
}

MMdlObserver::tSigSystemChanged DesObserver::SignalSystemChanged()
{
    return iSigSystemChanged;
}

MMdlObserver::tSigLogAdded DesObserver::SignalLogAdded()
{
    return iSigLogAdded;
}

void DesObserver::OnCompDeleting(Elem& aComp)
{
    iSigCompDeleted.emit(&aComp);
    SetModelChanged();
}

void DesObserver::OnCompAdding(Elem& aComp)
{
    iSigCompAdded.emit(&aComp);
    SetModelChanged();
}

void DesObserver::OnCompChanged(Elem& aComp)
{
    iSigCompChanged.emit(&aComp);
    SetModelChanged();
}

TBool DesObserver::OnCompRenamed(Elem& aComp, const string& aOldName)
{
    iSigCompRenamed.emit(&aComp, aOldName);
    SetModelChanged();
}

void DesObserver::OnContentChanged(Elem& aComp)
{
    iSigContentChanged.emit(&aComp);
}

void DesObserver::OnLogAdded(MLogRec::TLogRecCtg aCtg, Elem* aNode, const std::string& aContent)
{
    iSigLogAdded.emit(aCtg, aNode, aContent);
}

void DesObserver::OnLogRecDeleting(MLogRec* aLogRec)
{
    __ASSERT(aLogRec == iLogRec);
    aLogRec->RemoveLogObserver(this);
    iLogRec = NULL;
}




App::App(): iEnv(NULL), iMainWnd(NULL), iHDetView(NULL), iSaved(false), iChromoLim(0), iChanged(false)
{
    // Create model observer
    iDesObserver = new DesObserver();
    // Set log view
    iLogView = new LogViewL(iDesObserver);
    // Create studio environment
    iStEnv = new StEnv(iDesObserver, iLogView->GetDesLog());
    // Default logfilename
    iLogFileName = GetDefaultLogFileName();
    // Settings defaults
    MStSetting<bool>& ena_pheno = iStEnv->Settings().GetSetting(MStSettings::ESts_EnablePhenoModif, ena_pheno);
    ena_pheno.Set(true);
    // Create main window
    iMainWnd = new MainWnd();
    iMainWnd->maximize();
    iMainWnd->SetLogView(*iLogView);
    //iMainWnd->SetEnvLog(iLogFileName);
    //iMainWnd->UIManager()->signal_post_activate().connect(sigc::mem_fun(*this, &App::on_action));
    iMainWnd->UIManager()->get_action("ui/ToolBar/New")->signal_activate().connect(sigc::mem_fun(*this, &App::on_action_new));
    iMainWnd->UIManager()->get_action("ui/ToolBar/Open")->signal_activate().connect(sigc::mem_fun(*this, &App::on_action_open));
    iMainWnd->UIManager()->get_action("ui/ToolBar/Save")->signal_activate().connect(sigc::mem_fun(*this, &App::on_action_save));
    iMainWnd->UIManager()->get_action("ui/ToolBar/Save_as")->signal_activate().connect(sigc::mem_fun(*this, &App::on_action_saveas));
    iMainWnd->UIManager()->get_action("ui/ToolBar/Reload")->signal_activate().connect(sigc::mem_fun(*this, &App::on_action_recreate));
    iMainWnd->UIManager()->get_action("ui/MenuBar/MenuFile/Compact_as")->signal_activate().connect(sigc::mem_fun(*this, &App::on_action_compactas));
    iMainWnd->UIManager()->get_action("ui/ToolBar/Undo")->signal_activate().connect(sigc::mem_fun(*this, &App::on_action_undo));
    iMainWnd->UIManager()->get_action("ui/ToolBar/Redo")->signal_activate().connect(sigc::mem_fun(*this, &App::on_action_redo));
    // Create studio DES environment
    iStDesEnv = new StDesEnv(iMainWnd->UIManager(), iMainWnd->VisWindow());
    iStDesEnv->SigActionRecreate().connect(sigc::mem_fun(*this, &App::on_action_recreate));
    // Model specific nodes provider
    iMdlProv = new MdlProv("MdlProv", iStDesEnv, NULL);
    // Create model
    iHDetView = new HierDetailView(*iStEnv, iMainWnd->ClientWnd(), iMainWnd->UIManager());
    //OpenFile(KSpecFileName);
    // Navigation pane
    iNaviPane = new Navi(iDesObserver);
    //iNaviPane->SetDesEnv(iEnv);
    iNaviPane->show();
    iMainWnd->SetNaviPane(*iNaviPane);
    iNaviPane->NatHier().SignalCompSelected().connect(sigc::mem_fun(*iHDetView, &HierDetailView::on_comp_selected));
    // Parse resource file
    gtk_rc_parse(KRcFileName);
}

App::~App() {
    delete iMainWnd;
    delete iEnv;
    delete iStEnv;
    delete iStDesEnv;
    delete iMdlProv;
    delete iDesObserver;
}

void App::on_system_changed()
{
    // Set max order from the model
    iMaxOrder = iEnv->ChMgr()->GetMaxOrder() + 1;
    iChromoLim = 0;
    UpdataUndoRedo();
}

void App::on_action(const Glib::RefPtr<Gtk::Action>& aAction)
{
    std::cout << "Action" << std::endl;
}

void App::UpdataUndoRedo()
{
    Gtk::ToolItem* undo = dynamic_cast<Gtk::ToolItem*>(iMainWnd->UIManager()->get_widget("ui/ToolBar/Undo"));
    Gtk::ToolItem* redo = dynamic_cast<Gtk::ToolItem*>(iMainWnd->UIManager()->get_widget("ui/ToolBar/Redo"));
    undo->set_sensitive(iChromoLim < (iMaxOrder - iInitMaxOrder));
    redo->set_sensitive(iChromoLim > 0);
}

void App::on_action_undo()
{
    if (iChromoLim < iMaxOrder) {
	iChromoLim++;
	on_action_recreate();
    }
}

void App::on_action_redo()
{
    if (iChromoLim > 0) { 
	iChromoLim--;
	on_action_recreate();
    }
}

void App::on_action_new()
{
    if (CheckCurrentModelSaving()) {
	Gtk::FileChooserDialog dialog("Please choose a template for new system", Gtk::FILE_CHOOSER_ACTION_OPEN);
	dialog.set_transient_for(*iMainWnd);

	//Add response buttons the the dialog:
	dialog.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
	dialog.add_button(Gtk::Stock::OPEN, Gtk::RESPONSE_OK);
	int result = dialog.run();
	if (result == Gtk::RESPONSE_OK) {
	    std::string filename = dialog.get_filename();
	    iSpecFileName.clear();
	    iChromoLim = 0;
	    OpenFile(filename, true);
	    iInitMaxOrder = iEnv->ChMgr()->GetMaxOrder();
	    iSaved = EFalse;
	}
    }
}

void App::on_action_open()
{
    if (CheckCurrentModelSaving()) {
	Gtk::FileChooserDialog dialog("Please choose a file", Gtk::FILE_CHOOSER_ACTION_OPEN);
	dialog.set_transient_for(*iMainWnd);

	//Add response buttons the the dialog:
	dialog.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
	dialog.add_button(Gtk::Stock::OPEN, Gtk::RESPONSE_OK);
	int result = dialog.run();
	if (result == Gtk::RESPONSE_OK) {
	    string filename = dialog.get_filename();
	    iChromoLim = 0;
	    OpenFile(filename, false);
	    iInitMaxOrder = iEnv->ChMgr()->GetMaxOrder();
	    iSaved = EFalse;
	}
    }
}

void App::on_action_recreate()
{
    string cursor = iHDetView->GetCursor();
    if (iDesObserver->IsModelChanged()) {
	SaveTmp();
	OpenFile(GetDefaultTmpFileName(), true);
    }
    else {
	OpenFile(IsSystemChanged() ? GetDefaultTmpFileName() : iSpecFileName, true);
    }
    iHDetView->SetCursor(cursor);
}

void App::on_action_save()
{
    if (!iSpecFileName.empty()) {
	SaveFile(iSpecFileName);
	iSaved = ETrue;
    }
    else {
	on_action_saveas();
    }
}

void App::on_action_saveas()
{
    Gtk::FileChooserDialog dialog("Saving as - Please choose a file", Gtk::FILE_CHOOSER_ACTION_SAVE);
    dialog.set_transient_for(*iMainWnd);

    //Add response buttons the the dialog:
    dialog.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
    dialog.add_button(Gtk::Stock::SAVE_AS, Gtk::RESPONSE_OK);
    // Set filter
    Gtk::FileFilter filter;
    filter.set_name("DES model files");
    filter.add_mime_type("application/xml");
    dialog.add_filter(filter);

    int result = dialog.run();
    if (result == Gtk::RESPONSE_OK) {
	std::string filename = dialog.get_filename();
	SaveFile(filename);
	iSaved = ETrue;
	iSpecFileName = filename;
	iMainWnd->set_title(FormTitle(filename));
    }
}

void App::on_action_compactas()
{
    Gtk::FileChooserDialog dialog("Compacting chromo and save as - Please choose a file", Gtk::FILE_CHOOSER_ACTION_SAVE);
    dialog.set_transient_for(*iMainWnd);

    //Add response buttons the the dialog:
    dialog.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
    dialog.add_button(Gtk::Stock::SAVE_AS, Gtk::RESPONSE_OK);
    // Set filter
    Gtk::FileFilter filter;
    filter.set_name("DES model files");
    filter.add_mime_type("application/xml");
    dialog.add_filter(filter);

    int result = dialog.run();
    if (result == Gtk::RESPONSE_OK) {
	std::string filename = dialog.get_filename();
	CompactAndSaveFile(filename);
    }
}

string App::GetDefaultLogFileName() const
{
    const gchar* home = g_getenv("HOME");
    return string(home) + "/" + KLogFileName;
}

string App::GetDefaultTmpFileName() const
{
    const gchar* home = g_getenv("HOME");
    return string(home) + "/" + KTmpFileName;
}

bool App::CheckCurrentModelSaving()
{
    bool res = true;
    if (IsSystemChanged()) {
	Gtk::MessageDialog dialog(KMsgUnsaved, false, MESSAGE_INFO, BUTTONS_OK_CANCEL, true);
	dialog.add_button(KBtnTxtDiscard, Gtk::RESPONSE_REJECT);
	int result = dialog.run();
	if (result == Gtk::RESPONSE_OK) {
	    on_action_save();
	}
	else if (result == Gtk::RESPONSE_CANCEL) {
	    res = false;
	}
    }
    return res;
}

string App::FormTitle(const string& aFilePath)
{
    size_t pos = aFilePath.find_last_of("/");
    string filename = pos != string::npos ? aFilePath.substr(pos + 1) : aFilePath;
    return string(KAppName) + " [" + filename + "]";
}

bool App::IsSystemChanged() const
{
    // Taking into acoount short term and long term changes
    return iDesObserver->IsModelChanged() || iChanged;
}

void App::SaveTmp()
{
    SaveFile(GetDefaultTmpFileName());
}

void App::OpenFile(const string& aFileName, bool aAsTmp)
{
    if (iEnv != NULL) {
	iDesObserver->SetDes(NULL);
	delete iEnv;
	iEnv = NULL;
    }
    iEnv = new Env("DesEnv", aFileName, iLogFileName);
    iEnv->AddProvider(iMdlProv);
    iEnv->ChMgr()->SetLim(iChromoLim);
    iDesObserver->SetDes(iEnv);
    iEnv->ConstructSystem();
    iDesObserver->UpdateDesRootObserver();
    iHDetView->SetRoot(iEnv->Root());
    iHDetView->SetCursor(iEnv->Root());
    if (!aAsTmp) {
	iSpecFileName = aFileName;
	iMainWnd->set_title(FormTitle(aFileName));
    }
    else {
	iMainWnd->set_title(FormTitle(iSpecFileName.empty() ? KTitleUnsaved : iSpecFileName));
    }
    // Mark model as unchanged, initially
    iDesObserver->SetModelChanged(false);
    // Init max order from spec, to be able to do redo
    iMaxOrder = iEnv->ChMgr()->GetSpecMaxOrder();
    UpdataUndoRedo();
    // Start tracking the model changes in order to update undo/redo
    iDesObserver->SignalSystemChanged().connect(sigc::mem_fun(*this, &App::on_system_changed));
}

void App::SaveFile(const string& aFileName)
{
    iChanged = iChanged || iDesObserver->IsModelChanged();
    iEnv->Root()->Chromos().Save(aFileName);
}

void App::CompactAndSaveFile(const string& aFileName)
{
    iEnv->Root()->CompactChromo();
    iEnv->Root()->Chromos().Save(aFileName);
}


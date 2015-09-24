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

const string KModulesPath = "/usr/share/fap2-studio-gtk/modules/";

const string KTitleUnsaved = "unsaved";
const string KMsgUnsaved = "The model has been modified. Do you want to save it?";
const string KBtnTxtDiscard = "Discard";
const string KToolTip_Repair = "Repair";

const string KDlg_Opt1 = "The pass of optimization is done. Some not optimized mutations are remaining. Do the next pass?";
const string KDlg_Opt2 = "The optimization is completed successfully.";

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
    if (aDesEnv == NULL) {
	if (iDesEnv != NULL) {
	    iDesEnv->Logger()->RemoveLogObserver(this);
	    Elem* root = iDesEnv->Root();
	    root->SetObserver(NULL);
	    iDesEnv = NULL;
	}
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
	    iSigSystemCreated.emit();
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

MMdlObserver::tSigSystemCreated DesObserver::SignalSystemCreated()
{
    return iSigSystemCreated;
}

MMdlObserver::tSigLogAdded DesObserver::SignalLogAdded()
{
    return iSigLogAdded;
}

void DesObserver::OnCompDeleting(Elem& aComp, TBool aSoft)
{
    iSigCompDeleted.emit(&aComp);
    SetModelChanged();
}

void DesObserver::OnCompAdding(Elem& aComp)
{
    iSigCompAdded.emit(&aComp);
    SetModelChanged();
}

TBool DesObserver::OnCompChanged(Elem& aComp)
{
    iSigCompChanged.emit(&aComp);
    SetModelChanged();
    return true;
}

TBool DesObserver::OnCompRenamed(Elem& aComp, const string& aOldName)
{
    iSigCompRenamed.emit(&aComp, aOldName);
    SetModelChanged();
}

TBool DesObserver::OnContentChanged(Elem& aComp)
{
    iSigContentChanged.emit(&aComp);
    return true;
}

void DesObserver::OnLogAdded(long aTimestamp, MLogRec::TLogRecCtg aCtg, Elem* aNode, const std::string& aContent, int aMutId)
{
    iSigLogAdded.emit(aTimestamp, aCtg, aNode, aMutId,  aContent);
}

void DesObserver::OnLogRecDeleting(MLogRec* aLogRec)
{
    __ASSERT(aLogRec == iLogRec);
    aLogRec->RemoveLogObserver(this);
    iLogRec = NULL;
}

void DesObserver::AddObservable(MLogRec* aObservable)
{
    __ASSERT(iLogRec == NULL);
    iLogRec = aObservable;
}

void DesObserver::RemoveObservable(MLogRec* aObservable)
{
    __ASSERT(iLogRec == aObservable);
    iLogRec = NULL;
}



App::App(): iEnv(NULL), iMainWnd(NULL), iHDetView(NULL), iSaved(false), iChromoLim(0), iChanged(false), 
    iRepair(false), mWrkThread(NULL)
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
    ena_pheno.SigChanged().connect(sigc::mem_fun(*this, &App::on_setting_changed_pheno_enable));
    ena_pheno.Set(false);
    MStSetting<Glib::ustring>& pinned_mut_node = iStEnv->Settings().GetSetting(MStSettings::ESts_PinnedMutNode, pinned_mut_node);
    pinned_mut_node.Set("");
    MStSetting<Glib::ustring>& st_modules_path = iStEnv->Settings().GetSetting(MStSettings::ESts_ModulesPath, st_modules_path);
    st_modules_path.Set("/usr/share/fap2-studio-gtk/modules/");
    // Enable mutation even if broken critical deps are detected
    MStSetting<bool>& ena_mut_critdeps = iStEnv->Settings().GetSetting(MStSettings::ESts_EnableMutWithCritDep, ena_mut_critdeps);
    ena_mut_critdeps.Set(false);
    // Settings: enable process optimization
    MStSetting<bool>& disbl_opt = iStEnv->Settings().GetSetting(MStSettings::ESts_DisableOpt, disbl_opt);
    disbl_opt.SigChanged().connect(sigc::mem_fun(*this, &App::on_setting_changed_disable_opt));
    disbl_opt.Set(false);
    // Create main window
    iMainWnd = new MainWnd(iStEnv);
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
    iMainWnd->UIManager()->get_action("ui/MenuBar/MenuFile/Optimize")->signal_activate().connect(sigc::mem_fun(*this, &App::on_action_optimize));
    iMainWnd->UIManager()->get_action("ui/ToolBar/Undo")->signal_activate().connect(sigc::mem_fun(*this, &App::on_action_undo));
    iMainWnd->UIManager()->get_action("ui/ToolBar/Redo")->signal_activate().connect(sigc::mem_fun(*this, &App::on_action_redo));
    iMainWnd->UIManager()->get_action("ui/ToolBar/Repair")->signal_activate().connect(sigc::mem_fun(*this, &App::on_action_repair));
    iMainWnd->UIManager()->get_action("ui/ToolBar/Disable_opt")->signal_activate().connect(sigc::mem_fun(*this, 
		&App::on_action_disable_opt));
    Gtk::ToolItem* repair_ti = dynamic_cast<Gtk::ToolItem*>(iMainWnd->UIManager()->get_widget("ui/ToolBar/Repair"));
    repair_ti->set_tooltip_text(KToolTip_Repair);
    // Create studio DES environment
    iStDesEnv = new StDesEnv(iMainWnd->UIManager(), iMainWnd->VisWindow());
    iStDesEnv->SigActionRecreate().connect(sigc::mem_fun(*this, &App::on_action_recreate));
    // Model specific nodes provider
    iMdlProv = new MdlProv("MdlProv", iStDesEnv, NULL);
    // Create model
    iHDetView = new HierDetailView(*iStEnv, iMainWnd->ClientWnd(), iMainWnd->UIManager());
    iHDetView->SignalRecreateRequested().connect(sigc::mem_fun(*this, &App::on_action_recreate));
    iHDetView->SignalCompSelected().connect(sigc::mem_fun(*this, &App::on_comp_selected));
    iLogView->SignalLogRecActivated().connect(sigc::mem_fun(iHDetView, &HierDetailView::on_logrec_activated));
    //OpenFile(KSpecFileName);
    // Navigation pane
    iNaviPane = new Navi(*iStEnv, iDesObserver);
    //iNaviPane->SetDesEnv(iEnv);
    iNaviPane->show();
    iMainWnd->SetNaviPane(*iNaviPane);
    iNaviPane->NatHier().SignalCompSelected().connect(sigc::mem_fun(*iHDetView, &HierDetailView::on_comp_selected));
    iNaviPane->NatHier().SignalCompActivated().connect(sigc::mem_fun(*iHDetView, &HierDetailView::on_comp_activated));
    iLogView->SignalLogRecActivated().connect(sigc::mem_fun(iNaviPane->ChromoTreeView(), &ChromoTree::on_logrec_activated));
    // Parse resource file
    gtk_rc_parse(KRcFileName);
    // Update menu and toolbar
    InitialUpdate();
    // Connect handler of model created (model creation is doing in worker thread)
    mSigMdlCreated.connect(sigc::mem_fun(*this, &App::on_model_created));
}

App::~App() {
    // Deattach observer to avoid massive notifications from the model
    iDesObserver->SetDes(NULL);
    delete iEnv;
    delete iMainWnd;
    delete iStEnv;
    delete iStDesEnv;
    // iMdlProv is owned by iEnv, so no need to delete it
    delete iDesObserver;
    // It is an error if the thread is still running at this point.
    __ASSERT(mWrkThread == NULL);
}

void App::on_setting_changed_pheno_enable()
{
}

void App::on_setting_changed_disable_opt()
{
}

void App::on_comp_selected(Elem* aComp)
{
    iLogView->Select(aComp, MLogRec::EErr);
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

void App::InitialUpdate()
{
    // File/Compact
    // Isn't supported at the moment, dim
    Gtk::MenuItem* compact = dynamic_cast<Gtk::MenuItem*>(iMainWnd->UIManager()->
	    get_widget("ui/MenuBar/MenuFile/Compact_as"));
    compact->set_sensitive(false);
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

void App::on_action_repair()
{
    iRepair = true;
    on_action_recreate();
}

void App::on_action_new()
{
    if (CheckCurrentModelSaving()) {
	Gtk::FileChooserDialog dialog("Please choose a template for new system", Gtk::FILE_CHOOSER_ACTION_OPEN);
	dialog.set_current_folder(KTemplDirName);
	dialog.set_transient_for(*iMainWnd);

	//Add response buttons the the dialog:
	dialog.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
	dialog.add_button(Gtk::Stock::OPEN, Gtk::RESPONSE_OK);
	int result = dialog.run();
	if (result == Gtk::RESPONSE_OK) {
	    std::string filename = dialog.get_filename();
	    iSpecFileName.clear();
	    mCurrentCursor.clear();
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
	    mCurrentCursor.clear();
	    iChromoLim = 0;
	    OpenFile(filename, false);
	    iInitMaxOrder = iEnv->ChMgr()->GetMaxOrder();
	    iSaved = EFalse;
	}
    }
}

void App::on_action_recreate()
{
    mCurrentCursor = iHDetView->GetCursor();
    if (iDesObserver->IsModelChanged()) {
	SaveTmp();
	OpenFile(GetDefaultTmpFileName(), true);
    }
    else {
	OpenFile(IsSystemChanged() ? GetDefaultTmpFileName() : iSpecFileName, true);
    }
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

void App::on_action_optimize()
{
    Optimize();
}

void App::on_action_disable_opt()
{
    Glib::RefPtr<Gtk::Action> action = iMainWnd->ActionGroup()->get_action("Disable_opt");
    Gtk::ToggleAction* taction = dynamic_cast<Gtk::ToggleAction*>(action.operator->());
    MStSetting<bool>& disable_opt = iStEnv->Settings().GetSetting(MStSettings::ESts_DisableOpt, disable_opt);
    bool active = taction->get_active();
    disable_opt.Set(active);
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
    SaveFile(GetDefaultTmpFileName(), false);
}

void App::CreateModel()
{
    try {
	iEnv->ConstructSystem();
    } catch (std::exception e) {
	std::cerr << "exception caught: " << e.what() << '\n';
    }
    mSigMdlCreated.emit();
}

void App::on_model_created()
{
    mWrkThread->join();
    mWrkThread = NULL;
    iDesObserver->UpdateDesRootObserver();
    iHDetView->SetRoot(iEnv->Root());
    iHDetView->SetCursor(mCurrentCursor);
    if (mCurrentCursor.empty()) {
	iHDetView->SetCursor(iEnv->Root());
    } else {
	iHDetView->SetCursor(mCurrentCursor);
    }
    // Mark model as unchanged, initially
    iDesObserver->SetModelChanged(iEnv->ChMgr()->EnableFixErrors());
    iChanged = false;
    iEnv->ChMgr()->SetEnableFixErrors(false);
    // Init max order from spec, to be able to do redo
    iMaxOrder = iEnv->ChMgr()->GetSpecMaxOrder();
    UpdataUndoRedo();
    // Start tracking the model changes in order to update undo/redo
    iDesObserver->SignalSystemChanged().connect(sigc::mem_fun(*this, &App::on_system_changed));
    //iMainWnd->HideSpinner();
}

void App::OpenFile(const string& aFileName, bool aAsTmp)
{
    /*
    iMainWnd->ShowSpinner();
    for (int c = 0; c < 1000; c++) {
    Gtk::Main::instance()->iteration(false);
    }
    */
    if (iEnv != NULL) {
	iDesObserver->SetDes(NULL);
	iEnv->RemoveProvider(iMdlProv);
	delete iEnv;
	iEnv = NULL;
    }
    iEnv = new Env("DesEnv", aFileName, iLogFileName);
    iEnv->AddProvider(iMdlProv);
    iEnv->ImpsMgr()->AddImportsPaths(KModulesPath);
    iEnv->ChMgr()->SetLim(iChromoLim);
    iEnv->ChMgr()->SetEnableFixErrors(iRepair);
    iEnv->ChMgr()->SetEnableCheckSafety(false);
    MStSetting<bool>& ena_pheno = iStEnv->Settings().GetSetting(MStSettings::ESts_EnablePhenoModif, ena_pheno);
    bool ena_pheno_val = ena_pheno.Get(ena_pheno_val);
    iEnv->ChMgr()->SetEnablePhenoModif(ena_pheno_val);
    // Enable by default support of chromo invariance with respect to muts position
    iEnv->ChMgr()->SetEnableReposMuts(true);
    // Disable processing of optimization
    MStSetting<bool>& disable_opt_s = iStEnv->Settings().GetSetting(MStSettings::ESts_DisableOpt, disable_opt_s);
    bool disable_opt = disable_opt_s.Get(disable_opt);
    iEnv->ChMgr()->SetEnableOptimization(!disable_opt);
    iRepair = false;
    iDesObserver->SetDes(iEnv);
    if (!aAsTmp) {
	iSpecFileName = aFileName;
	iMainWnd->set_title(FormTitle(aFileName));
    }
    else {
	iMainWnd->set_title(FormTitle(iSpecFileName.empty() ? KTitleUnsaved : iSpecFileName));
    }
    mWrkThread = Glib::Threads::Thread::create(sigc::mem_fun(*this, &App::CreateModel));
    /*
    try {
	iEnv->ConstructSystem();
    } catch (std::exception e) {
	std::cerr << "exception caught: " << e.what() << '\n';
    }
    */
    /*
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
    iDesObserver->SetModelChanged(iEnv->ChMgr()->EnableFixErrors());
    iChanged = false;
    iEnv->ChMgr()->SetEnableFixErrors(false);
    // Init max order from spec, to be able to do redo
    iMaxOrder = iEnv->ChMgr()->GetSpecMaxOrder();
    UpdataUndoRedo();
    // Start tracking the model changes in order to update undo/redo
    iDesObserver->SignalSystemChanged().connect(sigc::mem_fun(*this, &App::on_system_changed));
    //iMainWnd->HideSpinner();
    */
}

void App::SaveFile(const string& aFileName, bool aUnorder)
{
    iChanged = iChanged || iDesObserver->IsModelChanged();
    MChromo& chromo = iEnv->Root()->Chromos();
    if (aUnorder) {
	chromo.Root().DeOrder();
    }
    chromo.Save(aFileName);
}

void App::CompactAndSaveFile(const string& aFileName)
{
    iEnv->Root()->CompactChromo();
    iEnv->Root()->Chromos().Save(aFileName);
}

// TODO [YB] To implement automatic optimization
void App::Optimize()
{
    bool changed = false;
    int result = Gtk::RESPONSE_CANCEL;
    do {
	string cursor = iHDetView->GetCursor();
	changed = iEnv->Root()->CompactChromo();
	on_action_save();
	OpenFile(iSpecFileName, false);
	iHDetView->SetCursor(cursor);

	Gtk::MessageDialog dialog(changed ? KDlg_Opt1:KDlg_Opt2, false, MESSAGE_INFO, 
		changed?BUTTONS_OK_CANCEL:BUTTONS_OK, true);
	dialog.set_default_response(Gtk::RESPONSE_OK);
	dialog.set_transient_for(*iMainWnd);
	result = dialog.run();
    } while (changed && result == Gtk::RESPONSE_OK);
}

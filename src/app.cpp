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

/* Time slice of FAP environment, in milliseconds */
const gint KFapeTimeSlice = 50;


const string sType = "DesObserver";

const string& DesObserver::Type()
{
    return sType;
}

DesObserver::DesObserver(): iDesEnv(NULL)
{
}

void DesObserver::SetDes(MEnv* aDesEnv)
{
    assert(aDesEnv != NULL && iDesEnv == NULL || aDesEnv == NULL);
    if (iDesEnv != NULL) {
	Elem* root = iDesEnv->Root();
	root->SetObserver(NULL);
	iDesEnv = NULL;
    }
    else {
	iDesEnv = aDesEnv;
	Elem* root = iDesEnv->Root();
	root->SetObserver(this);
    }
    iSigDesEnvChanged.emit();
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

void DesObserver::OnCompDeleting(Elem& aComp)
{
    iSigCompDeleted.emit(&aComp);
}

void DesObserver::OnCompAdding(Elem& aComp)
{
    iSigCompAdded.emit(&aComp);
}

void DesObserver::OnCompChanged(Elem& aComp)
{
    iSigCompChanged.emit(&aComp);
}

TBool DesObserver::OnCompRenamed(Elem& aComp, const string& aOldName)
{
    iSigCompRenamed.emit(&aComp, aOldName);
}


App::App(): iEnv(NULL), iMainWnd(NULL), iHDetView(NULL) {
    // Create model observer
    iDesObserver = new DesObserver();
    // Create studio environment
    iStEnv = new StEnv(iDesObserver);
    // Default logfilename
    iLogFileName = GetDefaultLogFileName();
    // Create main window
    iMainWnd = new MainWnd();
    iMainWnd->maximize();
    iMainWnd->SetEnvLog(iLogFileName);
    //iMainWnd->UIManager()->signal_post_activate().connect(sigc::mem_fun(*this, &App::on_action));
    iMainWnd->UIManager()->get_action("ui/ToolBar/Open")->signal_activate().connect(sigc::mem_fun(*this, &App::on_action_open));
    iMainWnd->UIManager()->get_action("ui/ToolBar/Save_as")->signal_activate().connect(sigc::mem_fun(*this, &App::on_action_saveas));
    // Create studio DES environment
    iStDesEnv = new StDesEnv(iMainWnd->UIManager());
    // Model specific nodes provider
    iMdlProv = new MdlProv("MdlProv", iStDesEnv);
    // Create model
    iSpecFileName = KSpecFileName;
    iEnv = new Env("DesEnv", KSpecFileName, iLogFileName);
    iEnv->AddProvider(iMdlProv);
    iEnv->ConstructSystem();
    iDesObserver->SetDes(iEnv);
    // Open main hier detail view
    iHDetView = new HierDetailView(*iStEnv, iMainWnd->ClientWnd(), iMainWnd->UIManager());
    iHDetView->SetRoot(iEnv->Root());
    iHDetView->SetCursor(iEnv->Root());
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

void App::on_action(const Glib::RefPtr<Gtk::Action>& aAction)
{
    std::cout << "Action" << std::endl;
}

void App::on_action_open()
{
    std::cout << "Action Open" << std::endl;
    Gtk::FileChooserDialog dialog("Please choose a file", Gtk::FILE_CHOOSER_ACTION_OPEN);
    dialog.set_transient_for(*iMainWnd);

    //Add response buttons the the dialog:
    dialog.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
    dialog.add_button(Gtk::Stock::OPEN, Gtk::RESPONSE_OK);
    int result = dialog.run();
    if (result == Gtk::RESPONSE_OK) {
	std::string filename = dialog.get_filename();
	OpenFile(filename, false);
    }
}

void App::on_action_saveas()
{
    Gtk::FileChooserDialog dialog("Please choose a file", Gtk::FILE_CHOOSER_ACTION_OPEN);
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
    }
}

string App::GetDefaultLogFileName() const
{
    const gchar* home = g_getenv("HOME");
    return string(home) + "/" + KLogFileName;
}

void App::OpenFile(const string& aFileName, bool aAsTmp)
{
    if (iEnv != NULL) {
	//iNaviPane->SetDesEnv(NULL);
	iDesObserver->SetDes(NULL);
	delete iEnv;
	iEnv = NULL;
    }
    iEnv = new Env("DesEnv", aFileName, iLogFileName);
    iEnv->AddProvider(iMdlProv);
    iEnv->ConstructSystem();
    //iNaviPane->SetDesEnv(iEnv);
    iDesObserver->SetDes(iEnv);
    iHDetView->SetRoot(iEnv->Root());
    iHDetView->SetCursor(iEnv->Root());
}

void App::SaveFile(const string& aFileName)
{
    iEnv->Root()->Chromos().Save(aFileName);
}


#include "app.h"
#include <gtkmm.h>
#include "gtkmm/object.h"
#include <iostream>

const char* KLogFileName = "fap2-studio.log";
const char* KTmpFileName = ".fap2-studio-tmp.xml";
const char* KSpecFileName = "/usr/share/fap2-studio-gtk/templ/empty.xml";
const char* KTemplDirName = "/usr/share/fap2-studio-gtk/templ";
const char* KAppName = "fap2-studio";

/* Time slice of FAP environment, in milliseconds */
const gint KFapeTimeSlice = 50;

App::App(): iEnv(NULL), iMainWnd(NULL), iHDetView(NULL) {
    // Create studio environment
    iStEnv = new StEnv();
    // Default logfilename
    iLogFileName = GetDefaultLogFileName();
    // Create model
    iSpecFileName = KSpecFileName;
    iEnv = new Env("DesEnv", KSpecFileName, iLogFileName);
    iEnv->ConstructSystem();
    // Create main window
    iMainWnd = new MainWnd();
    //iMainWnd->UIManager()->signal_post_activate().connect(sigc::mem_fun(*this, &App::on_action));
    iMainWnd->UIManager()->get_action("ui/ToolBar/Open")->signal_activate().connect(sigc::mem_fun(*this, &App::on_action_open));
    // Open main hier detail view
    iHDetView = new HierDetailView(*iStEnv, iMainWnd->ClientWnd(), iMainWnd->UIManager());
    iHDetView->SetRoot(iEnv->Root());
    iHDetView->SetCursor(iEnv->Root());
}

App::~App() {
    delete iMainWnd;
    delete iEnv;
    delete iStEnv;
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

string App::GetDefaultLogFileName() const
{
    const gchar* home = g_getenv("HOME");
    return string(home) + "/" + KLogFileName;
}

void App::OpenFile(const string& aFileName, bool aAsTmp)
{
    if (iEnv != NULL) {
	delete iEnv;
	iEnv = NULL;
    }
    iEnv = new Env("DesEnv", aFileName, iLogFileName);
    iEnv->ConstructSystem();
    iHDetView->SetRoot(iEnv->Root());
    iHDetView->SetCursor(iEnv->Root());
}


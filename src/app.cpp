#include "app.h"
#include "gtkmm/object.h"
#include <iostream>

const char* KLogFileName = "fap-studio.log";
const char* KTmpFileName = ".fap-studio-tmp.xml";
const char* KSpecFileName = "/usr/share/fap-studio-gtk/templ/empty.xml";
const char* KTemplDirName = "/usr/share/fap-studio-gtk/templ";
const char* KAppName = "fap-studio";

/* Time slice of FAP environment, in milliseconds */
const gint KFapeTimeSlice = 50;

App::App(): iMainWnd(NULL) {
    // Default logfilename
    iLogFileName = GetDefaultLogFileName();
    // Create model
    iSpecFileName = KSpecFileName;
    iEnv = new Env("DesEnv", KSpecFileName, iLogFileName);
    // Create main window
    iMainWnd = new MainWnd();
    //iMainWnd->UIManager()->signal_post_activate().connect(sigc::mem_fun(*this, &App::on_action));
    iMainWnd->UIManager()->get_action("ui/ToolBar/Open")->signal_activate().connect(sigc::mem_fun(*this, &App::on_action_open));
}

App::~App() {
    delete iMainWnd;
    delete iEnv;
}

void App::on_action(const Glib::RefPtr<Gtk::Action>& aAction)
{
    std::cout << "Action" << std::endl;
}

void App::on_action_open()
{
    std::cout << "Action Open" << std::endl;
}

string App::GetDefaultLogFileName() const
{
    const gchar* home = g_getenv("HOME");
    return string(home) + "/" + KLogFileName;
}

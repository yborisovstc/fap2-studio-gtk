#include "app.h"

const char* KLogFileName = "fap-studio.log";
const char* KTmpFileName = ".fap-studio-tmp.xml";
const char* KSpecFileName = "/usr/share/fap-studio-gtk/templ/empty.xml";
const char* KTemplDirName = "/usr/share/fap-studio-gtk/templ";
const char* KAppName = "fap-studio";

/* Time slice of FAP environment, in milliseconds */
const gint KFapeTimeSlice = 50;

App::App(): Glib::Object()  {
}

~App::App() {
}

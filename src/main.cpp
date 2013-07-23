
#include "mainwnd.h"
#include <gtkmm/main.h>

int main (int argc, char *argv[])
{
    Gtk::Main kit(argc, argv);

    MainWnd mainwnd;

    //Shows the window and returns when it is closed.
    Gtk::Main::run(mainwnd);

    return 0;
}



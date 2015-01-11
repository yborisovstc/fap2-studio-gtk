
#include "app.h"
#include <gtkmm/main.h>

int main (int argc, char *argv[])
{
    Gtk::Main kit(argc, argv);

    App* app = new App();

    //Shows the window and returns when it is closed.
    Gtk::Main::run(app->Wnd());

    delete app;

    return 0;
}



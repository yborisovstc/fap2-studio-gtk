
#ifndef __FAPSTU_GTK_MAINWND_H
#define __FAPSTU_GTK_MAINWND_H

#include <gtkmm/button.h>
#include <gtkmm/window.h>

class MainWnd : public Gtk::Window
{
    public:
	MainWnd();
	virtual ~MainWnd();

    protected:
	// Signal handlers:
	void on_button_clicked();
	// Member widgets:
	Gtk::Button m_button;
};

#endif 

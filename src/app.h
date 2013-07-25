
#ifndef __FAPSTU_GTK_APP_H
#define __FAPSTU_GTK_APP_H

#include <env.h>
#include <string.h>

#include "mainwnd.h"


class App
{
    public:
	App();
	virtual ~App();

	MainWnd& Wnd() const {return *iMainWnd;};
    private:
	string GetDefaultLogFileName() const;
    private:
	void on_action(const Glib::RefPtr<Gtk::Action>& aAction);
	void on_action_open();
    private:
	Env* iEnv;
	MainWnd* iMainWnd;
	string iLogFileName;
	string iSpecFileName;
};


#endif 

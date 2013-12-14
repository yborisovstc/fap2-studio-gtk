
#ifndef __FAPSTU_GTK_APP_H
#define __FAPSTU_GTK_APP_H

#include <env.h>
#include <string.h>

#include "mainwnd.h"
#include "stenv.h"
#include "hierdetailview.h"
#include "navi.h"
#include "mdlprov.h"


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
	void on_action_saveas();
	void OpenFile(const string& aFileName, bool aAsTmp = false);
	void SaveFile(const string& aFileName);
    private:
	// DES environment
	Env* iEnv;
	// Studio environment
	StEnv* iStEnv;
	StDesEnv* iStDesEnv;
	MainWnd* iMainWnd;
	HierDetailView* iHDetView;
	string iLogFileName;
	string iSpecFileName;
	// Navigation pane widget
	Navi* iNaviPane;
	// Model nodes provider
	MdlProv* iMdlProv;
};


#endif 

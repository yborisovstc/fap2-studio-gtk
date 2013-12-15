
#ifndef __FAPSTU_GTK_APP_H
#define __FAPSTU_GTK_APP_H

#include <env.h>
#include <melem.h>
#include <string.h>

#include "mainwnd.h"
#include "stenv.h"
#include "hierdetailview.h"
#include "navi.h"
#include "mdlprov.h"
#include "mdesobs.h"

// Simple extender of DES root events
class DesObserver: public MBase, public MMdlObserver, public MCompsObserver
{
    public:
	static const string& Type();
	DesObserver();
	void SetDes(MEnv* aDesEnv);
	virtual void *DoGetObj(const std::string& aName);
	// From MMdlObserver
	virtual tSigDesEnvChanged SignalDesEnvChanged();
	virtual tSigCompDeleted SignalCompDeleted();
	virtual tSigCompAdded SignalCompAdded();
	virtual tSigCompChanged SignalCompChanged();
	virtual tSigCompRenamed SignalCompRenamed();
	virtual MEnv* DesEnv();
	// From MCompsObserver
	virtual void OnCompDeleting(Elem& aComp);
	virtual void OnCompAdding(Elem& aComp);
	virtual void OnCompChanged(Elem& aComp);
	virtual TBool OnCompRenamed(Elem& aComp, const string& aOldName);
    protected:
	MEnv* iDesEnv;
	tSigDesEnvChanged iSigDesEnvChanged;
	tSigCompDeleted iSigCompDeleted;
	tSigCompAdded iSigCompAdded;
	tSigCompChanged iSigCompChanged;
	tSigCompRenamed iSigCompRenamed;
};

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
	// Model observer
	DesObserver* iDesObserver;
};


#endif 

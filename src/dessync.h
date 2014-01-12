
#ifndef __FAPSTU_GTK_DESSYNC_H
#define __FAPSTU_GTK_DESSYNC_H

#include <elem.h>
#include "msdesenv.h"
#include <gtkmm/uimanager.h>

// DES synchronizer agent

class ADesSync: public Elem
{
    public:
	static const char* Type() { return "ADesSync";};
	static string PEType();
	ADesSync(const string& aName = string(), Elem* aMan = NULL, MEnv* aEnv = NULL, MSDesEnv* aSDesEnv = NULL);
	virtual ~ADesSync();
    protected:
	// From Base
	virtual void *DoGetObj(const char *aName, TBool aIncUpHier = ETrue, const RqContext* aCtx = NULL);
	// Signal handlers:
	void on_action_run();
	void on_action_stop();
	void on_action_pause();
	void on_action_next();
	gboolean OnTimerEvent();
    protected:
	void DoStep();
    private:
	int iCount;
	MSDesEnv* iSDesEnv;
	bool iRunning;
	bool iStopped;
	Gtk::UIManager::ui_merge_id iUiId;
	Glib::RefPtr<Gtk::ActionGroup> iActionGroup;
	Glib::RefPtr<Glib::TimeoutSource> iTimer;
};


#endif

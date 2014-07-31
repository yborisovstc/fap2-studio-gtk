
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
class DesObserver: public MBase, public MMdlObserver, public MCompsObserver, public MLogObserver
{
    public:
	static const string& Type();
	DesObserver();
	virtual ~DesObserver();
	void SetDes(MEnv* aDesEnv);
	void UpdateDesRootObserver();
	bool IsModelChanged() const;
	void SetModelChanged(bool aChanged = true);
	virtual void *DoGetObj(const std::string& aName);
	// From MMdlObserver
	virtual tSigDesEnvChanged SignalDesEnvChanged();
	virtual tSigCompDeleted SignalCompDeleted();
	virtual tSigCompAdded SignalCompAdded();
	virtual tSigCompChanged SignalCompChanged();
	virtual tSigCompRenamed SignalCompRenamed();
	virtual tSigContentChanged SignalContentChanged();
	virtual tSigSystemChanged SignalSystemChanged();
	virtual tSigLogAdded SignalLogAdded();
	virtual tSigSystemCreated SignalSystemCreated();
	virtual MEnv* DesEnv();
	// From MCompsObserver
	virtual void OnCompDeleting(Elem& aComp);
	virtual void OnCompAdding(Elem& aComp);
	virtual void OnCompChanged(Elem& aComp);
	virtual TBool OnCompRenamed(Elem& aComp, const string& aOldName);
	virtual void OnContentChanged(Elem& aComp);
	// From MLogObserver
	virtual void OnLogAdded(MLogRec::TLogRecCtg aCtg, Elem* aNode, const std::string& aContent);
	virtual void OnLogRecDeleting(MLogRec* aLogRec);
    protected:
	MEnv* iDesEnv;
	tSigDesEnvChanged iSigDesEnvChanged;
	tSigCompDeleted iSigCompDeleted;
	tSigCompAdded iSigCompAdded;
	tSigCompChanged iSigCompChanged;
	tSigCompRenamed iSigCompRenamed;
	tSigContentChanged iSigContentChanged;
	tSigSystemChanged iSigSystemChanged;
	tSigLogAdded iSigLogAdded;
	tSigSystemCreated iSigSystemCreated;
	bool iChanged;
	MLogRec* iLogRec;
};

class App
{
    public:
	App();
	virtual ~App();

	MainWnd& Wnd() const {return *iMainWnd;};
    private:
	string GetDefaultLogFileName() const;
	string GetDefaultTmpFileName() const;
	void SaveTmp();
    private:
	void on_action(const Glib::RefPtr<Gtk::Action>& aAction);
	void on_action_new();
	void on_action_open();
	void on_action_save();
	void on_action_saveas();
	void on_action_compactas();
	void on_action_recreate();
	void on_action_undo();
	void on_action_redo();
	void on_system_changed();
	void OpenFile(const string& aFileName, bool aAsTmp = false);
	void SaveFile(const string& aFileName);
	void CompactAndSaveFile(const string& aFileName);
	string FormTitle(const string& aFilePath);
	void UpdataUndoRedo();
	bool IsSystemChanged() const;
	bool CheckCurrentModelSaving();
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
	// TODO [YB] Do we need it? It is actually not used in app logic
	bool iSaved;
	int iChromoLim;
	int iMaxOrder;
	// Max orider of chromo right after system loading
	int iInitMaxOrder;
	// Sign of model was changed sinse opening from persistient spec (not tmp)
	// DES observer IsModelChanged indicates the status only sinse model loading,
	// but this can happen when loading from tmp spec
	bool iChanged;
	LogViewL* iLogView;
};


#endif 

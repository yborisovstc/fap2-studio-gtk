
#ifndef __FAPSTU_GTK_APP_H
#define __FAPSTU_GTK_APP_H

#include <env.h>
#include <melem.h>
#include <string.h>
#include <sserver.h>

#include "mainwnd.h"
#include "stenv.h"
#include "hierdetailview.h"
#include "navi.h"
#include "mdlprov.h"
#include "mdesobs.h"
#include "dessrv.h"

// Simple extender of DES root events
class DesObserver: public MBase, public MMdlObserver, public MAgentObserver, public MLogObserver
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
	virtual tSigTLogAdded SignalTLogAdded();
	virtual tSigSystemCreated SignalSystemCreated();
	virtual MEnv* DesEnv();
	// From MAgentObserver
	virtual void OnCompDeleting(MElem& aComp, TBool aSoft = true, TBool aModif = EFalse);
	virtual void OnCompAdding(MElem& aComp, TBool aModif = EFalse);
	virtual TBool OnCompChanged(MElem& aComp, const string& aName=string(), TBool aModif = EFalse);
	virtual TBool OnCompRenamed(MElem& aComp, const string& aOldName);
	virtual TBool OnChanged(MElem& aComp);
	virtual void OnCompMutated(const MElem* aNode);
	// From MIface	
	virtual MIface* Call(const string& aSpec, string& aRes);
	virtual string Mid() const;
	// From MLogObserver
	virtual void OnLogAdded(long aTimestamp, TLogRecCtg aCtg, const MElem* aNode, const std::string& aContent, int aMutId = 0);
	virtual void OnLogAdded(const TLog& aLog);
	virtual void OnLogRecDeleting(MLogRec* aLogRec);
	virtual void AddObservable(MLogRec* aObservable);
	virtual void RemoveObservable(MLogRec* aObservable);
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
	tSigTLogAdded iSigTLogAdded;
	tSigSystemCreated iSigSystemCreated;
	bool iChanged;
	MLogRec* iLogRec;
};

class BaseClient;
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
	void on_action_close();
	void on_action_save();
	void on_action_saveas();
	void on_action_compactas();
	void on_action_optimize();
	void on_action_recreate();
	void on_action_undo();
	void on_action_redo();
	void on_action_repair();
	void on_action_disable_opt();
	void on_system_changed();
	void on_setting_changed_pheno_enable();
	void on_setting_changed_disable_opt();
	void on_comp_selected(MElem* aComp);
	void CloseFile();
	void OpenFile(const string& aFileName, bool aAsTmp = false);
	void SaveFile(const string& aFileName, bool aUnorder = true);
	void CompactAndSaveFile(const string& aFileName);
	// Optimizes models chromo
	void Optimize();
	string FormTitle(const string& aFilePath);
	void UpdataUndoRedo();
	void InitialUpdate();
	bool IsSystemChanged() const;
	bool CheckCurrentModelSaving();
	bool Idle();
    private:
	// DES environment
	Env* iEnv;
	// Server
	Server* mDesSrv;
	static const int mDesSrvPort;
	Glib::Threads::Thread* mDesSrvThread;
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
	bool iRepair;
	LogViewL* iLogView;
};


#endif 

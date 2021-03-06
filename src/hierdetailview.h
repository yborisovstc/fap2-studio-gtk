#ifndef __FAP2STU_HIERDETAILVIEW_H
#define __FAP2STU_HIERDETAILVIEW_H

#include <vector>
#include <elem.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/uimanager.h>
#include <gtkmm/alignment.h>
#include <gtkmm/actiongroup.h>

#include "elemdetrp.h"
#include "toolitems.h"
#include "msenv.h"
#include "mdrp.h"

// System layer detail view. It exposes the structure of given layer of system hierarchy. 
// View handles moving up/down hierarchy, mutation of element focused on
//
class HierDetailView
{
    public:
	typedef std::vector<void*> TNavHist;
	typedef sigc::signal<void> tSigRecreateRequested;
	typedef sigc::signal<void, MElem*> tSigCompSelected;
    public:
	HierDetailView(MSEnv& aStEnv, Gtk::ScrolledWindow& aCont, const Glib::RefPtr<Gtk::UIManager>& aUiMgr);
	virtual ~HierDetailView();
	void SetRoot(MElem* aRoot);
	void SetCursor(MElem* aRoot, bool FromHist = false);
	void SetCursor(const string& aUri);
	void Reset();
	string GetCursor() const;
	tSigRecreateRequested SignalRecreateRequested() {return mSigRecreateRequested;};
	tSigCompSelected SignalCompSelected() { return mSigCompSelected;};
	// Signal handlers:
	void on_comp_selected(MElem* aComp);
	void on_comp_activated(MElem* aComp);
	void on_logrec_activated(const string& aNodeUri, int aMutId);
    protected:
	// Signal handlers:
	void on_action_insert();
	void on_action_up();
	void on_action_undo();
	void on_action_redo();
	void on_action_goback();
	void on_action_goforward();
	void on_action_goparent();
	void on_action_attention();
	void on_action_spec_mut_node();
	void on_action_pin_mut_node();
	void on_cont_size_alocated(Allocation& alloc);
	void on_drp_drag_motion(Gtk::Widget& widget, int x, int y);
	void on_drp_attention(const string& aInfo);
	void on_drp_reload_required();
	bool on_parent_press_event(GdkEventKey* aEvent);
    protected:
	void UpdataHistNavUI();
	void UpdatePinMutNode();
	void UpdateBtnUp();
	void UpdateBtnGoParent();
	void ResetAttention();
    private:
	// Environment
	MSEnv& iStEnv;
	Gtk::ScrolledWindow& iContWnd;
	Gtk::Alignment* iAlignent;
	Glib::RefPtr<Gtk::UIManager> iUiMgr;
	MElem* iRoot;
	string iCursor;
	// Node detail representation
	MDrp* iDetRp;
	Glib::RefPtr<Gtk::ActionGroup> irActionGroup;
	TiLabel* iTbNameHd;
	TiLabel* iTbName;
	TiLabel* iTbParentHd;
	TiLabel* iTbParent;
	// System navigation history
	TNavHist iNavHist;
	TNavHist::iterator iNavHistIter;
	tSigRecreateRequested mSigRecreateRequested;
	Glib::ustring mAttention;
	tSigCompSelected mSigCompSelected;
};

#endif

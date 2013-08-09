#ifndef __FAP2STU_HIERDETAILVIEW_H
#define __FAP2STU_HIERDETAILVIEW_H

#include <elem.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/uimanager.h>
#include <gtkmm/alignment.h>
#include <gtkmm/actiongroup.h>

#include "elemdetrp.h"
#include "toolitems.h"
#include "msenv.h"

// System layer detail view. It exposes the structure of given layer of system hierarchy. 
// View handles moving up/down hierarchy, mutation of element focused on
//
class HierDetailView
{
    public:
	HierDetailView(MSEnv& aStEnv, Gtk::Container& aCont, const Glib::RefPtr<Gtk::UIManager>& aUiMgr);
	virtual ~HierDetailView();
	void SetRoot(Elem* aRoot);
	void SetCursor(Elem* aRoot);
    protected:
	// Signal handlers:
	void on_action_up();
	void on_comp_selected(Elem* aComp);
    private:
	// Environment
	MSEnv& iStEnv;
	Gtk::Container& iContWnd;
	Gtk::Alignment* iAlignent;
	Glib::RefPtr<Gtk::UIManager> iUiMgr;
	Elem* iRoot;
	ElemDetRp* iDetRp;
	Glib::RefPtr<Gtk::ActionGroup> irActionGroup;
	TiLabel* iTbNameHd;
	TiLabel* iTbName;
	TiLabel* iTbParentHd;
	TiLabel* iTbParent;
};

#endif

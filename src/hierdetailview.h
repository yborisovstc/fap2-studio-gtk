#ifndef __FAP2STU_HIERDETAILVIEW_H
#define __FAP2STU_HIERDETAILVIEW_H

#include <elem.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/uimanager.h>

#include "elemdetrp.h"

// System layer detail view. It exposes the structure of given layer of system hierarchy. 
// View handles moving up/down hierarchy, mutation of element focused on
//
class HierDetailView
{
    public:
	HierDetailView(Gtk::Container& aCont, const Glib::RefPtr<Gtk::UIManager>& aUiMgr);
	virtual ~HierDetailView();
	void SetRoot(Elem* aRoot);
	void SetFocuse(Elem* aRoot);
    private:
	Gtk::Container& iContWnd;
	const Glib::RefPtr<Gtk::UIManager>& iUiMgr;
	Elem* iRoot;
	ElemDetRp* iDetRp;
};

#endif

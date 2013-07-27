#ifndef __FAP2STU_HIERDETAILVIEW_H
#define __FAP2STU_HIERDETAILVIEW_H

#include <gtkmm/scrolledwindow.h>

// System layer detail view. It exposes the structure of given layer of system hierarchy. 
// View handles moving up/down hierarchy, mutation of element focused on
//
class HierDetailView
{
    public:
	HierDetailView();
	virtual ~HierDetailView();
	Gtk::Widget& Window();
    private:
	Gtk::ScrolledWindow iScrolledWnd;
};

#endif

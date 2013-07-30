#include "hierdetailview.h"

HierDetailView::HierDetailView(Gtk::Container& aCont, const Glib::RefPtr<Gtk::UIManager>& aUiMgr): iContWnd(aCont), iUiMgr(aUiMgr), iDetRp(NULL)
{
    // Extending toolbar
    Gtk::Widget* pToolBar = iUiMgr->get_widget("/ToolBar");
}

HierDetailView::~HierDetailView()
{
    delete iDetRp;
}

void HierDetailView::SetRoot(Elem* aRoot)
{
    assert(aRoot != NULL);
    iRoot = aRoot;
}

void HierDetailView::SetFocuse(Elem* aElem)
{
    assert(aElem != NULL);
    if (iDetRp != NULL) {
	delete iDetRp;
	iDetRp = NULL;
    }
    iDetRp = new ElemDetRp(aElem);
    iContWnd.add(*iDetRp);
    iDetRp->show();
}

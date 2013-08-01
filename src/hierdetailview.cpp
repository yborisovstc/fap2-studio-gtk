#include "hierdetailview.h"

HierDetailView::HierDetailView(Gtk::Container& aCont, const Glib::RefPtr<Gtk::UIManager>& aUiMgr): 
    iContWnd(aCont), iUiMgr(aUiMgr), iDetRp(NULL), iAlignent()
{
    // Extending toolbar
    Gtk::Widget* pToolBar = iUiMgr->get_widget("/ToolBar");
    iAlignent = new Gtk::Alignment(1.0, 1.0, 1.0, 1.0);
    iContWnd.add(*iAlignent);
    iAlignent->show();
}

HierDetailView::~HierDetailView()
{
    delete iDetRp;
    delete iAlignent;
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
    iAlignent->add(*iDetRp);
    iDetRp->show();
}

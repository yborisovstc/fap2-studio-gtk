#include "elemdetrp.h"

ElemDetRp::ElemDetRp(Elem* aElem): iElem(aElem)
{
    // Add components
    for (std::vector<Elem*>::iterator it = iElem->Comps().begin(); it != iElem->Comps().end(); it++) {
	Elem* comp = *it;
	assert(comp != NULL);
	ElemCompRp* rp = new ElemCompRp(comp);
	add(*rp);
	iCompRps[comp] = rp;
	rp->show();
    }
}

ElemDetRp::~ElemDetRp()
{
}

bool ElemDetRp::on_expose_event(GdkEventExpose* aEvent)
{
    // Draw body rect
    Gtk::Allocation alc = get_allocation();
    Glib::RefPtr<Gdk::Window> drw = get_window();
    Glib::RefPtr<Gtk::Style> style = get_style(); 	
    Glib::RefPtr<Gdk::GC> gc = style->get_fg_gc(get_state());
    drw->draw_rectangle(gc, FALSE, iBodyAlc.get_x(), iBodyAlc.get_y(), iBodyAlc.get_width() - 1, iBodyAlc.get_height() - 1);
    // Head separator
    GtkAllocation head_alc; iHead->Allocation(&head_alc);
    gdk_draw_line(BinWnd(), Gc(), iBodyAlc.x, head_alc.height, iBodyAlc.x + iBodyAlc.width - 1, head_alc.height);
    // Draw init rect
    GtkAllocation init_alc; iInit->Allocation(&init_alc);
    gdk_draw_rectangle(BinWnd(), Gc(), FALSE, init_alc.x -1 , init_alc.y -1 , init_alc.width + 1, init_alc.height + 1);
    // Draw trans rect
    GtkAllocation tran_alc; iTrans->Allocation(&tran_alc);
    gdk_draw_rectangle(BinWnd(), Gc(), FALSE, tran_alc.x -1 , tran_alc.y -1 , tran_alc.width + 1, tran_alc.height + 1);
}



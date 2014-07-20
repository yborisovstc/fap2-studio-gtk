#include <iostream>
#include "common.h"
#include "elemcomprp.h"


// Using dedicated DnD target for component 
static GtkTargetEntry targetentries[] =
{
//    { (gchar*) "STRING", GTK_TARGET_SAME_WIDGET, KTei_ElemCrp },
    { (gchar*) KDnDTarg_Comp, 0, 0 },
};

ElemCrpCtxMenu::ElemCrpCtxMenu(): Gtk::Menu()
{
}



	
ElemCompHead::ElemCompHead(const Elem& aElem): iElem(aElem)
{
    // Create Name
    iName = new Gtk::Label();
    iName->set_text(iElem.Name());
    iName->show();
    pack_start(*iName, false, false, KCompHeaderLabelsGap);
    // Create Parent
    iParent = new Gtk::Label();
    iParent->set_text(iElem.EType());
    iParent->show();
    pack_start(*iParent, false, false, 2);
}

ElemCompHead::~ElemCompHead()
{
    delete iName;
    delete iParent;
}

bool ElemCompHead::on_expose_event(GdkEventExpose* aEvent)
{
    Gtk::HBox::on_expose_event(aEvent);
    // Parent separator
    Gtk::Allocation alc = get_allocation();
    Gtk::Allocation p_alc = iParent->get_allocation();
    Glib::RefPtr<Gdk::Window> drw = get_window();
    Glib::RefPtr<Gtk::Style> style = get_style(); 	
    Glib::RefPtr<Gdk::GC> gc = style->get_fg_gc(get_state());
    gint x = p_alc.get_x() - KCompHeaderLabelsGap/2;
    drw->draw_line(gc, x, alc.get_y(), x, alc.get_height() - 1);
}


ElemCompRp::ElemCompRp(Elem* aElem): iElem(aElem), iHead(NULL), iHighlighted(false), iLArea(MCrp::EUnknown)
{
    bool isatt = iElem->IsChromoAttached();
    // Set name
    if (isatt) {
	set_name("ElemCompRp");
    }
    else {
	set_name("ElemCompRp~da");
    }
    // Add header
    iHead = new ElemCompHead(*iElem);
    add(*iHead);
    iHead->show();
    // Add content
    string cont;
    iElem->GetCont(cont);
    iContent.set_line_wrap(true);
    iContent.set_text(cont);
    add(iContent);
    iContent.show();

    // Set events mask
    add_events(Gdk::BUTTON_PRESS_MASK);
    iHead->iName->signal_button_press_event().connect(sigc::mem_fun(*this, &ElemCompRp::on_name_button_press));
    // [DnD] We would need here both MOVE and COPY actions available, when COPY is initiated with CTRL button,
    // but GTK doesn't provide the convenient way to implement this, ref gtk_drag_get_event_actions in gtkdnd.c 
    // So the currently COPY is priority but MOVE initiated via SHIFT
    drag_source_set(Gtk::ArrayHandle_TargetEntry(targetentries, 1, Glib::OWNERSHIP_NONE), Gdk::MODIFIER_MASK, Gdk::ACTION_COPY | Gdk::ACTION_MOVE);
}

void ElemCompRp::Construct()
{
}

ElemCompRp::~ElemCompRp()
{
    delete iHead;
}


void ElemCompRp::on_realize()
{
    Gtk::Layout::on_realize();
    // Base construct
    Construct();
}

bool ElemCompRp::on_expose_event(GdkEventExpose* aEvent)
{
    Gtk::Layout::on_expose_event(aEvent);
    // Draw body rect
    Glib::RefPtr<Gdk::Window> drw = get_bin_window();
    Glib::RefPtr<Gtk::Style> style = get_style(); 	
    Glib::RefPtr<Gdk::GC> gc = style->get_fg_gc(get_state());
    drw->draw_rectangle(gc, false, iBodyAlc.get_x(), iBodyAlc.get_y(), iBodyAlc.get_width() - 1, iBodyAlc.get_height() - 1);
    // Head separator
    Gtk::Allocation head_alc = iHead->get_allocation();
    drw->draw_line(gc, iBodyAlc.get_x(), head_alc.get_height(), iBodyAlc.get_x() + iBodyAlc.get_width() - 1, head_alc.get_height());
}

void ElemCompRp::on_size_allocate(Gtk::Allocation& aAllc)
{
    Gtk::Layout::on_size_allocate(aAllc);
    Gtk::Requisition head_req = iHead->size_request();
    // Calculate allocation of comp body
    iBodyAlc = Gtk::Allocation(0, 0, aAllc.get_width(), aAllc.get_height());
    // Allocate header
    Gtk::Allocation head_alc = Gtk::Allocation(iBodyAlc.get_x(), iBodyAlc.get_y(), iBodyAlc.get_width(), head_req.height);
    iHead->size_allocate(head_alc);
    // Allocate content
    Gtk::Allocation cont_alc = Gtk::Allocation(iBodyAlc.get_x(), iBodyAlc.get_y() + head_alc.get_height() + KViewCompEmptyBodyHight/2,
	    iBodyAlc.get_width(), aAllc.get_height() - head_req.height - KViewCompEmptyBodyHight);
    iContent.size_allocate(cont_alc);
}

void ElemCompRp::on_size_request(Gtk::Requisition* aRequisition)
{
    Gtk::Requisition head_req = iHead->size_request();
    // Body width
    gint body_w = head_req.width;
    TInt body_h = KViewCompEmptyBodyHight;
    aRequisition->width = body_w; 
    aRequisition->height = head_req.height + body_h;
    // Updating with size of content
    Gtk::Requisition cont_req = iContent.size_request();
    aRequisition->height += cont_req.height;
    aRequisition->width = max(aRequisition->width, cont_req.width + 2*KViewElemCrpInnerBorder); 
}

bool ElemCompRp::on_name_button_press(GdkEventButton* event)
{
    iSigButtonPressName.emit(event);
}

void ElemCompRp::DoSetHighlighted(bool aSet)
{
    if (aSet != iHighlighted) {
	iHighlighted = aSet;
	if (iHighlighted) {
	    std::cout << "Elem [" << iElem->Name() << "]: higligted" << std::endl;
	    set_state(Gtk::STATE_PRELIGHT);
	    //set_state(Gtk::STATE_SELECTED);
	}
	else {
	    set_state(Gtk::STATE_NORMAL);
	}
    }
}

bool ElemCompRp::on_button_press_event(GdkEventButton* aEvent)
{
    iSigButtonPress.emit(aEvent);
}

void ElemCompRp::on_drag_data_get(const Glib::RefPtr<Gdk::DragContext>&, Gtk::SelectionData& data, guint info, guint time)
{
    std::cout << "ElemCompRp on_drag_data_get"  << std::endl;
    GUri uri;
    iElem->GetUri(uri, iElem->GetMan());
    std::string suri = uri.GetUri();
    data.set(KDnDTarg_Comp, suri);
}


const string sType = "ElemCrp";

const string& ElemCrp::Type()
{
    return sType;
}

string ElemCrp::EType()
{
    return ":Elem";
}

ElemCrp::ElemCrp(Elem* aElem)
{
    iRp = new ElemCompRp(aElem);
}

ElemCrp::~ElemCrp()
{
    delete iRp;
}

void *ElemCrp::DoGetObj(const string& aName)
{
    void* res = NULL;
    if (aName ==  Type()) {
	res = this;
    }
    return res;
}

Gtk::Widget& ElemCrp::Widget()
{
    return *iRp;
}

MCrp::tSigButtonPressName ElemCrp::SignalButtonPressName()
{
    return iRp->iSigButtonPressName;
}

MCrp::tSigButtonPress ElemCrp::SignalButtonPress()
{
    return iRp->iSigButtonPress;
}

MCrp::tSigUpdated ElemCrp::SignalUpdated()
{
    return iRp->iSigUpdated;
}

bool ElemCrp::IsActionSupported(Action aAction)
{
    return (aAction == EA_Remove || aAction == EA_Rename || aAction == EA_Save_Chromo
	    || aAction == EA_Edit_Content);
}

bool ElemCrp::Dragging()
{
    return false;
}

void ElemCrp::SetHighlighted(bool aSet)
{
    iRp->DoSetHighlighted(aSet);
}

Elem* ElemCrp::Model()
{
    return iRp->iElem;
}

void ElemCrp::GetContentUri(GUri& aUri)
{
}

void ElemCrp::SetLArea(int aArea)
{
    iRp->iLArea = aArea;
}

int ElemCrp::GetLArea() const
{
    return iRp->iLArea;
}

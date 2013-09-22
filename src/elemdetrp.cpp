
#include <iostream>
#include "common.h"
#include "elemdetrp.h"

static GtkTargetEntry targetentries[] =
{
    { (gchar*) "STRING",        0, 0 },
    { (gchar*) "text/plain",    0, 1 },
    { (gchar*) "text/uri-list", 0, 2 },
};

/*
   static const Gtk::TargetEntry targetentries[] =
   {
   Gtk::TargetEntry("STRING", Gtk::TARGET_SAME_APP, 0),
   Gtk::TargetEntry("text/plain", Gtk::TARGET_SAME_APP, 1),
   Gtk::TargetEntry("text/uri-list", Gtk::TARGET_SAME_APP, 2)
   };
   */

ElemDetRp::ElemDetRp(Elem* aElem, const MCrpProvider& aCrpProv): Gtk::Layout(), iElem(aElem), iCrpProv(aCrpProv)
{
    // Base construct
    Construct();
    //drag_dest_set(Gtk::DEST_DEFAULT_ALL);
    drag_dest_set(Gtk::ArrayHandle_TargetEntry(targetentries, 3, Glib::OWNERSHIP_NONE));
}

ElemDetRp::~ElemDetRp()
{
    Erase();
}

void ElemDetRp::Construct()
{
    // Add components
    for (std::vector<Elem*>::iterator it = iElem->Comps().begin(); it != iElem->Comps().end(); it++) {
	Elem* comp = *it;
	assert(comp != NULL);
	MCrp* rp = iCrpProv.CreateRp(*comp);
	Gtk::Widget& rpw = rp->Widget();
	//	rp->signal_button_press_event().connect(sigc::mem_fun(*this, &ElemDetRp::on_comp_button_press));
	rpw.signal_button_press_event().connect(sigc::bind<Elem*>(sigc::mem_fun(*this, &ElemDetRp::on_comp_button_press_ext), comp));
	rp->SignalButtonPressName().connect(sigc::bind<Elem*>(sigc::mem_fun(*this, &ElemDetRp::on_comp_button_press_name), comp));
	add(rpw);
	iCompRps[comp] = rp;
	rpw.show();
    }
}

void ElemDetRp::Erase()
{
    // Remove components
    for (tCrps::iterator it = iCompRps.begin(); it != iCompRps.end(); it++) {
	MCrp* rp = it->second;
	delete rp;
    }
    iCompRps.clear();

}

void ElemDetRp::on_size_allocate(Gtk::Allocation& aAllc)
{
//    Gtk::Layout::on_size_allocate(aAllc);
//    Set allocation to itself and resize the windows. Refer gtk_layout_size_allocate 
//    Gtk::Layout::on_size_allocate can be invoked instead but it does childs size allocation internally
//    so it causes ovwrhead because we allocate child sizes here. 
//    Probably better would be to set layout children requisition first and then run Gtk::Layout::on_size_allocate
//    which does childs size allocation then. But this approach doesn't work for me - it causes continous resize

    set_allocation(aAllc);
    if (get_realized()) {
	get_window()->move_resize(aAllc.get_x(), aAllc.get_y(), aAllc.get_width(), aAllc.get_height());
	get_bin_window()->resize(aAllc.get_width(), aAllc.get_height());
    }

    // Allocate components
    int compb_x = aAllc.get_width()/2, compb_y = KViewCompGapHight;
    for (std::vector<Elem*>::iterator it = iElem->Comps().begin(); it != iElem->Comps().end(); it++) {
	MCrp* crp = iCompRps.at(*it);
	Gtk::Widget* comp = &(crp->Widget());
	Gtk::Requisition req = comp->size_request();
	int comp_body_center_x = req.width / 2;
	Gtk::Allocation allc = Gtk::Allocation(compb_x - comp_body_center_x, compb_y, req.width, req.height);
	comp->size_allocate(allc);
	compb_y += req.height + KViewCompGapHight;
    }
}

void ElemDetRp::on_size_request(Gtk::Requisition* aReq)
{
    // Calculate size of comps
    int comp_w = 0, comp_h = KViewCompGapHight;
    for (std::map<Elem*, MCrp*>::iterator it = iCompRps.begin(); it != iCompRps.end(); it++) {
	Gtk::Widget* cpw = &(it->second->Widget());
	Gtk::Requisition req = cpw->size_request();
	comp_w = max(comp_w, req.width);
	comp_h += req.height + KViewCompGapHight;
    }

    aReq->width = comp_w; 
    aReq->height = comp_h + KViewCompGapHight;
}

bool ElemDetRp::on_comp_button_press(GdkEventButton* event)
{
    std::cout << "on_comp_button_press" << std::endl;
}

bool ElemDetRp::on_comp_button_press_ext(GdkEventButton* event, Elem* aComp)
{
    std::cout << "on_comp_button_press, comp [" << aComp->Name() << "]" << std::endl;
//    iSigCompSelected.emit(aComp);
}

void ElemDetRp::on_comp_button_press_name(GdkEventButton* event, Elem* aComp)
{
}

bool ElemDetRp::on_drag_drop(const Glib::RefPtr<Gdk::DragContext>& context, int x, int y, guint time)
{
    //bool res = Layout::on_drag_drop(context, x, y, time);
    return true;
}

void ElemDetRp::on_drag_data_received(const Glib::RefPtr<Gdk::DragContext>& context, int x, int y, const Gtk::SelectionData& selection_data, guint info, guint time)
{
    std::string targ = selection_data.get_target();
    Glib::ustring data = selection_data.get_text();
    context->drag_finish(true, true, time);
    on_node_dropped(data);
}

void ElemDetRp::on_node_dropped(const std::string& aUri)
{
    add_node(aUri);
}

void ElemDetRp::add_node(const std::string& aParentUri)
{
    MChromo& mut = iElem->Mutation();
    ChromoNode smutr = mut.Root();
    ChromoNode comp = smutr.AddChild(ENt_Node);
    comp.SetAttr(ENa_Parent, aParentUri);
    comp.SetAttr(ENa_Id, "Test_name");
    iElem->Mutate();
    Erase();
    Construct();
    //Refresh();
}


	

const string sElemDrpType = "ElemDrp";

const string& ElemDrp::Type()
{
    return sElemDrpType;
}

string ElemDrp::EType()
{
    return Elem::PEType();
}

ElemDrp::ElemDrp(Elem* aElem, const MCrpProvider& aCrpProv)
{
    iRp = new ElemDetRp(aElem, aCrpProv);
}

ElemDrp::~ElemDrp()
{
    delete iRp;
}

void *ElemDrp::DoGetObj(const string& aName)
{
    void* res = NULL;
    if (aName ==  Type()) {
	res = this;
    }
    if (aName ==  MDrp::Type()) {
	res = (MDrp*) this;
    }
    return res;
}

Gtk::Widget& ElemDrp::Widget()
{
    return *iRp;
}

ElemDrp::tSigCompSelected ElemDrp::SignalCompSelected()
{
    return iRp->iSigCompSelected;
}

Elem* ElemDrp::Model()
{
    return iRp->iElem;
}



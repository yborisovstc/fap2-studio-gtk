
#ifndef __FAP2STU_ELEMDETRP_H
#define __FAP2STU_ELEMDETRP_H

#include <map>
#include <gtkmm/layout.h>
#include <gtkmm/box.h>
#include <gtkmm/enums.h>
#include <elem.h>
#include "elemcomprp.h"
#include "mprov.h"
#include "mcrp.h"
#include "common.h"

using namespace Gtk;

class ElemDetRp: public Gtk::Layout, public MCrpMgr
{
    friend class ElemDrp;
    public:
    typedef std::map<Elem*, MCrp*> tCrps;
    public:
	ElemDetRp(Elem* aElem, const MCrpProvider& aCrpProv);
	virtual ~ElemDetRp();
	Elem* GetElem();
    protected:
	virtual void on_realize();
	virtual void on_size_allocate(Gtk::Allocation& aAlloc);
	virtual void on_size_request(Gtk::Requisition* aRequisition);
	virtual bool on_drag_motion (const Glib::RefPtr<Gdk::DragContext>& context, int x, int y, guint time);
	virtual bool on_drag_drop(const Glib::RefPtr<Gdk::DragContext>& context, int x, int y, guint time);
	virtual void on_drag_data_received(const Glib::RefPtr<Gdk::DragContext>& context, int x, int y, 
		const Gtk::SelectionData& selection_data, guint info, guint time);
	virtual void on_drag_leave(const Glib::RefPtr<Gdk::DragContext>& context, guint time);
	// Signal handlers
	bool on_comp_button_press_ext(GdkEventButton* event, Elem* aComp);
	void on_comp_button_press_name(GdkEventButton* event, Elem* aComp);
	void on_comp_button_press(GdkEventButton* event, Elem* aComp);
	// From MCrpMgr
	virtual bool IsTypeAllowed(const std::string& aType) const;
    protected:
	virtual void on_node_dropped(const std::string& aUri);
    protected:
	virtual void Construct();
	void Erase();
	void Refresh();
	void add_node(const std::string& aParentUri);
	void rename_node(const std::string& aNodeUri, const std::string& aNewName);
	void remove_node(const std::string& aNodeUri);
	void change_content(const std::string& aNodeUri, const std::string& aNewContent);
	void move_node(const std::string& aNodeUri, const std::string& aDestUri);
	void ShowCrpCtxDlg(GdkEventButton* event, Elem* aComp);
	void on_comp_menu_rename();
	void on_comp_menu_remove();
	void on_comp_menu_edit_content();
    protected:
	// Compact representations  provider
	const MCrpProvider& iCrpProv;
	Elem* iElem;
	tCrps iCompRps; // Components representations
	MDrp::tSigCompSelected iSigCompSelected;
	Gtk::Menu iCrpContextMenu;
	Elem* iCompSelected;
	std::map<MCrp::Action, Gtk::Menu_Helpers::MenuElem> iCompMenuElems;
	TDnDTarg iDnDTarg;
	MCrp* iDropBaseCandidate;
	std::string iDndReceivedData;
};

class ElemDrp: public MDrp
{
    public:
	static const string& Type();
	static string EType();
    public:
	ElemDrp(Elem* aElem, const MCrpProvider& aCrpProv);
	virtual ~ElemDrp();
	// From MDrp
	virtual void *DoGetObj(const string& aName);
	virtual Gtk::Widget& Widget();
	virtual Elem* Model();
	virtual tSigCompSelected SignalCompSelected();
    private:
	ElemDetRp* iRp;
};

#endif

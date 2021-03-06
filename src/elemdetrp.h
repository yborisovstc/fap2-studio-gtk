
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
using namespace std;

class ElemDetRp: public Gtk::Layout, public MCrpMgr
{
    friend class ElemDrp;
    public:
    typedef std::map<MElem*, MCrp*> tCrps;
    public:
	ElemDetRp(MElem* aElem, const MCrpProvider& aCrpProv, MSEnv& aStEnv);
	virtual ~ElemDetRp();
	MElem* GetElem();
    public:
	// Signal handlers
	bool on_comp_button_press_ext(GdkEventButton* event, MElem* aComp);
	void on_comp_button_press_name(GdkEventButton* event, MElem* aComp);
	void on_comp_button_press(GdkEventButton* event, MElem* aComp);
    protected:
	virtual void on_realize();
	virtual void on_size_allocate(Gtk::Allocation& aAlloc);
	virtual void on_size_request(Gtk::Requisition* aRequisition);
	virtual bool on_drag_motion (const Glib::RefPtr<Gdk::DragContext>& context, int x, int y, guint time);
	virtual bool on_drag_drop(const Glib::RefPtr<Gdk::DragContext>& context, int x, int y, guint time);
	virtual void on_drag_data_received(const Glib::RefPtr<Gdk::DragContext>& context, int x, int y, 
		const Gtk::SelectionData& selection_data, guint info, guint time);
	virtual void on_drag_leave(const Glib::RefPtr<Gdk::DragContext>& context, guint time);
	// From MCrpMgr
	virtual bool IsTypeAllowed(const std::string& aType) const;
    protected:
	virtual void on_node_dropped(const std::string& aUri);
    protected:
	virtual void Construct();
	virtual void DoUdno();
	void Erase();
	void Refresh();
	void do_add_node(const std::string& aName, const std::string& aParentUri, const std::string& aNeighborUri);
	void add_node(const std::string& aParentUri, const std::string& aNeighborUri = string());
	void rename_node(const std::string& aNodeUri, const std::string& aNewName);
	void remove_node(const std::string& aNodeUri);
	void change_content(const string& aNodeUri, const string& aCntPath, const string& aNewContent, bool aRef = false);
	void move_node(const std::string& aNodeUri, const std::string& aDestUri);
	void import(const std::string& aUri);
	void ShowCrpCtxDlg(GdkEventButton* event, MElem* aComp);
	void on_comp_menu_rename();
	void on_comp_menu_remove();
	void on_comp_menu_edit_content();
	void on_comp_menu_save_chromo();
	bool IsCrpLogged(MCrp* aCrp, TLogRecCtg aCtg) const;
	bool DoIsActionSupported(MElem* aComp, const MCrp::Action& aAction);
	void DoOnActionInsert();
	void ShiftCompToEnd(MElem* aOwner, MElem* aComp);
	bool IsMutOwnerBased(TNodeType aMut) const;
    protected:
	MSEnv& mStEnv;
	// Compact representations  provider
	const MCrpProvider& iCrpProv;
	MElem* iElem;
	tCrps iCompRps; // Components representations
	MDrp::tSigCompSelected iSigCompSelected;
	MDrp::tSigCompActivated iSigCompActivated;
	MDrp::tSigDragMotion iSigDragMotion;
	Gtk::Menu iCrpContextMenu;
	MElem* iCompSelected;
	std::map<MCrp::Action, Gtk::Menu_Helpers::MenuElem> iCompMenuElems;
	TDnDTarg iDnDTarg;
	MCrp* iDropBaseCandidate;
	std::string iDndReceivedData;
	MDrp::tSigAttention mSignalAttention;
	bool iReloadRequired;
	MDrp::tSigReloadRequired mSigReloadRequired;

};

class ElemDrp: public MDrp
{
    public:
	static const string& Type();
	static string EType();
    public:
	ElemDrp(MElem* aElem, const MCrpProvider& aCrpProv, MSEnv& aStEnv);
	virtual ~ElemDrp();
	// From MDrp
	virtual void *DoGetObj(const string& aName);
	virtual Gtk::Widget& Widget();
	virtual MElem* Model();
	virtual tSigCompSelected SignalCompSelected();
	virtual tSigCompSelected SignalCompActivated();
	virtual tSigDragMotion SignalDragMotion();
	virtual tSigAttention SignalAttention();
	virtual tSigReloadRequired SignalReloadRequired() {return iRp->mSigReloadRequired;};
	virtual bool IsActionSupported(MElem* aComp, const MCrp::Action& aAction) {return iRp->DoIsActionSupported(aComp, aAction);};
	virtual void Udno();
	virtual void OnActionInsert();
    private:
	ElemDetRp* iRp;
};

#endif

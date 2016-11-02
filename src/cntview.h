#ifndef ___FAP2STU_CTNVIEW_H_
#define ___FAP2STU_CTNVIEW_H_

#include <gtkmm/dialog.h>
#include <gtkmm/treemodel.h>
#include <gtkmm/treeview.h>
#include <gtkmm/liststore.h>
#include <gtkmm/treemodelcolumn.h>
#include <gtkmm/treedragsource.h>
#include <gtkmm/scrolledwindow.h>

#include <melem.h>
#include <menv.h>
#include "msenv.h"
#include "mdesobs.h"


using namespace std;
using namespace Gtk;

// Column record
class ContentTreeClrec: public Gtk::TreeModelColumnRecord
{
    public:
	enum ColumnIndex {
	    KCol_Name = 0,
	    KCol_Value = 1,
	    KCol_Path = 2,
	};
    public:
	Gtk::TreeModelColumn<Glib::ustring> name;
	Gtk::TreeModelColumn<Glib::ustring> value;
	Gtk::TreeModelColumn<Glib::ustring> path;
    public:
	ContentTreeClrec() { add(name); add(value); add(path);};
};

// Tree model
#if GLIB_CHECK_VERSION(2, 42, 0)
class ContentTreeMdl: public Gtk::TreeModel, public Gtk::TreeDragSource, public Glib::Object
#else
class ContentTreeMdl: public Glib::Object, public Gtk::TreeModel, public Gtk::TreeDragSource
#endif
{
    public:
	class GlueItem
	{
	    public:
		GlueItem(const string& aContName): mContName(aContName) {};
		string mContName;
	};
    public:
	static Glib::RefPtr<ContentTreeMdl> create(const MElem& aAgent);
	ContentTreeMdl(const MElem& aAgent);
	virtual ~ContentTreeMdl();
	static const ContentTreeClrec& ColRec() {return iColRec;};
	// Model events handlers
	void on_comp_deleting(MElem* aComp);
	void on_comp_adding(MElem* aComp);
	void on_comp_changed(MElem* aComp);
    protected:
	// From Gtk::TreeModel
	virtual Gtk::TreeModelFlags get_flags_vfunc() const;
	virtual int get_n_columns_vfunc() const;
	virtual GType get_column_type_vfunc(int index) const;
	virtual int iter_n_root_children_vfunc() const;
	virtual int iter_n_children_vfunc(const iterator& iter) const;
	virtual bool iter_children_vfunc(const iterator& parent, iterator& iter) const;
	virtual bool iter_has_child_vfunc(const iterator& iter) const;
	virtual bool iter_nth_child_vfunc(const iterator& parent, int n, iterator& iter) const;
	virtual bool iter_nth_root_child_vfunc(int n, iterator& iter) const;
	virtual bool iter_parent_vfunc(const iterator& child, iterator& iter) const;
	virtual Path get_path_vfunc(const iterator& iter) const;
	virtual bool get_iter_vfunc(const Path& path, iterator& iter) const;
	virtual bool iter_is_valid(const iterator& iter) const;
	virtual void get_value_vfunc(const TreeModel::iterator& iter, int column, Glib::ValueBase& value) const;
	virtual bool iter_next_vfunc(const iterator& iter, iterator& iter_next) const;
	virtual void set_value_impl(const iterator& row, int column, const Glib::ValueBase& value);
	virtual void on_row_changed (const TreeModel::Path& path, const TreeModel::iterator& iter);
	// From Gtk::TreeDragSource
	virtual bool row_draggable_vfunc(const TreeModel::Path& path) const;
	virtual bool drag_data_get_vfunc(const TreeModel::Path& path, Gtk::SelectionData& selection_data) const;
	virtual bool drag_data_delete_vfunc(const TreeModel::Path& path);
    private:
	bool IsIterValid(const iterator& iter) const;
	void UpdateStamp();
	string get_next_comp(const string& aComp);
	GlueItem* AddGlueItem(const string& aContName) const;
	int GetRowIndex(const iterator& iter) const;
	int GetCompInd(const string& aComp) const;
    private:
	// Agent
	const MElem& mAgent;
	// Column record, contains info of column types
	static const ContentTreeClrec iColRec;
	// Stamp, is used for securing iterator's associating to model
	int iStamp;
	typedef list<GlueItem*> tGiList;
	mutable tGiList iGiList;
};

// Content navigation widget
class NaviContent: public Gtk::TreeView, public MAgentObserver
{
    public:
	typedef sigc::signal<void, string> tSigCompSelected;
	typedef sigc::signal<void, string> tSigCompActivated;
    public:
	NaviContent(MElem& aAgent);
	virtual ~NaviContent();
	// TODO [YB] To move out to iface like MHierNavigator, and implement, the same for MDrp
	virtual tSigCompSelected SignalCompSelected();
	virtual tSigCompActivated SignalCompActivated();
	// From MAgentObserver
	virtual void OnCompDeleting(MElem& aComp, TBool aSoft = ETrue);
	virtual void OnCompAdding(MElem& aComp);
	virtual TBool OnCompChanged(MElem& aComp, const string& aContName = string());
	virtual TBool OnChanged(MElem& aComp);
	virtual TBool OnCompRenamed(MElem& aComp, const string& aOldName);
	virtual MIface* Call(const string& aSpec, string& aRes);
	virtual string Mid() const;
    protected:
	virtual void on_drag_begin(const Glib::RefPtr<Gdk::DragContext>& context);
	virtual void on_drag_data_get(const Glib::RefPtr<Gdk::DragContext >& context, Gtk::SelectionData& selection_data, guint info, guint time);
	virtual bool on_button_press_event(GdkEventButton* event);
	virtual void on_row_activated(const TreeModel::Path& path, TreeViewColumn* column);
    protected:
	void set_source_row(const Glib::RefPtr<Gdk::DragContext>& context, Glib::RefPtr<Gtk::TreeModel>& model, Gtk::TreePath& source_row);
	void UnsetAgent();
	void SetAgent();
    private:
	MElem& mAgent;
	int iPressX, iPressY;
	tSigCompSelected iSigCompSelected;
	tSigCompActivated iSigCompActivated;
};

// Add content dialog 
class ContentAddDlg: public Gtk::Dialog
{
    public:
	enum {
	    EActionCancel,
	    EActionOK
	};
    public:
	ContentAddDlg(const string& aTitle);
	void GetData(string& aName, string &aValue);
    protected:
	Gtk::HBox* mNameArea;
	Gtk::HBox* mValueArea;
	Gtk::Label* mNameLabel;
	Gtk::Label* mValueLabel;
	Gtk::TextView* mName;
	Gtk::TextView* mValue;
};


// Content select dialog 
class ContentSelectDlg: public Gtk::Dialog
{
    public:
	enum {
	    EActionCancel,
	    EActionOK
	};
	enum {
	    ERsp_Add = 1
	} TResponse;
    public:
	ContentSelectDlg(const string& aTitle, MElem& aAgent);
	~ContentSelectDlg();
	void GetData(string& aData);
    protected:
	void OnModelRowActivated(const TreeModel::Path& aPath, TreeViewColumn* aColumn); 
	void OnColumnChanged(); 
	void OnCursorChanged(); 
	void OnAddButtonPressed(); 
	void OnRmButtonPressed(); 
	virtual void on_response(int response_id);
    protected:
	// Navigation tree of content
	NaviContent* mNavi;
	ScrolledWindow mSw;
	MElem& mAgent;
	Button* mBtnAdd;
	Button* mBtnRm;
};


#endif


#ifndef __FAPSTU_GTK_NAVI_H
#define __FAPSTU_GTK_NAVI_H

#include <gtkmm/notebook.h>
#include <gtkmm/treemodel.h>
#include <gtkmm/treeview.h>
#include <gtkmm/liststore.h>
#include <gtkmm/treemodelcolumn.h>
#include <gtkmm/treedragsource.h>

#include <melem.h>
#include <menv.h>

using namespace std;
using namespace Gtk;

// Native nodes tree column record
class NatnTreeClrec: public Gtk::TreeModelColumnRecord
{
    public:
	Gtk::TreeModelColumn<Glib::ustring> name;
    public:
	NatnTreeClrec() { add(name);};
};

// Native nodes tree model
class NatnTreeMdl: public Glib::Object, public Gtk::TreeModel, public Gtk::TreeDragSource
{
    public:
	class GlueItem
	{
	    public:
		GlueItem(int aRowIndex): iRowIndex(aRowIndex) {};
		int iRowIndex;
	};

    public:
	static Glib::RefPtr<NatnTreeMdl> create(MProvider* aNatnProv);
	NatnTreeMdl(MProvider* aNatnProv);
	virtual ~NatnTreeMdl();
	NatnTreeClrec& ColRec() {return iColRec;};
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
	bool iter_next_vfunc(const iterator& iter, iterator& iter_next) const;
	// From Gtk::TreeDragSource
	virtual bool row_draggable_vfunc(const TreeModel::Path& path) const;
	virtual bool drag_data_get_vfunc(const TreeModel::Path& path, Gtk::SelectionData& selection_data) const;
	virtual bool drag_data_delete_vfunc(const TreeModel::Path& path);
    private:
	bool IsIterValid(const iterator& iter) const;
	GlueItem* AddGlueItem(int aRowIndex) const;
	int GetRowIndex(const iterator& iter) const;
    private:
	// Native nodes provider
	MProvider* iNatnProv;
	// Column record, contains info of column types
	NatnTreeClrec iColRec;
	// Stamp, is used for securing iterator's associating to model
	int iStamp;
	// Nodes info
	vector<string> iNodesInfo;
	// Glue Items list
	typedef std::list<GlueItem*> tGiList;
	mutable tGiList iGiList;
};

// Native nodes navigation widget
class NaviNatN: public Gtk::TreeView
{
    public:
	NaviNatN();
	virtual ~NaviNatN();
	void SetDesEnv(MEnv* aDesEnv);
    protected:
	virtual void on_drag_begin(const Glib::RefPtr<Gdk::DragContext>& context);
	virtual void on_drag_data_get(const Glib::RefPtr<Gdk::DragContext >& context, Gtk::SelectionData& selection_data, guint info, guint time);
	virtual bool on_button_press_event(GdkEventButton* event);
    protected:
	void set_source_row(const Glib::RefPtr<Gdk::DragContext>& context, Glib::RefPtr<Gtk::TreeModel>& model, Gtk::TreePath& source_row);
    private:
	// DES environment
	MEnv* iDesEnv; 
	int iPressX, iPressY;
};



// Current hier tree column record
class HierTreeClrec: public Gtk::TreeModelColumnRecord
{
    public:
	enum ColumnIndex {
	    KCol_Name = 0,
	    KCol_Elem = 1,
	};
    public:
	Gtk::TreeModelColumn<Glib::ustring> name;
	Gtk::TreeModelColumn<void*> elem;
    public:
	HierTreeClrec() { add(name); add(elem);};
};

// Current hier tree model
class HierTreeMdl: public Glib::Object, public Gtk::TreeModel, public Gtk::TreeDragSource, 
    public MCompsObserver
{
    public:
	static Glib::RefPtr<HierTreeMdl> create(MEnv* aDesEnv);
	HierTreeMdl(MEnv* aDesEnv);
	virtual ~HierTreeMdl();
	static const HierTreeClrec& ColRec() {return iColRec;};
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
	bool iter_next_vfunc(const iterator& iter, iterator& iter_next) const;
	// From Gtk::TreeDragSource
	virtual bool row_draggable_vfunc(const TreeModel::Path& path) const;
	virtual bool drag_data_get_vfunc(const TreeModel::Path& path, Gtk::SelectionData& selection_data) const;
	virtual bool drag_data_delete_vfunc(const TreeModel::Path& path);
	// From MCompsObserver
	virtual void OnCompDeleting(Elem& aComp);
	virtual void OnCompAdding(Elem& aComp);
	virtual void OnCompChanged(Elem& aComp);
    private:
	bool IsIterValid(const iterator& iter) const;
	void UpdateStamp();
	Elem* get_next_comp(Elem* aComp);
    private:
	// Provider provider
	MEnv* iDesEnv;
	// Column record, contains info of column types
	static const HierTreeClrec iColRec;
	// Root of hier
	Elem* iRoot;
	// Stamp, is used for securing iterator's associating to model
	int iStamp;
};

// Current hier navigation widget
class NaviHier: public Gtk::TreeView
{
    public:
	typedef sigc::signal<void, Elem*> tSigCompSelected;
    public:
	NaviHier();
	virtual ~NaviHier();
	void SetDesEnv(MEnv* aDesEnv);
	// TODO [YB] To move out to iface like MHierNavigator, and implement, the same for MDrp
	virtual tSigCompSelected SignalCompSelected();
    protected:
	virtual void on_drag_begin(const Glib::RefPtr<Gdk::DragContext>& context);
	virtual void on_drag_data_get(const Glib::RefPtr<Gdk::DragContext >& context, Gtk::SelectionData& selection_data, guint info, guint time);
	virtual bool on_button_press_event(GdkEventButton* event);
	virtual void on_row_activated(const TreeModel::Path& path, TreeViewColumn* column);
    protected:
	void set_source_row(const Glib::RefPtr<Gdk::DragContext>& context, Glib::RefPtr<Gtk::TreeModel>& model, Gtk::TreePath& source_row);
    private:
	// DES environment
	MEnv* iDesEnv; 
	int iPressX, iPressY;
	tSigCompSelected iSigCompSelected;
};



// Modules tree column record
class ModulesTreeClrec: public Gtk::TreeModelColumnRecord
{
    public:
	Gtk::TreeModelColumn<Glib::ustring> name;
    public:
	ModulesTreeClrec() { add(name);};
};

// Modules navigation widget
class NaviModules: public Gtk::TreeView
{
    public:
	NaviModules();
	virtual ~NaviModules();
	void SetDesEnv(MEnv* aDesEnv);
    protected:
	virtual void on_drag_begin(const Glib::RefPtr<Gdk::DragContext>& context);
	virtual void on_drag_data_get(const Glib::RefPtr<Gdk::DragContext >& context, Gtk::SelectionData& selection_data, guint info, guint time);
	virtual bool on_button_press_event(GdkEventButton* event);
    protected:
	void set_source_row(const Glib::RefPtr<Gdk::DragContext>& context, Glib::RefPtr<Gtk::TreeModel>& model, Gtk::TreePath& source_row);
	static int FilterModulesDirEntries(const struct dirent *aEntry);
    private:
	// DES environment
	MEnv* iDesEnv; 
	// Column record, contains info of column types
	static const ModulesTreeClrec iColRec;
	int iPressX, iPressY;
};

// Navigation widget
class Navi: public Gtk::Notebook
{
    public:
	Navi();
	virtual ~Navi();
	void SetDesEnv(MEnv* aDesEnv);
	NaviHier& NatHier();
    private:
	// DES environment
	MEnv* iDesEnv; 
	// Navigation tree of native nodes
	NaviNatN* iNatn;
	// Navigation tree of modules
	NaviModules* iNatMod;
	// Navigation tree of current hier
	NaviHier* iNatHier;
};


#endif


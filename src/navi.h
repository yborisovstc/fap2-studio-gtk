
#ifndef __FAPSTU_GTK_NAVI_H
#define __FAPSTU_GTK_NAVI_H

#include <gtkmm/notebook.h>
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
#include "chromoview.h"

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
	NaviNatN(MMdlObserver* aDesObs);
	virtual ~NaviNatN();
	void SetDesEnv(MEnv* aDesEnv);
    protected:
	virtual void on_drag_begin(const Glib::RefPtr<Gdk::DragContext>& context);
	virtual void on_drag_data_get(const Glib::RefPtr<Gdk::DragContext >& context, Gtk::SelectionData& selection_data, guint info, guint time);
	virtual bool on_button_press_event(GdkEventButton* event);
    protected:
	void set_source_row(const Glib::RefPtr<Gdk::DragContext>& context, Glib::RefPtr<Gtk::TreeModel>& model, Gtk::TreePath& source_row);
	void on_des_env_changed();
    private:
	// DES observer
	MMdlObserver* iDesObs;
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
class HierTreeMdl: public Glib::Object, public Gtk::TreeModel, public Gtk::TreeDragSource/*, public MCompsObserver*/
{
    public:
	static Glib::RefPtr<HierTreeMdl> create(MEnv* aDesEnv);
	HierTreeMdl(MEnv* aDesEnv);
	virtual ~HierTreeMdl();
	static const HierTreeClrec& ColRec() {return iColRec;};
	// Model events handlers
	void on_comp_deleting(Elem* aComp);
	void on_comp_adding(Elem* aComp);
	void on_comp_changed(Elem* aComp);
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
	virtual TBool OnCompChanged(Elem& aComp);
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
	typedef sigc::signal<void, Elem*> tSigCompActivated;
    public:
	NaviHier(MMdlObserver* aDesObs);
	virtual ~NaviHier();
	void SetDesEnv(MEnv* aDesEnv);
	// TODO [YB] To move out to iface like MHierNavigator, and implement, the same for MDrp
	virtual tSigCompSelected SignalCompSelected();
	virtual tSigCompActivated SignalCompActivated();
    protected:
	virtual void on_drag_begin(const Glib::RefPtr<Gdk::DragContext>& context);
	virtual void on_drag_data_get(const Glib::RefPtr<Gdk::DragContext >& context, Gtk::SelectionData& selection_data, guint info, guint time);
	virtual bool on_button_press_event(GdkEventButton* event);
	virtual void on_row_activated(const TreeModel::Path& path, TreeViewColumn* column);
    protected:
	void set_source_row(const Glib::RefPtr<Gdk::DragContext>& context, Glib::RefPtr<Gtk::TreeModel>& model, Gtk::TreePath& source_row);
	void on_des_env_changed();
	void on_des_root_added();
    private:
	// DES observer
	MMdlObserver* iDesObs;
	// DES environment
	MEnv* iDesEnv; 
	int iPressX, iPressY;
	tSigCompSelected iSigCompSelected;
	tSigCompActivated iSigCompActivated;
	bool iRootAdded;
};



// Modules tree column record
class ModulesTreeClrec: public Gtk::TreeModelColumnRecord
{
    public:
	enum ColumnIndex {
	    KCol_Name = 0,
	    KCol_Uri = 1,
	};
    public:
	Gtk::TreeModelColumn<Glib::ustring> name;
	Gtk::TreeModelColumn<Glib::ustring> uri;
    public:
	ModulesTreeClrec() { add(name);add(uri);};
};

// Modules tree model
class ModulesTreeMdl: public Glib::Object, public Gtk::TreeModel, public Gtk::TreeDragSource
{
    public:
	static Glib::RefPtr<ModulesTreeMdl> create(MEnv* aDesEnv);
	ModulesTreeMdl(MEnv* aDesEnv);
	virtual ~ModulesTreeMdl();
	static const ModulesTreeClrec& ColRec() {return iColRec;};
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
	void UpdateStamp();
	Chromo* create_modules_chromo(const string& aModName) const;
	ChromoNode create_node_from_iter(const iterator& iter) const;
	Chromo* get_chromo_from_iter(const iterator& iter) const;
	void set_iter(iterator& iter, const ChromoNode& node, const Chromo* chromo) const;
	void set_iter(iterator& iter, const ChromoNode& node, const iterator& chromo_src) const;
	int get_next_module_ind(const string& aModName) const;
	bool is_root_iter(const iterator& iter) const;
    private:
	// Provider provider
	MEnv* iDesEnv;
	// Column record, contains info of column types
	static const ModulesTreeClrec iColRec;
	// Stamp, is used for securing iterator's associating to model
	int iStamp;
};

// Modules navigation widget
class NaviModules: public Gtk::TreeView
{
    public:
	NaviModules(MSEnv& aStEnv, MMdlObserver* aDesObs);
	virtual ~NaviModules();
	void SetDesEnv(MEnv* aDesEnv);
    protected:
	virtual void on_drag_begin(const Glib::RefPtr<Gdk::DragContext>& context);
	virtual void on_drag_data_get(const Glib::RefPtr<Gdk::DragContext >& context, Gtk::SelectionData& selection_data, guint info, guint time);
	virtual bool on_button_press_event(GdkEventButton* event);
    protected:
	void set_source_row(const Glib::RefPtr<Gdk::DragContext>& context, Glib::RefPtr<Gtk::TreeModel>& model, Gtk::TreePath& source_row);
	static int FilterModulesDirEntries(const struct dirent *aEntry);
	void on_des_env_changed();
    private:
	// Environment
	MSEnv& iStEnv;
	// DES observer
	MMdlObserver* iDesObs;
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
	Navi(MSEnv& aStEnv, MMdlObserver* aDesObs);
	virtual ~Navi();
	void SetDesEnv(MEnv* aDesEnv);
	NaviHier& NatHier();
	ChromoTree& ChromoTreeView() {return *iChromoTree;};
    private:
	// Environment
	MSEnv& iStEnv;
	// DES observer
	MMdlObserver* iDesObs;
	// Navigation tree of native nodes
	NaviNatN* iNatn;
	ScrolledWindow iNatnSw;
	// Navigation tree of modules
	NaviModules* iNatMod;
	ScrolledWindow iNatModSw;
	// Navigation tree of current hier
	NaviHier* iNatHier;
	ScrolledWindow iNatHierSw;
	// Navigation tree of chromo
	ChromoTree* iChromoTree;
	ScrolledWindow iChromoSw;
};


#endif


#ifndef __FAPSTU_GTK_CHROMOVIEW_H
#define __FAPSTU_GTK_CHROMOVIEW_H

#include <gtkmm/treemodel.h>
#include <gtkmm/treeview.h>
#include <gtkmm/liststore.h>
#include <gtkmm/treemodelcolumn.h>
#include <gtkmm/treedragsource.h>
#include <gtkmm/scrolledwindow.h>

#include <melem.h>
#include <menv.h>
#include "mdesobs.h"


using namespace std;
using namespace Gtk;

// Chromo tree column record
class ChromoTreeClrec: public Gtk::TreeModelColumnRecord
{
    public:
	enum ColumnIndex {
	    KCol_Name = 0,
	    KCol_Attr = 1,
	};
    public:
	Gtk::TreeModelColumn<Glib::ustring> name;
	Gtk::TreeModelColumn<Glib::ustring> attr;
    public:
	ChromoTreeClrec() { add(name); add(attr);};
};

// Chromo tree model
class ChromoTreeMdl: public Glib::Object, public Gtk::TreeModel, public Gtk::TreeDragSource
{
    public:
	static Glib::RefPtr<ChromoTreeMdl> create(Elem* aRoot, MEnv* aDesEnv);
	ChromoTreeMdl(Elem* aRoot, MEnv* aDesEnv);
	virtual ~ChromoTreeMdl();
	static const ChromoTreeClrec& ColRec() {return iColRec;};
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
	virtual void OnCompChanged(Elem& aComp);
    private:
	bool IsIterValid(const iterator& iter) const;
	void UpdateStamp();
	Elem* get_next_comp(Elem* aComp);
    private:
	// Provider provider
	MEnv* iDesEnv;
	// Column record, contains info of column types
	static const ChromoTreeClrec iColRec;
	// Root of hier
	Elem* iRoot;
	ChromoNode iRnode;
	// Stamp, is used for securing iterator's associating to model
	int iStamp;
};

// Chromo navigation widget
class ChromoTree: public Gtk::TreeView
{
    public:
	typedef sigc::signal<void, Elem*> tSigCompSelected;
    public:
	ChromoTree(MMdlObserver* aDesObs);
	virtual ~ChromoTree();
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
	void on_des_env_changed();
	void on_des_root_added();
    private:
	// DES observer
	MMdlObserver* iDesObs;
	// DES environment
	MEnv* iDesEnv; 
	int iPressX, iPressY;
	tSigCompSelected iSigCompSelected;
	bool iRootAdded;
};

#endif


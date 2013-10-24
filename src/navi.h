
#ifndef __FAPSTU_GTK_NAVI_H
#define __FAPSTU_GTK_NAVI_H

#include <gtkmm/notebook.h>
#include <gtkmm/treemodel.h>
#include <gtkmm/treeview.h>
#include <gtkmm/treemodelcolumn.h>
#include <gtkmm/treedragsource.h>

#include <menv.h>

using namespace std;

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


// Navigation widget
class Navi: public Gtk::Notebook
{
    public:
	Navi();
	virtual ~Navi();
	void SetDesEnv(MEnv* aDesEnv);
    private:
	// DES environment
	MEnv* iDesEnv; 
	// Navigation tree of native nodes
	NaviNatN* iNatn;
};


#endif

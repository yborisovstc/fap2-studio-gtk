
#ifndef __FAPSTU_GTK_LOGVIEW_H
#define __FAPSTU_GTK_LOGVIEW_H

#include <gtkmm/textview.h>
#include <gtkmm/treeview.h>
#include <giomm/file.h>
#include "mdesobs.h"
#include "mdeslog.h"

using namespace Gtk;
using namespace Gio;
using namespace std;

// Log view
class LogView: public TextView
{
    public:
	LogView();
	virtual ~LogView();
	void SetLogFileName(const string& aLogFileName);
    private:
	static gboolean handle_timer_event(gpointer data);
	void on_changed(const Glib::RefPtr<File>& file, const Glib::RefPtr<File>& other_file, FileMonitorEvent event_type);
	static void handle_filemon_event(GFileMonitor *monitor, GFile *file, GFile *other_file, GFileMonitorEvent event_type, gpointer user_data);
	void OnFileChanged(GFileMonitorEvent event);
	void OnFileContentChanged();
	void OnFileCreated();
	void OnFileDeleted();
    private:
	static const int KLogViewBufLen;
	string iLogFileName;
	Glib::RefPtr<File> iLogFile;
	Glib::RefPtr<FileMonitor> iFileMon;
	Glib::RefPtr<FileInputStream> iInpStream;
	Glib::RefPtr<TextBuffer> iBuffer;
	char* iLogBuffer;
};


// Log list view model column record
class LogListClrec: public Gtk::TreeModel::ColumnRecord
{
    public:
	enum ColumnIndex {
	    ECol_Ctg = 0,
	    ECol_Mnode = 1,
	    ECol_Mutid = 2,
	    ECol_Content = 3,
	};
    public:
	Gtk::TreeModelColumn<Glib::ustring> ctg;
	Gtk::TreeModelColumn<Glib::ustring> mnode;
	Gtk::TreeModelColumn<int> mutid;
	Gtk::TreeModelColumn<Glib::ustring> content;
    public:
	LogListClrec() { add(ctg); add(mnode); add(mutid); add(content);};
};

// Des log
class SDesLog: public MDesLog
{
    friend class LogViewL;
    public:
	SDesLog() {};
	virtual ~SDesLog();
	// From MDesLog
	virtual const TLog& LogData() const;
	virtual bool IsNodeLogged(Elem* aNode, MLogRec::TLogRecCtg aCtg) const;
    protected:
	TLog mLog;
};

// Log view based on log records list
class LogViewL: public TreeView
{
    public:
	typedef sigc::signal<void, const string&, int> tSigLogRecActivated;
    public:
	LogViewL(MMdlObserver* aDesObs);
	virtual ~LogViewL();
	void SetDesEnv(MEnv* aDesEnv);
	MDesLog* GetDesLog() {return &mDesLog;};
	void Select(Elem* aNode, MLogRec::TLogRecCtg aCtg);
	tSigLogRecActivated SignalLogRecActivated() {return mSigLogRecActivated;};
    protected:
	void on_log_added(MLogRec::TLogRecCtg aCtg, Elem* aNode, int aMutId, const std::string& aContent);
	void on_des_env_changed();
	static const string& CtgName(MLogRec::TLogRecCtg aCtg); 
	virtual void on_row_activated(const TreeModel::Path& path, TreeViewColumn* column);
    private:
	// DES observer
	MMdlObserver* iDesObs;
	// DES environment
	MEnv* iDesEnv; 
	// Column record, contains info of column types
	LogListClrec iColRec;
	// Des log
	SDesLog mDesLog;
	tSigLogRecActivated mSigLogRecActivated;
};

#endif

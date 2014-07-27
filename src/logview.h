
#ifndef __FAPSTU_GTK_LOGVIEW_H
#define __FAPSTU_GTK_LOGVIEW_H

#include <gtkmm/textview.h>
#include <gtkmm/treeview.h>
#include <giomm/file.h>
#include "mdesobs.h"

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
	Gtk::TreeModelColumn<Glib::ustring> ctg;
	Gtk::TreeModelColumn<Glib::ustring> mnode;
	Gtk::TreeModelColumn<Glib::ustring> content;
    public:
	LogListClrec() { add(ctg); add(mnode); add(content);};
};

// Log view based on log records list
class LogViewL: public TreeView
{
    public:
	LogViewL(MMdlObserver* aDesObs);
	virtual ~LogViewL();
	void SetDesEnv(MEnv* aDesEnv);
    protected:
	void on_log_added(MLogRec::TLogRecCtg aCtg, Elem* aNode, const std::string& aContent);
	void on_des_env_changed();
    private:
	// DES observer
	MMdlObserver* iDesObs;
	// DES environment
	MEnv* iDesEnv; 
	// Column record, contains info of column types
	LogListClrec iColRec;
};

#endif

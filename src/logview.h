
#ifndef __FAPSTU_GTK_LOGVIEW_H
#define __FAPSTU_GTK_LOGVIEW_H

#include <gtkmm/textview.h>
#include <giomm/file.h>

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

#endif

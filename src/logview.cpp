
#include "logview.h"

const int LogView::KLogViewBufLen = 4000;

LogView::LogView(): TextView(), iFileMon(NULL), iInpStream(NULL)
{
    set_editable(false);
    // Buffer
    iBuffer = TextBuffer::create();
    set_buffer(iBuffer);
    //iLogBuffer.reserve(KLogViewBufLen);
    iLogBuffer = (char*) g_malloc(KLogViewBufLen);
}

LogView::~LogView()
{
}


void LogView::SetLogFileName(const string& aLogFileName)
{
    iLogFileName = aLogFileName;
    // Open log
    iLogFile = File::create_for_path(iLogFileName);
    // Create monitor
    iFileMon = iLogFile->monitor_file(Gio::FILE_MONITOR_NONE);
    iFileMon->signal_changed().connect(sigc::mem_fun(this, &LogView::on_changed));
    // Open file for reading
    GError* err;
    iInpStream = iLogFile->read();
}

void LogView::on_changed(const Glib::RefPtr<File>& file, const Glib::RefPtr<File>& other_file, FileMonitorEvent event)
{
    if (event == Gio::FILE_MONITOR_EVENT_CHANGED) {
	OnFileContentChanged();
    }
    else if (event == Gio::FILE_MONITOR_EVENT_DELETED) {
	OnFileDeleted();
    }
    else if (event == Gio::FILE_MONITOR_EVENT_CREATED) {
	iInpStream = iLogFile->read();
    }
}

void LogView::OnFileDeleted()
{
    iInpStream.reset();
    // Clean buffer
    iBuffer->erase(iBuffer->begin(), iBuffer->end());
}

void LogView::OnFileContentChanged()
{
    // Get the new content from file 
    iLogBuffer[0] = 0x00;
    gssize size = 1;
    TextBuffer::iterator it = iBuffer->end();
    while (size != 0) {
	size = iInpStream->read(iLogBuffer, KLogViewBufLen);
	if (size > 0) {
	    it = iBuffer->insert(iBuffer->end(), (const gchar*) iLogBuffer, &iLogBuffer[size]);
	    // TODO YB hack
	    while(gtk_events_pending()) gtk_main_iteration();
	    gboolean res = scroll_to(it, 0.0);
	}
    }
}



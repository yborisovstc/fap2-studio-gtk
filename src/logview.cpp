
#include "logview.h"
#include <gtkmm/liststore.h>
#include <gtkmm/treepath.h>

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


// Des log
SDesLog::~SDesLog()
{
}

const MDesLog::TLog& SDesLog::LogData() const
{
    return mLog;
}

bool SDesLog::IsNodeLogged(MElem* aNode, MLogRec::TLogRecCtg aCtg) const
{
    return mLog.count(MDesLog::TLogKey(aNode, aCtg));
}

// Log view based on log records list
LogViewL::LogViewL(MMdlObserver* aDesObs): iDesObs(aDesObs)
{
    set_headers_visible(true);
    Glib::RefPtr<TreeSelection> sel = get_selection();
    sel->set_mode(SELECTION_MULTIPLE);
    SetDesEnv(iDesObs->DesEnv());
    iDesObs->SignalDesEnvChanged().connect(sigc::mem_fun(*this, &LogViewL::on_des_env_changed));
    iDesObs->SignalLogAdded().connect(sigc::mem_fun(*this, &LogViewL::on_log_added));
}

LogViewL::~LogViewL()
{
}

void LogViewL::SetDesEnv(MEnv* aDesEnv)
{
    if (aDesEnv != iDesEnv) {
	unset_model();
	remove_all_columns();
	Glib::RefPtr<TreeModel> curmdl = get_model();
	curmdl.reset();
	iDesEnv = aDesEnv;
	mDesLog.mLog.clear();
	if (iDesEnv != NULL) {
	    Glib::RefPtr<ListStore> mdl = ListStore::create(iColRec);
	    set_model(mdl);
	    append_column( "Time", iColRec.timestamp);
	    append_column( "Type", iColRec.ctg);
	    append_column( "Node", iColRec.mnode);
	    append_column( "Content", iColRec.content);
	}
    }
}

void LogViewL::on_des_env_changed()
{
    SetDesEnv(iDesObs->DesEnv());
}

const string KCtgErr = "ERR";
const string KCtgInfo = "INF";
const string KCtgWrn = "WRN";
const string KCtgDbg = "DBG";

const string& LogViewL::CtgName(MLogRec::TLogRecCtg aCtg) 
{
    if (aCtg == MLogRec::EErr) return KCtgErr;
    else if (aCtg == MLogRec::EInfo) return KCtgInfo;
    else if (aCtg == MLogRec::EWarn) return KCtgWrn;
    else if (aCtg == MLogRec::EDbg) return KCtgDbg;
    else __ASSERT(false);
}

void LogViewL::on_log_added(long aTimeStamp, MLogRec::TLogRecCtg aCtg, const MElem* aNode, int aMutId, const std::string& aContent)
{
    Glib::RefPtr<TreeModel> mdl = get_model();
    //ListStore* lmdl = (ListStore*) mdl.operator->();
    Glib::RefPtr<ListStore> lsmdl = Glib::RefPtr<ListStore>::cast_dynamic<TreeModel>(mdl);
    TreeIter it = lsmdl->append();
    it->set_value(iColRec.timestamp, Glib::ustring::compose("%1", aTimeStamp));
    it->set_value(iColRec.ctg, Glib::ustring(CtgName(aCtg).c_str()));
    GUri fullpath;
    if (aNode != NULL) {
	aNode->GetUri(fullpath);
    }
    it->set_value(iColRec.mnode, Glib::ustring(fullpath.GetUri(ETrue).c_str()));
    it->set_value(iColRec.mutid, aMutId);
    it->set_value(iColRec.content, Glib::ustring(aContent.c_str()));
    mDesLog.mLog.insert(MDesLog::TLogVal(MDesLog::TLogKey(aNode, aCtg), MDesLog::TLogData(aContent)));
}

void LogViewL::Select(MElem* aNode, MLogRec::TLogRecCtg aCtg)
{
    get_selection()->unselect_all();
    GUri upath;
    aNode->GetUri(upath);
    Glib::ustring path = upath.GetUri(true);
    TreeIter it = get_model()->get_iter(TreePath("0"));
    for (; it.operator bool(); it++) {
	Glib::ustring ctg = it->get_value(iColRec.ctg);
	Glib::ustring cpath = it->get_value(iColRec.mnode);
	if (ctg == Glib::ustring(CtgName(aCtg)) && path == cpath) {
	    get_selection()->select(it);
	}
    }
}

void LogViewL::on_row_activated(const TreeModel::Path& path, TreeViewColumn* column)
{
    TreeIter it = get_model()->get_iter(path);
    TreeModel::Row row = *it;
    int mutid = row.get_value(iColRec.mutid);
    Glib::ustring cpath = it->get_value(iColRec.mnode);
    mSigLogRecActivated.emit(cpath, mutid);
}


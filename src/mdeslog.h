
#ifndef __FAP2STU_GTK_MDESLOG_H
#define __FAP2STU_GTK_MDESLOG_H

// DES log

class MDesLog
{
    public:
	struct TLogData {
	    string content;  
	    TLogData(const string& c): content(c) {}
	    bool operator< (const TLogData& r) const { return content < r.content;} 
	}; 
	typedef pair<const MElem*, TLogRecCtg> TLogKey;
	typedef pair<TLogKey, TLogData> TLogVal;
    public:
	typedef std::multimap<TLogKey, TLogData> TLog;
    public:
	virtual const TLog& LogData() const = 0;
	virtual bool IsNodeLogged(MElem* aNode, TLogRecCtg aCtg) const = 0;
};

#endif

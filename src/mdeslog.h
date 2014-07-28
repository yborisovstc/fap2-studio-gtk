
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
    public:
	typedef std::map<Elem*, TLogData> TLog;
    public:
	virtual const TLog& Errors() const = 0;
	virtual const TLog& Warnings() const = 0;
};

#endif

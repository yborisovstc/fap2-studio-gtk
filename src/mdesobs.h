
#ifndef __FAP2STU_MDESOBS_H
#define __FAP2STU_MDESOBS_H

#include <string>
#include <elem.h>
#include "mbase.h"


// Model observer interface

class MMdlObserver
{
    public:
	static std::string Type() { return "MMdlObserver";};
    public:
	typedef sigc::signal<void> tSigSystemChanged;
	typedef sigc::signal<void> tSigDesEnvChanged;
	typedef sigc::signal<void> tSigSystemCreated;
	typedef sigc::signal<void, MElem*> tSigCompDeleted;
	typedef sigc::signal<void, MElem*> tSigCompAdded;
	typedef sigc::signal<void, MElem*> tSigCompChanged;
	typedef sigc::signal<void, MElem*> tSigContentChanged;
	typedef sigc::signal<void, MElem*, const std::string&> tSigCompRenamed;
	typedef sigc::signal<void, long, TLogRecCtg, const MElem*, int, const std::string&> tSigLogAdded;
	typedef sigc::signal<void, const TLog&> tSigTLogAdded;
    public:
	virtual tSigSystemChanged SignalSystemChanged () = 0;
	virtual tSigDesEnvChanged SignalDesEnvChanged () = 0;
	virtual tSigCompDeleted SignalCompDeleted() = 0;
	virtual tSigCompAdded SignalCompAdded() = 0;
	virtual tSigCompChanged SignalCompChanged() = 0;
	virtual tSigCompRenamed SignalCompRenamed() = 0;
	virtual tSigContentChanged SignalContentChanged() = 0;
	virtual tSigLogAdded SignalLogAdded() = 0;
	virtual tSigTLogAdded SignalTLogAdded() = 0;
	virtual tSigSystemCreated SignalSystemCreated() = 0;
	virtual MEnv* DesEnv() = 0;
};


#endif


#ifndef __FAP2STU_DATACRP_H
#define __FAP2STU_DATACRP_H

#include "incapscrp.h"

//  Value representation
class ValueRp: public Gtk::Label
{
    public:
	ValueRp(Elem* aModel);
    protected:
	Elem* iElem;
};

// Data incaps compact representation
// Similar to incaps but displaying value of property "Value" also
class DataCrp: public IncapsCrp
{
    public:
	static const string& Type();
	static string EType();
    public:
	DataCrp(Elem* aElem);
	virtual ~DataCrp();
    protected:
	virtual void Construct();
	virtual void on_size_allocate(Gtk::Allocation& 	aAlloc);
	virtual void on_size_request(Gtk::Requisition* aRequisition);
	virtual void *DoGetObj(const string& aName);
	// From MCrp
	virtual void GetContentUri(GUri& aUri);
	virtual bool IsActionSupported(Action aAction);
    protected:
	static const string KValueUri;
	static const string KModelName;
	ValueRp* iValue;
};

#endif

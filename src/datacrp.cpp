#include <prop.h>
#include <syst.h>
#include "datacrp.h"
#include "common.h"

using namespace Gtk;

//  Connection point representation
ValueRp::ValueRp(MElem* aModel, MMdlObserver* aMdlObs): iElem(aModel), iMdlObs(aMdlObs), mCntId(string())
{
    // Set text from Value
    MProp* prop = iElem->GetObj(prop);
    assert(prop != NULL);
    set_text(prop->Value());
    iMdlObs->SignalCompChanged().connect(sigc::mem_fun(*this, &ValueRp::on_comp_changed));
    iMdlObs->SignalContentChanged().connect(sigc::mem_fun(*this, &ValueRp::on_comp_changed));
}

ValueRp::ValueRp(MElem* aModel, const string& aCntId, MMdlObserver* aMdlObs): iElem(aModel), iMdlObs(aMdlObs), mCntId(aCntId)
{
    string cntval;
    iElem->GetCont(cntval, mCntId);
    set_text(cntval);
    iMdlObs->SignalCompChanged().connect(sigc::mem_fun(*this, &ValueRp::on_comp_changed));
    iMdlObs->SignalContentChanged().connect(sigc::mem_fun(*this, &ValueRp::on_comp_changed));
}

void ValueRp::on_comp_changed(MElem* aComp)
{
    if (aComp == iElem) {
	//std::cout << "DataRp::on_comp_changed" << std::endl;
	if (mCntId.empty()) {
	MProp* prop = iElem->GetObj(prop);
	assert(prop != NULL);
	set_text(prop->Value());
	} else { // Multicontent
	    string cntval;
	    iElem->GetCont(cntval, mCntId);
	    set_text(cntval);
	}
    }
}



const string DataCrp::KModelName = "DataS";
const string DataCrp::KValueUri = "./Value";
const string DataCrp::KValueCntId = "Value";

const string sCrpType = "DataCrp";

const string& DataCrp::Type()
{
    return sCrpType;
}

string DataCrp::EType()
{
    return IncapsCrp::EType() + GUri::KParentSep + KModelName;
}

DataCrp::DataCrp(MElem* aElem, MMdlObserver* aMdlObs): IncapsCrp(aElem, aMdlObs), iValue(NULL)
{
}

DataCrp::~DataCrp()
{
    delete iValue;
}

void DataCrp::Construct()
{
    // Base construction
    IncapsCrp::Construct();
    // Add CPs
    MElem* val = iElem->GetNode(KValueUri);
    if (val != NULL) {
	iValue = new ValueRp(val, iMdlObs);
    } else {
	string cntval;
	TBool cntexst = iElem->GetCont(cntval, KValueCntId);
	assert(cntexst);
	iValue = new ValueRp(iElem, KValueCntId, iMdlObs);
    }
    add(*iValue);
    iValue->show();
}

void *DataCrp::DoGetObj(const string& aName)
{
    void* res = NULL;
    if (aName ==  Type()) {
	res = this;
    }
    else if (aName ==  MCrpConnectable::Type()) {
	res = (MCrpConnectable*) this;
    }
    return res;
}

void DataCrp::on_size_allocate(Gtk::Allocation& aAllc)
{
    IncapsCrp::on_size_allocate(aAllc);
    Gtk::Requisition main_req;
    IncapsCrp::on_size_request(&main_req);
    Gtk::Requisition val_req = iValue->size_request();
    // Allocate value
    int vx = (aAllc.get_width() - val_req.width)/2;
    Allocation val_alc = Allocation(vx, iBodyAlc.get_y() + main_req.height, val_req.width, val_req.height);
    iValue->size_allocate(val_alc);
}

void DataCrp::on_size_request(Gtk::Requisition* aReq)
{
    IncapsCrp::on_size_request(aReq);
    // Updating with size of value
    Gtk::Requisition value_req = iValue->size_request();
    aReq->height = aReq->height + value_req.height + KViewElemCrpInnerBorder;
    aReq->width = max(aReq->width, value_req.width + 2*KViewElemCrpInnerBorder); 
}

void DataCrp::GetContentUri(GUri& aUri)
{
    aUri.AppendElem("Prop", "Value");
}

bool DataCrp::IsActionSupported(Action aAction)
{
    return (aAction == EA_Remove || aAction == EA_Rename || aAction == EA_Edit_Content);
}

#include <prop.h>
#include <syst.h>
#include "datacrp.h"
#include "common.h"

//  Connection point representation
ValueRp::ValueRp(Elem* aModel): iElem(aModel)
{
    // Set text from Value
    MProp* prop = iElem->GetObj(prop);
    assert(prop != NULL);
    set_text(prop->Value());
}


const string DataCrp::KModelName = "DataS";
const string DataCrp::KValueUri = "Prop:Value";

const string sCrpType = "DataCrp";

const string& DataCrp::Type()
{
    return sCrpType;
}

string DataCrp::EType()
{
    return IncapsCrp::EType() + GUri::KParentSep + KModelName;
}

DataCrp::DataCrp(Elem* aElem, MMdlObserver* aMdlObs): IncapsCrp(aElem, aMdlObs), iValue(NULL)
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
    Elem* val = iElem->GetNode(KValueUri);
    assert(val != NULL);
    iValue = new ValueRp(val);
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
    Gtk::Requisition head_req = iHead->size_request();
    // Allocate value
    Gtk::Allocation val_alc = Gtk::Allocation(iBodyAlc.get_x(), iBodyAlc.get_y() + head_req.height + KViewCompEmptyBodyHight/2,
	    iBodyAlc.get_width(), aAllc.get_height() - head_req.height - KViewCompEmptyBodyHight);
    iValue->size_allocate(val_alc);
}

void DataCrp::on_size_request(Gtk::Requisition* aReq)
{
    IncapsCrp::on_size_request(aReq);
    // Updating with size of value
    Gtk::Requisition value_req = iValue->size_request();
    aReq->height = max(aReq->height, value_req.height);
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

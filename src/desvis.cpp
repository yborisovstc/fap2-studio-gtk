
#include <mdata.h>
#include "desvis.h"

const string KEventBtn_Srep = "TPL,SI:Type,SI:State,SI:Button,SI:X,SI:Y 0 0 0 0 0";
const string KMotionEvent_Srep = "TPL,SI:Type,SI:State,SI:X,SI:Y 0 0 0 0";
const string KAllocation_Srep = "TPL,SI:X,SI:Y,SI:W,SI:H 0 0 0 0";

// Agent of top level window

string AWindow::PEType()
{
    return Elem::PEType() + GUri::KParentSep + Type();
}

AWindow::AWindow(const string& aName, Elem* aMan, MEnv* aEnv, MSDesEnv* aSDesEnv): 
    Elem(aName, aMan, aEnv), iSDesEnv(aSDesEnv)
{
    SetEType(Type(), Elem::PEType());
    SetParent(Type());
}

AWindow::AWindow(Elem* aMan, MEnv* aEnv, MSDesEnv* aSDesEnv): 
    Elem(Type(), aMan, aEnv), iSDesEnv(aSDesEnv)
{
    SetEType(Elem::PEType());
    SetParent(Elem::PEType());
}

void *AWindow::DoGetObj(const char *aName, TBool aIncUpHier, const RqContext* aCtx)
{
    void* res = NULL;
    if (strcmp(aName, Type()) == 0) {
	res = this;
    } 
    else if (strcmp(aName, MVisContainer::Type()) == 0) {
	res = (MVisContainer*) this;
    } 
    else {
	res = Elem::DoGetObj(aName, aIncUpHier);
    }
    return res;
}

Container& AWindow::GetContainer()
{
    return iSDesEnv->VisWindow();
}

void AWindow::OnChildChanged(Widget* aChild, MVisChild::TPar aPar)
{
}


// Base of Agent of widgets

TInt AVisWidget::ParentSizeProv::Value()
{
    return iHost->GetParData(iData);
}

string AVisWidget::ParentSizeProvVar::VarGetIfid()
{ 
    return MDtGet<Sdata<int> >::Type();
}

void AVisWidget::ParentSizeProvVar::DtGet(Sdata<int>& aData)
{
    aData.mValid = true;
    aData.mData = iHost->GetParData(iData);
}

void *AVisWidget::ParentSizeProvVar::DoGetDObj(const char *aName)
{
    void* res = NULL;
    if (strcmp(aName, MDtGet<Sdata<int> >::Type()) == 0) res = (MDtGet<Sdata<int> >*) this;
    return res;
}

// Data provider for GdkEventButton
string AVisWidget::EventButtonProv::VarGetIfid()
{ 
    return MDtGet<NTuple>::Type();
}

void *AVisWidget::EventButtonProv::DoGetDObj(const char *aName)
{
    void* res = NULL;
    if (strcmp(aName, MDtGet<NTuple>::Type()) == 0) res = (MDtGet<NTuple>*) this;
    return res;
}

void AVisWidget::EventButtonProv::DtGet(NTuple& aData)
{
    iHost->GetBtnPressEvent(aData);
    aData.mValid = true;
}

// Data provider for ButoonRelease GdkEventButton
string AVisWidget::EventButtonReleaseProv::VarGetIfid()
{ 
    return MDtGet<NTuple>::Type();
}

void *AVisWidget::EventButtonReleaseProv::DoGetDObj(const char *aName)
{
    void* res = NULL;
    if (strcmp(aName, MDtGet<NTuple>::Type()) == 0) res = (MDtGet<NTuple>*) this;
    return res;
}

void AVisWidget::EventButtonReleaseProv::DtGet(NTuple& aData)
{
    iHost->GetBtnReleaseEvent(aData);
    aData.mValid = true;
}


// Data provider for GdkEventMotion
string AVisWidget::EventMotionProv::VarGetIfid()
{ 
    return MDtGet<NTuple>::Type();
}

void *AVisWidget::EventMotionProv::DoGetDObj(const char *aName)
{
    void* res = NULL;
    if (strcmp(aName, MDtGet<NTuple>::Type()) == 0) res = (MDtGet<NTuple>*) this;
    return res;
}

void AVisWidget::EventMotionProv::DtGet(NTuple& aData)
{
    iHost->GetMotionEvent(aData);
    aData.mValid = true;
}

// Data provider for Allocation
string AVisWidget::AllocationProv::VarGetIfid()
{
    return MDtGet<NTuple>::Type();
}

void *AVisWidget::AllocationProv::DoGetDObj(const char *aName)
{
    void* res = NULL;
    if (strcmp(aName, MDtGet<NTuple>::Type()) == 0) res = (MDtGet<NTuple>*) this;
    return res;
}

void AVisWidget::AllocationProv::DtGet(NTuple& aData)
{
    aData = iHost->mAllocation;
    aData.mValid = true;
}




string AVisWidget::PEType()
{
    return Elem::PEType() + GUri::KParentSep + Type();
}

bool AVisWidget::mInit = false;
AVisWidget::tStatesMap  AVisWidget::mStatesMap;

AVisWidget::AVisWidget(const string& aName, Elem* aMan, MEnv* aEnv): Elem(aName, aMan, aEnv),
    iWidget(NULL), iX(0), iY(0), iH(10), iW(10)
{
    SetEType(Type(), Elem::PEType());
    SetParent(Type());
    iParProvW.SetData(ParentSizeProv::ED_W, this);
    iParProvH.SetData(ParentSizeProv::ED_H, this);
    iParProvVarW.SetData(ParentSizeProv::ED_W, this);
    iParProvVarH.SetData(ParentSizeProv::ED_H, this);
    mBtnPressEvtProv.SetHost(this);
    mBtnPressEvt.FromString(KEventBtn_Srep);
    mBtnReleaseEvtProv.SetHost(this);
    mBtnReleaseEvt.FromString(KEventBtn_Srep);
    mMotionEvtProv.SetHost(this);
    mMotionEvt.FromString(KMotionEvent_Srep);
    mAllocationProv.SetHost(this);
    mAllocation.FromString(KAllocation_Srep);
    // TODO [YB] To use the published data "Widget_State" from module ../Widged_common
    // instead of the internal constants.
    if (!mInit) {
	mStatesMap[Gtk::STATE_NORMAL] = string("Normal");
	mStatesMap[Gtk::STATE_ACTIVE] = string("Active");
	mStatesMap[Gtk::STATE_PRELIGHT] = string("Prelight");
	mStatesMap[Gtk::STATE_SELECTED] = string("Selected");
	mStatesMap[Gtk::STATE_INSENSITIVE] = string("Insensitive");
	mInit = true;
    }
}

AVisWidget::AVisWidget(Elem* aMan, MEnv* aEnv): Elem(Type(), aMan, aEnv),
    iWidget(NULL), iX(0), iY(0), iH(10), iW(10)
{
    SetEType(Elem::PEType());
    SetParent(Elem::PEType());
    iParProvW.SetData(ParentSizeProv::ED_W, this);
    iParProvH.SetData(ParentSizeProv::ED_H, this);
    iParProvVarW.SetData(ParentSizeProv::ED_W, this);
    iParProvVarH.SetData(ParentSizeProv::ED_H, this);
    mBtnPressEvtProv.SetHost(this);
    mBtnPressEvt.FromString(KEventBtn_Srep);
    mBtnReleaseEvtProv.SetHost(this);
    mBtnReleaseEvt.FromString(KEventBtn_Srep);
    mMotionEvtProv.SetHost(this);
    mMotionEvt.FromString(KMotionEvent_Srep);
    mAllocationProv.SetHost(this);
    mAllocation.FromString(KAllocation_Srep);
}

void AVisWidget::Construct()
{
    if (iWidget != NULL) {
	iWidget->add_events(Gdk::BUTTON_PRESS_MASK | Gdk::BUTTON_RELEASE_MASK | Gdk::POINTER_MOTION_MASK);
	iWidget->signal_button_press_event().connect(sigc::mem_fun(*this, &AVisWidget::OnButtonPress));
	iWidget->signal_button_release_event().connect(sigc::mem_fun(*this, &AVisWidget::OnButtonPress));
	iWidget->signal_motion_notify_event().connect(sigc::mem_fun(*this, &AVisWidget::OnMotion));
    }
}

void *AVisWidget::DoGetObj(const char *aName, TBool aIncUpHier, const RqContext* aCtx)
{
    void* res = NULL;
    if (strcmp(aName, Type()) == 0) {
	res = this;
    } 
    else if (strcmp(aName, MVisChild::Type()) == 0) {
	res = (MVisChild*) this;
    } 
    else if (strcmp(aName, MACompsObserver::Type()) == 0) {
	res = (MACompsObserver*) this;
    } 
    else if (strcmp(aName, MDesObserver::Type()) == 0) {
	res = (MDesObserver*) this;
    } 
    else {
	res = Elem::DoGetObj(aName, aIncUpHier);
    }
    return res;
}

void AVisWidget::UpdateIfi(const string& aName, const RqContext* aCtx)
{
    void* res = NULL;
    TIfRange rr;
    TBool resg = EFalse;
    RqContext ctx(this, aCtx);
    if (strcmp(aName.c_str(), Type()) == 0) {
	res = this;
    }
    else if (strcmp(aName.c_str(), MDIntGet::Type()) == 0) {
	Elem* cpw = GetNode("./../../Prov_PW");
	Elem* cph = GetNode("./../../Prov_PH");
	if (aCtx->IsInContext(cpw)) {
	    res = (MDIntGet*) &iParProvW;
	}
	else if (aCtx->IsInContext(cph)) {
	    res = (MDIntGet*) &iParProvH;
	}
    }
    else if (strcmp(aName.c_str(), MDVarGet::Type()) == 0 || strcmp(aName.c_str(), MDtGet<Sdata<int> >::Type()) == 0) {
	bool isdvar = strcmp(aName.c_str(), MDVarGet::Type()) == 0;
	Elem* cpw = GetNode("./../../Prov_PW");
	Elem* cph = GetNode("./../../Prov_PH");
	Elem* bpe = GetNode("./../../BtnPressEvent");
	Elem* bre = GetNode("./../../BtnReleaseEvent");
	Elem* mte = GetNode("./../../MotionEvent");
	Elem* alc = GetNode("./../../Allocation");
	if (aCtx->IsInContext(cpw)) {
	    if (isdvar) {
		res = (MDVarGet*) &iParProvVarW;
	    }
	    else {
		res = (MDtGet<Sdata<int> >*) &iParProvVarW;
	    }
	}
	else if (aCtx->IsInContext(cph)) {
	    if (isdvar) {
		res = (MDVarGet*) &iParProvVarH;
	    }
	    else {
		res = (MDtGet<Sdata<int> >*) &iParProvVarH;
	    }
	}
	else if (aCtx->IsInContext(bpe)) {
	    if (isdvar) {
		res = (MDVarGet*) &mBtnPressEvtProv;
	    }
	}
	else if (aCtx->IsInContext(bre)) {
	    if (isdvar) {
		res = (MDVarGet*) &mBtnReleaseEvtProv;
	    }
	}
	else if (aCtx->IsInContext(mte)) {
	    if (isdvar) {
		res = (MDVarGet*) &mMotionEvtProv;
	    }
	}
	else if (aCtx->IsInContext(alc)) {
	    if (isdvar) {
		res = (MDVarGet*) &mAllocationProv;
	    }
	}
    }
    else {
	res = DoGetObj(aName.c_str(), EFalse, aCtx);
    }
    if (res != NULL) {
	InsertIfCache(aName, aCtx, this, res);
    }
}

Widget& AVisWidget::GetChild()
{
    return *iWidget;
}

TInt AVisWidget::GetParData(ParentSize::TData aData)
{
    TInt res = 0;
    // Update parent of widget. It is requied because there is no notification of parent change or
    // connection to parent change. In fact the observation via HandleCompChanged give us only
    // info of local connection change, but not the whole connection chain change. Ref grayb uc_010
    Container* parent = iWidget != NULL ? iWidget->get_parent() : NULL;
    if (parent == NULL) {
	Elem* eprntcp = GetNode("./../../Child");
	if (eprntcp != NULL) {
	    MVisContainer* mcont = (MVisContainer*) eprntcp->GetSIfiC(MVisContainer::Type(), this);
	    if (mcont != NULL) {
		if (iWidget->get_parent() == NULL) {
		    Container& cont = mcont->GetContainer();
		    cont.add(*iWidget);
		    parent = iWidget->get_parent();
		}
	    }
	}
    }
    if (parent != NULL) {
	Allocation palc = parent->get_allocation();
	res = aData == ParentSizeProv::ED_W ? palc.get_width() : palc.get_height();
    }
    return res;
}

int AVisWidget::GetParInt(TPar aPar)
{
    int res = 0;
    if (aPar == EPar_Y) {
	res = iY;
    }
    return res;
}

MVisContainer* AVisWidget::GetVisContainer()
{
    MVisContainer* res = NULL;
    Elem* eprntcp = Host()->GetNode("./Child");
    if (eprntcp != NULL) {
	res = (MVisContainer*) eprntcp->GetSIfiC(MVisContainer::Type(), this);
    }
    else {
	Logger()->Write(MLogRec::EErr, this, "Input [Child] not exists");
    }
    return res;
}

TBool AVisWidget::HandleCompChanged(Elem& aContext, Elem& aComp)
{
    TBool res = ETrue;
    Elem* eprntcp = aContext.GetNode("./Child");
    if (eprntcp != NULL) {
	if (eprntcp == &aComp || eprntcp->IsComp(&aComp)) {
	    MVisContainer* mcont = (MVisContainer*) eprntcp->GetSIfiC(MVisContainer::Type(), this);
	    if (iWidget != NULL) {
		if (mcont != NULL) {
		    if (iWidget->get_parent() == NULL) {
			Container& cont = mcont->GetContainer();
			cont.add(*iWidget);
		    }
		    else {
			Logger()->Write(MLogRec::EErr, this, "Attempt to attach already attached child");
			res = EFalse;
		    }
		}
		else if (iWidget->get_parent() != NULL) {
		    iWidget->get_parent()->remove(*iWidget);
		}
	    }
	}
    }
    else {
	Logger()->Write(MLogRec::EErr, this, "Input [Child] not exists");
	res = EFalse;
    }
    return res;
}

Elem* AVisWidget::Host() 
{
    return iMan->GetMan();
}


bool AVisWidget::GetDataInt(const string& aInpUri, int& aData) 
{
    bool res = false;
    Elem* einp = Host()->GetNode(aInpUri);
    if (einp != NULL) {
	MDIntGet* mdata = (MDIntGet*) einp->GetSIfiC(MDIntGet::Type(), this);
	if (mdata != NULL) {
	    aData = mdata->Value();
	    res = true;
	}
	else {
	    // Trying variable data
	    MDVarGet* mvget = (MDVarGet*) einp->GetSIfiC(MDVarGet::Type(), this);
	    if (mvget != NULL) {
		MDtGet<Sdata<int> >* mdata = mvget->GetDObj(mdata);
		if (mdata != NULL) {
		    Sdata<int> sd;
		    mdata->DtGet(sd);
		    if (sd.mValid) {
			aData = sd.mData;
		    }
		    res = true;
		}
	    }
	}
    }
    else {
	Logger()->Write(MLogRec::EErr, this, "Input [%s] not exists", aInpUri.c_str());
    }
    return res;
}

bool AVisWidget::GetInpState(const string& aInpUri, Gtk::StateType& aData)
{
    bool res = false;
    Elem* einp = Host()->GetNode(aInpUri);
    if (einp != NULL) {
	// Trying variable data
	MDVarGet* mvget = (MDVarGet*) einp->GetSIfiC(MDVarGet::Type(), this);
	if (mvget != NULL) {
	    MDtGet<Enum>* mdata = mvget->GetDObj(mdata);
	    if (mdata != NULL) {
		Enum sd;
		mdata->DtGet(sd);
		if (sd.mValid) {
		    Gtk::StateType data = (Gtk::StateType) sd.mData;
		    if (sd.mSet.at(data) == mStatesMap.at(data)) {
			aData = data;
		    }
		}
		res = true;
	    }
	}
    }
    else {
	Logger()->Write(MLogRec::EErr, this, "Input [%s] not exists", aInpUri.c_str());
    }
    return res;
}

void AVisWidget::OnUpdated_X(int aOldData)
{
}

void AVisWidget::OnUpdated_Y(int aOldData)
{
}

void AVisWidget::OnUpdated_W(int aOldData)
{
}

void AVisWidget::OnUpdated_H(int aOldData)
{
}

void AVisWidget::OnUpdated()
{
    MVisContainer* cont = GetVisContainer();
    // Data providers updated - update the data
    // X coord
    bool res;
    int sX, oldX = iX;
    res = GetDataInt("./Inp_X/Int/PinData", sX);
    if (res && sX != iX) {
	iX = sX;
	// Set allocation data
	Sdata<int>* adata = dynamic_cast<Sdata<int>*> (mAllocation.GetElem("X"));
	__ASSERT(adata != NULL);
	adata->Set(iX);
	OnUpdated_X(oldX);
    }
    // Y coord
    int sY, oldY = iY;
    res = GetDataInt("./Inp_Y/Int/PinData", sY);
    if (res && sY != iY) {
	iY = sY;
	// Set allocation data
	Sdata<int>* adata = dynamic_cast<Sdata<int>*> (mAllocation.GetElem("Y"));
	__ASSERT(adata != NULL);
	adata->Set(iY);
	OnUpdated_Y(oldY);
    }
    // Width
    int sW, oldW = iW;
    res = GetDataInt("./Inp_W/Int/PinData", sW);
    if (res && sW != iW) {
	iW = sW;
	// Set allocation data
	Sdata<int>* adata = dynamic_cast<Sdata<int>*> (mAllocation.GetElem("W"));
	__ASSERT(adata != NULL);
	adata->Set(iW);
	OnUpdated_W(oldW);
    }
    // Heigth
    int sH, oldH = iH;
    res = GetDataInt("./Inp_H/Int/PinData", sH);
    if (res && sH != iH) {
	iH = sH;
	// Set allocation data
	Sdata<int>* adata = dynamic_cast<Sdata<int>*> (mAllocation.GetElem("H"));
	__ASSERT(adata != NULL);
	adata->Set(iH);
	OnUpdated_H(oldH);
    }
    // State
    Gtk::StateType state;
    res = GetInpState("./Inp_State/Int/PinData", state);
    if (res && iWidget != NULL && state != iWidget->get_state()) {
	iWidget->set_state(state);
    }
}

void AVisWidget::OnActivated()
{
}

bool AVisWidget::OnButtonPress(GdkEventButton* aEvent)
{
    return HandleButtonPress(aEvent);
}

bool AVisWidget::OnButtonRelease(GdkEventButton* aEvent)
{
    return HandleButtonRelease(aEvent);
}

bool AVisWidget::OnMotion(GdkEventMotion* aEvent)
{
    return HandleMotion(aEvent);
}

bool AVisWidget::HandleButtonPress(GdkEventButton* aEvent)
{
    // Set event data
    Sdata<int>* etype = dynamic_cast<Sdata<int>*> (mBtnPressEvt.GetElem("Type"));
    __ASSERT(etype != NULL);
    etype->Set(aEvent->type);
    Sdata<int>* estate = dynamic_cast<Sdata<int>*> (mBtnPressEvt.GetElem("State"));
    __ASSERT(estate != NULL);
    estate->Set(aEvent->state);
    Sdata<int>* ex = dynamic_cast<Sdata<int>*> (mBtnPressEvt.GetElem("X"));
    __ASSERT(ex != NULL);
    ex->Set(aEvent->x);
    Sdata<int>* ey = dynamic_cast<Sdata<int>*> (mBtnPressEvt.GetElem("Y"));
    __ASSERT(ey != NULL);
    ey->Set(aEvent->y);
    // Activate dependencies
    ActivateDeps("./../../BtnPressEvent/Int/PinObs");
    return false;
}

bool AVisWidget::HandleButtonRelease(GdkEventButton* aEvent)
{
    // Set event data
    Sdata<int>* etype = dynamic_cast<Sdata<int>*> (mBtnReleaseEvt.GetElem("Type"));
    __ASSERT(etype != NULL);
    etype->Set(aEvent->type);
    Sdata<int>* estate = dynamic_cast<Sdata<int>*> (mBtnReleaseEvt.GetElem("State"));
    __ASSERT(estate != NULL);
    estate->Set(aEvent->state);
    Sdata<int>* ex = dynamic_cast<Sdata<int>*> (mBtnReleaseEvt.GetElem("X"));
    __ASSERT(ex != NULL);
    ex->Set(aEvent->x);
    Sdata<int>* ey = dynamic_cast<Sdata<int>*> (mBtnReleaseEvt.GetElem("Y"));
    __ASSERT(ey != NULL);
    ey->Set(aEvent->y);
    // Activate dependencies
    ActivateDeps("./../../BtnReleaseEvent/Int/PinObs");
    return false;
}

void AVisWidget::ActivateDeps(const string& aUri) {
    Elem* eobs = GetNode(aUri);
    __ASSERT(eobs != NULL);
    RqContext ctx(this);
    TIfRange range = eobs->GetIfi(MDesObserver::Type());
    for (IfIter it = range.first; it != range.second; it++) {
	MDesObserver* mobs = (MDesObserver*) (*it);
	if (mobs != NULL) {
	    mobs->OnUpdated();
	}
    }
}

bool AVisWidget::HandleMotion(GdkEventMotion* aEvent)
{
    // Set event data
    Sdata<int>* etype = dynamic_cast<Sdata<int>*> (mMotionEvt.GetElem("Type"));
    __ASSERT(etype != NULL);
    etype->Set(aEvent->type);
    Sdata<int>* estate = dynamic_cast<Sdata<int>*> (mMotionEvt.GetElem("State"));
    __ASSERT(estate != NULL);
    estate->Set(aEvent->state);
    Sdata<int>* ex = dynamic_cast<Sdata<int>*> (mMotionEvt.GetElem("X"));
    __ASSERT(ex != NULL);
    ex->Set(aEvent->x);
    Sdata<int>* ey = dynamic_cast<Sdata<int>*> (mMotionEvt.GetElem("Y"));
    __ASSERT(ey != NULL);
    ey->Set(aEvent->y);
    // Activate dependencies
    ActivateDeps("./../../MotionEvent/Int/PinObs");
    return false;
}


// Agent of container with fixed layout

string AVisFixed::PEType()
{
    return AVisWidget::PEType() + GUri::KParentSep + Type();
}

AVisFixed::AVisFixed(const string& aName, Elem* aMan, MEnv* aEnv): AVisWidget(aName, aMan, aEnv)
{
    SetEType(Type(), AVisWidget::PEType());
    SetParent(Type());
    Fixed* fx = new Fixed(); 
    fx->set_reallocate_redraws(true);
    iWidget = fx;
    iWidget->show();
}

AVisFixed::AVisFixed(Elem* aMan, MEnv* aEnv): AVisWidget(Type(), aMan, aEnv)
{
    SetEType(AVisWidget::PEType());
    SetParent(AVisWidget::PEType());
    Fixed* fx = new Fixed(); 
    fx->set_reallocate_redraws(true);
    iWidget = fx;
    iWidget->show();
}

void *AVisFixed::DoGetObj(const char *aName, TBool aIncUpHier, const RqContext* aCtx)
{
    void* res = NULL;
    if (strcmp(aName, Type()) == 0) {
	res = this;
    } 
    else if (strcmp(aName, MVisContainer::Type()) == 0) {
	res = (MVisContainer*) this;
    } 
    else {
	res = AVisWidget::DoGetObj(aName, aIncUpHier);
    }
    return res;
}

Fixed* AVisFixed::GetFixed()
{
    return dynamic_cast<Fixed*>(iWidget);
}

Container& AVisFixed::GetContainer()
{
    return *GetFixed();
}

void AVisFixed::OnChildChanged(Widget* aChild, MVisChild::TPar aPar)
{
}

// Agent of drawing area

VisDrwArea::VisDrwArea(AVisWidget* aHost): DrawingArea(), mHost(aHost)
{
}

bool VisDrwArea::on_expose_event(GdkEventExpose* aEvent)
{
    /*
    DrawingArea::on_expose_event(aEvent);
    Glib::RefPtr<Gdk::Window> drw = get_window();
    Glib::RefPtr<Gtk::Style> style = get_style(); 	
    Glib::RefPtr<Gdk::GC> gc = style->get_fg_gc(get_state());
    Allocation alc = get_allocation();
    drw->draw_rectangle(gc, true, 0, 0, alc.get_width() - 1, alc.get_height() - 1);
    */
    mHost->Logger()->Write(MLogRec::EInfo, mHost, "on_expose_event");
    Elem* eobs = mHost->GetNode("./../../DrawingArea");
    __ASSERT(eobs != NULL);
    Elem::TIfRange range = eobs->GetIfi(MVisDrawingElem::Type());
    for (Elem::IfIter it = range.first; it != range.second; it++) {
	MVisDrawingElem* mobs = (MVisDrawingElem*) (*it);
	if (mobs != NULL) {
	    mobs->OnExpose(aEvent);
	}
    }
}

void VisDrwArea::on_size_allocate(Allocation& aAlloc)
{
    DrawingArea::on_size_allocate(aAlloc);
}

void VisDrwArea::on_size_request(Requisition* aReq)
{
    mHost->Logger()->Write(MLogRec::EInfo, mHost, "on_size_request");
    DrawingArea::on_size_request(aReq);
}



string AVisDrawing::PEType()
{
    return AVisWidget::PEType() + GUri::KParentSep + Type();
}

AVisDrawing::AVisDrawing(const string& aName, Elem* aMan, MEnv* aEnv): AVisWidget(aName, aMan, aEnv)
{
    SetEType(Type(), AVisWidget::PEType());
    SetParent(Type());
    iWidget = new VisDrwArea(this); 
    iWidget->set_size_request(iW, iH);
    iWidget->set_name("AVisDrawing");
    iWidget->show();
    Construct();
}

AVisDrawing::AVisDrawing(Elem* aMan, MEnv* aEnv): AVisWidget(Type(), aMan, aEnv)
{
    SetEType(AVisWidget::PEType());
    SetParent(AVisWidget::PEType());
    iWidget = new VisDrwArea(this); 
    iWidget->set_size_request(iW, iH);
    iWidget->show();
}

void *AVisDrawing::DoGetObj(const char *aName, TBool aIncUpHier, const RqContext* aCtx)
{
    void* res = NULL;
    if (strcmp(aName, Type()) == 0) {
	res = this;
    } 
    else if (strcmp(aName, MVisDrawingArea::Type()) == 0) {
	res = (MVisDrawingArea*) this;
    } 
    else {
	res = AVisWidget::DoGetObj(aName, aIncUpHier);
    }
    return res;
}

Elem::TIfRange AVisDrawing::GetDrawingElems()
{
    Elem* eobs = GetNode("./../../DrawingArea");
    __ASSERT(eobs != NULL);
    Elem::TIfRange range = eobs->GetIfi(MVisDrawingElem::Type());
    return range;
}

VisDrwArea* AVisDrawing::GetDrawing()
{
    return dynamic_cast<VisDrwArea*>(iWidget);
}

void AVisDrawing::OnUpdated_X(int aX)
{
    Allocation alc = iWidget->get_allocation();
    Container* parent = iWidget->get_parent();
    // X change handles only by Fixed parent
    Fixed* fx = dynamic_cast<Fixed*>(parent);
    if (fx != NULL) {
	fx->move(*iWidget, iX, iY);
    }
}


void AVisDrawing::OnUpdated_Y(int aY)
{
    Allocation alc = iWidget->get_allocation();
    Container* parent = iWidget->get_parent();
    // Y change handles only by Fixed parent
    Fixed* fx = dynamic_cast<Fixed*>(parent);
    if (fx != NULL) {
	fx->move(*iWidget, iX, iY);
    }
}

void AVisDrawing::OnUpdated_W(int aOldData)
{
    Allocation alc = iWidget->get_allocation();
    Container* parent = iWidget->get_parent();
    VisDrwArea* drw = GetDrawing();
    drw->set_size_request(iW, iH);
    drw->queue_draw_area(alc.get_x(), alc.get_y(), iW, iH);
}

void AVisDrawing::OnUpdated_H(int aOldData)
{
    Allocation alc = iWidget->get_allocation();
    Container* parent = iWidget->get_parent();
    VisDrwArea* drw = GetDrawing();
    drw->set_size_request(iW, iH);
    drw->queue_draw_area(alc.get_x(), alc.get_y(), iW, iH);
}

void AVisDrawing::GetAllocation(Rectangle& aRect)
{
    VisDrwArea* drw = GetDrawing();
    aRect = drw->get_allocation();
}

Glib::RefPtr<Gdk::Window> AVisDrawing::GetWindow()
{
    VisDrwArea* drw = GetDrawing();
    return drw->get_window();
}

/*
Glib::RefPtr<Widget> AVisDrawing::GetWidget()
{
    return Glib::RefPtr<Widget>(GetDrawing());
}
*/

bool AVisDrawing::HandleButtonPress(GdkEventButton* aEvent)
{
    bool res = AVisWidget::HandleButtonPress(aEvent);
    if (!res) {
	Elem::TIfRange range = GetDrawingElems();
	for (Elem::IfIter it = range.first; it != range.second; it++) {
	    MVisDrawingElem* mobs = (MVisDrawingElem*) (*it);
	    if (mobs != NULL) {
		mobs->OnAreaButtonPress(aEvent);
	    }
	}
    }
    return res;
}

bool AVisDrawing::HandleButtonRelease(GdkEventButton* aEvent)
{
    bool res = AVisWidget::HandleButtonRelease(aEvent);
    if (!res) {
	Elem::TIfRange range = GetDrawingElems();
	for (Elem::IfIter it = range.first; it != range.second; it++) {
	    MVisDrawingElem* mobs = (MVisDrawingElem*) (*it);
	    if (mobs != NULL) {
		mobs->OnAreaButtonRelease(aEvent);
	    }
	}
    }
    return res;
}

bool AVisDrawing::HandleMotion(GdkEventMotion* aEvent)
{
    bool res = AVisWidget::HandleMotion(aEvent);
    if (!res) {
	Elem::TIfRange range = GetDrawingElems();
	for (Elem::IfIter it = range.first; it != range.second; it++) {
	    MVisDrawingElem* mobs = (MVisDrawingElem*) (*it);
	    if (mobs != NULL) {
		mobs->OnAreaMotion(aEvent);
	    }
	}
    }
    return res;
}


// Agent of drawing element

string AVisDrawingElem::PEType()
{
    return AVisWidget::PEType() + GUri::KParentSep + Type();
}

AVisDrawingElem::AVisDrawingElem(const string& aName, Elem* aMan, MEnv* aEnv): AVisWidget(aName, aMan, aEnv)
{
    SetEType(Type(), AVisWidget::PEType());
    SetParent(Type());
}

AVisDrawingElem::AVisDrawingElem(Elem* aMan, MEnv* aEnv): AVisWidget(Type(), aMan, aEnv)
{
    SetEType(AVisWidget::PEType());
    SetParent(AVisWidget::PEType());
}

void *AVisDrawingElem::DoGetObj(const char *aName, TBool aIncUpHier, const RqContext* aCtx)
{
    void* res = NULL;
    if (strcmp(aName, Type()) == 0) {
	res = this;
    } else if (strcmp(aName, MVisDrawingElem::Type()) == 0){
	res = (MVisDrawingElem*) this;
    } else {
	res = AVisWidget::DoGetObj(aName, aIncUpHier);
    }
    return res;
}

void AVisDrawingElem::OnExpose(GdkEventExpose* aEvent)
{
    MVisDrawingArea* mda = GetDrawingArea();
    Glib::RefPtr<Gdk::Window> drw = mda->GetWindow();
    //Glib::RefPtr<Gdk::Window> drw = Glib::wrap((GdkWindowObject*) aEvent->window);
    Glib::RefPtr<Gdk::GC> gc = Gdk::GC::create(drw);
    Gdk::Color bg_color;
    bg_color.set("#ff0000");
    gc->get_colormap()->alloc_color(bg_color);
    gc->set_background(bg_color);
    gc->set_foreground(bg_color);
    drw->draw_rectangle(gc, true, iX, iY, iW - 1, iH - 1);
}

void AVisDrawingElem::OnUpdated_X(int aOldData)
{
    OnAllocUpdated(Rectangle(aOldData, iY, iW, iH));
}

void AVisDrawingElem::OnUpdated_Y(int aOldData)
{
    OnAllocUpdated(Rectangle(iX, aOldData, iW, iH));
}

void AVisDrawingElem::OnUpdated_W(int aOldData)
{
    OnAllocUpdated(Rectangle(iX, iY, aOldData, iH));
}

void AVisDrawingElem::OnUpdated_H(int aOldData)
{
    OnAllocUpdated(Rectangle(iX, iY, iW, aOldData));
}

MVisDrawingArea* AVisDrawingElem::GetDrawingArea()
{
    MVisDrawingArea* res = NULL;
    Elem* edacp = GetNode("./../../DrawingElem");
    if (edacp != NULL) {
	res = (MVisDrawingArea*) edacp->GetSIfiC(MVisDrawingArea::Type(), this);
    }
    return res;
}

void AVisDrawingElem::OnAllocUpdated(const Rectangle& aOldAlloc)
{
    MVisDrawingArea* mda = GetDrawingArea();
    if (mda != NULL) {
	Glib::RefPtr<Gdk::Window> wnd = mda->GetWindow();
	wnd->clear_area(aOldAlloc.get_x(), aOldAlloc.get_y(), aOldAlloc.get_width(), aOldAlloc.get_height());
	wnd->invalidate_rect(Rectangle(iX, iY, iW, iH), true);
	ActivateDeps("./../../Allocation/Int/PinObs");
    }
}

TInt AVisDrawingElem::GetParData(ParentSize::TData aData)
{
    TInt res = 0;
    MVisDrawingArea* mda = GetDrawingArea();
    if (mda != NULL) {
	Allocation palc;
	mda->GetAllocation(palc);
	res = aData == ParentSizeProv::ED_W ? palc.get_width() : palc.get_height();
    }
    return res;
}

bool AVisDrawingElem::OnAreaButtonPress(GdkEventButton* aEvent)
{
    return AVisWidget::HandleButtonPress(aEvent);
}

bool AVisDrawingElem::OnAreaButtonRelease(GdkEventButton* aEvent)
{
    return AVisWidget::HandleButtonRelease(aEvent);
}

bool AVisDrawingElem::OnAreaMotion(GdkEventMotion* aEvent)
{
    return AVisWidget::HandleMotion(aEvent);
}

/*
TBool AVisDrawingElem::HandleCompChanged(Elem& aContext, Elem& aComp)
{
    TBool res = AVisWidget::HandleCompChanged(aContext, aComp);
    if (res) {
	Elem* edecp = aContext.GetNode("./DrawingElem");
	if (edecp != NULL) {
	    if (edecp == &aComp || edecp->IsComp(&aComp)) {
		MVisDrawingArea* mde = (MVisDrawingArea*) edecp->GetSIfiC(MVisDrawingArea::Type(), this);
		Glib::RefPtr<Widget> wde = mde->GetWidget();
		if (mde != NULL) {
		    wde->signal_button_press_event().connect(sigc::mem_fun(*this, &AVisDrawingElem::OnAreaButtonPress));
		    wde->signal_button_release_event().connect(sigc::mem_fun(*this, &AVisDrawingElem::OnAreaButtonPress));
		}
	    }
	}
	else {
	    Logger()->Write(MLogRec::EErr, this, "Input [DrawingElem] doesn't exist");
	    res = EFalse;
	}
    }
    return res;
}
*/

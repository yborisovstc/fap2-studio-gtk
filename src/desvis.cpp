
#include <mdata.h>
#include "desvis.h"


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

string AVisWidget::PEType()
{
    return Elem::PEType() + GUri::KParentSep + Type();
}

AVisWidget::AVisWidget(const string& aName, Elem* aMan, MEnv* aEnv): Elem(aName, aMan, aEnv),
    iWidget(NULL), iX(0), iY(0), iH(10), iW(10)
{
    SetEType(Type(), Elem::PEType());
    SetParent(Type());
    iParProvW.SetData(ParentSizeProv::ED_W, this);
    iParProvH.SetData(ParentSizeProv::ED_H, this);
}

AVisWidget::AVisWidget(Elem* aMan, MEnv* aEnv): Elem(Type(), aMan, aEnv),
    iWidget(NULL), iX(0), iY(0), iH(10), iW(10)
{
    SetEType(Elem::PEType());
    SetParent(Elem::PEType());
    iParProvW.SetData(ParentSizeProv::ED_W, this);
    iParProvH.SetData(ParentSizeProv::ED_H, this);
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
	Elem* cpw = GetNode("../../Prov_PW");
	Elem* cph = GetNode("../../Prov_PH");
	if (aCtx->IsInContext(cpw)) {
	    res = (MDIntGet*) &iParProvW;
	}
	else if (aCtx->IsInContext(cph)) {
	    res = (MDIntGet*) &iParProvH;
	}
    }
    else {
	res = DoGetObj(aName.c_str(), EFalse, aCtx);
    }
    if (res != NULL) {
	InsertIfCache(aName, ToCacheRCtx(aCtx), this, res);
    }
}

Widget& AVisWidget::GetChild()
{
    return *iWidget;
}

TInt AVisWidget::GetParData(ParentSizeProv::TData aData)
{
    TInt res = 0;
    // Update parent of widget. It is requied because there is no notification of parent change or
    // connection to parent change. In fact the observation via HandleCompChanged give us only
    // info of local connection change, but not the whole connection chain change. Ref grayb uc_010
    Container* parent = iWidget->get_parent();
    if (parent == NULL) {
	Elem* eprntcp = GetNode("../../Child");
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
    Elem* eprntcp = Host()->GetNode("Child");
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
    TBool res = EFalse;
    Elem* eprntcp = aContext.GetNode("Child");
    if (eprntcp != NULL) {
	if (eprntcp == &aComp || eprntcp->IsComp(&aComp)) {
	    MVisContainer* mcont = (MVisContainer*) eprntcp->GetSIfiC(MVisContainer::Type(), this);
	    if (mcont != NULL) {
		if (iWidget->get_parent() == NULL) {
		    Container& cont = mcont->GetContainer();
		    cont.add(*iWidget);
		}
		else {
		    Logger()->Write(MLogRec::EErr, this, "Attempt to attach already attached child");
		}
	    }
	    else if (iWidget->get_parent() != NULL) {
		iWidget->get_parent()->remove(*iWidget);
	    }
	}
    }
    else {
	Logger()->Write(MLogRec::EErr, this, "Input [Child] not exists");
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
    }
    else {
	Logger()->Write(MLogRec::EErr, this, "Input [%s] not exists", aInpUri.c_str());
    }
    return res;
}

void AVisWidget::OnUpdated_X(int aX)
{
}

void AVisWidget::OnUpdated_Y(int aY)
{
}

void AVisWidget::OnUpdated_W(int aY)
{
}

void AVisWidget::OnUpdated_H(int aY)
{
}

void AVisWidget::OnUpdated()
{
    MVisContainer* cont = GetVisContainer();
    // Data providers updated - update the data
    // X coord
    bool res;
    int sX;
    res = GetDataInt("Inp_X/Int/PinData", sX);
    if (res && sX != iX) {
	iX = sX;
	OnUpdated_X(iX);
    }
    // Y coord
    int sY;
    res = GetDataInt("Inp_Y/Int/PinData", sY);
    if (res && sY != iY) {
	iY = sY;
	OnUpdated_Y(iY);
    }
    // Width
    int sW;
    res = GetDataInt("Inp_W/Int/PinData", sW);
    if (res && sW != iW) {
	iW = sW;
	OnUpdated_W(iW);
    }
    // Heigth
    int sH;
    res = GetDataInt("Inp_H/Int/PinData", sH);
    if (res && sH != iH) {
	iH = sH;
	OnUpdated_H(iH);
    }
}

void AVisWidget::OnActivated()
{
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

VisDrwArea::VisDrwArea(): DrawingArea()
{
}

bool VisDrwArea::on_expose_event(GdkEventExpose* aEvent)
{
    DrawingArea::on_expose_event(aEvent);
    Glib::RefPtr<Gdk::Window> drw = get_window();
    Glib::RefPtr<Gtk::Style> style = get_style(); 	
    Glib::RefPtr<Gdk::GC> gc = style->get_fg_gc(get_state());
    Allocation alc = get_allocation();
    drw->draw_rectangle(gc, false, 0, 0, alc.get_width() - 1, alc.get_height() - 1);
}

void VisDrwArea::on_size_allocate(Allocation& aAlloc)
{
    DrawingArea::on_size_allocate(aAlloc);
}

void VisDrwArea::on_size_request(Requisition* aReq)
{
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
    iWidget = new VisDrwArea(); 
    iWidget->set_size_request(iW, iH);
    iWidget->show();
}

AVisDrawing::AVisDrawing(Elem* aMan, MEnv* aEnv): AVisWidget(Type(), aMan, aEnv)
{
    SetEType(AVisWidget::PEType());
    SetParent(AVisWidget::PEType());
    iWidget = new VisDrwArea(); 
    iWidget->set_size_request(iW, iH);
    iWidget->show();
}

void *AVisDrawing::DoGetObj(const char *aName, TBool aIncUpHier, const RqContext* aCtx)
{
    void* res = NULL;
    if (strcmp(aName, Type()) == 0) {
	res = this;
    } 
    else {
	res = AVisWidget::DoGetObj(aName, aIncUpHier);
    }
    return res;
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

void AVisDrawing::OnUpdated_W(int aData)
{
    Allocation alc = iWidget->get_allocation();
    Container* parent = iWidget->get_parent();
    VisDrwArea* drw = GetDrawing();
    drw->set_size_request(iW, iH);
    drw->queue_draw_area(alc.get_x(), alc.get_y(), iW, iH);
}

void AVisDrawing::OnUpdated_H(int aData)
{
    Allocation alc = iWidget->get_allocation();
    Container* parent = iWidget->get_parent();
    VisDrwArea* drw = GetDrawing();
    drw->set_size_request(iW, iH);
    drw->queue_draw_area(alc.get_x(), alc.get_y(), iW, iH);
}

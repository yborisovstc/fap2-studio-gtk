
#ifndef __FAPSTU_GTK_DESVIS_H
#define __FAPSTU_GTK_DESVIS_H

#include <elem.h>
#include <mdes.h>
#include <mdata.h>
#include "msdesenv.h"
#include <gtkmm/fixed.h>
#include <gtkmm/drawingarea.h>

// DES GTK visualization elems

using namespace Gtk;

class MVisChild 
{
    public:
	enum TPar {
	    EPar_Any,
	    EPar_X,
	    EPar_Y
	};
    public:
	static const char* Type() { return "MVisChild";};
	virtual Widget& GetChild() = 0;
	virtual int GetParInt(TPar) = 0;
};

class MVisContainer 
{
    public:
	static const char* Type() { return "MVisContainer";};
	virtual Container& GetContainer() = 0;
	virtual void OnChildChanged(Widget* aChild, MVisChild::TPar aPar) = 0;
};

// Agent of top level window
class AWindow: public Elem, public MVisContainer
{
    public:
	static const char* Type() { return "AWindow";};
	static string PEType();
	AWindow(const string& aName = string(), Elem* aMan = NULL, MEnv* aEnv = NULL, MSDesEnv* aSDesEnv = NULL);
	AWindow(Elem* aMan = NULL, MEnv* aEnv = NULL, MSDesEnv* aSDesEnv = NULL);
	// From MVisContainer
	virtual Container& GetContainer();
	virtual void OnChildChanged(Widget* aChild, MVisChild::TPar aPar);
    protected:
	// From Base
	virtual void *DoGetObj(const char *aName, TBool aIncUpHier = ETrue, const RqContext* aCtx = NULL);
    protected:
	MSDesEnv* iSDesEnv;
};


// Base of Agent of widgets
class AVisWidget: public Elem, public MVisChild, public MACompsObserver, public MDesObserver
{
    private:
	// Data provider base
	class DataProv {
	    public:
		void SetHost(AVisWidget* aHost)  {iHost = aHost;};
	    protected:
		AVisWidget* iHost;
	};
	class ParentSize {
	    public:
	    enum TData { ED_W, ED_H };
	    ParentSize() {};
	    void SetData(TData aData, AVisWidget* aHost) { iData = aData; iHost = aHost;};
	    protected:
	    TData iData;
	    AVisWidget* iHost;
	};
	class ParentSizeProv: public ParentSize, public MDIntGet {
	    public:
	    // From MDIntGet
	    virtual TInt Value();
	};
	class ParentSizeProvVar: public ParentSize, public MDVarGet, public MDtGet<Sdata<int> > {
	    public:
	    // From MDVarGet
	    virtual string VarGetIfid() const;
	    virtual void *DoGetDObj(const char *aName);
	    // From MDtGet
	    virtual void DtGet(Sdata<int>& aData);
	};
	// Data provider for GdkEventButton
	class EventButtonProv: public DataProv, public MDVarGet, public MDtGet<NTuple> {
	    public:
	    // From MDVarGet
	    virtual string VarGetIfid() const;
	    virtual void *DoGetDObj(const char *aName);
	    // From MDtGet
	    virtual void DtGet(NTuple& aData);
	};
    public:
	static const char* Type() { return "AVisWidget";};
	static string PEType();
	AVisWidget(const string& aName = string(), Elem* aMan = NULL, MEnv* aEnv = NULL);
	AVisWidget(Elem* aMan = NULL, MEnv* aEnv = NULL);
	void Construct();
	// From MVisChild
	virtual Widget& GetChild();
	virtual int GetParInt(TPar);
	// From MACompsObserver
	virtual TBool HandleCompChanged(Elem& aContext, Elem& aComp);
	// From MDesObserver
	virtual void OnUpdated();
	virtual void OnActivated();
    protected:
	TInt GetParData(ParentSizeProv::TData aData);
	void GetBtnPressEvent(NTuple& aData) { aData = mBtnPressEvt;};
	// From Base
	virtual void *DoGetObj(const char *aName, TBool aIncUpHier = ETrue, const RqContext* aCtx = NULL);
	// Ifaces cache
	virtual void UpdateIfi(const string& aName, const RqContext* aCtx = NULL);
	// Widget events handling
	bool OnButtonPress(GdkEventButton* aEvent);
    protected:
	virtual void OnUpdated_X(int aData);
	virtual void OnUpdated_Y(int aData);
	virtual void OnUpdated_W(int aData);
	virtual void OnUpdated_H(int aData);
    private:
	Elem* Host();
	bool GetDataInt(const string& aInpUri, int& aData);
	MVisContainer* GetVisContainer();
    protected:
	Widget* iWidget;
	ParentSizeProv iParProvW;
	ParentSizeProv iParProvH;
	ParentSizeProvVar iParProvVarW;
	ParentSizeProvVar iParProvVarH;
	EventButtonProv mBtnPressEvtProv;
	int iY;
	int iX;
	int iW;
	int iH;
	int iBtnPressEvent;
	NTuple mBtnPressEvt;
};

// Agent of container with fixed layout
class AVisFixed: public AVisWidget, public MVisContainer
{
    public:
	static const char* Type() { return "AVisFixed";};
	static string PEType();
	AVisFixed(const string& aName = string(), Elem* aMan = NULL, MEnv* aEnv = NULL);
	AVisFixed(Elem* aMan = NULL, MEnv* aEnv = NULL);
	Fixed* GetFixed();
	// From MVisContainer
	virtual Container& GetContainer();
	virtual void OnChildChanged(Widget* aChild, MVisChild::TPar aPar);
    protected:
	// From Base
	virtual void *DoGetObj(const char *aName, TBool aIncUpHier = ETrue, const RqContext* aCtx = NULL);
};

class VisDrwArea: public DrawingArea
{
    friend class AVisDrawing;
    public:
	VisDrwArea();
	virtual bool on_expose_event(GdkEventExpose* aEvent);
	virtual void on_size_allocate(Allocation& aAlloc);
	virtual void on_size_request(Requisition* aReq);
};

// Agent of drawing area
class AVisDrawing: public AVisWidget
{
    public:
	static const char* Type() { return "AVisDrawing";};
	static string PEType();
	AVisDrawing(const string& aName = string(), Elem* aMan = NULL, MEnv* aEnv = NULL);
	AVisDrawing(Elem* aMan = NULL, MEnv* aEnv = NULL);
	VisDrwArea* GetDrawing();
    protected:
	virtual void OnUpdated_X(int aX);
	virtual void OnUpdated_Y(int aY);
	virtual void OnUpdated_W(int aData);
	virtual void OnUpdated_H(int aData);
    protected:
	// From Base
	virtual void *DoGetObj(const char *aName, TBool aIncUpHier = ETrue, const RqContext* aCtx = NULL);
};


#endif

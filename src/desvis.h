
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
using namespace Gdk;

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
class AWindow: public Elem, public MVisContainer, public MAgent
{
    public:
	static const char* Type() { return "AWindow";};
	static string PEType();
	AWindow(const string& aName = string(), MElem* aMan = NULL, MEnv* aEnv = NULL, MSDesEnv* aSDesEnv = NULL);
	AWindow(MElem* aMan = NULL, MEnv* aEnv = NULL, MSDesEnv* aSDesEnv = NULL);
	// From MVisContainer
	virtual Container& GetContainer();
	virtual void OnChildChanged(Widget* aChild, MVisChild::TPar aPar);
	// From MAgent
	MIface* MAgent_DoGetIface(const string& aName) override;
    protected:
	// From Base
	virtual void *DoGetObj(const char *aName);
    protected:
	MSDesEnv* iSDesEnv;
};


// Base of Agent of widgets
class AVisWidget: public Elem, public MVisChild, public MACompsObserver, public MAgent
{
    public:
	typedef std::map<Gtk::StateType, string>  tStatesMap;
    protected:
	class DesObserver: public MDesObserver {
	    public:
		DesObserver(AVisWidget& aHost): mHost(aHost) {}
		// From MDesObserver
		virtual void OnUpdated() { mHost.doOnUpdated();}
		virtual void OnActivated() { mHost.doOnActivated();}
		virtual MIface* Call(const string& aSpec, string& aRes) {return NULL;}
		virtual string Mid() const {return string();}
	    private:
		AVisWidget& mHost;
	};
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
	    MIface* MDIntGet_Call(const string& aSpec, string& aRes) override {return NULL;}
	    string MDIntGet_Mid() const override {return string();}
	};
	class ParentSizeProvVar: public ParentSize, public MDVarGet, public MDtGet<Sdata<int> > {
	    public:
	    // From MDVarGet
	    virtual string VarGetIfid();
	    virtual void *DoGetDObj(const char *aName);
	    // From MDtGet
	    virtual void DtGet(Sdata<int>& aData);
	};
	// Data provider for ButtonPress GdkEventButton
	class EventButtonProv: public DataProv, public MDVarGet, public MDtGet<NTuple> {
	    public:
	    // From MDVarGet
	    virtual string VarGetIfid();
	    virtual void *DoGetDObj(const char *aName);
	    // From MDtGet
	    virtual void DtGet(NTuple& aData);
	};
	// Data provider for ButtonRelease GdkEventButton
	class EventButtonReleaseProv: public DataProv, public MDVarGet, public MDtGet<NTuple> {
	    public:
	    // From MDVarGet
	    virtual string VarGetIfid();
	    virtual void *DoGetDObj(const char *aName);
	    // From MDtGet
	    virtual void DtGet(NTuple& aData);
	};
	// Data provider for GdkEventMotion
	class EventMotionProv: public DataProv, public MDVarGet, public MDtGet<NTuple> {
	    public:
	    // From MDVarGet
	    virtual string VarGetIfid();
	    virtual void *DoGetDObj(const char *aName);
	    // From MDtGet
	    virtual void DtGet(NTuple& aData);
	};
	// Data provider for Allocation
	class AllocationProv: public DataProv, public MDVarGet, public MDtGet<NTuple> {
	    public:
	    // From MDVarGet
	    virtual string VarGetIfid();
	    virtual void *DoGetDObj(const char *aName);
	    // From MDtGet
	    virtual void DtGet(NTuple& aData);
	};
    public:
	static const char* Type() { return "AVisWidget";};
	static string PEType();
	AVisWidget(const string& aName = string(), MElem* aMan = NULL, MEnv* aEnv = NULL);
	AVisWidget(MElem* aMan = NULL, MEnv* aEnv = NULL);
	void Construct();
	// From MVisChild
	virtual Widget& GetChild();
	virtual int GetParInt(TPar);
	// From MACompsObserver
	virtual TBool HandleCompChanged(MElem& aContext, MElem& aComp, const string& aName=string());
	// From MAgent
	MIface* MAgent_DoGetIface(const string& aName) override;
    protected:
	virtual TInt GetParData(ParentSize::TData aData);
	void GetBtnPressEvent(NTuple& aData) { aData = mBtnPressEvt;};
	void GetBtnReleaseEvent(NTuple& aData) { aData = mBtnReleaseEvt;};
	void GetMotionEvent(NTuple& aData) { aData = mMotionEvt;};
	void ActivateDeps(const string& aUri);
	// From Base
	virtual void *DoGetObj(const char *aName);
	// Ifaces cache
	void UpdateIfi(const string& aName, const TICacheRCtx& aCtx) override;
	// Widget events handling
	bool OnButtonPress(GdkEventButton* aEvent);
	bool OnButtonRelease(GdkEventButton* aEvent);
	bool OnMotion(GdkEventMotion* aEvent);
	// From MDesObserver
	virtual void doOnUpdated();
	virtual void doOnActivated();
    protected:
	virtual void OnUpdated_X(int aOldData);
	virtual void OnUpdated_Y(int aOldData);
	virtual void OnUpdated_W(int aOldData);
	virtual void OnUpdated_H(int aOldData);
	virtual bool HandleButtonPress(GdkEventButton* aEvent);
	virtual bool HandleButtonRelease(GdkEventButton* aEvent);
	virtual bool HandleMotion(GdkEventMotion* aEvent);
    private:
	MElem* Host();
	bool GetDataInt(const string& aInpUri, int& aData);
	bool GetInpState(const string& aInpUri, Gtk::StateType& aData);
	MVisContainer* GetVisContainer();
    protected:
	Widget* iWidget;
	DesObserver mDesObserver;
	ParentSizeProv iParProvW;
	ParentSizeProv iParProvH;
	ParentSizeProvVar iParProvVarW;
	ParentSizeProvVar iParProvVarH;
	EventButtonProv mBtnPressEvtProv;
	EventButtonReleaseProv mBtnReleaseEvtProv;
	EventMotionProv mMotionEvtProv;
	AllocationProv mAllocationProv;
	int iY;
	int iX;
	int iW;
	int iH;
	NTuple mBtnPressEvt;
	NTuple mBtnReleaseEvt;
	NTuple mMotionEvt;
	NTuple mAllocation;
	static bool mInit;
	static tStatesMap  mStatesMap;
};

// Agent of container with fixed layout
class AVisFixed: public AVisWidget, public MVisContainer
{
    public:
	static const char* Type() { return "AVisFixed";};
	static string PEType();
	AVisFixed(const string& aName = string(), MElem* aMan = NULL, MEnv* aEnv = NULL);
	AVisFixed(MElem* aMan = NULL, MEnv* aEnv = NULL);
	Fixed* GetFixed();
	// From MVisContainer
	virtual Container& GetContainer();
	virtual void OnChildChanged(Widget* aChild, MVisChild::TPar aPar);
    protected:
	// From Base
	virtual void *DoGetObj(const char *aName);
};

class VisDrwArea: public DrawingArea
{
    friend class AVisDrawing;
    public:
	VisDrwArea(AVisWidget* aHost);
	virtual bool on_expose_event(GdkEventExpose* aEvent);
	virtual void on_size_allocate(Allocation& aAlloc);
	virtual void on_size_request(Requisition* aReq);
	AVisWidget* mHost;
};

// Interface of drawing area, that uses drawing elements to performs draw 
class MVisDrawingArea
{
    public:
	static const char* Type() { return "MVisDrawingArea";};
	virtual void GetAllocation(Rectangle& aRect) = 0;
	virtual Glib::RefPtr<Gdk::Window> GetWindow() = 0;
	// [YB] Isn't fully conpliant to agents interaction approach, to reconsider
	//virtual Glib::RefPtr<Widget> GetWidget() = 0;
};


// Interface of drawing element, that performs partial draw in drawing area
class MVisDrawingElem
{
    public:
	static const char* Type() { return "MVisDrawingElem";};
	virtual void OnExpose(GdkEventExpose* aEvent) = 0;
	virtual bool OnAreaButtonPress(GdkEventButton* aEvent) = 0;
	virtual bool OnAreaButtonRelease(GdkEventButton* aEvent) = 0;
	virtual bool OnAreaMotion(GdkEventMotion* aEvent) = 0;
};

// Agent of drawing area
class AVisDrawing: public AVisWidget, public MVisDrawingArea
{
    public:
	static const char* Type() { return "AVisDrawing";};
	static string PEType();
	AVisDrawing(const string& aName = string(), MElem* aMan = NULL, MEnv* aEnv = NULL);
	AVisDrawing(MElem* aMan = NULL, MEnv* aEnv = NULL);
	VisDrwArea* GetDrawing();
    protected:
	MElem::TIfRange GetDrawingElems();
    protected:
	// From AVisWidget
	virtual void OnUpdated_X(int aOldData);
	virtual void OnUpdated_Y(int aOldData);
	virtual void OnUpdated_W(int aOldData);
	virtual void OnUpdated_H(int aOldData);
	virtual bool HandleButtonPress(GdkEventButton* aEvent);
	virtual bool HandleButtonRelease(GdkEventButton* aEvent);
	virtual bool HandleMotion(GdkEventMotion* aEvent);
    protected:
	// From Base
	virtual void *DoGetObj(const char *aName);
	// From MVisDrawingArea
	virtual void GetAllocation(Rectangle& aRect);
	virtual Glib::RefPtr<Gdk::Window> GetWindow();
	// virtual Glib::RefPtr<Widget> GetWidget();
};


// Agent of drawing element
class AVisDrawingElem: public AVisWidget, public MVisDrawingElem
{
    public:
	static const char* Type() { return "AVisDrawingElem";};
	static string PEType();
	AVisDrawingElem(const string& aName = string(), MElem* aMan = NULL, MEnv* aEnv = NULL);
	AVisDrawingElem(MElem* aMan = NULL, MEnv* aEnv = NULL);
    protected:
	void OnAllocUpdated(const Rectangle& aOldAlloc);
	MVisDrawingArea* GetDrawingArea();
	//bool OnAreaButtonPress(GdkEventButton* aEvent);
    public:
	// From MVisDrawingElem
	virtual void OnExpose(GdkEventExpose* aEvent);
	virtual bool OnAreaButtonPress(GdkEventButton* aEvent);
	virtual bool OnAreaButtonRelease(GdkEventButton* aEvent);
	virtual bool OnAreaMotion(GdkEventMotion* aEvent);
    protected:
	// From Base
	virtual void *DoGetObj(const char *aName);
	// From MACompsObserver
	//virtual TBool HandleCompChanged(MElem& aContext, MElem& aComp);
	// From AVisWidget
	virtual void OnUpdated_X(int aOldData);
	virtual void OnUpdated_Y(int aOldData);
	virtual void OnUpdated_W(int aOldData);
	virtual void OnUpdated_H(int aOldData);
	virtual TInt GetParData(ParentSize::TData aData);
};



#endif

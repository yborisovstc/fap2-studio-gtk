
#ifndef __FAP2STU_INCAPSDRP_H
#define __FAP2STU_INCAPSDRP_H

#include "sysdrp.h"

using namespace Gtk;

class MCapsCrp
{
    public:
	enum TSectId { ESI_Left = 0, ESI_Right = 1 };
    public:
	static const string& Type();
    public:
	virtual Allocation& SectAlloc(TSectId aSetId) = 0;
	virtual void GetAlloc(Allocation& aAllc) const = 0;
};

// CRP for Capsule
// TODO [YB] Not used at the moment, to consider remove
class CapsCrp: public Widget, public MCrp, public MCapsCrp
{
    public:
	static const string& Type();
	static string EType();
    public:
	CapsCrp(Elem* aElem);
	virtual ~CapsCrp();
	// From MCrp
	virtual Gtk::Widget& Widget();
	virtual void *DoGetObj(const string& aName);
	virtual tSigButtonPressName SignalButtonPressName();
	virtual tSigButtonPress SignalButtonPress();
	virtual tSigUpdated SignalUpdated();
	virtual bool IsActionSupported(Action aAction);
	virtual void GetContentUri(GUri& aUri);
	virtual bool Dragging();
	virtual void SetHighlighted(bool aSet);
	virtual void SetErroneous(bool aSet);
	virtual Elem* Model();
	virtual void SetLArea(int aArea);
	virtual int GetLArea() const;
	virtual void SetDnDTargSupported(int aTarg);
	virtual bool IsDnDTargSupported(TDnDTarg aTarg) const;
	virtual bool IsIntersected(int aX, int aY) const;
	// From Widget
	virtual bool on_button_press_event(GdkEventButton* aEvent);
	// From MCapsCrp
	virtual Allocation& SectAlloc(TSectId aSetId);
	virtual void GetAlloc(Allocation& aAllc) const;
    protected:
	virtual bool on_expose_event(GdkEventExpose* event);
	virtual void on_size_request(Gtk::Requisition* aRequisition);
	bool IsPointIn(int aX, int aY) const;
	static bool IsInRect(int aX, int aY, const Allocation& aAllc);
    private:
	Elem* iElem;
	MCrp::tSigUpdated iSigUpdated;
	MCrp::tSigButtonPress iSigButtonPress;
	MCrp::tSigButtonPressName iSigButtonPressName;
	Allocation iLeftSectAlc;
	Allocation iRightSectAlc;
	int iDnDSupp;
};

class IncapsDrp: public SysDrp
{
    public:
	static std::string Type() { return string("IncapsDrp");};
    public:
	static string EType();
    public:
	IncapsDrp(Elem* aElem, const MCrpProvider& aCrpProv, MSEnv& aStEnv);
	virtual ~IncapsDrp();
    protected:
	virtual void Construct();
	void PreLayoutRps();
	TBool IsTypeOf(const string& aType, const string& aParent) const;
	virtual Elem* GetCompOwning(Elem* aElem);
	// From MCrpMgr
	virtual bool IsTypeAllowed(const std::string& aType) const;
	// From MDrp
	virtual void *DoGetObj(const string& aName);
	//  From SysDrp
	virtual void on_size_allocate(Gtk::Allocation& aAlloc);
	virtual void on_size_request(Gtk::Requisition* aRequisition);
    protected:
	static const string KCapsUri;
	static const string KIncapsType;
	static const string KExtdType;
	CapsCrp* mCapsLCrp;
};

#endif

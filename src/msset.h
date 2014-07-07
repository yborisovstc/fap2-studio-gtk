
#ifndef __FAP2STU_GTK_MSSET_H
#define __FAP2STU_GTK_MSSET_H

#include <gtkmm.h>

// Settings interface

template <typename T> class MStSetting
{
    public:
	typedef sigc::signal<void> tSigChanged;
	virtual const T& Get(const T& aSetting) const = 0;
	virtual void Set(const T& aSetting) = 0;
	virtual tSigChanged SigChanged() = 0;
};

class MStSettings
{
    public:
	enum TStSett {
	    ESts_EnablePhenoModif
	};
    public:
	virtual void* DoGetSetting(TStSett aSettId) = 0;
	template <typename T> MStSetting<T>& GetSetting(TStSett aSettId, MStSetting<T>& aSetting) {
	    return *((MStSetting<T>*) DoGetSetting(aSettId));
	};
};

#endif

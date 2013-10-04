
#ifndef __FAP2STU_DLGBASE_H
#define __FAP2STU_DLGBASE_H

#include <gtkmm/dialog.h>
#include <gtkmm/entry.h>
#include <gtkmm/textview.h>


using namespace std;

// Parameter edit dialog 
class ParEditDlg: public Gtk::Dialog
{
    public:
	enum {
	    EActionCancel,
	    EActionOK
	};
    public:
	ParEditDlg(const string& aTitle, const string& aData);
	void GetData(string& aData);
    protected:
	Gtk::Entry* iEntry;
};


// Text edit dialog 
class TextEditDlg: public Gtk::Dialog
{
    public:
	enum {
	    EActionCancel,
	    EActionOK
	};
    public:
	TextEditDlg(const string& aTitle, const string& aData);
	void GetData(string& aData);
    protected:
	Gtk::TextView* iTextView;
};


#endif

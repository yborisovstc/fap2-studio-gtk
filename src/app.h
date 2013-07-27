
#ifndef __FAPSTU_GTK_APP_H
#define __FAPSTU_GTK_APP_H

#include "mainwnd.h"


class App
{
    public:
	App();
	virtual ~App();

	MainWnd& Wnd() const {return *iMainWnd;};
    private:
	Env* iEnv;
	MainWnd* iMainWnd;
};


#endif 

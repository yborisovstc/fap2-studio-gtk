
#ifndef __FAPSTU_GTK_DESSYNC_H
#define __FAPSTU_GTK_DESSYNC_H

#include <elem.h>
#include "msdesenv.h"

// DES synchronizer agent

class ADesSync: public Elem
{
    public:
	static const char* Type() { return "ADesSync";};
	static string PEType();
	ADesSync(const string& aName = string(), Elem* aMan = NULL, MEnv* aEnv = NULL, MSDesEnv* aSDesEnv = NULL);
    protected:
	// From Base
	virtual void *DoGetObj(const char *aName, TBool aIncUpHier = ETrue, const RqContext* aCtx = NULL);
	// Signal handlers:
	void on_action_run();
	void on_action_stop();
    private:
	MSDesEnv* iSDesEnv;
};


#endif

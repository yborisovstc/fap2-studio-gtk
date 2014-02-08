#ifndef __FAPSTU_GTK_MDLPROV_H
#define __FAPSTU_GTK_MDLPROV_H

#include <prov.h>
#include <msdesenv.h>

// Default provider for model
//
class MdlProv: public GProvider
{
    public:
	MdlProv(const string &aName, MSDesEnv* aSDesEnv, MEnv* aEnv);
	// From MProvider
	virtual Elem* CreateNode(const string& aType, const string& aName, Elem* aMan, MEnv* aEnv);
	virtual Elem* GetNode(const string& aUri);
	virtual void AppendNodesInfo(vector<string>& aInfo);
    private:
	vector<string> iNodesInfo;
	MSDesEnv* iSDesEnv;
};


#endif

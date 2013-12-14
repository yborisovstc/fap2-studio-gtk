
#include "mdlprov.h"
#include "dessync.h"



MdlProv::MdlProv(const string &aName, MSDesEnv* aSDesEnv): GProvider(aName), iSDesEnv(aSDesEnv)
{
}

Elem* MdlProv::CreateNode(const string& aType, const string& aName, Elem* aMan, MEnv* aEnv)
{
    Elem* res = NULL;
    if (aType.compare(ADesSync::Type()) == 0) {
	res = new ADesSync(aName, aMan, aEnv, iSDesEnv);
    }
    return res;
}

void MdlProv::AppendNodesInfo(vector<string>& aInfo)
{
    aInfo.push_back(ADesSync::Type());
}


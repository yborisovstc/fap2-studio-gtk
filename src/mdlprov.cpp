
#include "mdlprov.h"
#include "dessync.h"
#include "desvis.h"



MdlProv::MdlProv(const string &aName, MSDesEnv* aSDesEnv): GProvider(aName), iSDesEnv(aSDesEnv)
{
}

Elem* MdlProv::CreateNode(const string& aType, const string& aName, Elem* aMan, MEnv* aEnv)
{
    Elem* res = NULL;
    if (aType.compare(ADesSync::Type()) == 0) {
	res = new ADesSync(aName, aMan, aEnv, iSDesEnv);
    }
    else if (aType.compare(AWindow::Type()) == 0) {
	res = new AWindow(aName, aMan, aEnv, iSDesEnv);
    }
    else if (aType.compare(AVisFixed::Type()) == 0) {
	res = new AVisFixed(aName, aMan, aEnv);
    }
    else if (aType.compare(AVisDrawing::Type()) == 0) {
	res = new AVisDrawing(aName, aMan, aEnv);
    }
    return res;
}

void MdlProv::AppendNodesInfo(vector<string>& aInfo)
{
    aInfo.push_back(ADesSync::Type());
    aInfo.push_back(AWindow::Type());
}



#include "mdlprov.h"
#include "dessync.h"
#include "desvis.h"



MdlProv::MdlProv(const string &aName, MSDesEnv* aSDesEnv, MEnv* aEnv): GProvider(aName, aEnv), iSDesEnv(aSDesEnv)
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
    else if (aType.compare(AVisDrawingElem::Type()) == 0) {
	res = new AVisDrawingElem(aName, aMan, aEnv);
    }
    if (res != NULL) {
	MElem* parent = GetNode(aType);
	if (parent != NULL) {
	    parent->AppendChild(res);
	}
    }
    return res;
}

Elem* MdlProv::GetNode(const string& aUri){
    MProvider* prov = iEnv->Provider();
    Elem* res = NULL;
    if (iReg.count(aUri) > 0) {
	res = iReg.at(aUri);
    }
    else { 
	Elem* parent = NULL;
	if (aUri.compare(ADesSync::Type()) == 0) {
	    parent = prov->GetNode("Elem");
	    res = new ADesSync(NULL, iEnv, iSDesEnv);
	}
	else if (aUri.compare(AWindow::Type()) == 0) {
	    parent = prov->GetNode("Elem");
	    res = new AWindow(NULL, iEnv, iSDesEnv);
	}
	else if (aUri.compare(AVisWidget::Type()) == 0) {
	    parent = prov->GetNode("Elem");
	    res = new AVisWidget(NULL, iEnv);
	}
	else if (aUri.compare(AVisFixed::Type()) == 0) {
	    parent = prov->GetNode("AVisWidget");
	    res = new AVisFixed(NULL, iEnv);
	}
	else if (aUri.compare(AVisDrawing::Type()) == 0) {
	    parent = prov->GetNode("AVisWidget");
	    res = new AVisDrawing(NULL, iEnv);
	}
	else if (aUri.compare(AVisDrawingElem::Type()) == 0) {
	    parent = prov->GetNode("AVisWidget");
	    res = new AVisDrawingElem(NULL, iEnv);
	}
	if (res != NULL) {
	    if (parent != NULL) {
		parent->AppendChild(res);
	    }
	    iReg.insert(TRegVal(aUri, res));
	}
    }
    return res;
}


void MdlProv::AppendNodesInfo(vector<string>& aInfo)
{
    aInfo.push_back(ADesSync::Type());
    aInfo.push_back(AWindow::Type());
    aInfo.push_back(AVisWidget::Type());
    aInfo.push_back(AVisFixed::Type());
    aInfo.push_back(AVisDrawing::Type());
    aInfo.push_back(AVisDrawingElem::Type());
}


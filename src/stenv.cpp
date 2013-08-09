#include "stenv.h"
#include "provdef.h"

StEnv::StEnv()
{
    iCrpProv = new DefCrpProv();
}

StEnv::~StEnv()
{
    delete iCrpProv;
}

MCrpProvider& StEnv::CrpProvider()
{
    return *iCrpProv;
}


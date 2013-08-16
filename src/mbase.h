
#ifndef __FAP2STU_MBASE_H
#define __FAP2STU_MBASE_H

#include <string>

class MBase
{
    public:
	virtual ~MBase() {};
	template <class T> T* GetObj(T* aInst) {return aInst = static_cast<T*>(DoGetObj(aInst->Type()));};
	virtual void *DoGetObj(const std::string& aName) = 0;

};

#endif


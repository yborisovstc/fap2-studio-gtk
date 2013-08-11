
#ifndef __FAP2STU_GTK_MENV_H
#define __FAP2STU_GTK_MENV_H

// Environmet interface

class MCrpProvider;
class MDrpProvider;

class MSEnv
{
    public:
	virtual MCrpProvider& CrpProvider() = 0;
	virtual MDrpProvider& DrpProvider() = 0;
};

#endif

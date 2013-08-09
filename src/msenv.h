
#ifndef __FAP2STU_GTK_MENV_H
#define __FAP2STU_GTK_MENV_H

// Environmet interface

class MCrpProvider;

class MSEnv
{
    public:
	virtual MCrpProvider& CrpProvider() = 0;
};

#endif

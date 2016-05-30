#ifndef __FAP2STU_GCSESSION_H
#define __FAP2STU_GCSESSION_H

#include <fapcsbase.h>
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/in.h>

#define PORT 30667

using namespace std;

class DesSrv {
    public:
        DesSrv();
        DesSrv(int aPort);
        ~DesSrv();
	void Construct();
        bool AcceptAndDispatch();
    private:
	int mPort;
        int mServerSock, mClientSock;
        struct sockaddr_in mServerAddr, mClientAddr;
};


// Session supported running in glib events loop
//
class GcSession : public CSessionBase {
    public:
        GcSession();
        GcSession(int sock);
        virtual ~GcSession();
        virtual bool Run();
};

#endif

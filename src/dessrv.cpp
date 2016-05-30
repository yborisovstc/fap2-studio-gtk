#include <errno.h>
#include <glibmm.h>
#include "dessrv.h"

using namespace std;

DesSrv::DesSrv(int aPort): mPort(aPort)
{
}

DesSrv::DesSrv(): mPort(0)
{
}

void DesSrv::Construct()
{
    //For setsock opt (REUSEADDR)
    int yes = 1;
    //Init serverSock and start listen()'ing
    mServerSock = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
    memset(&mServerAddr, 0, sizeof(sockaddr_in));
    mServerAddr.sin_family = AF_INET;
    mServerAddr.sin_addr.s_addr = INADDR_ANY;
    int port = mPort == 0 ? PORT : mPort;
    mServerAddr.sin_port = htons(port);
    //Avoid bind error if the socket was not close()'d last time;
    setsockopt(mServerSock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
    if (bind(mServerSock, (struct sockaddr *) &mServerAddr, sizeof(sockaddr_in)) < 0) {
        cerr << "Failed to bind";
    }
    listen(mServerSock, 5);
}

DesSrv::~DesSrv()
{
    if (mServerSock != 0) {
	close(mServerSock);
    }
}

// Main loop: Blocks at accept(), until a new connection arrives.
// When it happens, create a new thread to handle the new client.
bool DesSrv::AcceptAndDispatch() {
    bool res = true;
    socklen_t cliSize = sizeof(sockaddr_in);
    int sock = accept(mServerSock, (struct sockaddr *) &mClientAddr, &cliSize);
    if (sock >= 0) {
	GcSession* cs = new GcSession(sock);
	Glib::signal_idle().connect(sigc::mem_fun(*cs, &GcSession::Run));
    } else if (errno != EAGAIN) {
	res = false;
	cerr << "Error on accept";
    }
    return res;
}



const int KBufSize = 2048;

GcSession::GcSession(): CSessionBase()
{
}

GcSession::GcSession(int sock): CSessionBase(sock)
{
}

GcSession::~GcSession()
{
}

bool GcSession::Run()
{
    char buffer[KBufSize-25];
    int index, n;
    bool res = true;
    while (1) {
	memset(buffer, 0, sizeof buffer);
	n = recv(mSock, buffer, sizeof buffer, MSG_DONTWAIT);
	//CSessionBase disconnected?
	if(n == 0) {
	    cout << "CSessionBase [" << mId << "] disconnected" << endl;
	    close(mSock);
	    //Remove client in Static sClients <vector> (Critical section!)
	    index = FindSessionIndex(this);
	    cout << "Removed client session, id: " << sClients[index]->mId << endl;
	    sClients.erase(sClients.begin() + index);
	    delete this;
	    res = false;
	    break;
	} else if (n < 0) {
	    if (errno != EAGAIN) {
		cerr << "Error while receiving message from client: " << mId << endl;
	    }
	} else {
	    //Message received.
	    HandleMessage(string(buffer));
	}
    }
    return res;
}



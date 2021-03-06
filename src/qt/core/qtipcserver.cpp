/*******************************************************************************************
 
			Hash(BEGIN(Satoshi[2010]), END(W.J.[2012])) == Videlicet[2014] ++
   
 [Learn and Create] Viz. http://www.opensource.org/licenses/mit-license.php
  
*******************************************************************************************/

#include <boost/algorithm/string.hpp>
#include <boost/interprocess/ipc/message_queue.hpp>
#include <boost/tokenizer.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include "../util/ui_interface.h"
#include "../util/util.h"
#include "qtipcserver.h"

using namespace boost::interprocess;
using namespace boost::posix_time;
using namespace boost;
using namespace std;

void ipcShutdown()
{
    message_queue::remove(NEXUS_URI_QUEUE_NAME);
}

void ipcThread(void* parg)
{
    message_queue* mq = (message_queue*)parg;
    char strBuf[257];
    size_t nSize;
    unsigned int nPriority;
    loop
    {
        ptime d = boost::posix_time::microsec_clock::universal_time() + millisec(100);
        if(mq->timed_receive(&strBuf, sizeof(strBuf), nSize, nPriority, d))
        {
            ThreadSafeHandleURI(std::string(strBuf, nSize));
            Sleep(1000);
        }
        if (fShutdown)
        {
            ipcShutdown();
            break;
        }
    }
    ipcShutdown();
}

void ipcInit()
{
#ifdef MAC_OSX
    // TODO: implement Nexus: URI handling the Mac Way
    return;
#endif
#ifdef WIN32
    // TODO: THOROUGHLY test boost::interprocess fix,
    // and make sure there are no Windows argument-handling exploitable
    // problems.
    return;
#endif

    message_queue* mq;
    char strBuf[257];
    size_t nSize;
    unsigned int nPriority;
    try {
        mq = new message_queue(open_or_create, NEXUS_URI_QUEUE_NAME, 2, 256);

        // Make sure we don't lose any Nexus: URIs
        for (int i = 0; i < 2; i++)
        {
            ptime d = boost::posix_time::microsec_clock::universal_time() + millisec(1);
            if(mq->timed_receive(&strBuf, sizeof(strBuf), nSize, nPriority, d))
            {
                ThreadSafeHandleURI(std::string(strBuf, nSize));
            }
            else
                break;
        }

        // Make sure only one Nexus instance is listening
        message_queue::remove(NEXUS_URI_QUEUE_NAME);
        mq = new message_queue(open_or_create, NEXUS_URI_QUEUE_NAME, 2, 256);
    }
    catch (interprocess_exception &ex) {
        return;
    }
    if (!CreateThread(ipcThread, mq))
    {
        delete mq;
    }
}

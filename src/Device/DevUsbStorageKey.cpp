// DevUsbStorageKey.cpp: implementation of the CDevUsbStorageKey class.
//
//////////////////////////////////////////////////////////////////////

#include	"../Logs.h"

#include "DevHotPlug.h"
#include <ez_fs.h>
#include "ez_system_api.h"

#include "DevUsbStorageKey.h"

#ifndef _DEBUG_THIS
//    #define _DEBUG_THIS
#endif
#ifdef _DEBUG_THIS
	#define DEB(x) x
	#define DBG(x) x
#else
	#define DEB(x)
	#define DBG(x)
#endif

#ifndef __trip
	#define __trip printf("-W-%d::%s(%d)\n", (int)time(NULL), __FILE__, __LINE__);
#endif
#ifndef __fline
	#define __fline printf("%s(%d)--", __FILE__, __LINE__);
#endif

#define ARG_USED(x) (void)&x;

PATTERN_SINGLETON_IMPLEMENT(CDevUsbStorageKey);
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
// (const char*pName, int nPriority, int nMsgQueSize = 0, DWORD dwStackSize = 0)
CDevUsbStorageKey::CDevUsbStorageKey() :CEZThread("CDevUsbStorageKey", THREAD_PRIORITY_DEFAULT, 100/*msg size*/), m_SigBuffer(2/*SIGNAL_NODE_NEW*/)
{
    m_iUser = 0;
    ez_trace("CDevUsbStorageKey Enter--------\n");
}

CDevUsbStorageKey::~CDevUsbStorageKey()
{
    ez_trace("CDevUsbStorageKey Leave--------\n");

}

EZTHREAD_BOOL CDevUsbStorageKey::Start(CEZObject * pObj, DevUsbStorageKeySignalProc_t pProc)
{
    CEZLock __lock(m_MutexSigBuffer);

    EZTHREAD_BOOL bRet = EZTHREAD_BOOL_FALSE;

    if(m_SigBuffer.Attach(pObj, pProc) < 0)
    {
        __fline;
        printf("attach error\n");
        return bRet;
    }

    if (m_iUser==0)
    {
        CreateThread();
        StartHotPlug();

        bRet = EZTHREAD_BOOL_TRUE;
    }
    else
    {

        bRet = EZTHREAD_BOOL_TRUE;

        DBG(
            __fline;
            printf("no need, CreateThread, have %d users.\n", m_iUser);
        );

    }
    m_iUser++;
DBG(
    __fline;
    printf("m_iUser:%d\n", m_iUser);
);
    return bRet;

}
EZTHREAD_BOOL CDevUsbStorageKey::Stop(CEZObject * pObj, DevUsbStorageKeySignalProc_t pProc)
{
    printf("CDevUsbStorageKey::Stop\n");
    EZTHREAD_BOOL bRet = EZTHREAD_BOOL_FALSE;

    CEZLock __lock(m_MutexSigBuffer);

    // 这不科学， 但是为了避免detach失败 而不停止线程
    // 使用者自行注意start和stop成对调用
    if(m_iUser>0)
    {
        m_iUser--;
    }


    if(m_SigBuffer.Detach(pObj, pProc) == 0)
    {
        // good
        // m_iUser--;
        bRet = EZTHREAD_BOOL_TRUE;
    }
    else
    {
        // error
        __trip;
        printf("Detach failed.\n");
    }

    if (m_iUser==0)
    {

        bRet = DestroyThread(EZTHREAD_BOOL_TRUE);
        StopHotPlug();

        if (EZTHREAD_BOOL_FALSE == bRet)
        {
            __trip;
            printf("DestroyThread failed.\n");
        }
        else
        {
            DBG(
                printf("DestroyThread OK.\n");
            );
        }
    }
    else
    {
        DBG(
            __fline;
            printf("no need, DestroyThread, have %d users.\n", m_iUser);
        );
    }

    return bRet;
}

//int ez_load_file_to_memory(const char *filename, char **result);
#define TMP_MOUNT_POINT "/tmp/gna/"
const char * const pExistFile = TMP_MOUNT_POINT"a.txt";

void CDevUsbStorageKey::ThreadProc()
{
    int ii=0;
    while (m_bLoop)
    {
        DBG(
            __fline;
            printf("CDevUsbStorageKey::ThreadProc.\n");
        );

        // 发送消息 (unsigned int msg, EZTHREAD_PARAM wpa = 0, EZTHREAD_PARAM lpa = 0, unsigned int priority = 0);
        //g_Consumers.SendMessage(100, ii, ii-1, 0);

        // 回调
        // // cmd, packet, len, time

        ii++;

        //usleep(100);
        //sleep(2);
        MSG_NODE msg;
        // EZTHREAD_BOOL_TRUE - 不阻塞
        if (EZTHREAD_BOOL_FALSE == RecvMessage(&msg, EZTHREAD_BOOL_TRUE))
        {
            // timeout, todo some maintain
            //__trip ;
            ez_sleep(1, 0);
		
            continue;
        }

        // on msg

        CEZLock l(m_LockLis);

        // use msg

        std::list<PartionNode>::iterator i= m_ListHotMsg.begin();
        if (i==m_ListHotMsg.end())
        {
            // never
            __trip;
            continue;
        }

        DBG(
            __fline;
            std::cout << "m_ListHotMsg.size():" << m_ListHotMsg.size() << std::endl;
        );

        if (MSG_HOTPLUG_USB_PARTION_ADD != i->iCmd)
        {
            __fline;
            printf("onCmd:%d, we use MSG_HOTPLUG_USB_PARTION_ADD(%d) only.\n"
                   , i->iCmd
                   , MSG_HOTPLUG_USB_PARTION_ADD);
        }
        else
        {
            // use the first msg
            DBG(
                __trip;
                i->dump();
            );

            m_SigBuffer(MSG_USB_STORAGE_KEY_FOUNDED, NULL, 0, time(NULL));

            std::string __devname = "/dev/" + i->strPartition;

            std::string strCmd = "";
            strCmd += "mkdir -p ";
            strCmd += TMP_MOUNT_POINT ;
            strCmd += " ; umount ";
            //strCmd += " ;sudo umount ";
            strCmd += TMP_MOUNT_POINT ;

            //strCmd += " ;sudo mount -t vfat ";
            strCmd += " ; mount -t vfat ";
            strCmd += __devname ;
            strCmd += " ";
            strCmd += TMP_MOUNT_POINT;

            std::string __par_mount_file;
            //
#ifdef LINUX_AM335X
            // 使用udev
            __par_mount_file = "/media/" + i->strPartition + "/a.txt";

#else
            // self mount
            system(strCmd.c_str());
            __par_mount_file = pExistFile;
#endif

            DBG(
                std::cout << strCmd << std::endl;
            );
            ez_sleep(1, 0);

            char *content = NULL;
            int file_size = 0;


            content = NULL;
            file_size = ez_load_file_to_memory(__par_mount_file.c_str(), &content);

            if (file_size <= 0)
            {
                printf("Error loading file:%s\n", __par_mount_file.c_str());
            }
            else
            {
                // file ok
                DBG(
                    printf("ez_load_file_to_memory file size:%d\n", file_size);
                );
                std::string __strFile(content, file_size);

                DBG(
                    std::cout << __strFile << std::endl;
                );

                // 回调
                m_SigBuffer(MSG_USB_STORAGE_KEY_ON_KEY_CONTENTS, __strFile.c_str(), __strFile.size(), time(NULL));

            }

            if (content != NULL)
            {
                free (content);
                content = NULL;
            }
        }

        m_ListHotMsg.erase(i);
        DBG(
            __fline;
            std::cout << "m_ListHotMsg.size():" << m_ListHotMsg.size() << std::endl;
        );
    }

    //delete cfgnetcommon;
}


void CDevUsbStorageKey::OnPartition(int iCMD, char *pPacket, int iCh, time_t msg_time)
{
    CEZLock l(m_LockLis);

    DBG(
        __fline;
        printf("CALLBACK - OnPartition: %d, 0x%0x, %d, %ld\n", iCMD, (unsigned int)pPacket, iCh, msg_time);
    );
    PartionNode __node;
    __node.iCmd = iCMD;
    __node.strPartition.assign((char *)pPacket, iCh);
    __node.ttPushed = msg_time;

    m_ListHotMsg.push_back(__node);
    DBG(
        __trip;
        __node.dump();
    );

    SendMessage(iCMD, 1, 1, 0);
}

void CDevUsbStorageKey::StartHotPlug()
{
    g_DevHotPlug.Start(this, (CDevHotPlug::DevHotPlugSignalProc_t)&CDevUsbStorageKey::OnPartition);
}

void CDevUsbStorageKey::StopHotPlug()
{
    g_DevHotPlug.Stop(this, (CDevHotPlug::DevHotPlugSignalProc_t)&CDevUsbStorageKey::OnPartition);
}

/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

void PartionNode::dump()
{
    std::cout << "iCmd:" << iCmd << std::endl;
    std::cout << "strPartition:" << strPartition << std::endl;
    std::cout << "ttPushed:" << ttPushed << std::endl;
}


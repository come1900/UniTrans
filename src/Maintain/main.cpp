/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * GeneralAgent.cpp - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: GeneralAgent.cpp 0001 2012-04-06 09:58:43Z WuJunjie $
 *
 *  Explain:
 *     -使用main线程-
 *
 *  Update:
 *     2011-10-13 22:00:11 WuJunjie Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#include "GeneralAgent.h"
#ifdef EXAMPLE_GENERALSOCKETPROCESSOR
#include "GeneralSocketProcessor.h"
#endif

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

int main(int argc, char *argv[])
{

    g_MaintainAgent.Initialize(argc, argv);

    //g_MaintainAgent.SetDefaultConfig();

    printf("ConnectorCreate\n");
    g_MaintainAgent.ConnectorCreate();
    //启动单独的处理线程
#ifdef EXAMPLE_GENERALSOCKETPROCESSOR
    g_GeneralSocketProcessor.Start();
#endif

    // 测试连接、断开
    while (1)
    {
        g_MaintainAgent.Run();

#if defined(EXAMPLE_GENERALAGENTHTTPKEEPCONNECTOR) || defined(EXAMPLE_GENERALAGENTHTTPSOCKETCONNECTOR)
        static time_t ttLastSend = 0;
        static time_t ttNow = 0;
        // 测试用，暂时写在这里
        ttNow  = time(NULL);

        const char *pJson = "[{\"reason\":\"PostTest\",\"result\":\"0\"}]";
        //const char *pUri = "/SmsOverHttp/registerURL";
        const char *pUri = "/WebService/TestIF";

        const char *http_req_head =
            "POST %s HTTP/1.1\r\n"
            "Accept: */*\r\n"
            "User-Agent: Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1; SV1; .NET CLR 2.0.50727; .NET CLR 3.0.4506.2152; .NET CLR 3.5.30729)\r\n"
            "Host: sms.ezlibs.com\r\n"
            "Connection: Keep-Alive\r\n"
            "Content-length: %d\r\n\r\n"
            "%s";

        char http_req[1024];
        sprintf(http_req, http_req_head, pUri, strlen(pJson), pJson);

        if (ttNow-ttLastSend>3)
        {
            ttLastSend  = time(NULL);
            // 不得加1
            //Send2Socket(CGENERALAGENTHTTPKEEPCONNECTOR_SOCKET_NAME, http_req, strlen(http_req)+1 );
            g_MaintainAgent.Send2Socket(CGENERALAGENTHTTPKEEPCONNECTOR_SOCKET_NAME, http_req, strlen(http_req));
            __fline;
            printf("%d sended\n", strlen(http_req)+1);
        }
#endif //EXAMPLE_GENERALAGENTHTTPKEEPCONNECTOR    
    }

    return 0;
}

/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * TestMan.cpp - _explain_
 *
 * Copyright (C) 2013 GeminiLock-china.com, All Rights Reserved.
 *
 * $Id: TestMan.cpp 0001 2013-11-02 14:38:29Z wu_junjie $
 *
 *  Explain:
 *     UnitTest -- Intelligent system communication protocol
 *
 *  Update:
 *     2013-11-03 14:38:33 WuJunjie 549 Create
 *
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#include "TestMan.h"

#include "../YiHe1Stack.h"

#ifndef __trip
	#define __trip printf("-W-%d::%s(%d)\n", (int)time(NULL), __FILE__, __LINE__);
#endif
#ifndef __fline
	#define __fline printf("%s(%d)--", __FILE__, __LINE__);
#endif

CPPUNIT_TEST_SUITE_REGISTRATION( CTestMan );

void CTestMan::setUp()
{}
void CTestMan::tearDown()
{}

void CTestMan::testGetSignalControlerParaRequest()
{
    CMsgGetSignalControlerParaRequest __msg;
    CYiHe1Stack __stack;

    __msg.m_yhHeader.m_strVersion   = "1.0";
    __msg.m_yhHeader.m_strToken     = "__d279449b631fd968068732f3d83e9ea1";
    __msg.m_yhHeader.m_strFrom      = "ltgs";
    __msg.m_yhHeader.m_strTo        = "tz";
    __msg.m_yhHeader.m_strType      = "signalM";
    __msg.m_yhHeader.m_strSeq       = "1";

    __msg.m_yhOprAttr.m_strName     = "getSignalControlerPara";
    __msg.m_yhOprAttr.m_strOrder    = "1";

    __msg.m_yhObjAttr.m_strName     = "TSC";
    __msg.m_yhObjAttr.m_strSumCount = "1";

    __stack.Encode(__msg);

    std::string __strA = __stack.ToString();

    //std::cout << __strA << std::endl;

    CMsgGetSignalControlerParaRequest __msg_d;
    CYiHe1Stack __stack_d;

    __stack_d.Parse(__strA);
    __stack_d.Decode(__msg_d);

    //__msg_d.dump();
    //std::cout << "----------------------------------------------------------" << std::endl;
    //__msg.dump();

    CPPUNIT_ASSERT(__msg == __msg_d);
}

void CTestMan::testGetSignalControlerParaResponse()
{
    CMsgGetSignalControlerParaResponse __msg;
    CYiHe1Stack __stack;

    __msg.m_yhHeader.m_strVersion   = "1.0";
    __msg.m_yhHeader.m_strToken     = "d279449b631fd968068732f3d83e9ea1";
    __msg.m_yhHeader.m_strFrom      = "ltgs";
    __msg.m_yhHeader.m_strTo        = "tz";
    __msg.m_yhHeader.m_strType      = "signalM";
    __msg.m_yhHeader.m_strSeq       = "1";

    __msg.m_yhOprAttr.m_strName     = "getSignalControlerPara";
    __msg.m_yhOprAttr.m_strOrder    = "1";

    __msg.m_yhObjAttr.m_strName     = "TSC";
    __msg.m_yhObjAttr.m_strSumCount = "1";

    //    __msg.m_yhSignalControlerPara.m_strSignalControlerID    = "98776";
    //    __msg.m_yhSignalControlerPara.m_strSupplier             = "lt";
    //    __msg.m_yhSignalControlerPara.m_strType                 = "LT-XHJXT-10";
    //    __msg.m_yhSignalControlerPara.m_strCrossIDList          = "11";

    CYH1MsgSignalControlerPara __para;
    __para.m_strSignalControlerID    = "98776";
    __para.m_strSupplier             = "lt";
    __para.m_strType                 = "LT-XHJXT-10";
    __para.m_strCrossIDList          = "11";

    __msg.m_v_yhSignalControlerPara.push_back(__para);
    __para.m_strSignalControlerID    = "98777";
    __msg.m_v_yhSignalControlerPara.push_back(__para);

    __stack.Encode(__msg);

    std::string __strA = __stack.ToString();

    //std::cout << __strA << std::endl;

    CMsgGetSignalControlerParaResponse __msg_d;
    CYiHe1Stack __stack_d;

    __stack_d.Parse(__strA);
    __stack_d.Decode(__msg_d);

#if 0

    __msg_d.dump();
    std::cout << "----------------------------------------------------------" << std::endl;
    __msg.dump();
#endif

    CPPUNIT_ASSERT(__msg == __msg_d);

}
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
void CTestMan::testGetCrossParaRequest()
{
    CMsgGetCrossParaRequest __msg;
    CYiHe1Stack __stack;

    __msg.m_yhHeader.m_strVersion   = "1.0";
    __msg.m_yhHeader.m_strToken     = "d279449b631fd968068732f3d83e9ea1";
    __msg.m_yhHeader.m_strFrom      = "ltgs";
    __msg.m_yhHeader.m_strTo        = "tz";
    __msg.m_yhHeader.m_strType      = "signalM";
    __msg.m_yhHeader.m_strSeq       = "1";

    __msg.m_yhOprAttr.m_strName     = "GetCrossPara";
    __msg.m_yhOprAttr.m_strOrder    = "1";

    __msg.m_yhObjAttr.m_strName     = "TSC";
    __msg.m_yhObjAttr.m_strSumCount = "1";

    __msg.m_yhObjAttr.m_strSumCount = "1";

    __msg.m_strSignalControlerID = "2";

    __stack.Encode(__msg);

    std::string __strA = __stack.ToString();

    //std::cout << __strA << std::endl;

    CMsgGetCrossParaRequest __msg_d;
    CYiHe1Stack __stack_d;

    __stack_d.Parse(__strA);
    __stack_d.Decode(__msg_d);
#if 0

    __msg_d.dump();
    std::cout << "----------------------------------------------------------" << std::endl;
    __msg.dump();
#endif

    CPPUNIT_ASSERT(__msg == __msg_d);
}

void CTestMan::testGetCrossParaResponse()
{
    CMsgGetCrossParaResponse __msg;
    CYiHe1Stack __stack;

    __msg.m_yhHeader.m_strVersion   = "1.0";
    __msg.m_yhHeader.m_strToken     = "d279449b631fd968068732f3d83e9ea1";
    __msg.m_yhHeader.m_strFrom      = "ltgs";
    __msg.m_yhHeader.m_strTo        = "tz";
    __msg.m_yhHeader.m_strType      = "signalM";
    __msg.m_yhHeader.m_strSeq       = "1";

    __msg.m_yhOprAttr.m_strName     = "GetCrossPara";
    __msg.m_yhOprAttr.m_strOrder    = "1";

    __msg.m_yhObjAttr.m_strName     = "TSC";
    __msg.m_yhObjAttr.m_strSumCount = "1";

    __msg.m_strSignalControlerID = "1 ControlerID";

    CYH1MsgCrossPara __para;
    __para.m_strCrossID    = "98776";
    __para.m_strCrossName             = "lt";
    __para.m_strFeature                 = "LT-XHJXT-10";
    __para.m_strIsKey          = "1";

    __msg.m_v_yhCrossPara.push_back(__para);

    __stack.Encode(__msg);

    std::string __strA = __stack.ToString();

    //std::cout << __strA << std::endl;

    CMsgGetCrossParaResponse __msg_d;
    CYiHe1Stack __stack_d;

    __stack_d.Parse(__strA);
    __stack_d.Decode(__msg_d);

#if 0

    __msg_d.dump();
    std::cout << "----------------------------------------------------------" << std::endl;
    __msg.dump();
#endif

    CPPUNIT_ASSERT(__msg == __msg_d);
}

/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
void CTestMan::testGetCrossStatusRequest()
{
    CMsgGetCrossStatusRequest __msg;
    CYiHe1Stack __stack;

    __msg.m_yhHeader.m_strVersion   = "1.0";
    __msg.m_yhHeader.m_strToken     = "d279449b631fd968068732f3d83e9ea1";
    __msg.m_yhHeader.m_strFrom      = "ltgs";
    __msg.m_yhHeader.m_strTo        = "tz";
    __msg.m_yhHeader.m_strType      = "signalM";
    __msg.m_yhHeader.m_strSeq       = "1";

    __msg.m_yhOprAttr.m_strName     = "GetCrossPara";
    __msg.m_yhOprAttr.m_strOrder    = "1";

    __msg.m_yhObjAttr.m_strName     = "TSC";
    __msg.m_yhObjAttr.m_strSumCount = "1";

    __msg.m_yhObjAttr.m_strSumCount = "1";

    __msg.m_strCrossId = "2";

    __stack.Encode(__msg);

    std::string __strA = __stack.ToString();

    //std::cout << __strA << std::endl;

    CMsgGetCrossStatusRequest __msg_d;
    CYiHe1Stack __stack_d;

    __stack_d.Parse(__strA);
    __stack_d.Decode(__msg_d);
#if 0

    __msg_d.dump();
    std::cout << "----------------------------------------------------------" << std::endl;
    __msg.dump();
#endif

    CPPUNIT_ASSERT(__msg == __msg_d);
}

void CTestMan::testGetCrossStatusResponse()
{
    CMsgGetCrossStatusResponse __msg;
    CYiHe1Stack __stack;

    __msg.m_yhHeader.m_strVersion   = "1.0";
    __msg.m_yhHeader.m_strToken     = "d279449b631fd968068732f3d83e9ea1";
    __msg.m_yhHeader.m_strFrom      = "ltgs";
    __msg.m_yhHeader.m_strTo        = "tz";
    __msg.m_yhHeader.m_strType      = "signalM";
    __msg.m_yhHeader.m_strSeq       = "1";

    __msg.m_yhOprAttr.m_strName     = "GetCrossStatus";
    __msg.m_yhOprAttr.m_strOrder    = "1";

    __msg.m_yhObjAttr.m_strName     = "TSC";
    __msg.m_yhObjAttr.m_strSumCount = "1";

    //CYH1MsgCrossStatus __para;
    __msg.m_v_yhCrossStatus.m_strCrossID    = "98776";
    __msg.m_v_yhCrossStatus.m_strValue             = "statusvalue";

    //__msg.m_v_yhCrossStatus.push_back(__para);

    __stack.Encode(__msg);

    std::string __strA = __stack.ToString();

    //std::cout << __strA << std::endl;

    CMsgGetCrossStatusResponse __msg_d;
    CYiHe1Stack __stack_d;

    __stack_d.Parse(__strA);
    __stack_d.Decode(__msg_d);

#if 0

    __msg_d.dump();
    std::cout << "----------------------------------------------------------" << std::endl;
    __msg.dump();
#endif

    CPPUNIT_ASSERT(__msg == __msg_d);
}


/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * TestMan.cpp - _explain_
 *
 * Copyright (C) 2013 ezlibs.com, All Rights Reserved.
 *
 * $Id: TestMan.cpp 0001 2013-11-02 14:38:29Z WuJunjie $
 *
 *  Explain:
 *     UnitTest -- PssStack
 *
 *  Update:
 *     2013-11-03 14:38:33 WuJunjie 549 Create
 *
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#include "TestMan.h"

#include "../PssStack.h"

#ifndef _DEBUG_THIS
//#define _DEBUG_THIS
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

CPPUNIT_TEST_SUITE_REGISTRATION( CTestMan );

void CTestMan::setUp()
{}
void CTestMan::tearDown()
{}

void CTestMan::testEncodeCAckMsgSimple()
{
    CPssStack Stack;
#if 0

    CPssMsg_requestStart Msg;

    Msg.m_strSession = "6f61dfe3";
    Msg.m_strRTSPUrl = "rtsp://202.102.108.137:50059/realplay";
    Msg.m_strDevId = "812A930E4BC";
    Stack.Encode(Msg);
    std::cout << std::endl << "-->> CPssMsg_requestStart" << " Encode:" << std::endl;
    std::cout << std::endl <<  Stack.ToString() << std::endl << "<<--" << std::endl;

    CPssMsg_requestStart __Msg;
    CPssStack __Stack;
    __Stack.Parse(Stack.ToString());
    __Stack.Decode(__Msg);
    CPPUNIT_ASSERT(Msg.m_strSession == __Msg.m_strSession );
#endif
    //__Msg.dump();
}
void CTestMan::testEncodeAndDecodeCUniMsg_reqCreateAppKey()
{
    CUniMsg_reqCreateAppKey Msg;
    CPssStack Stack;

    Msg.Account  = "user";
    Msg.Password = "userPassword";
    Msg.Service  = "userService ";

    Stack.Encode(Msg);
DBG(
    std::cout << std::endl << "-->> CUniMsg_reqCreateAppKey" << " Encode:" << std::endl;
    std::cout << std::endl <<  Stack.ToString() << std::endl << "<<--" << std::endl;
);
    CUniMsg_reqCreateAppKey __Msg;
    CPssStack __Stack;
    __Stack.Parse(Stack.ToString());
    __Stack.Decode(__Msg);
    CPPUNIT_ASSERT(Msg.Account == __Msg.Account );
    //__Msg.dump();
}

void CTestMan::testEncodeAndDecodeCUniMsg_reqCreateAppKeyAck()
{
    CPssStack Stack;
    CUniMsg_reqCreateAppKeyAck Msg;

    Msg.Result    = "0";
    Msg.Reason    = "OK";
    Msg.AppKey    = "123456789";
    Msg.AppSecret = "9d72ea431edd01c1b548b9360113a9768a35e039";
    Msg.Validity  = "1532149217";

    Stack.Encode(Msg);
DBG(
    std::cout << std::endl << "-->> CUniMsg_reqCreateAppKeyAck" << " Encode:" << std::endl;
    std::cout << std::endl <<  Stack.ToString() << std::endl << "<<--" << std::endl;
);
    CUniMsg_reqCreateAppKeyAck __Msg;
    CPssStack __Stack;
    __Stack.Parse(Stack.ToString());
    __Stack.Decode(__Msg);
    CPPUNIT_ASSERT(Msg.AppKey == __Msg.AppKey );
    //__Msg.dump();
}

void CTestMan::testEncodeAndDecodeCUniMsg_reqGetSetGmLic()
{
    CUniMsg_reqGetSetGmLic Msg;
    CPssStack Stack;

    Msg.strGLic  = "28224939621657672957928";

    Stack.Encode(Msg);
DBG(
    std::cout << std::endl << "-->> CUniMsg_reqGetSetGmLic" << " Encode:" << std::endl;
    std::cout << std::endl <<  Stack.ToString() << std::endl << "<<--" << std::endl;
);
    CUniMsg_reqGetSetGmLic __Msg;
    CPssStack __Stack;
    __Stack.Parse(Stack.ToString());
    __Stack.Decode(__Msg);
    CPPUNIT_ASSERT(Msg.strGLic == __Msg.strGLic );
    //__Msg.dump();
}

void CTestMan::testEncodeAndDecodeCUniMsg_reqGetSetGmLicAck()
{
    CPssStack Stack;
    CUniMsg_reqGetSetGmLicAck Msg;

    Msg.Result    = "0";
    Msg.Reason    = "OK";

    Msg.m_iGLicVer = 0;
    Msg.m_strllValidSeconds = "100";//for GMINI_LIC_V1
    Msg.m_strttValidDate = "54655465";//for GMINI_LIC_V2
    Msg.strGLic  = "28224939621657672957928";

    Stack.Encode(Msg);
DBG(
    std::cout << std::endl << "-->> CUniMsg_reqGetSetGmLicAck" << " Encode:" << std::endl;
    std::cout << std::endl <<  Stack.ToString() << std::endl << "<<--" << std::endl;
);
    CUniMsg_reqGetSetGmLicAck __Msg;
    CPssStack __Stack;
    __Stack.Parse(Stack.ToString());
    __Stack.Decode(__Msg);
    CPPUNIT_ASSERT(Msg.strGLic == __Msg.strGLic );
    //__Msg.dump();
}

void CTestMan::testEncodeAndDecodeCUniMsg_reqConfigModify()
{
    CPssStack Stack;
    CUniMsg_reqConfigModify Msg;

    Msg.User    = "liveRecord";
    Msg.Section = "TykdMobileService";
    Msg.Name    = "HostRelative"; 
    Msg.Value   = "110.110.110.110||192.168.9.62&&218.90.228.144||192.168.201.106"; 

    Stack.Encode(Msg);
DBG(
    std::cout << std::endl << "-->> CUniMsg_reqConfigModify" << " Encode:" << std::endl;
    std::cout << std::endl <<  Stack.ToString() << std::endl << "<<--" << std::endl;
);
    CUniMsg_reqConfigModify __Msg;
    CPssStack __Stack;
    __Stack.Parse(Stack.ToString());
    __Stack.Decode(__Msg);
    CPPUNIT_ASSERT(Msg.User == __Msg.User );
    //__Msg.dump();
}

void CTestMan::testEncodeAndDecodeCPssMsgHlsPullTykdStart()
{
    CPssMsgHlsPullTykdStart Msg;
    CPssStack Stack;

    //Msg.m_strSession = "6f61dfe3";
    Msg.m_iDetails = 1;

    //Msg.Tykd.Account="account";
    //Msg.Tykd.Password="apassword";
    //Msg.Tykd.Session="2a19d248f889df3924ab3445f8a0c376";

    Msg.Stream.DevID="812A930AC7B";
    Msg.Stream.Description="trip";
    Msg.Stream.Channel="0";
    Msg.Stream.Stream="2";

    Msg.Hls.ValidPeriod="0";
    Msg.Hls.PlayUrl="0";
    Msg.Hls.Snapshot="0";
    Msg.Hls.Description="trip";
    Msg.Hls.StreamID="812A930AC7B";

    Stack.Encode(Msg);
DBG(
    std::cout << std::endl << "-->> CPssMsgHlsPullTykdStart" << " Encode:" << std::endl;
    std::cout << std::endl <<  Stack.ToString() << std::endl << "<<--" << std::endl;
);
    CPssMsgHlsPullTykdStart __Msg;
    CPssStack __Stack;
    __Stack.Parse(Stack.ToString());
    __Stack.Decode(__Msg);
    CPPUNIT_ASSERT(Msg.m_iDetails == __Msg.m_iDetails );
    //__Msg.dump();
}

void CTestMan::testEncodeAndDecodeCPssMsgHlsPullTykdStop()
{
    CPssMsgHlsPullTykdStop Msg;
    CPssStack Stack;

    Msg.m_iDetails = 1;

    //    Msg.Tykd.Account="account";
    //    Msg.Tykd.Password="apassword";
    //    Msg.Tykd.Session="2a19d248f889df3924ab3445f8a0c376";

    Msg.Stream.DevID="812A930AC7B";
    Msg.Stream.Description="trip";
    Msg.Stream.Channel="0";
    Msg.Stream.Stream="2";

    Msg.Hls.ValidPeriod="0";
    Msg.Hls.PlayUrl="0";
    Msg.Hls.Snapshot="0";
    Msg.Hls.Description="trip";
    Msg.Hls.StreamID="812A930AC7B";

    Stack.Encode(Msg);
DBG(
    std::cout << std::endl << "-->> CPssMsgHlsPullTykdStop" << " Encode:" << std::endl;
    std::cout << std::endl <<  Stack.ToString() << std::endl << "<<--" << std::endl;
);
    CPssMsgHlsPullTykdStop __Msg;
    CPssStack __Stack;
    __Stack.Parse(Stack.ToString());
    __Stack.Decode(__Msg);
    CPPUNIT_ASSERT(Msg.Stream.DevID == __Msg.Stream.DevID );
    //__Msg.dump();
}

void CTestMan::testEncodeAndDecodeCPssMsgHlsPullTykdStartAck()
{
    CPssMsgHlsPullTykdStartAck Msg;
    CPssStack Stack;

    Msg.Result = "0";
    Msg.Reason = "ok";

    Msg.Hls.ValidPeriod="0";
    Msg.Hls.PlayUrl="http://pss.ezlibs.com:18080/hls/812A930AC7B.m3u8";
    Msg.Hls.Snapshot="http://pss.ezlibs.com:18080/snap/812A930AC7B.jpg";
    Msg.Hls.Description="trip";
    //Msg.Hls.StreamID="812A930E4BC_ezLibs";

    Stack.Encode(Msg);
DBG(
    std::cout << std::endl << "-->> CPssMsgHlsPullTykdStartAck" << " Encode:" << std::endl;
    std::cout << std::endl <<  Stack.ToString() << std::endl << "<<--" << std::endl;
);
    CPssMsgHlsPullTykdStartAck __Msg;
    CPssStack __Stack;
    __Stack.Parse(Stack.ToString());
    __Stack.Decode(__Msg);
    CPPUNIT_ASSERT(Msg.Reason == __Msg.Reason );
    //__Msg.dump();
}
void CTestMan::testEncodeAndDecodeCPssMsgHlsPullTykdStopAck()
{
    CPssStack Stack;
    CPssMsgHlsPullTykdStopAck Msg;

    Msg.Result = "0";
    Msg.Reason = "ok";
    Stack.Encode(Msg);
DBG(
    std::cout << std::endl << "-->> CPssMsgHlsPullTykdStopAck" << " Encode:" << std::endl;
    std::cout << std::endl <<  Stack.ToString() << std::endl << "<<--" << std::endl;
);
    CPssMsgHlsPullTykdStopAck __Msg;
    CPssStack __Stack;
    __Stack.Parse(Stack.ToString());
    __Stack.Decode(__Msg);
    CPPUNIT_ASSERT(Msg.Reason == __Msg.Reason );
    //__Msg.dump();
}

void CTestMan::testEncodeAndDecodeCPssMsgRtspPullTykdStart()
{
    CPssMsgRtspPullTykdStart Msg;
    CPssStack Stack;

    Msg.m_iDetails = 1;

    //Msg.Tykd.Account="account";
    //Msg.Tykd.Password="apassword";
    //Msg.Tykd.Session="2a19d248f889df3924ab3445f8a0c376";

    Msg.Stream.DevID="812A930AC7B";
    Msg.Stream.Description="trip";
    Msg.Stream.Channel="0";
    Msg.Stream.Stream="2";

    Msg.Rtsp.ValidPeriod="0";
    Msg.Rtsp.PlayUrl="0";
    Msg.Rtsp.Snapshot="0";
    Msg.Rtsp.Description="trip";
    Msg.Rtsp.StreamID="812A930AC7B";

    Stack.Encode(Msg);
DBG(
    std::cout << std::endl << "-->> CPssMsgRtspPullTykdStart" << " Encode:" << std::endl;
    std::cout << std::endl <<  Stack.ToString() << std::endl << "<<--" << std::endl;
);
    CPssMsgRtspPullTykdStart __Msg;
    CPssStack __Stack;
    __Stack.Parse(Stack.ToString());
    __Stack.Decode(__Msg);
    CPPUNIT_ASSERT(Msg.m_iDetails == __Msg.m_iDetails );
    //__Msg.dump();
}


void CTestMan::testEncodeAndDecodeCPssMsgRtspPullTykdStop()
{
    CPssMsgRtspPullTykdStop Msg;
    CPssStack Stack;

    Msg.m_iDetails = 1;

    //    Msg.Tykd.Account="account";
    //    Msg.Tykd.Password="apassword";
    //    Msg.Tykd.Session="2a19d248f889df3924ab3445f8a0c376";

    Msg.Stream.DevID="812A930AC7B";
    Msg.Stream.Description="trip";
    Msg.Stream.Channel="0";
    Msg.Stream.Stream="2";

    Msg.Rtsp.ValidPeriod="0";
    Msg.Rtsp.PlayUrl="0";
    Msg.Rtsp.Snapshot="0";
    Msg.Rtsp.Description="trip";
    Msg.Rtsp.StreamID="812A930AC7B";

    Stack.Encode(Msg);
DBG(
    std::cout << std::endl << "-->> CPssMsgRtspPullTykdStop" << " Encode:" << std::endl;
    std::cout << std::endl <<  Stack.ToString() << std::endl << "<<--" << std::endl;
);
    CPssMsgRtspPullTykdStop __Msg;
    CPssStack __Stack;
    __Stack.Parse(Stack.ToString());
    __Stack.Decode(__Msg);
    CPPUNIT_ASSERT(Msg.Stream.DevID == __Msg.Stream.DevID );
    //__Msg.dump();
}

void CTestMan::testEncodeAndDecodeCPssMsgRtspPullTykdStartAck()
{
    CPssMsgRtspPullTykdStartAck Msg;
    CPssStack Stack;

    Msg.Result = "0";
    Msg.Reason = "ok";

    Msg.m_strMsgSession = "6f61dfe3";

    Msg.Rtsp.ValidPeriod="0";
    Msg.Rtsp.PlayUrl="http://pss.ezlibs.com:18080/hls/812A930AC7B.m3u8";
    Msg.Rtsp.Snapshot="http://pss.ezlibs.com:18080/snap/812A930AC7B.jpg";
    Msg.Rtsp.Description="trip";
    //Msg.Rtsp.StreamID="812A930E4BC_ezLibs";

    Stack.Encode(Msg);
DBG(
    std::cout << std::endl << "-->> CPssMsgRtspPullTykdStartAck" << " Encode:" << std::endl;
    std::cout << std::endl <<  Stack.ToString() << std::endl << "<<--" << std::endl;
);
    CPssMsgRtspPullTykdStartAck __Msg;
    CPssStack __Stack;
    __Stack.Parse(Stack.ToString());
    __Stack.Decode(__Msg);
    CPPUNIT_ASSERT(Msg.Reason == __Msg.Reason );
    //__Msg.dump();
}
void CTestMan::testEncodeAndDecodeCPssMsgRtspPullTykdStopAck()
{
    CPssStack Stack;
    CPssMsgRtspPullTykdStopAck Msg;

    Msg.Result = "0";
    Msg.Reason = "ok";
    Msg.m_strMsgSession = "6f61dfe3";

    Stack.Encode(Msg);
DBG(
    std::cout << std::endl << "-->> CPssMsgRtspPullTykdStopAck" << " Encode:" << std::endl;
    std::cout << std::endl <<  Stack.ToString() << std::endl << "<<--" << std::endl;
);
    CPssMsgRtspPullTykdStopAck __Msg;
    CPssStack __Stack;
    __Stack.Parse(Stack.ToString());
    __Stack.Decode(__Msg);
    CPPUNIT_ASSERT(Msg.Reason == __Msg.Reason );
    //__Msg.dump();
}
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
void CTestMan::testEncodeAndDecodeCPssMsgRtmpPullTykdStart()
{
    CPssMsgRtmpPullTykdStart Msg;
    CPssStack Stack;

    //Msg.m_strSession = "6f61dfe3";
    Msg.m_iDetails = 1;

    //Msg.Tykd.Account="account";
    //Msg.Tykd.Password="apassword";
    //Msg.Tykd.Session="2a19d248f889df3924ab3445f8a0c376";

    Msg.Stream.DevID="812A930AC7B";
    Msg.Stream.Description="trip";
    Msg.Stream.Channel="0";
    Msg.Stream.Stream="2";

    Msg.Rtmp.ValidPeriod="0";
    Msg.Rtmp.PlayUrl="0";
    Msg.Rtmp.Snapshot="0";
    Msg.Rtmp.Description="trip";
    Msg.Rtmp.StreamID="812A930AC7B";

    Stack.Encode(Msg);
DBG(
    std::cout << std::endl << "-->> CPssMsgRtmpPullTykdStart" << " Encode:" << std::endl;
    std::cout << std::endl <<  Stack.ToString() << std::endl << "<<--" << std::endl;
);
    CPssMsgRtmpPullTykdStart __Msg;
    CPssStack __Stack;
    __Stack.Parse(Stack.ToString());
    __Stack.Decode(__Msg);
    CPPUNIT_ASSERT(Msg.m_iDetails == __Msg.m_iDetails );
    //__Msg.dump();
}


void CTestMan::testEncodeAndDecodeCPssMsgRtmpPullTykdStop()
{
    CPssMsgRtmpPullTykdStop Msg;
    CPssStack Stack;

    Msg.m_iDetails = 1;

    //    Msg.Tykd.Account="account";
    //    Msg.Tykd.Password="apassword";
    //    Msg.Tykd.Session="2a19d248f889df3924ab3445f8a0c376";

    Msg.Stream.DevID="812A930AC7B";
    Msg.Stream.Description="trip";
    Msg.Stream.Channel="0";
    Msg.Stream.Stream="2";

    Msg.Rtmp.ValidPeriod="0";
    Msg.Rtmp.PlayUrl="0";
    Msg.Rtmp.Snapshot="0";
    Msg.Rtmp.Description="trip";
    Msg.Rtmp.StreamID="812A930AC7B";

    Stack.Encode(Msg);
DBG(
    std::cout << std::endl << "-->> CPssMsgRtmpPullTykdStop" << " Encode:" << std::endl;
    std::cout << std::endl <<  Stack.ToString() << std::endl << "<<--" << std::endl;
);
    CPssMsgRtmpPullTykdStop __Msg;
    CPssStack __Stack;
    __Stack.Parse(Stack.ToString());
    __Stack.Decode(__Msg);
    CPPUNIT_ASSERT(Msg.Stream.DevID == __Msg.Stream.DevID );
    //__Msg.dump();
}

void CTestMan::testEncodeAndDecodeCPssMsgRtmpPullTykdStartAck()
{
    CPssMsgRtmpPullTykdStartAck Msg;
    CPssStack Stack;

    Msg.Result = "0";
    Msg.Reason = "ok";

    Msg.Rtmp.ValidPeriod="0";
    Msg.Rtmp.PlayUrl="rtmp://pss.ezlibs.com:18090/live/112A0000034";
    Msg.Rtmp.Snapshot="http://pss.ezlibs.com:18080/snap/112A0000034.jpg";
    Msg.Rtmp.Description="trip";

    Msg.Storage.Type        = "2";
    Msg.Storage.Name        = "nvr://account:password@192.168.5.208:8000/sreq?http=80&rtsp=554&onvif=no";
    Msg.Storage.Channel     = "1";
    Msg.Storage.Description = "Hik";
    Msg.Storage.Status      = 1;
    Msg.Storage.Capacity    = 1024*1024;
    Msg.Storage.FreeSpace   = 1024;

    Stack.Encode(Msg);
DBG(
    std::cout << std::endl << "-->> " << __FUNCTION__ << " Encode:" << std::endl;
    std::cout << std::endl <<  Stack.ToString() << std::endl << "<<--" << std::endl;
);
    CPssMsgRtmpPullTykdStartAck __Msg;
    CPssStack __Stack;
    __Stack.Parse(Stack.ToString());
    __Stack.Decode(__Msg);
    CPPUNIT_ASSERT(Msg.Reason == __Msg.Reason );
    //__Msg.dump();
}

void CTestMan::testEncodeAndDecodeCPssMsgRtmpPullTykdStopAck()
{
    CPssStack Stack;
    CPssMsgRtmpPullTykdStopAck Msg;

    Msg.Result = "0";
    Msg.Reason = "ok";
    Stack.Encode(Msg);
DBG(
    std::cout << std::endl << "-->> CPssMsgRtmpPullTykdStopAck" << " Encode:" << std::endl;
    std::cout << std::endl <<  Stack.ToString() << std::endl << "<<--" << std::endl;
);
    CPssMsgRtmpPullTykdStopAck __Msg;
    CPssStack __Stack;
    __Stack.Parse(Stack.ToString());
    __Stack.Decode(__Msg);
    CPPUNIT_ASSERT(Msg.Reason == __Msg.Reason );
    //__Msg.dump();
}

void CTestMan::testEncodeAndDecodeCPandoraMsg_TykdPullStart()
{
    CPandoraMsg_TykdPullStart Msg;
    CPssStack Stack;

    Msg.m_strMsgSession = "6f61dfe3";

    Msg.Stream.DevID="812A930AC7B";
    Msg.Stream.Description="trip";
    Msg.Stream.Channel="0";
    Msg.Stream.Stream="2";

    Msg.Type = 0;

    Stack.Encode(Msg);
DBG(
    std::cout << std::endl << "-->> CPandoraMsg_TykdPullStart" << " Encode:" << std::endl;
    std::cout << std::endl <<  Stack.ToString() << std::endl << "<<--" << std::endl;
);
    CPandoraMsg_TykdPullStart __Msg;
    CPssStack __Stack;
    __Stack.Parse(Stack.ToString());
    __Stack.Decode(__Msg);
    CPPUNIT_ASSERT(Msg.m_strMsgSession == __Msg.m_strMsgSession );

}

void CTestMan::testEncodeAndDecodeCPandoraMsg_TykdPullStartAck()
{
    CPssStack Stack;
    CPandoraMsg_TykdPullStartAck Msg;

    Msg.m_strMsgSession = "6f61dfe3";

    Msg.Result = "0";
    Msg.Reason = "ok";
    Msg.hlsPlayUrl = "http://xxx.m3u8";
    Msg.rtmpPlayUrl = "rtmp://xxx";
    Msg.rtspPlayUrl = "rtsp://xxx.sdp";

    Stack.Encode(Msg);
DBG(
    std::cout << std::endl << "-->> CPandoraMsg_TykdPullStartAck" << " Encode:" << std::endl;
    std::cout << std::endl <<  Stack.ToString() << std::endl << "<<--" << std::endl;
);
    CPssMsgRtmpPullTykdStopAck __Msg;
    CPssStack __Stack;
    __Stack.Parse(Stack.ToString());
    __Stack.Decode(__Msg);
    CPPUNIT_ASSERT(Msg.Reason == __Msg.Reason );
}
//void CTestMan::testEncodeAndDecodeCPandoraMsg_TykdPullStop()
//{
//    CPandoraMsg_TykdPullStop Msg;
//    CPssStack Stack;
//
//    Msg.m_strMsgSession = "6f61dfe3";
//
//    Msg.Stream.DevID="812A930AC7B";
//    Msg.Stream.Description="trip";
//    Msg.Stream.Channel="0";
//    Msg.Stream.Stream="2";
//
//    Msg.Type = 0;
//
//    Stack.Encode(Msg);
//    std::cout << std::endl << "-->> CPandoraMsg_TykdPullStop" << " Encode:" << std::endl;
//    std::cout << std::endl <<  Stack.ToString() << std::endl << "<<--" << std::endl;
//
//    CPandoraMsg_TykdPullStop __Msg;
//    CPssStack __Stack;
//    __Stack.Parse(Stack.ToString());
//    __Stack.Decode(__Msg);
//    CPPUNIT_ASSERT(Msg.m_strMsgSession == __Msg.m_strMsgSession );
//
//}
//
//void CTestMan::testEncodeAndDecodeCPandoraMsg_TykdPullStopAck()
//{
//    CPssStack Stack;
//    CPandoraMsg_TykdPullStopAck Msg;
//
//    Msg.m_strMsgSession = "6f61dfe3";
//
//    Msg.Result = "0";
//    Msg.Reason = "ok";
//    Msg.hlsPlayUrl = "http://xxx.m3u8";
//    Msg.rtmpPlayUrl = "rtmp://xxx";
//    Msg.rtspPlayUrl = "rtsp://xxx.sdp";
//
//    Stack.Encode(Msg);
//    std::cout << std::endl << "-->> CPandoraMsg_TykdPullStopAck" << " Encode:" << std::endl;
//    std::cout << std::endl <<  Stack.ToString() << std::endl << "<<--" << std::endl;
//
//    CPssMsgRtmpPullTykdStopAck __Msg;
//    CPssStack __Stack;
//    __Stack.Parse(Stack.ToString());
//    __Stack.Decode(__Msg);
//    CPPUNIT_ASSERT(Msg.Reason == __Msg.Reason );
//}

void CTestMan::testEncodeAndDecodeCPandoraMsg_pssChannelCreate()
{
    CPssStack Stack;

    CPandoraMsg_pssChannelCreate Msg;

    Msg.Name = "000C298DB4D0";
    Msg.Desc = "gminilock";
    Msg.Type = "0";

    Stack.Encode(Msg);
DBG(
    std::cout << std::endl << "-->> CPandoraMsg_pssChannelCreate" << " Encode:" << std::endl;
    std::cout << std::endl <<  Stack.ToString() << std::endl << "<<--" << std::endl;
);
    CPandoraMsg_pssChannelCreate __Msg;
    CPssStack __Stack;
    __Stack.Parse(Stack.ToString());
    __Stack.Decode(__Msg);
    CPPUNIT_ASSERT(Msg.Name == __Msg.Name );
    CPPUNIT_ASSERT(Msg.Type == __Msg.Type );
}
void CTestMan::testEncodeAndDecodeCPandoraMsg_pssChannelCreateAck()
{
    CPssStack Stack;

    CPandoraMsg_pssChannelCreateAck Msg;

    Msg.Result = "0";
    Msg.Reason = "ok";

    Msg.PushUrlRtsp = "rtsp://61.160.149.159:18554/b60bf169ef10cd5e9b1d9b14e22ddafb684040b6.sdp";
    Msg.PushUrlRtmp = "rtmp://61.160.149.159:19090/man/b60bf169ef10cd5e9b1d9b14e22ddafb684040b6";
    Msg.PlayUrlHls  = "http://61.160.149.159:18080/hls/b60bf169ef10cd5e9b1d9b14e22ddafb684040b6.m3u8";
    Msg.PlayUrlRtmp = "rtmp://61.160.149.159:19090/man/b60bf169ef10cd5e9b1d9b14e22ddafb684040b6";
    Msg.PlayUrlRtsp = "rtsp://61.160.149.159:18554/b60bf169ef10cd5e9b1d9b14e22ddafb684040b6.sdp";

    Stack.Encode(Msg);
DBG(
    std::cout << std::endl << "-->> CPandoraMsg_pssChannelCreateAck" << " Encode:" << std::endl;
    std::cout << std::endl <<  Stack.ToString() << std::endl << "<<--" << std::endl;
);
    CPandoraMsg_pssChannelCreateAck __Msg;
    CPssStack __Stack;
    __Stack.Parse(Stack.ToString());
    __Stack.Decode(__Msg);
    CPPUNIT_ASSERT(Msg.Result == __Msg.Result );
}
void CTestMan::testEncodeAndDecodeCPandoraMsg_pssChannelCancel()
{
    CPssStack Stack;

    CPandoraMsg_pssChannelCancel Msg;

    Msg.Name = "000C298DB4D0";

    Stack.Encode(Msg);
DBG(
    std::cout << std::endl << "-->> CPandoraMsg_pssChannelCancel" << " Encode:" << std::endl;
    std::cout << std::endl <<  Stack.ToString() << std::endl << "<<--" << std::endl;
);
    CPandoraMsg_pssChannelCancel __Msg;
    CPssStack __Stack;
    __Stack.Parse(Stack.ToString());
    __Stack.Decode(__Msg);
    CPPUNIT_ASSERT(Msg.Name == __Msg.Name );
}
void CTestMan::testEncodeAndDecodeCPandoraMsg_pssChannelCancelAck()
{
    CPssStack Stack;

    CPandoraMsg_pssChannelCancelAck Msg;

    Msg.Result = "0";
    Msg.Reason = "ok";

    Msg.m_strMsgSession = "session_002";

    Stack.Encode(Msg);
DBG(
    std::cout << std::endl << "-->> CPandoraMsg_pssChannelCancelAck" << " Encode:" << std::endl;
    std::cout << std::endl <<  Stack.ToString() << std::endl << "<<--" << std::endl;
);
    CPandoraMsg_pssChannelCancelAck __Msg;
    CPssStack __Stack;
    __Stack.Parse(Stack.ToString());
    __Stack.Decode(__Msg);
    CPPUNIT_ASSERT(Msg.Result == __Msg.Result );
}

void CTestMan::testEncodeAndDecodeCPandoraMsg_pssChannelQuery()
{
    CPssStack Stack;

    CPandoraMsg_pssChannelQuery Msg;

    Msg.Name = "000C298DB4D0";

    Stack.Encode(Msg);
DBG(
    std::cout << std::endl << "-->> CPandoraMsg_pssChannelQuery" << " Encode:" << std::endl;
    std::cout << std::endl <<  Stack.ToString() << std::endl << "<<--" << std::endl;
);
    CPandoraMsg_pssChannelQuery __Msg;
    CPssStack __Stack;
    __Stack.Parse(Stack.ToString());
    __Stack.Decode(__Msg);
    CPPUNIT_ASSERT(Msg.Name == __Msg.Name );
}

void CTestMan::testEncodeAndDecodeCPandoraMsg_pssChannelQueryAck()
{
    CPssStack Stack;

    CPandoraMsg_pssChannelQueryAck Msg;

    Msg.Result = "0";
    Msg.Reason = "ok";

    Msg.PlayUrlHlsStatus = "0";
    Msg.PlayUrlRtspStatus = "0";
    Msg.AcceptedTimes = 100;

    Stack.Encode(Msg);
DBG(
    std::cout << std::endl << "-->> CPandoraMsg_pssChannelQueryAck" << " Encode:" << std::endl;
    std::cout << std::endl <<  Stack.ToString() << std::endl << "<<--" << std::endl;
);
    CPandoraMsg_pssChannelQueryAck __Msg;
    CPssStack __Stack;
    __Stack.Parse(Stack.ToString());
    __Stack.Decode(__Msg);
    CPPUNIT_ASSERT(Msg.Result == __Msg.Result );
    CPPUNIT_ASSERT(Msg.AcceptedTimes == __Msg.AcceptedTimes );
}

void CTestMan::testEncodeAndDecodeCPandoraMsg_pssChannelList()
{
    CPssStack Stack;

    CPandoraMsg_pssChannelList Msg;

    Msg.Type = "0";

    Stack.Encode(Msg);
DBG(
    std::cout << std::endl << "-->> CPandoraMsg_pssChannelList" << " Encode:" << std::endl;
    std::cout << std::endl <<  Stack.ToString() << std::endl << "<<--" << std::endl;
);
    CPandoraMsg_pssChannelList __Msg;
    CPssStack __Stack;
    __Stack.Parse(Stack.ToString());
    __Stack.Decode(__Msg);
    CPPUNIT_ASSERT(Msg.Type == __Msg.Type );
}

void CTestMan::testEncodeAndDecodeCPandoraMsg_pssChannelListAck()
{
    CPssStack Stack;

    CPandoraMsg_pssChannelListAck Msg;

    Msg.Result = "0";
    Msg.Reason = "ok";
    Msg.m_vChannel.push_back("000C298DB4D0_0");
    Msg.m_vChannel.push_back("000C298DB4D0_1");
    Msg.m_vChannel.push_back("000C298DB4D0_2");
    Msg.TotalItems = 3;

    Stack.Encode(Msg);
DBG(
    std::cout << std::endl << "-->> CPandoraMsg_pssChannelListAck" << " Encode:" << std::endl;
    std::cout << std::endl <<  Stack.ToString() << std::endl << "<<--" << std::endl;
);
    CPandoraMsg_pssChannelListAck __Msg;
    CPssStack __Stack;
    __Stack.Parse(Stack.ToString());
    __Stack.Decode(__Msg);
    CPPUNIT_ASSERT(Msg.Result == __Msg.Result );
}

void CTestMan::testEncodeAndDecodeCStreamCapabilityMsg()
{
    CPssStack Stack;

    CStreamCapabilityMsg Msg;

    Msg.BandwidthIn        = 1024*100;   // 入口带宽 bps
    Msg.BandwidthOut       = 1024*1000 ; // 出口带宽 bps
    Msg.CpuMark            = 800;        // cpu评分，
    Msg.Memory             = 1024;       // 内存数量 MByte
    Msg.StreamChannelTotal      = 200;        // 分发能力

    Stack.Encode(Msg);
DBG(
    std::cout << std::endl << "-->> CStreamCapabilityMsg" << " Encode:" << std::endl;
    std::cout << std::endl <<  Stack.ToString() << std::endl << "<<--" << std::endl;
);
    CStreamCapabilityMsg __Msg;
    CPssStack __Stack;
    __Stack.Parse(Stack.ToString());
    __Stack.Decode(__Msg);
    CPPUNIT_ASSERT(Msg.StreamChannelTotal == __Msg.StreamChannelTotal );
}

void CTestMan::testEncodeAndDecodeCPssMsgRecTykdStart()
{
    CPssMsgRecTykdStart Msg;
    CPssStack Stack;

    Msg.Stream.DevID="812A930AC7B";
    Msg.Stream.Description="Description";
    Msg.Stream.Channel="0";
    Msg.Stream.Stream="1";
    Msg.Stream.AudioEncode="1";

    Msg.Rec.ValidPeriod="36000";
    Msg.Rec.FilePeriod="300";

    Stack.Encode(Msg);
DBG(
    std::cout << std::endl << "-->> CPssMsgRecTykdStart" << " Encode:" << std::endl;
    std::cout << std::endl <<  Stack.ToString() << std::endl << "<<--" << std::endl;
);
    CPssMsgRecTykdStart __Msg;
    CPssStack __Stack;
    __Stack.Parse(Stack.ToString());
    __Stack.Decode(__Msg);
    CPPUNIT_ASSERT(Msg.Stream.DevID == __Msg.Stream.DevID );
    //__Msg.dump();
}
void CTestMan::testEncodeAndDecodeCPssMsgRecTykdStartAck()
{
    CPssMsgRecTykdStartAck Msg;
    CPssStack Stack;

    Msg.Result = "0";
    Msg.Reason = "ok";


    Stack.Encode(Msg);
DBG(
    std::cout << std::endl << "-->> CPssMsgRecTykdStartAck" << " Encode:" << std::endl;
    std::cout << std::endl <<  Stack.ToString() << std::endl << "<<--" << std::endl;
);
    CPssMsgRecTykdStartAck __Msg;
    CPssStack __Stack;
    __Stack.Parse(Stack.ToString());
    __Stack.Decode(__Msg);
    CPPUNIT_ASSERT(Msg.Reason == __Msg.Reason );
    //__Msg.dump();
}


void CTestMan::testEncodeAndDecodeCPssMsgRecTykdStop()
{
    CPssMsgRecTykdStop Msg;
    CPssStack Stack;


    Msg.Stream.DevID="812A930AC7B";
    Msg.Stream.Description="Description";
    Msg.Stream.Channel="0";
    Msg.Stream.Stream="2";


    Stack.Encode(Msg);
DBG(
    std::cout << std::endl << "-->> CPssMsgRecTykdStop" << " Encode:" << std::endl;
    std::cout << std::endl <<  Stack.ToString() << std::endl << "<<--" << std::endl;
);
    CPssMsgRecTykdStop __Msg;
    CPssStack __Stack;
    __Stack.Parse(Stack.ToString());
    __Stack.Decode(__Msg);
    CPPUNIT_ASSERT(Msg.Stream.DevID == __Msg.Stream.DevID );
    //__Msg.dump();
}

void CTestMan::testEncodeAndDecodeCPssMsgRecTykdStopAck()
{
    CPssStack Stack;
    CPssMsgRecTykdStopAck Msg;

    Msg.Result = "0";
    Msg.Reason = "ok";

    Stack.Encode(Msg);
DBG(
    std::cout << std::endl << "-->> CPssMsgRecTykdStopAck" << " Encode:" << std::endl;
    std::cout << std::endl <<  Stack.ToString() << std::endl << "<<--" << std::endl;
);
    CPssMsgRecTykdStopAck __Msg;
    CPssStack __Stack;
    __Stack.Parse(Stack.ToString());
    __Stack.Decode(__Msg);
    CPPUNIT_ASSERT(Msg.Reason == __Msg.Reason );
    //__Msg.dump();
}

void CTestMan::testEncodeAndDecodeCPssMsgRecTykdDel()
{
    CPssMsgRecTykdDel Msg;
    CPssStack Stack;

    Msg.Stream.DevID="812A930AC7B";
    Msg.Stream.Description="Description";
    Msg.Stream.Channel="0";
    Msg.Stream.Stream="1";

    Msg.StartDt="1487638357";
    Msg.EndDt="1487638397";

    Stack.Encode(Msg);
DBG(
    std::cout << std::endl << "-->> CPssMsgRecTykdDel" << " Encode:" << std::endl;
    std::cout << std::endl <<  Stack.ToString() << std::endl << "<<--" << std::endl;
);
    CPssMsgRecTykdStart __Msg;
    CPssStack __Stack;
    __Stack.Parse(Stack.ToString());
    __Stack.Decode(__Msg);
    CPPUNIT_ASSERT(Msg.Stream.DevID == __Msg.Stream.DevID );
    //__Msg.dump();
}
void CTestMan::testEncodeAndDecodeCPssMsgRecTykdDelAck()
{
    CPssMsgRecTykdDelAck Msg;
    CPssStack Stack;

    Msg.Result = "0";
    Msg.Reason = "ok";


    Stack.Encode(Msg);
DBG(
    std::cout << std::endl << "-->> CPssMsgRecTykdDelAck" << " Encode:" << std::endl;
    std::cout << std::endl <<  Stack.ToString() << std::endl << "<<--" << std::endl;
);
    CPssMsgRecTykdDelAck __Msg;
    CPssStack __Stack;
    __Stack.Parse(Stack.ToString());
    __Stack.Decode(__Msg);
    CPPUNIT_ASSERT(Msg.Reason == __Msg.Reason );
    //__Msg.dump();
}

void CTestMan::testEncodeAndDecodeCPssMsgRecTykdQuery()
{
    CPssMsgRecTykdQuery Msg;
    CPssStack Stack;

    Msg.Stream.DevID="812A930AC7B";
    //Msg.Stream.Description="Description";
    Msg.Stream.Channel="0";
    Msg.Stream.Stream="1";

    Msg.StartDt="1487638357";
    Msg.EndDt="1487638397";

    Msg.Items="10";
    Msg.Pages="5";

    Stack.Encode(Msg);
DBG(
    std::cout << std::endl << "-->> CPssMsgRecTykdQuery" << " Encode:" << std::endl;
    std::cout << std::endl <<  Stack.ToString() << std::endl << "<<--" << std::endl;
);
    CPssMsgRecTykdQuery __Msg;
    CPssStack __Stack;
    __Stack.Parse(Stack.ToString());
    __Stack.Decode(__Msg);
    CPPUNIT_ASSERT(Msg.m_iDetails == __Msg.m_iDetails );
    //__Msg.dump();
}
void CTestMan::testEncodeAndDecodeCPssMsgRecTykdQueryAck()
{
    CPssMsgRecTykdQueryAck Msg;
    CPssStack Stack;

    Msg.Result = "0";
    Msg.Reason = "ok";

    Msg.Items="10";
    Msg.Pages="5";
    Msg.TotalItems=50;

    CFileMsg __file;

    __file.Size    = "40465872";
    __file.StartDt = "1487638357";
    __file.EndDt   = "1487638397";

    __file.SnapUrl = "http://201.102.101.133:18080/rec/media_file/defRecSnap.jpg";
    __file.MotionUrl = "http://201.102.101.133:18080/rec/media_file/defRecMotion.mp4";

    __file.Url = "http://201.102.101.133:18080/rec/media_file/812A930E4BC_0_1/20170221/812A930E4BC_0_1-20170221_002016.mp4";
    Msg.Files.push_back(__file);
    __file.Url = "http://202.102.101.133:18080/rec/media_file/812A930E4BC_0_1/20170221/812A930E4BC_0_1-20170221_002016.mp4";
    Msg.Files.push_back(__file);
    __file.Url = "http://203.102.101.133:18080/rec/media_file/812A930E4BC_0_1/20170221/812A930E4BC_0_1-20170221_002016.mp4";
    Msg.Files.push_back(__file);

    Stack.Encode(Msg);
DBG(
    std::cout << std::endl << "-->> CPssMsgRecTykdQueryAck" << " Encode:" << std::endl;
    std::cout << std::endl <<  Stack.ToString() << std::endl << "<<--" << std::endl;
);
    CPssMsgRecTykdQueryAck __Msg;
    CPssStack __Stack;
    __Stack.Parse(Stack.ToString());
    __Stack.Decode(__Msg);
    CPPUNIT_ASSERT(Msg.Reason == __Msg.Reason );
    __Msg.dump();
}

void CTestMan::testEncodeAndDecodeCPssMsgNvpAccouontCreate()
{
    CPssMsgNvpAccouontCreate Msg;
    CPssStack Stack;

    Msg.MP       = "MP      ";
    Msg.Name     = "Name    ";
    Msg.Nick     = "Nick    ";
    Msg.Sex      = "Sex     ";
    Msg.Birthday = "Birthday";
    Msg.Add      = "Add     ";


    Stack.Encode(Msg);
DBG(
    std::cout << std::endl << "-->> CPssMsgNvpAccouontCreate" << " Encode:" << std::endl;
    std::cout << std::endl <<  Stack.ToString() << std::endl << "<<--" << std::endl;
);
    CPssMsgNvpAccouontCreate __Msg;
    CPssStack __Stack;
    __Stack.Parse(Stack.ToString());
    __Stack.Decode(__Msg);
    CPPUNIT_ASSERT(Msg.MP == __Msg.MP );
    //__Msg.dump();
}

void CTestMan::testEncodeAndDecodeCPssMsgNvpAccouontCreateAck()
{
    CPssStack Stack;
    CPssMsgNvpAccouontCreateAck Msg;

    Msg.Result = "0";
    Msg.Reason = "ok";

    Stack.Encode(Msg);
DBG(
    std::cout << std::endl << "-->> CPssMsgNvpAccouontCreateAck" << " Encode:" << std::endl;
    std::cout << std::endl <<  Stack.ToString() << std::endl << "<<--" << std::endl;
);
    CPssMsgNvpAccouontCreateAck __Msg;
    CPssStack __Stack;
    __Stack.Parse(Stack.ToString());
    __Stack.Decode(__Msg);
    CPPUNIT_ASSERT(Msg.Reason == __Msg.Reason );
    //__Msg.dump();
}

void CTestMan::testEncodeAndDecodeCPssMsgNvpAccouontModPwd()
{
    CPssMsgNvpAccouontMod Msg;
    CPssStack Stack;

    Msg.LoginName   = "18115886801";
    Msg.Password = "Password";

    Stack.Encode(Msg);
DBG(
    std::cout << std::endl << "-->> CPssMsgNvpAccouontMod" << " Encode:" << std::endl;
    std::cout << std::endl <<  Stack.ToString() << std::endl << "<<--" << std::endl;
);
    CPssMsgNvpAccouontMod __Msg;
    CPssStack __Stack;
    __Stack.Parse(Stack.ToString());
    __Stack.Decode(__Msg);
    CPPUNIT_ASSERT(Msg.LoginName == __Msg.LoginName );}

void CTestMan::testEncodeAndDecodeCPssMsgNvpShareCam2WeChat()
{
    CPssMsgNvpShareCam2WeChat Msg;
    CPssStack Stack;

    Msg.DevID   = "DevID  ";
    Msg.Channel = "Channel";
    Msg.Stream  = "Stream ";
    Msg.Title   = "Title  ";
    Msg.Memo    = "Memo   ";
    Msg.Addr    = "Addr   ";
    Msg.EndDt   = "EndDt  ";


    Stack.Encode(Msg);
DBG(
    std::cout << std::endl << "-->> CPssMsgNvpShareCam2WeChat" << " Encode:" << std::endl;
    std::cout << std::endl <<  Stack.ToString() << std::endl << "<<--" << std::endl;
);
    CPssMsgNvpShareCam2WeChat __Msg;
    CPssStack __Stack;
    __Stack.Parse(Stack.ToString());
    __Stack.Decode(__Msg);
    CPPUNIT_ASSERT(Msg.DevID == __Msg.DevID );
    //__Msg.dump();
}

void CTestMan::testEncodeAndDecodeCPssMsgNvpShareCam2WeChatAck()
{
    CPssStack Stack;
    CPssMsgNvpShareCam2WeChatAck Msg;

    Msg.Result = "0";
    Msg.Reason = "ok";

    Stack.Encode(Msg);
DBG(
    std::cout << std::endl << "-->> CPssMsgNvpShareCam2WeChatAck" << " Encode:" << std::endl;
    std::cout << std::endl <<  Stack.ToString() << std::endl << "<<--" << std::endl;
);
    CPssMsgNvpShareCam2WeChatAck __Msg;
    CPssStack __Stack;
    __Stack.Parse(Stack.ToString());
    __Stack.Decode(__Msg);
    CPPUNIT_ASSERT(Msg.Reason == __Msg.Reason );
    //__Msg.dump();
}

void CTestMan::testEncodeAndDecodeCPssMsgNvpRegister()
{
    CPssMsgNvpRegister Msg;
    CPssStack Stack;

    Msg.Sequence  = "1";
    Msg.Type      = "1";
    Msg.Vendor    = "Oracle";
    Msg.Module    = "K100W";
    Msg.DevID     = "812A930AC7C";
    Msg.DevKey    = "B5DD4F86";
    Msg.AppVer    = "10.3.1(14E304-General-20170511)";
    Msg.Addr      = "Beijing China";
    Msg.LanMac    = "00:0c:29:8d:b4:d0";
    Msg.LanIP     = "192.168.10.20";
    Msg.WLanMac   = "00:0c:29:8d:b4:d1";
    Msg.WLanIP    = "192.168.1.2";
    Msg.DataUrl   = "rtsp://192.168.1.2:18554";
    Msg.CommUrl   = "tcp://192.168.1.2:37777";
    Msg.WebUrl    = "http://192.168.1.2:80";

    Msg.DataUrlTu = "rtsp://61.160.149.159:18554/112A0000034_0_2.sdp?s=51cd7c1337d4955323496069e5ca848ef390edc7";
    Msg.CommUrlTu = "tcp://61.160.149.177:37687";
    Msg.WebUrlTu  = "http://61.160.149.177:37688";


    Stack.Encode(Msg);
DBG(
    std::cout << std::endl << "-->> " << __FUNCTION__ << " Encode:" << std::endl;
    std::cout << std::endl <<  Stack.ToString() << std::endl << "<<--" << std::endl;
);
    CPssMsgNvpRegister __Msg;
    CPssStack __Stack;
    __Stack.Parse(Stack.ToString());
    __Stack.Decode(__Msg);
    CPPUNIT_ASSERT(Msg.DevID == __Msg.DevID );
    //__Msg.dump();
}

void CTestMan::testEncodeAndDecodeCPssMsgNvpRegisterAck()
{
    // ipc
    {
        CPssStack Stack;
        CPssMsgNvpRegisterAck Msg;

        Msg.Result = "0";
        Msg.Reason = "ok";

        Msg.UrlDataSrv = "turn://nvpt2.ezlibs.com:18080";
        Msg.UrlCommSrv = "http://nvps3.ezlibs.com:18080";

        Msg.UrlStorSrv = "http://nvps4.ezlibs.com:18080";
        Msg.UrlEventsSrv = "http://nvps5.ezlibs.com:18080";
        Msg.UrlAppSrv = "http://nvps6.ezlibs.com:18080";

        Stack.Encode(Msg);
DBG(
        std::cout << std::endl << "-->> " << __FUNCTION__ << " Encode:" << std::endl;
        std::cout << std::endl <<  Stack.ToString() << std::endl << "<<--" << std::endl;
);
        CPssMsgNvpRegisterAck __Msg;
        CPssStack __Stack;
        __Stack.Parse(Stack.ToString());
        __Stack.Decode(__Msg);
        CPPUNIT_ASSERT(Msg.Reason == __Msg.Reason );
    }

    // register
    // 不返回url的情况
    {
        CPssStack Stack;
        CPssMsgNvpRegisterAck Msg;

        Msg.Result = "0";
        Msg.Reason = "ok";

        //Msg.UrlDataSrv = "turn://nvps2.ezlibs.com:18080";
        //Msg.UrlCommSrv = "http://nvps2.ezlibs.com:18080";

        Stack.Encode(Msg);
DBG(
        std::cout << std::endl << "-->> " << __FUNCTION__ << " Encode:" << std::endl;
        std::cout << std::endl <<  Stack.ToString() << std::endl << "<<--" << std::endl;
);
    }

    // register
    // 返回ads的情况
    {
        CPssStack Stack;
        CPssMsgNvpRegisterAck Msg;

        Msg.Result = "0";
        Msg.Reason = "ok";

        //Msg.UrlDataSrv = "turn://nvps2.ezlibs.com:18080";
        //Msg.UrlCommSrv = "http://nvps2.ezlibs.com:18080";

        // file 1
        {
            CAdsFileMsg __file;

            __file.Url      = "http://221.224.63.102:18080/ads/guide0.png";
            __file.Type     = "1";
            __file.Length   = "";
            __file.Delay    = "3";
            __file.StartDt  = "1493971368";
            __file.EndDt    = "1493971369";

            Msg.AdsFiles.push_back(__file);
        }
        // file 2
        {
            CAdsFileMsg __file;

            __file.Url      = "http://221.224.63.102:18080/ads/guide1.png";
            __file.Type     = "1";
            __file.Length   = "";
            __file.Delay    = "3";
            __file.StartDt  = "1493971368";
            __file.EndDt    = "1493971369";

            Msg.AdsFiles.push_back(__file);
        }
        // file 3
        {
            CAdsFileMsg __file;

            __file.Url      = "http://221.224.63.102:18080/ads/guide2.png";
            __file.Type     = "1";
            __file.Length   = "";
            __file.Delay    = "3";
            __file.StartDt  = "1493971368";
            __file.EndDt    = "1493971369";

            Msg.AdsFiles.push_back(__file);
        }

        Stack.Encode(Msg);
DBG(
        std::cout << std::endl << "-->> " << __FUNCTION__ << " Encode:" << std::endl;
        std::cout << std::endl <<  Stack.ToString() << std::endl << "<<--" << std::endl;
);
    }

}

void CTestMan::testEncodeAndDecodeCPssMsgNvpTouchDev()
{
    CPssMsgNvpTermInfo Msg;
    CPssStack Stack;

    Msg.DevID = "812A930E4BC";
    Msg.DevKey = "C5OMM8AA";

    Stack.Encode(Msg);
DBG(
    std::cout << std::endl << "-->> CPssMsgNvpTermInfo" << " Encode:" << std::endl;
    std::cout << std::endl <<  Stack.ToString() << std::endl << "<<--" << std::endl;
);
    CPssMsgNvpTermInfo __Msg;
    CPssStack __Stack;
    __Stack.Parse(Stack.ToString());
    __Stack.Decode(__Msg);
    CPPUNIT_ASSERT(Msg.DevID == __Msg.DevID );
    //__Msg.dump();
}

void CTestMan::testEncodeAndDecodeCPssMsgNvpTouchDevAck()
{
    CPssStack Stack;
    CPssMsgNvpTermInfoAck Msg;

    Msg.Result = "0";
    Msg.Reason = "ok";
    Msg.Binded = "1";
    Msg.Online = "1";

    Stack.Encode(Msg);
DBG(
    std::cout << std::endl << "-->> CPssMsgNvpTermInfoAck" << " Encode:" << std::endl;
    std::cout << std::endl <<  Stack.ToString() << std::endl << "<<--" << std::endl;
);
    CPssMsgNvpTermInfoAck __Msg;
    CPssStack __Stack;
    __Stack.Parse(Stack.ToString());
    __Stack.Decode(__Msg);
    CPPUNIT_ASSERT(Msg.Reason == __Msg.Reason );
    //__Msg.dump();
}

void CTestMan::testEncodeAndDecodeCPssMsgHlsPullStreamStart()
{
    // 海康
    {
        CPssMsgHlsPullStreamStart Msg;
        CPssStack Stack;

        Msg.ID    = "C056E37537D2_1_1";
        Msg.Type  = "2";
        Msg.ValidPeriod  = "3600";

        Msg.StandStream.Url           = "rtsp://110.167.76.80:554" ;
        Msg.StandStream.Account       = "admin" ;
        Msg.StandStream.Passwd        = "12345" ;
        Msg.StandStream.Channel       = "1" ;
        Msg.StandStream.Subtype       = "1" ;
        Msg.StandStream.Location      = "010-111" ;
        Msg.StandStream.VideoEncode   = "0" ;
        Msg.StandStream.VideoResize   = "0" ;
        Msg.StandStream.VideoRefps    = "0" ;
        Msg.StandStream.AudioEncode   = "0" ;
        Msg.StandStream.Description   = "NiuXinshan" ;

        Msg.Storage.Type          = "2" ;
        Msg.Storage.Name          = "nvr://account:password@192.168.5.208:8000/sreq?http=80&rtsp=554&onvif=no" ;
        //Msg.Storage.Account       = "admin" ;
        //Msg.Storage.Passwd        = "admin123" ;
        Msg.Storage.Channel       = "1" ;
        Msg.Storage.Description   = "nvr_hik" ;

        Stack.Encode(Msg);
DBG(
        std::cout << std::endl << "-->> " << __FUNCTION__ << " Encode:" << std::endl;
        std::cout << std::endl <<  Stack.ToString() << std::endl << "<<--" << std::endl;
);
        CPssMsgHlsPullStreamStart __Msg;
        CPssStack __Stack;
        __Stack.Parse(Stack.ToString());
        __Stack.Decode(__Msg);
        CPPUNIT_ASSERT(Msg.ID == __Msg.ID );
        CPPUNIT_ASSERT(Msg.StandStream.Url == __Msg.StandStream.Url );
        CPPUNIT_ASSERT(Msg.Storage.Name == __Msg.Storage.Name );
    }
    // 全球眼
    {
        CPssMsgHlsPullStreamStart Msg;
        CPssStack Stack;

        Msg.ID  = "011001010561160004";
        Msg.Type  = "4";

        Msg.StandStream.Url           = "http://58.223.251.12:9001" ;
        Msg.StandStream.Account       = "Account" ;
        Msg.StandStream.Passwd        = "Passwd" ;
        Msg.StandStream.Channel       = "n/a" ;
        Msg.StandStream.Subtype       = "n/a" ;
        Msg.StandStream.Location      = "010-111" ;
        Msg.StandStream.VideoEncode   = "0" ;
        Msg.StandStream.Description   = "mega eye" ;

        Stack.Encode(Msg);
DBG(
        std::cout << std::endl << "-->> " << __FUNCTION__ << " Encode:" << std::endl;
        std::cout << std::endl <<  Stack.ToString() << std::endl << "<<--" << std::endl;
);
        CPssMsgHlsPullStreamStart __Msg;
        CPssStack __Stack;
        __Stack.Parse(Stack.ToString());
        __Stack.Decode(__Msg);
        CPPUNIT_ASSERT(Msg.ID == __Msg.ID );
    }
}
#if 0
void CTestMan::testEncodeAndDecodeCPssMsgPullStreamStart()
{
    // tykd
    {
        CPssMsgPullStreamStart Msg;
        CPssStack Stack;

        Msg.ID    = "112B0048070";
        Msg.Type  = "5";//ezDef_StreamType_Str_tykd
        Msg.ValidPeriod  = "3600";

        Msg.StreamSrc.Url           = "n/a" ;
        Msg.StreamSrc.Account       = "pxgadj" ;
        Msg.StreamSrc.Passwd        = "12345@abc" ;
        Msg.StreamSrc.Channel       = "1" ;
        Msg.StreamSrc.Subtype       = "1" ;
        Msg.StreamSrc.Location      = "010-111" ;
        Msg.StreamSrc.VideoEncode   = "0" ;
        Msg.StreamSrc.VideoResize   = "0" ;
        Msg.StreamSrc.VideoRefps    = "0" ;
        Msg.StreamSrc.AudioEncode   = "0" ;
        Msg.StreamSrc.Description   = "peixian.mzwsy" ;

        Msg.StreamDst.Url           = "rtp://192.168.9.214:5004" ;
        Msg.StreamDst.Account       = "" ;
        Msg.StreamDst.Passwd        = "" ;
        Msg.StreamDst.Channel       = "" ;
        Msg.StreamDst.Subtype       = "" ;
        Msg.StreamDst.Location      = "" ;
        Msg.StreamDst.VideoEncode   = "0" ;
        Msg.StreamDst.VideoResize   = "0" ;
        Msg.StreamDst.VideoRefps    = "0" ;
        Msg.StreamDst.AudioEncode   = "0" ;
        Msg.StreamDst.Description   = "peixian.mzwsy" ;

        Stack.Encode(Msg);
DBG(
        std::cout << std::endl << "-->> " << __FUNCTION__ << " Encode:" << std::endl;
        std::cout << std::endl <<  Stack.ToString() << std::endl << "<<--" << std::endl;
);
        CPssMsgPullStreamStart __Msg;
        CPssStack __Stack;
        __Stack.Parse(Stack.ToString());
        __Stack.Decode(__Msg);
        CPPUNIT_ASSERT(Msg.ID == __Msg.ID );
    }
}
#endif
void CTestMan::testEncodeAndDecodeCPssMsgDeliveryStreamStartAck()
{
    CPssMsgDeliveryStreamStartAck Msg;
    CPssStack Stack;

    Msg.Result = "0";
    Msg.Reason = "ok";

    //Msg.Delivery.ValidPeriod="0";
    Msg.Delivery.UrlHls="http://pss.ezlibs.com:18080/hls/46ebff14c54107c15a49fea4d33b545e0a787393.m3u8";
    Msg.Delivery.UrlRtmp="rtmp://pss.ezlibs.com:19090/live/46ebff14c54107c15a49fea4d33b545e0a787393";
    Msg.Delivery.UrlRtsp="rtsp://pss.ezlibs.com:18554/46ebff14c54107c15a49fea4d33b545e0a787393.sdp";
    Msg.Delivery.Snapshot="http://pss.ezlibs.com:18080/snap/46ebff14c54107c15a49fea4d33b545e0a787393.jpg";
    Msg.Delivery.Description="trip";

    Stack.Encode(Msg);
DBG(
    std::cout << std::endl << "-->> CPssMsgDeliveryStreamStartAck" << " Encode:" << std::endl;
    std::cout << std::endl <<  Stack.ToString() << std::endl << "<<--" << std::endl;
);
    CPssMsgHlsPullTykdStartAck __Msg;
    CPssStack __Stack;
    __Stack.Parse(Stack.ToString());
    __Stack.Decode(__Msg);
    CPPUNIT_ASSERT(Msg.Reason == __Msg.Reason );
    //__Msg.dump();
}

void CTestMan::testEncodeAndDecodeCPssMsgRecStreamStart()
{
    // 海康
    {
        CPssMsgRecStreamStart Msg;
        CPssStack Stack;

        Msg.ID  = "C056E37537D2_1_1";
        Msg.Type  = "2";

        Msg.StandStream.Url           = "rtsp://110.167.76.80:554" ;
        Msg.StandStream.Account       = "admin" ;
        Msg.StandStream.Passwd        = "12345" ;
        Msg.StandStream.Channel       = "1" ;
        Msg.StandStream.Subtype       = "1" ;
        Msg.StandStream.Location      = "010-111" ;
        Msg.StandStream.VideoEncode   = "0" ;
        Msg.StandStream.Description   = "NiuXinshan" ;

        Msg.Rec.ValidPeriod = "30000";
        Msg.Rec.FilePeriod= "300";
        Msg.Rec.RecPeriod= "300";

        Stack.Encode(Msg);
//DBG(
        std::cout << std::endl << "-->> " << __FUNCTION__ << " Encode:" << std::endl;
        std::cout << std::endl <<  Stack.ToString() << std::endl << "<<--" << std::endl;
//);
        CPssMsgRecStreamStart __Msg;
        CPssStack __Stack;
        __Stack.Parse(Stack.ToString());
        __Stack.Decode(__Msg);
        CPPUNIT_ASSERT(Msg.ID == __Msg.ID );
    }
    // 全球眼
    {
        CPssMsgRecStreamStart Msg;
        CPssStack Stack;

        Msg.ID  = "010175010501420004";
        Msg.Type  = "4";

        Msg.StandStream.Url           = "http://58.223.251.12:9001" ;
        Msg.StandStream.Account       = "tykjt_njjj" ;
        Msg.StandStream.Passwd        = "njjj12345" ;
        Msg.StandStream.Channel       = "1" ;
        Msg.StandStream.Subtype       = "1" ;
        Msg.StandStream.Location      = "010-111" ;
        Msg.StandStream.VideoEncode   = "0" ;
        Msg.StandStream.Description   = "mega eye" ;

        Msg.Rec.ValidPeriod = "3000";
        Msg.Rec.FilePeriod= "300";

        Stack.Encode(Msg);
DBG(
        std::cout << std::endl << "-->> " << __FUNCTION__ << " Encode:" << std::endl;
        std::cout << std::endl <<  Stack.ToString() << std::endl << "<<--" << std::endl;
);
        CPssMsgRecStreamStart __Msg;
        CPssStack __Stack;
        __Stack.Parse(Stack.ToString());
        __Stack.Decode(__Msg);
        CPPUNIT_ASSERT(Msg.ID == __Msg.ID );
    }
}

void CTestMan::testEncodeAndDecodeCPssMsgRecStreamStartAck()
{
    CPssMsgRecStreamStartAck Msg;
    CPssStack Stack;

    Msg.Result = "0";
    Msg.Reason = "ok";

    Msg.Status       = "";
    Msg.UpStatus     = "";
    Msg.UsedSpace    = 0;
    Msg.UsedUpSpace  = 0;

    Stack.Encode(Msg);
DBG(
    std::cout << std::endl << "-->> " << __FUNCTION__ << " Encode:" << std::endl;
    std::cout << std::endl <<  Stack.ToString() << std::endl << "<<--" << std::endl;
);
    CPssMsgRecStreamStartAck __Msg;
    CPssStack __Stack;
    __Stack.Parse(Stack.ToString());
    __Stack.Decode(__Msg);
    CPPUNIT_ASSERT(Msg.Reason == __Msg.Reason );
    CPPUNIT_ASSERT(Msg.Status == __Msg.Status );
    CPPUNIT_ASSERT(Msg.UsedSpace == __Msg.UsedSpace );
    //__Msg.dump();
}

void CTestMan::testEncodeAndDecodeCPssMsgEventsPub()
{
    CPssMsgEventsPub Msg;
    CPssStack Stack;

    Msg.DevID = "811BF642B07";

    CEventMsg __file;

    __file.Type    = "1";
    __file.Data    = "1";
    __file.StartDt = "1487638357";
    __file.EndDt   = "1487638397";
    __file.Description    = "1";

    Msg.Event.push_back(__file);
    __file.Type    = "2";
    __file.Data    = "222";
    Msg.Event.push_back(__file);
    __file.Type    = "3";
    __file.Data    = "333";
    Msg.Event.push_back(__file);

    Stack.Encode(Msg);

DBG(
    std::cout << std::endl << "-->> CPssMsgEventsPub" << " Encode:" << std::endl;
    std::cout << std::endl <<  Stack.ToString() << std::endl << "<<--" << std::endl;
);
    CPssMsgEventsPub __Msg;
    CPssStack __Stack;
    __Stack.Parse(Stack.ToString());
    __Stack.Decode(__Msg);
    CPPUNIT_ASSERT(Msg.DevID == __Msg.DevID );
    __Msg.dump();

}

void CTestMan::testEncodeDecode_requestMsgDeviceList()
{
    CPssStack Stack;

    CPssMsg_requestMsgDeviceList Msg;

    Msg.m_strGroupID  = "GID";
    Msg.m_iPageSize = 9999;
    Msg.m_iPages    = 1;


    Stack.Encode(Msg);

DBG(
    std::cout << std::endl << "-->> CPssMsg_requestMsgDeviceList" << " Encode:" << std::endl;
    std::cout << std::endl <<  Stack.ToString() << std::endl << "<<--" << std::endl;
);
    CPssStack __Stack;
    CPssMsg_requestMsgDeviceList __Msg;

    __Stack.Parse(Stack.ToString());
    __Stack.Decode(__Msg);

    CPPUNIT_ASSERT(Msg.m_strGroupID == __Msg.m_strGroupID );

}

void CTestMan::testEncodeDecode_requestMsgDeviceListAck()
{
    CPssStack Stack;

    CPssMsg_requestMsgDeviceListAck Msg;
    CDeviceMsg Value;

    Msg.Result = "0";
    Msg.Reason = "ok";

    Msg.m_strGroupID      = "GID-001";
    Msg.m_iTotalCount   = 2;
    Msg.m_iPages        = 1;

    Value.m_strDevID = "Dev1";
    Value.m_strCode = "T1";
    Value.m_strVersion = "1.0.1";
    Value.m_strStatus = "1";
    Value.m_strLastOnTime = "1527688051";
    Value.m_strName         = "OfficeCam";
    Value.m_strVendor       = "SomeBody";
    Value.m_strLocation     = "Location";
    Value.m_strDescription  = "Description";

    Value.m_strChannelNum   = "1";
    Value.m_strHostIp       = "192.168.9.193";
    Value.m_strHttpPort     = "80";
    Value.m_strRtspPort     = "554";
    Value.m_strTcpPort      = "8000";


    Msg.Devices.push_back(Value);


    Value.m_strDevID = "Dev2";
    Value.m_strCode = "T1";
    Value.m_strStatus = "1";
    Value.m_strLastOnTime = "1527688051";

    Msg.Devices.push_back(Value);

    Stack.Encode(Msg);

DBG(
    std::cout << std::endl << "-->> CPssMsg_requestMsgDeviceListAck" << " Encode:" << std::endl;
    std::cout << std::endl <<  Stack.ToString() << std::endl << "<<--" << std::endl;
);
    CPssStack __Stack;
    CPssMsg_requestMsgDeviceListAck __Msg;

    __Stack.Parse(Stack.ToString());
    __Stack.Decode(__Msg);

    CPPUNIT_ASSERT(Msg.Result == __Msg.Result );
    CPPUNIT_ASSERT(Msg.m_strGroupID == __Msg.m_strGroupID );
}

void CTestMan::testEncodeDecode_requestMsgOption()
{
    CPssStack Stack;

    CUniMsg_requestMsgOption Msg;

    Msg.m_strAgent  = "PssClient-1.0";

    Stack.Encode(Msg);

DBG(
    std::cout << std::endl << "-->> CUniMsg_requestMsgOption" << " Encode:" << std::endl;
    std::cout << std::endl <<  Stack.ToString() << std::endl << "<<--" << std::endl;
);
    CPssStack __Stack;
    CUniMsg_requestMsgOption __Msg;

    __Stack.Parse(Stack.ToString());
    __Stack.Decode(__Msg);

    CPPUNIT_ASSERT(Msg.m_strAgent == __Msg.m_strAgent );

}

void CTestMan::testEncodeDecode_requestMsgOptionAck()
{
    CPssStack Stack;

    CUniMsg_requestMsgOptionAck Msg;

    CFirmMsg Value;

    Msg.Result = "0";
    Msg.Reason = "ok";
    Msg.m_strServer      = "liveStreamming-uniTrans.195:203";

    Value.m_strDesc = "hikvision.com";
    Value.m_strCode = "2";
    Msg.Firms.push_back(Value);

    Value.m_strDesc = "dahuatech.com";
    Value.m_strCode = "3";
    Msg.Firms.push_back(Value);

    Stack.Encode(Msg);

DBG(
    std::cout << std::endl << "-->> CUniMsg_requestMsgOptionAck" << " Encode:" << std::endl;
    std::cout << std::endl <<  Stack.ToString() << std::endl << "<<--" << std::endl;
);
    CPssStack __Stack;
    CUniMsg_requestMsgOptionAck __Msg;

    __Stack.Parse(Stack.ToString());
    __Stack.Decode(__Msg);

    CPPUNIT_ASSERT(Msg.Result == __Msg.Result );
    CPPUNIT_ASSERT(Msg.m_strServer == __Msg.m_strServer );
}

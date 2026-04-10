/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * TestMan.h - _explain_
 *
 * Copyright (C) 2013 gminiTech.com, All Rights Reserved.
 *
 * $Id: TestMan.h 0001 2013-11-02 14:38:29Z wu_junjie $
 *
 *  Explain:
 *     UnitTest -- CInterThings_gLockStack
 *
 *  Update:
 *     2013-11-02 14:38:33 WuJunjie 549 Create
 *
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#if !defined(_MANTEST_H_)
#define _MANTEST_H_

#include <cppunit/extensions/HelperMacros.h>

class CTestMan : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE( CTestMan );

    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
    CPPUNIT_TEST( testEncodeCAckMsgSimple );

    CPPUNIT_TEST( testEncodeAndDecodeCUniMsg_reqCreateAppKey );
    CPPUNIT_TEST( testEncodeAndDecodeCUniMsg_reqCreateAppKeyAck );

    CPPUNIT_TEST( testEncodeAndDecodeCUniMsg_reqGetSetGmLic );
    CPPUNIT_TEST( testEncodeAndDecodeCUniMsg_reqGetSetGmLicAck );

    CPPUNIT_TEST( testEncodeAndDecodeCUniMsg_reqConfigModify );

#if 1
	// hls ok
    CPPUNIT_TEST( testEncodeAndDecodeCPssMsgHlsPullTykdStart );
    CPPUNIT_TEST( testEncodeAndDecodeCPssMsgHlsPullTykdStop );
    CPPUNIT_TEST( testEncodeAndDecodeCPssMsgHlsPullTykdStartAck );
    CPPUNIT_TEST( testEncodeAndDecodeCPssMsgHlsPullTykdStopAck );
#endif

#if 1
	// rtsp ok
    CPPUNIT_TEST( testEncodeAndDecodeCPssMsgRtspPullTykdStart );
    CPPUNIT_TEST( testEncodeAndDecodeCPssMsgRtspPullTykdStop );
    CPPUNIT_TEST( testEncodeAndDecodeCPssMsgRtspPullTykdStartAck );
    CPPUNIT_TEST( testEncodeAndDecodeCPssMsgRtspPullTykdStopAck );
#endif

#if 1
	// rtmp
    CPPUNIT_TEST( testEncodeAndDecodeCPssMsgRtmpPullTykdStart );
    CPPUNIT_TEST( testEncodeAndDecodeCPssMsgRtmpPullTykdStop );
    CPPUNIT_TEST( testEncodeAndDecodeCPssMsgRtmpPullTykdStartAck );
    CPPUNIT_TEST( testEncodeAndDecodeCPssMsgRtmpPullTykdStopAck );
#endif

#if 1
	// PandoraMsg
    CPPUNIT_TEST( testEncodeAndDecodeCPandoraMsg_TykdPullStart );
    CPPUNIT_TEST( testEncodeAndDecodeCPandoraMsg_TykdPullStartAck );
//    CPPUNIT_TEST( testEncodeAndDecodeCPandoraMsg_TykdPullStop );
//    CPPUNIT_TEST( testEncodeAndDecodeCPandoraMsg_TykdPullStopAck );
#endif

#if 1 // 2016-11-11 9:26:24 通道管理
	// PandoraMsg
    CPPUNIT_TEST( testEncodeAndDecodeCPandoraMsg_pssChannelCreate);
    CPPUNIT_TEST( testEncodeAndDecodeCPandoraMsg_pssChannelCreateAck);
    CPPUNIT_TEST( testEncodeAndDecodeCPandoraMsg_pssChannelCancel);
    CPPUNIT_TEST( testEncodeAndDecodeCPandoraMsg_pssChannelCancelAck);
    CPPUNIT_TEST( testEncodeAndDecodeCPandoraMsg_pssChannelQuery);
    CPPUNIT_TEST( testEncodeAndDecodeCPandoraMsg_pssChannelQueryAck);
    CPPUNIT_TEST( testEncodeAndDecodeCPandoraMsg_pssChannelList);
    CPPUNIT_TEST( testEncodeAndDecodeCPandoraMsg_pssChannelListAck);
#endif

    CPPUNIT_TEST( testEncodeAndDecodeCStreamCapabilityMsg);

#if 1
	// rec ok
    CPPUNIT_TEST( testEncodeAndDecodeCPssMsgRecTykdStart );
    CPPUNIT_TEST( testEncodeAndDecodeCPssMsgRecTykdStartAck );
    CPPUNIT_TEST( testEncodeAndDecodeCPssMsgRecTykdStop );
    CPPUNIT_TEST( testEncodeAndDecodeCPssMsgRecTykdStopAck );
    CPPUNIT_TEST( testEncodeAndDecodeCPssMsgRecTykdDel );
    CPPUNIT_TEST( testEncodeAndDecodeCPssMsgRecTykdDelAck );
    CPPUNIT_TEST( testEncodeAndDecodeCPssMsgRecTykdQuery );
    CPPUNIT_TEST( testEncodeAndDecodeCPssMsgRecTykdQueryAck );
#endif
//创建用户
#if 1
    CPPUNIT_TEST( testEncodeAndDecodeCPssMsgNvpAccouontCreate );
    CPPUNIT_TEST( testEncodeAndDecodeCPssMsgNvpAccouontCreateAck );

    // 修改密码
    CPPUNIT_TEST( testEncodeAndDecodeCPssMsgNvpAccouontModPwd );
#endif//
#if 1
    CPPUNIT_TEST( testEncodeAndDecodeCPssMsgNvpShareCam2WeChat );
    CPPUNIT_TEST( testEncodeAndDecodeCPssMsgNvpShareCam2WeChatAck );
#endif//
#if 1
    CPPUNIT_TEST( testEncodeAndDecodeCPssMsgNvpRegister );
    CPPUNIT_TEST( testEncodeAndDecodeCPssMsgNvpRegisterAck );
    CPPUNIT_TEST( testEncodeAndDecodeCPssMsgNvpTouchDev);
    CPPUNIT_TEST( testEncodeAndDecodeCPssMsgNvpTouchDevAck );
#endif//

#if 1
    CPPUNIT_TEST( testEncodeAndDecodeCPssMsgHlsPullStreamStart );
#endif//
    //CPPUNIT_TEST( testEncodeAndDecodeCPssMsgPullStreamStart );

#if 1
    CPPUNIT_TEST( testEncodeAndDecodeCPssMsgRecStreamStart );
    CPPUNIT_TEST( testEncodeAndDecodeCPssMsgRecStreamStartAck );
#endif//

#if 1
    CPPUNIT_TEST( testEncodeAndDecodeCPssMsgEventsPub );
#endif//

    //
    CPPUNIT_TEST( testEncodeAndDecodeCPssMsgDeliveryStreamStartAck );
    // 2018-10-22 10:17:53
    CPPUNIT_TEST( testEncodeDecode_requestMsgDeviceList );
    CPPUNIT_TEST( testEncodeDecode_requestMsgDeviceListAck );

    CPPUNIT_TEST( testEncodeDecode_requestMsgOption );
    CPPUNIT_TEST( testEncodeDecode_requestMsgOptionAck );
    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

    CPPUNIT_TEST_SUITE_END();

public:
    void testSmsStackParse();

    void testEncodeCAckMsgSimple();

    void testEncodeAndDecodeCUniMsg_reqCreateAppKey();
    void testEncodeAndDecodeCUniMsg_reqCreateAppKeyAck();

    void testEncodeAndDecodeCUniMsg_reqGetSetGmLic();
    void testEncodeAndDecodeCUniMsg_reqGetSetGmLicAck();

    void testEncodeAndDecodeCUniMsg_reqConfigModify();

    void testEncodeAndDecodeCPssMsgHlsPullTykdStart();
    void testEncodeAndDecodeCPssMsgHlsPullTykdStop();
    void testEncodeAndDecodeCPssMsgHlsPullTykdStartAck();
    void testEncodeAndDecodeCPssMsgHlsPullTykdStopAck();

    void testEncodeAndDecodeCPssMsgRtspPullTykdStart();
    void testEncodeAndDecodeCPssMsgRtspPullTykdStop();
    void testEncodeAndDecodeCPssMsgRtspPullTykdStartAck();
    void testEncodeAndDecodeCPssMsgRtspPullTykdStopAck();

    void testEncodeAndDecodeCPssMsgRtmpPullTykdStart();
    void testEncodeAndDecodeCPssMsgRtmpPullTykdStop();
    void testEncodeAndDecodeCPssMsgRtmpPullTykdStartAck();
    void testEncodeAndDecodeCPssMsgRtmpPullTykdStopAck();

    void testEncodeAndDecodeCPandoraMsg_TykdPullStart();
    void testEncodeAndDecodeCPandoraMsg_TykdPullStartAck();
    void testEncodeAndDecodeCPandoraMsg_TykdPullStop();
    void testEncodeAndDecodeCPandoraMsg_TykdPullStopAck();
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
// 通道管理
	void testEncodeAndDecodeCPandoraMsg_pssChannelCreate();
	void testEncodeAndDecodeCPandoraMsg_pssChannelCreateAck();
	void testEncodeAndDecodeCPandoraMsg_pssChannelCancel();
	void testEncodeAndDecodeCPandoraMsg_pssChannelCancelAck();
	void testEncodeAndDecodeCPandoraMsg_pssChannelQuery();
	void testEncodeAndDecodeCPandoraMsg_pssChannelQueryAck();
	void testEncodeAndDecodeCPandoraMsg_pssChannelList();
	void testEncodeAndDecodeCPandoraMsg_pssChannelListAck();
	
	void testEncodeAndDecodeCStreamCapabilityMsg();
	
    void testEncodeAndDecodeCPssMsgRecTykdStart();
    void testEncodeAndDecodeCPssMsgRecTykdStartAck();
    void testEncodeAndDecodeCPssMsgRecTykdStop();
    void testEncodeAndDecodeCPssMsgRecTykdStopAck();
    void testEncodeAndDecodeCPssMsgRecTykdDel();
    void testEncodeAndDecodeCPssMsgRecTykdDelAck();
    void testEncodeAndDecodeCPssMsgRecTykdQuery();
    void testEncodeAndDecodeCPssMsgRecTykdQueryAck();

    void testEncodeAndDecodeCPssMsgNvpAccouontCreate();
    void testEncodeAndDecodeCPssMsgNvpAccouontCreateAck();
    void testEncodeAndDecodeCPssMsgNvpAccouontModPwd();
    void testEncodeAndDecodeCPssMsgNvpShareCam2WeChat();
    void testEncodeAndDecodeCPssMsgNvpShareCam2WeChatAck();

    void testEncodeAndDecodeCPssMsgNvpRegister();
    void testEncodeAndDecodeCPssMsgNvpRegisterAck();
    void testEncodeAndDecodeCPssMsgNvpTouchDev();
    void testEncodeAndDecodeCPssMsgNvpTouchDevAck();

    void testEncodeAndDecodeCPssMsgHlsPullStreamStart();
    void testEncodeAndDecodeCPssMsgPullStreamStart();
    void testEncodeAndDecodeCPssMsgRecStreamStart();
    void testEncodeAndDecodeCPssMsgRecStreamStartAck();
    void testEncodeAndDecodeCPssMsgDeliveryStreamStartAck();

    void testEncodeAndDecodeCPssMsgEventsPub();

    void testEncodeDecode_requestMsgDeviceList();
    void testEncodeDecode_requestMsgDeviceListAck();

    void testEncodeDecode_requestMsgOption();
    void testEncodeDecode_requestMsgOptionAck();

/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
    void setUp();
    void tearDown();
};

#endif // _MANTEST_H_

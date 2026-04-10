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
    CPPUNIT_TEST(testEncodeDecode_requestMsg);
    CPPUNIT_TEST(testEncodeDecode_requestMsgAck);

    CPPUNIT_TEST(testEncodeDecode_requestMsgPub_IO_Out);
    CPPUNIT_TEST(testEncodeDecode_requestMsgShareHID);

    CPPUNIT_TEST( testEncodeDecode_requestMsgPubValue);
    CPPUNIT_TEST( testEncodeDecode_requestMsgPubValueAck);
    CPPUNIT_TEST( testEncodeDecode_requestMsgPubValueList);
    CPPUNIT_TEST( testEncodeDecode_requestMsgPubValueListAck);

    CPPUNIT_TEST( testEncodeDecode_requestMsgSetSchedule);
    CPPUNIT_TEST( testEncodeDecode_requestMsgGetSchedule);
    CPPUNIT_TEST( testEncodeDecode_requestMsgGetScheduleAck);

    CPPUNIT_TEST( testEncodeDecode_requestMsgTaskPub);
    CPPUNIT_TEST( testEncodeDecode_requestMsgTaskList);
    CPPUNIT_TEST( testEncodeDecode_requestMsgTaskListAck);
    CPPUNIT_TEST( testEncodeDecode_requestMsgTaskDetails);
    CPPUNIT_TEST( testEncodeDecode_requestMsgTaskDetailsAck);

    CPPUNIT_TEST( testEncodeDecode_requestMsgDevices);
    CPPUNIT_TEST( testEncodeDecode_requestMsgDevicesAck);

    CPPUNIT_TEST( testEncodeDecode_requestMsgSetShpc);
    CPPUNIT_TEST( testEncodeDecode_requestMsgSetShpcAck);
    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

    CPPUNIT_TEST_SUITE_END();

public:

    void testEncodeDecode_requestMsg();
    void testEncodeDecode_requestMsgAck();

    void testEncodeDecode_requestMsgPub_IO_Out();
    void testEncodeDecode_requestMsgShareHID();

    void testEncodeDecode_requestMsgPubValue();
    void testEncodeDecode_requestMsgPubValueAck();
    void testEncodeDecode_requestMsgPubValueList();
    void testEncodeDecode_requestMsgPubValueListAck();

    void testEncodeDecode_requestMsgSetSchedule();
    void testEncodeDecode_requestMsgGetSchedule();
    void testEncodeDecode_requestMsgGetScheduleAck();

    void testEncodeDecode_requestMsgTaskPub();
    void testEncodeDecode_requestMsgTaskList();
    void testEncodeDecode_requestMsgTaskListAck();
    void testEncodeDecode_requestMsgTaskDetails();
    void testEncodeDecode_requestMsgTaskDetailsAck();

    void testEncodeDecode_requestMsgDevices();
    void testEncodeDecode_requestMsgDevicesAck();

    void testEncodeDecode_requestMsgSetShpc();
    void testEncodeDecode_requestMsgSetShpcAck();
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
    void setUp();
    void tearDown();
};

#endif // _MANTEST_H_

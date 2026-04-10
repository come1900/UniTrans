/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * TestMan.h - _explain_
 *
 * Copyright (C) 2013 gminiTech.com, All Rights Reserved.
 *
 * $Id: TestMan.h 0001 2013-11-02 14:38:29Z wu_junjie $
 *
 *  Explain:
 *     UnitTest -- CpromQLStack
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
    CPPUNIT_TEST( test_CMmtMsgPlayUrlList );
    CPPUNIT_TEST( test_CMmtMsgValuesIpcamDialing );
    CPPUNIT_TEST( test_CMmtMsgValuesIpcamDialingAck );
    CPPUNIT_TEST( test_CMsgMmtReqValueCamDialingAck );
    CPPUNIT_TEST( test_CMsgMmtReqValueCamDialing );
    CPPUNIT_TEST( test_CMmtMsgCamDialing );
    CPPUNIT_TEST( test_CMsgPromQlValue );
    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

    CPPUNIT_TEST_SUITE_END();

public:
    void test_CMmtMsgPlayUrlList();
    void test_CMmtMsgValuesIpcamDialing();
    void test_CMmtMsgValuesIpcamDialingAck();
    void test_CMsgMmtReqValueCamDialingAck();
    void test_CMsgMmtReqValueCamDialing();
    void test_CMmtMsgCamDialing();
    void test_CMsgPromQlValue();

    void testSmsStackParse();
    void setUp();
    void tearDown();
};

#endif // _MANTEST_H_

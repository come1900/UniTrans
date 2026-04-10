/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * TestMan.h - _explain_
 *
 * Copyright (C) 2013 GeminiLock-china.com, All Rights Reserved.
 *
 * $Id: TestMan.h 0001 2013-11-02 14:38:29Z wu_junjie $
 *
 *  Explain:
 *     UnitTest -- Intelligent system communication protocol
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
    CPPUNIT_TEST( testSmsStackParse );

    CPPUNIT_TEST( testEncodeCAckMsgSimple );
    CPPUNIT_TEST( testEncodeCAckMsgSession );
    CPPUNIT_TEST( testED_CSmsMsg_RegisterReq);

    CPPUNIT_TEST( testCSmsMsg_requestAuth);
    CPPUNIT_TEST( testCSmsMsg_requestAuthACK);

    CPPUNIT_TEST( testCSmsMsg_requestSend);
    CPPUNIT_TEST( testCSmsMsg_requestSendACK);

    CPPUNIT_TEST_SUITE_END();

public:
    void testSmsStackParse();
    void testEncodeCAckMsgSimple();
    void testEncodeCAckMsgSession();

    void testED_CSmsMsg_RegisterReq();

    void testCSmsMsg_requestAuth();
    void testCSmsMsg_requestAuthACK();

    void testCSmsMsg_requestSend();
    void testCSmsMsg_requestSendACK();

    void setUp();
    void tearDown();
};

#endif // _MANTEST_H_

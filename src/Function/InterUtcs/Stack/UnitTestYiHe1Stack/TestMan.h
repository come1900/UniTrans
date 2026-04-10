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

    CPPUNIT_TEST( testGetSignalControlerParaRequest );//
    CPPUNIT_TEST( testGetSignalControlerParaResponse );

    CPPUNIT_TEST( testGetCrossParaRequest );//
    CPPUNIT_TEST( testGetCrossParaResponse );

    CPPUNIT_TEST( testGetCrossStatusRequest );//
    CPPUNIT_TEST( testGetCrossStatusResponse );

    CPPUNIT_TEST_SUITE_END();

public:

    void testGetSignalControlerParaRequest();
    void testGetSignalControlerParaResponse();

    void testGetCrossParaRequest();
    void testGetCrossParaResponse();

    void testGetCrossStatusRequest();
    void testGetCrossStatusResponse();

    void setUp();
    void tearDown();
};

#endif // _MANTEST_H_


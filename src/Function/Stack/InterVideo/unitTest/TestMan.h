/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * TestMan.h - _explain_
 *
 * Copyright (C) 2013 ezlibs.com, All Rights Reserved.
 *
 * $Id: TestMan.h 0001 2018-10-15 8:54:19Z WuJunjie $
 *
 *  Explain:
 *     UnitTest -- InterVideoStack
 *
 *  Update:
 *     2018-10-15 8:54:19 WuJunjie 549 Create
 *
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#if !defined(_TESTMAN_H_)
#define _TESTMAN_H_

#include <cppunit/extensions/HelperMacros.h>

class CTestMan : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE( CTestMan );

    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
    CPPUNIT_TEST( testEncodeDecode_addDevice );
    CPPUNIT_TEST( testEncodeDecode_addDeviceAck );

    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

    CPPUNIT_TEST_SUITE_END();

public:

    void testEncodeDecode_addDevice();
    void testEncodeDecode_addDeviceAck();

/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
    void setUp();
    void tearDown();
};

#endif // _MANTEST_H_

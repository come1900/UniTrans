/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * TestMan.h - _explain_
 *
 * Copyright (C) 2012 tiansu-china.com, All Rights Reserved.
 *
 * $Id: TestMan.h 0001 2012-5-8 14:38:29Z wu_junjie $
 *
 *  Explain:
 *     TeHomeMsg -- Telecom eHome Msg
 *
 *  Update:
 *     2012-5-8 14:38:33 WuJunjie 549 Create
 *
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#if !defined(_MANTEST_H_)
#define _MANTEST_H_

#include <cppunit/extensions/HelperMacros.h>

class CTestMan : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE( CTestMan );

    CPPUNIT_TEST( testGeminiKeyFullFunc                         );
    CPPUNIT_TEST( testGeminiKeyCreate                         );
    CPPUNIT_TEST( testGeminiKeyLock                         );
    CPPUNIT_TEST( testGeminiKeyUnlock                         );

    CPPUNIT_TEST_SUITE_END();

public:

    void testGeminiKeyFullFunc                        ();
    void testGeminiKeyCreate                 ();
    void testGeminiKeyLock                 ();
    void testGeminiKeyUnlock                 ();

    void setUp()
    {
        ;
    }
    void tearDown()
    {
        ;
    }
};

#endif // _MANTEST_H_

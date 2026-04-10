/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * TestMan.h - _explain_
 *
 * Copyright (C) 2011 jsict.com, All Rights Reserved.
 *
 * $Id: TestMan.h 5884 2019-02-27 05:28:44Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2019-02-27 05:28:44  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/


#if !defined(_MANTEST_H_)
#define _MANTEST_H_

#include <cppunit/extensions/HelperMacros.h>

class CTestMan : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE( CTestMan );

    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
    CPPUNIT_TEST( testEncodeDecode_CKmsMsg_requestMsgGenerateKeypair );
    CPPUNIT_TEST( testEncodeDecode_CKmsMsg_requestMsgGenerateKeypairAck );

    CPPUNIT_TEST( testEncodeDecode_CKmsMsg_requestMsgEncryptAsymmetric );
    CPPUNIT_TEST( testEncodeDecode_CKmsMsg_requestMsgEncryptAsymmetricAck );

    CPPUNIT_TEST( testEncodeDecode_CKmsMsg_requestMsgDecryptAsymmetric );
    CPPUNIT_TEST( testEncodeDecode_CKmsMsg_requestMsgDecryptAsymmetricAck );

    CPPUNIT_TEST( testEncodeDecode_CKmsMsg_requestMsgGenerateKey );
    CPPUNIT_TEST( testEncodeDecode_CKmsMsg_requestMsgGenerateKeyAck );
    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

    CPPUNIT_TEST_SUITE_END();

public:

    // ·Ç¶Ô³Æ
    void testEncodeDecode_CKmsMsg_requestMsgGenerateKeypair();
    void testEncodeDecode_CKmsMsg_requestMsgGenerateKeypairAck();

    void testEncodeDecode_CKmsMsg_requestMsgEncryptAsymmetric();
    void testEncodeDecode_CKmsMsg_requestMsgEncryptAsymmetricAck();

    void testEncodeDecode_CKmsMsg_requestMsgDecryptAsymmetric();
    void testEncodeDecode_CKmsMsg_requestMsgDecryptAsymmetricAck();

    // ¶Ô³Æ
    void testEncodeDecode_CKmsMsg_requestMsgGenerateKey();
    void testEncodeDecode_CKmsMsg_requestMsgGenerateKeyAck();

/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
    void setUp();
    void tearDown();
};

#endif // _MANTEST_H_

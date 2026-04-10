/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * TestMan.cpp - _explain_
 *
 * Copyright (C) 2011 jsict.com, All Rights Reserved.
 *
 * $Id: TestMan.cpp 5884 2019-02-27 05:28:40Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2019-02-27 05:28:40  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/


#include "TestMan.h"

#include "../KmsStack.h"

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

void CTestMan::testEncodeDecode_CKmsMsg_requestMsgGenerateKeypair()
{
    CKmsStack Stack;

    CKmsMsg_requestMsgGenerateKeypair Msg;

    Msg.m_strKmsID = "KmsID_C056E37537D2";

    Stack.Encode(Msg);

    std::cout << std::endl << "-->> CKmsMsg_requestMsgGenerateKeypair" << " Encode:" << std::endl;
    std::cout << std::endl <<  Stack.ToString() << std::endl << "<<--" << std::endl;

    CKmsStack __Stack;
    CKmsMsg_requestMsgGenerateKeypair __Msg;

    __Stack.Parse(Stack.ToString());
    __Stack.Decode(__Msg);

    CPPUNIT_ASSERT(Msg.m_strKmsID == __Msg.m_strKmsID );

    //__Msg.dump();
}

void CTestMan::testEncodeDecode_CKmsMsg_requestMsgGenerateKeypairAck()
{
    CKmsStack Stack;

    CKmsMsg_requestMsgGenerateKeypairAck Msg;

    Msg.m_strKmsID = "KmsID_C056E37537D2";

    Stack.Encode(Msg);

    std::cout << std::endl << "-->> CKmsMsg_requestMsgGenerateKeypairAck" << " Encode:" << std::endl;
    std::cout << std::endl <<  Stack.ToString() << std::endl << "<<--" << std::endl;

    CKmsStack __Stack;
    CKmsMsg_requestMsgGenerateKeypairAck __Msg;

    __Stack.Parse(Stack.ToString());
    __Stack.Decode(__Msg);

    CPPUNIT_ASSERT(Msg.m_strKmsID == __Msg.m_strKmsID );

    //__Msg.dump();
}

void CTestMan::testEncodeDecode_CKmsMsg_requestMsgGenerateKey()
{
    CKmsStack Stack;

    CKmsMsg_requestMsgGenerateKey Msg;

    Msg.m_strKmsID = "KmsID_C056E37537D2";

    Stack.Encode(Msg);

    std::cout << std::endl << "-->> CKmsMsg_requestMsgGenerateKey" << " Encode:" << std::endl;
    std::cout << std::endl <<  Stack.ToString() << std::endl << "<<--" << std::endl;

    CKmsStack __Stack;
    CKmsMsg_requestMsgGenerateKey __Msg;

    __Stack.Parse(Stack.ToString());
    __Stack.Decode(__Msg);

    CPPUNIT_ASSERT(Msg.m_strKmsID == __Msg.m_strKmsID );

    //__Msg.dump();
}

void CTestMan::testEncodeDecode_CKmsMsg_requestMsgGenerateKeyAck()
{
    CKmsStack Stack;

    CKmsMsg_requestMsgGenerateKeyAck Msg;

    Msg.m_strKmsID = "KmsID_C056E37537D2";

    Stack.Encode(Msg);

    std::cout << std::endl << "-->> CKmsMsg_requestMsgGenerateKeyAck" << " Encode:" << std::endl;
    std::cout << std::endl <<  Stack.ToString() << std::endl << "<<--" << std::endl;

    CKmsStack __Stack;
    CKmsMsg_requestMsgGenerateKeyAck __Msg;

    __Stack.Parse(Stack.ToString());
    __Stack.Decode(__Msg);

    CPPUNIT_ASSERT(Msg.m_strKmsID == __Msg.m_strKmsID );

    //__Msg.dump();
}

void CTestMan::testEncodeDecode_CKmsMsg_requestMsgEncryptAsymmetric()
{
    CKmsStack Stack;

    CKmsMsg_requestMsgEncryptAsymmetric Msg;

    Msg.m_strKmsID = "KmsID_C056E37537D2";
    Msg.m_strPublicKey = "m_strPublicKey";
    Msg.m_strPlainText = "m_strPlainText";

    Stack.Encode(Msg);

    std::cout << std::endl << "-->> CKmsMsg_requestMsgEncryptAsymmetric" << " Encode:" << std::endl;
    std::cout << std::endl <<  Stack.ToString() << std::endl << "<<--" << std::endl;

    CKmsStack __Stack;
    CKmsMsg_requestMsgEncryptAsymmetric __Msg;

    __Stack.Parse(Stack.ToString());
    __Stack.Decode(__Msg);

    CPPUNIT_ASSERT(Msg.m_strKmsID == __Msg.m_strKmsID );

    //__Msg.dump();
}

void CTestMan::testEncodeDecode_CKmsMsg_requestMsgEncryptAsymmetricAck()
{
    CKmsStack Stack;

    CKmsMsg_requestMsgEncryptAsymmetricAck Msg;

    Msg.m_strKmsID = "KmsID_C056E37537D2";
    Msg.m_strCipherText = "m_strCipherText";

    Stack.Encode(Msg);

    std::cout << std::endl << "-->> CKmsMsg_requestMsgEncryptAsymmetricAck" << " Encode:" << std::endl;
    std::cout << std::endl <<  Stack.ToString() << std::endl << "<<--" << std::endl;

    CKmsStack __Stack;
    CKmsMsg_requestMsgEncryptAsymmetricAck __Msg;

    __Stack.Parse(Stack.ToString());
    __Stack.Decode(__Msg);

    CPPUNIT_ASSERT(Msg.m_strKmsID == __Msg.m_strKmsID );

    //__Msg.dump();
}

void CTestMan::testEncodeDecode_CKmsMsg_requestMsgDecryptAsymmetric()
{
    CKmsStack Stack;

    CKmsMsg_requestMsgDecryptAsymmetric Msg;

    Msg.m_strKmsID = "KmsID_C056E37537D2";
    Msg.m_strPrivateKey = "m_strPrivateKey";
    Msg.m_strCipherText = "m_strCipherText";

    Stack.Encode(Msg);

    std::cout << std::endl << "-->> CKmsMsg_requestMsgDecryptAsymmetric" << " Encode:" << std::endl;
    std::cout << std::endl <<  Stack.ToString() << std::endl << "<<--" << std::endl;

    CKmsStack __Stack;
    CKmsMsg_requestMsgDecryptAsymmetric __Msg;

    __Stack.Parse(Stack.ToString());
    __Stack.Decode(__Msg);

    CPPUNIT_ASSERT(Msg.m_strKmsID == __Msg.m_strKmsID );

    //__Msg.dump();
}

void CTestMan::testEncodeDecode_CKmsMsg_requestMsgDecryptAsymmetricAck()
{
    CKmsStack Stack;

    CKmsMsg_requestMsgDecryptAsymmetricAck Msg;

    Msg.m_strKmsID = "KmsID_C056E37537D2";
    Msg.m_strPlainText = "m_strPlainText";

    Stack.Encode(Msg);

    std::cout << std::endl << "-->> CKmsMsg_requestMsgDecryptAsymmetricAck" << " Encode:" << std::endl;
    std::cout << std::endl <<  Stack.ToString() << std::endl << "<<--" << std::endl;

    CKmsStack __Stack;
    CKmsMsg_requestMsgDecryptAsymmetricAck __Msg;

    __Stack.Parse(Stack.ToString());
    __Stack.Decode(__Msg);

    CPPUNIT_ASSERT(Msg.m_strKmsID == __Msg.m_strKmsID );

    //__Msg.dump();
}

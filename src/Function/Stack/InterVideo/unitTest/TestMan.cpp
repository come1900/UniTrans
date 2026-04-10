/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * TestMan.cpp - _explain_
 *
 * Copyright (C) 2013 ezlibs.com, All Rights Reserved.
 *
 * $Id: TestMan.cpp 0001 2018-10-15 8:54:19Z WuJunjie $
 *
 *  Explain:
 *     UnitTest -- InterVideoStack
 *
 *  Update:
 *     2018-10-15 8:54:19 WuJunjie 549 Create
 *
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#include "TestMan.h"

#include "../InterVideoStack.h"

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

void CTestMan::testEncodeDecode_addDevice()
{
    CInterVideoStack Stack;

    CInterVideoMsg_addDevice Msg;

    Msg.m_strDevID = "G1751453539";

    Stack.Encode(Msg);

    std::cout << std::endl << "-->> CInterVideoMsg_addDevice" << " Encode:" << std::endl;
    std::cout << std::endl <<  Stack.ToString() << std::endl << "<<--" << std::endl;

    CInterVideoStack __Stack;
    CInterVideoMsg_addDevice __Msg;

    __Stack.Parse(Stack.ToString());
    __Stack.Decode(__Msg);

    CPPUNIT_ASSERT(Msg.m_strDevID == __Msg.m_strDevID );

    //__Msg.dump();
}

void CTestMan::testEncodeDecode_addDeviceAck()
{
    CInterVideoStack Stack;

    CInterVideoMsg_addDeviceAck Msg;

    Msg.m_strDevID = "G1751453539";

    Stack.Encode(Msg);

    std::cout << std::endl << "-->> CInterVideoMsg_addDeviceAck" << " Encode:" << std::endl;
    std::cout << std::endl <<  Stack.ToString() << std::endl << "<<--" << std::endl;

    CInterVideoStack __Stack;
    CInterVideoMsg_addDeviceAck __Msg;

    __Stack.Parse(Stack.ToString());
    __Stack.Decode(__Msg);

    CPPUNIT_ASSERT(Msg.m_strDevID == __Msg.m_strDevID );

    //__Msg.dump();
}


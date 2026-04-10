/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * TestMan.cpp - _explain_
 *
 * Copyright (C) 2013 GeminiLock-china.com, All Rights Reserved.
 *
 * $Id: TestMan.cpp 0001 2013-11-02 14:38:29Z wu_junjie $
 *
 *  Explain:
 *     UnitTest -- Intelligent system communication protocol
 *
 *  Update:
 *     2013-11-03 14:38:33 WuJunjie 549 Create
 *
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#include "TestMan.h"

#include "../SmsStack.h"

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

void CTestMan::testSmsStackParse()
{
    CSmsStack __CSmsStack;

    //std::cout<< endl << "-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-" << std::endl;
    {
        std::string strValue = "{\"name\":\"json\",\"age\":\"123\",\"array\":[{\"cpp\":\"jsoncpp\"},{\"java\":\"jsoninjava\"},{\"php\":\"support\"}]}";
        std::string document;

        std::string javaPasedValue;

        __CSmsStack.Parse(strValue);
        __CSmsStack.ToString(document);
        //std::cout << document << std::endl;


        // std::cout << "__CSmsStack.GetValue()[\"name\"].asString(): " << __CSmsStack.GetValue()["name"].asString() << std::endl;
        // std::cout << "__CSmsStack.GetValue()[\"age\"].asString(): " << __CSmsStack.GetValue()["age"].asString() << std::endl;
        CPPUNIT_ASSERT( __CSmsStack.GetValue()["name"].asString() == "json" );
        CPPUNIT_ASSERT( __CSmsStack.GetValue()["age"].asString() == "123" );

        // find the value in array
        const Json::Value arrayObj = __CSmsStack.GetValue()["array"];
        for (unsigned int i = 0; i < arrayObj.size(); i++)
        {
            if (!arrayObj[i].isMember("java"))
            {
                continue;
            }

            //std::cout << "__CSmsStack.GetValue()[\"array\"][\"cpp\"]: " << arrayObj[i]["java"].asString() << std::endl;
            javaPasedValue = arrayObj[i]["java"].asString();
            CPPUNIT_ASSERT( arrayObj[i]["java"].asString() == "jsoninjava" );

        }
    }
}

void CTestMan::testEncodeCAckMsgSimple()
{
    CSmsStack __CSmsStack;

    //    std::cout<< endl << "-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-" << std::endl;
    //    {
    //        __CSmsStack.Encode();
    //        std::cout << __CSmsStack.ToString() << std::endl;
    //    }

    std::string result = "100";

    //std::cout<< endl << "-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-" << std::endl;
    {
        CAckMsgSimple Msg;
        Msg.Result = result;
        Msg.Reason = "no such msg";
        //std::string strEncoded = "{\n   \"reason\" : \"no such msg\",\n   \"result\" : \"100\"\n}\n";
        std::string strEncoded = "{ \"Result\" : \"100\", \"ResultMsg\" : \"no such msg\" }\n";

        __CSmsStack.Encode(Msg);
        //__fline;
        //        std::cout << "[" <<  __CSmsStack.ToString() << "]" << std::endl;
        //        std::cout << __CSmsStack.ToString().size() << std::endl;
        //        std::cout << strEncoded.size() << std::endl;
        //        std::cout << strEncoded << std::endl;

        CPPUNIT_ASSERT(__CSmsStack.ToString() == strEncoded );
    }
    // decode after encode
    {
        CAckMsgSimple Msg;

        __CSmsStack.Decode(Msg);

        //        cout << endl;
        //        Msg.dump();
        //        cout << endl;

        CPPUNIT_ASSERT(Msg.Result == result );
    }

}
void CTestMan::testEncodeCAckMsgSession()
{
    CSmsStack __CSmsStack;

    //std::cout<< endl << "-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-" << std::endl;
    {
        CAckMsgSession Msg;
        Msg.Result = "100";
        Msg.Reason = "no such msg";
        Msg.SessionId = "76D663CC00754954434A4FBF37C4CC4E";
        //std::string strEncoded = "{\n   \"SessionId\" : \"76D663CC00754954434A4FBF37C4CC4E\",\n   \"reason\" : \"no such msg\",\n   \"result\" : \"100\"\n}\n";
        std::string strEncoded = "{ \"Result\" : \"100\", \"ResultMsg\" : \"no such msg\", \"SessionId\" : \"76D663CC00754954434A4FBF37C4CC4E\" }\n";
        __CSmsStack.Encode(Msg);

        //        std::cout << __CSmsStack.ToString() << std::endl;
        //        std::cout << __CSmsStack.ToString().size() << std::endl;
        //        std::cout << strEncoded.size() << std::endl;
        //        std::cout << strEncoded << std::endl;
        CPPUNIT_ASSERT(__CSmsStack.ToString() == strEncoded );
    }

    // decode after encode
    {
        CAckMsgSession Msg;

        __CSmsStack.Decode(Msg);

        //        cout << endl;
        //        Msg.dump();
        //        cout << endl;

        CPPUNIT_ASSERT(Msg.Result == "100" );
    }
}

void CTestMan::testED_CSmsMsg_RegisterReq							   ()
{
    CSmsStack __CSmsStack;

    //std::cout << endl << "-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-" << std::endl;
    CSmsMsg_RegisterReq Msg;
    Msg.ProductID		= "VProductID";
    Msg.AuthCode 	= "VAuthCode";
    Msg.AuthName		= "VAuthName";
    Msg.Password	= "VPassword";

    Msg.m_LocalStatus.LocalIp = "VLocalIp";
    Msg.m_LocalStatus.LocalDateTime = "VLocalDateTime";

    //std::string strEncoded = "{\n   \"AuthCode\" : \"VAuthCode\",\n   \"AuthName\" : \"VAuthName\",\n   \"Password\" : \"VPassword\",\n   \"ProductID\" : \"VProductID\"\n}\n";
    std::string strEncoded = "{ \"AuthCode\" : \"VAuthCode\", \"AuthName\" : \"VAuthName\", \"LocalDateTime\" : \"VLocalDateTime\", \"LocalIp\" : \"VLocalIp\", \"Password\" : \"VPassword\", \"ProductID\" : \"VProductID\" }\n";

    __CSmsStack.Encode(Msg);

    //std::cout << __CSmsStack.ToString() << std::endl;
    //std::cout << __CSmsStack.ToString().size() << std::endl;
    //std::cout << strEncoded.size() << std::endl;
    //std::cout << strEncoded << std::endl;

    CPPUNIT_ASSERT(__CSmsStack.ToString() == strEncoded );
    /*
    {
       "AuthCode" : "VAuthCode",
       "AuthName" : "VAuthName",
       "LocalDateTime" : "VLocalDateTime",
       "LocalIp" : "VLocalIp",
       "Password" : "VPassword",
       "ProductID" : "VProductID"
    }
    */
    // decode after encode
    CSmsMsg_RegisterReq Msg2;
    __CSmsStack.Decode(Msg2);
    CPPUNIT_ASSERT(Msg.ProductID == "VProductID" );

    //Msg2.dump();

    /////////////////////////////////////////////////////////
    CSmsStack __CSmsStack_197;
    CSmsMsg_RegisterAck Msg_RegisterAck;
    CSmsMsg_ServiceAgent __service;

    //?y3?ê
    __service.ServiceName        = "PubServiceGetPubIP4";
    __service.ServiceLocation  = "http://pubservice.ezlibs.com:60089/PubService/getPubIP4";
    //Msg_RegisterAck.m_Service.push_back(__service);
    Msg_RegisterAck.m_Service.push_back(__service);
    Msg_RegisterAck.m_ackInfo.Result = "0";
    Msg_RegisterAck.m_ackInfo.Reason = "ok";
    Msg_RegisterAck.m_Session = "00000000000000000000";
    __CSmsStack_197.Encode(Msg_RegisterAck);
    //std::cout << std::endl;
    //std::cout << __CSmsStack_197.ToString() << std::endl;

    ///////////////////////////////////////////////////////////

    CSmsStack __CSmsStackAck;

    std::string strAck = "[{\"Service\":[{\"ServiceLocation\":\"http://pubservice.ezlibs.com:60089/PubService/getPubIP4\",\"ServiceName\":\"PubServiceGetPubIP4\"},{\"ServiceLocation\":\"http://pubservice.ezlibs.com:60089/PubService/getPubIP4\",\"ServiceName\":\"PubServiceGetPubIP4\"}],\"reason\":\"ok\",\"result\":\"0\",\"session\":\"00000000000000000000\"}]";
    //__CSmsStackAck.Parse(__CSmsStack_197.ToString());
    __CSmsStackAck.Parse(strAck);

    //    std::cout << std::endl << "__CSmsStackAck.Parse" <<std::endl;
    //    std::cout << __CSmsStackAck.ToString() << std::endl;

    CSmsMsg_RegisterAck __ackMsg;
    // strAck 前后多了[]，数据非法， so failed
    CPPUNIT_ASSERT(__CSmsStackAck.Decode(__ackMsg) == false );

    __CSmsStackAck.Parse(__CSmsStack_197.ToString());
    CPPUNIT_ASSERT(__CSmsStackAck.Decode(__ackMsg) == true );
    CPPUNIT_ASSERT(__ackMsg.m_Session == Msg_RegisterAck.m_Session );

    //    if (__CSmsStackAck.Decode(__ackMsg))
    //    {
    //        std::cout << std::endl << "__ackMsg.dump();" << std::endl << "-----------------------" << std::endl;
    //        __ackMsg.dump();
    //        std::cout << std::endl << "-----------------------" << std::endl;
    //    }
    //    else
    //    {
    //        std::cout << std::endl << "__CSmsStackAck.Decode(__ackMsg) FAILED!!" << std::endl;
    //    }

}

void CTestMan::testCSmsMsg_requestAuth()
{
    CSmsStack __CSmsStack;

    CSmsMsg_requestAuth Msg;

    Msg.m_strAccount  	    = "zhangsan";
    Msg.m_strPassword 		= "123456";
    Msg.m_strSessionID	    = "6a614744f04c5a6088eb5c9343e0bc24bbfc7001";

    Msg.m_LocalSystemInfo.m_strHostID    = "ezlibsSmartServer";
    Msg.m_LocalSystemInfo.m_strSerial      = "20112211000000000155";
    Msg.m_LocalSystemInfo.m_strDescription = "GeminiLock service agent";
    Msg.m_LocalSystemInfo.m_strType        = "ss_GeminiLock";
    Msg.m_LocalSystemInfo.m_strLocalDate   = "2013-11-08 16:55:53 +0800";


    CMsgService __Service;

    __Service.m_strServiceID   = "2011221100000000015501";
    __Service.m_strDescription = "GeminiLock service agent";
    __Service.m_strURI         = "ss1.faceaip.com";
    //__Service.m_strPort        = "12345";
    __Service.m_strProtocol    = "ss_GeminiLock_http";
    Msg.m_vLocalService.push_back(__Service);

    __Service.m_strServiceID   = "2011221100000000015502";
    __Service.m_strDescription = "GeminiLock service agent";
    __Service.m_strURI         = "ss2.faceaip.com";
    //__Service.m_strPort        = "12346";
    __Service.m_strProtocol    = "ss_GeminiLock_http";
    Msg.m_vLocalService.push_back(__Service);

    std::string strEncoded = "{ \"Account\" : \"zhangsan\", \"Password\" : \"123456\", \"Service\" : [ { \"Description\" : \"GeminiLock service agent\", \"Protocol\" : \"ss_GeminiLock_http\", \"ServiceID\" : \"2011221100000000015501\", \"URI\" : \"ss1.faceaip.com\" }, { \"Description\" : \"GeminiLock service agent\", \"Protocol\" : \"ss_GeminiLock_http\", \"ServiceID\" : \"2011221100000000015502\", \"URI\" : \"ss2.faceaip.com\" } ], \"SessionID\" : \"6a614744f04c5a6088eb5c9343e0bc24bbfc7001\", \"System\" : { \"Description\" : \"GeminiLock service agent\", \"HostName\" : \"ezlibsSmartServer\", \"LocalDate\" : \"2013-11-08 16:55:53 +0800\", \"Serial\" : \"20112211000000000155\", \"Type\" : \"ss_GeminiLock\" } }\n";

    __CSmsStack.Encode(Msg);

    std::cout << std::endl << "-->> CSmsMsg_requestAuth" << " Encode:" << std::endl;
    std::cout << std::endl <<  __CSmsStack.ToString() << std::endl << "<<--" << std::endl;

    //std::cout << endl << "-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-" << std::endl;
    //std::cout << __CSmsStack.ToString() << std::endl;
    //Msg.dump();

    CPPUNIT_ASSERT(__CSmsStack.ToString() == strEncoded );

    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

    CSmsStack __CSmsStackParse;
    __CSmsStackParse.Parse(strEncoded);
    CSmsMsg_requestAuth MsgAuth;

    if (__CSmsStackParse.Decode(MsgAuth))
    {
        //MsgAuth.dump();
    }

    CPPUNIT_ASSERT(Msg.m_strAccount == Msg.m_strAccount );

}

void CTestMan::testCSmsMsg_requestAuthACK()
{
    CSmsStack __CSmsStack;

    CSmsMsg_requestAuthACK Msg;

    Msg.Result         = "0";
    Msg.Reason      = "ok";
    Msg.m_strSessionID      = "6f61dfe3-1ec9-4123-826e-37862efd901d";
    Msg.m_strSessionTimeout = "120";

    Msg.m_LocalSystemInfo.m_strHostID    = "ezlibsSmartServer";
    Msg.m_LocalSystemInfo.m_strSerial      = "20112211000000000155";
    Msg.m_LocalSystemInfo.m_strDescription = "GeminiLock service agent";
    Msg.m_LocalSystemInfo.m_strType        = "ss_GeminiLock";
    Msg.m_LocalSystemInfo.m_strLocalDate   = "2013-11-08 16:55:53 +0800";

    CMsgService __Service;

    __Service.m_strServiceID   = "2011221100000000015501";
    __Service.m_strDescription = "GeminiLock service agent";
    __Service.m_strURI         = "ss1.faceaip.com";
    //__Service.m_strPort        = "12345";
    __Service.m_strProtocol    = "ss_GeminiLock_http";
    Msg.m_vLocalService.push_back(__Service);

    __Service.m_strServiceID   = "2011221100000000015502";
    __Service.m_strDescription = "GeminiLock service agent";
    __Service.m_strURI         = "ss2.faceaip.com";
    //__Service.m_strPort        = "12346";
    __Service.m_strProtocol    = "ss_GeminiLock_http";
    Msg.m_vLocalService.push_back(__Service);

    std::string strEncoded = "{ \"Result\" : \"0\", \"ResultMsg\" : \"ok\", \"Service\" : [ { \"Description\" : \"GeminiLock service agent\", \"Protocol\" : \"ss_GeminiLock_http\", \"ServiceID\" : \"2011221100000000015501\", \"URI\" : \"ss1.faceaip.com\" }, { \"Description\" : \"GeminiLock service agent\", \"Protocol\" : \"ss_GeminiLock_http\", \"ServiceID\" : \"2011221100000000015502\", \"URI\" : \"ss2.faceaip.com\" } ], \"SessionID\" : \"6f61dfe3-1ec9-4123-826e-37862efd901d\", \"SessionTimeout\" : \"120\", \"System\" : { \"Description\" : \"GeminiLock service agent\", \"HostName\" : \"ezlibsSmartServer\", \"LocalDate\" : \"2013-11-08 16:55:53 +0800\", \"Serial\" : \"20112211000000000155\", \"Type\" : \"ss_GeminiLock\" } }\n";

    __CSmsStack.Encode(Msg);

    std::cout << std::endl << "-->> CSmsMsg_requestAuthACK" << " Encode:" << std::endl;
    std::cout << std::endl <<  __CSmsStack.ToString() << std::endl << "<<--" << std::endl;
    //std::cout << endl << "-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-" << std::endl;
    //std::cout << __CSmsStack.ToString() << std::endl;
    //Msg.dump();

    CPPUNIT_ASSERT(__CSmsStack.ToString() == strEncoded );

    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

    CSmsStack __CSmsStackParse;
    __CSmsStackParse.Parse(strEncoded);
    CSmsMsg_requestAuthACK MsgAuth;

    if (__CSmsStackParse.Decode(MsgAuth))
    {
        //MsgAuth.dump();
    }

    CPPUNIT_ASSERT(Msg.m_strSessionID == MsgAuth.m_strSessionID );
}

void CTestMan::testCSmsMsg_requestSend()
{
    CSmsMsg_requestSend Msg;

    Msg.m_strSessionID	    = "null";

    Msg.m_LocalSystemInfo.m_strHostID    = "ezlibsSmartServer";
    Msg.m_LocalSystemInfo.m_strSerial      = "20112211000000000155";
    Msg.m_LocalSystemInfo.m_strDescription = "GeminiLock service agent";
    Msg.m_LocalSystemInfo.m_strType        = "ss_GeminiLock";
    Msg.m_LocalSystemInfo.m_strLocalDate   = "2013-11-08 16:55:53 +0800";

    Msg.m_From.m_strServiceID   = "2011221100000000015501";
    Msg.m_From.m_strDescription = "GeminiLock service agent";
    Msg.m_From.m_strURI         = "ss1.faceaip.com";
    //Msg.m_From.m_strPort        = "12345";
    Msg.m_From.m_strProtocol    = "ss_GeminiLock_http";

    Msg.m_Dest.m_strServiceID   = "2011221100000000015501";
    Msg.m_Dest.m_strDescription = "GeminiLock service agent";
    Msg.m_Dest.m_strURI         = "ss1.faceaip.com";
    //Msg.m_Dest.m_strPort        = "12345";
    Msg.m_Dest.m_strProtocol    = "ss_GeminiLock_http";

    Msg.m_Data.m_strType     = "string";
    Msg.m_Data.m_strEncode   = "string";
    Msg.m_Data.m_strLen      = "0";
    Msg.m_Data.m_strValue    = "hello json";

    CSmsStack __CSmsStack;
    __CSmsStack.Encode(Msg);

    std::string strEncoded = "{ \"Dest\" : { \"Description\" : \"GeminiLock service agent\", \"Protocol\" : \"ss_GeminiLock_http\", \"ServiceID\" : \"2011221100000000015501\", \"URI\" : \"ss1.faceaip.com\" }, \"From\" : { \"Description\" : \"GeminiLock service agent\", \"Protocol\" : \"ss_GeminiLock_http\", \"ServiceID\" : \"2011221100000000015501\", \"URI\" : \"ss1.faceaip.com\" }, \"Msg\" : { \"Encode\" : \"string\", \"Len\" : \"0\", \"Type\" : \"string\", \"Value\" : \"hello json\" }, \"SessionID\" : \"null\", \"System\" : { \"Description\" : \"GeminiLock service agent\", \"HostName\" : \"ezlibsSmartServer\", \"LocalDate\" : \"2013-11-08 16:55:53 +0800\", \"Serial\" : \"20112211000000000155\", \"Type\" : \"ss_GeminiLock\" } }\n";
    CPPUNIT_ASSERT(__CSmsStack.ToString() == strEncoded );

    std::cout << std::endl << "-->> CSmsMsg_requestSend" << " Encode:" << std::endl;
    std::cout << std::endl <<  __CSmsStack.ToString() << std::endl << "<<--" << std::endl;
    //    std::cout << endl << "-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-" << std::endl;
    //    std::cout << __CSmsStack.ToString() << std::endl;
    //    std::cout << endl << "-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-" << std::endl;
    //    std::cout << strEncoded << std::endl;

    CSmsStack __CSmsStackParse;
    __CSmsStackParse.Parse(strEncoded);
    CSmsMsg_requestSend MsgSD;

    if (__CSmsStackParse.Decode(MsgSD))
    {
        //MsgSD.dump();
    }

    CPPUNIT_ASSERT(Msg.m_strSessionID == MsgSD.m_strSessionID );

}

void CTestMan::testCSmsMsg_requestSendACK()
{
    CSmsMsg_requestSendACK Msg;

    Msg.m_strSessionID	    = "6f61dfe3-1ec9-4123-826e-37862efd901d";
    Msg.Result	    = "0";
    Msg.Reason	    = "ok";

    Msg.m_LocalSystemInfo.m_strHostID    = "ezlibsSmartServer";
    Msg.m_LocalSystemInfo.m_strSerial      = "20112211000000000155";
    Msg.m_LocalSystemInfo.m_strDescription = "GeminiLock service agent";
    Msg.m_LocalSystemInfo.m_strType        = "ss_GeminiLock";
    Msg.m_LocalSystemInfo.m_strLocalDate   = "2013-11-08 16:55:53 +0800";

    Msg.m_From.m_strServiceID   = "2011221100000000015501";
    Msg.m_From.m_strDescription = "GeminiLock service agent";
    Msg.m_From.m_strURI         = "ss1.faceaip.com";
    //Msg.m_From.m_strPort        = "12345";
    Msg.m_From.m_strProtocol    = "ss_GeminiLock_http";

    Msg.m_Dest.m_strServiceID   = "2011221100000000015501";
    Msg.m_Dest.m_strDescription = "GeminiLock service agent";
    Msg.m_Dest.m_strURI         = "ss1.faceaip.com";
    //Msg.m_Dest.m_strPort        = "12345";
    Msg.m_Dest.m_strProtocol    = "ss_GeminiLock_http";

    CSmsStack __CSmsStack;
    __CSmsStack.Encode(Msg);

    std::string strEncoded = "{ \"Dest\" : { \"Description\" : \"GeminiLock service agent\", \"Protocol\" : \"ss_GeminiLock_http\", \"ServiceID\" : \"2011221100000000015501\", \"URI\" : \"ss1.faceaip.com\" }, \"From\" : { \"Description\" : \"GeminiLock service agent\", \"Protocol\" : \"ss_GeminiLock_http\", \"ServiceID\" : \"2011221100000000015501\", \"URI\" : \"ss1.faceaip.com\" }, \"Result\" : \"0\", \"ResultMsg\" : \"ok\", \"SessionID\" : \"6f61dfe3-1ec9-4123-826e-37862efd901d\", \"System\" : { \"Description\" : \"GeminiLock service agent\", \"HostName\" : \"ezlibsSmartServer\", \"LocalDate\" : \"2013-11-08 16:55:53 +0800\", \"Serial\" : \"20112211000000000155\", \"Type\" : \"ss_GeminiLock\" } }\n";
    CPPUNIT_ASSERT(__CSmsStack.ToString() == strEncoded );


    std::cout << std::endl << "-->> CSmsMsg_requestSendACK" << " Encode:" << std::endl;
    std::cout << std::endl <<  __CSmsStack.ToString() << std::endl << "<<--" << std::endl;
    //std::cout << endl << "-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-" << std::endl;
    //std::cout << __CSmsStack.ToString() << std::endl;
    //    std::cout << endl << "-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-" << std::endl;
    //    std::cout << strEncoded << std::endl;

    //strEncoded = __CSmsStack.ToString();

    CSmsStack __CSmsStackParse;
    __CSmsStackParse.Parse(strEncoded);
    CSmsMsg_requestSendACK MsgSD;

    if (__CSmsStackParse.Decode(MsgSD))
    {
        //MsgSD.dump();
    }

    CPPUNIT_ASSERT(Msg.m_strSessionID == MsgSD.m_strSessionID );
}

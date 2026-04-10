/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * TestMan.cpp - _explain_
 *
 * Copyright (C) 2013 ezlibs.com, All Rights Reserved.
 *
 * $Id: TestMan.cpp 0001 2013-11-02 14:38:29Z WuJunjie $
 *
 *  Explain:
 *     UnitTest -- PqStack
 *
 *  Update:
 *     2013-11-03 14:38:33 WuJunjie 549 Create
 *
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#include "TestMan.h"

#include "../PqStack.h"

// #define _DEBUG_THIS
#include <ez_def_devel_debug.h>

CPPUNIT_TEST_SUITE_REGISTRATION( CTestMan );

void CTestMan::setUp()
{}
void CTestMan::tearDown()
{}

void CTestMan::testEncodeDecode_requestMsg()
{
    CPqStack Stack;

    CPqMsg_requestMsg Msg;

    Msg.m_strSession = "6f61dfe3";
    Msg.m_strDevID = "G1751453539";
    Msg.m_strMsgBody = "MsgBody";

    Stack.Encode(Msg);

    std::cout << std::endl << "-->> CPqMsg_requestMsg" << " Encode:" << std::endl;
    DBG(std::cout << std::endl <<  Stack.ToString() << std::endl << "<<--" << std::endl;);

    CPqStack __Stack;
    CPqMsg_requestMsg __Msg;

    __Stack.Parse(Stack.ToString());
    __Stack.Decode(__Msg);

    CPPUNIT_ASSERT(Msg.m_strSession == __Msg.m_strSession );

    //__Msg.dump();
}

void CTestMan::testEncodeDecode_requestMsgAck()
{
    CPqStack Stack;

    CPqMsg_requestMsgAck Msg;

    Msg.m_strSession = "6f61dfe3";
    Msg.m_strDevID = "G1751453539";
    Msg.m_strMsgBody = "MsgBody";

    Stack.Encode(Msg);

    std::cout << std::endl << "-->> CPqMsg_requestMsgAck" << " Encode:" << std::endl;
    DBG(std::cout << std::endl <<  Stack.ToString() << std::endl << "<<--" << std::endl;);

    CPqStack __Stack;
    CPqMsg_requestMsgAck __Msg;

    __Stack.Parse(Stack.ToString());
    __Stack.Decode(__Msg);

    CPPUNIT_ASSERT(Msg.m_strSession == __Msg.m_strSession );

    //__Msg.dump();
}

void CTestMan::testEncodeDecode_requestMsgPub_IO_Out()
{
    CPqStack Stack;

    CPqMsg_requestMsgPub_IO_Out Msg;

    Msg.m_strDevID = "G1751453539";
    Msg.m_strChannel = "1";
    Msg.m_strMethod = "0";

    Stack.Encode(Msg);

    std::cout << std::endl << "-->> CPqMsg_requestMsgPub_IO_Out" << " Encode:" << std::endl;
    DBG(std::cout << std::endl <<  Stack.ToString() << std::endl << "<<--" << std::endl;);

    CPqStack __Stack;
    CPqMsg_requestMsgPub_IO_Out __Msg;

    __Stack.Parse(Stack.ToString());
    __Stack.Decode(__Msg);

    CPPUNIT_ASSERT(Msg.m_strChannel == __Msg.m_strChannel );

    //__Msg.dump();
}

void CTestMan::testEncodeDecode_requestMsgShareHID()
{
    CPqStack Stack;

    CPqMsg_requestMsgShareHID Msg;

    Msg.m_strDevID = "G1189185272";
    Msg.m_strIFNo  = "1";
    Msg.m_strCmdType = "k";
    Msg.m_strUsualKey = "abc";
    Msg.m_strControlKey = "";

    Msg.m_strMouseKey		 = "0";
    Msg.m_strMouseXMove		 = "0";
    Msg.m_strMouseYMove		 = "0";
    Msg.m_strMouseRollerMove = "0";

    Stack.Encode(Msg);
    
    std::cout << std::endl
              << "-->> CPqMsg_requestMsgShareHID"
              << " Encode:" << std::endl;
    DBG(
    std::cout << std::endl
              << Stack.ToString() << std::endl
              << "<<--" << std::endl;
      );

    CPqStack __Stack;
    CPqMsg_requestMsgShareHID __Msg;

    __Stack.Parse(Stack.ToString());
    __Stack.Decode(__Msg);

    CPPUNIT_ASSERT(Msg.m_strCmdType == __Msg.m_strCmdType );

    DBG(__Msg.dump(););
}

void CTestMan::testEncodeDecode_requestMsgPubValue()
{
    CPqStack Stack;

    CPqMsg_requestMsgPubValue Msg;
    CValueMsg Value;

    Msg.m_strDevID = "G1751453539";

    Value.m_strChannel = "1";
    Value.m_strType = "Temperature";
    Value.m_strValue = "15";

    Msg.Values.push_back(Value);


    Value.m_strChannel = "2";
    Value.m_strType = "Humidity";
    Value.m_strValue = "55";

    Msg.Values.push_back(Value);

    Stack.Encode(Msg);

    std::cout << std::endl << "-->> CPqMsg_requestMsgPubValue" << " Encode:" << std::endl;
    DBG(std::cout << std::endl <<  Stack.ToString() << std::endl << "<<--" << std::endl;);

    CPqStack __Stack;
    CPqMsg_requestMsgPubValue __Msg;

    __Stack.Parse(Stack.ToString());
    __Stack.Decode(__Msg);

    CPPUNIT_ASSERT(Msg.m_strDevID == __Msg.m_strDevID );

    //__Msg.dump();
}

void CTestMan::testEncodeDecode_requestMsgPubValueAck()
{
    CPqStack Stack;

    CPqMsg_requestMsgPubValueAck Msg;
    CValueMsg Value;

    Msg.Result = "0";
    Msg.Reason = "ok";

    Msg.m_strDevID = "G1751453539";

    Value.m_strChannel = "1";
    Value.m_strType = "Temperature";
    Value.m_strValue = "15";

    Msg.Values.push_back(Value);


    Value.m_strChannel = "2";
    Value.m_strType = "Humidity";
    Value.m_strValue = "55";

    Msg.Values.push_back(Value);

    Stack.Encode(Msg);

    std::cout << std::endl << "-->> CPqMsg_requestMsgPubValueAck" << " Encode:" << std::endl;
    DBG(std::cout << std::endl <<  Stack.ToString() << std::endl << "<<--" << std::endl;);

    CPqStack __Stack;
    CPqMsg_requestMsgPubValueAck __Msg;

    __Stack.Parse(Stack.ToString());
    __Stack.Decode(__Msg);

    CPPUNIT_ASSERT(Msg.Result == __Msg.Result );
    CPPUNIT_ASSERT(Msg.m_strDevID == __Msg.m_strDevID );

    //__Msg.dump();
}

void CTestMan::testEncodeDecode_requestMsgPubValueList()
{
    CPqStack Stack;
    CPqMsg_requestMsgPubValueList Msg;
    Msg.m_strGroupID = "701818788";

    {
        CPqMsg_requestMsgPubValue __DeviceValues;
        CValueMsg Value;

        __DeviceValues.m_strDevID = "G1751453539";

        Value.m_strChannel = "1";
        Value.m_strType = "Temperature";
        Value.m_strValue = "15";
        __DeviceValues.Values.push_back(Value);


        Value.m_strChannel = "2";
        Value.m_strType = "Humidity";
        Value.m_strValue = "55";
        __DeviceValues.Values.push_back(Value);

        Msg.DeviceValues.push_back(__DeviceValues);
    }

    {
        CPqMsg_requestMsgPubValue __DeviceValues;
        CValueMsg Value;

        __DeviceValues.m_strDevID = "G1751453540";

        Value.m_strChannel = "1";
        Value.m_strType = "Temperature";
        Value.m_strValue = "15";
        __DeviceValues.Values.push_back(Value);


        Value.m_strChannel = "2";
        Value.m_strType = "Humidity";
        Value.m_strValue = "55";
        __DeviceValues.Values.push_back(Value);

        Msg.DeviceValues.push_back(__DeviceValues);
    }

    Stack.Encode(Msg);

    std::cout << std::endl << "-->> testEncodeDecode_requestMsgPubValueList" << " Encode:" << std::endl;
    DBG(std::cout << std::endl <<  Stack.ToString() << std::endl << "<<--" << std::endl;);

    CPqStack __Stack;
    CPqMsg_requestMsgPubValueList __Msg;

    __Stack.Parse(Stack.ToString());
    __Stack.Decode(__Msg);

    CPPUNIT_ASSERT(__Msg.m_strGroupID == Msg.m_strGroupID );
    CPPUNIT_ASSERT(__Msg.DeviceValues.size() == 2 );
}

void CTestMan::testEncodeDecode_requestMsgPubValueListAck()
{
    CPqStack Stack;
    CPqMsg_requestMsgPubValueListAck Msg;
    Msg.Result = "0";
    Msg.Reason = "Ok";
    Msg.m_strGroupID = "701818788";

    {
        CPqMsg_requestMsgPubValue __DeviceValues;
        CValueMsg Value;

        __DeviceValues.m_strDevID = "G1751453539";

        Value.m_strChannel = "1";
        Value.m_strType = "Temperature";
        Value.m_strValue = "15";
        __DeviceValues.Values.push_back(Value);


        Value.m_strChannel = "2";
        Value.m_strType = "Humidity";
        Value.m_strValue = "55";
        __DeviceValues.Values.push_back(Value);

        Msg.DeviceValues.push_back(__DeviceValues);
    }

    {
        CPqMsg_requestMsgPubValue __DeviceValues;
        CValueMsg Value;

        __DeviceValues.m_strDevID = "G1751453540";

        Value.m_strChannel = "1";
        Value.m_strType = "Temperature";
        Value.m_strValue = "15";
        __DeviceValues.Values.push_back(Value);


        Value.m_strChannel = "2";
        Value.m_strType = "Humidity";
        Value.m_strValue = "55";
        __DeviceValues.Values.push_back(Value);

        Msg.DeviceValues.push_back(__DeviceValues);
    }

    Stack.Encode(Msg);

    std::cout << std::endl << "-->> testEncodeDecode_requestMsgPubValueListAck" << " Encode:" << std::endl;
    DBG(std::cout << std::endl <<  Stack.ToString() << std::endl << "<<--" << std::endl;);

    CPqStack __Stack;
    CPqMsg_requestMsgPubValueListAck __Msg;

    __Stack.Parse(Stack.ToString());
    __Stack.Decode(__Msg);

    CPPUNIT_ASSERT(__Msg.m_strGroupID == Msg.m_strGroupID );
    CPPUNIT_ASSERT(__Msg.DeviceValues.size() == 2 );
}

void CTestMan::testEncodeDecode_requestMsgSetSchedule()
{
    CPqStack Stack;

    CPqMsg_requestMsgSetSchedule Msg;
    CScheduleMsg Value;

    Msg.m_strDevID = "G1751453539";

    Value.m_strScheduleID = "S001";
    Value.m_strMinute = "*";
    Value.m_strHour   = "*";
    Value.m_strDay    = "*";
    Value.m_strMonth  = "*";
    Value.m_strDate   = "*";
    Value.m_strTaskID   = "TaskA";
    Value.m_strTaskType= "1";
    Value.m_strTaskData= "100";

    Msg.Schedules.push_back(Value);


    Value.m_strScheduleID = "S002";
    Value.m_strMinute = "1";
    Value.m_strHour   = "2";
    Value.m_strTaskID   = "TaskB";
    Value.m_strTaskType= "1";

    Msg.Schedules.push_back(Value);

    Stack.Encode(Msg);

    std::cout << std::endl << "-->> CPqMsg_requestMsgSetSchedule" << " Encode:" << std::endl;
    DBG(std::cout << std::endl <<  Stack.ToString() << std::endl << "<<--" << std::endl;);

    CPqStack __Stack;
    CPqMsg_requestMsgSetSchedule __Msg;

    __Stack.Parse(Stack.ToString());
    __Stack.Decode(__Msg);

    CPPUNIT_ASSERT(Msg.m_strDevID == __Msg.m_strDevID );

    //__Msg.dump();
}


void CTestMan::testEncodeDecode_requestMsgGetSchedule()
{
    CPqStack Stack;

    CPqMsg_requestMsgGetSchedule Msg;
    CScheduleMsg Value;

    Msg.m_strDevID = "G1751453539";

    Stack.Encode(Msg);

    std::cout << std::endl << "-->> CPqMsg_requestMsgGetSchedule" << " Encode:" << std::endl;
    DBG(std::cout << std::endl <<  Stack.ToString() << std::endl << "<<--" << std::endl;);

    CPqStack __Stack;
    CPqMsg_requestMsgGetSchedule __Msg;

    __Stack.Parse(Stack.ToString());
    __Stack.Decode(__Msg);

    CPPUNIT_ASSERT(Msg.m_strDevID == __Msg.m_strDevID );

    //__Msg.dump();
}

void CTestMan::testEncodeDecode_requestMsgGetScheduleAck()
{
    CPqStack Stack;

    CPqMsg_requestMsgGetScheduleAck __ackMsg;
    CScheduleMsg Value;

    __ackMsg.Result = "0";
    __ackMsg.Reason = "OK";
    __ackMsg.m_strDevID = "G1751453539";

    Value.m_strScheduleID = "S001";
    Value.m_strMinute = "*";
    Value.m_strHour   = "*";
    Value.m_strDay    = "*";
    Value.m_strMonth  = "*";
    Value.m_strDate   = "*";
    Value.m_strTaskID   = "TaskA";
    Value.m_strTaskType= "1";
    Value.m_strTaskData = "100";

    __ackMsg.Schedules.push_back(Value);


    Value.m_strScheduleID = "S002";
    Value.m_strMinute = "1";
    Value.m_strHour   = "2";
    Value.m_strTaskID   = "TaskB";
    Value.m_strTaskType= "1";

    __ackMsg.Schedules.push_back(Value);

    Stack.Encode(__ackMsg);

    std::cout << std::endl << "-->> CPqMsg_requestMsgGetScheduleAck" << " Encode:" << std::endl;
    DBG(std::cout << std::endl <<  Stack.ToString() << std::endl << "<<--" << std::endl;);

    CPqStack __Stack;
    CPqMsg_requestMsgGetScheduleAck __Msg;

    __Stack.Parse(Stack.ToString());
    __Stack.Decode(__Msg);

    CPPUNIT_ASSERT(__ackMsg.m_strDevID == __Msg.m_strDevID );

    //__Msg.dump();
}

void CTestMan::testEncodeDecode_requestMsgTaskPub()
{
    CPqStack Stack;

    CPqMsg_requestMsgTaskPub Msg;
    CValueMsg Value;

    Msg.m_strDevID = "G1751453539";
    Msg.m_strTaskID = "TaskA";

    Value.m_strChannel = "1";
    Value.m_strType = "Temperature";
    Value.m_strValue = "15";
    Msg.Values.push_back(Value);

    Value.m_strChannel = "1";
    Value.m_strType = "PH";
    Value.m_strValue = "7";
    Msg.Values.push_back(Value);

    Stack.Encode(Msg);

    std::cout << std::endl << "-->> CPqMsg_requestMsgTaskPub" << " Encode:" << std::endl;
    DBG(std::cout << std::endl <<  Stack.ToString() << std::endl << "<<--" << std::endl;);

    CPqStack __Stack;
    CPqMsg_requestMsgTaskPub __Msg;

    __Stack.Parse(Stack.ToString());
    __Stack.Decode(__Msg);

    CPPUNIT_ASSERT(Msg.m_strDevID == __Msg.m_strDevID );

    //__Msg.dump();
}
void CTestMan::testEncodeDecode_requestMsgTaskList()
{
    CPqStack Stack;

    CPqMsg_requestMsgTaskList Msg;
    CTaskMsg Value;

    Msg.m_strDevID = "G1751453539";

    Stack.Encode(Msg);

    std::cout << std::endl << "-->> CPqMsg_requestMsgTaskList" << " Encode:" << std::endl;
    DBG(std::cout << std::endl <<  Stack.ToString() << std::endl << "<<--" << std::endl;);

    CPqStack __Stack;
    CPqMsg_requestMsgTaskList __Msg;

    __Stack.Parse(Stack.ToString());
    __Stack.Decode(__Msg);

    CPPUNIT_ASSERT(Msg.m_strDevID == __Msg.m_strDevID );

    //__Msg.dump();
}


void CTestMan::testEncodeDecode_requestMsgTaskListAck()
{
    CPqStack Stack;

    CPqMsg_requestMsgTaskListAck Msg;
    CTaskMsg Value;

    Msg.m_strDevID = "G1751453539";

    Value.m_strTaskID = "TaskA";
    Value.m_strTaskType= "1";
    Value.m_strOption= "IoOut 1 On;IoOut 3 On;";
    Value.m_strMemo= "Wash";

    Msg.Tasks.push_back(Value);


    Value.m_strTaskID = "TaskB";
    Value.m_strTaskType= "1";
    Value.m_strOption= "IoOut 1 Off&&IoOut 2 On&&IoOut 3 Off";
    Value.m_strMemo= "Working";

    Msg.Tasks.push_back(Value);

    Stack.Encode(Msg);

    std::cout << std::endl << "-->> CPqMsg_requestMsgTaskList" << " Encode:" << std::endl;
    DBG(std::cout << std::endl <<  Stack.ToString() << std::endl << "<<--" << std::endl;);

    CPqStack __Stack;
    CPqMsg_requestMsgTaskListAck __Msg;

    __Stack.Parse(Stack.ToString());
    __Stack.Decode(__Msg);

    CPPUNIT_ASSERT(Msg.m_strDevID == __Msg.m_strDevID );

    //__Msg.dump();
}

void CTestMan::testEncodeDecode_requestMsgTaskDetails()
{
    CPqStack Stack;

    CPqMsg_requestMsgTaskDetails Msg;
    CTaskMsg Value;

    Msg.m_strDevID = "G1751453539";
    Msg.m_strTaskID = "TaskA";
    Msg.m_strTaskType= "1";
    Msg.m_strStartDt   = "1487638397";
    Msg.m_strEndDt     = "1487638357";

    Stack.Encode(Msg);

    std::cout << std::endl << "-->> CPqMsg_requestMsgTaskDetails" << " Encode:" << std::endl;
    DBG(std::cout << std::endl <<  Stack.ToString() << std::endl << "<<--" << std::endl;);

    CPqStack __Stack;
    CPqMsg_requestMsgTaskDetails __Msg;

    __Stack.Parse(Stack.ToString());
    __Stack.Decode(__Msg);

    CPPUNIT_ASSERT(Msg.m_strDevID == __Msg.m_strDevID );

    //__Msg.dump();
}

void CTestMan::testEncodeDecode_requestMsgTaskDetailsAck()
{
    CPqStack Stack;

    CPqMsg_requestMsgTaskDetailsAck __ackMsg;
    CTaskValue __taskValue;
    CValueMsg Value;

    __ackMsg.Result = "0";
    __ackMsg.Reason = "ok";
    __ackMsg.m_strDevID = "G1751453539";

    __taskValue.m_strDataID    = "1e3ac1d64f1363d37dc4d8af5cbb4d6f";
    __taskValue.m_strTriggerID = "S01";
    __taskValue.m_strTaskID    = "TaskA";
    __taskValue.m_strTaskType= "1";
    __taskValue.m_strStartDt   = "1487638397";
    __taskValue.m_strEndDt     = "1487638357";

    __taskValue.m_strLastTaskID    = "TaskA";
    __taskValue.m_strLastTaskType  = "1";
    __taskValue.m_strLastTaskValue = "111";

    Value.m_strChannel = "1";
    Value.m_strType = "Temperature";
    Value.m_strValue = "15";

    __taskValue.Values.push_back(Value);
    Value.m_strChannel = "2";
    Value.m_strType = "Humidity";
    Value.m_strValue = "55";
    __taskValue.Values.push_back(Value);

    __ackMsg.TaskValues.push_back(__taskValue);

    ///////////////
    __taskValue.m_strDataID    = "df07e219d03ec0f687135fa21d2f315f";
    __taskValue.m_strTriggerID = "S01";
    __taskValue.m_strTaskID    = "TaskB";
    __taskValue.m_strTaskType= "1";
    __taskValue.m_strStartDt   = "1487638387";
    __taskValue.m_strEndDt     = "1487638457";

    __taskValue.m_strLastTaskID    = "TaskA";
    __taskValue.m_strLastTaskType  = "1";
    __taskValue.m_strLastTaskValue = "111";

    __ackMsg.TaskValues.push_back(__taskValue);


    Stack.Encode(__ackMsg);

    std::cout << std::endl << "-->> CPqMsg_requestMsgTaskDetailsAck" << " Encode:" << std::endl;
    DBG(std::cout << std::endl <<  Stack.ToString() << std::endl << "<<--" << std::endl;);

    CPqStack __Stack;
    CPqMsg_requestMsgTaskDetailsAck __Msg;

    __Stack.Parse(Stack.ToString());
    __Stack.Decode(__Msg);

    CPPUNIT_ASSERT(__ackMsg.m_strDevID == __Msg.m_strDevID );
    CPPUNIT_ASSERT(__ackMsg.TaskValues.size() == __Msg.TaskValues.size() );

    __Msg.dump();
}

void CTestMan::testEncodeDecode_requestMsgDevices()
{
    CPqStack Stack;

    CPqMsg_requestMsgDeviceList Msg;

    Msg.m_strGroupID  = "GID";
    Msg.m_iPageSize = 9999;
    Msg.m_iPages    = 1;


    Stack.Encode(Msg);

    std::cout << std::endl << "-->> CPqMsg_requestMsgDeviceList" << " Encode:" << std::endl;
    DBG(std::cout << std::endl <<  Stack.ToString() << std::endl << "<<--" << std::endl;);

    CPqStack __Stack;
    CPqMsg_requestMsgDeviceList __Msg;

    __Stack.Parse(Stack.ToString());
    __Stack.Decode(__Msg);

    CPPUNIT_ASSERT(Msg.m_strGroupID == __Msg.m_strGroupID );

    //__Msg.dump();
}

void CTestMan::testEncodeDecode_requestMsgDevicesAck()
{
    CPqStack Stack;

    CPqMsg_requestMsgDeviceListAck Msg;
    CDeviceMsg Value;

    Msg.Result = "0";
    Msg.Reason = "ok";

    Msg.m_strGroupID      = "GID1";
    Msg.m_iTotalCount   = 2;
    Msg.m_iPages        = 1;

    Value.m_strDevID = "Dev1";
    Value.m_strCode = "T1";
    Value.m_strStatus = "1";
    Value.m_strLastOnTime = "1527688051";

    Msg.Devices.push_back(Value);


    Value.m_strDevID = "Dev2";
    Value.m_strCode = "T1";
    Value.m_strStatus = "1";
    Value.m_strLastOnTime = "1527688051";

    Msg.Devices.push_back(Value);

    Stack.Encode(Msg);

    std::cout << std::endl << "-->> CPqMsg_requestMsgDeviceListAck" << " Encode:" << std::endl;
    DBG(std::cout << std::endl <<  Stack.ToString() << std::endl << "<<--" << std::endl;);

    CPqStack __Stack;
    CPqMsg_requestMsgDeviceListAck __Msg;

    __Stack.Parse(Stack.ToString());
    __Stack.Decode(__Msg);

    CPPUNIT_ASSERT(Msg.Result == __Msg.Result );
    CPPUNIT_ASSERT(Msg.m_strGroupID == __Msg.m_strGroupID );

    //__Msg.dump();
}
void CTestMan::testEncodeDecode_requestMsgSetShpc()
{
    CPqStack Stack;

    CPqMsg_requestMsgSetShpc Msg;
    CShpc Value;

    Msg.m_strDevID    = "G1751453539";
    Msg.m_strFunc     = "/sbin/shpc.jsict.ini";
    Msg.m_strsHost    = "vsa.shp.ezlibs.com";
    Msg.m_strsPort    = 18701;
    Msg.m_strsTocken  = "d9239799243904debc55fddb9107cc4fe58ee9bb";


    Value.m_strcIteam       = "G1536007353VH1-554";
    Value.m_strcType        = "tcp";
    Value.m_strcHost        = "192.168.182.61";
    Value.m_strcPort        = 554;
    Value.m_strcRemontePort = 16348;

    Msg.Shpcs.push_back(Value);

    Stack.Encode(Msg);

    std::cout << std::endl << "-->> CPqMsg_requestMsgSetShpc" << " Encode:" << std::endl;
    DBG(std::cout << std::endl <<  Stack.ToString() << std::endl << "<<--" << std::endl;);

    CPqStack __Stack;
    CPqMsg_requestMsgSetShpc __Msg;

    __Stack.Parse(Stack.ToString());
    __Stack.Decode(__Msg);

    CPPUNIT_ASSERT(Msg.m_strDevID == __Msg.m_strDevID );
    CPPUNIT_ASSERT(1 == __Msg.Shpcs.size() );
    //__Msg.dump();
}

void CTestMan::testEncodeDecode_requestMsgSetShpcAck()
{
    CPqStack Stack;

    CPqMsg_requestMsgSetShpcAck Msg;
    CShpc Value;

    Msg.Result        = "0";
    Msg.Reason        = "OK";

    Msg.m_strDevID    = "G1751453539";
    Msg.m_strFunc     = "/sbin/shpc.jsict.ini";
    Msg.m_strsHost    = "vsa.shp.ezlibs.com";
    Msg.m_strsPort    = 18701;
    Msg.m_strsTocken  = "d9239799243904debc55fddb9107cc4fe58ee9bb";

    Value.m_strcIteam       = "G1536007353VH1-554";
    Value.m_strcType        = "tcp";
    Value.m_strcHost        = "192.168.182.61";
    Value.m_strcPort        = 554;
    Value.m_strcRemontePort = 16348;
    Msg.Shpcs.push_back(Value);

    Value.m_strcIteam       = "G1536007353VH1-80";
    Value.m_strcType        = "tcp";
    Value.m_strcHost        = "192.168.182.61";
    Value.m_strcPort        = 80;
    Value.m_strcRemontePort = 16349;
    Msg.Shpcs.push_back(Value);

    Stack.Encode(Msg);

    std::cout << std::endl << "-->> CPqMsg_requestMsgSetShpcAck" << " Encode:" << std::endl;
    // std::cout << std::endl <<  Stack.ToString() << std::endl << "<<--" << std::endl;

    CPqStack __Stack;
    CPqMsg_requestMsgSetShpcAck __Msg;

    __Stack.Parse(Stack.ToString());
    __Stack.Decode(__Msg);

    CPPUNIT_ASSERT(Msg.m_strDevID == __Msg.m_strDevID );
    CPPUNIT_ASSERT(2 == __Msg.Shpcs.size() );
    //__Msg.dump();
}

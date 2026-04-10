/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * TestMan.cpp - _explain_
 *
 * Copyright (C) 2013 gminiTech.com, All Rights Reserved.
 *
 * $Id: TestMan.cpp 0001 2013-11-02 14:38:29Z wu_junjie $
 *
 *  Explain:
 *     UnitTest -- CpromQLStack
 *
 *  Update:
 *     2013-11-03 14:38:33 WuJunjie 549 Create
 *
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
#include <ezUtilCPP/StringOpr.h>

#include "TestMan.h"

#include "../promQLStack.h"

//#define _DEBUG_THIS
#include <ezutil/ez_def_devel_debug.h>

CPPUNIT_TEST_SUITE_REGISTRATION( CTestMan );

void CTestMan::test_CMmtMsgPlayUrlList()
{
    CpromQLStack Stack;
    CMmtMsgPlayUrlList msg;
    std::string str_msg = "{\"result\":0,\"UrlList\":[{\"Url\":\"http://4.49.7.25:30010/rtp/0BEBD12C.flv\",\"Type\":\"Flv\"},{\"Url\":\"http://4.49.7.25:30010/rtp/0BEBD12C/hls.m3u8\",\"Type\":\"Hls\"},{\"Url\":\"rtmp://4.49.7.25:30008/rtp/0BEBD12C\",\"Type\":\"Rtmp\"},{\"Url\":\"rtsp://4.49.7.25:30006/rtp/0BEBD12C\",\"Type\":\"Rtsp\"}]}";

    Stack.Parse(str_msg);
    Stack.Decode(msg);
    msg.dump();

    CpromQLStack encStack;
    encStack.Encode(msg); //

// DBG(
    std::cout << std::endl << "-->> CMmtMsgPlayUrlList" << " Encode:" << std::endl;
    std::cout << std::endl <<  encStack.ToString() << std::endl << "<<--" << std::endl;
// );

    CpromQLStack __Stack;
    CMmtMsgPlayUrlList __msg;

    __Stack.Parse(encStack.ToString());
    __Stack.Decode(__msg);

    CPPUNIT_ASSERT(msg.Result == __msg.Result );
}

void CTestMan::test_CMmtMsgValuesIpcamDialing()
{
    CpromQLStack Stack;
    CMmtMsgValuesIpcamDialing msg;

    msg.m_strMmtSid = "0000132020000001310162294";
    msg.Items = 100;
    
    Stack.Encode(msg); //
DBG(
    std::cout << std::endl << "-->> CMmtMsgValuesIpcamDialing" << " Encode:" << std::endl;
    std::cout << std::endl <<  Stack.ToString() << std::endl << "<<--" << std::endl;
);

    CpromQLStack __Stack;
    CMmtMsgValuesIpcamDialing __msg;

    __Stack.Parse(Stack.ToString());
    __Stack.Decode(__msg);

    CPPUNIT_ASSERT(msg.m_strMmtSid == __msg.m_strMmtSid );
}

void CTestMan::test_CMmtMsgValuesIpcamDialingAck()
{
    CpromQLStack Stack;
    CMmtMsgValuesIpcamDialingAck msg;

    msg.Items = 100;
    msg.RemainingItems = 1000;

    // 0000132020000001310162294
    {
        // CMsgMmtReqValueCamDialing msg;
        CMmtMsgValueCamDialing dialineMsg;
        dialineMsg.DevId = "0000132020000001310162294";

        CDialingResult __result;
        __result.m_strResult = "0";
        __result.m_strResultMsg = "http";
        __result.m_strTime = 1619769995;
        dialineMsg.DialingResults.push_back(__result);

        __result.m_strResult = "1";
        __result.m_strResultMsg = "rtsp";
        __result.m_strTime = 1619769998;
        dialineMsg.DialingResults.push_back(__result);

        msg.ValuesIpcamDialing.push_back(dialineMsg);
    }
    Stack.Encode(msg); //

DBG(
    std::cout << std::endl << "-->> CMmtMsgValuesIpcamDialingAck" << " Encode:" << std::endl;
    std::cout << std::endl <<  Stack.ToString() << std::endl << "<<--" << std::endl;
);

    CpromQLStack __Stack;
    CMmtMsgValuesIpcamDialingAck __msg;

    __Stack.Parse(Stack.ToString());
    __Stack.Decode(__msg);

    CPPUNIT_ASSERT(msg.RemainingItems == __msg.RemainingItems );
    CPPUNIT_ASSERT(__msg.ValuesIpcamDialing.size()==1);
    std::list<CMmtMsgValueCamDialing>::iterator ci = __msg.ValuesIpcamDialing.begin();
    CPPUNIT_ASSERT(ci->DialingResults.size()==2);
}

void CTestMan::test_CMsgMmtReqValueCamDialingAck()
{
    CpromQLStack Stack;
    CMsgMmtReqValueCamDialingAck msg;

    msg.DevId = "0000132020000001310162294";

    CDialingResult __result;
    __result.m_strResult = "0";
    __result.m_strResultMsg = "http";
    __result.m_strTime = 1619769995;
    msg.DialingResults.push_back(__result);

    __result.m_strResult = "1";
    __result.m_strResultMsg = "rtsp";
    __result.m_strTime = 1619769998;
    msg.DialingResults.push_back(__result);

    Stack.Encode(msg); //
DBG(
    std::cout << std::endl << "-->> CMsgMmtReqValueCamDialingAck" << " Encode:" << std::endl;
    std::cout << std::endl <<  Stack.ToString() << std::endl << "<<--" << std::endl;
);

    CpromQLStack __Stack;
    CMsgMmtReqValueCamDialingAck __msg;

    __Stack.Parse(Stack.ToString());
    __Stack.Decode(__msg);

    CPPUNIT_ASSERT(msg.DevId == __msg.DevId );
}

void CTestMan::test_CMsgMmtReqValueCamDialing()
{
    CpromQLStack Stack;
    CMsgMmtReqValueCamDialing msg;

    msg.DevId = "0000132020000001310162294";
    
    Stack.Encode(msg); //
DBG(
    std::cout << std::endl << "-->> CMsgMmtReqValueCamDialing" << " Encode:" << std::endl;
    std::cout << std::endl <<  Stack.ToString() << std::endl << "<<--" << std::endl;
);

    CpromQLStack __Stack;
    CMsgMmtReqValueCamDialing __msg;

    __Stack.Parse(Stack.ToString());
    __Stack.Decode(__msg);

    CPPUNIT_ASSERT(msg.DevId == __msg.DevId );
}

void CTestMan::test_CMmtMsgCamDialing()
{
    CpromQLStack Stack;
    CMmtMsgCamDialing msg;

    msg.DevId = "0000132020000001310162294";
    msg.MediaSource.iType        = 18; // 无锡运维
    msg.MediaSource.Account     = "account";
    msg.MediaSource.Passwd      = "passwd";
    msg.MediaSource.Channel     = "1";
    msg.MediaSource.Subtype     = "1";
    msg.MediaSource.PlayUrl     = "rtsp://4.49.7.25:30006/rtp/0BEBE398";
    msg.MediaSource.Description = "MeiCun.wx";

    msg.Task.Priority = 1;
    msg.Task.Period   = 1;
    msg.Task.Times    = 1;
    
    Stack.Encode(msg); //
DBG(
    std::cout << std::endl << "-->> CMmtMsgCamDialing" << " Encode:" << std::endl;
    std::cout << std::endl <<  Stack.ToString() << std::endl << "<<--" << std::endl;
);

    CpromQLStack __Stack;
    CMmtMsgCamDialing __msg;

    __Stack.Parse(Stack.ToString());
    __Stack.Decode(__msg);

    CPPUNIT_ASSERT(msg.DevId == __msg.DevId );
    CPPUNIT_ASSERT(msg.MediaSource.iType == __msg.MediaSource.iType );
}

// 指标采集
void CTestMan::test_CMsgPromQlValue()
{
    CpromQLStack __Stack;

    // 数据采集而来， 先解码， 后编码外传

    // 解码
    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

//    std::string strValue = "[{\"status\":\"success\",\"data\":{\"resultType\":\"vector\",\"result\":[{\"metric\":{\"instance\":\"WXVM-08-VAS-WPC-99\"},\"value\":[1618469852.042,\"0.8691911254708251\"]},{\"metric\":{\"instance\":\"gts11AtHXYun\"},\"value\":[1618469852.042,\"14.300076331614353\"]}]}}]";
//    std::string strValueType = PromQlMsg_Key_CpuUsage;

    std::string strValue = "{\"status\":\"success\",\"data\":{\"resultType\":\"vector\",\"result\":[{\"metric\":{\"__name__\":\"up\",\"instance\":\"WXVM-01-APP\",\"job\":\"0101-视频运维\"},\"value\":[1618885833.146,\"0\"]},{\"metric\":{\"__name__\":\"up\",\"instance\":\"WXVM-02-DBS\",\"job\":\"0101-视频运维\"},\"value\":[1618885833.146,\"0\"]},{\"metric\":{\"__name__\":\"up\",\"instance\":\"WXVM-03-CNT\",\"job\":\"0101-视频运维\"},\"value\":[1618885833.146,\"0\"]},{\"metric\":{\"__name__\":\"up\",\"instance\":\"WXVM-04-VIN\",\"job\":\"0101-视频运维\"},\"value\":[1618885833.146,\"0\"]},{\"metric\":{\"__name__\":\"up\",\"instance\":\"WXVM-05-VIN\",\"job\":\"0101-视频运维\"},\"value\":[1618885833.146,\"0\"]},{\"metric\":{\"__name__\":\"up\",\"instance\":\"WXVM-06-VDT\",\"job\":\"0101-视频运维\"},\"value\":[1618885833.146,\"0\"]},{\"metric\":{\"__name__\":\"up\",\"instance\":\"WXVM-07-VDT-WINT400\",\"job\":\"0101-视频运维\"},\"value\":[1618885833.146,\"0\"]},{\"metric\":{\"__name__\":\"up\",\"instance\":\"WXVM-08-VAS-WPC-99\",\"job\":\"0101-视频运维\"},\"value\":[1618885833.146,\"1\"]},{\"metric\":{\"__name__\":\"up\",\"instance\":\"WXVM-PPG-DB\",\"job\":\"ppgdb\"},\"value\":[1618885833.146,\"1\"]},{\"metric\":{\"__name__\":\"up\",\"instance\":\"gts11AtHXYun\",\"job\":\"0101-Turn.通用.天翼看店\"},\"value\":[1618885833.146,\"1\"]}]}}";
    std::string strValueType = PromQlMsg_Key_HostUp;


    ez_Trim(strValue, "[");
    ez_Trim(strValue, "]");

    __Stack.Parse(strValue);

    CMsgPromQlValue __msg;
    __Stack.Decode(__msg); //

DBG(
    __msg.dump();
);

    // 编码
    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
    CpromQLStack __StackSnd;
    __StackSnd.Encode(__msg, strValueType); //

DBG(
        std::cout << std::endl
                  << "-->> CpromQLStack"
                  << " Encode:" << std::endl;
        std::cout << std::endl
                  << __StackSnd.ToString() << std::endl
                  << "<<--" << std::endl;
);

    // CPPUNIT_ASSERT(MsgDecoded.m_strKeyVersion == result );
    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
}
// void CTestMan::test_CMsgPromQlMemUsage()
// {
//     CpromQLStack __Stack;

//     // 数据采集而来， 先解码， 后编码外传

//     // 解码
//     /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

//     std::string result = "{\"status\":\"success\",\"data\":{\"resultType\":\"vector\",\"result\":[{\"metric\":{\"instance\":\"WXVM-08-VAS-WPC-99\",\"job\":\"0101-视频运维\"},\"value\":[1618792443.465,\"11.77978511678927\"]},{\"metric\":{\"instance\":\"gts11AtHXYun\",\"job\":\"0101-Turn.通用.天翼看店\"},\"value\":[1618792443.465,\"22.053641656664823\"]}]}}";
//     // ez_Trim(result, "[");
//     // ez_Trim(result, "]");

//     __Stack.Parse(result);

//     CMsgPromQlMemUsage __msg;
//     __Stack.Decode(__msg); //

//     //    cout << endl;
//     __msg.dump();
//     //    cout << endl;
//     // 编码
//     /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
//     // 
//     // __msg.Result = "0";
//     // __msg.Reason = "OK";
//     CpromQLStack __StackSnd;
//     __StackSnd.Encode(__msg); //
    
// DBG(
//         std::cout << std::endl
//                   << "-->> CpromQLStack"
//                   << " Encode:" << std::endl;
//         std::cout << std::endl
//                   << __StackSnd.ToString() << std::endl
//                   << "<<--" << std::endl;
// );

//     // CPPUNIT_ASSERT(MsgDecoded.m_strKeyVersion == result );
//     /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
// }
// void CTestMan::test_CMsgPromQlDiskUsage()
// {
//     CpromQLStack __Stack;

//     // 数据采集而来， 先解码， 后编码外传

//     // 解码
//     /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

//     std::string result = "{\"status\":\"success\",\"data\":{\"resultType\":\"vector\",\"result\":[{\"metric\":{\"instance\":\"WXVM-08-VAS-WPC-99\"},\"value\":[1618793892.817,\"80.67512026810412\"]},{\"metric\":{\"instance\":\"gts11AtHXYun\"},\"value\":[1618793892.817,\"36.9510687794711\"]}]}}";
//     // ez_Trim(result, "[");
//     // ez_Trim(result, "]");

//     __Stack.Parse(result);

//     CMsgPromQlDiskUsage __msg;
//     __Stack.Decode(__msg); //

//     //    cout << endl;
//     __msg.dump();
//     //    cout << endl;
//     // 编码
//     /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
//     // 
//     // __msg.Result = "0";
//     // __msg.Reason = "OK";
//     CpromQLStack __StackSnd;
//     __StackSnd.Encode(__msg); //
    
// DBG(
//         std::cout << std::endl
//                   << "-->> CpromQLStack"
//                   << " Encode:" << std::endl;
//         std::cout << std::endl
//                   << __StackSnd.ToString() << std::endl
//                   << "<<--" << std::endl;
// );

//     // CPPUNIT_ASSERT(MsgDecoded.m_strKeyVersion == result );
//     /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
// }
void CTestMan::setUp()
{}
void CTestMan::tearDown()
{}

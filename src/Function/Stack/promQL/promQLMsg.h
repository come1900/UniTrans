/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * promQLMsg.h - _explain_
 *
 * Copyright (C) 2013 ezlibs.com, All Rights Reserved.
 *
 * $Id: promQLMsg.h 0001 2013-11-02 14:38:29Z wu_junjie $
 *
 *  Explain:
 *     promQLMsg -- promQL http api
 *     Mmt -- miner maintain
 *
 *  Update:
 *     2013-11-02 14:38:33 WuJunjie 549 Create
 *
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#ifndef _promQLMsg_H
#define _promQLMsg_H

#include <string>
#include <vector>
#include <list>
#include <iostream>
#include <iomanip>
#include "../UniTransRestMsg.h"

const std::string MmtMsg_Key_MmtSid = "MmtSid";
class CMsgMmt_requestMsg
{
public:
    std::string m_strMmtSid;

    CMsgMmt_requestMsg()
    {
        m_strMmtSid = "";
    }

    void dump()
    {
#ifdef _DEV_THIS_WITH_COUT
        std::cout << MmtMsg_Key_MmtSid << ":" << m_strMmtSid << std::endl;
#endif //_DEV_THIS_WITH_COUT
    }
};

const std::string PromQlMsg_Key_Value                                = "Value";
const std::string PromQlMsg_Key_Time                                 = "Time";

class CValues
{
public:
    std::string m_strValue;//数值
    double m_strTime;// utc time(NULL)
    
    CValues()
    {
        m_strTime=0;
    }
    void dump()
    {
#ifdef _DEV_THIS_WITH_COUT
        std::cout << std::setw(12) << std::right << PromQlMsg_Key_Value << ":" << m_strValue << std::endl;
        std::cout << std::setw(12) << std::right << PromQlMsg_Key_Time << ":" << std::fixed << std::setprecision(3) << m_strTime << std::endl;
        // printf("%s:%.3f\n", PromQlMsg_Key_Time.c_str(), m_strTime);
#endif //_DEV_THIS_WITH_COUT
    }
};

const std::string PromQlMsg_Key_Host                                = "Host";
class CHostValues : public CValues
{
public:
    std::string m_strHost;// 主机
    void dump()
    {
#ifdef _DEV_THIS_WITH_COUT
        std::cout << std::setw(12) << std::right << PromQlMsg_Key_Host << ":" << m_strHost << std::endl;
        CValues::dump();
#endif //_DEV_THIS_WITH_COUT
    }

    bool bValid()
    {
        return  !(m_strValue.empty() || m_strHost.empty()) ;
    }
};

//
// PromQlValue
//
const std::string PromQlMsg_Key_TotalItems = "TotalItems";
const std::string PromQlMsg_Key_HostUp = "HostUp";
const std::string PromQlMsg_Key_CpuUsage = "CpuUsage";
const std::string PromQlMsg_Key_MemUsage = "MemUsage";
const std::string PromQlMsg_Key_MemTotal = "MemTotal";
const std::string PromQlMsg_Key_MemAvailable = "MemAvailable";
const std::string PromQlMsg_Key_DiskUsage = "DiskUsage";
const std::string PromQlMsg_Key_DiskTotal = "DiskTotal";
const std::string PromQlMsg_Key_DiskAvailable = "DiskAvailable";
const std::string PromQlMsg_Key_NetworkUpperrate = "NetworkUpperrate";
const std::string PromQlMsg_Key_NetworkDownwardrate = "NetworkDownwardrate";
class CMsgPromQlValue : public CAckMsgSimple
{
public:
    unsigned int iTotalItems;
    std::vector<CHostValues> m_vHost;

    CMsgPromQlValue()
    {
        Result = "0";
        Reason = "OK";
        iTotalItems = 0;
    }

    void dump()
    {
#ifdef _DEV_THIS_WITH_COUT
        // for (unsigned int ii=0; ii<Msg.m_vChannel.size(); ii++)
        std::cout << "HostSize"
                  << ":" << m_vHost.size() << std::endl;

        for (std::vector<CHostValues>::iterator iter = m_vHost.begin(); iter != m_vHost.end(); iter++)
        {
            iter->dump();
            std::cout << std::endl;
        }
#endif //_DEV_THIS_WITH_COUT
    }
};

// 摄像机检测
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

const std::string PromQlMsg_Key_MediaSource_Type        = "Type";
const std::string PromQlMsg_Key_MediaSource_Account     = "Account";
const std::string PromQlMsg_Key_MediaSource_Passwd      = "Passwd";
const std::string PromQlMsg_Key_MediaSource_Channel     = "Channel";
const std::string PromQlMsg_Key_MediaSource_Subtype     = "Subtype";
const std::string PromQlMsg_Key_MediaSource_PlayUrl     = "PlayUrl";
const std::string PromQlMsg_Key_MediaSource_Description = "Description";

class CMediaSource
{
public:
    int iType          ;
    std::string Account       ;
    std::string Passwd        ;
    std::string Channel       ;
    std::string Subtype       ;
    std::string PlayUrl       ;
    std::string Description   ;

    void dump()
    {
#ifdef _DEV_THIS_WITH_COUT
        std::cout << PromQlMsg_Key_MediaSource_Type           << ":" << iType         << std::endl;
        std::cout << PromQlMsg_Key_MediaSource_Account        << ":" << Account       << std::endl;
        std::cout << PromQlMsg_Key_MediaSource_Passwd         << ":" << Passwd        << std::endl;
        std::cout << PromQlMsg_Key_MediaSource_Channel        << ":" << Channel       << std::endl;
        std::cout << PromQlMsg_Key_MediaSource_Subtype        << ":" << Subtype       << std::endl;
        std::cout << PromQlMsg_Key_MediaSource_PlayUrl        << ":" << PlayUrl       << std::endl;
        std::cout << PromQlMsg_Key_MediaSource_Description    << ":" << Description   << std::endl;
#endif //_DEV_THIS_WITH_COUT
    }
};

const std::string PromQlMsg_Key_Task_Priority        = "Priority";
const std::string PromQlMsg_Key_Task_Period          = "Period";
const std::string PromQlMsg_Key_Task_Times           = "Times";

class CTask
{
public:
    int Priority ;
    int Period   ;
    int Times    ;

    void dump()
    {
#ifdef _DEV_THIS_WITH_COUT
        std::cout << PromQlMsg_Key_Task_Priority    << ":" << Priority   << std::endl;
        std::cout << PromQlMsg_Key_Task_Period    << ":" << Period   << std::endl;
        std::cout << PromQlMsg_Key_Task_Times    << ":" << Times   << std::endl;
#endif //_DEV_THIS_WITH_COUT
    }
};

const std::string PromQlMsg_Key_DevId        = "DevId";
const std::string PromQlMsg_Key_MediaSource  = "MediaSource";
const std::string PromQlMsg_Key_Task         = "Task";
class CMmtMsgCamDialing
{
public:
    std::string  DevId         ;
    CMediaSource MediaSource   ;
    CTask        Task          ;

    void dump()
    {
#ifdef _DEV_THIS_WITH_COUT
        std::cout << PromQlMsg_Key_DevId    << ":" << DevId   << std::endl;

        std::cout << "::\t" << PromQlMsg_Key_MediaSource   << std::endl;
        MediaSource.dump();

        std::cout << "::\t" << PromQlMsg_Key_Task   << std::endl;
        Task.dump();
#endif //_DEV_THIS_WITH_COUT
    }
};

// const std::string PromQlMsg_Key_DevIdList        = "DevIds";
class CMsgMmtReqValueCamDialing
{
public:
    std::string DevId;

    CMsgMmtReqValueCamDialing()
    {
        DevId = "";
    }

    void dump()
    {
#ifdef _DEV_THIS_WITH_COUT
        std::cout << PromQlMsg_Key_DevId << ":" << DevId << std::endl;
#endif //_DEV_THIS_WITH_COUT
    }
};

const std::string PromQlMsg_Key_Result                = "Result";
const std::string PromQlMsg_Key_ResultMsg             = "ResultMsg";
class CDialingResult
{
public:
    std::string m_strResult;   //结果
    std::string m_strResultMsg;//结果数据；放置图片等
    double m_strTime;// utc time(NULL)
    CDialingResult()
    {
        m_strResult = "";
        m_strResultMsg = "";
        m_strTime = 0;
    }

    void dump()
    {
#ifdef _DEV_THIS_WITH_COUT
        std::cout << PromQlMsg_Key_Result << ":" << m_strResult << std::endl;
        std::cout << PromQlMsg_Key_ResultMsg << ":" << m_strResultMsg << std::endl;
        std::cout << std::setw(12) << std::right << PromQlMsg_Key_Time << ":" << std::fixed << std::setprecision(3) << m_strTime << std::endl;
#endif //_DEV_THIS_WITH_COUT
    }
};

const std::string PromQlMsg_Key_DialingResults        = "DialingResults";
class CMmtMsgValueCamDialing
{
public:
    std::string DevId;
    std::list<CDialingResult> DialingResults;

    CMmtMsgValueCamDialing()
    {
        DevId = "";
    }

    void dump()
    {
#ifdef _DEV_THIS_WITH_COUT
        std::cout << PromQlMsg_Key_DialingResults << ":" << DevId << std::endl;
        for (std::list<CDialingResult>::iterator ci = DialingResults.begin() ; ci != DialingResults.end() ; ++ci)
        {
            ci->dump();
        }
        
#endif //_DEV_THIS_WITH_COUT
    }
};
class CMsgMmtReqValueCamDialingAck:public CAckMsgSimple, public CMmtMsgValueCamDialing
{
public:

    CMsgMmtReqValueCamDialingAck()
    {
    }

    void dump()
    {
#ifdef _DEV_THIS_WITH_COUT
        CAckMsgSimple::dump();        
        CMmtMsgValueCamDialing::dump();        
#endif //_DEV_THIS_WITH_COUT
    }
};

const std::string MmtMsg_Key_Items                     = "Items";
// 批量返回检测结果请求消息
class CMmtMsgValuesIpcamDialing : public CMsgMmt_requestMsg
{
public:
    int Items;

    CMmtMsgValuesIpcamDialing()
    {
        Items = 0;
    }

    void dump()
    {
#ifdef _DEV_THIS_WITH_COUT
        std::cout << MmtMsg_Key_Items << ":" << Items << std::endl;
        CMsgMmt_requestMsg::dump();
#endif //_DEV_THIS_WITH_COUT
    }
};

// 批量返回检测结果消息
const std::string PromQlMsg_Key_RemainingItems            = "RemainingItems";
const std::string PromQlMsg_Key_ValuesIpcamDialing        = "ValuesIpcamDialing";
class CMmtMsgValuesIpcamDialingAck : public CAckMsgSimple
{
public:
    int Items;
    int RemainingItems;
    std::list<CMmtMsgValueCamDialing> ValuesIpcamDialing;

    CMmtMsgValuesIpcamDialingAck()
    {
        Items = 0;
        RemainingItems = 0;
    }

    void dump()
    {
#ifdef _DEV_THIS_WITH_COUT
        std::cout << MmtMsg_Key_Items << ":" << Items << std::endl;
        std::cout << PromQlMsg_Key_RemainingItems << ":" << RemainingItems << std::endl;
        std::cout << PromQlMsg_Key_ValuesIpcamDialing << std::endl;
        for (std::list<CMmtMsgValueCamDialing>::iterator ci = ValuesIpcamDialing.begin() ; ci != ValuesIpcamDialing.end() ; ++ci)
        {
            ci->dump();
        }

#endif //_DEV_THIS_WITH_COUT
    }
};
// 
const std::string PromQlMsg_CMmtMsgPlayUrl_Key_Url            = "Url";
const std::string PromQlMsg_CMmtMsgPlayUrl_Key_Type            = "Type";
class CMmtMsgPlayUrl
{
public:
    std::string Url;   //http://4.49.7.25:30010/rtp/0BEBC5BD/hls.m3u8
    std::string Type;//结果数据；放置图片等
    CMmtMsgPlayUrl()
    {
        Url = "";
        Type = "";
    }

    void dump()
    {
#ifdef _DEV_THIS_WITH_COUT
        std::cout << PromQlMsg_CMmtMsgPlayUrl_Key_Url << ":" << Url << std::endl;
        std::cout << PromQlMsg_CMmtMsgPlayUrl_Key_Type << ":" << Type << std::endl;
#endif //_DEV_THIS_WITH_COUT
    }
};
const std::string PromQlMsg_Key_result        = "result";
const std::string PromQlMsg_Key_UrlList        = "UrlList";
class CMmtMsgPlayUrlList
{
public:
    int Result;
    std::list<CMmtMsgPlayUrl> UrlList;

    CMmtMsgPlayUrlList()
    {
        Result = 0;
    }
    void dump()
    {
#ifdef _DEV_THIS_WITH_COUT
        std::cout << PromQlMsg_Key_result << ":" << Result << std::endl;
        std::cout << PromQlMsg_Key_UrlList << "::::" << std::endl;
        for (std::list<CMmtMsgPlayUrl>::iterator ciUrl = UrlList.begin(); ciUrl != UrlList.end(); ++ciUrl)
        {
            ciUrl->dump();
        }
#endif //_DEV_THIS_WITH_COUT
    }
};
#endif /* _promQLMsg_H */

/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * YiHe1Msg.h - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: YiHe1Msg.h 5884 2014-10-12 04:33:15Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2014-10-12 04:33:15  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#ifndef _SmsMsg_H
#define _SmsMsg_H

#include <string>
#include <vector>
#include <iostream>

/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*-+-+-+-+-+-+-+- yi he tech interface msg define -+-+-+-+-+-+-+-+-+-+-+-*/
/*-+-+-+-+-+-+-+- 2014-10-12 16:32:45             -+-+-+-+-+-+-+-+-+-+-+-*/
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
//
// 消息组成元素——本身系统信息
//
//<Version  >版本号</Version>
//<Token    >令牌</Token>
//<From     >源地址</From>
//<To       >目的地址列表</To>
//<Type     >REQUEST</Type>
//<Seq      >序列号</Seq>
class CYH1MsgHeader
{
public:

    std::string m_strVersion;
    std::string m_strToken  ;
    std::string m_strFrom   ;
    std::string m_strTo     ;
    std::string m_strType   ;
    std::string m_strSeq    ;


    CYH1MsgHeader()
    {
        m_strVersion = "";
        m_strToken   = "";
        m_strFrom    = "";
        m_strTo      = "";
        m_strType    = "";
        m_strSeq     = "";
    }

    void dump()
    {
        //std::cout << "CYH1MsgHeader::dump:" << std::endl;

        std::cout << "Version:" << m_strVersion << std::endl;
        std::cout << "Token  :" << m_strToken   << std::endl;
        std::cout << "From   :" << m_strFrom    << std::endl;
        std::cout << "To     :" << m_strTo      << std::endl;
        std::cout << "Type   :" << m_strType    << std::endl;
        std::cout << "Seq    :" << m_strSeq     << std::endl;
    }
};

class CYH1MsgOperationAttr
{
public:

    std::string m_strOrder;
    std::string m_strName  ;

    CYH1MsgOperationAttr()
    {
        m_strOrder = "";
        m_strName   = "";
    }

    void dump()
    {
        std::cout << "m_strOrder:" << m_strOrder << std::endl;
        std::cout << "m_strName  :" << m_strName   << std::endl;
    }
};

class CYH1MsgObjectAttr
{
public:

    std::string m_strName  ;
    std::string m_strSumCount;

    CYH1MsgObjectAttr()
    {
        m_strName = "";
        m_strSumCount   = "";
    }

    void dump()
    {
        std::cout << "m_strName:" << m_strName << std::endl;
        std::cout << "m_strSumCount  :" << m_strSumCount   << std::endl;
    }
};


class CMsgGetSignalControlerParaRequest
{
public:
    CYH1MsgHeader m_yhHeader;
    CYH1MsgOperationAttr m_yhOprAttr;
    CYH1MsgObjectAttr m_yhObjAttr;

    CMsgGetSignalControlerParaRequest()
    {
    }

    void dump()
    {
        //std::cout << "CMsgGetSignalControlerParaRequest::dump:" << std::endl;
        m_yhHeader.dump();
        m_yhOprAttr.dump();
        m_yhObjAttr.dump();
    }

    /* for unit test */
    /* == */
    bool operator==(const CMsgGetSignalControlerParaRequest& obj) const
    {
        return this->m_yhHeader.m_strVersion==obj.m_yhHeader.m_strVersion
               && this->m_yhHeader.m_strType==obj.m_yhHeader.m_strType;
    }
};

class CYH1MsgSignalControlerPara
{
public:
    std::string m_strSignalControlerID;
    std::string m_strSupplier;
    std::string m_strType;
    std::string m_strCrossIDList; // vector, but 但是路口和信号机一一对应， 不必费事。

    CYH1MsgSignalControlerPara()
    {
        m_strSignalControlerID = "";
        m_strSupplier = "";
        m_strType = "";
        m_strCrossIDList = "";
    }

    void dump()
    {
        std::cout << "SignalControlerID:" << m_strSignalControlerID  << std::endl;
        std::cout << "Supplier         :" << m_strSupplier           << std::endl;
        std::cout << "Type             :" << m_strType               << std::endl;
        std::cout << "CrossIDList      :" << m_strCrossIDList        << std::endl;
    }
};

class CMsgGetSignalControlerParaResponse
{
public:
    CYH1MsgHeader m_yhHeader;
    CYH1MsgOperationAttr m_yhOprAttr;
    CYH1MsgObjectAttr m_yhObjAttr;

    //CYH1MsgSignalControlerPara m_yhSignalControlerPara;
    std::vector<CYH1MsgSignalControlerPara> m_v_yhSignalControlerPara;

    CMsgGetSignalControlerParaResponse()
    {
        ;
    }

    void dump()
    {
        m_yhHeader.dump();
        m_yhOprAttr.dump();
        m_yhObjAttr.dump();

        for (std::vector<CYH1MsgSignalControlerPara>::iterator it=m_v_yhSignalControlerPara.begin()
                ; it!=m_v_yhSignalControlerPara.end()
             ; it++)
        {
            it->dump();
        }

        //m_yhSignalControlerPara.dump();
    }
    /* for unit test */
    /* == */
    bool operator==(const CMsgGetSignalControlerParaResponse& obj) const
    {
        return this->m_yhHeader.m_strVersion==obj.m_yhHeader.m_strVersion
               && this->m_yhHeader.m_strType==obj.m_yhHeader.m_strType;
    }
};

class CMsgGetCrossParaRequest
{
public:
    CYH1MsgHeader m_yhHeader;
    CYH1MsgOperationAttr m_yhOprAttr;
    CYH1MsgObjectAttr m_yhObjAttr;
    std::string m_strSignalControlerID; 

    CMsgGetCrossParaRequest()
    {
    }

    void dump()
    {
        //std::cout << "CMsgGetCrossParaRequest::dump:" << std::endl;
        m_yhHeader.dump();
        m_yhOprAttr.dump();
        m_yhObjAttr.dump();
        std::cout << "SignalControlerID    :" << m_strSignalControlerID     << std::endl;
    }

    /* for unit test */
    /* == */
    bool operator==(const CMsgGetCrossParaRequest& obj) const
    {
        return this->m_yhHeader.m_strVersion==obj.m_yhHeader.m_strVersion
               && this->m_yhHeader.m_strType==obj.m_yhHeader.m_strType
               && this->m_strSignalControlerID==obj.m_strSignalControlerID;
    }
};

class CYH1MsgCrossPara
{
public:
    std::string m_strCrossID;
    std::string m_strCrossName;
    std::string m_strFeature;
    std::string m_strIsKey; // vector, but 但是路口和信号机一一对应， 不必费事。
        
    CYH1MsgCrossPara()
    {
        m_strCrossID = "";
        m_strCrossName = "";
        m_strFeature = "";
        m_strIsKey = "";
    }

    void dump()
    {
        std::cout << "CrossID   :" << m_strCrossID   << std::endl;
        std::cout << "CrossName :" << m_strCrossName << std::endl;
        std::cout << "Feature   :" << m_strFeature   << std::endl;
        std::cout << "IsKey     :" << m_strIsKey     << std::endl;
    }
};

class CMsgGetCrossParaResponse
{
public:
    CYH1MsgHeader m_yhHeader;
    CYH1MsgOperationAttr m_yhOprAttr;
    CYH1MsgObjectAttr m_yhObjAttr;
    std::string m_strSupplier; 

    std::vector<CYH1MsgCrossPara> m_v_yhCrossPara;
    std::string m_strSignalControlerID; 

    CMsgGetCrossParaResponse()
    {
        ;
    }

    void dump()
    {
        m_yhHeader.dump();
        m_yhOprAttr.dump();
        m_yhObjAttr.dump();

        for (std::vector<CYH1MsgCrossPara>::iterator it=m_v_yhCrossPara.begin()
                ; it!=m_v_yhCrossPara.end()
             ; it++)
        {
            it->dump();
        }

        std::cout << "m_strSupplier:" << m_strSupplier << std::endl;
        std::cout << "m_strSignalControlerID:" << m_strSignalControlerID << std::endl;
    }
    /* for unit test */
    /* == */
    bool operator==(const CMsgGetCrossParaResponse& obj) const
    {
        return this->m_yhHeader.m_strVersion==obj.m_yhHeader.m_strVersion
               && this->m_yhHeader.m_strType==obj.m_yhHeader.m_strType
               && this->m_strSignalControlerID==obj.m_strSignalControlerID;
    }
};

class CMsgGetCrossStatusRequest
{
public:
    CYH1MsgHeader m_yhHeader;
    CYH1MsgOperationAttr m_yhOprAttr;
    CYH1MsgObjectAttr m_yhObjAttr;
    std::string m_strCrossId; 

    CMsgGetCrossStatusRequest()
    {
    }

    void dump()
    {
        //std::cout << "CMsgGetCrossStatusRequest::dump:" << std::endl;
        m_yhHeader.dump();
        m_yhOprAttr.dump();
        m_yhObjAttr.dump();
        std::cout << "CrossId    :" << m_strCrossId     << std::endl;
    }

    /* for unit test */
    /* == */
    bool operator==(const CMsgGetCrossStatusRequest& obj) const
    {
        return this->m_yhHeader.m_strVersion==obj.m_yhHeader.m_strVersion
               && this->m_yhHeader.m_strType==obj.m_yhHeader.m_strType
               && this->m_strCrossId == obj.m_strCrossId;
    }
};

class CYH1MsgCrossStatus
{
public:
    std::string m_strCrossID;
    std::string m_strValue; //状态（见附表A.7）
    	
    CYH1MsgCrossStatus()
    {
        m_strCrossID = "";
        m_strValue = "";
    }

    void dump()
    {
        std::cout << "CrossID   :" << m_strCrossID   << std::endl;
        std::cout << "Value :" << m_strValue << std::endl;
    }
};

class CMsgGetCrossStatusResponse
{
public:
    CYH1MsgHeader m_yhHeader;
    CYH1MsgOperationAttr m_yhOprAttr;
    CYH1MsgObjectAttr m_yhObjAttr;

    //std::vector<CYH1MsgCrossStatus> m_v_yhCrossStatus;
    	CYH1MsgCrossStatus m_v_yhCrossStatus;

    CMsgGetCrossStatusResponse()
    {
        ;
    }

    void dump()
    {
        m_yhHeader.dump();
        m_yhOprAttr.dump();
        m_yhObjAttr.dump();
        m_v_yhCrossStatus.dump();

//        for (std::vector<CYH1MsgCrossStatus>::iterator it=m_v_yhCrossStatus.begin()
//                ; it!=m_v_yhCrossStatus.end()
//             ; it++)
//        {
//            it->dump();
//        }

    }
    /* for unit test */
    /* == */
    bool operator==(const CMsgGetCrossStatusResponse& obj) const
    {
        return this->m_yhHeader.m_strVersion==obj.m_yhHeader.m_strVersion
               && this->m_yhHeader.m_strType==obj.m_yhHeader.m_strType;
    }
};


class CMsgGetLaneParaRequest
{
public:
    std::string m_strHostName    ;

    CMsgGetLaneParaRequest()
    {
        m_strHostName    = "";
    }

    void dump()
    {
        //std::cout << "CMsgSystem::dump:" << std::endl;
        std::cout << "HostName    :" << m_strHostName     << std::endl;
    }
};

class CMsgGetLaneParaResponse
{
public:
    std::string m_strHostName    ;

    CMsgGetLaneParaResponse()
    {
        m_strHostName    = "";
    }

    void dump()
    {
        //std::cout << "CMsgSystem::dump:" << std::endl;
        std::cout << "HostName    :" << m_strHostName     << std::endl;
    }
};

class CMsgGetPhaseParaRequest
{
public:
    std::string m_strHostName    ;

    CMsgGetPhaseParaRequest()
    {
        m_strHostName    = "";
    }

    void dump()
    {
        //std::cout << "CMsgSystem::dump:" << std::endl;
        std::cout << "HostName    :" << m_strHostName     << std::endl;
    }
};

class CMsgGetPhaseParaResponse
{
public:
    std::string m_strHostName    ;

    CMsgGetPhaseParaResponse()
    {
        m_strHostName    = "";
    }

    void dump()
    {
        //std::cout << "CMsgSystem::dump:" << std::endl;
        std::cout << "HostName    :" << m_strHostName     << std::endl;
    }
};

class CMsgGetLampStatusRequest
{
public:
    std::string m_strHostName    ;

    CMsgGetLampStatusRequest()
    {
        m_strHostName    = "";
    }

    void dump()
    {
        //std::cout << "CMsgSystem::dump:" << std::endl;
        std::cout << "HostName    :" << m_strHostName     << std::endl;
    }
};

class CMsgGetLampStatusResponse
{
public:
    std::string m_strHostName    ;

    CMsgGetLampStatusResponse()
    {
        m_strHostName    = "";
    }

    void dump()
    {
        //std::cout << "CMsgSystem::dump:" << std::endl;
        std::cout << "HostName    :" << m_strHostName     << std::endl;
    }
};

class CMsgGetPlanParaRequest
{
public:
    std::string m_strHostName    ;

    CMsgGetPlanParaRequest()
    {
        m_strHostName    = "";
    }

    void dump()
    {
        //std::cout << "CMsgSystem::dump:" << std::endl;
        std::cout << "HostName    :" << m_strHostName     << std::endl;
    }
};
class CMsgGetPlanParaResponse
{
public:
    std::string m_strHostName    ;

    CMsgGetPlanParaResponse()
    {
        m_strHostName    = "";
    }

    void dump()
    {
        //std::cout << "CMsgSystem::dump:" << std::endl;
        std::cout << "HostName    :" << m_strHostName     << std::endl;
    }
};

class CMsgGetCrossStageRequest
{
public:
    std::string m_strHostName    ;

    CMsgGetCrossStageRequest()
    {
        m_strHostName    = "";
    }

    void dump()
    {
        //std::cout << "CMsgSystem::dump:" << std::endl;
        std::cout << "HostName    :" << m_strHostName     << std::endl;
    }
};
class CMsgGetCrossStageResponse
{
public:
    std::string m_strHostName    ;

    CMsgGetCrossStageResponse()
    {
        m_strHostName    = "";
    }

    void dump()
    {
        //std::cout << "CMsgSystem::dump:" << std::endl;
        std::cout << "HostName    :" << m_strHostName     << std::endl;
    }
};

class CMsgContorlLampRequest
{
public:
    std::string m_strHostName    ;

    CMsgContorlLampRequest()
    {
        m_strHostName    = "";
    }

    void dump()
    {
        //std::cout << "CMsgSystem::dump:" << std::endl;
        std::cout << "HostName    :" << m_strHostName     << std::endl;
    }
};
class CMsgContorlLampResponse
{
public:
    std::string m_strHostName    ;

    CMsgContorlLampResponse()
    {
        m_strHostName    = "";
    }

    void dump()
    {
        //std::cout << "CMsgSystem::dump:" << std::endl;
        std::cout << "HostName    :" << m_strHostName     << std::endl;
    }
};

class CMsgUpdateControlModeRequest
{
public:
    std::string m_strHostName    ;

    CMsgUpdateControlModeRequest()
    {
        m_strHostName    = "";
    }

    void dump()
    {
        //std::cout << "CMsgSystem::dump:" << std::endl;
        std::cout << "HostName    :" << m_strHostName     << std::endl;
    }
};
class CMsgUpdateControlModeResponse
{
public:
    std::string m_strHostName    ;

    CMsgUpdateControlModeResponse()
    {
        m_strHostName    = "";
    }

    void dump()
    {
        //std::cout << "CMsgSystem::dump:" << std::endl;
        std::cout << "HostName    :" << m_strHostName     << std::endl;
    }
};

#endif //_SmsMsg_H


/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * GRestHttpdSocket.h - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: GRestHttpdSocket.h 5884 2019-09-08 09:05:50Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2019-09-08 09:05:50  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#if !defined(_GRestHttpdSocket_H)
#define _GRestHttpdSocket_H

#include "HttpdSocket.h"

class CGRestHttpdSocket : public HttpdSocket
{
public:

    CGRestHttpdSocket(ISocketHandler& h);
    virtual ~CGRestHttpdSocket();

    /**
      * matchRequest是一个自定义头属性，表示该响应匹配哪个请求的。
      * 这在长连接异步请求响应时可以确保配对正确。
      */
    virtual void CreateHeader(size_t ContentLength, const std::string &matchRequest, const std::string &retCode/*="200"*/);
    virtual int SendMsg(const std::string &strMsg, const std::string &matchRequest/*=""*/, const std::string &retCode/*="200"*/);

    //测试接口不鉴权
    virtual int Do_reqHello(const std::string &strReq) ;
    //测试接口鉴权
    virtual int Do_reqEcho(const std::string &strReq) ;
    //详细信息接口-- 鉴权
    virtual int Do_reqDescribe(const std::string &strReq) ;
    //获取ip
    virtual int Do_reqIp(const std::string &strReq) ;

protected:
    // 服务器信息
    std::string m_strHttpHeader_Server;
    // Htp 返回码
    std::string m_strRetCode;

};

#endif // !defined(_GRestHttpdSocket_H)

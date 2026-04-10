/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * DataFacePss.h - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: DataFacePss.h 5884 2017-03-24 05:10:13Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2017-03-24 05:10:13  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#if !defined(_DataFacePss_H)
#define _DataFacePss_H

#include "acl_cpp/lib_acl.hpp"
#include "lib_acl.h"

static acl::string __keypre("pss");

class CDataFacePss// : public CEZObject
{
public:

    CDataFacePss(acl::string addr, int maxConn, int conn_timeout=5, int rw_timeout=1);
    virtual ~CDataFacePss();
    
    int getAppSecret(const std::string &strAppKey, std::string &strAppSecret);


private:
    acl::redis_client_pool *m_pPool;
};

#endif // !defined(_DataFacePss_H)

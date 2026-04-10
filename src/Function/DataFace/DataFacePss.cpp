/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * DataFacePss.cpp - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: DataFacePss.cpp 5884 2017-03-24 05:10:08Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2017-03-24 05:10:08  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>

#include "DataFacePss.h"

CDataFacePss::CDataFacePss(acl::string addr, int maxConn, int conn_timeout, int rw_timeout)
{
    m_pPool = new acl::redis_client_pool(addr.c_str(), maxConn);
    m_pPool->set_timeout(conn_timeout, rw_timeout);
}

CDataFacePss::~CDataFacePss()
{}

int CDataFacePss::getAppSecret(const std::string &strAppKey, std::string &strAppSecret)
{
    int iret = 0;
    acl::redis_client *conn = (acl::redis_client*) m_pPool->peek();

	std::cout << "getAppSecret:" << std::endl;

    if (conn==NULL)
    {
        return -1;
    }

    do
    {
        if (conn->eof())
        {

            acl::redis_connection redis_conn(conn);
            std::string passwd("64R0&P&aL1DhTDDTB#ZOFqTWZB67#V!s");
            if (redis_conn.auth(passwd.c_str()) == false)
            {
	std::cout << "auth failed." << std::endl;

                iret = -2;
                break;
                //return false;
            }
			
	std::cout << "auth OK, passwd:" << passwd << std::endl;

            acl::redis_hash redis_h;
            redis_h.set_client(conn);
            acl::string key;
            acl::string attr, value;

            //key.format("%s_%d", __keypre.c_str(), i);
            key = "AppKey";
            //attr.format("attr1");
            attr="701818788";
            //value.clear();

            redis_h.clear();
            if (redis_h.hget(key.c_str(), attr.c_str(), value) == false)
            {
                printf("failed hget key: %s, attr: %s\r\n", key.c_str(),
                       attr.c_str());

                iret = -3;
                break;
                //return false;
            }

            printf("hget key: %s, attr: %s -- %s \r\n", key.c_str(), attr.c_str(), value.c_str());
        }
    }
    while (0);

    // 回收连接对象
    m_pPool->put(conn, true);

    return iret;
}

#if defined(_FUNC_STANDALONE_UTIL)


int main(int argc, char *argv[])
{
    acl::acl_cpp_init();
	int iret;

    CDataFacePss df("libs.ezlibs.com:18099", 10);

    std::string strAppKey;
    std::string strAppSecret;
    iret = df.getAppSecret(strAppKey, strAppSecret);

    printf("iret:%d\n", iret);

    return 0;
}

#endif //_FUNC_STANDALONE_UTIL

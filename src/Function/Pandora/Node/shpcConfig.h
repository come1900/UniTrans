/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * shpcConfig.h - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: shpcConfig.h 5884 2018-04-20 02:39:12Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2018-04-20 02:39:12  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
#include <list>
#include <iostream>
#include "ezIniFile.h"

#define def_shpc_config_outIniFile "shpc.ini"

#define def_shpc_config_Key_Section_common       "common"
#define def_shpc_config_Key_item_server_addr     "server_addr"
#define def_shpc_config_Key_item_server_port     "server_port"
#define def_shpc_config_Key_item_privilege_token "privilege_token"
#define def_shpc_config_Key_item_tls_enable     "tls_enable"

#define def_shpc_config_Key_item_type            "type"
#define def_shpc_config_Key_item_local_ip        "local_ip"
#define def_shpc_config_Key_item_local_port      "local_port"
#define def_shpc_config_Key_item_remote_port     "remote_port"
#define def_shpc_config_Key_item_use_encryption     "use_encryption"
#define def_shpc_config_Key_item_use_compression     "use_compression"

class CshpService
{
public:
    std::string Section        ;

    std::string type           ;
    std::string local_ip       ;
    int local_port     ;
    int remote_port    ;

    int use_encryption     ;
    int use_compression     ;

    CshpService()
    {
        initial();
    }

    void initial()
    {
        Section     = "";
         type        = "";
         local_ip    = "";
         local_port  = 0;
         remote_port = 0;

         use_encryption = 3;
         use_compression = 3;
    }
};

class CshpcConfig
{
public:
    std::string server_addr     ;
    int server_port     ;
    std::string privilege_token ;

    int tls_enable     ;

    std::list<CshpService> shpServices;
    CshpService m_shpServiceTmp;

    std::string m_strFilePathName    ; // È«Ãû

    CshpcConfig();
    CshpcConfig(const std::string &strFilePathName);
    int Dump(std::string &strOut);
    // 
    bool bCfgValid();

    int Save(const std::string &strFilePathName="");
    int Load(const std::string &strFilePathName="");
};


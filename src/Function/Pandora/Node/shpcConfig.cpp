/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * shpcConfig.cpp - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: shpcConfig.cpp 5884 2018-04-20 02:39:22Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2018-04-20 02:39:22  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
#include <string.h>
#include <stdlib.h>

#include <list>
#include <iostream>

#include <str_opr.h>
#include <ini.h>

#include "shpcConfig.h"

//#define _DEBUG_THIS
#include <ez_def_devel_debug.h>

CshpcConfig::CshpcConfig()
{
    server_addr     = "";
    server_port     = 0;
    privilege_token = "";

    tls_enable     = 3;

    m_strFilePathName = def_shpc_config_outIniFile;
}

CshpcConfig::CshpcConfig(const std::string &strFilePathName)
{
    CshpcConfig();

    m_strFilePathName = strFilePathName;
}

int CshpcConfig::Dump(std::string &strOut)
{
    std::cout << __FUNCTION__ << " ------------------ " << m_strFilePathName   << std::endl; // 全名
    std::cout << "[" << "common"  << "]" << std::endl;
    std::cout << "\t server_addr=" << server_addr     << std::endl;
    std::cout << "\t server_port=" << server_port     << std::endl;
    std::cout << "\t privilege_token=" << privilege_token << std::endl;
    std::cout << "\t tls_enable=" << tls_enable      << std::endl;

    //std::list<CshpService> shpServices;
    for (std::list<CshpService>::const_iterator ci = shpServices.begin() ; ci != shpServices.end() ; ++ci)
    {
        std::cout << "[" << ci->Section  << "]" << std::endl;
        std::cout << "\t type=" << ci->type << std::endl;
        std::cout << "\t local_ip=" << ci->local_ip << std::endl;
        std::cout << "\t local_port=" << ci->local_port << std::endl;
        std::cout << "\t remote_port=" << ci->remote_port << std::endl;

        if (ci->use_encryption==1 || ci->use_encryption==0)
            std::cout << "\t use_encryption=" << ci->use_encryption << std::endl;
        if (ci->use_compression==1 || ci->use_compression==0)
            std::cout << "\t use_compression=" << ci->use_compression << std::endl;
    }

    return 0;
}

bool CshpcConfig::bCfgValid()
{
    if (server_addr.empty()
        || server_port <= 0
        || shpServices.empty())
    {
        return false;
    }

    return true;
}

int CshpcConfig::Save(const std::string &strFilePathName)
{
    CezIniFile __IniFile;

    if (strFilePathName.length()>0)
    {
        m_strFilePathName = strFilePathName;
    }

    int iret = 0;
    do
    {
        bool bExist=__IniFile.Open(m_strFilePathName, CezIniFile::EnOpenFileWrite);

        if(!bExist)
        {
            std::cout << "Can not open ini file:" << m_strFilePathName << std::endl;

            iret = -100;
            break;
        }

        char __dtBuf[GET_DATE_TIME_STRING_DEFAULT_STR_LEN] = {'\0'};
        bool bWriteCommentDt        = __IniFile.WriteComment(get_date_time_string(__dtBuf, def_EZ_DATE_TIME_STRING_FORMAT));
        bool bWriteCommentCp        = __IniFile.WriteComment("CshpcConfig.CezIniFile.ezlibs.com\n");
        if (!bWriteCommentDt || !bWriteCommentCp)
        {
            iret = -105;
            break;
        }

        bool bWriteSection_common        = __IniFile.WriteSection(def_shpc_config_Key_Section_common);
        if (!bWriteSection_common      )
        {
            iret = -101;
            break;
        }

        bool bWriteItem_server_addr      = __IniFile.WriteItem(def_shpc_config_Key_item_server_addr     , server_addr    );//"server_addr"
        if (!bWriteItem_server_addr    )
        {
            iret = -102;
            break;
        }
        bool bWriteItem_server_port      = __IniFile.WriteItem(def_shpc_config_Key_item_server_port     , server_port    );//"server_port"
        if (!bWriteItem_server_port    )
        {
            iret = -103;
            break;
        }
        bool bWriteItem_privilege_token  = __IniFile.WriteItem(def_shpc_config_Key_item_privilege_token , privilege_token);//"privilege_token"
        if (!bWriteItem_privilege_token)
        {
            iret = -104;
            break;
        }

        if (1==tls_enable || 2==tls_enable)
        {
            bool bWriteItem_tls_enable  = __IniFile.WriteItem(def_shpc_config_Key_item_tls_enable , 1==tls_enable?"true":"false");//"tls_enable"
            if (!bWriteItem_tls_enable)
            {
                iret = -105;
                break;
            }
        }

        // write services
        for (std::list<CshpService>::const_iterator ci = shpServices.begin() ; ci != shpServices.end() ; ++ci)
        {
            bool bWriteItem_blank             = __IniFile.WriteItem();//write blank line
            if (!bWriteItem_blank           )
            {
                iret = -111;
                break;
            }

            bool bWriteSection_ItemSection   = __IniFile.WriteSection(ci->Section);
            if (!bWriteSection_ItemSection )
            {
                iret = -112;
                break;
            }

            if (2==ci->use_encryption || 1==ci->use_encryption)
            {
                bool bWriteItem_use_encryption = __IniFile.WriteItem(def_shpc_config_Key_item_use_encryption , 1==ci->use_encryption?"true":"false");//"type"
                if (!bWriteItem_use_encryption           )
                {
                    iret = -113;
                    break;
                }
            }
            if (2==ci->use_compression || 1==ci->use_compression)
            {
                bool bWriteItem_use_compression = __IniFile.WriteItem(def_shpc_config_Key_item_use_compression , 1==ci->use_compression?"true":"false");//"type"
                if (!bWriteItem_use_compression           )
                {
                    iret = -114;
                    break;
                }
            }

            bool bWriteItem_type             = __IniFile.WriteItem(def_shpc_config_Key_item_type            , ci->type           );//"type"
            if (!bWriteItem_type           )
            {
                iret = -115;
                break;
            }
            bool bWriteItem_local_ip         = __IniFile.WriteItem(def_shpc_config_Key_item_local_ip        , ci->local_ip       );//"local_ip"
            if (!bWriteItem_local_ip       )
            {
                iret = -116;
                break;
            }
            bool bWriteItem_local_port       = __IniFile.WriteItem(def_shpc_config_Key_item_local_port      , ci->local_port     );//"local_port"
            if (!bWriteItem_local_port     )
            {
                iret = -117;
                break;
            }
            bool bWriteItem_remote_port      = __IniFile.WriteItem(def_shpc_config_Key_item_remote_port     , ci->remote_port    );//"remote_port"
            if (!bWriteItem_remote_port    )
            {
                iret = -118;
                break;
            }
        } // for services
    }
    while(0);

    __IniFile.Close();

    return iret;
}

static int callback_Load(void* user, const char* section, const char* name, const char* value)
{
    CshpcConfig *__pShpcCfg = (CshpcConfig *)user;

    //static char prev_section[128] = "";
    //static CshpService __shpService;
    static std::string strPrevSection = "";

    //if (strcmp(section, prev_section)) {
    if (section!=NULL
        && section[0] != '\0'
        && strPrevSection != section)
    {
        strPrevSection = section;
        DBG(
            printf("[%s]\n", strPrevSection.c_str());
        );

        if (__pShpcCfg!=NULL
            && !__pShpcCfg->m_shpServiceTmp.Section.empty()
            && __pShpcCfg->m_shpServiceTmp.Section != def_shpc_config_Key_Section_common
           )
        {
            __pShpcCfg->shpServices.push_back(__pShpcCfg->m_shpServiceTmp);
        }


        __pShpcCfg->m_shpServiceTmp.initial();
        __pShpcCfg->m_shpServiceTmp.Section = strPrevSection;

    }

    DBG(
        printf("%s = %s\n", name, value);
    );

    if (NULL != __pShpcCfg)
    {
        if (def_shpc_config_Key_Section_common == strPrevSection)
        {
            if (std::string(name) == def_shpc_config_Key_item_server_addr)
            {
                __pShpcCfg->server_addr = value;
            }
            else if (std::string(name) == def_shpc_config_Key_item_server_port)
            {
                __pShpcCfg->server_port = atoi(value);
            }
            if (std::string(name) == def_shpc_config_Key_item_privilege_token)
            {
                __pShpcCfg->privilege_token = value;
            }
            else if (std::string(name) == def_shpc_config_Key_item_tls_enable)
            {
                __pShpcCfg->tls_enable = atoi(value);
            }
            else
            {
                // what??
                //__trip;
            }

        }
        else
        {
            if (std::string(name) == def_shpc_config_Key_item_type)
            {
                __pShpcCfg->m_shpServiceTmp.type = value;
            }
            else if (std::string(name) == def_shpc_config_Key_item_local_ip)
            {
                __pShpcCfg->m_shpServiceTmp.local_ip = value;
            }
            else if (std::string(name) == def_shpc_config_Key_item_local_port)
            {
                __pShpcCfg->m_shpServiceTmp.local_port = atoi(value);
            }
            else if (std::string(name) == def_shpc_config_Key_item_remote_port)
            {
                __pShpcCfg->m_shpServiceTmp.remote_port = atoi(value);
            }
            else if (std::string(name) == def_shpc_config_Key_item_use_encryption)
            {
                __pShpcCfg->m_shpServiceTmp.use_encryption = (std::string(value)=="true" ? 1 : 0);
            }
            else if (std::string(name) == def_shpc_config_Key_item_use_compression)
            {
                __pShpcCfg->m_shpServiceTmp.use_compression = (std::string(value)=="true" ? 1 : 0);
            }
            else
            {
                // what??
                //__trip;
            }

        }//
    }//	if (NULL != __pShpcCfg)

    return 1;
}

int CshpcConfig::Load(const std::string &strFilePathName)
{
    if (strFilePathName.length()>0)
    {
        m_strFilePathName = strFilePathName;
    }

    int iret = 0;

    m_shpServiceTmp.initial();
    shpServices.clear();
    iret = ini_parse(m_strFilePathName.c_str(), callback_Load, this);

    if (iret < 0)
    {
        printf("Can't read '%s'!\n", m_strFilePathName.c_str());
        return -2;
    }
    //最后一个
    if (!m_shpServiceTmp.Section.empty()
        && m_shpServiceTmp.Section != def_shpc_config_Key_Section_common
       )
    {
        shpServices.push_back(m_shpServiceTmp);
    }
DBG(
    std::string strOut;
    this->Dump( strOut);
);
    return iret;
}

//
#ifdef _FUNC_STANDALONE_UTIL
/*
g++ -D_FUNC_STANDALONE_UTIL -I/home/wujj/libs/include/ezutil -I/home/wujj/libs/include/ezUtilCPP shpcConfig.cpp -L/home/wujj/libs/lib -lezUtilCPP-linux  -lezutil-linux -o shpcc
*/
int main(int argc, char *argv[])
{
    int iret;

    CshpcConfig shpConfig;

    shpConfig.server_addr      = "vsa.shp.ezlibs.com";
    shpConfig.server_port      = 18701;
    shpConfig.privilege_token  = "d9239799243904debc55fddb9107cc4fe58ee9bb";
    shpConfig.tls_enable      = 1;

    CshpService shpService;
    shpService.Section      = "G1751453539-22";
    shpService.type             = "tcp";//= tcp
    shpService.local_ip         = "127.0.0.1";//= 127.0.0.1
    shpService.local_port       = 22;//= 22
    shpService.remote_port      = 16286;//= 16286
    //shpService.use_encryption      = 1;//= 1,2
    //shpService.use_compression      = 1;//= 1,2
    shpConfig.shpServices.push_back(shpService);

    shpService.Section      = "G1751453539-80";
    shpService.type             = "tcp";//= tcp
    shpService.local_ip         = "127.0.0.1";//= 127.0.0.1
    shpService.local_port       = 80;//= 22
    shpService.remote_port      = 16287;//= 16286
    shpService.use_encryption      = 1;//= 1,0
    shpService.use_compression      = 1;//= 1,0
    shpConfig.shpServices.push_back(shpService);

    iret = shpConfig.Save();

    printf("shpConfig.Save:%d\n", iret);

    {
        CshpcConfig shpConfigLoad;
        printf("shpConfig.Load:%d\n", iret);
        shpConfigLoad.Load();
    }

    return 0;
}
#endif //#ifdef _FUNC_STANDALONE_UTIL

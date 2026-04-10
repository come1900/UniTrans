/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * mysql_addUser.cpp - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: mysql_addUser.cpp 5884 2017-06-19 09:17:33Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2017-06-19 09:17:33  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

// g++ -o addTerminal -I /usr/include/mysql/ -L /usr/lib/mysql -lmysqlclient mysql_addTerminal.cpp
// g++ -o addTerminal -I /usr/include/mysql/ -L /usr/lib64/mysql -lmysqlclient mysql_addTerminal.cpp
//
#include <stdio.h>
#include <string.h>
//#include <stdlib.h>

#include <string>
#include <vector>
#include <iostream>
#include <sstream> // std::string
#include "ez_util_platform_Exec.h"/*ez_Exec*/

//#include <strstream> // char

#include <mysql.h>
#include <str_opr.h>

#ifndef _DEBUG_THIS
//    #define _DEBUG_THIS
#endif
#ifdef _DEBUG_THIS
        #define DEB(x) x
        #define DBG(x) x
#else
        #define DEB(x)
        #define DBG(x)
#endif

#ifndef __trip
        #define __trip printf("-W-%d::%s(%d)\n", (int)time(NULL), __FILE__, __LINE__);
#endif
#ifndef __fline
        #define __fline printf("%s(%d)--", __FILE__, __LINE__);
#endif

#define ARG_USED(x) (void)&x;

//#define _REMONT_102_DBG_MYSQL_SVR

#include "mysql_addUser.h"
// 获取密码用
int myexec(const char *cmd, std::vector<std::string> &resvec)
{
    resvec.clear();

    FILE *pp = popen(cmd, "r"); //建立管道

    if (!pp)
    {
        return -1;
    }

    char tmp[1024]; //设置一个合适的长度，以存储每一行输出

    while (fgets(tmp, sizeof(tmp), pp) != NULL)
    {
        if (tmp[strlen(tmp) - 1] == '\n')
        {
            tmp[strlen(tmp) - 1] = '\0'; //去除换行符
        }
        resvec.push_back(tmp);
    }

    pclose(pp); //关闭管道

    return resvec.size();
}

int pst1_mysql_addUser(const char *p_Mp, const char *p_Passwd, const char *p_Area)
{
    int iret = 0;
    MYSQL *conn = NULL;

    /* db configure*/
    const char *server = DEF_PST1_DB_MYSQL_SERVER;
    const int  port =DEF_PST1_DB_MYSQL_PORT;
    const char *user = DEF_PST1_DB_MYSQL_USER;
    const char *password = DEF_PST1_DB_MYSQL_PASSWORD;
    const char *database = DEF_PST1_DB_MYSQL_DATABASE;

    do
    {
        conn = mysql_init(NULL);

        if( conn == NULL )
        {
            fprintf(stderr, "connect error: %s\n", mysql_error(conn));

            iret = -1;
            break;

        }

        /* Connect to database */
        if (!mysql_real_connect(conn, server,
                                user, password, database, port, NULL, 0))
        {

            fprintf(stderr, "connect error: %s\n", mysql_error(conn));

            iret = -2;
            break;

        }

        // 获取当前时间备用
        char *pBuf;
        char strBuffDateTime[GET_DATE_TIME_STRING_DEFAULT_STR_LEN]={'\0'};
        pBuf = get_date_time_string(strBuffDateTime, NULL);
        ARG_USED(pBuf);
        ARG_USED(strBuffDateTime);
        if (NULL == pBuf)
        {
            strcpy(strBuffDateTime, "2017-6-4 10:39:21"); 
            pBuf = strBuffDateTime; 
        }

        {
            std::stringstream __sqlInsert;// strstream::str 基于char*, sstream 基于std::string

            //__sqlInsert << "INSERT INTO `sys_user` (`ROW_ID`, `CREATETIME`, `UPdatetimeTIME`, `CREATEUSERID`, `UPdatetimeUSERID`, `DEPTID`, `DELFLAG`, `MOBILE`, `COMPANY_ID`, `ORDERCODE`, `BIRTHDAY`, `SEX`, `REALNAME`, `INITPASSWORD`, `LOGINNAME`, `DEPTCODE`, `LOGNNAME`, `USERTYPE`, `UPDATETIME`, `UPDATEUSERID`, `phone`, `FIRST_LOGIN_FLAG`, `SMSFLAG`, `MASTERID`, `DEVICETOKEN`, `APP`, `LOGINSMSFLAG`, `ISDIRECTFLOW`, `TYKD_LAST_LOGIN_TIME`, `TYKJ_LAST_LOGIN_TIME`, `SJKD_LAST_LOGIN_TIME`, `tykd_msg_push_flag`, `TYKJ_MSG_PUSH_FLAG`, `sjkd_msg_push_flag`, `AntiFalseAlarmFlag`, `ONLINEFLAG`, `userIconFileName`, `dayCaptureFlag`, `LOCALTURNIP`, `aliasLoginName`, `openID`, `weChatNo`, `smsStartTime`, `smsEndTime`, `smsInterval`) VALUES (md5('sys_user_"<< p_Mp <<"'), '2017-6-19 17:29:56', NULL, NULL, NULL, '402880e92db5d2ee012db601b2220004', 0, '"<< p_Mp <<"', md5('pa_company_"<< p_Mp <<"'), NULL, NULL, NULL, '"<< p_Mp <<"', 'NTcyNzQNDwIBBQx3WFA=', '"<< p_Mp <<"', NULL, NULL, '1', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 'N', '1', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);";
            __sqlInsert << "INSERT INTO `sys_user` (`ROW_ID`, `CREATETIME`, `UPdatetimeTIME`, `CREATEUSERID`, `UPdatetimeUSERID`, `DEPTID`, `DELFLAG`, `MOBILE`, `COMPANY_ID`, `ORDERCODE`, `BIRTHDAY`, `SEX`, `REALNAME`, `INITPASSWORD`, `LOGINNAME`, `DEPTCODE`, `LOGNNAME`, `USERTYPE`, `UPDATETIME`, `UPDATEUSERID`, `phone`, `FIRST_LOGIN_FLAG`, `SMSFLAG`, `MASTERID`, `DEVICETOKEN`, `APP`, `LOGINSMSFLAG`, `ISDIRECTFLOW`, `TYKD_LAST_LOGIN_TIME`, `TYKJ_LAST_LOGIN_TIME`, `SJKD_LAST_LOGIN_TIME`, `tykd_msg_push_flag`, `TYKJ_MSG_PUSH_FLAG`, `sjkd_msg_push_flag`, `AntiFalseAlarmFlag`, `ONLINEFLAG`, `userIconFileName`, `dayCaptureFlag`, `LOCALTURNIP`, `aliasLoginName`, `openID`, `weChatNo`, `smsStartTime`, `smsEndTime`, `smsInterval`) VALUES (md5('sys_user_"<< p_Mp <<"'), '"<< strBuffDateTime <<"', NULL, NULL, NULL, '402880e92db5d2ee012db601b2220004', 0, '"<< p_Mp <<"', md5('pa_company_"<< p_Mp <<"'), NULL, NULL, NULL, '"<< p_Mp <<"', 'NTcyNzQNDwIBBQx3WFA=', '"<< p_Mp <<"', NULL, NULL, '1', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 'N', '1', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);";
            DBG(
                std::cout << "Sql:[[[" << __sqlInsert.str() << "]]]" << std::endl;
            );
            int iRetInsert = mysql_query(conn, __sqlInsert.str().c_str());

            if (iRetInsert == 0)
            {
                DBG(
                    printf("insert ok.\n");
                );
                iret = 0;
            }
            else
            {
                fprintf(stderr, "%s\n", mysql_error(conn));

                iret = -4;
                break;
            }
        }
        {
            std::stringstream __sqlInsert;// strstream::str 基于char*, sstream 基于std::string
            //__sqlInsert << "INSERT INTO `pa_company` (`ROW_ID`, `CREATETIME`, `UPDATETIME`, `CREATEUSERID`, `UPDATEUSERID`, `DEPTID`, `DELFLAG`, `COMPANY_CODE`, `COMPANY_NAME`, `ADDRESS`, `TPYE`, `SOUECE`, `TERMINAL_COUNT`, `LINKMAN_NAME`, `MOBILEPHONE`, `EMAIL`, `SPACE`, `CENTER_STORAGE_FLAG`, `REMAINING_AMOUNT`, `ENABLE_DATE`, `DISABLE_DATE`, `REMARK`, `AREA_ID`, `ENABLE_FLAG`, `BROADBANDACCOUNT`, `MCUNUM`, `IMSI`, `FEE`, `SHAREFLAG`, `EXPIREDTIME`, `LASTCREATETIME`, `ISDATAUP`, `ESTORELICENCE`, `otherContact`, `SPACE_DAY`, `ID_NUMBER`, `accessToken`) VALUES (md5('pa_company_"<< p_Mp <<"'), '2017-6-19 17:29:56', NULL, NULL, NULL, '402880e92db5d2ee012db601b2220004', 0, '"<< p_Mp <<"', '"<< p_Mp <<"', NULL, '1', '2', NULL, NULL, '"<< p_Mp <<"', NULL, '1', 'Y', NULL, NULL, NULL, NULL, '402881e83ebb7c1d013ebb956f5a0004', 'Y', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);";
            __sqlInsert << "INSERT INTO `pa_company` (`ROW_ID`, `CREATETIME`, `UPDATETIME`, `CREATEUSERID`, `UPDATEUSERID`, `DEPTID`, `DELFLAG`, `COMPANY_CODE`, `COMPANY_NAME`, `ADDRESS`, `TPYE`, `SOUECE`, `TERMINAL_COUNT`, `LINKMAN_NAME`, `MOBILEPHONE`, `EMAIL`, `SPACE`, `CENTER_STORAGE_FLAG`, `REMAINING_AMOUNT`, `ENABLE_DATE`, `DISABLE_DATE`, `REMARK`, `AREA_ID`, `ENABLE_FLAG`, `BROADBANDACCOUNT`, `MCUNUM`, `IMSI`, `FEE`, `SHAREFLAG`, `EXPIREDTIME`, `LASTCREATETIME`, `ISDATAUP`, `ESTORELICENCE`, `otherContact`, `SPACE_DAY`, `ID_NUMBER`, `accessToken`) VALUES (md5('pa_company_"<< p_Mp <<"'), '"<< strBuffDateTime <<"', NULL, NULL, NULL, '402880e92db5d2ee012db601b2220004', 0, '"<< p_Mp <<"', '"<< p_Mp <<"', NULL, '1', '2', NULL, NULL, '"<< p_Mp <<"', NULL, '1', 'Y', NULL, NULL, NULL, NULL, '402881e83ebb7c1d013ebb956f5a0004', 'Y', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);";

            DBG(
                std::cout << "Sql:[[[" << __sqlInsert.str() << "]]]" << std::endl;
            );
            int iRetInsert = mysql_query(conn, __sqlInsert.str().c_str());

            if (iRetInsert == 0)
            {
                DBG(
                    printf("insert ok.\n");
                );
                iret = 0;
            }
            else
            {
                fprintf(stderr, "%s\n", mysql_error(conn));

                iret = -4;
                break;
            }
        }
    }
    while (0);


    if (conn)
    {
        mysql_close(conn);
    }

    return iret;
}

//
int pst1_mysql_modUser(const char *p_LoginName, const char *p_Passwd)
{
    int iret = 0;
    MYSQL *conn = NULL;

    /* db configure*/
    const char *server = DEF_PST1_DB_MYSQL_SERVER;
    const int  port =DEF_PST1_DB_MYSQL_PORT;
    const char *user = DEF_PST1_DB_MYSQL_USER;
    const char *password = DEF_PST1_DB_MYSQL_PASSWORD;
    const char *database = DEF_PST1_DB_MYSQL_DATABASE;

    do
    {
        conn = mysql_init(NULL);

        if( conn == NULL )
        {
            fprintf(stderr, "connect error: %s\n", mysql_error(conn));

            iret = -1;
            break;

        }

        /* Connect to database */
        if (!mysql_real_connect(conn, server,
                                user, password, database, port, NULL, 0))
        {

            fprintf(stderr, "connect error: %s\n", mysql_error(conn));

            iret = -2;
            break;

        }
        // 修改
        {

            std::string strCmd = std::string("TencPwd.sh ") + p_Passwd;
            std::vector<std::string> resvec;

            //int i_myexec_ret = myexec(strCmd.c_str(), resvec);
            int i_myexec_ret = ez_Exec(strCmd.c_str(), resvec);
            ARG_USED(i_myexec_ret);
            if (resvec.size()<1 || resvec[0].size()<2)
            {
                iret = -101;
                break;
            }
            else
            {

                fprintf(stderr, "get(%s):[%s]\n", p_Passwd, resvec[0].c_str());
            }

            std::stringstream __sqlInsert;// strstream::str 基于char*, sstream 基于std::string

            __sqlInsert << "UPDATE sys_user SET INITPASSWORD = '"<< resvec[0] <<"' WHERE LOGINNAME='"<< p_LoginName <<"';";
            //DBG(
            std::cout << "Sql:[[[" << __sqlInsert.str() << "]]]" << std::endl;
            //);
            int iRetInsert = mysql_query(conn, __sqlInsert.str().c_str());

            if (iRetInsert == 0)
            {
                DBG(
                    printf("insert ok.\n");
                );
                iret = 0;
            }
            else
            {
                fprintf(stderr, "%s\n", mysql_error(conn));

                iret = -4;
                break;
            }
        }

    }
    while (0);


    if (conn)
    {
        mysql_close(conn);
    }

    return iret;
}

#if defined(_FUNC_STANDALONE_UTIL)
int main(int argc, char *argv[])
{
    int iret = 0;
    iret = pst1_mysql_addUser("18963609582", "pwd", "suzhou");

    printf("pst1_mysql_addUser:%d\n", iret);

    return 0;
}
#endif //#if defined(_FUNC_STANDALONE_UTIL)

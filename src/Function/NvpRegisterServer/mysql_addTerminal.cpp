
// g++ -o addTerminal -I /usr/include/mysql/ -L /usr/lib/mysql -lmysqlclient mysql_addTerminal.cpp
// g++ -D_FUNC_STANDALONE_UTIL -D_REMONT_102_DBG_MYSQL_SVR -o addTerminal -I /usr/include/mysql/ -I/home/wujj/libs/include/ezutil/ -L/home/wujj/libs/lib/ -L /usr/lib64/mysql -lmysqlclient mysql_addTerminal.cpp -lezutil-linux
//

#include <stdio.h>
#include <string.h>
//#include <stdlib.h>

#include <string>
#include <iostream>
#include <sstream> // std::string

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
//
//int pst1_mysql_addTerminal(const char *p_devID, const char *p_devKey, const char *p_devModel);

#include "mysql_addTerminal.h"

#define PST1_MYSQL_DEVICE_ID_LEN 32
int pst1_mysql_addTerminal(const char *p_devID, const char *p_devKey, const char *p_devModel)
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


        /* send SQL query */
        char __bufSql[1024];
        snprintf(__bufSql, sizeof(__bufSql)-1, "SELECT ROW_ID from pa_device WHERE DCODE='%s';", p_devModel);

DBG(
        printf("__bufSql:%s\n", __bufSql);
);
        if (mysql_query(conn, __bufSql))
        {

            fprintf(stderr, "%s\n", mysql_error(conn));

            iret = -2;
            break;
        }

        // 获取该型号的id
        char __bufDEVICE_ID[PST1_MYSQL_DEVICE_ID_LEN+1] = {'\0'};
        MYSQL_RES *res = NULL;
        res = mysql_use_result(conn);
        if (res)
        {
            printf("p_devModel:%s", p_devModel);

            MYSQL_ROW row;
            //while ((row = mysql_fetch_row(res)) != NULL)
            if ((row = mysql_fetch_row(res)) != NULL)
            {
            	// 检查id长度
            	if (strlen(row[0])!=PST1_MYSQL_DEVICE_ID_LEN)
            	{
                    printf("row[0]:%s not valid.\n", row[0]);
                    iret = -3;
                    break;
            	}
            	
                strncpy(__bufDEVICE_ID, row[0], PST1_MYSQL_DEVICE_ID_LEN);
                printf("  %s\n", __bufDEVICE_ID);
                //printf("  %s\n", row[0]);
            }

            mysql_free_result(res);
        }
        
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

        std::stringstream __sqlInsert;// strstream::str 基于char*, sstream 基于std::string
        //OK_1 __sqlInsert << "INSERT INTO `pa_terminal` (`ROW_ID`, `CREATETIME`, `UPDATETIME`, `CREATEUSERID`, `UPDATEUSERID`, `DEPTID`, `DELFLAG`, `TCODE`, `TNAME`, `INSTALL_DATE`, `INSTALL_LOCATION`, `LONGITUDE`, `LATITUDE`, `PRODUCTION_DATE`, `VERSION`, `FIRM_ID`, `DEVICE_ID`, `AREA_ID`, `COMPANY_ID`, `ENABLE_FLAG`, `LocalIP`, `RtspLocalPort`, `TKEY`, `UpnpIP`, `RtspUpnpPort`, `NetType`, `ChannelNum`, `PTZFlag`, `Video`, `Channel`, `EStoreFlag`, `Onvif`, `EURL`, `EAccount`, `Epassword`, `FtpIP`, `FtpPort`, `FtpAccount`, `FtpPassword`, `TurnRelayIP`, `RtspTurnRelayPort`, `LocalStoreFlag`, `KY_TERMINAL_STORAGE_PORT`, `KY_DYNAMIC_IP_FLAG`, `KY_ALARM_VIDEO_UPLOAD_FLAG`, `KY_PLATFORM_STORAGE_FLAG`, `KY_TERMINAL_STORAGE_FLAG`, `Espace`, `DIDOAlarmFlag`, `infraredDetectionFlag`, `manualAlarmFlag`, `motionDetectionFlag`, `BACKUP_MINA`, `MAIN_MINA`, `USE_NUM`, `CHANNEL_NAMES`, `MAIN_TURNSERVER`, `BACKUP_TURNSERVER`, `DIDONUM`, `ERecordBucket`, `EPictureBucket`, `EActivateFlag`, `ESpaceLoopFlag`, `EOpenEstoreFlag`, `CURRENT_VERSION`, `FtpFileName`, `ISDATAUP`, `TSOURCE`, `HX_OSDSetFlag`, `HX_AlarmPloicyFlag`, `HX_AlarmPictureFlag`, `HX_AlarmRecordFlag`, `HX_VoicePlayFlag`, `HX_CloudStorage`, `HX_AlarmVoiceFlag`, `HX_AlarmLightFlag`, `HX_RTSPDownloadFlag`, `HX_HTTPDownloadFlag`, `DIDODesc`, `DIDOAlias`, `MINA_NODE`, `HX_FocusFlag`, `HX_PresetPointFlag`, `HX_CruiseFlag`, `HX_WatchPresetFlag`, `presetPointIds`, `cruiseIds`, `inUseCruise`, `HX_PtzSpeedFlag`, `broadbandAccount`, `HX_ImageInversionFlag`, `dataType`, `dataNum`, `SMSFLAG`, `estorelicence`, `cloudstoragetype`, `AntiFalseAlarmFlag`, `media_server_type`, `blackFlag`, `shareFlag`, `screenNum`, `spliteScreen`, `ESPACE_DAY`, `captureFileName`, `securityAlarmFlag`, `topFlag`, `devMsFlag`, `accessToken`) VALUES ('4ae6ee4b5c6e21af015c70f8a3370012', '2017-6-4 10:39:21', NULL, '', '', '402880e92db5d2ee012db601b2220004', 0, '"<< p_devID <<"', 'DeviceName', NULL, '', NULL, NULL, NULL, '', '4ae6e5994095832301409599dde60006', '"<< __bufDEVICE_ID <<"', '402881e43ecf58e2013ecf61194f0004', NULL, 'Y', NULL, NULL, '"<< p_devKey <<"', NULL, NULL, NULL, '1', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, '000000005c4f4267015c4f6034b50012', '000000005c4f4267015c4f6034b50012', NULL, NULL, '8a81486b44d2bcbe0144d2c8b3ba0003', '8a81486b44d2bcbe0144d2c8b3ba0003', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, NULL, NULL, NULL, NULL, '0', NULL, NULL, 0, NULL, 0, NULL, NULL, NULL, NULL, NULL);";
        //__sqlInsert << "INSERT INTO `pa_terminal` (`ROW_ID`, `CREATETIME`, `UPDATETIME`, `CREATEUSERID`, `UPDATEUSERID`, `DEPTID`, `DELFLAG`, `TCODE`, `TNAME`, `INSTALL_DATE`, `INSTALL_LOCATION`, `LONGITUDE`, `LATITUDE`, `PRODUCTION_DATE`, `VERSION`, `FIRM_ID`, `DEVICE_ID`, `AREA_ID`, `COMPANY_ID`, `ENABLE_FLAG`, `LocalIP`, `RtspLocalPort`, `TKEY`, `UpnpIP`, `RtspUpnpPort`, `NetType`, `ChannelNum`, `PTZFlag`, `Video`, `Channel`, `EStoreFlag`, `Onvif`, `EURL`, `EAccount`, `Epassword`, `FtpIP`, `FtpPort`, `FtpAccount`, `FtpPassword`, `TurnRelayIP`, `RtspTurnRelayPort`, `LocalStoreFlag`, `KY_TERMINAL_STORAGE_PORT`, `KY_DYNAMIC_IP_FLAG`, `KY_ALARM_VIDEO_UPLOAD_FLAG`, `KY_PLATFORM_STORAGE_FLAG`, `KY_TERMINAL_STORAGE_FLAG`, `Espace`, `DIDOAlarmFlag`, `infraredDetectionFlag`, `manualAlarmFlag`, `motionDetectionFlag`, `BACKUP_MINA`, `MAIN_MINA`, `USE_NUM`, `CHANNEL_NAMES`, `MAIN_TURNSERVER`, `BACKUP_TURNSERVER`, `DIDONUM`, `ERecordBucket`, `EPictureBucket`, `EActivateFlag`, `ESpaceLoopFlag`, `EOpenEstoreFlag`, `CURRENT_VERSION`, `FtpFileName`, `ISDATAUP`, `TSOURCE`, `HX_OSDSetFlag`, `HX_AlarmPloicyFlag`, `HX_AlarmPictureFlag`, `HX_AlarmRecordFlag`, `HX_VoicePlayFlag`, `HX_CloudStorage`, `HX_AlarmVoiceFlag`, `HX_AlarmLightFlag`, `HX_RTSPDownloadFlag`, `HX_HTTPDownloadFlag`, `DIDODesc`, `DIDOAlias`, `MINA_NODE`, `HX_FocusFlag`, `HX_PresetPointFlag`, `HX_CruiseFlag`, `HX_WatchPresetFlag`, `presetPointIds`, `cruiseIds`, `inUseCruise`, `HX_PtzSpeedFlag`, `broadbandAccount`, `HX_ImageInversionFlag`, `dataType`, `dataNum`, `SMSFLAG`, `estorelicence`, `cloudstoragetype`, `AntiFalseAlarmFlag`, `media_server_type`, `blackFlag`, `shareFlag`, `screenNum`, `spliteScreen`, `ESPACE_DAY`, `captureFileName`, `securityAlarmFlag`, `topFlag`, `devMsFlag`, `accessToken`) VALUES (md5('" << p_devID << "'), '2017-6-4 10:39:21', NULL, '', '', '402880e92db5d2ee012db601b2220004', 0, '"<< p_devID <<"', '"<< p_devID <</*DeviceName*/"', NULL, '', NULL, NULL, NULL, '', '4ae6e5994095832301409599dde60006', '"<< __bufDEVICE_ID <<"', '402881e43ecf58e2013ecf61194f0004', NULL, 'Y', NULL, NULL, '"<< p_devKey <<"', NULL, NULL, NULL, '1', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, '000000005c4f4267015c4f6034b50012', '000000005c4f4267015c4f6034b50012', NULL, NULL, '8a81486b44d2bcbe0144d2c8b3ba0003', '8a81486b44d2bcbe0144d2c8b3ba0003', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, NULL, NULL, NULL, NULL, '0', NULL, NULL, 0, NULL, 0, NULL, NULL, NULL, NULL, NULL);";
        // use strBuffDateTime
        __sqlInsert << "INSERT INTO `pa_terminal` (`ROW_ID`, `CREATETIME`, `UPDATETIME`, `CREATEUSERID`, `UPDATEUSERID`, `DEPTID`, `DELFLAG`, `TCODE`, `TNAME`, `INSTALL_DATE`, `INSTALL_LOCATION`, `LONGITUDE`, `LATITUDE`, `PRODUCTION_DATE`, `VERSION`, `FIRM_ID`, `DEVICE_ID`, `AREA_ID`, `COMPANY_ID`, `ENABLE_FLAG`, `LocalIP`, `RtspLocalPort`, `TKEY`, `UpnpIP`, `RtspUpnpPort`, `NetType`, `ChannelNum`, `PTZFlag`, `Video`, `Channel`, `EStoreFlag`, `Onvif`, `EURL`, `EAccount`, `Epassword`, `FtpIP`, `FtpPort`, `FtpAccount`, `FtpPassword`, `TurnRelayIP`, `RtspTurnRelayPort`, `LocalStoreFlag`, `KY_TERMINAL_STORAGE_PORT`, `KY_DYNAMIC_IP_FLAG`, `KY_ALARM_VIDEO_UPLOAD_FLAG`, `KY_PLATFORM_STORAGE_FLAG`, `KY_TERMINAL_STORAGE_FLAG`, `Espace`, `DIDOAlarmFlag`, `infraredDetectionFlag`, `manualAlarmFlag`, `motionDetectionFlag`, `BACKUP_MINA`, `MAIN_MINA`, `USE_NUM`, `CHANNEL_NAMES`, `MAIN_TURNSERVER`, `BACKUP_TURNSERVER`, `DIDONUM`, `ERecordBucket`, `EPictureBucket`, `EActivateFlag`, `ESpaceLoopFlag`, `EOpenEstoreFlag`, `CURRENT_VERSION`, `FtpFileName`, `ISDATAUP`, `TSOURCE`, `HX_OSDSetFlag`, `HX_AlarmPloicyFlag`, `HX_AlarmPictureFlag`, `HX_AlarmRecordFlag`, `HX_VoicePlayFlag`, `HX_CloudStorage`, `HX_AlarmVoiceFlag`, `HX_AlarmLightFlag`, `HX_RTSPDownloadFlag`, `HX_HTTPDownloadFlag`, `DIDODesc`, `DIDOAlias`, `MINA_NODE`, `HX_FocusFlag`, `HX_PresetPointFlag`, `HX_CruiseFlag`, `HX_WatchPresetFlag`, `presetPointIds`, `cruiseIds`, `inUseCruise`, `HX_PtzSpeedFlag`, `broadbandAccount`, `HX_ImageInversionFlag`, `dataType`, `dataNum`, `SMSFLAG`, `estorelicence`, `cloudstoragetype`, `AntiFalseAlarmFlag`, `media_server_type`, `blackFlag`, `shareFlag`, `screenNum`, `spliteScreen`, `ESPACE_DAY`, `captureFileName`, `securityAlarmFlag`, `topFlag`, `devMsFlag`, `accessToken`) VALUES (md5('" << p_devID << "'), '"<< strBuffDateTime <<"', NULL, '', '', '402880e92db5d2ee012db601b2220004', 0, '"<< p_devID <<"', '"<< p_devID <</*DeviceName*/"', NULL, '', NULL, NULL, NULL, '', '4ae6e5994095832301409599dde60006', '"<< __bufDEVICE_ID <<"', '402881e43ecf58e2013ecf61194f0004', NULL, 'Y', NULL, NULL, '"<< p_devKey <<"', NULL, NULL, NULL, '1', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, '000000005c4f4267015c4f6034b50012', '000000005c4f4267015c4f6034b50012', NULL, NULL, '8a81486b44d2bcbe0144d2c8b3ba0003', '8a81486b44d2bcbe0144d2c8b3ba0003', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, NULL, NULL, NULL, NULL, '0', NULL, NULL, 0, NULL, 0, NULL, NULL, NULL, NULL, NULL);";

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
    while (0);


    if (conn)
    {
        mysql_close(conn);
    }

    return iret;
}

// p_devOnline 0- offline, 1-online, 3-noThisDev, 4-unknow
// p_devBinded 0- not, 1-binded, 3-noThisDev, 4-unknow
int pst1_mysql_getTerminalInfo(const char *p_devID, const char *p_devKey, int *p_devOnline, int *p_devBinded)
{
    int iret = 0;
    MYSQL *conn = NULL;

    /* db configure*/
    const char *server = DEF_PST1_DB_MYSQL_SERVER;
    const int  port =DEF_PST1_DB_MYSQL_PORT;
    const char *user = DEF_PST1_DB_MYSQL_USER;
    const char *password = DEF_PST1_DB_MYSQL_PASSWORD;
    const char *database = DEF_PST1_DB_MYSQL_DATABASE;

    *p_devOnline = 4;
    *p_devBinded = 4;

    //
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

        /* bind info */
        char __bufSql[256];
        snprintf(__bufSql, sizeof(__bufSql)-1, "SELECT COMPANY_ID FROM pa_terminal WHERE TCODE='%s';", p_devID);

DBG(
        printf("__bufSql:%s\n", __bufSql);
);
        if (mysql_query(conn, __bufSql))
        {

            fprintf(stderr, "%s\n", mysql_error(conn));

            iret = -2;
            break;
        }

        MYSQL_RES *res = NULL;
        res = mysql_use_result(conn);
        if (res)
        {
DBG(
            printf("p_devID:%s\n", p_devID);
);

            MYSQL_ROW row;

            if ((row = mysql_fetch_row(res)) != NULL)
            {

                // not found
                if (row[0] ==NULL 
					|| strlen(row[0]) < 10)
                {
DBG(
                    printf("row[0]:%s not valid.\n", row[0]);
);
                    *p_devBinded = 0;
                    iret = -3;
                    //break;
                }
                else
                {
                    *p_devBinded = 1;
                    iret = 0;
                }

DBG(
                printf("*p_devBinded:%d\n", *p_devBinded);
);
            }
            // not found
            else
            {
DBG(
                printf("*mysql_fetch_row NULL\n");
);
                *p_devBinded = 3;

                iret = -3;
            }

            mysql_free_result(res);
        }
        else
        {
DBG(
            fprintf(stderr, "%s\n", mysql_error(conn));
);

            iret = -4;
            break;
        }
    }
    while (0); // 

    /* online info */
    do
    {

        char __bufSql[256];
        snprintf(__bufSql, sizeof(__bufSql)-1, "SELECT ONLINE_FLAG FROM pa_terminal_realtime WHERE TCODE='%s';", p_devID);

DBG(
        printf("__bufSql:%s\n", __bufSql);
);
        if (mysql_query(conn, __bufSql))
        {

            fprintf(stderr, "%s\n", mysql_error(conn));

            iret = -2;
            break;
        }

        MYSQL_RES *res = NULL;
        res = mysql_use_result(conn);
        if (res)
        {
DBG(
            printf("p_devID:%s\n", p_devID);
);

            MYSQL_ROW row;

            if ((row = mysql_fetch_row(res)) != NULL)
            {

                // not found
                if (row[0] ==NULL 
					|| strlen(row[0]) < 1)
                //if (row[0][0] == '\0')
                {

                    *p_devOnline = 3;
                    iret = -3;
                    break;
                }
            	
DBG(
                printf(" row[0][0] %c\n", row[0][0]);
);

                if (row[0][0]=='Y' ||row[0][0]=='y')
                    *p_devOnline = 1;
                else if(row[0][0]=='N' ||row[0][0]=='n')
                    *p_devOnline = 0;
                else// (row[0]=='Y' ||row[0]=='y')
                    *p_devOnline = 3;

DBG(
                printf("*p_devOnline:%d\n", *p_devOnline);
);
            }
            // not found
            else
            {
                *p_devOnline = 3;
DBG(
                printf("*mysql_fetch_row NULL\n");
);
                iret = -3;
            }

            mysql_free_result(res);
        }
        else
        {
DBG(
            fprintf(stderr, "%s\n", mysql_error(conn));
);

            iret = -4;
            break;
        }
    }
    while (0); // 

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

#if 0
    iret = pst1_mysql_addTerminal("81128215534", "ZYPS9CKQ", "DS-2CD-IPC-2829B-T1");

    printf("pst1_mysql_addTerminal:%d\n", iret);
#endif

    int TermOnline = -1;
    int TermBinded = -1;
    // 不存在
    iret = pst1_mysql_getTerminalInfo("81147350326", "ZYPS9CKQ", &TermOnline, &TermBinded);
    printf("pst1_mysql_getTerminalInfo:%d, %d, %d\n", iret, TermOnline, TermBinded);

    // y
    TermOnline = -1;
    TermBinded = -1;
    iret = pst1_mysql_getTerminalInfo("81100007186", "ZYPS9CKQ", &TermOnline, &TermBinded);
    printf("pst1_mysql_getTerminalInfo:%d, %d, %d\n", iret, TermOnline, TermBinded);

    return 0;
}
#endif //#if defined(_FUNC_STANDALONE_UTIL)

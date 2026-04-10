/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * GeminiKeyCreator.cpp - brief
 *
 * Copyright (C) 2013 ezlibs.com, All Rights Reserved.
 *
 * $Id: GeminiKeyCreator.cpp 5884 2013-08-29 03:14:43Z WuJunjie $
 *
 *  Notes:
 *     -
 *      explain
 *     -
 *
 *  Update:
 *     2013-08-29 03:14:43 WuJunjie Create
 *     2013-09-26 10:46:37 WuJunjie add colum para
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#include <string.h>
#ifdef LINUX
#include <sys/time.h> /*gettimeofday*/

#include <getopt.h>
#include <unistd.h>
#endif //LINUX

#include <iostream>
#include <vector>

#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/err.h>
#include <sqlite3.h>

#include <str_opr.h>
#include <base64.h>
#include <md5.h>
#include <ez_system_api.h>

#include "GeminiKeyStack.h"

#ifndef _DEBUG_THIS
// #define _DEBUG_THIS
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

#define DEFAULT_COLUM_PER_FILE 10000*100

//GEMINI_KEY_LEN_BYTES "4096"
//
#define CREATE_TABLE_SQL "CREATE TABLE IF NOT EXISTS GeminiKey("  	   \
                      "ID             INT                       ," \
                      "dtCreate       DATETIME          NOT NULL," \
                      "keyLock        VARCHAR(4096)     NOT NULL," \
                      "keyUnlock      VARCHAR(4096)     NOT NULL," \
                      "UID            INT                       ," \
                      "dtUsed         DATETIME          NOT NULL," \
                      "CID            INT );"

/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

void signal_proc(int signo);
void this_usage(int argc, char *argv[]);

/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

static int tutorial_callback(void *NotUsed, int argc, char **argv, char **azColName)
{
    int i;
    for(i=0; i<argc; i++)
    {
        printf("%s = %s ", azColName[i], argv[i] ? argv[i] : "NULL");
    }
    printf("\n");
    return 0;
}
//ret SQLITE_OK when succeeded
int OpenAndCreateDB(sqlite3 **db, char *fname);

void testGeminiKey();
//int createGeminiKey(bool bForceNewDb/**/);
int createGeminiKey(int iColum, bool bForceNewDb/**/, bool bDumpKey);

#define CPPUNIT_ASSERT(x)

int main(int argc, char *argv[])
{
    int c;
    bool bShowHelp = false;
    bool bDumpKey = false;
    char *dbColum = NULL;
    int idbColum = DEFAULT_COLUM_PER_FILE;

    while ((c = getopt (argc, argv, "dhc:")) != -1)
    {
        switch (c)
        {
            case 'd':
            {
                bDumpKey = true;
                break;
            }
            case 'h':
            {
                bShowHelp = true;
                break;
            }
            case 'c':
            {
                dbColum = optarg;
                idbColum = atoi(dbColum);
                if (idbColum<0)
                {
                    idbColum = DEFAULT_COLUM_PER_FILE;
                }

                break;
            }
            case '?':
            {
                if (optopt == 'c')
                    fprintf (stderr, "Option -%c requires an argument.\n", optopt);
                else if (isprint (optopt))
                    fprintf (stderr, "Unknown option `-%c'.\n", optopt);
                else
                    fprintf (stderr,
                             "Unknown option character `\\x%x'.\n",
                             optopt);
                return 1;
            }
            default:
            {
                // abort ();
            }
        }
    }

    //    DBG(
    printf("\n\n\n");
    __fline;
    printf ("idbColum = %d, bShowHelp = %d, bDumpKey = %d\n",
            idbColum, bShowHelp, bDumpKey);
    //    );

    if (bShowHelp)
    {
        this_usage(argc, argv);
        return 0;
    }

    struct timeval tv_main_start;
    struct timeval tv_start;
    struct timeval tv_now;
    struct timeval t_sub_time;
    gettimeofday(&tv_main_start, NULL);

    for(unsigned int iTimes=0;        ;iTimes++)
    {
        std::cout << ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Runing : " << iTimes << std::endl;

        gettimeofday(&tv_start, NULL)        ;

        if ( idbColum>0
             && (iTimes%idbColum==0) )
        {
            createGeminiKey(idbColum, true, bDumpKey);
        }
        else
        {
            //createGeminiKey(false);
            createGeminiKey(idbColum, false, bDumpKey);
        }

        gettimeofday(&tv_now, NULL);
        ez_timeval_subtract(&t_sub_time, &tv_now, &tv_start);

        struct timeval tv_tmp_elipse;
        //gettimeofday(&tv_now, NULL);
        ez_timeval_subtract(&tv_tmp_elipse, &tv_now, &tv_main_start);

        //__fline;
        printf("\t createGeminiKey used:%5d(ms), runed:%15d(ms)\n"
               ,  (int)(t_sub_time.tv_sec*1000 + t_sub_time.tv_usec / 1000.0)
               ,  (int)(tv_tmp_elipse.tv_sec*1000 + tv_tmp_elipse.tv_usec / 1000.0));

        //Lock.dump();
        //Unlock.dump();
    }
    return 0;
}

#define GEMINIKEY_CREAT_DB_NAME "geminikey-%s.db3"
int OpenAndCreateDB(sqlite3 **db, char *fname)
{
    int rc;
    char *zErrMsg = 0;
    //char *sql;


    //assert( sqlite3_libversion_number()==SQLITE_VERSION_NUMBER );
    /* Open database */
    DBG(
        fprintf(stdout, "Open database:%s ", fname);
    );
    rc = sqlite3_open(fname, db);
    if( rc )
    {
        fprintf(stderr, "failed: %s\n", sqlite3_errmsg(*db));
        return -1;
        //exit(0);
    }
    else
    {
        DBG(
            fprintf(stdout, "succeeded.\n");
        );
    }

    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
    // "keyUnlock      varchar(2048)     NOT NULL,"
    // 2014-11-6 9:14:43 to 4096 wujj

    /* Create SQL statement */
#if 0
    const char *sql = "CREATE TABLE IF NOT EXISTS GeminiKey("  	   \
                      "ID             INT                       ," \
                      "dtCreate       DATETIME          NOT NULL," \
                      "keyLock        VARCHAR(4096)     NOT NULL," \
                      "keyUnlock      VARCHAR(4096)     NOT NULL," \
                      "UID            INT                       ," \
                      "dtUsed         DATETIME          NOT NULL," \
                      "CID            INT );";
#endif //

    const char *sql = CREATE_TABLE_SQL;

    //printf("CREATE_TABLE_SQL:%s\n", sql);

    /* Execute SQL statement */
    rc = sqlite3_exec(*db, sql, NULL/*tutorial_callback*/, 0, &zErrMsg);
    if( rc != SQLITE_OK )
    {
        fprintf(stderr, "SQL error(%d): %s\n", rc, zErrMsg);
        sqlite3_free(zErrMsg);
        sqlite3_close(*db);

        return -2;
    }
    else
    {
        DBG(
            fprintf(stdout, "Table created successfully\n");
        );
    }

    return SQLITE_OK;
}

int generateGeminiKeyDBFileName(char *fname, int len)
{
    char dtNow[GET_DATE_TIME_STRING_DEFAULT_STR_LEN+1];
    get_date_time_string(dtNow, "%04d%02d%02d%02d%02d%02d");

    return snprintf(fname, len, GEMINIKEY_CREAT_DB_NAME, dtNow);
}

int createGeminiKey(int iColum, bool bForceNewDb/**/, bool bDumpKey)
{
    const std::string strPID = "13359211641994461155";

    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

    CGeminiKeyStack __CGeminiKeyStackCreate;
    CGeminiKeyStack __CGeminiKeyStackLock;
    CGeminiKeyStack __CGeminiKeyStackUnlock;

    GeminiKey Lock;
    GeminiKey Unlock;

    __CGeminiKeyStackCreate.GeminiKeyCreate(Lock, Unlock, "13359211641994461155", GEMINI_KEY_BITS);
    //__CGeminiKeyStackCreate.GeminiKeyCreate(Lock, Unlock, strPID, "", "gminitech.com", GEMINI_KEY_BITS/*2048*/);

    GeminiKeys __KeysLock;
    __KeysLock.m_vecGeminiKey.push_back(Lock);
    __CGeminiKeyStackLock.Encode(__KeysLock);

    GeminiKeys __KeysUnlock;
    __KeysUnlock.m_vecGeminiKey.push_back(Unlock);
    __CGeminiKeyStackUnlock.Encode(__KeysUnlock);

    //std::string strLock = __CGeminiKeyStackLock.ToString();

    // ∑¢––∞ÊKey
    std::string strExportLock;
    __CGeminiKeyStackLock.GeminiKeyExport(strExportLock);

    std::string strExportUnlock;
    __CGeminiKeyStackUnlock.GeminiKeyExport(strExportUnlock);

    //#ifndef CPP_UNIT
#if 1
    if (bDumpKey)
    {
        __fline;
        std::cout << "GeminiKey Created:" << std::endl;
        std::cout << "--------------------------------------------- strExportLock(" << strExportLock.size() << ")" << std::endl;
        std::cout << strExportLock << std::endl;
        std::cout << "--------------------------------------------- strExportUnlock(" << strExportUnlock.size() << ")" << std::endl;
        std::cout << strExportUnlock << std::endl;
        std::cout << "---------------------------------------------" << std::endl;
    }
#endif

#if 1// save to db
    if (iColum>0)
    {

        sqlite3 *db=NULL;
        char *zErrMsg = 0;
        int  rc;
        char *sql=NULL;

        /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
        static char fname[GET_DATE_TIME_STRING_DEFAULT_STR_LEN+16]={'\0'};

        if ( fname[0]=='\0'
             || bForceNewDb )
        {
            int iret = generateGeminiKeyDBFileName(fname, sizeof(fname));

            printf("\t generateGeminiKeyDBFileName(%d):%s\n", iret, fname);
        }

        rc = OpenAndCreateDB(&db, fname);
        if( rc != SQLITE_OK )
        {
            fprintf(stderr, "OpenAndCreateDB failed.\n");
            return -1;
        }
        /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
        /* Create SQL statement */
        //    sql = "INSERT INTO GeminiKey (ID       ,dtCreate ,keyLock  ,keyUnlock,UID      ,dtUsed   ,CID ) "
        //          "VALUES (%d, '%s', '%s', '%s', %d, '%s', %d ); ";
        //"VALUES (1, '2013-09-14 11:55:33', 'Lock1', 'Unlock1', 0, '2013-09-14 11:56:30', 7 ); ";

        int IDInit = 0;
        char dtCreate[GET_DATE_TIME_STRING_DEFAULT_STR_LEN+1];
        // lock
        //ulock
        int iUidIni = 0;
        const char *dtUsed = "2013-08-28 00:00:00";
        int iCID = 7;

        //char dtCreate[GET_DATE_TIME_STRING_DEFAULT_STR_LEN+1];
        //get_date_time_string(dtCreate, NULL);

        char strcSqlSaveGeminiKey[2048*3];
        //std::string strSqlSaveGeminiKey;
        snprintf(strcSqlSaveGeminiKey, sizeof(strcSqlSaveGeminiKey)
                 , "INSERT INTO GeminiKey (ID, dtCreate, keyLock, keyUnlock, UID, dtUsed, CID) VALUES (%d, '%s', '%s', '%s', %d, '%s', %d ); "
                 , IDInit
                 , dtCreate
                 , strExportLock.c_str()
                 , strExportUnlock.c_str()
                 , iUidIni
                 , dtUsed
                 , iCID);
        sql = strcSqlSaveGeminiKey;
        //std::cout << "--------------------------------------------- strcSqlSaveGeminiKey(" << strlen(strcSqlSaveGeminiKey) << ")" << std::endl;
        //std::cout << strcSqlSaveGeminiKey << std::endl;

        /* Execute SQL statement */
        rc = sqlite3_exec(db, sql, tutorial_callback, 0, &zErrMsg);
        if( rc != SQLITE_OK )
        {
            fprintf(stderr, "SQL error: %s\n", zErrMsg);
            sqlite3_free(zErrMsg);
        }
        else
        {
            DBG(
                fprintf(stdout, "Records created successfully\n");
            );
        }
        /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
        sqlite3_close(db);

    }
#endif // save to db

    return 0;

}

const char * const copy_right = "@(#) Copyright (c) 2013 gminiTech.com.  All rights reserved";
const char * const desc="P_2013-09-25.GeminiKey.1010";

void ShowVersion()
{
    printf("---------------------------\n\n");
    printf("%s\n", copy_right);
    //printf("%s\n", desc);
    printf("$ GeminiLock: %s,v 2.6 %s %s Exp $\n\n", __FILE__, __DATE__, __TIME__);
    printf("---------------------------\n");
}

void this_usage(int argc, char *argv[])
{
    ShowVersion();

    printf ("Usage: %s [-hc]\n\n", argv[0]);

    printf ("Create GeminiKey.\n\n");

    printf ("Options:\n");
    printf ("\t -c      <dbcolum> db max colum\n");
    printf ("\t -d      dump key\n");
    printf ("\t -h      help ...\n");
    printf ("\n");
}


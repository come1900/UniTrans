/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * GeminiKeyPerformance.cpp - brief
 *
 * Copyright (C) 2013 ezlibs.com, All Rights Reserved.
 *
 * $Id: GeminiKeyPerformance.cpp 5884 2014-01-27 11:16:10Z WuJunjie $
 *
 *  Notes:
 *     -
 *      创建 key 的 性能测试
 *     -
 *
 *  Update:
 *     2014-01-27 11:16:10 WuJunjie Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#include <string.h>
#ifdef LINUX
#include <sys/time.h> /*gettimeofday*/
#endif //LINUX

#include <iostream>
#include <vector>
#include <signal.h>

#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/err.h>

#include "GeminiKeyStack.h"

#include <str_opr.h>
#include <base64.h>
#include <md5.h>
#include <ez_system_api.h>

#ifndef _DEBUG_THIS
 #define _DEBUG_THIS
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


#include <sqlite3.h>


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
int OpenAndCreateDB(sqlite3 **db);

void testGeminiKey();
int createGeminiKey();

#define CPPUNIT_ASSERT(x)

	unsigned int total_createDuration = 0;// 创建key耗费时间
	unsigned int total_createNumber = 0;// 创建个数
	unsigned int total_runedDuration = 0;//程序运行时间

void signal_proc(int signo);

int main(int argc, char *argv[])
{
    struct timeval tv_main_start;
    struct timeval tv_start;
    struct timeval tv_now;
    struct timeval t_sub_time;
    
    signal(SIGPIPE, signal_proc);

    signal(SIGTERM, signal_proc);
    signal(SIGKILL, signal_proc);
    signal(SIGQUIT, signal_proc);
    signal(SIGINT, signal_proc);
    
    gettimeofday(&tv_main_start, NULL);

    for(int iTimes=0;;iTimes++)
    {
        std::cout << ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Run iTimes:" << iTimes << std::endl;

        gettimeofday(&tv_start, NULL);
        //testGeminiKey();
        createGeminiKey();
        total_createNumber++;

        gettimeofday(&tv_now, NULL);
        ez_timeval_subtract(&t_sub_time, &tv_now, &tv_start);
        total_createDuration+=(int)(t_sub_time.tv_sec*1000 + t_sub_time.tv_usec / 1000.0);

        struct timeval tv_tmp_elipse;
        //gettimeofday(&tv_now, NULL);
        ez_timeval_subtract(&tv_tmp_elipse, &tv_now, &tv_main_start);
        total_runedDuration = (int)(tv_tmp_elipse.tv_sec*1000 + tv_tmp_elipse.tv_usec / 1000.0);

        __fline;
//        printf("testGeminiKey used:%5d(ms), runed:%15d(ms)\n"
//               ,  (int)(t_sub_time.tv_sec*1000 + t_sub_time.tv_usec / 1000.0)
//               ,  (int)(tv_tmp_elipse.tv_sec*1000 + tv_tmp_elipse.tv_usec / 1000.0));
        printf("testGeminiKey used:%5d(ms), runed:%15d(ms)\n"
               ,  (int)(t_sub_time.tv_sec*1000 + t_sub_time.tv_usec / 1000.0)
               ,  total_runedDuration);

        //Lock.dump();
        //Unlock.dump();
    }
    return 0;
}


void testGeminiKey()
{
    CGeminiKeyStack __CGeminiKeyStackCreate;
    CGeminiKeyStack __CGeminiKeyStackLock;
    CGeminiKeyStack __CGeminiKeyStackUnlock;

    GeminiKey Lock;
    GeminiKey Unlock;

    //__CGeminiKeyStackCreate.GeminiKeyCreate(Lock, Unlock, "13359211641994461155", 1024);
    __CGeminiKeyStackCreate.GeminiKeyCreate(Lock, Unlock, "13359211641994461155", 2048);

    GeminiKeys __KeysLock;
    __KeysLock.m_vecGeminiKey.push_back(Lock);
    __CGeminiKeyStackLock.Encode(__KeysLock);

    GeminiKeys __KeysUnlock;
    __KeysUnlock.m_vecGeminiKey.push_back(Unlock);
    __CGeminiKeyStackUnlock.Encode(__KeysUnlock);

    std::string strLock = __CGeminiKeyStackLock.ToString();

    // 发行版Key
    std::string strExportLock;
    __CGeminiKeyStackLock.GeminiKeyExport(strExportLock);

    std::string strExportUnlock;
    __CGeminiKeyStackUnlock.GeminiKeyExport(strExportUnlock);

#ifndef CPP_UNIT

    __fline;
    std::cout << "GeminiKey Created:" << std::endl;
    std::cout << "--------------------------------------------- strExportLock(" << strExportLock.size() << ")" << std::endl;
    std::cout << strExportLock << std::endl;
    std::cout << "--------------------------------------------- strExportUnlock(" << strExportUnlock.size() << ")" << std::endl;
    std::cout << strExportUnlock << std::endl;
    std::cout << "---------------------------------------------" << std::endl;
#endif

    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

    // 使用
    CGeminiKeyStack __CGeminiKeyStackGenLock;
    __CGeminiKeyStackGenLock.Parse(strLock);
    GeminiKeys __KeysLockBack;
    __CGeminiKeyStackGenLock.Decode(__KeysLockBack);
    // 用例 —— encode 和decode接口正常
    CPPUNIT_ASSERT(__CGeminiKeyStackLock.ToString() == __CGeminiKeyStackGenLock.ToString() );

    CGeminiKeyStack CGeminiKeyStack_testImportLock;
    CGeminiKeyStack CGeminiKeyStack_testImportUnlock;

    CGeminiKeyStack_testImportLock.GeminiKeyImport(strExportLock);
    CGeminiKeyStack_testImportUnlock.GeminiKeyImport(strExportUnlock);

    GeminiKeys __KeysGenLock;
    CGeminiKeyStack_testImportLock.Decode(__KeysGenLock);
    GeminiKeys __KeysGenUnlock;
    CGeminiKeyStack_testImportUnlock.Decode(__KeysGenUnlock);

    // 恢复成功 gemini_KeyExport vs gemini_KeyImport
    CPPUNIT_ASSERT(__KeysLock == __KeysGenLock);
    CPPUNIT_ASSERT(__KeysUnlock == __KeysGenUnlock);

#if 0

    __KeysLock.dump();
    __KeysGenLock.dump();
    __KeysUnlock.dump();
    __KeysGenUnlock.dump();
#endif

    //锁门
    CGeminiKeyStack CGeminiKeyStack_testLockLock;
    CGeminiKeyStack CGeminiKeyStack_testImportLockUnlock;

    std::string PlainText;
    std::string LockedData;

    // 检查并校验数据
    bool bRet;
    bRet = __KeysGenLock.checkVerifyCode();
    CPPUNIT_ASSERT(bRet == true);
    bRet = __KeysGenUnlock.checkVerifyCode();
    CPPUNIT_ASSERT(bRet == true);

    int iret = 0;
    // use
    iret = CGeminiKeyStack_testLockLock.GeminiLock(__KeysGenLock, PlainText, LockedData);
    CPPUNIT_ASSERT(iret == 0);
#ifndef CPP_UNIT

    __fline;
    std::cout << "Locked:" << (iret==0?"Succeeded":"Failed") << std::endl;
    std::cout << "--------------------------------------------- PlainText(" << PlainText.size() << ")" << std::endl;
    std::cout << PlainText << std::endl;
    std::cout << "--------------------------------------------- LockedData(" << LockedData.size() << ")"  << std::endl;
    std::cout << LockedData << std::endl;
    std::cout << "---------------------------------------------" << std::endl;
#endif

    // 开锁
    iret = CGeminiKeyStack_testImportLockUnlock.GeminiUnlock(__KeysGenUnlock, PlainText, LockedData);
    CPPUNIT_ASSERT(iret == 0);
#ifndef CPP_UNIT

    __fline;
    std::cout << "Unlocked:" << (iret==0?"Succeeded":"Failed")    << std::endl;
#endif

}


#define GEMINIKEY_CREAT_DB_NAME "geminikey-%s.db3"
int OpenAndCreateDB(sqlite3 **db)
{
    int rc;
    char *zErrMsg = 0;
    //char *sql;

    char dtNow[GET_DATE_TIME_STRING_DEFAULT_STR_LEN+1];
    get_date_time_string(dtNow, "%04d%02d%02d%02d%02d%02d");

    static char fname[GET_DATE_TIME_STRING_DEFAULT_STR_LEN+16]={0};
    if (strlen(fname)<2)
    {
        snprintf(fname, sizeof(fname), GEMINIKEY_CREAT_DB_NAME, dtNow);
    }

    //assert( sqlite3_libversion_number()==SQLITE_VERSION_NUMBER );
    /* Open database */
    fprintf(stdout, "Open database:%s ", fname);
    rc = sqlite3_open(fname, db);
    if( rc )
    {
        fprintf(stderr, "failed: %s\n", sqlite3_errmsg(*db));
        return -1;
        //exit(0);
    }
    else
    {
        fprintf(stdout, "succeeded.\n");
    }

    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
    /* Create SQL statement */
    const char *sql = "CREATE TABLE IF NOT EXISTS GeminiKey("  \
                      "ID             INT                       ," \
                      "dtCreate       datetime          NOT NULL," \
                      "keyLock        varchar(2048)     NOT NULL," \
                      "keyUnlock      varchar(2048)     NOT NULL," \
                      "UID            INT                       ," \
                      "dtUsed         datetime          NOT NULL," \
                      "CID            INT );";

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
        fprintf(stdout, "Table created successfully\n");
    }

    return SQLITE_OK;
}



int createGeminiKey()
{
    const std::string strPID = "13359211641994461155";

#if 0
    sqlite3 *db=NULL;
    int  rc;

    rc = OpenAndCreateDB(&db);
    if( rc != SQLITE_OK )
    {
        fprintf(stderr, "OpenAndCreateDB failed.\n");
        return -1;
    }
#endif
    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

    CGeminiKeyStack __CGeminiKeyStackCreate;
    CGeminiKeyStack __CGeminiKeyStackLock;
    CGeminiKeyStack __CGeminiKeyStackUnlock;

    GeminiKey Lock;
    GeminiKey Unlock;

    //__CGeminiKeyStackCreate.GeminiKeyCreate(Lock, Unlock, "13359211641994461155", 1024);
    __CGeminiKeyStackCreate.GeminiKeyCreate(Lock, Unlock, strPID, 2048);

    GeminiKeys __KeysLock;
    __KeysLock.m_vecGeminiKey.push_back(Lock);
    __CGeminiKeyStackLock.Encode(__KeysLock);

    GeminiKeys __KeysUnlock;
    __KeysUnlock.m_vecGeminiKey.push_back(Unlock);
    __CGeminiKeyStackUnlock.Encode(__KeysUnlock);

    std::string strLock = __CGeminiKeyStackLock.ToString();

    // 发行版Key
    std::string strExportLock;
    __CGeminiKeyStackLock.GeminiKeyExport(strExportLock);

    std::string strExportUnlock;
    __CGeminiKeyStackUnlock.GeminiKeyExport(strExportUnlock);

    //#ifndef CPP_UNIT
#if 0

    __fline;
    std::cout << "GeminiKey Created:" << std::endl;
    std::cout << "--------------------------------------------- strExportLock(" << strExportLock.size() << ")" << std::endl;
    std::cout << strExportLock << std::endl;
    std::cout << "--------------------------------------------- strExportUnlock(" << strExportUnlock.size() << ")" << std::endl;
    std::cout << strExportUnlock << std::endl;
    std::cout << "---------------------------------------------" << std::endl;
#endif

// 写入数据库
#if 0
    char *zErrMsg = 0;
    char *sql=NULL;

    sqlite3 *db=NULL;
    int  rc;

    rc = OpenAndCreateDB(&db);
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
        fprintf(stdout, "Records created successfully\n");
    }
    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
    sqlite3_close(db);

#endif // save to db

#if 0
    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
    CGeminiKeyStack CGeminiKeyStack_testImportLock;
    CGeminiKeyStack CGeminiKeyStack_testImportUnlock;

    CGeminiKeyStack_testImportLock.GeminiKeyImport(strExportLock);
    CGeminiKeyStack_testImportUnlock.GeminiKeyImport(strExportUnlock);

    GeminiKeys __KeysGenLock;
    CGeminiKeyStack_testImportLock.Decode(__KeysGenLock);
    GeminiKeys __KeysGenUnlock;
    CGeminiKeyStack_testImportUnlock.Decode(__KeysGenUnlock);

    // 恢复成功 gemini_KeyExport vs gemini_KeyImport
    CPPUNIT_ASSERT(__KeysLock == __KeysGenLock);
    CPPUNIT_ASSERT(__KeysUnlock == __KeysGenUnlock);

#if 0

    __KeysLock.dump();
    __KeysGenLock.dump();
    __KeysUnlock.dump();
    __KeysGenUnlock.dump();
#endif

    //锁门
    CGeminiKeyStack CGeminiKeyStack_testLockLock;
    CGeminiKeyStack CGeminiKeyStack_testImportLockUnlock;

    std::string PlainText;
    std::string LockedData;

    // 检查并校验数据
    bool bRet;
    bRet = __KeysGenLock.CheckVerifyCode();
    CPPUNIT_ASSERT(bRet == true);
    bRet = __KeysGenUnlock.CheckVerifyCode();
    CPPUNIT_ASSERT(bRet == true);

    int iret = 0;
    // use
    iret = CGeminiKeyStack_testLockLock.GeminiLock(__KeysGenLock, PlainText, LockedData);
    CPPUNIT_ASSERT(iret == 0);
#ifndef CPP_UNIT

    __fline;
    std::cout << "Locked:" << (iret==0?"Succeeded":"Failed") << std::endl;
    std::cout << "--------------------------------------------- PlainText(" << PlainText.size() << ")" << std::endl;
    std::cout << PlainText << std::endl;
    std::cout << "--------------------------------------------- LockedData(" << LockedData.size() << ")"  << std::endl;
    std::cout << LockedData << std::endl;
    std::cout << "---------------------------------------------" << std::endl;
#endif

    // 开锁
    iret = CGeminiKeyStack_testImportLockUnlock.GeminiUnlock(__KeysGenUnlock, PlainText, LockedData);
    CPPUNIT_ASSERT(iret == 0);
#ifndef CPP_UNIT

    __fline;
    std::cout << "Unlocked:" << (iret==0?"Succeeded":"Failed")    << std::endl;
#endif
#endif //0


    return 0;

}

void signal_proc(int signo)
{
    if(signo == SIGTERM 	\
    	|| signo == SIGKILL \
    	|| signo == SIGQUIT \
    	|| signo == SIGINT   \
    	)
    {
//        g_Solar.haveException(signo, (char *)"on exit signal");
//        g_Solar.Term();

        std::cout << std::endl << "Result :(ms)" << std::endl;
        std::cout << "total_createNumber:" << total_createNumber << std::endl;
        std::cout << "total_createDuration:" << total_createDuration << std::endl;
        std::cout << "performance of createDuration:" << (unsigned int)( (double)total_createDuration/total_createNumber) << std::endl;
        	
        std::cout << "total_runedDuration:" << total_runedDuration << std::endl;

        exit(0);
    }
    else if(signo == SIGPIPE)
    {
//        g_Solar.haveException(signo, (char *)"on signal:SIGPIPE");
    }
}

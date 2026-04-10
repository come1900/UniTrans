/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * mysql_addUser.h - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: mysql_addUser.h 5884 2017-06-19 09:17:24Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2017-06-19 09:17:24  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/


#if !defined(_mysql_addUser_H)
#define _mysql_addUser_H

//
#if defined(_REMONT_102_DBG_MYSQL_SVR)
    #define DEF_PST1_DB_MYSQL_SERVER     "221.224.63.102"
    #define DEF_PST1_DB_MYSQL_PORT       13306
    #define DEF_PST1_DB_MYSQL_USER       "admin"
    #define DEF_PST1_DB_MYSQL_PASSWORD   "password"
    #define DEF_PST1_DB_MYSQL_DATABASE   "prado_t1"
#else // _REMONT_102_DBG_MYSQL_SVR
    #define DEF_PST1_DB_MYSQL_SERVER     "localhost"
    #define DEF_PST1_DB_MYSQL_PORT       3306
    #define DEF_PST1_DB_MYSQL_USER       "admin"
    #define DEF_PST1_DB_MYSQL_PASSWORD   "password"
    #define DEF_PST1_DB_MYSQL_DATABASE   "prado"
#endif // _REMONT_102_DBG_MYSQL_SVR

#ifdef __cplusplus
extern "C"
{
#endif

    int pst1_mysql_addUser(const char *p_Mp, const char *p_Passwd, const char *p_Area);
    int pst1_mysql_modUser(const char *p_LoginName, const char *p_Passwd);

#ifdef __cplusplus
}
#endif

#endif // !defined(_mysql_addUser_H)

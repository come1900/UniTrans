/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * mysql_addTerminal.h - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: mysql_addTerminal.h 5884 2017-06-05 11:24:01Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2017-06-05 11:24:01  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/


#if !defined(_pst1_pub_def_H)
#define _pst1_pub_def_H

//
#if defined(_REMONT_102_DBG_MYSQL_SVR)
    #define DEF_PST1_DB_MYSQL_SERVER     "221.224.71.130"
    #define DEF_PST1_DB_MYSQL_PORT       33306
    #define DEF_PST1_DB_MYSQL_USER       "admin"
    #define DEF_PST1_DB_MYSQL_PASSWORD   "password"
    #define DEF_PST1_DB_MYSQL_DATABASE   "prado"
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

int pst1_mysql_addTerminal(const char *p_devID, const char *p_devKey, const char *p_devModel);

int pst1_mysql_getTerminalInfo(const char *p_devID, const char *p_devKey, int *p_devOnline, int *p_devBinded);

#ifdef __cplusplus
}
#endif

#endif // !defined(_pst1_pub_def_H)

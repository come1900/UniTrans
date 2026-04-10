/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * defKeyValue.h - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: defKeyValue.h 5884 2018-06-12 11:06:14Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2018-06-12 11:06:14  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
#include <string>
#include <vector>
#include <map>

#if !defined(_defKeyValue_H)
#define _defKeyValue_H

typedef  std::map<std::string, std::string> def_uniKeyValue;
typedef  std::vector<std::string> def_uniKeyVector;

const std::string defKeyValue_Key_HostIP           = "HostIP"           ;
const std::string defKeyValue_Key_InternetIP       = "InternetIP"       ;

#endif // !defined(_defKeyValue_H)

/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * defGRest.h - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: defGRest.h 5884 2019-11-22 10:06:20Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2019-11-22 10:06:20  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#if !defined(_defGRest_H)
#define _defGRest_H

#define def_GRestServer_ServerName "com.ezlibs.GRestd"

#define def_Pandora_Req_getGminiLicense  "/pdr/glic/get"
#define def_Pandora_Req_setGminiLicense  "/pdr/glic/set"

//appkey 管理类
#define def_Pandora_Req_getAppKey  "/pdr/appk/get"
#define def_Pandora_Req_setAppKey  "/pdr/appk/set"
#define def_Pandora_Req_listAppKey  "/pdr/appk/list"
#define def_Pandora_Req_manAppKeyCreate  "/man/appkey/create"
#define def_Pandora_Req_manAppKeyCancel  "/man/appkey/cancel"

//配置修改
#define def_Pandora_Req_manConfigMod  "/man/config/mod"
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
class CdefGRest// : public CEZObject
{
public:

    CdefGRest();
    virtual ~CdefGRest();

private:

};

#endif // !defined(_defGRest_H)

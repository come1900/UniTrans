/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * websFunction.cpp - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: websFunction.cpp 5884 2013-04-08 04:23:26Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2013-04-08 04:23:26  Create
 *     2014-11-6 11:45:01  Modify  解决文字过长，页面显示不全问题（文字过长）
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#include <stdio.h>
#include <string.h> /* for strncpy */
#include	<unistd.h>
#include	<sys/types.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <net/if.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h> /*sleep*/

#include <str_opr.h> /* for strncpy */

#include "net_work_interface.h"
// json libs
#ifdef FUNC_GEMINILOCK
#include "../Function/GeminiKey/GeminiKeyStack.h"
#include "../Function/GeminiLock/FunGeminiLock.h"
#endif

#ifdef _USE_MODULES_EZCONFIG
#include "../Configs/ConfigGeneral.h"
//#include "../Configs/ConfigNetworkInterface.h"
//#include "../Configs/ConfigNTPClient.h"
//#include "../Configs/ConfigSerial.h"
//#include "../Configs/ConfigSerialShare.h"
#endif //_USE_MODULES_EZCONFIG

#include	"../Logs.h"
#ifndef STANDLONE_UTIL
#include "../Solar.h"
#endif //STANDLONE_UTIL

#include	"uemf.h"
#include	"wsIntrn.h"

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

// "<html>" in websHeader
#define PAGE_TOP_STR "<head>"\
					"<meta http-equiv=\"Content-Type\" content=\"application/xhtml+xml; charset=GB2312\" />" \
					 "<title>SmartPorts</title>"\
					 "<link rel=\"stylesheet\" href=\"/style/normal_ws.css\" type=\"text/css\">"\
					 "</head>"\
					 "<body>"

#define PAGE_BTN_STR "</body>"

/******************************************************************************/
/*
 *	conn_manage.asp.
 */
static int aspGetNetwork(int eid, webs_t wp, int argc, char_t **argv)
{
    char	*if_name, *want_para;

    int ret = 0;
    char l_mac_get[MAC_STR_SIZ];

#ifdef __EXAMPLE_EZCONFIG

    CConfigNet __cfgNet;
    CConfigNetworkInterface __cfgNetworkInterface;
    __cfgNet.update();
    __cfgNetworkInterface.update();
#endif


    if (ejArgs(argc, argv, T("%s %s"), &if_name, &want_para) < 2)
    {
        websError(wp, 400, T("Insufficient args\n"));
        return -1;
    }

    // 检查网络接口
    int index = -1;
    if (strcmp(if_name, "general")==0)
    {
        index=0;
    }
    else
    {
#ifdef __EXAMPLE_EZCONFIG
        for (int ii=0; ii<MAX_NETWORK_INTERFACE_NUM; ii++)
        {
            if (__cfgNetworkInterface.getConfig(ii).strIFName == if_name)
            {
                index = ii;
            }
        }
#else
        index = 0;
#endif //

    }

    if (index<0)
    {
        websError(wp, 400, T("Insufficient args\n"));
        return -1;
    }

    if (strcmp(want_para, "auto") == 0)
    {
#ifdef __EXAMPLE_EZCONFIG
        return websWrite(wp, T("%s"), __cfgNetworkInterface.getConfig(index).bDhcp==1?"checked=checked":"");
#else

        return websWrite(wp, T("%s"), 1?"checked=checked":"");
#endif //

    }
    // 下列重新获取，便于页面更新后可以查看是否设置成功
    else if (strcmp(want_para, "ip") == 0)
    {
        //int ez_ip_get(const char *p_if_name, char *ip_get, char *bcast_get, char *mask_get, char *mac_get);
        ret = ez_ip_get(if_name, l_mac_get, NULL, NULL, NULL);
        if (ret<0)
        {
            //strcpy(l_mac_get, "failed get");
            l_mac_get[0] = '\0';
        }
        return websWrite(wp, T("%s"), l_mac_get);
    }
    else if (strcmp(want_para, "mask") == 0)
    {
        ret = ez_ip_get(if_name, NULL, NULL, l_mac_get, NULL);
        if (ret<0)
        {
            //strcpy(l_mac_get, "failed get");
            l_mac_get[0] = '\0';
        }
        return websWrite(wp, T("%s"), l_mac_get);
    }
    else if (strcmp(want_para, "gateway") == 0)
    {
        ret = ez_get_gateway(if_name, l_mac_get);
        if (ret<0)
        {
            //strcpy(l_mac_get, "failed get");
            l_mac_get[0] = '\0';
        }
        return websWrite(wp, T("%s"), l_mac_get);
    }
    else if (strcmp(want_para, "mac") == 0)
    {
        ret = ez_ip_get(if_name, NULL, NULL, NULL, l_mac_get);
        if (ret<0)
        {
            //strcpy(l_mac_get, "failed get");
            l_mac_get[0] = '\0';
        }

        return websWrite(wp, T("%s"), l_mac_get);
    }
    else if (strcmp(want_para, "link") == 0)
    {
        // -1 -- error , details can check errno
        // 1 -- interface link up
        // 0 -- interface link down.
        ret = get_netlink_status(if_name);

        return websWrite(wp, T("%d"), ret);
    }
    else if (strcmp(want_para, "default") == 0)
    {
#ifdef __EXAMPLE_EZCONFIG
        return websWrite(wp, T("%s"), __cfgNetworkInterface.getConfig(index).bDefault==1?"checked=checked":"");
#else

        return websWrite(wp, T("%s"), 1?"checked=checked":"");
#endif //

    }
    else if (strcmp(want_para, "PrimaryDNS") == 0)
    {
#ifdef __EXAMPLE_EZCONFIG
        return websWrite(wp, T("%s"), __cfgNet.getConfig().strPrimaryDNS.c_str());
#else

        return websWrite(wp, T("%s"), "8.8.8.8");
#endif //

    }
    else if (strcmp(want_para, "SecondaryDNS") == 0)
    {
#ifdef __EXAMPLE_EZCONFIG
        return websWrite(wp, T("%s"), __cfgNet.getConfig().strSecondaryDNS.c_str());
#else

        return websWrite(wp, T("%s"), "8.8.8.8");
#endif //

    }
    else
    {
        return websWrite(wp, T("%d"), 0);
        //return -1;
    }

    return 0;
}

/******************************************************************************/
/*
 *	set form for posted data (in-memory CGI). This will be called when the
 *	form in web/form_set_eth.asp is invoked. Set browser to "localhost/conn_manage.asp" to test.
 */
#ifndef MAX_NETWORK_INTERFACE_NUM
 #define MAX_NETWORK_INTERFACE_NUM 4
 #endif //MAX_NETWORK_INTERFACE_NUM
const char *network_iface_inputname[] =
    {"eth%d_auto", "eth%d_ipv4", "eth%d_mask", "eth%d_gwv4"
    };
static void formSetNetwork(webs_t wp, char_t *path, char_t *query)
{
    char_t	*gen_pri_dns = NULL;
    char_t	*gen_sec_dns = NULL;
    char fieldName[]={"eth6666_mask"};

    char_t	*if_auto[MAX_NETWORK_INTERFACE_NUM] = {NULL};
    char_t	*if_ipv4[MAX_NETWORK_INTERFACE_NUM] = {NULL};
    char_t	*if_mask[MAX_NETWORK_INTERFACE_NUM] = {NULL};
    char_t	*if_gwv4[MAX_NETWORK_INTERFACE_NUM] = {NULL};

    char_t	*if_default = NULL;
    int	idefault = -1;
    int iret;

    ARG_USED(idefault);
    ARG_USED(iret);

#ifdef __EXAMPLE_EZCONFIG

    CConfigNet __cfgNet;
    CConfigNetworkInterface __cfgNetworkInterface;
    __cfgNet.update();
    __cfgNetworkInterface.update();

    int iNetworkInterfaceNum = __cfgNet.getConfig().iNetworkInterfaceNum;
#else // __EXAMPLE_EZCONFIG

    int iNetworkInterfaceNum = 0;
#endif // __EXAMPLE_EZCONFIG

    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
    for(int ii=0; ii<iNetworkInterfaceNum; ii++)
    {
        sprintf(fieldName, network_iface_inputname[0], ii);
        if_auto[ii] = websGetVar(wp, T(fieldName), NULL);
        sprintf(fieldName, network_iface_inputname[1], ii);
        if_ipv4[ii] = websGetVar(wp, T(fieldName), NULL);
        sprintf(fieldName, network_iface_inputname[2], ii);
        if_mask[ii] = websGetVar(wp, T(fieldName), NULL);
        sprintf(fieldName, network_iface_inputname[3], ii);
        if_gwv4[ii] = websGetVar(wp, T(fieldName), NULL);

        trim_blank(if_auto[ii], 0);
        trim_blank(if_ipv4[ii], 0);
        trim_blank(if_mask[ii], 0);
        trim_blank(if_gwv4[ii], 0);
    }

    if_default = websGetVar(wp, T("default_eth"), NULL);
    gen_pri_dns = websGetVar(wp, T("PrimaryDNS"), NULL);
    gen_sec_dns = websGetVar(wp, T("SecondaryDNS"), NULL);

    trim_blank(if_default, 0);
    trim_blank(gen_pri_dns, 0);
    trim_blank(gen_sec_dns, 0);

    websHeader(wp);
    websWrite(wp, T(PAGE_TOP_STR));

    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
    // check and set
    //dns
#ifdef __EXAMPLE_EZCONFIG

    if (0 == ip4_valid(gen_pri_dns))
    {
        if (__cfgNet.getConfig().strPrimaryDNS != gen_pri_dns)
            __cfgNet.getConfig().strPrimaryDNS = gen_pri_dns;
    }

    if (0 == ip4_valid(gen_sec_dns))
    {
        if (__cfgNet.getConfig().strSecondaryDNS != gen_sec_dns)
            __cfgNet.getConfig().strSecondaryDNS = gen_sec_dns;
    }

    // ip etc
    for(int ii=0; ii<iNetworkInterfaceNum; ii++)
    {
        // default -- only one
        __cfgNetworkInterface.getConfig(ii).bDefault = 0;
        if (__cfgNetworkInterface.getConfig(ii).strIFName == if_default)
        {
            idefault = ii;
            __cfgNetworkInterface.getConfig(ii).bDefault = 1;
        }

        // 设定了auto 其他值就取不到了
        if (if_auto[ii])
        {
            __fline;
            printf("if_auto[%d]:%s, pass the static ip value.\n", ii, if_auto[ii]);
            __cfgNetworkInterface.getConfig(ii).bDhcp = 1;
            continue;
        }

        if (0 != ip4_valid(if_ipv4[ii]))
        {
            __trip;
            printf("if_ipv4[%d]:%s\n", ii, if_ipv4[ii]);
            if_ipv4[ii] = NULL;
        }
        if (ip4_valid(if_mask[ii])<0)
        {
            __trip;
            printf("if_ipv4[%d]:%s\n", ii, if_mask[ii]);
            if_mask[ii] = NULL;
        }
        if (0 != ip4_valid(if_gwv4[ii]))
        {
            __trip;
            printf("if_ipv4[%d]:%s\n", ii, if_gwv4[ii]);
            if_gwv4[ii] = NULL;
        }

        //__fline;
        //printf("if [%d]:[%s] [%s] [%s]\n", ii, if_ipv4[ii], if_mask[ii], if_gwv4[ii]);

        iret = ip4_mask_gateway_match(if_ipv4[ii], if_mask[ii], if_gwv4[ii]);
        if (iret < 0)
        {
            websWrite(wp, T("<H2>parameter ERROR</H2><ul>"
                            "<li>iret: %d</li>"
                            "</ul>")
                      , iret
                     );

            __trip;
            printf("ip4_mask_gateway_match error [%d]:[%s] [%s] [%s]\n", iret, if_ipv4[ii], if_mask[ii], if_gwv4[ii]);
        }
        else
        {
            __fline;
            __cfgNetworkInterface.getConfig(ii).bDhcp = 0;
#if 1

            if (if_ipv4[ii]!=NULL && __cfgNetworkInterface.getConfig(ii).strIp != if_ipv4[ii])
            {
                __fline;
                printf("strIp changed.\n");
                __cfgNetworkInterface.getConfig(ii).strIp = if_ipv4[ii];
            }
            if (if_mask[ii]!=NULL && __cfgNetworkInterface.getConfig(ii).strMask != if_mask[ii])
            {
                __fline;
                printf("strMask changed.\n");
                __cfgNetworkInterface.getConfig(ii).strMask = if_mask[ii];
            }
            if (if_gwv4[ii]!=NULL && __cfgNetworkInterface.getConfig(ii).strGateWay != if_gwv4[ii])
            {
                __fline;
                printf("strMask changed.\n");
                __cfgNetworkInterface.getConfig(ii).strGateWay = if_gwv4[ii];
            }
#endif

        }
        DBG(
            printf("ip4_mask_gateway_match : %d\n\t%s\n\t%s\n\t%s\n\tResult -- %s\n"
                   , iret, if_ipv4[ii], if_mask[ii], if_gwv4[ii], iret==0?"OK":"Failed");
        );
    }
    // 页面未传
    if (idefault<0)
    {
        __cfgNetworkInterface.getConfig(0).bDefault = 1;
    }

    __cfgNetworkInterface.commit();
    __cfgNet.commit();

    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
    // 打印输入信息
    for(int ii=0; ii<iNetworkInterfaceNum; ii++)
    {
        websWrite(wp, T("<h2>%s</h2><ul>"
                        "<li>auto: %s</li>"
                        "<li>ipv4: %s</li>"
                        "<li>mask: %s</li>"
                        "<li>gwv4: %s</li>"
                        "</ul>")
                  , __cfgNetworkInterface.getConfig(ii).strIFName.c_str()
                  , if_auto[ii]== NULL?"NULL":if_auto[ii]
                  , if_ipv4[ii]== NULL?"NULL":if_ipv4[ii]
                  , if_mask[ii]== NULL?"NULL":if_mask[ii]
                  , if_gwv4[ii]== NULL?"NULL":if_gwv4[ii]
                 );
    }
#endif // #ifdef __EXAMPLE_EZCONFIG

    websWrite(wp, T("<H2>General</H2><ul>"
                    "<li>default: %s</li>"
                    "<li>pdns: %s</li>"
                    "<li>sdns: %s</li>"
                    "</ul>")
              , if_default  == NULL?"NULL":if_default
              , gen_pri_dns == NULL?"NULL":gen_pri_dns
              , gen_sec_dns == NULL?"NULL":gen_sec_dns
             );
    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

    websWrite(wp, T(PAGE_BTN_STR));
    websFooter(wp);
    websDone(wp, 200);
}

/*
*/
static int aspGetNet(int eid, webs_t wp, int argc, char_t **argv)
{
    char_t	*want_para;
    ARG_USED(want_para);
#ifdef __EXAMPLE_EZCONFIG

    CConfigNet __cfgNet;
    __cfgNet.update();

    if (ejArgs(argc, argv, T("%s"), &want_para) < 1)
    {
        websError(wp, 400, T("Insufficient args\n"));
        return -1;
    }

    if (strcmp(want_para, "sshd_port") == 0)
    {
        return websWrite(wp, T("%d"), __cfgNet.getConfig().iSshdPort);
    }
    else if (strcmp(want_para, "sshd_port_prompt") == 0)
    {
        return websWrite(wp, T("%s"), __cfgNet.getConfig().iSshdPort>0?"Running, Click to Close":"Stoped, Click to Run");
    }
    else if (strcmp(want_para, "garp_period") == 0)
    {
        return websWrite(wp, T("%d"), __cfgNet.getConfig().iGarpPeriod);
    }
    else if (strcmp(want_para, "garp_period_prompt") == 0)
    {
        return websWrite(wp, T("%s"), __cfgNet.getConfig().iGarpPeriod>0?"Running, Click to Close":"Stoped, Click to Run");
    }
    else if (strcmp(want_para, "sadp_port") == 0)
    {
        return websWrite(wp, T("%d"), __cfgNet.getConfig().iSADPPort);
    }
    else if (strcmp(want_para, "sadp_port_prompt") == 0)
    {
        return websWrite(wp, T("%s"), __cfgNet.getConfig().iSADPPort>0?"Running, Click to Close":"Stoped, Click to Run");
    }
    else
    {
        return websWrite(wp, T("%s"), "unsupport");
    }
#else
    websWrite(wp, T("%s"), "0");
#endif //#ifdef __EXAMPLE_EZCONFIG

    return 0;
}
#define MARK_NET_OPR_RESET "-1"
//#define SHOW_POST_STR // DEBUG
static void formSetNet(webs_t wp, char_t *path, char_t *query)
{
    char_t	*p_sshd_port = websGetVar(wp, T("sshd_port"), NULL);
    char_t	*p_garp_period = websGetVar(wp, T("garp_period"), NULL);
    char_t	*p_sadp_port = websGetVar(wp, T("sadp_port"), NULL); //

    ARG_USED(p_sshd_port);
    ARG_USED(p_garp_period);
    ARG_USED(p_sadp_port);

#ifdef __EXAMPLE_EZCONFIG

    CConfigNet __cfgNet;
    __cfgNet.update();
#endif

#ifdef SHOW_POST_STR

    websHeader(wp);
    websWrite(wp, T(PAGE_TOP_STR));
#endif //SHOW_POST_STR

#ifdef __EXAMPLE_EZCONFIG

    if (p_sshd_port)
    {
        trim_blank(p_sshd_port, 0);

#ifdef SHOW_POST_STR

        websWrite(wp, T("<p>p_sshd_port     :%s</p>\n"), p_sshd_port);
#endif //SHOW_POST_STR

        if (strcmp(MARK_NET_OPR_RESET, p_sshd_port)==0)
        {
            DBG(        __trip; );
            if (__cfgNet.getConfig().iSshdPort>0)
            {
                DBG(        __trip; );
                __cfgNet.getConfig().iSshdPort = 0;
            }
            else
            {
                DBG(        __trip; );
                __cfgNet.getConfig().iSshdPort = DEFAULT_CFG_NET_SshdPort;
            }
        }
    }
    if (p_garp_period)
    {
        trim_blank(p_garp_period, 0);

#ifdef SHOW_POST_STR

        websWrite(wp, T("<p>p_garp_period     :%s</p>\n"), p_garp_period);
#endif //SHOW_POST_STR

        if (strcmp(MARK_NET_OPR_RESET, p_garp_period)==0)
        {
            DBG(        __trip; );
            if (__cfgNet.getConfig().iGarpPeriod>0)
            {
                DBG(        __trip; );
                __cfgNet.getConfig().iGarpPeriod = 0;
            }
            else
            {
                DBG(        __trip; );
                __cfgNet.getConfig().iGarpPeriod = DEFAULT_CFG_NET_GarpPeriod;
            }
        }
    }

    if (p_sadp_port)
    {
        trim_blank(p_sadp_port, 0);

#ifdef SHOW_POST_STR

        websWrite(wp, T("<p>p_sadp_port     :%s</p>\n"), p_sadp_port);
#endif //SHOW_POST_STR

        if (strcmp(MARK_NET_OPR_RESET, p_sadp_port)==0)
        {
            DBG(        __trip; );
            if (__cfgNet.getConfig().iSADPPort>0)
            {
                DBG(        __trip; );
                __cfgNet.getConfig().iSADPPort = 0;
            }
            else
            {
                DBG(        __trip; );
                __cfgNet.getConfig().iSADPPort = DEFAULT_CFG_NET_SADPPort;
            }
        }
    }

    __cfgNet.commit();
#endif //#ifdef __EXAMPLE_EZCONFIG

#ifdef SHOW_POST_STR

    websWrite(wp, T(PAGE_BTN_STR));
    websFooter(wp);
    websDone(wp, 200);
#else

    usleep(1000*30);
    websRedirect(wp, "form_set_eth.asp");

    return;
#endif //SHOW_POST_STR

}


/*此例子取用一个参数*/
static int aspGetDateTime(int eid, webs_t wp, int argc, char_t **argv)
{
    //__fline;
    //printf("%s\n", __FUNCTION__);

    char_t	*want_para;

    if (ejArgs(argc, argv, T("%s"), &want_para) < 1)
    {
        websError(wp, 400, T("Insufficient args\n"));
        return -1;
    }
#ifdef __EXAMPLE_EZCONFIG

    CConfigGeneral __cfgGeneral;
    CConfigNTPClient __cfgNTPClient;

    __cfgGeneral.update();
    __cfgNTPClient.update();

    if (strcmp(want_para, "ntp_enable") == 0)
    {
        return websWrite(wp, T("%s"), __cfgNTPClient.getConfig().UpdatePeriod>0?"checked=checked":"");
    }
    else if (strcmp(want_para, "ntp_server") == 0)
    {
        return websWrite(wp, T("%s"), __cfgNTPClient.getConfig().strNtpServer.c_str());
    }
    else if (strcmp(want_para, "ntp_checkperiod") == 0)
    {
        return websWrite(wp, T("%d"), __cfgNTPClient.getConfig().UpdatePeriod>=5?__cfgNTPClient.getConfig().UpdatePeriod:600);
    }
    else if (strcmp(want_para, "autotimezone") == 0)
    {
        return websWrite(wp, T("%s"), __cfgNTPClient.getConfig().iTimeZoneAuto==1?"checked=checked":"");
    }

    else if (strcmp(want_para, "sys_date_time") == 0)
    {
        char pBuff[32];
        get_date_time_string(pBuff, NULL);
        return websWrite(wp, T("%s"), pBuff);
    }
    else if (strcmp(want_para, "sys_timezone") == 0)
    {
        return websWrite(wp, T("%d"), __cfgGeneral.getConfig().iTimeZone);
    }
    else if (strcmp(want_para, "sys_date_format") == 0)
    {
        return websWrite(wp, T("%d"), __cfgGeneral.getConfig().iDateTimeFormat);
    }
    else
    {
        return websWrite(wp, T("%s"), "unsupport");
    }
#endif //#ifdef __EXAMPLE_EZCONFIG

    return 0;
}

static void formSetDateTime(webs_t wp, char_t *path, char_t *query)
{
    char_t *p_ntp_enable     	= websGetVar(wp, T("ntp_enable"), 		NULL);
    char_t *p_ntp_server     	= websGetVar(wp, T("ntp_server"), 		NULL);
    char_t *p_ntp_checkperiod	= websGetVar(wp, T("ntp_checkperiod"), 	NULL);
    char_t *p_autotimezone   	= websGetVar(wp, T("autotimezone"), 	NULL);
    char_t *p_sys_date_time  	= websGetVar(wp, T("sys_date_time"), 	NULL);
    char_t *p_sys_timezone   	= websGetVar(wp, T("sys_timezone"), 	NULL);
    char_t *p_sys_date_format	= websGetVar(wp, T("sys_date_format"), 	NULL);

    //int i_ntp_enable = 0;
    int i_ntp_checkperiod = 0;
    //int i_autotimezone = 0;
    int i_sys_timezone = 0;
    //int i_sys_date_format = 0;

    ARG_USED(i_ntp_checkperiod);
    ARG_USED(i_sys_timezone);

#ifdef __EXAMPLE_EZCONFIG
    // use ntp
    CConfigGeneral __cfgGeneral;
    CConfigNTPClient __cfgNTPClient;

    __cfgGeneral.update();
    __cfgNTPClient.update();

    if (p_ntp_enable!=NULL)
    {
        if (p_ntp_server!=NULL)
        {
            __cfgNTPClient.getConfig().strNtpServer = p_ntp_server;
        }

        if (p_ntp_checkperiod != NULL)
        {
            sscanf(p_ntp_checkperiod, "%d", &i_ntp_checkperiod);
        }
        __cfgNTPClient.getConfig().UpdatePeriod = i_ntp_checkperiod>5?i_ntp_checkperiod:5;

    }
    // use input time
    else
    {
        __cfgNTPClient.getConfig().UpdatePeriod = 0;

        if (p_sys_date_time!=NULL)
        {
            int retval = ez_set_system_time(p_sys_date_time);
            ARG_USED(retval);
            DBG(
                __fline;
                printf("SetSystemTime:%s -- %d\n", p_sys_date_time, retval);
            );
        }
    }

    if (p_sys_timezone != NULL)
    {
        sscanf(p_sys_timezone, "%d", &i_sys_timezone);

        if (i_sys_timezone>=0 && i_sys_timezone<=33)
        {
            __cfgGeneral.getConfig().iTimeZone = i_sys_timezone;
        }
        else
        {
            LOG4CPLUS_ERROR(LOG_OF_SWEBS, "i_sys_timezone NOT valid:"<<i_sys_timezone);
        }
    }

    __cfgNTPClient.commit();
    __cfgGeneral.commit();
#endif //#ifdef __EXAMPLE_EZCONFIG

#if 1

    websHeader(wp);
    websWrite(wp, T("<body>\n"));

    websWrite(wp, T("<p>ntp_enable     :%s</p>\n"), p_ntp_enable      == NULL?"NULL":p_ntp_enable     );
    websWrite(wp, T("<p>ntp_server     :%s</p>\n"), p_ntp_server      == NULL?"NULL":p_ntp_server     );
    websWrite(wp, T("<p>ntp_checkperiod:%s</p>\n"), p_ntp_checkperiod == NULL?"NULL":p_ntp_checkperiod);
    if (p_autotimezone)
        websWrite(wp, T("<p>autotimezone   :%s</p>\n"), p_autotimezone    == NULL?"NULL":p_autotimezone   );
    websWrite(wp, T("<p>sys_date_time  :%s</p>\n"), p_sys_date_time   == NULL?"NULL":p_sys_date_time  );
    websWrite(wp, T("<p>sys_timezone   :%s</p>\n"), p_sys_timezone    == NULL?"NULL":p_sys_timezone   );
    if (p_sys_date_format)
        websWrite(wp, T("<p>sys_date_format:%s</p>\n"), p_sys_date_format == NULL?"NULL":p_sys_date_format);


    websWrite(wp, T(PAGE_BTN_STR));

    websFooter(wp);
    websDone(wp, 200);

#else

    usleep(1000*100);
    websRedirect(wp, "form_ctrl_enocean.asp");
    return;

#endif
}

static int aspGetSys(int eid, webs_t wp, int argc, char_t **argv)
{
    char_t	*want_para;

    if (ejArgs(argc, argv, T("%s"), &want_para) < 1)
    {
        __trip;
        websError(wp, 400, T("Insufficient args\n"));
        return -1;
    }

#ifdef _USE_MODULES_EZCONFIG
    CConfigGeneral __cfgGeneral;
    __cfgGeneral.update();

    if (strcmp(want_para, "sys_name") == 0)
    {
        return websWrite(wp, T("%s"), __cfgGeneral.getConfig().strCode.c_str());
    }
    else if (strcmp(want_para, "sys_copyright") == 0)
    {
#ifndef STANDLONE_UTIL
        return websWrite(wp, T("%s"), g_Solar.GetCopyRights());
#else

        return websWrite(wp, T("%s"), "SmartWebs STANDLONE_UTIL");
#endif

    }
    else if (strcmp(want_para, "sys_product_id") == 0)
    {
        return websWrite(wp, T("%s"), __cfgGeneral.getConfig().strProductID.c_str());
    }
    else if (strcmp(want_para, "sys_serial_no") == 0)
    {
        return websWrite(wp, T("%s"), __cfgGeneral.getConfig().strSerial.c_str());
    }
    else if (strcmp(want_para, "sys_version") == 0)
    {
        return websWrite(wp, T("%s"), __cfgGeneral.getConfig().strVersion.c_str());
    }
    else if (strcmp(want_para, "sys_language") == 0)
    {
        return websWrite(wp, T("%d"), __cfgGeneral.getConfig().iLanguage);
    }
    else if (strcmp(want_para, "sys_run_period") == 0)
    {
#ifndef STANDLONE_UTIL
        return websWrite(wp, T("%d"), g_Solar.GetRunPeriod());
#else

        return websWrite(wp, T("%d"), -1);
#endif

    }
    else if (strcmp(want_para, "sys_start_date_time") == 0)
    {
        char pBuff[32];
#ifndef STANDLONE_UTIL

        time2string(g_Solar.GetStartTime(), pBuff, NULL);
#else

        time2string(time(NULL), pBuff, NULL);
#endif

        return websWrite(wp, T("%s"), pBuff);
    }
    else if (strcmp(want_para, "sys_internet") == 0)
    {
#ifndef STANDLONE_UTIL
        //return websWrite(wp, T("%s"), g_Solar.IsOnInternet()?"ON":"OFF");
        return websWrite(wp, T("%s"), 1?"ON":"OFF");
#else

        return websWrite(wp, T("%s"), 1?"ON":"OFF");
#endif

    }
    else if (strcmp(want_para, "sys_nat_ip") == 0)
    {
#ifndef STANDLONE_UTIL
        //return websWrite(wp, T("%s"),  g_Solar.IsOnInternet()?g_Solar.GetNatIP():"NotValid");
        return websWrite(wp, T("%s"), 1?"0.0.0.0":"OFF");

#else

        return websWrite(wp, T("%s"), 1?"0.0.0.0":"OFF");
#endif

    }
    else
    {
        return websWrite(wp, T("%s"), "unsupport");
    }
#endif//#ifdef _USE_MODULES_EZCONFIG

    return 0;
}

static void formSetSys(webs_t wp, char_t *path, char_t *query)
{
    char_t *p_sys_reboot     	= websGetVar(wp, T("sys_reboot"), 		NULL);
    char_t *p_sys_restore     	= websGetVar(wp, T("sys_restore"), 		NULL);

    char_t *p_sys_name		    = websGetVar(wp, T("sys_name"		 ), NULL);
    char_t *p_sys_product_id	= websGetVar(wp, T("sys_product_id"	 ), NULL);
    char_t *p_sys_serial_no	    = websGetVar(wp, T("sys_serial_no"	 ), NULL);
    char_t *p_sys_version	    = websGetVar(wp, T("sys_version"	 ), NULL);
    char_t *p_sys_language	    = websGetVar(wp, T("sys_language"	 ), NULL);
    // ret page

    ARG_USED(p_sys_reboot);
    ARG_USED(p_sys_restore);
    ARG_USED(p_sys_name);
    ARG_USED(p_sys_product_id);
    ARG_USED(p_sys_serial_no);
    ARG_USED(p_sys_version);
    ARG_USED(p_sys_language);

#ifdef _USE_MODULES_EZCONFIG

    CConfigGeneral __cfgGeneral;
    __cfgGeneral.update();

#if 1

    websHeader(wp);
    websWrite(wp, T("<body>\n"));

    if (p_sys_name)
    {
        trim_blank(p_sys_name, 0);
        int __len = strlen(p_sys_name);
        if (__len>0 && __len<256)
        {
            __cfgGeneral.getConfig().strCode = p_sys_name;
        }
        websWrite(wp, T("<p>p_sys_name     :%s</p>\n"), p_sys_name      == NULL?"NULL":p_sys_name     );
    }

    if (p_sys_language)
    {
        trim_blank(p_sys_language, 0);
        int __len = strlen(p_sys_language);
        if (__len>0 && __len<128)
        {
            __cfgGeneral.getConfig().iLanguage = atoi(p_sys_language);
        }

        websWrite(wp, T("<p>p_sys_language     :%s</p>\n"), p_sys_language      == NULL?"NULL":p_sys_language     );
    }

    if (p_sys_product_id)
    {
        trim_blank(p_sys_product_id, 0);
        int __len = strlen(p_sys_product_id);

        if (__len>0 && __len<256)
        {
            __cfgGeneral.getConfig().strProductID = p_sys_product_id;
        }

        websWrite(wp, T("<p>p_sys_product_id     :%s</p>\n"), p_sys_product_id      == NULL?"NULL":p_sys_product_id     );
    }
    if (p_sys_serial_no)
    {
        websWrite(wp, T("<p>p_sys_serial_no     :%s</p>\n"), p_sys_serial_no      == NULL?"NULL":p_sys_serial_no     );
    }

    if (p_sys_version)
    {
        websWrite(wp, T("<p>p_sys_version     :%s</p>\n"), p_sys_version      == NULL?"NULL":p_sys_version     );
    }

    if (p_sys_reboot)
    {

        websWrite(wp, T("<p>p_sys_reboot     :%s</p>\n"), p_sys_reboot      == NULL?"NULL":p_sys_reboot     );
        websWrite(wp, T("<p>reboot ...</p>\n"));
    }
    else if (0)
    {
        websWrite(wp, T("<p>reboot not found.</p>\n"));
    }

    if (p_sys_restore)
    {
        //websWrite(wp, T("<p>p_sys_restore     :%s</p>\n"), p_sys_restore      == NULL?"NULL":p_sys_restore     );
        websWrite(wp, T("<p>restore ... use default config to access</p>\n"));

#ifndef STANDLONE_UTIL

        g_Solar.Restore();
#endif

    }

    websWrite(wp, T(PAGE_BTN_STR));

    websFooter(wp);
    websDone(wp, 200);

#else

    usleep(1000*100);
    websRedirect(wp, "form_ctrl_enocean.asp");
    return;

#endif

    __cfgGeneral.commit();
#endif //ifdef _USE_MODULES_EZCONFIG

    if (p_sys_reboot)
    {
#ifdef _DEBUG_THIS

        __fline;
        printf("reboot ...\n");
#else

#ifndef STANDLONE_UTIL

        g_Solar.Reboot();
#endif
#endif

    }
}


// todo 2014-02-13 11:28:57
static int aspGeminiLockGetParam(int eid, webs_t wp, int argc, char_t **argv)
{
    char_t	*want_para;

    if (ejArgs(argc, argv, T("%s"), &want_para) < 1)
    {
        websError(wp, 400, T("Insufficient args\n"));
        return -1;
    }

    if (strcmp(want_para, "lock_status") == 0)
    {
        if ( (g_FunGeminiLock.getLockDevMask()) != 0)
        {
            //char tmpstr[32] = {0};
            char channel[128] = {0};
            char status[128] = {0};
            for (int ii=0; ii< g_FunGeminiLock.getLockNum(); ii++)
            {
                sprintf(channel, "%s%-12d", channel, ii+1);
                sprintf(status, "%s%-12s"
                        , status
                        , g_FunGeminiLock.GetStatus(ii)==EnLockStatus_Locked?"Used":"Available");
            }

            return websWrite(wp, T("%s\n%s"), channel, status);
        }
        else
        {
            return websWrite(wp, T("<p></p><pre>\n------\n%s\n------</pre><br/>\n"), "LockDevice not found, pls check ...");
        }
    }
    else if (strcmp(want_para, "sys_copyright") == 0)
    {
        return websWrite(wp, T("%s"), "SmartWebs STANDLONE_UTIL");
    }
    else
    {
        return websWrite(wp, T("want_para:%s"), want_para);
    }


    return 0;
}

static void formGeminiLockInputKey(webs_t wp, char_t *path, char_t *query)
{
    // key 工具
    char_t *p_GeminiKey     	= websGetVar(wp, T("GeminiKey"), 		NULL);
    // 锁和开锁
    char_t *p_GeminiLock     	= websGetVar(wp, T("GeminiLock"), 		NULL);

    websHeader(wp);
    websWrite(wp, T(PAGE_TOP_STR));

    int iret;

    if (p_GeminiKey)
    {
        trim_blank(p_GeminiKey, 0);
        //__fline;
        //printf("len:%d\n", strlen(p_GeminiKey));

        websWrite(wp, T("<h1>GeminiKey:</h1>\n"));
        //websWrite(wp, T("<p>[%s]</p>\n"), p_GeminiKey);
        websWrite(wp, T("<p></p><pre>Your GeminiKey:\n------\n%s\n------</pre>\n"), p_GeminiKey );

#ifdef FUNC_GEMINILOCK
        //    	iret = g_FunGeminiLock.OnGeminiKey(0, p_GeminiKey, strlen(p_GeminiKey), -1);
        GeminiKeys __KeysGenUnlock;
        iret = g_FunGeminiLock.OnKey_Chk(0, p_GeminiKey, strlen(p_GeminiKey), &__KeysGenUnlock);

        if (iret >=0)
        {
            std::string strKeys;
            __KeysGenUnlock.dump(strKeys);

            // 2014-11-6 11:44:43 wujj
            // 数据长于WEBS_BUFSIZE， 分次输出
            websWrite(wp, T("<p></p><pre>密钥数据:\n------\n"));

            int len=strKeys.size();
            while (len>0)
            {
                std::string s=strKeys.substr(strKeys.size()-len, WEBS_BUFSIZE);
                websWrite(wp, T("%s"), s.c_str() );

                len -= WEBS_BUFSIZE;
            }
            websWrite(wp, T("\n------</pre>\n"));
        }
        else if (OnGeminiKey_DataInvalidLen == iret)
        {
            websWrite(wp, T("<p></p><pre>\n------\n%s \n------</pre>\n"), "OnGeminiKey_DataInvalidLen");
        }
        else if (OnGeminiKey_NotAGeminiKey == iret)
        {
            websWrite(wp, T("<p></p><pre>\n------\n%s \n------</pre>\n"), "OnGeminiKey_NotAGeminiKey");
        }
        else if (OnGeminiKey_DecodeError == iret)
        {
            websWrite(wp, T("<p></p><pre>\n------\n%s \n------</pre>\n"), "OnGeminiKey_DecodeError");
        }
        else if (OnGeminiKey_EmptyKey == iret)
        {
            websWrite(wp, T("<p></p><pre>\n------\n%s \n------</pre>\n"), "OnGeminiKey_EmptyKey");
        }

        else
        {
            websWrite(wp, T("<p></p><pre>\n------\n%s \n------</pre>\n"), "unknow error");
        }
#endif //FUNC_GEMINILOCK

    }

    if (p_GeminiLock)
    {
        //__fline;
        //printf("len:%d\n", strlen(p_GeminiLock));
        websWrite(wp, T("<p></p><pre>\n------\n%s\n------</pre><br/>\n"), "正在校验，请稍候 ... ");

        iret = g_FunGeminiLock.OnKey_Use(0, p_GeminiLock, strlen(p_GeminiLock), -1);

        if (OnGeminiKey_DataInvalidLen == iret)
        {
            //websWrite(wp, T("<p></p><pre>\n------\n%s \n------</pre>\n"), "OnGeminiKey_DataInvalidLen");
            websWrite(wp, T("<p></p><pre>\n------\n%s\n------</pre>\n"), "密钥数据长度不正确");
        }
        else if (OnGeminiKey_NotAGeminiKey == iret)
        {
            //websWrite(wp, T("<p></p><pre>\n------\n%s \n------</pre>\n"), "OnGeminiKey_NotAGeminiKey");
            websWrite(wp, T("<p></p><pre>\n------\n%s\n------</pre>\n"), "非法数据");
        }
        else if (OnGeminiKey_DecodeError == iret)
        {
            //websWrite(wp, T("<p></p><pre>\n------\n%s \n------</pre>\n"), "OnGeminiKey_DecodeError");
            websWrite(wp, T("<p></p><pre>\n------\n%s\n------</pre>\n"), "数据解析失败");
        }
        else if (OnGeminiKey_EmptyKey == iret)
        {
            //websWrite(wp, T("<p></p><pre>\n------\n%s \n------</pre>\n"), "OnGeminiKey_EmptyKey");
            websWrite(wp, T("<p></p><pre>\n------\n%s\n------</pre>\n"), "不存在密钥");
        }
        //
        else if (OnGeminiLock_DoLock == iret)
        {
            //websWrite(wp, T("<p></p><pre>\n------\n%s \n------</pre>\n"), "OnGeminiLock_DoLock");
            websWrite(wp, T("<p></p><pre>执行:\n------\n%s 操作\n------</pre>\n"), "锁");
        }
        else if (OnGeminiLock_DoLock_ButLocked== iret)
        {
            //websWrite(wp, T("<p></p><pre>\n------\n%s \n------</pre>\n"), "OnGeminiLock_DoLock_ButLocked");
            websWrite(wp, T("<p></p><pre>\n------\n%s \n------</pre>\n"), "锁已被使用， 请等待开锁后再用");
        }
        else if (OnGeminiLock_DoUnLock == iret)
        {
            //websWrite(wp, T("<p></p><pre>\n------\n%s \n------</pre>\n"), "OnGeminiLock_DoUnLock");
            websWrite(wp, T("<p></p><pre>执行:\n------\n%s 操作\n------</pre>\n"), "开锁");
        }
        else if (OnGeminiLock_DoUnLock_ButUnLocked== iret)
        {
            //websWrite(wp, T("<p></p><pre>\n------\n%s \n------</pre>\n"), "OnGeminiLock_DoUnLock_ButUnLocked");
            websWrite(wp, T("<p></p><pre>\n------\n%s \n------</pre>\n"), "锁未被锁定， 请先锁后再开锁");
        }
        else if (OnGeminiLock_DoUnLock_NotPaired == iret)
        {
            websWrite(wp, T("<p></p><pre>\n------\n%s \n------</pre>\n"), "您的开锁钥不匹配，请叫您的小伙伴来开锁");
        }
        else
        {
            websWrite(wp, T("<p></p><pre>\n------\n%s \n------</pre>\n"), "未知错误");
        }

        std::string strStatus = "备用";

        switch (g_FunGeminiLock.GetStatus())
        {
            case EnLockStatus_Ready:
            {
                strStatus = "备用";
                break;
            }
            case EnLockStatus_Opened:
            {
                strStatus = "已打开";
                break;
            }
            case EnLockStatus_Locked:
            {
                strStatus = "已锁闭";
                break;
            }
            default:
            {
                strStatus = "错误状态";
                break;
            }
        }
        //__trip;
        LOG4CPLUS_INFO(LOG_OF_FGL, "strStatus:" << strStatus);
        websWrite(wp, T("<p></p><pre>锁状态:\n------\n%s\n------</pre>\n"), strStatus.c_str() );
    }

    websWrite(wp, T(PAGE_BTN_STR));

    websFooter(wp);
    websDone(wp, 200);
}


#ifdef __cplusplus
extern "C"
{
#endif

    void websFunctionInit()
    {
#ifdef DEV_ENOCEAN_SUPPORT

        ezserial_init();
        enocean_uart_init(DEFAULT_ENOCEAN_DEVICE_INTERFACE);
        //enocean_uart_init("/dev/ttySAC3");
        enocean_uart_open();
        enocean_uart_callback(enocean_callback);
        memset(&g_dev_status, 0, sizeof(g_dev_status));
#endif//DEV_ENOCEAN_SUPPORT

        websAspDefine(T("aspGetNet"), aspGetNet);
        websFormDefine(T("formSetNet"), formSetNet);
        websAspDefine(T("aspGetNetwork"), aspGetNetwork);
        websFormDefine(T("formSetNetwork"), formSetNetwork);

        //        websFormDefine(T("formCtrlEnocean"), formCtrlEnocean);
        //        websAspDefine(T("aspGetEnoceanStatus"), aspGetEnoceanStatus);

        websAspDefine(T("aspGetDateTime"), aspGetDateTime);
        websFormDefine(T("formSetDateTime"), formSetDateTime);

        websAspDefine(T("aspGetSys"), aspGetSys);
        websFormDefine(T("formSetSys"), formSetSys);

        //        websAspDefine(T("aspGetSerial"), aspGetSerial);
        //        websFormDefine(T("formSetSerial"), formSetSerial);

        websAspDefine(T("aspGeminiLockGetParam"), aspGeminiLockGetParam);
        websFormDefine(T("formGeminiLockInputKey"), formGeminiLockInputKey);
    }

    void websFunctionCleanup()
    {
#ifdef DEV_ENOCEAN_SUPPORT
        enocean_uart_close();
        ezserial_cleanup();
#endif

    }
#ifdef __cplusplus

}
#endif

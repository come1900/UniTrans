/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * GeminiLock.cpp - brief
 *
 * Copyright (C) 2013 ezlibs.com, All Rights Reserved.
 *
 * $Id: GeminiLock.cpp 5884 2014-02-09 11:39:34Z WuJunjie $
 *
 *  Notes:
 *     -
 *      示例基本使用过程
 *     -
 *
 *  Update:
 *     2014-02-09 11:39:34 WuJunjie Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#include <string.h>

#ifdef LINUX
#include <sys/time.h> /*gettimeofday*/
#endif //LINUX

#include <iostream>
//#include <vector>
//
//#include <openssl/rsa.h>
//#include <openssl/pem.h>
//#include <openssl/err.h>

//#include "GeminiKey/GeminiKey.h"
#include "../GeminiKeyStack.h"

#include <str_opr.h>
#include <base64.h>
#include <md5.h>
#include <ez_system_api.h>

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

//	sqlite> select keyLock from GeminiKey  limit 1;
const std::string g_strKeyLock = "susynVJglYkMfUf0TXm44MnelFSB+SLsYRNYYvbjSZxSdLFxLIHNjQJh5gjnYSecwapCrzmXUQT+NYKwgwpvdWQWviDgEm8aVxnRKgF6+jvqr26e0J1Y7ObVpHd7wEl6aGDSox3UwoMr7nrx79TEpxl3cW+UI0JXwxRzudAyQLJlWddW0di0z01bGqF7qXmiIqmtajZCAcwYJBKcukBkEZH6maBLAOww42iJSFM3Ocw8NMC1JN2tJBmHUZawoCKTrSW0hRvQKzxW41ftxsFJcJnaGBFBj9shq3LgtZ8YIWe3nEwSHntlzCtulNKlOp27c20hUbSWN1KPCZJDyoz9J6qkN6VYXUTkKZllZc2ITIlTFdxgxbt7vxu+oElHQUdn3j608Zo8mUOPImkVHoSTrNBMkydtm0D0118+IVXCoN50phLCfwAi9VOa86WKwbM+C5kqr90hrn83tnT31NMAl1QRVW6Yrv704Q2RIDUERmS/Xl8ksEh48vTxZf+x39oaSTbuAE6AM0tZmT1J8c13237oEavU+M+sz8y/wV9+As0tcxWA+OFkvyXrhA2akAipb/aQlCSXOtiYk09qMDFMeRe/yXgSk6qOtfQH3YwWZXCPdJV8k3gl63RBZ7jNFJH4H9yrvJA62Rs=";

//sqlite> select keyUnlock from GeminiKey  limit 1;
const std::string g_strKeyUnLock = "gya/DGER8Z3Ph+9rcDIH2e5+6tbejGH4Bx9X/4wT1HHaVYVKnMoqj7SAEi8sihCiFzSF/32uhvr3QRevHIWiq0n6kp33UdGjOKuvxsLhsAHbF6mUNXxieGvM1eHr+m6opdDOQuc+FyHoUHdnR6JL1KynupqijoCXLicYNU7XYJqQR2uptnSf1X/ypjN6211U+7SyiCIsgXJgb6SGeQNU+RxgU/HqdOnkvC0BTz/DBQTPXhxq2cbK4f8T8b7QpSMXbK+nuarCCfL/p+LbJFEylH+S4QvQf58yPxOuqbntOU5DjvhCfPlRzgqDjkbEA+Kfm8GeZEzrLBl6eNKjN7IigNt0SZg/8i4yWSgzMC72YoZiKJdC3Rl7av+uL3QhGNkRx60oakzvelRiyVjtuAbRcgVDDo13WhBc1MysoDRMaOx+kmMfOIzNNob26RjUuL2P9QIgGc4oihK4/NuLeVjSKCeC6+WVMtP5gzeH5HMCFd39RbxmGBnv+8sah0JuboFfMJAbINS/0SonY5+SeAr9T3YaykyPZNWgfSytkpkmB02FHk4T+c4e81bGQsvRjmz6xLsHxTZCgPy2G52AFy4ZaS4WlZCo1dTArO3CxmshYWKzE8azzMKzW2hqJX/PRCrYEz7DR3nFJu0dJn3k549psfIuPfjUxLIao4pIL30z0H706K6GbOmuNHlS8qBTxMBCqFbK/u853uGgpT1PPOrboEtczAJEldyKrnSYrdExnqzQjvjg4OaH7flwFJGS3aoW7huOtIHNscxlqKSWsrmORaGuz3EaZpjyc7mpM2agMphIWBn8UKH1mvvZp7ST+wqeM7mTPsivRii29A0Ss9yhWyBx0kfQRj+fHjKPGS/aXjcfX4+b8Z3W8dmJSOrEcENGyxRt3ZMOoGAV/3qk9wdpmzjuxa5m36T4kb9UdWv3Bvuhv2MZaA8kfZpqm5fKi6lRz7THnqVDlhTkOh9ofhqjesymOWU9Aws86xWdF8dZQ7f+33Yx/EuqKvpTv3VDiIYrAxC4THZjbgS2HG4lXyneEOhu84b9tRstTKSR4JLV9J8gVa8t2Bb2AgM+Ias3fegwFh4NlR6vZHaNH3joaQkt5nmHifgPEO1IV3tDP5UjVPAIG20E3Nw6CBNJmOZAvDHqKY1v5m8Z48/WqFGlFINvS+keTI4y3Mvkz04jb+08P7BAFBWLyfADNMSoP35gJDeJbVjWZXoJBzhQAXR3oWla/II7I96dVARuUmCfkJs9U+YCXf4UoQwUe4UEGmRS7H9X89LBLbBReSKjhYOx9txvpD+0C60aN56yUmXjcLAU7ddZaKXYf8/0VnHdFW4ixQwVuL2fjwlibHNwpDMywc8vb82UR5f1Dp3lVo2vfvFHdvXQ7AoImwDehjwwW328Lzf/WxCPvHVOljb2/ELziS8ET3r//d0VTVHhwTgQ7p5o11y9MQaqLi6AttJVkRMQYrymb2yMQwICqETlxT6Ke4wFNqCsoKOTV0b3E6cL4yI0keDchBcvniz9WcYt/+uwcuC7mSAROzFKPBkTSIGS6ya7vaMCyRNNlmIRyg8a/kbgwS66w3LQfVyfq+0wjOipBh1FzI92AytEsp7mGumRxj24Y1k+VVn1sWuBRQhHonuf5+0vlFKmbOzbJZ/+TBGQylak7yJgdX2ZGv9auZAkxteW2ED2strUjcVWkEYYyYAzIwyw9KKPYQLUn5e9hZzfbGistntkQUw/V7Yj0TEfvd/6sBdTz+eEuJlUytFDiYbzFGia61mD15IObiTRH1i72RPpErLEtIm90MqanbUOQZ4jUeXPjzkQEgndxlRtvoQky2CGTOwP8UML9BGWklQFnwN1Pxrg9W8rHyg2hiY75eG/F0p55V/EjY9CxxZ0039ZJ/a3YiTX2bBj/LJwm0uVMdxJ";

int useGeminiKey();
int tstLockConv2Struct(std::string &PlainText, std::string &LockedData);
int tstUnLockConv2Struct(const std::string &PlainText, const std::string &LockedData);

int tstLockUseStringKey(const std::string &strKeyLock, std::string &PlainText, std::string &LockedData);
int tstUnLockUseStringKey(const std::string &strKeyUnLock, const std::string &PlainText, const std::string &LockedData);

int main(int argc, char *argv[])
{
    int iTimes = 0;

    struct timeval tv_main_start;
    struct timeval tv_start;
    struct timeval tv_now;
    struct timeval t_sub_time;

    gettimeofday(&tv_main_start, NULL);

    std::string PlainText = "23818980895812659251";
    std::string LockedData ;//= "c2vcPfyasXQvzJRuvD8bGeKa/4nbBHm7bBPGYlk0r44EtmWLK3PUK5QHff5A2lzHvUllSAtOaPredmFg4qu7DTqsEj2He84NM9BFPcDU9LtRgdYB7CMGw1NxctzRJYG+bHEYqMymFAkCqj6expQjM6h3QUoV3O3Jg24aqXadQ/8=";

    //for(iTimes=0;;iTimes++)
    {
        std::cout << "useGeminiKey >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Run iTimes:" << iTimes << std::endl;
        iTimes++;

        gettimeofday(&tv_start, NULL);
        /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
        useGeminiKey();
        /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

        gettimeofday(&tv_now, NULL);
        ez_timeval_subtract(&t_sub_time, &tv_now, &tv_start);

        struct timeval tv_tmp_elipse;
        //gettimeofday(&tv_now, NULL);
        ez_timeval_subtract(&tv_tmp_elipse, &tv_now, &tv_main_start);

        __fline;
        printf("testGeminiKey used:%5d(ms), runed:%15d(ms)\n"
               ,  (int)(t_sub_time.tv_sec*1000 + t_sub_time.tv_usec / 1000.0)
               ,  (int)(tv_tmp_elipse.tv_sec*1000 + tv_tmp_elipse.tv_usec / 1000.0));

    }
    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

    {
        std::cout << "tstLockConv2Struct >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Run iTimes:" << iTimes << std::endl;
        iTimes++;

        gettimeofday(&tv_start, NULL);
        /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
        tstLockConv2Struct(PlainText, LockedData);
        /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

        gettimeofday(&tv_now, NULL);
        ez_timeval_subtract(&t_sub_time, &tv_now, &tv_start);

        struct timeval tv_tmp_elipse;
        //gettimeofday(&tv_now, NULL);
        ez_timeval_subtract(&tv_tmp_elipse, &tv_now, &tv_main_start);

        __fline;
        printf("testGeminiKey used:%5d(ms), runed:%15d(ms)\n"
               ,  (int)(t_sub_time.tv_sec*1000 + t_sub_time.tv_usec / 1000.0)
               ,  (int)(tv_tmp_elipse.tv_sec*1000 + tv_tmp_elipse.tv_usec / 1000.0));

    }
    {
        std::cout << "tstUnLockConv2Struct >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Run iTimes:" << iTimes << std::endl;
        iTimes++;

        gettimeofday(&tv_start, NULL);
        /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
        tstUnLockConv2Struct(PlainText, LockedData);
        /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

        gettimeofday(&tv_now, NULL);
        ez_timeval_subtract(&t_sub_time, &tv_now, &tv_start);

        struct timeval tv_tmp_elipse;
        //gettimeofday(&tv_now, NULL);
        ez_timeval_subtract(&tv_tmp_elipse, &tv_now, &tv_main_start);

        __fline;
        printf("testGeminiKey used:%5d(ms), runed:%15d(ms)\n"
               ,  (int)(t_sub_time.tv_sec*1000 + t_sub_time.tv_usec / 1000.0)
               ,  (int)(tv_tmp_elipse.tv_sec*1000 + tv_tmp_elipse.tv_usec / 1000.0));

    }
    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
    // 下面锁、开是 一对
    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
    {
        std::cout << "tstLockUseStringKey >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Run iTimes:" << iTimes << std::endl;
        iTimes++;

        gettimeofday(&tv_start, NULL);
        /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
        tstLockUseStringKey(g_strKeyLock, PlainText, LockedData);

#if 0
                std::cout 	\
                	<< "g_strKeyLock:" << std::endl << g_strKeyLock << std::endl	\
                	<< "PlainText:" << std::endl << PlainText << std::endl		\
                	<< "LockedData:" << std::endl << LockedData << std::endl;
#endif

        /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

        gettimeofday(&tv_now, NULL);
        ez_timeval_subtract(&t_sub_time, &tv_now, &tv_start);

        struct timeval tv_tmp_elipse;
        //gettimeofday(&tv_now, NULL);
        ez_timeval_subtract(&tv_tmp_elipse, &tv_now, &tv_main_start);

        __fline;
        printf("testGeminiKey used:%5d(ms), runed:%15d(ms)\n"
               ,  (int)(t_sub_time.tv_sec*1000 + t_sub_time.tv_usec / 1000.0)
               ,  (int)(tv_tmp_elipse.tv_sec*1000 + tv_tmp_elipse.tv_usec / 1000.0));

    }
    {
        std::cout << "tstUnLockUseStringKey >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Run iTimes:" << iTimes << std::endl;
        iTimes++;

        gettimeofday(&tv_start, NULL);
        /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

        tstUnLockUseStringKey(g_strKeyUnLock, PlainText, LockedData);

#if 0
                std::cout 	\
                	<< "g_strKeyUnLock:" << std::endl << g_strKeyUnLock << std::endl	\
                	<< "PlainText:" << std::endl << PlainText << std::endl		\
                	<< "LockedData:" << std::endl << LockedData << std::endl;
#endif
        /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

        gettimeofday(&tv_now, NULL);
        ez_timeval_subtract(&t_sub_time, &tv_now, &tv_start);

        struct timeval tv_tmp_elipse;
        //gettimeofday(&tv_now, NULL);
        ez_timeval_subtract(&tv_tmp_elipse, &tv_now, &tv_main_start);

        __fline;
        printf("testGeminiKey used:%5d(ms), runed:%15d(ms)\n"
               ,  (int)(t_sub_time.tv_sec*1000 + t_sub_time.tv_usec / 1000.0)
               ,  (int)(tv_tmp_elipse.tv_sec*1000 + tv_tmp_elipse.tv_usec / 1000.0));

    }

    return 0;
}
// 示例基本使用过程
/*
1. 已有： lock key / unlock key / any string
2. Lock 记录Any string / Locked String
3. Unlock
*/
int useGeminiKey()
{
    int iret;

    //	sqlite> select keyLock from GeminiKey  limit 1;
    std::string strKeyLock = "susynVJglYkMfUf0TXm44MnelFSB+SLsYRNYYvbjSZxSdLFxLIHNjQJh5gjnYSecwapCrzmXUQT+NYKwgwpvdWQWviDgEm8aVxnRKgF6+jvqr26e0J1Y7ObVpHd7wEl6aGDSox3UwoMr7nrx79TEpxl3cW+UI0JXwxRzudAyQLJlWddW0di0z01bGqF7qXmiIqmtajZCAcwYJBKcukBkEZH6maBLAOww42iJSFM3Ocw8NMC1JN2tJBmHUZawoCKTrSW0hRvQKzxW41ftxsFJcJnaGBFBj9shq3LgtZ8YIWe3nEwSHntlzCtulNKlOp27c20hUbSWN1KPCZJDyoz9J6qkN6VYXUTkKZllZc2ITIlTFdxgxbt7vxu+oElHQUdn3j608Zo8mUOPImkVHoSTrNBMkydtm0D0118+IVXCoN50phLCfwAi9VOa86WKwbM+C5kqr90hrn83tnT31NMAl1QRVW6Yrv704Q2RIDUERmS/Xl8ksEh48vTxZf+x39oaSTbuAE6AM0tZmT1J8c13237oEavU+M+sz8y/wV9+As0tcxWA+OFkvyXrhA2akAipb/aQlCSXOtiYk09qMDFMeRe/yXgSk6qOtfQH3YwWZXCPdJV8k3gl63RBZ7jNFJH4H9yrvJA62Rs=";

    //sqlite> select keyUnlock from GeminiKey  limit 1;
    std::string strKeyUnLock = "gya/DGER8Z3Ph+9rcDIH2e5+6tbejGH4Bx9X/4wT1HHaVYVKnMoqj7SAEi8sihCiFzSF/32uhvr3QRevHIWiq0n6kp33UdGjOKuvxsLhsAHbF6mUNXxieGvM1eHr+m6opdDOQuc+FyHoUHdnR6JL1KynupqijoCXLicYNU7XYJqQR2uptnSf1X/ypjN6211U+7SyiCIsgXJgb6SGeQNU+RxgU/HqdOnkvC0BTz/DBQTPXhxq2cbK4f8T8b7QpSMXbK+nuarCCfL/p+LbJFEylH+S4QvQf58yPxOuqbntOU5DjvhCfPlRzgqDjkbEA+Kfm8GeZEzrLBl6eNKjN7IigNt0SZg/8i4yWSgzMC72YoZiKJdC3Rl7av+uL3QhGNkRx60oakzvelRiyVjtuAbRcgVDDo13WhBc1MysoDRMaOx+kmMfOIzNNob26RjUuL2P9QIgGc4oihK4/NuLeVjSKCeC6+WVMtP5gzeH5HMCFd39RbxmGBnv+8sah0JuboFfMJAbINS/0SonY5+SeAr9T3YaykyPZNWgfSytkpkmB02FHk4T+c4e81bGQsvRjmz6xLsHxTZCgPy2G52AFy4ZaS4WlZCo1dTArO3CxmshYWKzE8azzMKzW2hqJX/PRCrYEz7DR3nFJu0dJn3k549psfIuPfjUxLIao4pIL30z0H706K6GbOmuNHlS8qBTxMBCqFbK/u853uGgpT1PPOrboEtczAJEldyKrnSYrdExnqzQjvjg4OaH7flwFJGS3aoW7huOtIHNscxlqKSWsrmORaGuz3EaZpjyc7mpM2agMphIWBn8UKH1mvvZp7ST+wqeM7mTPsivRii29A0Ss9yhWyBx0kfQRj+fHjKPGS/aXjcfX4+b8Z3W8dmJSOrEcENGyxRt3ZMOoGAV/3qk9wdpmzjuxa5m36T4kb9UdWv3Bvuhv2MZaA8kfZpqm5fKi6lRz7THnqVDlhTkOh9ofhqjesymOWU9Aws86xWdF8dZQ7f+33Yx/EuqKvpTv3VDiIYrAxC4THZjbgS2HG4lXyneEOhu84b9tRstTKSR4JLV9J8gVa8t2Bb2AgM+Ias3fegwFh4NlR6vZHaNH3joaQkt5nmHifgPEO1IV3tDP5UjVPAIG20E3Nw6CBNJmOZAvDHqKY1v5m8Z48/WqFGlFINvS+keTI4y3Mvkz04jb+08P7BAFBWLyfADNMSoP35gJDeJbVjWZXoJBzhQAXR3oWla/II7I96dVARuUmCfkJs9U+YCXf4UoQwUe4UEGmRS7H9X89LBLbBReSKjhYOx9txvpD+0C60aN56yUmXjcLAU7ddZaKXYf8/0VnHdFW4ixQwVuL2fjwlibHNwpDMywc8vb82UR5f1Dp3lVo2vfvFHdvXQ7AoImwDehjwwW328Lzf/WxCPvHVOljb2/ELziS8ET3r//d0VTVHhwTgQ7p5o11y9MQaqLi6AttJVkRMQYrymb2yMQwICqETlxT6Ke4wFNqCsoKOTV0b3E6cL4yI0keDchBcvniz9WcYt/+uwcuC7mSAROzFKPBkTSIGS6ya7vaMCyRNNlmIRyg8a/kbgwS66w3LQfVyfq+0wjOipBh1FzI92AytEsp7mGumRxj24Y1k+VVn1sWuBRQhHonuf5+0vlFKmbOzbJZ/+TBGQylak7yJgdX2ZGv9auZAkxteW2ED2strUjcVWkEYYyYAzIwyw9KKPYQLUn5e9hZzfbGistntkQUw/V7Yj0TEfvd/6sBdTz+eEuJlUytFDiYbzFGia61mD15IObiTRH1i72RPpErLEtIm90MqanbUOQZ4jUeXPjzkQEgndxlRtvoQky2CGTOwP8UML9BGWklQFnwN1Pxrg9W8rHyg2hiY75eG/F0p55V/EjY9CxxZ0039ZJ/a3YiTX2bBj/LJwm0uVMdxJ";


    // 准备锁钥匙
    GeminiKeys __KeysGenLock;
    CGeminiKeyStack CGeminiKeyStack_testImportLock;
    CGeminiKeyStack_testImportLock.GeminiKeyImport(strKeyLock);
    CGeminiKeyStack_testImportLock.Decode(__KeysGenLock);
    //__KeysGenLock.dump();

    //准备开锁钥匙
    GeminiKeys __KeysGenUnlock;
    CGeminiKeyStack CGeminiKeyStack_testImportUnlock;
    CGeminiKeyStack_testImportUnlock.GeminiKeyImport(strKeyUnLock);
    CGeminiKeyStack_testImportUnlock.Decode(__KeysGenUnlock);
    //__KeysGenUnlock.dump();

    //准备任意字串，不得超过245Bytes
    std::string PlainText = "23818980895812659251";
    std::string LockedData ;//= "c2vcPfyasXQvzJRuvD8bGeKa/4nbBHm7bBPGYlk0r44EtmWLK3PUK5QHff5A2lzHvUllSAtOaPredmFg4qu7DTqsEj2He84NM9BFPcDU9LtRgdYB7CMGw1NxctzRJYG+bHEYqMymFAkCqj6expQjM6h3QUoV3O3Jg24aqXadQ/8=";

    iret = CGeminiKeyStack_testImportLock.GeminiLock(__KeysGenLock, PlainText, LockedData);
    std::cout << "CGeminiKeyStack_testImportLock.GeminiLock " << std::string(iret==0?"Succeeded":"Failed") << std::endl;
    //std::cout << LockedData << std::endl;
    //锁门后， 将上述两个字符串记录起来， 被开锁校验。

    iret = CGeminiKeyStack_testImportUnlock.GeminiUnlock(__KeysGenUnlock, PlainText, LockedData);
    std::cout << "CGeminiKeyStack_testImportUnlock.GeminiUnlock " << std::string(iret==0?"Succeeded":"Failed") << std::endl;

    return 0;
}

int tstLockConv2Struct(std::string &PlainText, std::string &LockedData)
{
    int iret;

    //	sqlite> select keyLock from GeminiKey  limit 1;
    std::string strKeyLock = "susynVJglYkMfUf0TXm44MnelFSB+SLsYRNYYvbjSZxSdLFxLIHNjQJh5gjnYSecwapCrzmXUQT+NYKwgwpvdWQWviDgEm8aVxnRKgF6+jvqr26e0J1Y7ObVpHd7wEl6aGDSox3UwoMr7nrx79TEpxl3cW+UI0JXwxRzudAyQLJlWddW0di0z01bGqF7qXmiIqmtajZCAcwYJBKcukBkEZH6maBLAOww42iJSFM3Ocw8NMC1JN2tJBmHUZawoCKTrSW0hRvQKzxW41ftxsFJcJnaGBFBj9shq3LgtZ8YIWe3nEwSHntlzCtulNKlOp27c20hUbSWN1KPCZJDyoz9J6qkN6VYXUTkKZllZc2ITIlTFdxgxbt7vxu+oElHQUdn3j608Zo8mUOPImkVHoSTrNBMkydtm0D0118+IVXCoN50phLCfwAi9VOa86WKwbM+C5kqr90hrn83tnT31NMAl1QRVW6Yrv704Q2RIDUERmS/Xl8ksEh48vTxZf+x39oaSTbuAE6AM0tZmT1J8c13237oEavU+M+sz8y/wV9+As0tcxWA+OFkvyXrhA2akAipb/aQlCSXOtiYk09qMDFMeRe/yXgSk6qOtfQH3YwWZXCPdJV8k3gl63RBZ7jNFJH4H9yrvJA62Rs=";

    //sqlite> select keyUnlock from GeminiKey  limit 1;
    std::string strKeyUnLock = "gya/DGER8Z3Ph+9rcDIH2e5+6tbejGH4Bx9X/4wT1HHaVYVKnMoqj7SAEi8sihCiFzSF/32uhvr3QRevHIWiq0n6kp33UdGjOKuvxsLhsAHbF6mUNXxieGvM1eHr+m6opdDOQuc+FyHoUHdnR6JL1KynupqijoCXLicYNU7XYJqQR2uptnSf1X/ypjN6211U+7SyiCIsgXJgb6SGeQNU+RxgU/HqdOnkvC0BTz/DBQTPXhxq2cbK4f8T8b7QpSMXbK+nuarCCfL/p+LbJFEylH+S4QvQf58yPxOuqbntOU5DjvhCfPlRzgqDjkbEA+Kfm8GeZEzrLBl6eNKjN7IigNt0SZg/8i4yWSgzMC72YoZiKJdC3Rl7av+uL3QhGNkRx60oakzvelRiyVjtuAbRcgVDDo13WhBc1MysoDRMaOx+kmMfOIzNNob26RjUuL2P9QIgGc4oihK4/NuLeVjSKCeC6+WVMtP5gzeH5HMCFd39RbxmGBnv+8sah0JuboFfMJAbINS/0SonY5+SeAr9T3YaykyPZNWgfSytkpkmB02FHk4T+c4e81bGQsvRjmz6xLsHxTZCgPy2G52AFy4ZaS4WlZCo1dTArO3CxmshYWKzE8azzMKzW2hqJX/PRCrYEz7DR3nFJu0dJn3k549psfIuPfjUxLIao4pIL30z0H706K6GbOmuNHlS8qBTxMBCqFbK/u853uGgpT1PPOrboEtczAJEldyKrnSYrdExnqzQjvjg4OaH7flwFJGS3aoW7huOtIHNscxlqKSWsrmORaGuz3EaZpjyc7mpM2agMphIWBn8UKH1mvvZp7ST+wqeM7mTPsivRii29A0Ss9yhWyBx0kfQRj+fHjKPGS/aXjcfX4+b8Z3W8dmJSOrEcENGyxRt3ZMOoGAV/3qk9wdpmzjuxa5m36T4kb9UdWv3Bvuhv2MZaA8kfZpqm5fKi6lRz7THnqVDlhTkOh9ofhqjesymOWU9Aws86xWdF8dZQ7f+33Yx/EuqKvpTv3VDiIYrAxC4THZjbgS2HG4lXyneEOhu84b9tRstTKSR4JLV9J8gVa8t2Bb2AgM+Ias3fegwFh4NlR6vZHaNH3joaQkt5nmHifgPEO1IV3tDP5UjVPAIG20E3Nw6CBNJmOZAvDHqKY1v5m8Z48/WqFGlFINvS+keTI4y3Mvkz04jb+08P7BAFBWLyfADNMSoP35gJDeJbVjWZXoJBzhQAXR3oWla/II7I96dVARuUmCfkJs9U+YCXf4UoQwUe4UEGmRS7H9X89LBLbBReSKjhYOx9txvpD+0C60aN56yUmXjcLAU7ddZaKXYf8/0VnHdFW4ixQwVuL2fjwlibHNwpDMywc8vb82UR5f1Dp3lVo2vfvFHdvXQ7AoImwDehjwwW328Lzf/WxCPvHVOljb2/ELziS8ET3r//d0VTVHhwTgQ7p5o11y9MQaqLi6AttJVkRMQYrymb2yMQwICqETlxT6Ke4wFNqCsoKOTV0b3E6cL4yI0keDchBcvniz9WcYt/+uwcuC7mSAROzFKPBkTSIGS6ya7vaMCyRNNlmIRyg8a/kbgwS66w3LQfVyfq+0wjOipBh1FzI92AytEsp7mGumRxj24Y1k+VVn1sWuBRQhHonuf5+0vlFKmbOzbJZ/+TBGQylak7yJgdX2ZGv9auZAkxteW2ED2strUjcVWkEYYyYAzIwyw9KKPYQLUn5e9hZzfbGistntkQUw/V7Yj0TEfvd/6sBdTz+eEuJlUytFDiYbzFGia61mD15IObiTRH1i72RPpErLEtIm90MqanbUOQZ4jUeXPjzkQEgndxlRtvoQky2CGTOwP8UML9BGWklQFnwN1Pxrg9W8rHyg2hiY75eG/F0p55V/EjY9CxxZ0039ZJ/a3YiTX2bBj/LJwm0uVMdxJ";

    // 准备锁钥匙
    GeminiKeys __KeysGenLock;
    CGeminiKeyStack CGeminiKeyStack_testImportLock;
    CGeminiKeyStack_testImportLock.GeminiKeyImport(strKeyLock);
    CGeminiKeyStack_testImportLock.Decode(__KeysGenLock);
    //__KeysGenLock.dump();



    //准备任意字串，不得超过245Bytes
    //    std::string PlainText = "23818980895812659251";
    //    std::string LockedData ;//= "c2vcPfyasXQvzJRuvD8bGeKa/4nbBHm7bBPGYlk0r44EtmWLK3PUK5QHff5A2lzHvUllSAtOaPredmFg4qu7DTqsEj2He84NM9BFPcDU9LtRgdYB7CMGw1NxctzRJYG+bHEYqMymFAkCqj6expQjM6h3QUoV3O3Jg24aqXadQ/8=";

    iret = CGeminiKeyStack_testImportLock.GeminiLock(__KeysGenLock, PlainText, LockedData);
    std::cout << "CGeminiKeyStack_testImportLock.GeminiLock " << std::string(iret==0?"Succeeded":"Failed") << std::endl;
    //std::cout << LockedData << std::endl;

    return 0;
}

int tstUnLockConv2Struct(const std::string &PlainText, const std::string &LockedData)
{
    int iret;

    //	sqlite> select keyLock from GeminiKey  limit 1;
    std::string strKeyLock = "susynVJglYkMfUf0TXm44MnelFSB+SLsYRNYYvbjSZxSdLFxLIHNjQJh5gjnYSecwapCrzmXUQT+NYKwgwpvdWQWviDgEm8aVxnRKgF6+jvqr26e0J1Y7ObVpHd7wEl6aGDSox3UwoMr7nrx79TEpxl3cW+UI0JXwxRzudAyQLJlWddW0di0z01bGqF7qXmiIqmtajZCAcwYJBKcukBkEZH6maBLAOww42iJSFM3Ocw8NMC1JN2tJBmHUZawoCKTrSW0hRvQKzxW41ftxsFJcJnaGBFBj9shq3LgtZ8YIWe3nEwSHntlzCtulNKlOp27c20hUbSWN1KPCZJDyoz9J6qkN6VYXUTkKZllZc2ITIlTFdxgxbt7vxu+oElHQUdn3j608Zo8mUOPImkVHoSTrNBMkydtm0D0118+IVXCoN50phLCfwAi9VOa86WKwbM+C5kqr90hrn83tnT31NMAl1QRVW6Yrv704Q2RIDUERmS/Xl8ksEh48vTxZf+x39oaSTbuAE6AM0tZmT1J8c13237oEavU+M+sz8y/wV9+As0tcxWA+OFkvyXrhA2akAipb/aQlCSXOtiYk09qMDFMeRe/yXgSk6qOtfQH3YwWZXCPdJV8k3gl63RBZ7jNFJH4H9yrvJA62Rs=";

    //sqlite> select keyUnlock from GeminiKey  limit 1;
    std::string strKeyUnLock = "gya/DGER8Z3Ph+9rcDIH2e5+6tbejGH4Bx9X/4wT1HHaVYVKnMoqj7SAEi8sihCiFzSF/32uhvr3QRevHIWiq0n6kp33UdGjOKuvxsLhsAHbF6mUNXxieGvM1eHr+m6opdDOQuc+FyHoUHdnR6JL1KynupqijoCXLicYNU7XYJqQR2uptnSf1X/ypjN6211U+7SyiCIsgXJgb6SGeQNU+RxgU/HqdOnkvC0BTz/DBQTPXhxq2cbK4f8T8b7QpSMXbK+nuarCCfL/p+LbJFEylH+S4QvQf58yPxOuqbntOU5DjvhCfPlRzgqDjkbEA+Kfm8GeZEzrLBl6eNKjN7IigNt0SZg/8i4yWSgzMC72YoZiKJdC3Rl7av+uL3QhGNkRx60oakzvelRiyVjtuAbRcgVDDo13WhBc1MysoDRMaOx+kmMfOIzNNob26RjUuL2P9QIgGc4oihK4/NuLeVjSKCeC6+WVMtP5gzeH5HMCFd39RbxmGBnv+8sah0JuboFfMJAbINS/0SonY5+SeAr9T3YaykyPZNWgfSytkpkmB02FHk4T+c4e81bGQsvRjmz6xLsHxTZCgPy2G52AFy4ZaS4WlZCo1dTArO3CxmshYWKzE8azzMKzW2hqJX/PRCrYEz7DR3nFJu0dJn3k549psfIuPfjUxLIao4pIL30z0H706K6GbOmuNHlS8qBTxMBCqFbK/u853uGgpT1PPOrboEtczAJEldyKrnSYrdExnqzQjvjg4OaH7flwFJGS3aoW7huOtIHNscxlqKSWsrmORaGuz3EaZpjyc7mpM2agMphIWBn8UKH1mvvZp7ST+wqeM7mTPsivRii29A0Ss9yhWyBx0kfQRj+fHjKPGS/aXjcfX4+b8Z3W8dmJSOrEcENGyxRt3ZMOoGAV/3qk9wdpmzjuxa5m36T4kb9UdWv3Bvuhv2MZaA8kfZpqm5fKi6lRz7THnqVDlhTkOh9ofhqjesymOWU9Aws86xWdF8dZQ7f+33Yx/EuqKvpTv3VDiIYrAxC4THZjbgS2HG4lXyneEOhu84b9tRstTKSR4JLV9J8gVa8t2Bb2AgM+Ias3fegwFh4NlR6vZHaNH3joaQkt5nmHifgPEO1IV3tDP5UjVPAIG20E3Nw6CBNJmOZAvDHqKY1v5m8Z48/WqFGlFINvS+keTI4y3Mvkz04jb+08P7BAFBWLyfADNMSoP35gJDeJbVjWZXoJBzhQAXR3oWla/II7I96dVARuUmCfkJs9U+YCXf4UoQwUe4UEGmRS7H9X89LBLbBReSKjhYOx9txvpD+0C60aN56yUmXjcLAU7ddZaKXYf8/0VnHdFW4ixQwVuL2fjwlibHNwpDMywc8vb82UR5f1Dp3lVo2vfvFHdvXQ7AoImwDehjwwW328Lzf/WxCPvHVOljb2/ELziS8ET3r//d0VTVHhwTgQ7p5o11y9MQaqLi6AttJVkRMQYrymb2yMQwICqETlxT6Ke4wFNqCsoKOTV0b3E6cL4yI0keDchBcvniz9WcYt/+uwcuC7mSAROzFKPBkTSIGS6ya7vaMCyRNNlmIRyg8a/kbgwS66w3LQfVyfq+0wjOipBh1FzI92AytEsp7mGumRxj24Y1k+VVn1sWuBRQhHonuf5+0vlFKmbOzbJZ/+TBGQylak7yJgdX2ZGv9auZAkxteW2ED2strUjcVWkEYYyYAzIwyw9KKPYQLUn5e9hZzfbGistntkQUw/V7Yj0TEfvd/6sBdTz+eEuJlUytFDiYbzFGia61mD15IObiTRH1i72RPpErLEtIm90MqanbUOQZ4jUeXPjzkQEgndxlRtvoQky2CGTOwP8UML9BGWklQFnwN1Pxrg9W8rHyg2hiY75eG/F0p55V/EjY9CxxZ0039ZJ/a3YiTX2bBj/LJwm0uVMdxJ";



    //准备开锁钥匙
    GeminiKeys __KeysGenUnlock;
    CGeminiKeyStack CGeminiKeyStack_testImportUnlock;
    CGeminiKeyStack_testImportUnlock.GeminiKeyImport(strKeyUnLock);
    CGeminiKeyStack_testImportUnlock.Decode(__KeysGenUnlock);
    //__KeysGenUnlock.dump();


    iret = CGeminiKeyStack_testImportUnlock.GeminiUnlock(__KeysGenUnlock, PlainText, LockedData);
    std::cout << "CGeminiKeyStack_testImportUnlock.GeminiUnlock " << std::string(iret==0?"Succeeded":"Failed") << std::endl;

    return 0;
}

int tstLockUseStringKey(const std::string &strKeyLock, std::string &PlainText, std::string &LockedData)
{
    int iret;

    CGeminiKeyStack CGeminiKeyStack_testImportLock;

    iret = CGeminiKeyStack_testImportLock.GeminiLock(strKeyLock, PlainText, LockedData);
    std::cout << "CGeminiKeyStack_testImportLock.GeminiLock " << std::string(iret==0?"Succeeded":"Failed") << std::endl;

    return 0;
}

int tstUnLockUseStringKey(const std::string &strKeyUnLock, const std::string &PlainText, const std::string &LockedData)
{
    int iret;
    CGeminiKeyStack CGeminiKeyStack_testImportUnlock;
    iret = CGeminiKeyStack_testImportUnlock.GeminiUnlock(strKeyUnLock, PlainText, LockedData);
    std::cout << "CGeminiKeyStack_testImportUnlock.GeminiUnlock " << std::string(iret==0?"Succeeded":"Failed") << std::endl;

    return 0;
}

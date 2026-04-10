/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * TestMan.cpp - _explain_
 *
 * Copyright (C) 2012 tiansu-china.com, All Rights Reserved.
 *
 * $Id: TestMan.cpp 0001 2012-5-8 14:38:29Z wu_junjie $
 *
 *  Explain:
 *     TeHomeMsg -- Telecom eHome Msg
 *
 *  Update:
 *     2012-5-8 14:38:33 WuJunjie 549 Create
 *
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
#ifdef LINUX
#include <sys/time.h> /*gettimeofday*/
#endif //LINUX


#include "TestMan.h"

#include "../GeminiKeyStack.h"

#ifndef _DEBUG_THIS
//#define _DEBUG_THIS
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

CPPUNIT_TEST_SUITE_REGISTRATION( CTestMan );

void CTestMan::testGeminiKeyFullFunc()
{
    CGeminiKeyStack __CGeminiKeyStackCreate;
    CGeminiKeyStack __CGeminiKeyStackLock;
    CGeminiKeyStack __CGeminiKeyStackUnlock;

    GeminiKey Lock;
    GeminiKey Unlock;

    __CGeminiKeyStackCreate.GeminiKeyCreate(Lock, Unlock, "13359211641994461155", 2048);
    //__CGeminiKeyStackCreate.GeminiKeyCreate(Lock, Unlock, "13359211641994461155", "", "testGeminiKeyFullFunc", 1024);

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

void CTestMan::testGeminiKeyCreate                 ()
{
    CGeminiKeyStack __CGeminiKeyStackTmp;
    CGeminiKeyStack __CGeminiKeyStackLock;
    CGeminiKeyStack __CGeminiKeyStackUnlock;

    struct timeval tv_main_start;
    struct timeval tv_start;
    struct timeval tv_now;
    struct timeval t_sub_time;
    gettimeofday(&tv_main_start, NULL);

    GeminiKey Lock;
    GeminiKey Unlock;

    gettimeofday(&tv_start, NULL);
    __CGeminiKeyStackTmp.GeminiKeyCreate(Lock, Unlock, "13359211641994461155", 2048);
    //__CGeminiKeyStackTmp.GeminiKeyCreate(Lock, Unlock, "13359211641994461155", "", "testGeminiKeyCreate", 1024);

    gettimeofday(&tv_now, NULL);
    ez_timeval_subtract(&t_sub_time, &tv_now, &tv_start);

    struct timeval tv_tmp_elipse;
    ez_timeval_subtract(&tv_tmp_elipse, &tv_now, &tv_main_start);

    //    __fline;
    //    printf("gemini_KeyCreate used:%5d(ms), runed:%15d(ms)\n"
    //           ,  (int)(t_sub_time.tv_sec*1000 + t_sub_time.tv_usec / 1000.0)
    //           ,  (int)(tv_tmp_elipse.tv_sec*1000 + tv_tmp_elipse.tv_usec / 1000.0));

    //Lock.dump();
    //Unlock.dump();

    GeminiKeys __KeysLock;
    __KeysLock.m_vecGeminiKey.push_back(Lock);
    __CGeminiKeyStackLock.Encode(__KeysLock);

    GeminiKeys __KeysUnlock;
    __KeysUnlock.m_vecGeminiKey.push_back(Unlock);
    __CGeminiKeyStackUnlock.Encode(__KeysUnlock);

    //std::string strLock = __CGeminiKeyStackLock.ToString();
    //std::cout << strLock << std::endl;
    //std::string strUnlock = __CGeminiKeyStackUnlock.ToString();
    //std::cout << strUnlock << std::endl;

    // 发行版Key
    bool bRet;
    // 发行版Key
    std::string strExportLock;
    bRet = __CGeminiKeyStackLock.GeminiKeyExport(strExportLock);
    CPPUNIT_ASSERT(bRet == true);

    std::string strExportUnlock;
    bRet = __CGeminiKeyStackUnlock.GeminiKeyExport(strExportUnlock);
    CPPUNIT_ASSERT(bRet == true);

#if 0

    __fline;
    std::cout << "GeminiKey Created:" << std::endl;
    std::cout << "--------------------------------------------- strExportLock(" << strExportLock.size() << ")" << std::endl;
    std::cout << strExportLock << std::endl;
    std::cout << "--------------------------------------------- strExportUnlock(" << strExportUnlock.size() << ")" << std::endl;
    std::cout << strExportUnlock << std::endl;
    std::cout << "---------------------------------------------" << std::endl;
#endif
}

void CTestMan::testGeminiKeyLock                 ()
{
    CGeminiKeyStack CGeminiKeyStack_testImportLock;

    // 下面这对密钥任意复制而来
    // 加锁钥
    std::string strExportLock = "tGkU7aaHcwX6dOSeOQ0Zz2948U9s+GpcY4lEXtlfQgNOTGUfKjlm6PhZjTGRmr2q18SayFcU7bDIC0WpU32Yex/qTyVU5LTa2Vu6S0TthISvU8l/znBC02qrOS4BOrGLUN6JIltptWBHeHshuaTiluTjRl7ECJ0l3HHGqjdNh1ZYeS1tfz3N00772zwibO9On3APEjlhOtIK9teqlpvO+CZVOjUsTdo0mWWRVkEZGxoAhBYw3jxcvLYYvZ6hkb9gpxekyJ1ZUQ/X+NE4nPf+5MlsSQCB1cprMjc51CEoAN2MxZj4m5UbjEX2hnzIjlTRxthQXjmAE7dAO8oM5BD5hqaIIguDpvZ0Xhbo3khDTpmxarb3RDeXy/GnbmAdohqUsfReC2Wec8HO51ZNBHhsy8UgcqjVTVWAMzZUnZiFIAFzsMrtR+XyEUWjdMb4FVgUmP5xU0aD/rbKNJU5RForqB0ODvAZtc5VsV8lEb2gt2vJOncqmGgyomb8QINYMDeumBaYM0U/KhwpBFg2TcAeAA7ZfZyIdevHvwTagwMYz3P8YNYQ3etSyPBxYaoBWmsHvcuZ4iMg8F8qkgyC2QRtJV/dEn1CFw3h5xiWmxpOHNkdjscfdXCcLA==";
/*
--------------------------------------------- strExportLock(633)
tGkU7aaHcwX6dOSeOQ0Zz2948U9s+GpcY4lEXtlfQgNOTGUfKjlm6PhZjTGRmr2q18SayFcU7bDIC0WpU32Yex/qTyVU5LTa2Vu6S0TthISvU8l/znBC02qrOS4BOrGLUN6JIltptWBHeHshuaTiluTjRl7ECJ0l3HHGqjdNh1ZYeS1tfz3N00772zwibO9On3APEjlhOtIK9teqlpvO+CZVOjUsTdo0mWWRVkEZGxoAhBYw3jxcvLYYvZ6hkb9gpxekyJ1ZUQ/X+NE4nPf+5MlsSQCB1cprMjc51CEoAN2MxZj4m5UbjEX2hnzIjlTRxthQXjmAE7dAO8oM5BD5hqaIIguDpvZ0Xhbo3khDTpmxarb3RDeXy/GnbmAdohqUsfReC2Wec8HO51ZNBHhsy8UgcqjVTVWAMzZUnZiFIAFzsMrtR+XyEUWjdMb4FVgUmP5xU0aD/rbKNJU5RForqB0ODvAZtc5VsV8lEb2gt2vJOncqmGgyomb8QINYMDeumBaYM0U/KhwpBFg2TcAeAA7ZfZyIdevHvwTagwMYz3P8YNYQ3etSyPBxYaoBWmsHvcuZ4iMg8F8qkgyC2QRtJV/dEn1CFw3h5xiWmxpOHNkdjscfdXCcLA==
--------------------------------------------- strExportUnlock(1901)
D6QU/JuxdWJUC0+9GeEyzq1WVMpIxUfGkZgogBBkRbwlHyJytKPxnYGloQZl5v2V6n5t2VyXsBxUhBWa2gJNkW3yUenr0r3V1riw0fu3vOfGWGO4/sfk40r1nMfaGp0hYulHMV/THs1mar5k4r+odhxmspfEeVpGeedT5HwAJd5HO56+JX+U+ze0ueeuQ818RBhnhzmeMiPqdVfkxZCN6JRG6hvzHQMFDdZeSPVPxbTXiK2gI9SRRxwfSjhMgWGxjXTHeSRhk8MIE0y3guUOyCGJJJSIyyc5wndGTnKdsJ22uKXb48/K3bC61MgVa2/0myaueRn1fNwL1p6Pz6sDhymi+x63Jsh4g2fDuUirPMD7iM3sdvmjp1CJqoKVX9wQhUWnqfEZWovTuwdJf1AwjrRtHZHzEPJHIdMbp/4X0frfF7l7fLo6RtmNwBx9GFeh+9mIdcpP1sq/fbPw3UiUbcpw607GesLinlk4q6ChnNXoO8jLNN2HOYict8l6w7jyC1/woPLzFH6qgdP0DiQf9s5HIQYdmSZJ5VqU8nDTBYME28icnK0OsHeMWnhZvO60FFwbhBzLk8sGYwxZFxvIXgD/aCmpP7MMySc7XUX3u9KcYYL3qYt6TMDbVLUA0K8XPnvFdxV/CiIVKI2zvPSWm5JqwkkNRu+z8TpOFHXTmVeFg78UGNJ0GxyWC29iDWrDekZNiYzWg7yaiKcpz5DuoHy6RdypR7WNZxoL/23nSvKU9VnZxWsIjI7x3o1AtpgtztZc/Ux2i747Ew8Pk1bsberViNjWInQzOD2umXMEDlk+zwFRCVEoAuwSpd8rmtIG7kGfhGSK7PdBEEgeMRir2Ib67wLNZJTO3Qhb3gGxgxR89Wq6bL2+o7TAIzoARmVWNxYSLmuSZLHpG/YfaggT7lB+0Ll8dYbAhSgWagMwBO5NGzIdWyOlHc/XgteJmZS70l5hEaa3c8MRnU6H0GJIIZeFwOEXZvyYhDyLBi0jknOL3SQBXzgw7RpD0hWNybrOf22/bo9YfWClgCqN4qnDVBSGUDWnDYUbIq32Ov8rYke1+Wtlz2reeuD44+gPOgJdoXAq1Jg/6qev1qZghSftPZV95yp6w+xtbwzvzxSQlo1XP9ObJpUpINoDCYGVafpF3uS9Tfw2N8FE2Q0tgMAAPlmmtrBS/q2MWellPf/cVNjmQfvxjHfgZUlKhqEbDDTipT9d4jzQ4L8SMj0hZkxBB0XZat4s3nsOG5XjpR61Mk927tjZ3+Nchvd7t2soEVM+kOHUhZdzx/NUUe8VhlvB6MNpR4WrYGHoxpXVSZn29I62AUm4N8Z+8l+UK7kTIPt7bMH0pxsekAWG58lTLQmBVJG270pbbZ/3di1n6pDtEt8PI3g5MV22GnXtaAzEBoTmcM143rRnRjwbpgfYmdtl1XTrmlD/XpVxh7MgbZtEE5JL4GxgcWbPjvdgsZL2Yw7ZQM4omt+apMXwSUFdzE8H2Bwuo+AWW+XHIe18jSSowIooTEgt67iMLsqSF71t2rRkMrefHw+3/rEvY2fgswBt2fDFyqLFUGsnAI25396HBZbai1SXlHwmm1S+MRmaoDP5neC4jhqS956slTzBnghG9JRrr5FzoPR1kubK4RWFHzuVgG+y+nN63sd2Y6YA/gDJ+ziAdLucEtGorAumalmm+Un5ES3KCHtONIDYCsFrciODmT9otkagJtNt85YZ2OUz1I5rkjKscuCnulJ5NQjtmTL0Xg2CetnaTrpuEVrqjF6jGtih8QsAbqq9+DAiM+aMp3B7aIklTW8cwm5n85EWr8VWx+/giuh/mcho58Fxz5lDA8A/OUCuK9s0ZldTP9ZYpspcxMGCzXBNEWLr2lYIjwyUPttmZjRC7kTvT0sk5kA=
---------------------------------------------
*/
    // 解锁钥
    //std::string strExportUnlock = "IdPE0Ei2QN5HS4qFekToYRBST4qlNKN/MVRpl1VdXTUNXCvF3/Tg46kchcNCx/qXRac+37GrD4Wk34uxWwZhnnz2b18AMNA+1l7zHSh+nAwBvVpwGrKPfEJApTvebrxgNsEZsf37+Xq9ZOh2FYC/Qn5PeZAU/Wbk3CQSsojMNfzMnV176MGeQFp8yuFFX/AQYF2ZHMQkO7kFIN2LstsneiSS9vrDEHvvcyP5KUGg7P/zoXASiRjbIOku53N6hi3BOTaR/1hcAdmzhTPP1bb5IL1tBQe1CEBwIOvtbs+yNBULbK4AoRfx75ozji5Ex92Tc3ezt6r4+gPws63pPdTGL2NUfpss++AGiOyhriDYcwBybB9pGl0raZ8nAP4rDFxLbXdaCIntNZvPqK5yheshx1r3VUNN1bTzs9//xBj/VYNRc+1BPCEQB93WbXBcIJsqS8pc3yOlkIy8/xQJM6mVzf4lKQEm5PK7ADD9YimAiNEYGpdKKy88792urdu139VOCmIOJrqKeLlvoqb/GB9ow0q6jlnrkoQoNEihv/bpk45HhsK8RkFIfUNXzXkZuIefb2/jwk1kENO59kuXPxL2ygipfKhzi3Hkkd15kOp7zJwJRK/yJxrJN40oMfCT0BlkAsJWViCElKCZ/8bqnEHcI1oVp4gzl9kwy7971NxoC1wSQevs4UKqXQHUqXQSLdR70gjitfpvYs+ZpKK0G5gvRsvFDzs4DhUpcyka7eT5B6CgYq+a8wUBm3W2Fvs6xDVc1MjO5aKTKd1FZ34sgeNlwwZ9YTQHzrJUQPmfnPbotBuy+SWseBOzL15f1lGMrWEgJm3Tl8PWgu5KqeSfoVvHsUGNx30+EWfqvO5WbpgnR1t5jFrc5DS2fgAN5TljkszOvErswBrrjmWqjOOLJ9JwTPUxxLtygT7HQeRsYImhJqN+S9mdvRoj2jo7jWVERrBNrZTU/MdgAm8R+1M0wnTivsBfCJ+OpgTHG1LXwrwCtMkWP2bpbWWGJviYqnvAejR0twBm9Yeb4pn1stk6xQW7d0anOyoQvcUkATclLrz98gwSzftK8qMd3/dnOFdb8RLY5s1L1PLABRNOmKoGGMFwb5uzsx+EsPm6YOsgr3qWD+gMd+2IO8fO9yhzgHXuM3ft";

    CGeminiKeyStack_testImportLock.GeminiKeyImport(strExportLock);
    //CGeminiKeyStack_testImportUnlock.gemini_KeyImport(strExportUnlock);

    GeminiKeys __KeysGenLock;
    CGeminiKeyStack_testImportLock.Decode(__KeysGenLock);
    //GeminiKeys __KeysGenUnlock;
    //CGeminiKeyStack_testImportUnlock.Decode(__KeysGenUnlock);
    //锁门
    CGeminiKeyStack CGeminiKeyStack_testLockLock;
    //CGeminiKeyStack CGeminiKeyStack_testImportLockUnlock;

    std::string PlainText;
    std::string LockedData;

    // 检查并校验数据
    bool bRet=true;
    //bRet = __KeysGenLock.CheckVerifyCode();
    CPPUNIT_ASSERT(bRet == true);
    //bRet = __KeysGenUnlock.CheckVerifyCode();
    //CPPUNIT_ASSERT(bRet == true);

    int iret = 0;
    // use
    iret = CGeminiKeyStack_testLockLock.GeminiLock(__KeysGenLock, PlainText, LockedData);
    CPPUNIT_ASSERT(iret == 0);

#if 0

    __fline;
    std::cout << "Locked:" << (iret==0?"Succeeded":"Failed") << std::endl;
    std::cout << "--------------------------------------------- PlainText(" << PlainText.size() << ")" << std::endl;
    std::cout << PlainText << std::endl;
    std::cout << "--------------------------------------------- LockedData(" << LockedData.size() << ")"  << std::endl;
    std::cout << LockedData << std::endl;
    std::cout << "---------------------------------------------" << std::endl;
#endif
    if (1)    // 使用字符串锁门
    {
        CGeminiKeyStack LockMe;
        std::string strExportLockTmp = strExportLock;
        // use
        iret = LockMe.GeminiLock(strExportLockTmp, PlainText, LockedData);
        CPPUNIT_ASSERT(iret == 0);
    }

    if (1)    // 使用字符串锁门
    {
        CGeminiKeyStack LockMe;
        std::string strExportLockTmp = strExportLock+"|";
        // use
        iret = LockMe.GeminiLock(strExportLockTmp, PlainText, LockedData);
        //std::cout << "---------------------------------------------:" << iret << std::endl;
        CPPUNIT_ASSERT(iret == -1);//GeminiKeyImport failed
    }
    //iret = CGeminiKeyStack_testImportLockUnlock.GeminiUnlock(__KeysGenUnlock, PlainText, LockedData);
    //CPPUNIT_ASSERT(iret == 0);
}

void CTestMan::testGeminiKeyUnlock                 ()
{
    CGeminiKeyStack CGeminiKeyStack_testImportUnlock;
    GeminiKeys KeysGenUnlock;

    // 下面这对密钥任意复制而来
    // 加锁钥
    std::string strExportLock = "tGkU7aaHcwX6dOSeOQ0Zz2948U9s+GpcY4lEXtlfQgNOTGUfKjlm6PhZjTGRmr2q18SayFcU7bDIC0WpU32Yex/qTyVU5LTa2Vu6S0TthISvU8l/znBC02qrOS4BOrGLUN6JIltptWBHeHshuaTiluTjRl7ECJ0l3HHGqjdNh1ZYeS1tfz3N00772zwibO9On3APEjlhOtIK9teqlpvO+CZVOjUsTdo0mWWRVkEZGxoAhBYw3jxcvLYYvZ6hkb9gpxekyJ1ZUQ/X+NE4nPf+5MlsSQCB1cprMjc51CEoAN2MxZj4m5UbjEX2hnzIjlTRxthQXjmAE7dAO8oM5BD5hqaIIguDpvZ0Xhbo3khDTpmxarb3RDeXy/GnbmAdohqUsfReC2Wec8HO51ZNBHhsy8UgcqjVTVWAMzZUnZiFIAFzsMrtR+XyEUWjdMb4FVgUmP5xU0aD/rbKNJU5RForqB0ODvAZtc5VsV8lEb2gt2vJOncqmGgyomb8QINYMDeumBaYM0U/KhwpBFg2TcAeAA7ZfZyIdevHvwTagwMYz3P8YNYQ3etSyPBxYaoBWmsHvcuZ4iMg8F8qkgyC2QRtJV/dEn1CFw3h5xiWmxpOHNkdjscfdXCcLA==";
/*
--------------------------------------------- strExportLock(633)
tGkU7aaHcwX6dOSeOQ0Zz2948U9s+GpcY4lEXtlfQgNOTGUfKjlm6PhZjTGRmr2q18SayFcU7bDIC0WpU32Yex/qTyVU5LTa2Vu6S0TthISvU8l/znBC02qrOS4BOrGLUN6JIltptWBHeHshuaTiluTjRl7ECJ0l3HHGqjdNh1ZYeS1tfz3N00772zwibO9On3APEjlhOtIK9teqlpvO+CZVOjUsTdo0mWWRVkEZGxoAhBYw3jxcvLYYvZ6hkb9gpxekyJ1ZUQ/X+NE4nPf+5MlsSQCB1cprMjc51CEoAN2MxZj4m5UbjEX2hnzIjlTRxthQXjmAE7dAO8oM5BD5hqaIIguDpvZ0Xhbo3khDTpmxarb3RDeXy/GnbmAdohqUsfReC2Wec8HO51ZNBHhsy8UgcqjVTVWAMzZUnZiFIAFzsMrtR+XyEUWjdMb4FVgUmP5xU0aD/rbKNJU5RForqB0ODvAZtc5VsV8lEb2gt2vJOncqmGgyomb8QINYMDeumBaYM0U/KhwpBFg2TcAeAA7ZfZyIdevHvwTagwMYz3P8YNYQ3etSyPBxYaoBWmsHvcuZ4iMg8F8qkgyC2QRtJV/dEn1CFw3h5xiWmxpOHNkdjscfdXCcLA==
--------------------------------------------- strExportUnlock(1901)
D6QU/JuxdWJUC0+9GeEyzq1WVMpIxUfGkZgogBBkRbwlHyJytKPxnYGloQZl5v2V6n5t2VyXsBxUhBWa2gJNkW3yUenr0r3V1riw0fu3vOfGWGO4/sfk40r1nMfaGp0hYulHMV/THs1mar5k4r+odhxmspfEeVpGeedT5HwAJd5HO56+JX+U+ze0ueeuQ818RBhnhzmeMiPqdVfkxZCN6JRG6hvzHQMFDdZeSPVPxbTXiK2gI9SRRxwfSjhMgWGxjXTHeSRhk8MIE0y3guUOyCGJJJSIyyc5wndGTnKdsJ22uKXb48/K3bC61MgVa2/0myaueRn1fNwL1p6Pz6sDhymi+x63Jsh4g2fDuUirPMD7iM3sdvmjp1CJqoKVX9wQhUWnqfEZWovTuwdJf1AwjrRtHZHzEPJHIdMbp/4X0frfF7l7fLo6RtmNwBx9GFeh+9mIdcpP1sq/fbPw3UiUbcpw607GesLinlk4q6ChnNXoO8jLNN2HOYict8l6w7jyC1/woPLzFH6qgdP0DiQf9s5HIQYdmSZJ5VqU8nDTBYME28icnK0OsHeMWnhZvO60FFwbhBzLk8sGYwxZFxvIXgD/aCmpP7MMySc7XUX3u9KcYYL3qYt6TMDbVLUA0K8XPnvFdxV/CiIVKI2zvPSWm5JqwkkNRu+z8TpOFHXTmVeFg78UGNJ0GxyWC29iDWrDekZNiYzWg7yaiKcpz5DuoHy6RdypR7WNZxoL/23nSvKU9VnZxWsIjI7x3o1AtpgtztZc/Ux2i747Ew8Pk1bsberViNjWInQzOD2umXMEDlk+zwFRCVEoAuwSpd8rmtIG7kGfhGSK7PdBEEgeMRir2Ib67wLNZJTO3Qhb3gGxgxR89Wq6bL2+o7TAIzoARmVWNxYSLmuSZLHpG/YfaggT7lB+0Ll8dYbAhSgWagMwBO5NGzIdWyOlHc/XgteJmZS70l5hEaa3c8MRnU6H0GJIIZeFwOEXZvyYhDyLBi0jknOL3SQBXzgw7RpD0hWNybrOf22/bo9YfWClgCqN4qnDVBSGUDWnDYUbIq32Ov8rYke1+Wtlz2reeuD44+gPOgJdoXAq1Jg/6qev1qZghSftPZV95yp6w+xtbwzvzxSQlo1XP9ObJpUpINoDCYGVafpF3uS9Tfw2N8FE2Q0tgMAAPlmmtrBS/q2MWellPf/cVNjmQfvxjHfgZUlKhqEbDDTipT9d4jzQ4L8SMj0hZkxBB0XZat4s3nsOG5XjpR61Mk927tjZ3+Nchvd7t2soEVM+kOHUhZdzx/NUUe8VhlvB6MNpR4WrYGHoxpXVSZn29I62AUm4N8Z+8l+UK7kTIPt7bMH0pxsekAWG58lTLQmBVJG270pbbZ/3di1n6pDtEt8PI3g5MV22GnXtaAzEBoTmcM143rRnRjwbpgfYmdtl1XTrmlD/XpVxh7MgbZtEE5JL4GxgcWbPjvdgsZL2Yw7ZQM4omt+apMXwSUFdzE8H2Bwuo+AWW+XHIe18jSSowIooTEgt67iMLsqSF71t2rRkMrefHw+3/rEvY2fgswBt2fDFyqLFUGsnAI25396HBZbai1SXlHwmm1S+MRmaoDP5neC4jhqS956slTzBnghG9JRrr5FzoPR1kubK4RWFHzuVgG+y+nN63sd2Y6YA/gDJ+ziAdLucEtGorAumalmm+Un5ES3KCHtONIDYCsFrciODmT9otkagJtNt85YZ2OUz1I5rkjKscuCnulJ5NQjtmTL0Xg2CetnaTrpuEVrqjF6jGtih8QsAbqq9+DAiM+aMp3B7aIklTW8cwm5n85EWr8VWx+/giuh/mcho58Fxz5lDA8A/OUCuK9s0ZldTP9ZYpspcxMGCzXBNEWLr2lYIjwyUPttmZjRC7kTvT0sk5kA=
---------------------------------------------
*/
    // 解锁钥
    std::string strExportUnlock = "D6QU/JuxdWJUC0+9GeEyzq1WVMpIxUfGkZgogBBkRbwlHyJytKPxnYGloQZl5v2V6n5t2VyXsBxUhBWa2gJNkW3yUenr0r3V1riw0fu3vOfGWGO4/sfk40r1nMfaGp0hYulHMV/THs1mar5k4r+odhxmspfEeVpGeedT5HwAJd5HO56+JX+U+ze0ueeuQ818RBhnhzmeMiPqdVfkxZCN6JRG6hvzHQMFDdZeSPVPxbTXiK2gI9SRRxwfSjhMgWGxjXTHeSRhk8MIE0y3guUOyCGJJJSIyyc5wndGTnKdsJ22uKXb48/K3bC61MgVa2/0myaueRn1fNwL1p6Pz6sDhymi+x63Jsh4g2fDuUirPMD7iM3sdvmjp1CJqoKVX9wQhUWnqfEZWovTuwdJf1AwjrRtHZHzEPJHIdMbp/4X0frfF7l7fLo6RtmNwBx9GFeh+9mIdcpP1sq/fbPw3UiUbcpw607GesLinlk4q6ChnNXoO8jLNN2HOYict8l6w7jyC1/woPLzFH6qgdP0DiQf9s5HIQYdmSZJ5VqU8nDTBYME28icnK0OsHeMWnhZvO60FFwbhBzLk8sGYwxZFxvIXgD/aCmpP7MMySc7XUX3u9KcYYL3qYt6TMDbVLUA0K8XPnvFdxV/CiIVKI2zvPSWm5JqwkkNRu+z8TpOFHXTmVeFg78UGNJ0GxyWC29iDWrDekZNiYzWg7yaiKcpz5DuoHy6RdypR7WNZxoL/23nSvKU9VnZxWsIjI7x3o1AtpgtztZc/Ux2i747Ew8Pk1bsberViNjWInQzOD2umXMEDlk+zwFRCVEoAuwSpd8rmtIG7kGfhGSK7PdBEEgeMRir2Ib67wLNZJTO3Qhb3gGxgxR89Wq6bL2+o7TAIzoARmVWNxYSLmuSZLHpG/YfaggT7lB+0Ll8dYbAhSgWagMwBO5NGzIdWyOlHc/XgteJmZS70l5hEaa3c8MRnU6H0GJIIZeFwOEXZvyYhDyLBi0jknOL3SQBXzgw7RpD0hWNybrOf22/bo9YfWClgCqN4qnDVBSGUDWnDYUbIq32Ov8rYke1+Wtlz2reeuD44+gPOgJdoXAq1Jg/6qev1qZghSftPZV95yp6w+xtbwzvzxSQlo1XP9ObJpUpINoDCYGVafpF3uS9Tfw2N8FE2Q0tgMAAPlmmtrBS/q2MWellPf/cVNjmQfvxjHfgZUlKhqEbDDTipT9d4jzQ4L8SMj0hZkxBB0XZat4s3nsOG5XjpR61Mk927tjZ3+Nchvd7t2soEVM+kOHUhZdzx/NUUe8VhlvB6MNpR4WrYGHoxpXVSZn29I62AUm4N8Z+8l+UK7kTIPt7bMH0pxsekAWG58lTLQmBVJG270pbbZ/3di1n6pDtEt8PI3g5MV22GnXtaAzEBoTmcM143rRnRjwbpgfYmdtl1XTrmlD/XpVxh7MgbZtEE5JL4GxgcWbPjvdgsZL2Yw7ZQM4omt+apMXwSUFdzE8H2Bwuo+AWW+XHIe18jSSowIooTEgt67iMLsqSF71t2rRkMrefHw+3/rEvY2fgswBt2fDFyqLFUGsnAI25396HBZbai1SXlHwmm1S+MRmaoDP5neC4jhqS956slTzBnghG9JRrr5FzoPR1kubK4RWFHzuVgG+y+nN63sd2Y6YA/gDJ+ziAdLucEtGorAumalmm+Un5ES3KCHtONIDYCsFrciODmT9otkagJtNt85YZ2OUz1I5rkjKscuCnulJ5NQjtmTL0Xg2CetnaTrpuEVrqjF6jGtih8QsAbqq9+DAiM+aMp3B7aIklTW8cwm5n85EWr8VWx+/giuh/mcho58Fxz5lDA8A/OUCuK9s0ZldTP9ZYpspcxMGCzXBNEWLr2lYIjwyUPttmZjRC7kTvT0sk5kA=";

    // 解锁钥
    std::string strKeyUnlock = strExportUnlock;//"DYje4H/8o2AdHxaHWbWgxAJMJH0kkAKejG1U5fmpwoXNCn7mY4EDzqq+ygSRerA4sugtD/qVdQXTHFqel5HT3PmPosk0dxEX0mH5LYsa4kQHsteRcnUY4DkiaenAYZJCea4msAjlzM0A5UzpsiV8q4zdl6zTVfcY6dWhxk3ccmPMYa/rAxvp5ufJ8Ly2ktrrbnxs4UkRZthzwJPpjads3j21mifMdX60CTJyl1wr9YgzvWkyCgW82NBIDu6g2KdeKiHKojRhlyGYqooMOgEMFNGUuko+vYY7nyKzI45sENN1D3DpDZsRwvnYYa8PJf2WaqburSbyXRFf+XnSVC07W0+RaM/EjWfOn6k7EqpP/wF7Keva69VyuLF4QNPLLTIYMp7lp5WIeP+Jqnd56e2y5v6YzGiHo71Amc7j8wUvRcgJr+t5XRHpestESjicRYEGW0ZD75EpRi6zsrXD7crMn1VdOBv4KOQIz2+xBayiuOYKXsvPQI3TpSA6eAv19kk3aZo8lkSQvTZ05iPNyaExvS8/r4bJoOf71Nooiv7X8nBEJEOCjXAAx54U1qhJG02GM2/zCu/bS9jRPPtTGZIcaNzI18ZwvwTXa18aUVA7VDBmkkgKkoA6IlOfyBIx+gjYolV/QyS3/yoWZWLvQ7o5FzHmVsDFl5Q9dfesSe2eccfc9W/NPiLPwd1OA4/scwPd/cRph39uk0Ao/5PBPBpxDFMVrGnYhsVAuQ/SQgcKMRx8+nC6Mlavbf5tfxVZuuWmTCxe/J1KCd93GbuKNMk4GptwWDwllo7C2NnTCHeJ5Rv8LQlrTDKGOXZqxAT4uh7CoPXPYPNtbCw6YWn/qtbWMUqSxMxb7dUDGYqtMjEHA4PDN3UI+4aC5AjlleQ0Fr3vvqx0gHYoxkwkMM/s6fTTEzJNtEZFPcksQSoUgA1c6Yxb8ek/rGJX0oABldvWm4atwB9M1k2wxOsxCfwqV4IwVkT7HXNiNYXAk/dMkj3VwgwhH9yXYL4iKd31sodCsIrC8943MUkiT8tDhaxkOLMxdG3XDZ+4dSnJq4DkZLMIpgCoMhnhJU0B3DFw5E7FZqUawcYn2HlMmVIg6LYtuloTvA==";

    //CGeminiKeyStack_testImportLock.gemini_KeyImport(strExportLock);
    CGeminiKeyStack_testImportUnlock.GeminiKeyImport(strKeyUnlock);

    //GeminiKeys __KeysGenLock;
    //CGeminiKeyStack_testImportLock.Decode(__KeysGenLock);
    CGeminiKeyStack_testImportUnlock.Decode(KeysGenUnlock);

    //开锁
    //CGeminiKeyStack CGeminiKeyStack_testLockLock;
    CGeminiKeyStack CGeminiKeyStack_testImportLockUnlock;
    int iret = 0;

    // 从L271处复制而来
    std::string PlainText = "75427104603181083521";
    std::string LockedData = "JkvzjeI/7UVbhOfznlrWzAHDeOoo0PEDyUhsmW03U3vIWA92Gc0gtjvI3lMWzU6pr/spu0qR2X61voYmU4Xv1iKrSUmPrkJqqDHUpWvPrbnHx2WMqlaWtmhzc/4i+n1/M/8njwrVsHa+Kql/OExzXNqvF1GweeSbPba3aETOqTiB2XHFyl9sIfMUn99Gw1FKzPagK/c+e8d38ai2YzMH/OPFFbdm9JMo6CSIr+aQ4VdsI+HybGXz/7oN5mOygdrkb5bgnIVv9l8YlpLMSFst06519+lMFBFHjgE7oCUOM5wtmFjNixa8JddLoDS09lrWqr9JvhIzfA7zchsADwm+SQ==";
//-------------------------------------------- PlainText(20)
//75427104603181083521
//--------------------------------------------- LockedData(345)
//JkvzjeI/7UVbhOfznlrWzAHDeOoo0PEDyUhsmW03U3vIWA92Gc0gtjvI3lMWzU6pr/spu0qR2X61voYmU4Xv1iKrSUmPrkJqqDHUpWvPrbnHx2WMqlaWtmhzc/4i+n1/M/8njwrVsHa+Kql/OExzXNqvF1GweeSbPba3aETOqTiB2XHFyl9sIfMUn99Gw1FKzPagK/c+e8d38ai2YzMH/OPFFbdm9JMo6CSIr+aQ4VdsI+HybGXz/7oN5mOygdrkb5bgnIVv9l8YlpLMSFst06519+lMFBFHjgE7oCUOM5wtmFjNixa8JddLoDS09lrWqr9JvhIzfA7zchsADwm+SQ==
//---------------------------------------------    

    //自动校验了
    iret = CGeminiKeyStack_testImportLockUnlock.GeminiUnlock(KeysGenUnlock, PlainText, LockedData);
    CPPUNIT_ASSERT(iret == 0);// 密钥未校验 失败

#if 0

    __fline;
    std::cout << "Unlocked:" << (iret==0?"Succeeded":"Failed")    << std::endl;
    std::cout << "--------------------------------------------- PlainText(" << PlainText.size() << ")" << std::endl;
    std::cout << PlainText << std::endl;
    std::cout << "--------------------------------------------- LockedData(" << LockedData.size() << ")"  << std::endl;
    std::cout << LockedData << std::endl;
    std::cout << "---------------------------------------------" << std::endl;
#endif

    // 检查并校验数据
    bool bRet;
    bRet = KeysGenUnlock.checkVerifyCode();
    CPPUNIT_ASSERT(bRet == true);

    iret = CGeminiKeyStack_testImportLockUnlock.GeminiUnlock(KeysGenUnlock, PlainText, LockedData);
    CPPUNIT_ASSERT(iret == 0);

    // 解锁钥复制正常
    GeminiKeys KeysGenUnlock_checkKey = KeysGenUnlock;
    iret = CGeminiKeyStack_testImportLockUnlock.GeminiUnlock(KeysGenUnlock_checkKey, PlainText, LockedData);
    CPPUNIT_ASSERT(iret == 0);

    // 解锁钥校验失败
    KeysGenUnlock_checkKey.m_vecGeminiKey[0].strPeerID+="h";
    bRet = KeysGenUnlock_checkKey.checkVerifyCode();
    CPPUNIT_ASSERT(bRet == false);
    iret = CGeminiKeyStack_testImportLockUnlock.GeminiUnlock(KeysGenUnlock_checkKey, PlainText, LockedData);
    CPPUNIT_ASSERT(iret == -1);

    if (1)
    {
        // 明文和密文不匹配
        std::string PlainTextNoise = PlainText+"23818980895812659251";
        std::string LockedDataNoise =LockedData;
        KeysGenUnlock_checkKey = KeysGenUnlock;
        iret = CGeminiKeyStack_testImportLockUnlock.GeminiUnlock(KeysGenUnlock_checkKey, PlainTextNoise, LockedDataNoise);
        CPPUNIT_ASSERT(iret == -3);
    }

    if (1)
    {
        // 密文数据出错 rsa解密出错
        std::string PlainTextNoise = PlainText;
        std::string LockedDataNoise = "c3vcPfyasXQvzJRuvD8bGeKa/4nbBHm7bBPGYlk0r44EtmWLK3PUK5QHff5A2lzHvUllSAtOaPredmFg4qu7DTqsEj2He84NM9BFPcDU9LtRgdYB7CMGw1NxctzRJYG+bHEYqMymFAkCqj6expQjM6h3QUoV3O3Jg24aqXadQ/8=";
        KeysGenUnlock_checkKey = KeysGenUnlock;

        iret = CGeminiKeyStack_testImportLockUnlock.GeminiUnlock(KeysGenUnlock_checkKey, PlainTextNoise, LockedDataNoise);

        //__fline;
        //std::cout << "[" << iret << "]" << std::endl;

        CPPUNIT_ASSERT(iret == -2);
    }

    if (1)
    {
        // 数据部分丢失
        std::string PlainTextNoise = PlainText;
        //std::string LockedData = "c2vcPfyasXQvzJRuvD8bGeKa/4nbBHm7bBPGYlk0r44EtmWLK3PUK5QHff5A2lzHvUllSAtOaPredmFg4qu7DTqsEj2He84NM9BFPcDU9LtRgdYB7CMGw1NxctzRJYG+bHEYqMymFAkCqj6expQjM6h3QUoV3O3Jg24aqXadQ/8=";
        std::string LockedDataNoise = LockedData;

        LockedDataNoise.erase(std::find(LockedDataNoise.begin(), LockedDataNoise.end(), 'B'));
        LockedDataNoise.erase(LockedDataNoise.end() - 1);

        KeysGenUnlock_checkKey = KeysGenUnlock;

        iret = CGeminiKeyStack_testImportLockUnlock.GeminiUnlock(KeysGenUnlock_checkKey, PlainTextNoise, LockedDataNoise);

        //__fline;
        //std::cout << "LockedDataNoise:[" << LockedDataNoise << "]" << std::endl;

        CPPUNIT_ASSERT(iret == -2);
    }

    if (1)
    {
        // 数据部分丢失
        std::string PlainTextNoise = PlainText;
        std::string LockedDataNoise = LockedData;//"c2vcPfyasXQvzJRuvD8bGeKa/4nbBHm7bBPGYlk0r44EmWLK3PUK5QHff5A2lzHvUllSAtOPredmFg4qu7DTqsEj2He84NM9BPcDU9LtRgdYB7CMGw1NxctzRJYG+bHEYqMymFAkCqj6expQjM6h3QoV3O3Jg24aqXadQ/8";
        LockedDataNoise.erase(std::find(LockedDataNoise.begin(), LockedDataNoise.end(), 'B'));
        KeysGenUnlock_checkKey = KeysGenUnlock;

        iret = CGeminiKeyStack_testImportLockUnlock.GeminiUnlock(KeysGenUnlock_checkKey, PlainTextNoise, LockedDataNoise);

        //__fline;
        //std::cout << "[" << iret << "]" << std::endl;

        CPPUNIT_ASSERT(iret == -2);
    }

    if(1) //字符串 解锁
    {
        std::string strKeyUnlockTmp = strKeyUnlock ;
        // 数据部分丢失
        std::string PlainTextNoise = PlainText;
        std::string LockedDataNoise = LockedData;//"c2vcPfyasXQvzJRuvD8bGeKa/4nbBHm7bBPGYlk0r44EmWLK3PUK5QHff5A2lzHvUllSAtOPredmFg4qu7DTqsEj2He84NM9BPcDU9LtRgdYB7CMGw1NxctzRJYG+bHEYqMymFAkCqj6expQjM6h3QoV3O3Jg24aqXadQ/8";
        //LockedDataNoise.erase(std::find(LockedDataNoise.begin(), LockedDataNoise.end(), 'B'));
        //KeysGenUnlock_checkKey = KeysGenUnlock;

        iret = CGeminiKeyStack_testImportLockUnlock.GeminiUnlock(strKeyUnlockTmp, PlainTextNoise, LockedDataNoise);

        CPPUNIT_ASSERT(iret == 0);
    }

}


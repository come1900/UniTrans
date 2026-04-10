/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * TestMan.cpp - _explain_
 *
 * Copyright (C) 2013 gminiTech.com, All Rights Reserved.
 *
 * $Id: TestMan.cpp 0001 2013-11-02 14:38:29Z wu_junjie $
 *
 *  Explain:
 *     UnitTest -- CInterThings_gLockStack
 *
 *  Update:
 *     2013-11-03 14:38:33 WuJunjie 549 Create
 *
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#include "TestMan.h"

#include "../InterThings_gLockStack.h"

#ifndef __trip
    #define __trip printf("-W-%d::%s(%d)\n", (int)time(NULL), __FILE__, __LINE__);
#endif
#ifndef __fline
    #define __fline printf("%s(%d)--", __FILE__, __LINE__);
#endif

CPPUNIT_TEST_SUITE_REGISTRATION( CTestMan );

void CTestMan::setUp()
{}
void CTestMan::tearDown()
{}

void CTestMan::testEncodeCAckMsgSimple()
{
    CInterThings_gLockStack __CSmsStack;
    

    // 编码
    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
    std::string result = "1";
    //--------------------------------------------- strExportLock(621)
    //0ybAcPFCCxotoAd4QZErd2iOY/dIUx6Gs6aZQnoulxdQ6njegPL13aEJeQzVmAtTidPdrHXGif2SIVFQ5lAngDZ6aGOQfHhrDaTTQ42S2sk4v+E5ATSlLm8FseWB/h8j96bZcgbCp4XKJqzimucu54oiWzGsUZ5Nc+vDHHpnCMLsL1yaoiJn3YFXQsURqmr8g93HIzgCgmPPiK4lAgofEC6hCjudoAoQ2XO+veha9ErBHPkg8gw9yKVF8D6vrApgD+/yiT1MMZEwF0B/rG36k3v2Nc5iCFphboL0REN8M+EeBrdDBK87Iqu/0Lm+FDk8GAMsGvA67P1w5tL3NBXfH/1lptt8D3re0aNzDuxwcYCuS+fNwzfUGVDMPRSP/0dBaeL6XFsUHBOzXC5b+wfersLSneWVe0OjgRsgVPXfrGmJkBqWl8cny/gmEGipDMxwXW6gFPh1CMgY9Jk2Jfbp9+hRKASbo83ybLXgGOCSijqmO3O6h27G71DPizcTLzPvAswA91anxsTXKScv7tNhcpB6Hj37OhYwLCwxcNn2SijPxua3iWXJSmmtqVuCXDtnXBLEeNtxfJSS7tbk/wzOs1zgtJoTwKimnCEcXu/2uE8= 
    //--------------------------------------------- strExportUnlock(1913)
    //mWw7gYLPYJ58fKugHbR0d0i+bXl2D8cghp7ljnmlteFByzRo3NodHN96FgdJfK2rDoeFoYebMl2sZCcv3KWe7onMeIk+Eoa6me1XaypCZ0VR7ABvKsmb9mnDPGi9mZahQKPUp3/mmpsVCjU9reTyOPSM7pjNSz2Smhk8AhWOo3Z/Yf8zC1FXlF+hDGUv7F0Ej6So9sCBtfx32VMYFBso3Idis6cslMdFRFrWxjUohvMW33REsdFXHWCxXxBVn3GlmNbmuZlwXE1lNdSXDGE3pOOoRMMjx7AWvbPYlDoloq7J3ZxlirhuDrgB2MqpS9UFC7a7MyaJMJ/KA1Dswq1O7Ryo++N8HF++wFMOwcw/XFjIA+1TRjavp06V90jf51gyRSky24tWy3hou03PioWE4bDwVL2QTqhjyCh8TDB2YKQjqJmplkq5s/F6QkLhsm6jiZvJ0O7cqlPJbBT6l0E42SnY8i+aRDU5BV5acvW+BMs1VtxI5Mlizx6AWCvJ4++c5/qn8R00u3W2qJ4YTSsZJ9W4oa12h7Jc23AdEMEVS3jz7HIQ+Xyul9nciw9VJ+kzIDqOoiKCVI7YE4XhmsUip6k8fNrWzLSSKdAE8MPPjZrlZxbLBkWt3MRSJ5lfQF+BWow2yV3oxyzSSjpw4QMS+qrgxK5zOpkFlUUfZn75QBGBpZbB1fCh5Bukc/JVn6FJv+EmFaSviHRVdS56x0iUuDdxCuP4/UrYRDExUK8rIxOxpk/ytmAmO70gmZXFX6FmRFXDhuues/2ff3CGhJ3unAqiuh/+RXXruAXgHj47gvEyLRXkp7lXMDnZi4uItxVudHXrNZWI2kx2fHSQelO00OfSfY5ZGr0lQSliStAGLEPpPxXuhRq/YWcVsF0l7ix4+IkRQFuCjglZCEBE7DCVQ11vZuJtI4eZpL/Du2bxW4t0stHFHOpksh4OcY3gEYrIEes9/fVyU3kb6tTK3q5nixLIAc7tcDEn+Dgl6SSQ5Jm5ZhWhue8HWbtENjMq+Aepd740py9TpCaQJgkEoX8+j8CTxtJEI0Pce/BGbSdg5TLp0AEXVTLj3KPaw1ht8rAETjFizutamWc9KOKuL9NBxi8mFUwFcTOU4kpaSNjqaciKv5/9tkbkngz1GfElrlIWXpv+p3kVW9DkjjBDn2nCRj0OL1dQxPMK8dPOvZHiK9v0rgtc2ZOmRundNmlMKUNkrduV9wC7kJqGRNmPI/TVOclntHjP6PuiqhIk106YHQNl/ujSEJZSjiBAjhKL6w9zswv7MV+WXlHDoCNdCH4e2YMgG37migjnIVdmNMWeKekrVxmJfKfihSjzJztbLnmxWr+GXrlFrDT4++25HFGftm+vkg0nV9sOG2CxnTJPhBVCblvENN57QnNjBVweyrmSK7dnk6AP6WXmY1Gm/Sqs1P6s6Y/spMHTDzSQJCu8FuVIQ2bVPo4XRjsWvp88bXD72nOidmoI+gG3gkvywihMOpr+tQ+uvGfcbJC75fiJnzhntVY2ohEKNMcTz6MlrB4KgZUVeLwpbN0E2W0pS48aNQ44ydwtgB833S1rt9aIpzj7g48TDDU01lPKJXL0Fz/F2IZ5WX2mugsOGJrtKsWWufeNf15/AaMamBdy8KtcebSb8UWo4NRBekKScyTUVgiN3HOI6n00cQEHWqLHEtBSWJMJKp7CNWS0Xaeo78b8ifNAL6cNx1teRCIRWf4crnzVwCh+cTypUUjpn5Baf353Zk31+sRl8dNLTeXNmwt91KVkufYWibUs2YxW05olQROQX+4W0lOmxeTjzFAJG+LdF7sSJKyp7+wN9CWfLBifUnlwt6hf9bCW1Nx2Qt8hlUrwwqhL2zMpkplNeGLAczAcmBbJzZPqOEsvYscuOGKTx8awfo097mNn8g== 
    std::string strKeyLock = "0ybAcPFCCxotoAd4QZErd2iOY/dIUx6Gs6aZQnoulxdQ6njegPL13aEJeQzVmAtTidPdrHXGif2SIVFQ5lAngDZ6aGOQfHhrDaTTQ42S2sk4v+E5ATSlLm8FseWB/h8j96bZcgbCp4XKJqzimucu54oiWzGsUZ5Nc+vDHHpnCMLsL1yaoiJn3YFXQsURqmr8g93HIzgCgmPPiK4lAgofEC6hCjudoAoQ2XO+veha9ErBHPkg8gw9yKVF8D6vrApgD+/yiT1MMZEwF0B/rG36k3v2Nc5iCFphboL0REN8M+EeBrdDBK87Iqu/0Lm+FDk8GAMsGvA67P1w5tL3NBXfH/1lptt8D3re0aNzDuxwcYCuS+fNwzfUGVDMPRSP/0dBaeL6XFsUHBOzXC5b+wfersLSneWVe0OjgRsgVPXfrGmJkBqWl8cny/gmEGipDMxwXW6gFPh1CMgY9Jk2Jfbp9+hRKASbo83ybLXgGOCSijqmO3O6h27G71DPizcTLzPvAswA91anxsTXKScv7tNhcpB6Hj37OhYwLCwxcNn2SijPxua3iWXJSmmtqVuCXDtnXBLEeNtxfJSS7tbk/wzOs1zgtJoTwKimnCEcXu/2uE8=";
    std::string strKeyULock = "mWw7gYLPYJ58fKugHbR0d0i+bXl2D8cghp7ljnmlteFByzRo3NodHN96FgdJfK2rDoeFoYebMl2sZCcv3KWe7onMeIk+Eoa6me1XaypCZ0VR7ABvKsmb9mnDPGi9mZahQKPUp3/mmpsVCjU9reTyOPSM7pjNSz2Smhk8AhWOo3Z/Yf8zC1FXlF+hDGUv7F0Ej6So9sCBtfx32VMYFBso3Idis6cslMdFRFrWxjUohvMW33REsdFXHWCxXxBVn3GlmNbmuZlwXE1lNdSXDGE3pOOoRMMjx7AWvbPYlDoloq7J3ZxlirhuDrgB2MqpS9UFC7a7MyaJMJ/KA1Dswq1O7Ryo++N8HF++wFMOwcw/XFjIA+1TRjavp06V90jf51gyRSky24tWy3hou03PioWE4bDwVL2QTqhjyCh8TDB2YKQjqJmplkq5s/F6QkLhsm6jiZvJ0O7cqlPJbBT6l0E42SnY8i+aRDU5BV5acvW+BMs1VtxI5Mlizx6AWCvJ4++c5/qn8R00u3W2qJ4YTSsZJ9W4oa12h7Jc23AdEMEVS3jz7HIQ+Xyul9nciw9VJ+kzIDqOoiKCVI7YE4XhmsUip6k8fNrWzLSSKdAE8MPPjZrlZxbLBkWt3MRSJ5lfQF+BWow2yV3oxyzSSjpw4QMS+qrgxK5zOpkFlUUfZn75QBGBpZbB1fCh5Bukc/JVn6FJv+EmFaSviHRVdS56x0iUuDdxCuP4/UrYRDExUK8rIxOxpk/ytmAmO70gmZXFX6FmRFXDhuues/2ff3CGhJ3unAqiuh/+RXXruAXgHj47gvEyLRXkp7lXMDnZi4uItxVudHXrNZWI2kx2fHSQelO00OfSfY5ZGr0lQSliStAGLEPpPxXuhRq/YWcVsF0l7ix4+IkRQFuCjglZCEBE7DCVQ11vZuJtI4eZpL/Du2bxW4t0stHFHOpksh4OcY3gEYrIEes9/fVyU3kb6tTK3q5nixLIAc7tcDEn+Dgl6SSQ5Jm5ZhWhue8HWbtENjMq+Aepd740py9TpCaQJgkEoX8+j8CTxtJEI0Pce/BGbSdg5TLp0AEXVTLj3KPaw1ht8rAETjFizutamWc9KOKuL9NBxi8mFUwFcTOU4kpaSNjqaciKv5/9tkbkngz1GfElrlIWXpv+p3kVW9DkjjBDn2nCRj0OL1dQxPMK8dPOvZHiK9v0rgtc2ZOmRundNmlMKUNkrduV9wC7kJqGRNmPI/TVOclntHjP6PuiqhIk106YHQNl/ujSEJZSjiBAjhKL6w9zswv7MV+WXlHDoCNdCH4e2YMgG37migjnIVdmNMWeKekrVxmJfKfihSjzJztbLnmxWr+GXrlFrDT4++25HFGftm+vkg0nV9sOG2CxnTJPhBVCblvENN57QnNjBVweyrmSK7dnk6AP6WXmY1Gm/Sqs1P6s6Y/spMHTDzSQJCu8FuVIQ2bVPo4XRjsWvp88bXD72nOidmoI+gG3gkvywihMOpr+tQ+uvGfcbJC75fiJnzhntVY2ohEKNMcTz6MlrB4KgZUVeLwpbN0E2W0pS48aNQ44ydwtgB833S1rt9aIpzj7g48TDDU01lPKJXL0Fz/F2IZ5WX2mugsOGJrtKsWWufeNf15/AaMamBdy8KtcebSb8UWo4NRBekKScyTUVgiN3HOI6n00cQEHWqLHEtBSWJMJKp7CNWS0Xaeo78b8ifNAL6cNx1teRCIRWf4crnzVwCh+cTypUUjpn5Baf353Zk31+sRl8dNLTeXNmwt91KVkufYWibUs2YxW05olQROQX+4W0lOmxeTjzFAJG+LdF7sSJKyp7+wN9CWfLBifUnlwt6hf9bCW1Nx2Qt8hlUrwwqhL2zMpkplNeGLAczAcmBbJzZPqOEsvYscuOGKTx8awfo097mNn8g==";
    
    std::stringstream __ss;

    CMsgGeminiKeyString Msg;
    Msg.m_strKeyVersion = result;
    Msg.m_strKeyData = strKeyLock;
    //std::string strEncoded = "{ \"GeminiKey\" : \"no such msg\", \"Opr\" : \"100\" }\n";
    __ss \
        << "{ \"GeminiKey\" : \""
        << Msg.m_strKeyData
        <<"\", \"Memo\" : \""
        << Msg.m_strKeyVersion
        <<"\" }\n";
    __CSmsStack.Encode(Msg);

    // 编码的时候会将："/" 转义为 "\/" 故而不相等了
    //CPPUNIT_ASSERT(__CSmsStack.ToString() == __ss.str() );

    // 解码
    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
    CMsgGeminiKeyString MsgDecoded;

    __CSmsStack.Decode(MsgDecoded);

    //        cout << endl;
    //        Msg.dump();
    //        cout << endl;

    CPPUNIT_ASSERT(MsgDecoded.m_strKeyVersion == result );
    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
    // 打印， 备用
#if 0
    CInterThings_gLockStack stack_gLock;
    CInterThings_gLockStack stack_gULock;

    CMsgGeminiKeyString MsgLock;
    MsgLock.m_strMemo = result;
    MsgLock.m_strGeminiKey = strKeyLock;
    stack_gLock.Encode(MsgLock);
    std::cout << std::endl << "[" <<  stack_gLock.ToString() << "]" << std::endl;

    CMsgGeminiKeyString MsgULock;
    MsgULock.m_strMemo = result;
    MsgULock.m_strGeminiKey = strKeyULock;
    stack_gULock.Encode(MsgULock);
    std::cout << std::endl << "[" <<  stack_gULock.ToString() << "]" << std::endl;

/*
json串
{ "GeminiKey" : "0ybAcPFCCxotoAd4QZErd2iOY\/dIUx6Gs6aZQnoulxdQ6njegPL13aEJeQzVmAtTidPdrHXGif2SIVFQ5lAngDZ6aGOQfHhrDaTTQ42S2sk4v+E5ATSlLm8FseWB\/h8j96bZcgbCp4XKJqzimucu54oiWzGsUZ5Nc+vDHHpnCMLsL1yaoiJn3YFXQsURqmr8g93HIzgCgmPPiK4lAgofEC6hCjudoAoQ2XO+veha9ErBHPkg8gw9yKVF8D6vrApgD+\/yiT1MMZEwF0B\/rG36k3v2Nc5iCFphboL0REN8M+EeBrdDBK87Iqu\/0Lm+FDk8GAMsGvA67P1w5tL3NBXfH\/1lptt8D3re0aNzDuxwcYCuS+fNwzfUGVDMPRSP\/0dBaeL6XFsUHBOzXC5b+wfersLSneWVe0OjgRsgVPXfrGmJkBqWl8cny\/gmEGipDMxwXW6gFPh1CMgY9Jk2Jfbp9+hRKASbo83ybLXgGOCSijqmO3O6h27G71DPizcTLzPvAswA91anxsTXKScv7tNhcpB6Hj37OhYwLCwxcNn2SijPxua3iWXJSmmtqVuCXDtnXBLEeNtxfJSS7tbk\/wzOs1zgtJoTwKimnCEcXu\/2uE8=", "Opr" : "parse" }

{ "GeminiKey" : "mWw7gYLPYJ58fKugHbR0d0i+bXl2D8cghp7ljnmlteFByzRo3NodHN96FgdJfK2rDoeFoYebMl2sZCcv3KWe7onMeIk+Eoa6me1XaypCZ0VR7ABvKsmb9mnDPGi9mZahQKPUp3\/mmpsVCjU9reTyOPSM7pjNSz2Smhk8AhWOo3Z\/Yf8zC1FXlF+hDGUv7F0Ej6So9sCBtfx32VMYFBso3Idis6cslMdFRFrWxjUohvMW33REsdFXHWCxXxBVn3GlmNbmuZlwXE1lNdSXDGE3pOOoRMMjx7AWvbPYlDoloq7J3ZxlirhuDrgB2MqpS9UFC7a7MyaJMJ\/KA1Dswq1O7Ryo++N8HF++wFMOwcw\/XFjIA+1TRjavp06V90jf51gyRSky24tWy3hou03PioWE4bDwVL2QTqhjyCh8TDB2YKQjqJmplkq5s\/F6QkLhsm6jiZvJ0O7cqlPJbBT6l0E42SnY8i+aRDU5BV5acvW+BMs1VtxI5Mlizx6AWCvJ4++c5\/qn8R00u3W2qJ4YTSsZJ9W4oa12h7Jc23AdEMEVS3jz7HIQ+Xyul9nciw9VJ+kzIDqOoiKCVI7YE4XhmsUip6k8fNrWzLSSKdAE8MPPjZrlZxbLBkWt3MRSJ5lfQF+BWow2yV3oxyzSSjpw4QMS+qrgxK5zOpkFlUUfZn75QBGBpZbB1fCh5Bukc\/JVn6FJv+EmFaSviHRVdS56x0iUuDdxCuP4\/UrYRDExUK8rIxOxpk\/ytmAmO70gmZXFX6FmRFXDhuues\/2ff3CGhJ3unAqiuh\/+RXXruAXgHj47gvEyLRXkp7lXMDnZi4uItxVudHXrNZWI2kx2fHSQelO00OfSfY5ZGr0lQSliStAGLEPpPxXuhRq\/YWcVsF0l7ix4+IkRQFuCjglZCEBE7DCVQ11vZuJtI4eZpL\/Du2bxW4t0stHFHOpksh4OcY3gEYrIEes9\/fVyU3kb6tTK3q5nixLIAc7tcDEn+Dgl6SSQ5Jm5ZhWhue8HWbtENjMq+Aepd740py9TpCaQJgkEoX8+j8CTxtJEI0Pce\/BGbSdg5TLp0AEXVTLj3KPaw1ht8rAETjFizutamWc9KOKuL9NBxi8mFUwFcTOU4kpaSNjqaciKv5\/9tkbkngz1GfElrlIWXpv+p3kVW9DkjjBDn2nCRj0OL1dQxPMK8dPOvZHiK9v0rgtc2ZOmRundNmlMKUNkrduV9wC7kJqGRNmPI\/TVOclntHjP6PuiqhIk106YHQNl\/ujSEJZSjiBAjhKL6w9zswv7MV+WXlHDoCNdCH4e2YMgG37migjnIVdmNMWeKekrVxmJfKfihSjzJztbLnmxWr+GXrlFrDT4++25HFGftm+vkg0nV9sOG2CxnTJPhBVCblvENN57QnNjBVweyrmSK7dnk6AP6WXmY1Gm\/Sqs1P6s6Y\/spMHTDzSQJCu8FuVIQ2bVPo4XRjsWvp88bXD72nOidmoI+gG3gkvywihMOpr+tQ+uvGfcbJC75fiJnzhntVY2ohEKNMcTz6MlrB4KgZUVeLwpbN0E2W0pS48aNQ44ydwtgB833S1rt9aIpzj7g48TDDU01lPKJXL0Fz\/F2IZ5WX2mugsOGJrtKsWWufeNf15\/AaMamBdy8KtcebSb8UWo4NRBekKScyTUVgiN3HOI6n00cQEHWqLHEtBSWJMJKp7CNWS0Xaeo78b8ifNAL6cNx1teRCIRWf4crnzVwCh+cTypUUjpn5Baf353Zk31+sRl8dNLTeXNmwt91KVkufYWibUs2YxW05olQROQX+4W0lOmxeTjzFAJG+LdF7sSJKyp7+wN9CWfLBifUnlwt6hf9bCW1Nx2Qt8hlUrwwqhL2zMpkplNeGLAczAcmBbJzZPqOEsvYscuOGKTx8awfo097mNn8g==", "Opr" : "parse" }

// 转义备用
{ \"GeminiKey\" : \"0ybAcPFCCxotoAd4QZErd2iOY\/dIUx6Gs6aZQnoulxdQ6njegPL13aEJeQzVmAtTidPdrHXGif2SIVFQ5lAngDZ6aGOQfHhrDaTTQ42S2sk4v+E5ATSlLm8FseWB\/h8j96bZcgbCp4XKJqzimucu54oiWzGsUZ5Nc+vDHHpnCMLsL1yaoiJn3YFXQsURqmr8g93HIzgCgmPPiK4lAgofEC6hCjudoAoQ2XO+veha9ErBHPkg8gw9yKVF8D6vrApgD+\/yiT1MMZEwF0B\/rG36k3v2Nc5iCFphboL0REN8M+EeBrdDBK87Iqu\/0Lm+FDk8GAMsGvA67P1w5tL3NBXfH\/1lptt8D3re0aNzDuxwcYCuS+fNwzfUGVDMPRSP\/0dBaeL6XFsUHBOzXC5b+wfersLSneWVe0OjgRsgVPXfrGmJkBqWl8cny\/gmEGipDMxwXW6gFPh1CMgY9Jk2Jfbp9+hRKASbo83ybLXgGOCSijqmO3O6h27G71DPizcTLzPvAswA91anxsTXKScv7tNhcpB6Hj37OhYwLCwxcNn2SijPxua3iWXJSmmtqVuCXDtnXBLEeNtxfJSS7tbk\/wzOs1zgtJoTwKimnCEcXu\/2uE8=\", \"Opr\" : \"parse\" }

{ \"GeminiKey\" : \"mWw7gYLPYJ58fKugHbR0d0i+bXl2D8cghp7ljnmlteFByzRo3NodHN96FgdJfK2rDoeFoYebMl2sZCcv3KWe7onMeIk+Eoa6me1XaypCZ0VR7ABvKsmb9mnDPGi9mZahQKPUp3\/mmpsVCjU9reTyOPSM7pjNSz2Smhk8AhWOo3Z\/Yf8zC1FXlF+hDGUv7F0Ej6So9sCBtfx32VMYFBso3Idis6cslMdFRFrWxjUohvMW33REsdFXHWCxXxBVn3GlmNbmuZlwXE1lNdSXDGE3pOOoRMMjx7AWvbPYlDoloq7J3ZxlirhuDrgB2MqpS9UFC7a7MyaJMJ\/KA1Dswq1O7Ryo++N8HF++wFMOwcw\/XFjIA+1TRjavp06V90jf51gyRSky24tWy3hou03PioWE4bDwVL2QTqhjyCh8TDB2YKQjqJmplkq5s\/F6QkLhsm6jiZvJ0O7cqlPJbBT6l0E42SnY8i+aRDU5BV5acvW+BMs1VtxI5Mlizx6AWCvJ4++c5\/qn8R00u3W2qJ4YTSsZJ9W4oa12h7Jc23AdEMEVS3jz7HIQ+Xyul9nciw9VJ+kzIDqOoiKCVI7YE4XhmsUip6k8fNrWzLSSKdAE8MPPjZrlZxbLBkWt3MRSJ5lfQF+BWow2yV3oxyzSSjpw4QMS+qrgxK5zOpkFlUUfZn75QBGBpZbB1fCh5Bukc\/JVn6FJv+EmFaSviHRVdS56x0iUuDdxCuP4\/UrYRDExUK8rIxOxpk\/ytmAmO70gmZXFX6FmRFXDhuues\/2ff3CGhJ3unAqiuh\/+RXXruAXgHj47gvEyLRXkp7lXMDnZi4uItxVudHXrNZWI2kx2fHSQelO00OfSfY5ZGr0lQSliStAGLEPpPxXuhRq\/YWcVsF0l7ix4+IkRQFuCjglZCEBE7DCVQ11vZuJtI4eZpL\/Du2bxW4t0stHFHOpksh4OcY3gEYrIEes9\/fVyU3kb6tTK3q5nixLIAc7tcDEn+Dgl6SSQ5Jm5ZhWhue8HWbtENjMq+Aepd740py9TpCaQJgkEoX8+j8CTxtJEI0Pce\/BGbSdg5TLp0AEXVTLj3KPaw1ht8rAETjFizutamWc9KOKuL9NBxi8mFUwFcTOU4kpaSNjqaciKv5\/9tkbkngz1GfElrlIWXpv+p3kVW9DkjjBDn2nCRj0OL1dQxPMK8dPOvZHiK9v0rgtc2ZOmRundNmlMKUNkrduV9wC7kJqGRNmPI\/TVOclntHjP6PuiqhIk106YHQNl\/ujSEJZSjiBAjhKL6w9zswv7MV+WXlHDoCNdCH4e2YMgG37migjnIVdmNMWeKekrVxmJfKfihSjzJztbLnmxWr+GXrlFrDT4++25HFGftm+vkg0nV9sOG2CxnTJPhBVCblvENN57QnNjBVweyrmSK7dnk6AP6WXmY1Gm\/Sqs1P6s6Y\/spMHTDzSQJCu8FuVIQ2bVPo4XRjsWvp88bXD72nOidmoI+gG3gkvywihMOpr+tQ+uvGfcbJC75fiJnzhntVY2ohEKNMcTz6MlrB4KgZUVeLwpbN0E2W0pS48aNQ44ydwtgB833S1rt9aIpzj7g48TDDU01lPKJXL0Fz\/F2IZ5WX2mugsOGJrtKsWWufeNf15\/AaMamBdy8KtcebSb8UWo4NRBekKScyTUVgiN3HOI6n00cQEHWqLHEtBSWJMJKp7CNWS0Xaeo78b8ifNAL6cNx1teRCIRWf4crnzVwCh+cTypUUjpn5Baf353Zk31+sRl8dNLTeXNmwt91KVkufYWibUs2YxW05olQROQX+4W0lOmxeTjzFAJG+LdF7sSJKyp7+wN9CWfLBifUnlwt6hf9bCW1Nx2Qt8hlUrwwqhL2zMpkplNeGLAczAcmBbJzZPqOEsvYscuOGKTx8awfo097mNn8g==\", \"Opr\" : \"parse\" }

curl http://localhost:60088/gLock/onKey -d "{ \"GeminiKey\" : \"0ybAcPFCCxotoAd4QZErd2iOY\/dIUx6Gs6aZQnoulxdQ6njegPL13aEJeQzVmAtTidPdrHXGif2SIVFQ5lAngDZ6aGOQfHhrDaTTQ42S2sk4v+E5ATSlLm8FseWB\/h8j96bZcgbCp4XKJqzimucu54oiWzGsUZ5Nc+vDHHpnCMLsL1yaoiJn3YFXQsURqmr8g93HIzgCgmPPiK4lAgofEC6hCjudoAoQ2XO+veha9ErBHPkg8gw9yKVF8D6vrApgD+\/yiT1MMZEwF0B\/rG36k3v2Nc5iCFphboL0REN8M+EeBrdDBK87Iqu\/0Lm+FDk8GAMsGvA67P1w5tL3NBXfH\/1lptt8D3re0aNzDuxwcYCuS+fNwzfUGVDMPRSP\/0dBaeL6XFsUHBOzXC5b+wfersLSneWVe0OjgRsgVPXfrGmJkBqWl8cny\/gmEGipDMxwXW6gFPh1CMgY9Jk2Jfbp9+hRKASbo83ybLXgGOCSijqmO3O6h27G71DPizcTLzPvAswA91anxsTXKScv7tNhcpB6Hj37OhYwLCwxcNn2SijPxua3iWXJSmmtqVuCXDtnXBLEeNtxfJSS7tbk\/wzOs1zgtJoTwKimnCEcXu\/2uE8=\", \"Opr\" : \"parse\" }"
curl http://localhost:60088/gLock/onKey -d "{ \"GeminiKey\" : \"mWw7gYLPYJ58fKugHbR0d0i+bXl2D8cghp7ljnmlteFByzRo3NodHN96FgdJfK2rDoeFoYebMl2sZCcv3KWe7onMeIk+Eoa6me1XaypCZ0VR7ABvKsmb9mnDPGi9mZahQKPUp3\/mmpsVCjU9reTyOPSM7pjNSz2Smhk8AhWOo3Z\/Yf8zC1FXlF+hDGUv7F0Ej6So9sCBtfx32VMYFBso3Idis6cslMdFRFrWxjUohvMW33REsdFXHWCxXxBVn3GlmNbmuZlwXE1lNdSXDGE3pOOoRMMjx7AWvbPYlDoloq7J3ZxlirhuDrgB2MqpS9UFC7a7MyaJMJ\/KA1Dswq1O7Ryo++N8HF++wFMOwcw\/XFjIA+1TRjavp06V90jf51gyRSky24tWy3hou03PioWE4bDwVL2QTqhjyCh8TDB2YKQjqJmplkq5s\/F6QkLhsm6jiZvJ0O7cqlPJbBT6l0E42SnY8i+aRDU5BV5acvW+BMs1VtxI5Mlizx6AWCvJ4++c5\/qn8R00u3W2qJ4YTSsZJ9W4oa12h7Jc23AdEMEVS3jz7HIQ+Xyul9nciw9VJ+kzIDqOoiKCVI7YE4XhmsUip6k8fNrWzLSSKdAE8MPPjZrlZxbLBkWt3MRSJ5lfQF+BWow2yV3oxyzSSjpw4QMS+qrgxK5zOpkFlUUfZn75QBGBpZbB1fCh5Bukc\/JVn6FJv+EmFaSviHRVdS56x0iUuDdxCuP4\/UrYRDExUK8rIxOxpk\/ytmAmO70gmZXFX6FmRFXDhuues\/2ff3CGhJ3unAqiuh\/+RXXruAXgHj47gvEyLRXkp7lXMDnZi4uItxVudHXrNZWI2kx2fHSQelO00OfSfY5ZGr0lQSliStAGLEPpPxXuhRq\/YWcVsF0l7ix4+IkRQFuCjglZCEBE7DCVQ11vZuJtI4eZpL\/Du2bxW4t0stHFHOpksh4OcY3gEYrIEes9\/fVyU3kb6tTK3q5nixLIAc7tcDEn+Dgl6SSQ5Jm5ZhWhue8HWbtENjMq+Aepd740py9TpCaQJgkEoX8+j8CTxtJEI0Pce\/BGbSdg5TLp0AEXVTLj3KPaw1ht8rAETjFizutamWc9KOKuL9NBxi8mFUwFcTOU4kpaSNjqaciKv5\/9tkbkngz1GfElrlIWXpv+p3kVW9DkjjBDn2nCRj0OL1dQxPMK8dPOvZHiK9v0rgtc2ZOmRundNmlMKUNkrduV9wC7kJqGRNmPI\/TVOclntHjP6PuiqhIk106YHQNl\/ujSEJZSjiBAjhKL6w9zswv7MV+WXlHDoCNdCH4e2YMgG37migjnIVdmNMWeKekrVxmJfKfihSjzJztbLnmxWr+GXrlFrDT4++25HFGftm+vkg0nV9sOG2CxnTJPhBVCblvENN57QnNjBVweyrmSK7dnk6AP6WXmY1Gm\/Sqs1P6s6Y\/spMHTDzSQJCu8FuVIQ2bVPo4XRjsWvp88bXD72nOidmoI+gG3gkvywihMOpr+tQ+uvGfcbJC75fiJnzhntVY2ohEKNMcTz6MlrB4KgZUVeLwpbN0E2W0pS48aNQ44ydwtgB833S1rt9aIpzj7g48TDDU01lPKJXL0Fz\/F2IZ5WX2mugsOGJrtKsWWufeNf15\/AaMamBdy8KtcebSb8UWo4NRBekKScyTUVgiN3HOI6n00cQEHWqLHEtBSWJMJKp7CNWS0Xaeo78b8ifNAL6cNx1teRCIRWf4crnzVwCh+cTypUUjpn5Baf353Zk31+sRl8dNLTeXNmwt91KVkufYWibUs2YxW05olQROQX+4W0lOmxeTjzFAJG+LdF7sSJKyp7+wN9CWfLBifUnlwt6hf9bCW1Nx2Qt8hlUrwwqhL2zMpkplNeGLAczAcmBbJzZPqOEsvYscuOGKTx8awfo097mNn8g==\", \"Opr\" : \"parse\" }"

curl http://localhost:60088/gLock/onKey -d "{ \"GeminiKey\" : \"0ybAcPFCCxotoAd4QZErd2iOY\/dIUx6Gs6aZQnoulxdQ6njegPL13aEJeQzVmAtTidPdrHXGif2SIVFQ5lAngDZ6aGOQfHhrDaTTQ42S2sk4v+E5ATSlLm8FseWB\/h8j96bZcgbCp4XKJqzimucu54oiWzGsUZ5Nc+vDHHpnCMLsL1yaoiJn3YFXQsURqmr8g93HIzgCgmPPiK4lAgofEC6hCjudoAoQ2XO+veha9ErBHPkg8gw9yKVF8D6vrApgD+\/yiT1MMZEwF0B\/rG36k3v2Nc5iCFphboL0REN8M+EeBrdDBK87Iqu\/0Lm+FDk8GAMsGvA67P1w5tL3NBXfH\/1lptt8D3re0aNzDuxwcYCuS+fNwzfUGVDMPRSP\/0dBaeL6XFsUHBOzXC5b+wfersLSneWVe0OjgRsgVPXfrGmJkBqWl8cny\/gmEGipDMxwXW6gFPh1CMgY9Jk2Jfbp9+hRKASbo83ybLXgGOCSijqmO3O6h27G71DPizcTLzPvAswA91anxsTXKScv7tNhcpB6Hj37OhYwLCwxcNn2SijPxua3iWXJSmmtqVuCXDtnXBLEeNtxfJSS7tbk\/wzOs1zgtJoTwKimnCEcXu\/2uE8=\", \"Opr\" : \"parse\" }"
curl http://localhost:60088/gLock/onKey -d "{ \"GeminiKey\" : \"mWw7gYLPYJ58fKugHbR0d0i+bXl2D8cghp7ljnmlteFByzRo3NodHN96FgdJfK2rDoeFoYebMl2sZCcv3KWe7onMeIk+Eoa6me1XaypCZ0VR7ABvKsmb9mnDPGi9mZahQKPUp3\/mmpsVCjU9reTyOPSM7pjNSz2Smhk8AhWOo3Z\/Yf8zC1FXlF+hDGUv7F0Ej6So9sCBtfx32VMYFBso3Idis6cslMdFRFrWxjUohvMW33REsdFXHWCxXxBVn3GlmNbmuZlwXE1lNdSXDGE3pOOoRMMjx7AWvbPYlDoloq7J3ZxlirhuDrgB2MqpS9UFC7a7MyaJMJ\/KA1Dswq1O7Ryo++N8HF++wFMOwcw\/XFjIA+1TRjavp06V90jf51gyRSky24tWy3hou03PioWE4bDwVL2QTqhjyCh8TDB2YKQjqJmplkq5s\/F6QkLhsm6jiZvJ0O7cqlPJbBT6l0E42SnY8i+aRDU5BV5acvW+BMs1VtxI5Mlizx6AWCvJ4++c5\/qn8R00u3W2qJ4YTSsZJ9W4oa12h7Jc23AdEMEVS3jz7HIQ+Xyul9nciw9VJ+kzIDqOoiKCVI7YE4XhmsUip6k8fNrWzLSSKdAE8MPPjZrlZxbLBkWt3MRSJ5lfQF+BWow2yV3oxyzSSjpw4QMS+qrgxK5zOpkFlUUfZn75QBGBpZbB1fCh5Bukc\/JVn6FJv+EmFaSviHRVdS56x0iUuDdxCuP4\/UrYRDExUK8rIxOxpk\/ytmAmO70gmZXFX6FmRFXDhuues\/2ff3CGhJ3unAqiuh\/+RXXruAXgHj47gvEyLRXkp7lXMDnZi4uItxVudHXrNZWI2kx2fHSQelO00OfSfY5ZGr0lQSliStAGLEPpPxXuhRq\/YWcVsF0l7ix4+IkRQFuCjglZCEBE7DCVQ11vZuJtI4eZpL\/Du2bxW4t0stHFHOpksh4OcY3gEYrIEes9\/fVyU3kb6tTK3q5nixLIAc7tcDEn+Dgl6SSQ5Jm5ZhWhue8HWbtENjMq+Aepd740py9TpCaQJgkEoX8+j8CTxtJEI0Pce\/BGbSdg5TLp0AEXVTLj3KPaw1ht8rAETjFizutamWc9KOKuL9NBxi8mFUwFcTOU4kpaSNjqaciKv5\/9tkbkngz1GfElrlIWXpv+p3kVW9DkjjBDn2nCRj0OL1dQxPMK8dPOvZHiK9v0rgtc2ZOmRundNmlMKUNkrduV9wC7kJqGRNmPI\/TVOclntHjP6PuiqhIk106YHQNl\/ujSEJZSjiBAjhKL6w9zswv7MV+WXlHDoCNdCH4e2YMgG37migjnIVdmNMWeKekrVxmJfKfihSjzJztbLnmxWr+GXrlFrDT4++25HFGftm+vkg0nV9sOG2CxnTJPhBVCblvENN57QnNjBVweyrmSK7dnk6AP6WXmY1Gm\/Sqs1P6s6Y\/spMHTDzSQJCu8FuVIQ2bVPo4XRjsWvp88bXD72nOidmoI+gG3gkvywihMOpr+tQ+uvGfcbJC75fiJnzhntVY2ohEKNMcTz6MlrB4KgZUVeLwpbN0E2W0pS48aNQ44ydwtgB833S1rt9aIpzj7g48TDDU01lPKJXL0Fz\/F2IZ5WX2mugsOGJrtKsWWufeNf15\/AaMamBdy8KtcebSb8UWo4NRBekKScyTUVgiN3HOI6n00cQEHWqLHEtBSWJMJKp7CNWS0Xaeo78b8ifNAL6cNx1teRCIRWf4crnzVwCh+cTypUUjpn5Baf353Zk31+sRl8dNLTeXNmwt91KVkufYWibUs2YxW05olQROQX+4W0lOmxeTjzFAJG+LdF7sSJKyp7+wN9CWfLBifUnlwt6hf9bCW1Nx2Qt8hlUrwwqhL2zMpkplNeGLAczAcmBbJzZPqOEsvYscuOGKTx8awfo097mNn8g==\", \"Opr\" : \"parse\" }"

*/

#endif
}


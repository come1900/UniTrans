/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * GeminiKey.cpp - brief
 *
 * Copyright (C) 2013 ezlibs.com, All Rights Reserved.
 *
 * $Id: GeminiKey.cpp 5884 2013-09-02 10:26:32Z WuJunjie $
 *
 *  Notes:
 *     -
 *      explain
 *     -
 *
 *  Update:
 *     2013-09-02 10:26:32 WuJunjie Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
#include "GeminiKey.h"

GeminiKey::GeminiKey()
{
    m_bValid = false;
#ifdef USE_TIME_STAMP_IN_KEY    	
    strTimeStamp = "20141027185509";
#endif //USE_TIME_STAMP_IN_KEY
}

/* == */
bool GeminiKey::operator==(const GeminiKey& obj) const
{
    if (
        this->m_bValid != obj.m_bValid ||

        this->strVerifyCode != obj.strVerifyCode ||
        this->strPeerID        != obj.strPeerID        ||
        this->strKeyType       != obj.strKeyType       ||
#ifdef USE_TIME_STAMP_IN_KEY    	
        this->strTimeStamp       != obj.strTimeStamp       ||
//        this->strMemo       != obj.strMemo       ||
#endif
        this->strKeyString  != obj.strKeyString
    )
    {
        return false;
    }

    return true;
}

/* = */
GeminiKey& GeminiKey::operator = (const GeminiKey& obj)
{
    this->m_bValid  = obj.m_bValid ;

    this->strVerifyCode = obj.strVerifyCode;
    this->strPeerID        = obj.strPeerID       ;
    this->strKeyType       = obj.strKeyType      ;
    this->strKeyString  = obj.strKeyString ;

#ifdef USE_TIME_STAMP_IN_KEY    	
    this->strTimeStamp       = obj.strTimeStamp      ;
    this->strMemo  = obj.strMemo ;
#endif
    return *this;
}

// 生成校验码
int GeminiKey::GenVerifyCode(std::string &strVerifyCode)
{
    if (strPeerID.empty()||strKeyType.empty()||strKeyString.empty())
    {
        return -1;
    }

    char bufOut[VERIFY_CODE_LEN+1]={0};

    std::string tmpStr = strPeerID+strKeyType+strKeyString;// +strTimeStamp;
    memset(bufOut, 0, sizeof(bufOut));
    ez_md5_calc_8ascii_digit((unsigned char *)bufOut, VERIFY_CODE_LEN, (unsigned char *)tmpStr.c_str(), tmpStr.size());
    strVerifyCode = bufOut;

    return 0;
}
void GeminiKey::dump() const //discards qualifiers 声明不会改变
{
    std::cout << "strVerifyCode :" << strVerifyCode   << std::endl;
    std::cout << "strPeerID        :" << strPeerID          << std::endl;
    std::cout << "strKeyType       :" << strKeyType         << std::endl;
    std::cout << "strKeyString  :" << strKeyString    << std::endl;
#ifdef USE_TIME_STAMP_IN_KEY    	
    std::cout << "strTimeStamp  :" << strTimeStamp    << std::endl;
    std::cout << "strMemo  :" << strMemo    << std::endl;
#endif //USE_TIME_STAMP_IN_KEY
}

void GeminiKey::dump(std::string &strDest) const //discards qualifiers 声明不会改变
{
    strDest = "strVerifyCode :" + strVerifyCode   + "\n";
    strDest += "strPeerID        :" + strPeerID          + "\n";
    strDest += "strKeyType       :" + strKeyType         + "\n";
    strDest += "strKeyString  :" + strKeyString    + "\n";
#ifdef USE_TIME_STAMP_IN_KEY    	
    strDest += "strTimeStamp  :" + strTimeStamp    + "\n";
    strDest += "strMemo  :" + strMemo    + "\n";
#endif //USE_TIME_STAMP_IN_KEY
}

// 数据校验
bool GeminiKey::IsValid() const
{
#if 1
    return m_bValid;
#else

    std::string strVerifyCode;

    if (GenVerifyCode(strVerifyCode) >= 0 && strVerifyCode == strVerifyCode)
    {
        return true;
    }

    return false;
#endif
}

// 刷新校验码
int GeminiKey::freshVerifyCode()
{
    int iret = GenVerifyCode(strVerifyCode);

    if (iret>=0)
    {
        m_bValid = true;
    }
    else
    {
        m_bValid = false;
    }

    return iret;
}

bool GeminiKey::checkVerifyCode()
{
    std::string __strVerifyCode;

    m_bValid = false;

    if (GenVerifyCode(__strVerifyCode) >= 0 && __strVerifyCode == strVerifyCode)
    {
        m_bValid = true;
    }

    return m_bValid;
}


/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
GeminiKeys::GeminiKeys()
{
    Identity = GeminiKeyIdentity;
    Version = GeminiKeyVersion;
    strFunString = "LT"; // long tail
}
/* == */
bool GeminiKeys::operator==(const GeminiKeys& obj) const
{
    if (
        this->Identity != obj.Identity ||
        this->Version        != obj.Version ||
        this->m_vecGeminiKey.size() != obj.m_vecGeminiKey.size()
    )
    {
        return false;
    }

    for (size_t ii=0; ii<this->m_vecGeminiKey.size(); ii++)
    {
        if ( !(this->m_vecGeminiKey[ii]==obj.m_vecGeminiKey[ii]))
        {
            return false;
        }
    }

    return true;
}

/* = */
GeminiKeys& GeminiKeys::operator = (const GeminiKeys& obj)
{
    this->Identity = obj.Identity;
    this->Version        = obj.Version;
    this->m_vecGeminiKey.assign(obj.m_vecGeminiKey.begin(), obj.m_vecGeminiKey.end());

    return *this;
}

void GeminiKeys::dump() const //discards qualifiers 声明不会改变
{
    std::cout << "Identity :" << Identity   << std::endl;
    std::cout << "Version :" << Version   << std::endl;
    std::cout << "FunString :" << strFunString   << std::endl;

    for (std::vector<GeminiKey>::const_iterator iter = m_vecGeminiKey.begin();
         iter != m_vecGeminiKey.end(); ++iter)
    {
        iter->dump() ; // print each element in text

        std::cout << "--------------------------------------" << std::endl;
    }
}

void GeminiKeys::dump(std::string &strDest) const //discards qualifiers 声明不会改变
{
    std::string strKey;

    strDest = "Identity :" + Identity   + "\n";
    strDest += "Version :" + Version  + "\n";
    strDest += "FunString :" + strFunString  + "\n";

    for (std::vector<GeminiKey>::const_iterator iter = m_vecGeminiKey.begin();
         iter != m_vecGeminiKey.end(); ++iter)
    {
        iter->dump(strKey) ; // print each element in text
        strDest+=strKey;

        //std::cout << "--------------------------------------" << std::endl;
    }
}

// 密钥数据必须有，其他暂不做检查
bool GeminiKeys::isValid() const
{
    if (m_vecGeminiKey.empty())
    {
        return false;
    }

    for (std::vector<GeminiKey>::const_iterator iter = m_vecGeminiKey.begin();
         iter != m_vecGeminiKey.end(); ++iter)
    {
        if (!iter->IsValid())
        {
            return false;
        }
    }
    return true;
}

bool GeminiKeys::checkVerifyCode()
{
    if (m_vecGeminiKey.empty())
    {
        return false;
    }

    for (std::vector<GeminiKey>::iterator iter = m_vecGeminiKey.begin();
         iter != m_vecGeminiKey.end(); ++iter)
    {
        // 校验不通过
        if (!iter->checkVerifyCode())
        {
            return false;
        }
    }

    return true;
}


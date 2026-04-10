/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * Rsa.h - _explain_
 *
 * Copyright (C) 2011 jsict.com, All Rights Reserved.
 *
 * $Id: Rsa.h 5884 2019-03-21 02:35:18Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2019-03-21 02:35:18  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
#ifndef ezRSA_H
#define ezRSA_H

#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/err.h>
#include <string>

// 2048 可以加密字符串长度为256， 生成的密码为UUID， 长度为36 
// 1024 可以加密字符串长度为128， 生成的密码为UUID， 长度为36 
#define def_default_ezRSA_KEY_LENGTH 2048

// openssl.exe genrsa -out rsa_private_key.pem 1024
// openssl.exe rsa -in rsa_private_key.pem -pubout -out rsa_public_key.pem
class ezRSA
{
public:
    ezRSA();

    // 生成 ezRSA 私钥公钥
    bool generateRSAKey(std::string& priKey, std::string& pubKey, int bits=def_default_ezRSA_KEY_LENGTH);

    // 将 PEM 转换为大数字符串(只支持公钥方法，私钥方法需要导出n,e,d,p,q,dmp1,dmq1,iqmp所以这里就不支持了)
    bool PublicPemString2HexString(const std::string& key, std::string& modulus, std::string& exponent);
    //bool PrivatePemString2HexString(const std::string& key, std::string& modulus, std::string& exponent, std::string& pri_exponent);

    // 将大数字符串转换为 PEM 字符串
    bool PublicHexString2PemString(const std::string& modulus, const std::string& exponent, std::string& key);
    //bool PrivateHexString2PemString(const std::string& modulus, const std::string& exponent, const std::string& pri_exponent, std::string& key);

    // 公钥加密
    bool RsaPublicEncrypt(const std::string &plainText, const std::string &pubKey, std::string& cipherText);

    // 私钥解密
    bool RsaPrivateDecrypt(const std::string &cipherText, const std::string &priKey, std::string& plainText);

    // 私钥加密
    bool RsaPrivateEncrypt(const std::string &plainText, const std::string &priKey, std::string& cipherText);

    // 公钥解密
    bool RsaPublicDecrypt(const std::string &cipherText, const std::string &pubKey, std::string& plainText);

    // 获取错误信息
    std::string errorString();
};

#endif // ezRSA_H

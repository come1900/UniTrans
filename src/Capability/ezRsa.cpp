/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * Rsa.cpp - _explain_
 *
 * Copyright (C) 2011 jsict.com, All Rights Reserved.
 *
 * $Id: Rsa.cpp 5884 2019-03-21 02:35:09Z WuJunjie $
 *
 *  Explain:
 *     -
 *      _FUNC_STANDALONE_UTIL :: Unit Test
 *     -
 *
 *  Update:
 *     2019-03-21 02:35:09  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ezRsa.h"

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

ezRSA::ezRSA()
{
    ERR_load_crypto_strings();
}

bool ezRSA::generateRSAKey(std::string &priKey, std::string &pubKey, const int bits)
{
    bool result = false;

    // 生成密钥对
    unsigned long e = RSA_3;
    BIGNUM *bne = BN_new();
    int ret = BN_set_word(bne, e);
    RSA *keypair = RSA_new();
    if (keypair)
    {
        ret = RSA_generate_key_ex(keypair, bits, bne, NULL);
        if (ret == 1)
        {
            BIO *pri = BIO_new(BIO_s_mem());
            BIO *pub = BIO_new(BIO_s_mem());
            if (pri != NULL && pub != NULL)
            {
                ret = PEM_write_bio_RSAPrivateKey(pri, keypair, NULL, NULL, 0, NULL, NULL);
                ret = PEM_write_bio_RSAPublicKey(pub, keypair);

                // 获取长度
                size_t pri_len;
                size_t pub_len;
                pri_len = BIO_pending(pri);
                pub_len = BIO_pending(pub);

                // 密钥对读取到字符串
                char *pri_key = NULL;
                char *pub_key = NULL;
                pri_key = (char *)malloc(pri_len + 1);
                pub_key = (char *)malloc(pub_len + 1);

                BIO_read(pri, pri_key, pri_len);
                BIO_read(pub, pub_key, pub_len);

                pri_key[pri_len] = '\0';
                pub_key[pub_len] = '\0';

                priKey = std::string(pri_key);
                pubKey = std::string(pub_key);

                free(pri_key);
                free(pub_key);

                result = true;
            }
            if (pri)
                BIO_free_all(pri);
            if (pub)
                BIO_free_all(pub);
        }
        RSA_free(keypair);
    }
    return result;
}

bool ezRSA::PublicPemString2HexString(const std::string &key, std::string &modulus, std::string &exponent)
{
    bool result = false;
    BIO *keybio = BIO_new_mem_buf((unsigned char *)key.c_str(), -1);
    if (keybio != NULL)
    {
        RSA* rsa = NULL;
        rsa = PEM_read_bio_RSAPublicKey(keybio, &rsa, NULL, NULL);
        if (rsa)
        {
#if OPENSSL_VERSION_NUMBER < 0x10100000L
/* OpenSSL 1.0.2 and below (old code) */
            modulus = std::string(BN_bn2hex(rsa->n));
            exponent = std::string(BN_bn2hex(rsa->e));
#else
/* OpenSSL 1.1.0 and above (new code) */
            modulus = std::string(BN_bn2hex(RSA_get0_n(rsa)));
            exponent = std::string(BN_bn2hex(RSA_get0_e(rsa)));
#endif
            result = true;

            RSA_free(rsa);
        }
        BIO_free_all(keybio);
    }
    return result;
}

//bool ezRSA::PrivatePemString2HexString(const std::string &key, std::string &modulus, std::string &exponent, std::string &pri_exponent)
//{
//    bool result = false;
//    BIO *keybio = BIO_new_mem_buf((unsigned char *)key.c_str(), -1);
//    if (keybio != NULL)
//    {
//        RSA* rsa = NULL;
//        rsa = PEM_read_bio_RSAPrivateKey(keybio, &rsa, NULL, NULL);
//        if (rsa)
//        {
//            modulus = std::string(BN_bn2hex(rsa->n));
//            exponent = std::string(BN_bn2hex(rsa->e));
//            pri_exponent = std::string(BN_bn2hex(rsa->d));
//            result = true;

//            RSA_free(rsa);
//        }
//        BIO_free_all(keybio);
//    }
//    return result;
//}

bool ezRSA::PublicHexString2PemString(const std::string &modulus, const std::string &exponent, std::string &key)
{
    bool result = false;
    RSA *rsa = RSA_new();
    if (rsa)
    {
#if OPENSSL_VERSION_NUMBER < 0x10100000L
/* OpenSSL 1.0.2 and below (old code) */
        rsa->n = BN_new();
        rsa->e = BN_new();
        BN_hex2bn(&rsa->n, modulus.c_str());
        BN_hex2bn(&rsa->e, exponent.c_str());
#else
/* OpenSSL 1.1.0 and above (new code) */
        BIGNUM *rsa_n = BN_new();
        BIGNUM *rsa_e = BN_new();
        BN_hex2bn(&rsa_n, modulus.c_str());
        BN_hex2bn(&rsa_e, exponent.c_str());
        RSA_set0_key(rsa, rsa_n, rsa_e, NULL);
#endif

        BIO *bio = BIO_new(BIO_s_mem());
        if (bio != NULL)
        {
            PEM_write_bio_RSAPublicKey(bio, rsa);

            // 获取长度
            size_t len = BIO_pending(bio);

            // 密钥对读取到字符串
            char *buf = (char *)malloc(len + 1);
            BIO_read(bio, buf, len);
            buf[len] = '\0';

            key = std::string(buf);

            free(buf);
            BIO_free_all(bio);
            result = true;
        }

        RSA_free(rsa);
    }
    return result;
}

//bool ezRSA::PrivateHexString2PemString(const std::string &modulus, const std::string &exponent, const std::string& pri_exponent, std::string &key)
//{
//    bool result = false;
//    ezRSA *rsa = ezRSA_new();
//    if (rsa)
//    {

//        BIO *keybio = BIO_new_mem_buf((unsigned char *)modulus.c_str(), -1);
//        if (keybio != NULL)
//        {
//            RSA* rsa1 = NULL;
//            rsa1 = PEM_read_bio_RSAPrivateKey(keybio, &rsa1, NULL, NULL);
//            if (rsa1)
//            {

//                rsa->n = BN_new();
//                rsa->e = BN_new();
//                rsa->d = BN_new();
//                rsa->p = BN_new();
//                rsa->q = BN_new();
//                rsa->dmp1 = BN_new();
//                rsa->dmq1 = BN_new();
//                rsa->iqmp = BN_new();

//                BN_hex2bn(&rsa->n, BN_bn2hex(rsa1->n));
//                BN_hex2bn(&rsa->e, BN_bn2hex(rsa1->e));
//                BN_hex2bn(&rsa->d, BN_bn2hex(rsa1->d));
//                BN_hex2bn(&rsa->p, BN_bn2hex(rsa1->p));
//                BN_hex2bn(&rsa->q, BN_bn2hex(rsa1->q));

//                BN_hex2bn(&rsa->dmp1, BN_bn2hex(rsa1->dmp1));
//                BN_hex2bn(&rsa->dmq1, BN_bn2hex(rsa1->dmq1));
//                BN_hex2bn(&rsa->iqmp, BN_bn2hex(rsa1->iqmp));

//qDebug() <<" 1111";
//                qDebug() << BN_bn2hex(rsa1->d);
//                qDebug() <<" 1111";
//                qDebug() << BN_bn2hex(rsa1->p);
//qDebug() <<" 1111";
//                qDebug() << BN_bn2hex(rsa1->q);

//                qDebug() << BN_bn2hex(rsa1->dmp1);
//qDebug() <<" 1111";
//                qDebug() << BN_bn2hex(rsa1->dmq1);
//                qDebug() <<" 1111";
//                qDebug() << BN_bn2hex(rsa1->iqmp);

//qDebug() <<" ";
//                RSA_free(rsa1);
//            }
//            BIO_free_all(keybio);
//        }
//        //return result;

//        //rsa->n = BN_new();
//        //rsa->e = BN_new();
//        //rsa->d = BN_new();

//        //BN_hex2bn(&rsa->n, modulus.c_str());
//        //BN_hex2bn(&rsa->e, exponent.c_str());
//        //BN_hex2bn(&rsa->d, pri_exponent.c_str());

//        BIO *bio = BIO_new(BIO_s_mem());
//        if (bio != NULL)
//        {
//            PEM_write_bio_ezRSAPrivateKey(bio, rsa, NULL, NULL, 0, NULL, NULL);

//            // 获取长度
//            size_t len = BIO_pending(bio);;

//            // 密钥对读取到字符串
//            char *buf = (char *)malloc(len + 1);
//            BIO_read(bio, buf, len);
//            buf[len] = '\0';

//            key = std::string(buf);

//            free(buf);
//            BIO_free_all(bio);
//            result = true;
//        }

//        RSA_free(rsa);
//    }
//    return result;
//}

bool ezRSA::RsaPublicEncrypt(const std::string &plainText, const std::string &pubKey, std::string& cipherText)
{
    cipherText.clear();
    bool result = false;
    BIO *keybio = BIO_new_mem_buf((unsigned char *)pubKey.c_str(), -1);
    if (keybio != NULL)
    {
        RSA* rsa = NULL;
        rsa = PEM_read_bio_RSAPublicKey(keybio, &rsa, NULL, NULL);
        if (rsa)
        {
            // 分段加密
            // 根据密钥计算出ezRSA支持加密最大块长度
            int rsa_len = RSA_size(rsa);
            int rsa_block_len = rsa_len - RSA_PKCS1_PADDING_SIZE;
            DBG(
                printf("rsa_len:%d\n", rsa_len);
                printf("rsa_block_len:%d\n", rsa_block_len);
            );
            char *buf = (char *)malloc(rsa_len + 1);
            memset(buf, 0, rsa_len + 1);
            int from_len = plainText.length();
            const char *from = plainText.c_str();
            int offset = 0;
            while ((from_len - offset) > 0)
            {
                int len = rsa_block_len;
                if ((from_len - offset) < rsa_block_len)
                    len = (from_len - offset);

                // 加密函数
                int ret = RSA_public_encrypt(len, (const unsigned char*)(from+offset), (unsigned char*)buf, rsa, RSA_PKCS1_PADDING);
                if (ret >= 0)
                {
                    cipherText += std::string(buf, ret);
                    result = true;
                }
                else
                {
                    result = false;
                    break;
                }

                offset += len;
            }

            // 释放内存
            free(buf);
            RSA_free(rsa);
        }
        BIO_free_all(keybio);
    }
    return result;
}

bool ezRSA::RsaPrivateDecrypt(const std::string &cipherText, const std::string &priKey, std::string &plainText)
{
    plainText.clear();
    bool result = false;
    BIO *keybio = BIO_new_mem_buf((unsigned char *)priKey.c_str(), -1);
    if (keybio != NULL)
    {
        RSA* rsa = NULL;
        rsa = PEM_read_bio_RSAPrivateKey(keybio, &rsa, NULL, NULL);
        if (rsa)
        {
            // 分段加密
            // 根据密钥计算出ezRSA支持加密最大块长度
            int rsa_len = RSA_size(rsa);
            int rsa_block_len = rsa_len;
            char *buf = (char *)malloc(rsa_len + 1);
            memset(buf, 0, rsa_len + 1);
            int from_len = cipherText.length();
            const char *from = cipherText.c_str();
            int offset = 0;
            while ((from_len - offset) > 0)
            {
                int len = rsa_block_len;
                if ((from_len - offset) < rsa_block_len)
                    len = (from_len - offset);

                // 解密函数
                int ret = RSA_private_decrypt(len, (const unsigned char*)(from+offset), (unsigned char*)buf, rsa, RSA_PKCS1_PADDING);
                if (ret >= 0)
                {
                    plainText += std::string(buf, ret);
                    result = true;
                }
                else
                {
                    result = false;
                    break;
                }

                offset += len;
            }

            // 释放内存
            free(buf);
            RSA_free(rsa);
        }
        BIO_free_all(keybio);
    }
    return result;
}

bool ezRSA::RsaPrivateEncrypt(const std::string &plainText, const std::string &priKey, std::string &cipherText)
{
    cipherText.clear();
    bool result = false;
    BIO *keybio = BIO_new_mem_buf((unsigned char *)priKey.c_str(), -1);
    if (keybio != NULL)
    {
        RSA* rsa = NULL;
        rsa = PEM_read_bio_RSAPrivateKey(keybio, &rsa, NULL, NULL);
        if (rsa)
        {
            // 分段加密
            // 根据密钥计算出ezRSA支持加密最大块长度
            int rsa_len = RSA_size(rsa);
            int rsa_block_len = rsa_len - RSA_PKCS1_PADDING_SIZE;
            char *buf = (char *)malloc(rsa_len + 1);
            memset(buf, 0, rsa_len + 1);
            int from_len = plainText.length();
            const char *from = plainText.c_str();
            int offset = 0;
            while ((from_len - offset) > 0)
            {
                int len = rsa_block_len;
                if ((from_len - offset) < rsa_block_len)
                    len = (from_len - offset);

                // 加密函数
                int ret = RSA_private_encrypt(len, (const unsigned char*)(from+offset), (unsigned char*)buf, rsa, RSA_PKCS1_PADDING);
                if (ret >= 0)
                {
                    cipherText += std::string(buf, ret);
                    result = true;
                }
                else
                {
                    result = false;
                    break;
                }

                offset += len;
            }

            // 释放内存
            free(buf);
            RSA_free(rsa);
        }
        BIO_free_all(keybio);
    }
    return result;
}

bool ezRSA::RsaPublicDecrypt(const std::string &cipherText, const std::string &pubKey, std::string &plainText)
{
    plainText.clear();
    bool result = false;
    BIO *keybio = BIO_new_mem_buf((unsigned char *)pubKey.c_str(), -1);
    if (keybio != NULL)
    {
        RSA* rsa = NULL;
        rsa = PEM_read_bio_RSAPublicKey(keybio, &rsa, NULL, NULL);
        if (rsa)
        {
            // 分段加密
            // 根据密钥计算出ezRSA支持加密最大块长度
            int rsa_len = RSA_size(rsa);
            int rsa_block_len = rsa_len;
            char *buf = (char *)malloc(rsa_len + 1);
            memset(buf, 0, rsa_len + 1);
            int from_len = cipherText.length();
            const char *from = cipherText.c_str();
            int offset = 0;
            while ((from_len - offset) > 0)
            {
                int len = rsa_block_len;
                if ((from_len - offset) < rsa_block_len)
                    len = (from_len - offset);

                // 解密函数
                int ret = RSA_public_decrypt(len, (const unsigned char*)(from+offset), (unsigned char*)buf, rsa, RSA_PKCS1_PADDING);
                if (ret >= 0)
                {
                    plainText += std::string(buf, ret);
                    result = true;
                }
                else
                {
                    result = false;
                    break;
                }

                offset += len;
            }

            // 释放内存
            free(buf);
            RSA_free(rsa);
        }
        BIO_free_all(keybio);
    }
    return result;
}

std::string ezRSA::errorString()
{
    const char *err = ERR_reason_error_string(ERR_get_error());
    return err;
}

#ifdef _FUNC_STANDALONE_UTIL

#include <stdio.h>
#include <iostream>

#include <ezutil/ez_system_api.h>

// g++ -D_FUNC_STANDALONE_UTIL -I$HOME/libs/include/ -I$HOME/libs/openssl-linux/include/ -L$HOME/libs/lib -L$HOME/libs/openssl-linux/lib ezRsa.cpp -lcrypto -lezutil-linux

int main(int argc, char *argv[])
{
    std::string priKey;
    std::string pubKey;
    ezRSA __rs;
    bool bRet = false;

    unsigned int iTimes = 0;
    struct timeval tv_start;
    struct timeval tv_now;
    struct timeval t_sub_time;

    printf("%8s %18s %10s %s\n", "Round", "Do", "Used(ms)", "Memo");

    do
    {
        printf("%8u ", ++iTimes );

        // 秘钥生成
        {
            gettimeofday(&tv_start, NULL);
            printf("%18s ", "generateRSAKey");

            bRet = __rs.generateRSAKey(priKey, pubKey);

            gettimeofday(&tv_now, NULL);
            ez_timeval_subtract(&t_sub_time, &tv_now, &tv_start);
            printf("%10d ",  (int)(t_sub_time.tv_sec*1000 + t_sub_time.tv_usec / 1000.0) );

            if (bRet)
            {
                printf("%s\n", "Success");
                //std::cout << "prikey =========================" << std::endl << priKey << std::endl;
                //std::cout << "pubkey =========================" << std::endl << pubKey << std::endl;
            }
            else
            {
                printf("%s\n", "Failed");

                break;
            }


        }

        //
        // 加解密
        {
            printf("%8s ", "");

            std::string plainText = "f7f8117a-f4fa-0c0c-867f-94b301026d77 ----";
            std::string cipherText = "";
            //std::cout << "plainText =========================" << std::endl << plainText << std::endl;

            gettimeofday(&tv_start, NULL);
            printf("%18s ", "RsaPublicEncrypt");

            bRet = __rs.RsaPublicEncrypt(plainText, pubKey, cipherText);

            gettimeofday(&tv_now, NULL);
            ez_timeval_subtract(&t_sub_time, &tv_now, &tv_start);
            printf("%10d ",  (int)(t_sub_time.tv_sec*1000 + t_sub_time.tv_usec / 1000.0) );

            if (bRet)
            {
                printf("%s\n", "Success");
            }
            else
            {
                printf("%s\n", "Failed");
                break;
            }

            printf("%8s ", "");

            std::string plainTextGetFromDecrypt ;

            gettimeofday(&tv_start, NULL);
            printf("%18s ", "RsaPrivateDecrypt");

            bRet = __rs.RsaPrivateDecrypt(cipherText, priKey, plainTextGetFromDecrypt);

            gettimeofday(&tv_now, NULL);
            ez_timeval_subtract(&t_sub_time, &tv_now, &tv_start);
            printf("%10d ",  (int)(t_sub_time.tv_sec*1000 + t_sub_time.tv_usec / 1000.0) );

            if (bRet)
            {
                //std::cout << "plainTextGetFromDecrypt =========================" << std::endl << plainTextGetFromDecrypt << std::endl;
                if (plainTextGetFromDecrypt == plainText)
                {
                    //std::cout << "plainTextGetFromDecrypt GOOD" << std::endl;
                    printf("%s\n", "Success");

                }
                else
                {
                    printf("%s\n", "Failed");
                    break;

                }

            }
            else
            {
                printf("%s\n", "Failed");
                break;
            }

        }
    }
    while(1);

    return 0;
}

#endif //#ifdef _FUNC_STANDALONE_UTIL


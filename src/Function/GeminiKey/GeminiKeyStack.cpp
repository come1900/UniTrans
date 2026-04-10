/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * GeminiKeyStack.cpp - _explain_
 *
 * Copyright (C) 2012 ezlibs.com, All Rights Reserved.
 *
 * $Id: GeminiKeyStack.cpp 0001 2012-5-8 14:38:29Z wu_junjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2012-5-8 14:38:33 WuJunjie 549 Create
 *     2012-05-25 10:21:25 WuJunjie 549 all decode interface ready
 * 
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#ifdef LINUX
#include <sys/time.h> /*gettimeofday*/
#endif //LINUX

#ifdef HAVE_LIBUUID
#include <uuid/uuid.h>
#endif

#include <assert.h>
#include "Compression.h"
#include <des.h>

#include "str_opr.h"

#include "GeminiKeyStack.h"

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

const char * const GeminiKey_ENC_PWD = "jwhzgemini";

// json列标题，使用短格式，增加时确保不得重复
const char * const MARK_IDENTITY        = "I"  ;
const char * const MARK_VERSION         = "V"  ;
const char * const MARK_KEY             = "K" ;
const char * const MARK_VERIFYCODE      = "F" ;
const char * const MARK_PID             = "P"  ;
const char * const MARK_TYPE            = "T"  ;
const char * const MARK_KEYSTRING       = "S"  ;
#ifdef USE_TIME_STAMP_IN_KEY    	
const char * const MARK_TIMESTAMP       = "I" ;
const char * const MARK_MEMO            = "M"  ;
#endif
const char * const MARK_FUNSTRING       = "U"  ;

CGeminiKeyStack::CGeminiKeyStack()
{
    m_bDataValid = false;
    m_iError = 0;
    m_strError = "";
}

CGeminiKeyStack::~CGeminiKeyStack()
{}

bool CGeminiKeyStack::Parse( const std::string &document)
{
#if 0
    // 启用严格模式，让非法的json解析时直接返回false，不自动容错。
    // 编译失败， 库？
    Json::Features f = Json::Features::strictMode();
    Json::Reader reader(f);
#else

    Json::Reader reader;
#endif

    bool parsingSuccessful = reader.parse( document, m_JsonValue );

    if ( !parsingSuccessful )
    {
        printf( "Failed to parse %s file: \n%s\n",
                document.c_str(),
                reader.getFormatedErrorMessages().c_str() );

        m_strError = reader.getFormatedErrorMessages();
        m_iError = -1;
    }
    else
    {
        m_strError = "";
        m_iError = 0;
    }

    setValid(parsingSuccessful);
    return parsingSuccessful;
}

bool CGeminiKeyStack::ToString( std::string &document)
{
    if (!m_bDataValid)
    {
        return false;
    }

    //Json::StyledWriter writer(document);
    Json::FastWriter writer(document);
    writer.write( m_JsonValue );

    return true;
}

const std::string &CGeminiKeyStack::ToString()
{
    if (!m_bDataValid)
    {
        m_strDocument = "";
    }
    else
    {
        ToString(m_strDocument);
    }

    return m_strDocument;//generate json string:),here all is done
}

bool CGeminiKeyStack::isValid()
{
    return m_bDataValid;
}

void CGeminiKeyStack::setValid(bool bValid)
{
    m_bDataValid = bValid;
}

const int CGeminiKeyStack::getError() const
{
    return m_iError;
}

const std::string &CGeminiKeyStack::getErrorMessages() const
{
    return m_strError;
}

const Json::Value &CGeminiKeyStack::GetValue() const
{
    return m_JsonValue;
}

/* To get the C-string PEM form: */
int CGeminiKeyStack::dump_PrivateKeyAsPem(RSA *r, char *pBuf)
{
    int keylen;
    char *pem_key;
    BIO *bio = BIO_new(BIO_s_mem());
    PEM_write_bio_RSAPrivateKey(bio, r, NULL, NULL, 0, NULL, NULL);

    keylen = BIO_pending(bio);
    pem_key = (char *)calloc(keylen+1, 1); /* Null-terminate */
    assert(pem_key);
    BIO_read(bio, pem_key, keylen);

    if (pBuf)
    {
        sprintf(pBuf, "%s", pem_key);
    }
    else
    {
        printf("%s", pem_key);
    }
    BIO_free_all(bio);
    free(pem_key);

    return keylen;
}

/* To get the C-string PEM form: */
int CGeminiKeyStack::dump_PublicKeyAsPem(RSA *r, char *pBuf)
{
    int keylen;
    char *pem_key;
    int rv;

    BIO *bio = BIO_new(BIO_s_mem());
    //PEM_write_bio_RSAPrivateKey(bio, r, NULL, NULL, 0, NULL, NULL);

    rv = PEM_write_bio_RSA_PUBKEY(bio,r);
    ARG_USED(rv);

    keylen = BIO_pending(bio);
    pem_key = (char *)calloc(keylen+1, 1); /* Null-terminate */
    BIO_read(bio, pem_key, keylen);

    if (pBuf)
    {
        sprintf(pBuf, "%s", pem_key);
    }
    else
    {
        printf("%s", pem_key);
    }
    BIO_free_all(bio);
    free(pem_key);

    return keylen;
}

int CGeminiKeyStack::geminiRandString(const unsigned char * const pSeed, const int iSeedLen, unsigned char *pOut, const int iOutNum)
{
    // joke?
    if (pOut==NULL || iOutNum<1)
    {
        return -1;
    }

    char pSeedMd5[33] = {0};// md5 str 32
    char TimeNowMd5[33] = {0};// md5 str 32
    char tmpBuf[128] = {0};//

    // single thread, do not add lock here
#ifdef HAVE_LIBUUID

    char uuidBuff[37] = {0};
    uuid_t uuidGenerated;
    uuid_generate_random(uuidGenerated);
    uuid_unparse(uuidGenerated, uuidBuff);


    struct timeval t_now_time;
    gettimeofday(&t_now_time, NULL);
    snprintf(tmpBuf, sizeof(tmpBuf), "%8ld%s%8ld", t_now_time.tv_usec, uuidBuff, t_now_time.tv_sec);
#else

    // md5(time)
    struct timeval t_now_time;
    gettimeofday(&t_now_time, NULL);
    snprintf(tmpBuf, sizeof(tmpBuf), "%ld%ld", t_now_time.tv_usec, t_now_time.tv_sec);
#endif

    ez_md5_calc_hexstr((unsigned char *)TimeNowMd5, (unsigned char *)tmpBuf, strlen(tmpBuf));
    //__fline;
    //printf("TimeNowMd5 str:%s\n", TimeNowMd5);
    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

    // md5(seed)
    if (pSeed!=NULL && iSeedLen>0)
    {
        //const char * const pOrig = "hellostr";

        //memset(pSeedMd5, 0, sizeof(pSeedMd5));
        ez_md5_calc_hexstr((unsigned char *)pSeedMd5, (unsigned char *)pSeed, iSeedLen);
        //__fline;
        //printf("pSeedMd5 str:%s\n", pSeedMd5);
    }
    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
    snprintf(tmpBuf, sizeof(tmpBuf), "%s%s", pSeedMd5, TimeNowMd5);
    memset(pOut, 0, iOutNum);
    ez_md5_calc_8ascii_digit((unsigned char *)pOut, iOutNum, (unsigned char *)tmpBuf, strlen(tmpBuf));
    //printf("tmpBuf str:%s\n", tmpBuf);
    //printf("ez_md5_calc_8ascii_digit\n%s\n", pOut);

    return iOutNum;
}

int CGeminiKeyStack::GeminiKeyCreate(GeminiKey &Lock
                                     , GeminiKey &Unlock
                                     , const std::string &strPid
#ifdef USE_TIME_STAMP_IN_KEY    	
                                     , const std::string &strTimeStamp
                                     , const std::string &strMemo
#endif //USE_TIME_STAMP_IN_KEY
                                     , const int iBits)
{
    RSA *r;
    int bits=iBits,ret;
    unsigned long  e=RSA_3;
    BIGNUM *bne;

    bne=BN_new();
    ret=BN_set_word(bne,e);
    r=RSA_new();
    ret=RSA_generate_key_ex(r,bits,bne,NULL);
    if(ret!=1)
    {
        fprintf(stderr, "Error: RSA_generate_key_ex err!\n");
        BN_clear_free(bne);
        RSA_free(r);
        return -1;
    }

    if (!RSA_check_key(r))
    {
        fprintf(stderr, "Error: Problems while generating RSA Key.\nRetry.\n");
        BN_clear_free(bne);
        RSA_free(r);
        return -2;
    }

    /////////////////////
    int len;

    char *pBuffer = (char *)malloc(iBits);
    if (pBuffer==NULL)
    {
        fprintf(stderr, "Error: Outof memory.\n");
        BN_clear_free(bne);
        RSA_free(r);
        return -3;
    }

#ifdef USE_TIME_STAMP_IN_KEY    	
    char pBuff[32] = {0};
    get_date_time_string(pBuff, "%04d%02d%02d%02d%02d%02d");
    std::string __strTimeStamp = strTimeStamp.size()<14/*20141027185509*/?pBuff:strTimeStamp;
#endif //USE_TIME_STAMP_IN_KEY
    ////////////////////////////////////////////////////
    len = dump_PublicKeyAsPem(r, pBuffer);
    ARG_USED(len);
    DBG(
        __fline;
        printf("dump_PublicKeyAsPem:%d\n", len);
    );
    Lock.strPeerID = strPid;
#ifdef USE_TIME_STAMP_IN_KEY    	
    // begin 2014-11-1 20:43:24
    Lock.strTimeStamp = __strTimeStamp;
    Lock.strMemo = strMemo;
    // end 2014-11-1 20:44:00
#endif// USE_TIME_STAMP_IN_KEY    	
    Lock.strKeyType = GeminiKeyTypeLockKey; //GeminiKey::LockKey;//"L";
    Lock.strKeyString = pBuffer;
    Lock.freshVerifyCode();

    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
    len = dump_PrivateKeyAsPem(r, pBuffer);
    DBG(
        __fline;
        printf("dump_PrivateKeyAsPem:%d\n", len);
    );

    Unlock.strKeyString = pBuffer;
    Unlock.strPeerID = strPid;
#ifdef USE_TIME_STAMP_IN_KEY    	
    Unlock.strTimeStamp = __strTimeStamp;
    Unlock.strMemo = strMemo;
#endif
    Unlock.strKeyType = GeminiKeyTypeUnlockKey ;//GeminiKey::UnlockKey;//"U";
    Unlock.freshVerifyCode();
    DBG(
        __fline;
        printf("Lock.IsValid:%d\n", Lock.IsValid());
        __fline;
        printf("Unlock.IsValid:%d\n", Unlock.IsValid());
    );
    //////////////////////////////////////////////

    BN_clear_free(bne);
    RSA_free(r);
    free(pBuffer);

    return 0;
}

// 上锁, 务必先自行调用一遍 GeminiKey::FreshVerifyCode()， 否则ret -1
// PlainText --返回明文
// LockedData -- 返回加密后数据， base64
// ret 0-ok, -1 -data error -2:rsa enc error failed
int CGeminiKeyStack::GeminiLock(const GeminiKeys &LockKey, std::string &PlainText, std::string &LockedData)
{
    if (!LockKey.isValid())
    {
        return -1;
    }

    unsigned char randOut[PLAN_TEXT_BYTES+1] = {0};
    int randlen = geminiRandString((const unsigned char *)LockKey.m_vecGeminiKey[0].strVerifyCode.c_str()
                                   , LockKey.m_vecGeminiKey[0].strVerifyCode.size()
                                   , randOut
                                   , PLAN_TEXT_BYTES);

    ARG_USED(randlen);
    //    __fline;
    //    printf("geminiRandString(%d):%s\n", randlen, randOut);


    PlainText = (char *)randOut;

    BIO *bpo = BIO_new_mem_buf((char *)LockKey.m_vecGeminiKey[0].strKeyString.c_str(), -1);
    assert(bpo);
    RSA *pubKey = PEM_read_bio_RSA_PUBKEY(bpo, NULL, NULL, NULL);
    ERR_load_crypto_strings();

    if ( !pubKey )
    {
        __trip;
        printf("%s\n", ERR_error_string(ERR_get_error(), NULL));

        BIO_free(bpo);
        //RSA_free(pubKey);
        ERR_free_strings();

        return -1;
    }

    int rsa_length = RSA_size(pubKey);
    StlVecUnChar vecEncrypted;
    vecEncrypted.reserve(rsa_length);
    vecEncrypted.assign(rsa_length, 0);// clear

    //unsigned char retencrypted[25600] = { 0 };

    //int resultEncrypt = RSA_public_encrypt(PADDING, str, encrypted, pubKey, PADDING);
    DBG(
        __fline;
        printf("RSA_public_encrypt(%d) -- [%s]\n", PLAN_TEXT_BYTES, randOut);
    );
    int resultEncrypt = RSA_public_encrypt( PLAN_TEXT_BYTES, (const unsigned char*)randOut, (unsigned char *)&vecEncrypted[0], pubKey, RSA_PKCS1_PADDING ) ;
    //flen(strlen(str)+1 here) must be less than RSA_size(rsa) - 11 for the PKCS #1 v1.5 based padding modes
    if ( resultEncrypt == -1 )
    {
        __trip;
        printf("%s\n", ERR_error_string(ERR_get_error(), NULL));
        BIO_free(bpo);
        RSA_free(pubKey);
        ERR_free_strings();

        return -1;
    }
    DBG(
        __fline;
        std::cout << "resultEncrypt:" << resultEncrypt << std::endl;
    );
    //to base64
    int bufBase64Len = ez_base64encode_len(resultEncrypt); // 返回的长度含结束符
    StlVecUnChar vecBase64;
    vecBase64.reserve(bufBase64Len);
    vecBase64.assign(bufBase64Len, '\0');// clear

    int ibase64Len = ez_base64encode((char *)&vecBase64[0], (const char*)&vecEncrypted[0], resultEncrypt);
    ARG_USED(ibase64Len);
    DBG(
        __fline;
        printf("ez_base64encode:%d: buflen:%d bufBase64Len, %d, ==vecBase64.size():%d\n", ibase64Len, strlen((char *)&vecBase64[0]), bufBase64Len, vecBase64.size());
    );
    LockedData.assign(vecBase64.begin(), vecBase64.end());//

    BIO_free(bpo);
    RSA_free(pubKey);
    ERR_free_strings();

    return 0;
}

//开锁
// 上锁, 务必先自行调用一遍 GeminiKey::FreshVerifyCode()， 否则ret -1
// PlainText --明文
// LockedData -- 密文 base64
// ret 0-ok, -1 -data error -2:rsa enc error failed. -3 - pwd error
int CGeminiKeyStack::GeminiUnlock(const GeminiKeys &UnlockKey
                                  , const std::string &PlainText
                                  , const std::string &LockedData)
{
    if (!UnlockKey.isValid())
    {
        return -1;
    }

    //
    int bufBase64Len = ez_base64decode_len(LockedData.c_str()); // 返回的长度含结束符
    StlVecUnChar vecBase64;
    vecBase64.reserve(bufBase64Len);
    vecBase64.assign(bufBase64Len, '\0');// clear

    ///////////////////
    // restore b64 data
    int restored_encrypted_len=0;

    restored_encrypted_len = ez_base64decode((char *)&vecBase64[0], LockedData.c_str());

    DBG(
        __fline;
        printf("resultEncrypt datalen:%d\n", restored_encrypted_len);
    );
    ///////////////////

    /*
     * Now decrypt this very string with the private key
     */
    BIO *bpop = BIO_new_mem_buf((char *)UnlockKey.m_vecGeminiKey[0].strKeyString.c_str(), -1);
    RSA *privKey = PEM_read_bio_RSAPrivateKey(bpop, NULL, NULL, NULL);

    ERR_load_crypto_strings();

    if ( !privKey )
    {
        printf("%s\n", ERR_error_string(ERR_get_error(), NULL));
        BIO_free(bpop);
        //RSA_free(privKey);
        ERR_free_strings();

        return -1;
    }

    ERR_load_crypto_strings();
    int rsa_length = RSA_size(privKey);
    ARG_USED(rsa_length);

    //buf = malloc(EVP_PKEY_size(pubKey));
    //buf2 = malloc(EVP_PKEY_size(pubKey));

    //unsigned char *decrypted = (unsigned char *)malloc(EVP_PKEY_size(privKey)+1);//EVP_PKEY_size(priv_key)
    // 这里不能按明文定义缓存长度，因为可能有坏数据
    //unsigned char decrypted[PLAN_TEXT_BYTES+1]={0};
    StlVecUnChar vecDecrypted;
    vecDecrypted.reserve(rsa_length);
    vecDecrypted.assign(rsa_length, 0);// clear

    if (1/*decrypted!=NULL*/)
    {
        //memset(decrypted, 0, EVP_PKEY_size(privKey)+1);

        //unsigned char decrypted[2]={0};

        //使用恢复的数据
        int resultDecrypt = RSA_private_decrypt( restored_encrypted_len, &vecBase64[0], &vecDecrypted[0], privKey, RSA_PKCS1_PADDING);

        if ( resultDecrypt == -1 )
        {
#ifdef SHOW_ERROR_MSG
            __trip;
            printf("%s\n", ERR_error_string(ERR_get_error(), NULL));
#endif // SHOW_ERROR_MSG

            BIO_free(bpop);
            RSA_free(privKey);
            ERR_free_strings();

            return -2;
        }

        DBG(
            __fline;
            printf("decrypted string(%d) -- [%s]\n", resultDecrypt, (char *)&vecDecrypted[0]);
        );
    }

    BIO_free(bpop);
    RSA_free(privKey);
    ERR_free_strings();

    // 检查是否配对
    std::string strTmp;
    strTmp.assign(vecDecrypted.begin(), vecDecrypted.end());
    strTmp = (char *)strTmp.c_str();//(char *)&vecDecrypted[0];

    if(PlainText != strTmp)
    {
#ifdef SHOW_ERROR_MSG
        __trip;
        std::cout << PlainText <<"!="<< strTmp << std::endl;
#endif

        return -3;
    }

#if 0
    __fline;
    std::cout << "---------------------------------------------" << std::endl;
    std::cout << PlainText << std::endl;
    std::cout << "---------------------------------------------" << std::endl;
    std::cout << LockedData << std::endl;
    std::cout << "---------------------------------------------" << std::endl;
#endif

    return 0;
}

// 上锁
// PlainText --返回明文
// LockedData -- 返回加密后数据， base64
// ret 0-ok, -1 -data error -2:rsa enc error failed.
int CGeminiKeyStack::GeminiLock(const std::string &strKey, std::string &PlainText, std::string &LockedData)
{
    //bool bret;
    int iret;

    iret = GeminiKeyImport(strKey);
    if (iret<0)
    {
        return -1;
    }

    GeminiKeys __KeysGenLock;
    iret = Decode(__KeysGenLock);
    if (iret != 0)
    {
        return -2;
    }

    if (__KeysGenLock.checkVerifyCode() == false)
    {
        return -3;
    }

    if (__KeysGenLock.m_vecGeminiKey[0].strKeyType != GeminiKeyTypeLockKey)
    {
        return -4;
    }

    return GeminiLock(__KeysGenLock, PlainText, LockedData);
}

int CGeminiKeyStack::GeminiUnlock(const std::string &strKey
                                  , const std::string &PlainText
                                  , const std::string &LockedData)
{
    int iret;

    iret = GeminiKeyImport(strKey);
    if (iret<0)
    {
        return -1;
    }

    GeminiKeys __KeysGenLock;
    iret = Decode(__KeysGenLock);
    if (iret != 0)
    {
        return -2;
    }

    if (__KeysGenLock.checkVerifyCode() == false)
    {
        return -3;
    }

    if (__KeysGenLock.m_vecGeminiKey[0].strKeyType != GeminiKeyTypeUnlockKey)
    {
        return -4;
    }

    return GeminiUnlock(__KeysGenLock, PlainText, LockedData);
}

//// to json
//void Encode(const GeminiKeys                        &Msg);
//// to class
//bool Decode(GeminiKeys                        &Msg);
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
void CGeminiKeyStack::Encode(const GeminiKeys                        &Msg)
{
    Json::Value var;
    unsigned int ii;
    //Msg.dump();

    var[MARK_IDENTITY] = Msg.Identity;
    var[MARK_VERSION] = Msg.Version;
    var[MARK_FUNSTRING] = Msg.strFunString;
    
    Json::Value operate_all;//store all pieces
    for (ii=0; ii<Msg.m_vecGeminiKey.size(); ii++)
    {
        Json::Value operate;//store all pieces

        operate[MARK_VERIFYCODE]     = Msg.m_vecGeminiKey[ii].strVerifyCode       ;
        operate[MARK_PID]  = Msg.m_vecGeminiKey[ii].strPeerID    ;
        operate[MARK_TYPE]     = Msg.m_vecGeminiKey[ii].strKeyType       ;
        operate[MARK_KEYSTRING]  = Msg.m_vecGeminiKey[ii].strKeyString    ;
#ifdef USE_TIME_STAMP_IN_KEY    	
        operate[MARK_TIMESTAMP]  = Msg.m_vecGeminiKey[ii].strTimeStamp    ;
        operate[MARK_MEMO]  = Msg.m_vecGeminiKey[ii].strMemo    ;
#endif
        operate_all.append(operate);
    }
    var[MARK_KEY] = operate_all;

    m_JsonValue = var;
    setValid();
#if 0

    __trip;
    std::cout << std::endl << "------------------------------------" << std::endl;
    std::cout << ToString() << std::endl;
    std::cout << std::endl << "------------------------------------" << std::endl;
#endif
}

int CGeminiKeyStack::Decode(GeminiKeys &Msg)
{
    // 库中默认断言退出，先查看数据是否正常
    if(!(m_JsonValue.type()==Json::objectValue))
    {
        return -1;
    }

    Msg.Identity = m_JsonValue[MARK_IDENTITY].asString();
    Msg.Version = m_JsonValue[MARK_VERSION].asString();
    Msg.strFunString = m_JsonValue[MARK_FUNSTRING].asString();

    const Json::Value arrayOperate = m_JsonValue[MARK_KEY];

    for (unsigned int i = 0; i < arrayOperate.size(); i++)
    {
        GeminiKey _operate;
        _operate.strVerifyCode       = arrayOperate[i][MARK_VERIFYCODE]     .asString();
        _operate.strPeerID    = arrayOperate[i][MARK_PID]  .asString();
        _operate.strKeyType       = arrayOperate[i][MARK_TYPE]     .asString();
        _operate.strKeyString    = arrayOperate[i][MARK_KEYSTRING]  .asString();
#ifdef USE_TIME_STAMP_IN_KEY    	
        _operate.strTimeStamp    = arrayOperate[i][MARK_TIMESTAMP]  .asString();
        _operate.strMemo    = arrayOperate[i][MARK_MEMO]  .asString();
#endif
        // 查下校验码
        if (_operate.checkVerifyCode() )
        {
            Msg.m_vecGeminiKey.push_back(_operate);
        }
    }

    return 0;
}

bool CGeminiKeyStack::GeminiKeyExport(std::string &document)
{
    if (!isValid())
    {
        return false;
    }

    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
    // compress
    StlVecUnChar OriBuff;
    StlVecUnChar CompressedBuff;
    Compression __compress(true, Compression::BEST_COMPRESSION);
    __compress.Init();

    ToString(m_strDocument);
    OriBuff.assign(m_strDocument.begin(), m_strDocument.end());//
    __compress.Deflate(OriBuff, CompressedBuff);
    DBG(
        __fline;
        printf("OriBuff.size():%d, CompressedBuff.size():%d\n", OriBuff.size(), CompressedBuff.size());
    );
    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
    // encrypt
    // gen pwd
    //char buf_DesEncrypt[1024*10];// 加密后的长度为8的整数倍， 大于等于元数据 ((iOrigLen+7)&0xfffffff8);
    StlVecUnChar vecBufDesEncrypt;
    int DesEncryptLen = ((CompressedBuff.size()+7)&0xfffffff8);// DesEncryptLen+=10;//修正一下

    //unsigned char *pBufDesEncrypt = new unsigned char[DesEncryptLen];
    //if (pBufDesEncrypt==NULL)
    //	{return false;}

    vecBufDesEncrypt.reserve(DesEncryptLen);
    vecBufDesEncrypt.assign(DesEncryptLen, '\0');// clear

    // 防止破译
    char pPwd[33] = {0};
    ez_md5_calc_8ascii_digit((unsigned char *)pPwd, sizeof(pPwd)-1, (unsigned char *)GeminiKey_ENC_PWD, strlen(GeminiKey_ENC_PWD));
    DBG(
        __fline;
        printf("enc pwd:%s\n", pPwd);
    );
    //DesEncrypt
    int idataLenDesEncrypt = ez_des_ecb_encrypt(&vecBufDesEncrypt[0], &CompressedBuff[0], CompressedBuff.size(), pPwd, strlen(pPwd));
    // 观察下应该相等
    DBG(
        __fline;
        printf("DesEncrypt idataLenDesEncrypt:%d, == vecBufDesEncrypt.size():%d\n", idataLenDesEncrypt, vecBufDesEncrypt.size());
    );
    //base 64

    int bufBase64Len = ez_base64encode_len(idataLenDesEncrypt); // 返回的长度含结束符
    StlVecUnChar vecBase64;
    vecBase64.reserve(bufBase64Len);
    vecBase64.assign(bufBase64Len, '\0');// clear

    int ibase64Len = ez_base64encode((char *)&vecBase64[0], (char *)&vecBufDesEncrypt[0], idataLenDesEncrypt);
    ARG_USED(ibase64Len);
    DBG(
        __fline;
        printf("ez_base64encode:%d: buflen:%d bufBase64Len, %d, ==vecBase64.size():%d\n", ibase64Len, strlen((char *)&vecBase64[0]), bufBase64Len, vecBase64.size());
    );
    document.assign(vecBase64.begin(), vecBase64.end());//

    //__fline;
    //printf("document.size():%d\n", document.size());

    return true;
}
// 0, OK
// -1, too short -- not a b64string
// -2, bad string -- not a b64string
// -3, decrypt error
// -4, decompress error
// -5, bad string -- not a json style
int CGeminiKeyStack::GeminiKeyImport(const std::string &document)
{
    int iret;

    //de base64
    //char bufBase64[1024*10]={0};
    int document_base64decode_len = ez_base64decode_len((char *)document.c_str());
    if (document_base64decode_len <= 0)
    {
        //__trip;
        return -1;
    }

    StlVecUnChar vecBase64Decode;
    vecBase64Decode.reserve(document_base64decode_len);
    vecBase64Decode.assign(document_base64decode_len, '\0');// clear

    int iret_document_base64decode_len = ez_base64decode((char *)&vecBase64Decode[0], (char *)document.c_str());
    if (iret_document_base64decode_len < 0)
    {
        //__trip;
        return -2;
    }

    DBG(
        __fline;
        printf("iret_document_base64decode_len:%d == document_base64decode_len:%d\n", iret_document_base64decode_len, document_base64decode_len);
    );

    // decrypt
    // des解密缓存
    StlVecUnChar vecBufDesDecrypt;
    vecBufDesDecrypt.reserve(iret_document_base64decode_len);
    vecBufDesDecrypt.assign(iret_document_base64decode_len, '\0');// clear

    char pPwd[33] = {0};
    ez_md5_calc_8ascii_digit((unsigned char *)pPwd, sizeof(pPwd)-1, (unsigned char *)GeminiKey_ENC_PWD, strlen(GeminiKey_ENC_PWD));
    DBG(
        __fline;
        printf("enc pwd:%s\n", pPwd);
    );
    //DesEncrypt
    int idataLenDesDecrypt = ez_des_ecb_decrypt(&vecBufDesDecrypt[0], &vecBase64Decode[0], iret_document_base64decode_len, pPwd, strlen(pPwd));
    ARG_USED(idataLenDesDecrypt);
    if (idataLenDesDecrypt < 0)
    {
        return -3;
    }

    // 观察下应该相等
    DBG(
        __fline;
        printf("DesEncrypt idataLenDesEncrypt:%d, >= vecBufDesEncrypt.size():%d\n", idataLenDesDecrypt, vecBufDesDecrypt.size());
    );
    // decompress
    StlVecUnChar imp_OriBuff;
    //StlVecUnChar CompressedBuff;
    Compression __decompress(false);
    __decompress.Init();

    iret = __decompress.Inflate(vecBufDesDecrypt, imp_OriBuff);
    if (iret != Z_OK)
    {
        __trip;
        return -4;
    }

    DBG(
        __fline;
        printf("OriBuff.size():%d, CompressedBuff.size():%d\n", imp_OriBuff.size(), vecBufDesDecrypt.size());
    );

    std::string strJsonString;
    strJsonString.assign(imp_OriBuff.begin(), imp_OriBuff.end());// to string

    //std::cout << strJsonString << std::endl;
    bool bret = Parse(strJsonString);
    if (false == bret)
    {
        return -5;
    }

    return 0;
}
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/


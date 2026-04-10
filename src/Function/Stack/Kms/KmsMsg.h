/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * KmsMsg.h - _explain_
 *
 * Copyright (C) 2011 jsict.com, All Rights Reserved.
 *
 * $Id: KmsMsg.h 5884 2019-02-27 05:26:28Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2019-02-27 05:26:28  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/


#ifndef _KmsMsg_H
#define _KmsMsg_H

#include <string>
#include <vector>
#include <list>
#include <iostream>

#include "../UniTransRestMsg.h"

//#define _DEV_THIS_WITH_COUT 1

/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

const std::string KmsMsg_Key_KmsID   = "KmsID";
class CKmsMsg_requestMsgGenerateKeypair
{
public:
    std::string m_strKmsID;   // ID

    CKmsMsg_requestMsgGenerateKeypair()
    {
        m_strKmsID   = "";
    }

    void dump()
    {
#ifdef _DEV_THIS_WITH_COUT
        std::cout << KmsMsg_Key_KmsID    << ":" << m_strKmsID   << std::endl;
#endif //_DEV_THIS_WITH_COUT

    }// dump
};

const std::string KmsMsg_Key_PublicKey   = "PublicKey";
const std::string KmsMsg_Key_PrivateKey  = "PrivateKey";

class CKmsMsg_requestMsgGenerateKeypairAck : public CAckMsgSimple
{
public:
    std::string m_strKmsID;   // ID
    std::string m_strPublicKey;
    std::string m_strPrivateKey;

    CKmsMsg_requestMsgGenerateKeypairAck()
    {
        m_strKmsID       = "";
        m_strPublicKey   = "";
        m_strPrivateKey  = "";
    }

    void dump()
    {
#ifdef _DEV_THIS_WITH_COUT
        CAckMsgSimple::dump();

        std::cout << KmsMsg_Key_KmsID      << ":" << m_strKmsID       << std::endl;
        std::cout << KmsMsg_Key_PublicKey  << ":" << m_strPublicKey   << std::endl;
        std::cout << KmsMsg_Key_PrivateKey << ":" << m_strPrivateKey  << std::endl;
#endif //_DEV_THIS_WITH_COUT

    }// dump
};

class CKmsMsg_requestMsgGenerateKey
{
public:
    std::string m_strKmsID;   // ID

    CKmsMsg_requestMsgGenerateKey()
    {
        m_strKmsID   = "";
    }

    void dump()
    {
#ifdef _DEV_THIS_WITH_COUT
        std::cout << KmsMsg_Key_KmsID    << ":" << m_strKmsID   << std::endl;
#endif //_DEV_THIS_WITH_COUT

    }// dump
};

const std::string KmsMsg_Key_Key   = "Key";

class CKmsMsg_requestMsgGenerateKeyAck : public CAckMsgSimple
{
public:
    std::string m_strKmsID;   // ID
    std::string m_strKey;

    CKmsMsg_requestMsgGenerateKeyAck()
    {
        m_strKmsID       = "";
        m_strKey   = "";
    }

    void dump()
    {
#ifdef _DEV_THIS_WITH_COUT
        CAckMsgSimple::dump();

        std::cout << KmsMsg_Key_KmsID      << ":" << m_strKmsID       << std::endl;
        std::cout << KmsMsg_Key_Key  << ":" << m_strKey   << std::endl;
#endif //_DEV_THIS_WITH_COUT

    }// dump
};
const std::string KmsMsg_Key_PlainText  = "PlainText";

class CKmsMsg_requestMsgEncryptAsymmetric
{
public:
    std::string m_strKmsID;   // ID
    std::string m_strPublicKey;
    std::string m_strPlainText;

    CKmsMsg_requestMsgEncryptAsymmetric()
    {
        m_strKmsID       = "";
        m_strPublicKey   = "";
        m_strPlainText  = "";
    }

    void dump()
    {
#ifdef _DEV_THIS_WITH_COUT
        std::cout << KmsMsg_Key_KmsID      << ":" << m_strKmsID       << std::endl;
        std::cout << KmsMsg_Key_PublicKey  << ":" << m_strPublicKey   << std::endl;
        std::cout << KmsMsg_Key_PlainText << ":" << m_strPlainText  << std::endl;
#endif //_DEV_THIS_WITH_COUT

    }// dump
};

const std::string KmsMsg_Key_CipherText   = "CipherText";

class CKmsMsg_requestMsgEncryptAsymmetricAck : public CAckMsgSimple
{
public:
    std::string m_strKmsID;   // ID
    std::string m_strCipherText;

    CKmsMsg_requestMsgEncryptAsymmetricAck()
    {
        m_strKmsID       = "";
        m_strCipherText   = "";
    }

    void dump()
    {
#ifdef _DEV_THIS_WITH_COUT
        CAckMsgSimple::dump();

        std::cout << KmsMsg_Key_KmsID      << ":" << m_strKmsID       << std::endl;
        std::cout << KmsMsg_Key_CipherText  << ":" << m_strCipherText   << std::endl;
#endif //_DEV_THIS_WITH_COUT

    }// dump
};

class CKmsMsg_requestMsgDecryptAsymmetric
{
public:
    std::string m_strKmsID;   // ID
    std::string m_strPrivateKey;
    std::string m_strCipherText;

    CKmsMsg_requestMsgDecryptAsymmetric()
    {
        m_strKmsID       = "";
        m_strPrivateKey   = "";
        m_strCipherText  = "";
    }

    void dump()
    {
#ifdef _DEV_THIS_WITH_COUT
        std::cout << KmsMsg_Key_KmsID      << ":" << m_strKmsID       << std::endl;
        std::cout << KmsMsg_Key_PrivateKey  << ":" << m_strPrivateKey   << std::endl;
        std::cout << KmsMsg_Key_CipherText << ":" << m_strCipherText  << std::endl;
#endif //_DEV_THIS_WITH_COUT

    }// dump
};

class CKmsMsg_requestMsgDecryptAsymmetricAck : public CAckMsgSimple
{
public:
    std::string m_strKmsID;   // ID
    std::string m_strPlainText;

    CKmsMsg_requestMsgDecryptAsymmetricAck()
    {
        m_strKmsID       = "";
        m_strPlainText   = "";
    }

    void dump()
    {
#ifdef _DEV_THIS_WITH_COUT
        CAckMsgSimple::dump();

        std::cout << KmsMsg_Key_KmsID      << ":" << m_strKmsID       << std::endl;
        std::cout << KmsMsg_Key_PlainText  << ":" << m_strPlainText   << std::endl;
#endif //_DEV_THIS_WITH_COUT

    }// dump
};
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#endif /* _KmsMsg_H */

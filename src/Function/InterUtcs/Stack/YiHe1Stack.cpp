/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * YiHe1Stack.cpp - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: YiHe1Stack.cpp 5884 2014-10-12 04:33:45Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2014-10-12 04:33:45  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#include "YiHe1Stack.h"

#include <assert.h>

CYiHe1Stack::CYiHe1Stack()
{
    m_bDataValid = false;
    m_iError = 0;
    m_strError = "";
}

CYiHe1Stack::~CYiHe1Stack()
{}

bool CYiHe1Stack::Parse( const std::string &document)
{
    bool parsingSuccessful = false;
    m_xmlDocument.Parse(document.c_str());

    if (m_xmlDocument.Error())
    {
        parsingSuccessful = false;

        printf( "Failed to parse \n");

        m_strError = "Failed to parse";//reader.getFormatedErrorMessages();
        m_iError = -1;
    }
    else
    {
        parsingSuccessful = true;

        m_strError = "";
        m_iError = 0;
    }

    setValid(parsingSuccessful);
    return parsingSuccessful;
}

bool CYiHe1Stack::ToString( std::string &document)
{
    if (!m_bDataValid)
    {
        return false;
    }

    TiXmlPrinter printer;
    printer.SetIndent( "  " );
    m_xmlDocument.Accept( &printer );

    document = printer.CStr();

    return true;
}

const std::string &CYiHe1Stack::ToString()
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

bool CYiHe1Stack::isValid()
{
    return m_bDataValid;
}

void CYiHe1Stack::setValid(bool bValid)
{
    m_bDataValid = bValid;
}

const int CYiHe1Stack::getError() const
{
    return m_iError;
}

const std::string &CYiHe1Stack::getErrorMessages() const
{
    return m_strError;
}

void CYiHe1Stack::AddIntElement(TiXmlElement &root, const std::string &subTitle, int value)
{
    char array[12] = {0};
    sprintf(array, "%d", value);
    TiXmlText txt(array);
    TiXmlElement subItem(subTitle.c_str());
    subItem.InsertEndChild(txt);
    root.InsertEndChild(subItem);
}

//?|??|?¡ãoot??????????atext,?3¡§¡êatitle|???
void CYiHe1Stack::AddStrElement(TiXmlElement& root, const std::string& subTitle,
                                const std::string& value)
{
    TiXmlText txt(value.c_str());
    TiXmlElement subItem(subTitle.c_str());
    subItem.InsertEndChild(txt);
    root.InsertEndChild(subItem);
}

void  CYiHe1Stack::Encode(const CYH1MsgHeader &Msg)
{}
bool CYiHe1Stack::Decode(CYH1MsgHeader &Msg)
{
    return true;
}

// just example
void CYiHe1Stack::Encode()
{
    m_xmlDocument.Clear();

    TiXmlDeclaration * decl = new TiXmlDeclaration( "1.0", "UTF-8", ""/*standalone*/ );
    m_xmlDocument.LinkEndChild( decl );

    //        if (m_xmlDocument.Error())
    //        {
    //            printf("doc.Error().\n");
    //            iRet = -1;
    //        }
    //        else
    //{
    TiXmlElement item("Message");
    {

        AddStrElement(item,"Version","1.0");
        AddStrElement(item,"Token","d279449b631fd968068732f3d83e9ea1");
        AddStrElement(item,"From","ezlibs");
        AddStrElement(item,"To","faceaip");
        AddStrElement(item,"Type","lock");
        AddStrElement(item,"Seq","1");

        TiXmlElement itemBody("Body");
        {
            TiXmlElement itemOperation("Operation");
            itemOperation.SetAttribute("order", "oorder");
            itemOperation.SetAttribute("name", "getSignalControlerPara");
            {
                TiXmlElement itemObject ("Object");
                itemObject.SetAttribute("name", "TSC");
                itemObject.SetAttribute("sumCount", "1");
                itemOperation.InsertEndChild(itemObject);
            }

            itemBody.InsertEndChild(itemOperation);
        }

        item.InsertEndChild(itemBody);
    }
    m_xmlDocument.InsertEndChild(item);
    //}

    setValid();
}

void CYiHe1Stack::Encode(const CMsgGetSignalControlerParaRequest &Msg)
{
    m_xmlDocument.Clear();

    TiXmlDeclaration * decl = new TiXmlDeclaration( "1.0", "UTF-8", ""/*standalone*/ );
    m_xmlDocument.LinkEndChild( decl );

    //        if (m_xmlDocument.Error())
    //        {
    //            printf("doc.Error().\n");
    //            iRet = -1;
    //        }
    //        else
    //{
    TiXmlElement item("Message");
    {
        AddStrElement(item,"Version"    , Msg.m_yhHeader.m_strVersion);
        AddStrElement(item,"Token"      , Msg.m_yhHeader.m_strToken  );
        AddStrElement(item,"From"       , Msg.m_yhHeader.m_strFrom   );
        AddStrElement(item,"To"         , Msg.m_yhHeader.m_strTo     );
        AddStrElement(item,"Type"       , Msg.m_yhHeader.m_strType   );
        AddStrElement(item,"Seq"        , Msg.m_yhHeader.m_strSeq    );

        TiXmlElement itemBody("Body");
        {
            TiXmlElement itemOperation("Operation");
            itemOperation.SetAttribute("order", Msg.m_yhOprAttr.m_strOrder);
            itemOperation.SetAttribute("name",  Msg.m_yhOprAttr.m_strName );
            {
                TiXmlElement itemObject ("Object");
                itemObject.SetAttribute("name"     , Msg.m_yhObjAttr.m_strName    );
                itemObject.SetAttribute("sumCount" , Msg.m_yhObjAttr.m_strSumCount);
                itemOperation.InsertEndChild(itemObject);
            }

            itemBody.InsertEndChild(itemOperation);
        }

        item.InsertEndChild(itemBody);
    }
    m_xmlDocument.InsertEndChild(item);
    //}

    setValid();
}

bool CYiHe1Stack::Decode(CMsgGetSignalControlerParaRequest &Msg)
{
    if (m_xmlDocument.Error())
    {
        return false;
    }

    TiXmlElement *RootElement = m_xmlDocument.RootElement();

    //TiXmlNode*  itemMessage = RootElement->FirstChild( "Message" );
    TiXmlElement *FirstPerson = RootElement->FirstChildElement();
    //char *pVersion
    //printf("version:%s\n", FirstPerson->FirstChild()->Value());
    Msg.m_yhHeader.m_strVersion = FirstPerson->FirstChild()->Value();

    TiXmlElement *AgeElement = FirstPerson->NextSiblingElement();
    //printf("Token:%s\n", AgeElement->FirstChild()->Value());
    Msg.m_yhHeader.m_strToken = AgeElement->FirstChild()->Value();

    TiXmlElement *From = AgeElement->NextSiblingElement();
    //printf("From:%s\n", From->FirstChild()->Value());
    Msg.m_yhHeader.m_strFrom = From->FirstChild()->Value();

    TiXmlElement *To = From->NextSiblingElement();
    //printf("To:%s\n", To->FirstChild()->Value());
    Msg.m_yhHeader.m_strTo = To->FirstChild()->Value();

    TiXmlElement *Type = To->NextSiblingElement();
    //printf("Type:%s\n", Type->FirstChild()->Value());
    Msg.m_yhHeader.m_strType = Type->FirstChild()->Value();

    TiXmlElement *Seq = Type->NextSiblingElement();
    //printf("%s:%s\n", Seq->FirstChild()->Name(), Seq->FirstChild()->Value());
    Msg.m_yhHeader.m_strSeq = Seq->FirstChild()->Value();

    TiXmlElement *Body = Seq->NextSiblingElement();
    assert(Body);
    TiXmlElement *Operation = Body->FirstChildElement(); // ????? FirstChildElement
    assert(Operation);
    TiXmlAttribute *IDAttributeOperationName = Operation->FirstAttribute();
    assert(IDAttributeOperationName);
    TiXmlAttribute *IDAttributeOperationsumCount = IDAttributeOperationName->Next();
    assert(IDAttributeOperationsumCount);
    Msg.m_yhOprAttr.m_strName = IDAttributeOperationName->Value();
    Msg.m_yhOprAttr.m_strOrder = IDAttributeOperationsumCount->Value();


    TiXmlElement *Object = Operation->FirstChildElement();
    assert(Object);
    TiXmlAttribute *IDAttributeObjectName = Object->FirstAttribute();
    assert(IDAttributeObjectName);
    TiXmlAttribute *IDAttributeObjectsumCount = IDAttributeObjectName->Next();
    assert(IDAttributeObjectsumCount);

    //printf("ObjectName:%s\n", IDAttributeObjectName->Value());
    Msg.m_yhObjAttr.m_strName = IDAttributeObjectName->Value();
    //printf("ObjectsumCount:%s\n", IDAttributeObjectsumCount->Value());
    Msg.m_yhObjAttr.m_strSumCount = IDAttributeObjectsumCount->Value();

    return true;
}

void CYiHe1Stack::Encode(const CMsgGetSignalControlerParaResponse &Msg)
{
    m_xmlDocument.Clear();

    TiXmlDeclaration * decl = new TiXmlDeclaration( "1.0", "UTF-8", ""/*standalone*/ );
    m_xmlDocument.LinkEndChild( decl );

    TiXmlElement item("Message");
    {
        AddStrElement(item,"Version"    , Msg.m_yhHeader.m_strVersion);
        AddStrElement(item,"Token"      , Msg.m_yhHeader.m_strToken  );
        AddStrElement(item,"From"       , Msg.m_yhHeader.m_strFrom   );
        AddStrElement(item,"To"         , Msg.m_yhHeader.m_strTo     );
        AddStrElement(item,"Type"       , Msg.m_yhHeader.m_strType   );
        AddStrElement(item,"Seq"        , Msg.m_yhHeader.m_strSeq    );

        TiXmlElement itemBody("Body");
        {
            TiXmlElement itemOperation("Operation");
            itemOperation.SetAttribute("order", Msg.m_yhOprAttr.m_strOrder);
            itemOperation.SetAttribute("name",  Msg.m_yhOprAttr.m_strName );
            {
                TiXmlElement itemObject ("Object");
                itemObject.SetAttribute("name"     , Msg.m_yhObjAttr.m_strName    );
                itemObject.SetAttribute("sumCount" , Msg.m_yhObjAttr.m_strSumCount);
                {
                    TiXmlElement itemSignalControlerParas("SignalControlerParas");
                    {
                        // will be many
                        for (size_t ii=0; ii<Msg.m_v_yhSignalControlerPara.size(); ii++)
                        {

                            TiXmlElement itemSignalControlerPara("SignalControlerPara");
                            {

                                //AddStrElement(itemSignalControlerPara,"SignalControlerID" , Msg.m_yhSignalControlerPara.m_strSignalControlerID    );
                                //AddStrElement(itemSignalControlerPara,"Supplier"          , Msg.m_yhSignalControlerPara.m_strSupplier    );
                                //AddStrElement(itemSignalControlerPara,"Type"              , Msg.m_yhSignalControlerPara.m_strType    );
                                //AddStrElement(itemSignalControlerPara,"CrossIDList"           , Msg.m_yhSignalControlerPara.m_strCrossIDList    );
                                AddStrElement(itemSignalControlerPara,"SignalControlerID"   , Msg.m_v_yhSignalControlerPara[ii].m_strSignalControlerID    );
                                AddStrElement(itemSignalControlerPara,"Supplier"            , Msg.m_v_yhSignalControlerPara[ii].m_strSupplier    );
                                AddStrElement(itemSignalControlerPara,"Type"                , Msg.m_v_yhSignalControlerPara[ii].m_strType    );
                                AddStrElement(itemSignalControlerPara,"CrossIDList"         , Msg.m_v_yhSignalControlerPara[ii].m_strCrossIDList    );

                                itemSignalControlerParas.InsertEndChild(itemSignalControlerPara);
                            }
                        }
                        itemObject.InsertEndChild(itemSignalControlerParas);
                    }

                    //AddStrElement(item,"Version"  , Msg.m_yhHeader.m_strVersion);
                }

                itemOperation.InsertEndChild(itemObject);
            }

            itemBody.InsertEndChild(itemOperation);
        }

        item.InsertEndChild(itemBody);
    }
    m_xmlDocument.InsertEndChild(item);
    //}

    setValid();
}

bool CYiHe1Stack::Decode(CMsgGetSignalControlerParaResponse &Msg)
{
    if (m_xmlDocument.Error())
    {
        return false;
    }
    TiXmlHandle  docHandle( &m_xmlDocument );

    TiXmlElement *RootElement = m_xmlDocument.RootElement();

    //TiXmlNode*  itemMessage = RootElement->FirstChild( "Message" );
    TiXmlElement *FirstPerson = RootElement->FirstChildElement();
    //char *pVersion
    //printf("version:%s\n", FirstPerson->FirstChild()->Value());
    Msg.m_yhHeader.m_strVersion = FirstPerson->FirstChild()->Value();

    TiXmlElement *AgeElement = FirstPerson->NextSiblingElement();
    //printf("Token:%s\n", AgeElement->FirstChild()->Value());
    Msg.m_yhHeader.m_strToken = AgeElement->FirstChild()->Value();

    TiXmlElement *From = AgeElement->NextSiblingElement();
    //printf("From:%s\n", From->FirstChild()->Value());
    Msg.m_yhHeader.m_strFrom = From->FirstChild()->Value();

    TiXmlElement *To = From->NextSiblingElement();
    //printf("To:%s\n", To->FirstChild()->Value());
    Msg.m_yhHeader.m_strTo = To->FirstChild()->Value();

    TiXmlElement *Type = To->NextSiblingElement();
    //printf("Type:%s\n", Type->FirstChild()->Value());
    Msg.m_yhHeader.m_strType = Type->FirstChild()->Value();

    TiXmlElement *Seq = Type->NextSiblingElement();
    //printf("Seq:%s\n", Seq->FirstChild()->Value());
    Msg.m_yhHeader.m_strSeq = Seq->FirstChild()->Value();

    TiXmlElement* Operation = docHandle.FirstChild( "Message" ).FirstChild( "Body" ).FirstChild( "Operation" ).ToElement();
    assert(Operation);
    TiXmlAttribute *IDAttributeOperationName = Operation->FirstAttribute();
    assert(IDAttributeOperationName);
    TiXmlAttribute *IDAttributeOperationsumCount = IDAttributeOperationName->Next();
    assert(IDAttributeOperationsumCount);
    Msg.m_yhOprAttr.m_strName = IDAttributeOperationName->Value();
    Msg.m_yhOprAttr.m_strOrder = IDAttributeOperationsumCount->Value();


    //    TiXmlElement *Object = Operation->FirstChildElement();
    TiXmlElement* Object = docHandle.FirstChild( "Message" ).FirstChild( "Body" ).FirstChild( "Operation" ).FirstChild( "Object" ).ToElement();
    assert(Object);
    TiXmlAttribute *IDAttributeObjectName = Object->FirstAttribute();
    assert(IDAttributeObjectName);
    TiXmlAttribute *IDAttributeObjectsumCount = IDAttributeObjectName->Next();
    assert(IDAttributeObjectsumCount);
    //printf("ObjectName:%s\n", IDAttributeObjectName->Value());
    Msg.m_yhObjAttr.m_strName = IDAttributeObjectName->Value();
    //printf("ObjectsumCount:%s\n", IDAttributeObjectsumCount->Value());
    Msg.m_yhObjAttr.m_strSumCount = IDAttributeObjectsumCount->Value();

    TiXmlElement* SignalControlerPara = docHandle \
                                        .FirstChild( "Message" ) \
                                        .FirstChild( "Body" ) \
                                        .FirstChild( "Operation" ) \
                                        .FirstChild( "Object" ) \
                                        .FirstChild( "SignalControlerParas" ) \
                                        .FirstChild( "SignalControlerPara" ).ToElement();
    assert(SignalControlerPara);
    while ( SignalControlerPara )
    {
        CYH1MsgSignalControlerPara m_yhSignalControlerPara;

        TiXmlElement *SignalControlerID = SignalControlerPara->FirstChildElement();
        assert(SignalControlerID);
        //char *pVersion
        //printf("SignalControlerID:%s\n", SignalControlerID->FirstChild()->Value());
        m_yhSignalControlerPara.m_strSignalControlerID = SignalControlerID->FirstChild()->Value();

        TiXmlElement *Supplier = SignalControlerID->NextSiblingElement();
        assert(Supplier);
        //printf("Supplier:%s\n", Supplier->FirstChild()->Value());
        m_yhSignalControlerPara.m_strSupplier = Supplier->FirstChild()->Value();

        TiXmlElement *SignalControlerParaType = Supplier->NextSiblingElement();
        assert(SignalControlerParaType);
        //printf("Type:%s\n", SignalControlerParaType->FirstChild()->Value());
        m_yhSignalControlerPara.m_strType = SignalControlerParaType->FirstChild()->Value();

        TiXmlElement *CrossIDList = SignalControlerParaType->NextSiblingElement();
        assert(CrossIDList);
        //printf("CrossIDList:%s\n", CrossIDList->FirstChild()->Value());
        m_yhSignalControlerPara.m_strCrossIDList = CrossIDList->FirstChild()->Value();

        Msg.m_v_yhSignalControlerPara.push_back(m_yhSignalControlerPara);

        SignalControlerPara = SignalControlerPara->NextSiblingElement();
    }
    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

    return true;
}

/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
//2014-10-20 16:32:46
void CYiHe1Stack::Encode(const CMsgGetCrossParaRequest &Msg)
{
    m_xmlDocument.Clear();

    TiXmlDeclaration * decl = new TiXmlDeclaration( "1.0", "UTF-8", ""/*standalone*/ );
    m_xmlDocument.LinkEndChild( decl );

    //        if (m_xmlDocument.Error())
    //        {
    //            printf("doc.Error().\n");
    //            iRet = -1;
    //        }
    //        else
    //{
    TiXmlElement item("Message");
    {
        AddStrElement(item,"Version"    , Msg.m_yhHeader.m_strVersion);
        AddStrElement(item,"Token"      , Msg.m_yhHeader.m_strToken  );
        AddStrElement(item,"From"       , Msg.m_yhHeader.m_strFrom   );
        AddStrElement(item,"To"         , Msg.m_yhHeader.m_strTo     );
        AddStrElement(item,"Type"       , Msg.m_yhHeader.m_strType   );
        AddStrElement(item,"Seq"        , Msg.m_yhHeader.m_strSeq    );

        TiXmlElement itemBody("Body");
        {
            TiXmlElement itemOperation("Operation");
            itemOperation.SetAttribute("order", Msg.m_yhOprAttr.m_strOrder);
            itemOperation.SetAttribute("name",  Msg.m_yhOprAttr.m_strName );
            {
                TiXmlElement itemObject ("Object");
                itemObject.SetAttribute("name"     , Msg.m_yhObjAttr.m_strName    );
                itemObject.SetAttribute("sumCount" , Msg.m_yhObjAttr.m_strSumCount);

                AddStrElement(itemObject,"SignalControlerID"        , Msg.m_strSignalControlerID );

                itemOperation.InsertEndChild(itemObject);
            }

            itemBody.InsertEndChild(itemOperation);
        }

        item.InsertEndChild(itemBody);
    }
    m_xmlDocument.InsertEndChild(item);
    //}

    setValid();
}

bool CYiHe1Stack::Decode(CMsgGetCrossParaRequest &Msg)
{
    if (m_xmlDocument.Error())
    {
        return false;
    }

    TiXmlElement *RootElement = m_xmlDocument.RootElement();

    //TiXmlNode*  itemMessage = RootElement->FirstChild( "Message" );
    TiXmlElement *FirstPerson = RootElement->FirstChildElement();
    //char *pVersion
    //printf("version:%s\n", FirstPerson->FirstChild()->Value());
    Msg.m_yhHeader.m_strVersion = FirstPerson->FirstChild()->Value();

    TiXmlElement *AgeElement = FirstPerson->NextSiblingElement();
    //printf("Token:%s\n", AgeElement->FirstChild()->Value());
    Msg.m_yhHeader.m_strToken = AgeElement->FirstChild()->Value();

    TiXmlElement *From = AgeElement->NextSiblingElement();
    //printf("From:%s\n", From->FirstChild()->Value());
    Msg.m_yhHeader.m_strFrom = From->FirstChild()->Value();

    TiXmlElement *To = From->NextSiblingElement();
    //printf("To:%s\n", To->FirstChild()->Value());
    Msg.m_yhHeader.m_strTo = To->FirstChild()->Value();

    TiXmlElement *Type = To->NextSiblingElement();
    //printf("Type:%s\n", Type->FirstChild()->Value());
    Msg.m_yhHeader.m_strType = Type->FirstChild()->Value();

    TiXmlElement *Seq = Type->NextSiblingElement();
    //printf("%s:%s\n", Seq->FirstChild()->Name(), Seq->FirstChild()->Value());
    Msg.m_yhHeader.m_strSeq = Seq->FirstChild()->Value();

    TiXmlElement *Body = Seq->NextSiblingElement();
    assert(Body);
    TiXmlElement *Operation = Body->FirstChildElement(); // ????? FirstChildElement
    assert(Operation);
    TiXmlAttribute *IDAttributeOperationName = Operation->FirstAttribute();
    assert(IDAttributeOperationName);
    TiXmlAttribute *IDAttributeOperationsumCount = IDAttributeOperationName->Next();
    assert(IDAttributeOperationsumCount);
    Msg.m_yhOprAttr.m_strName = IDAttributeOperationName->Value();
    Msg.m_yhOprAttr.m_strOrder = IDAttributeOperationsumCount->Value();


    TiXmlElement *FirstObject = Operation->FirstChildElement();
    assert(FirstObject);
    TiXmlAttribute *IDAttributeObjectName = FirstObject->FirstAttribute();
    assert(IDAttributeObjectName);
    TiXmlAttribute *IDAttributeObjectsumCount = IDAttributeObjectName->Next();
    assert(IDAttributeObjectsumCount);

    //printf("ObjectName:%s\n", IDAttributeObjectName->Value());
    Msg.m_yhObjAttr.m_strName = IDAttributeObjectName->Value();
    //printf("ObjectsumCount:%s\n", IDAttributeObjectsumCount->Value());
    Msg.m_yhObjAttr.m_strSumCount = IDAttributeObjectsumCount->Value();

    TiXmlElement *FirstControler = FirstObject->FirstChildElement();
    assert(FirstControler);
    Msg.m_strSignalControlerID = FirstControler->FirstChild()->Value();

    return true;
}

void CYiHe1Stack::Encode(const CMsgGetCrossParaResponse &Msg)
{
    m_xmlDocument.Clear();

    TiXmlDeclaration * decl = new TiXmlDeclaration( "1.0", "UTF-8", ""/*standalone*/ );
    m_xmlDocument.LinkEndChild( decl );

    TiXmlElement item("Message");
    {
        AddStrElement(item,"Version"    , Msg.m_yhHeader.m_strVersion);
        AddStrElement(item,"Token"      , Msg.m_yhHeader.m_strToken  );
        AddStrElement(item,"From"       , Msg.m_yhHeader.m_strFrom   );
        AddStrElement(item,"To"         , Msg.m_yhHeader.m_strTo     );
        AddStrElement(item,"Type"       , Msg.m_yhHeader.m_strType   );
        AddStrElement(item,"Seq"        , Msg.m_yhHeader.m_strSeq    );

        TiXmlElement itemBody("Body");
        {
            TiXmlElement itemOperation("Operation");
            itemOperation.SetAttribute("order", Msg.m_yhOprAttr.m_strOrder);
            itemOperation.SetAttribute("name",  Msg.m_yhOprAttr.m_strName );
            {
                TiXmlElement itemObject ("Object");
                itemObject.SetAttribute("name"     , Msg.m_yhObjAttr.m_strName    );
                itemObject.SetAttribute("sumCount" , Msg.m_yhObjAttr.m_strSumCount);
                AddStrElement(itemObject,"supplier"        , Msg.m_strSupplier);
                {
                    TiXmlElement itemSignalControlerParas("CrossParas");
                    {
                        AddStrElement(itemSignalControlerParas,"SignalControlerID"          , Msg.m_strSignalControlerID     );

                        // will be many
                        for (size_t ii=0; ii<Msg.m_v_yhCrossPara.size(); ii++)
                        {

                            TiXmlElement itemSignalControlerPara("CrossPara");
                            {

                                AddStrElement(itemSignalControlerPara,"CrossID"   , Msg.m_v_yhCrossPara[ii].m_strCrossID    );
                                AddStrElement(itemSignalControlerPara,"CrossName" , Msg.m_v_yhCrossPara[ii].m_strCrossName  );
                                AddStrElement(itemSignalControlerPara,"Feature"   , Msg.m_v_yhCrossPara[ii].m_strFeature    );
                                AddStrElement(itemSignalControlerPara,"IsKey"     , Msg.m_v_yhCrossPara[ii].m_strIsKey      );

                                itemSignalControlerParas.InsertEndChild(itemSignalControlerPara);
                            }
                        }
                        itemObject.InsertEndChild(itemSignalControlerParas);
                    }

                    //AddStrElement(item,"Version"  , Msg.m_yhHeader.m_strVersion);
                }

                itemOperation.InsertEndChild(itemObject);
            }

            itemBody.InsertEndChild(itemOperation);
        }

        item.InsertEndChild(itemBody);
    }
    m_xmlDocument.InsertEndChild(item);
    //}

    setValid();
}

bool CYiHe1Stack::Decode(CMsgGetCrossParaResponse &Msg)
{
    if (m_xmlDocument.Error())
    {
        return false;
    }
    TiXmlHandle  docHandle( &m_xmlDocument );

    TiXmlElement *RootElement = m_xmlDocument.RootElement();

    //TiXmlNode*  itemMessage = RootElement->FirstChild( "Message" );
    TiXmlElement *FirstPerson = RootElement->FirstChildElement();
    //char *pVersion
    //printf("version:%s\n", FirstPerson->FirstChild()->Value());
    Msg.m_yhHeader.m_strVersion = FirstPerson->FirstChild()->Value();

    TiXmlElement *AgeElement = FirstPerson->NextSiblingElement();
    //printf("Token:%s\n", AgeElement->FirstChild()->Value());
    Msg.m_yhHeader.m_strToken = AgeElement->FirstChild()->Value();

    TiXmlElement *From = AgeElement->NextSiblingElement();
    //printf("From:%s\n", From->FirstChild()->Value());
    Msg.m_yhHeader.m_strFrom = From->FirstChild()->Value();

    TiXmlElement *To = From->NextSiblingElement();
    //printf("To:%s\n", To->FirstChild()->Value());
    Msg.m_yhHeader.m_strTo = To->FirstChild()->Value();

    TiXmlElement *Type = To->NextSiblingElement();
    //printf("Type:%s\n", Type->FirstChild()->Value());
    Msg.m_yhHeader.m_strType = Type->FirstChild()->Value();

    TiXmlElement *Seq = Type->NextSiblingElement();
    //printf("Seq:%s\n", Seq->FirstChild()->Value());
    Msg.m_yhHeader.m_strSeq = Seq->FirstChild()->Value();

    TiXmlElement* Operation = docHandle.FirstChild( "Message" ).FirstChild( "Body" ).FirstChild( "Operation" ).ToElement();
    assert(Operation);
    TiXmlAttribute *IDAttributeOperationName = Operation->FirstAttribute();
    assert(IDAttributeOperationName);
    TiXmlAttribute *IDAttributeOperationsumCount = IDAttributeOperationName->Next();
    assert(IDAttributeOperationsumCount);
    Msg.m_yhOprAttr.m_strName = IDAttributeOperationName->Value();
    Msg.m_yhOprAttr.m_strOrder = IDAttributeOperationsumCount->Value();


    //    TiXmlElement *Object = Operation->FirstChildElement();
    TiXmlElement* Object = docHandle.FirstChild( "Message" ).FirstChild( "Body" ).FirstChild( "Operation" ).FirstChild( "Object" ).ToElement();
    assert(Object);
    TiXmlAttribute *IDAttributeObjectName = Object->FirstAttribute();
    assert(IDAttributeObjectName);
    TiXmlAttribute *IDAttributeObjectsumCount = IDAttributeObjectName->Next();
    assert(IDAttributeObjectsumCount);
    //printf("ObjectName:%s\n", IDAttributeObjectName->Value());
    Msg.m_yhObjAttr.m_strName = IDAttributeObjectName->Value();
    //printf("ObjectsumCount:%s\n", IDAttributeObjectsumCount->Value());
    Msg.m_yhObjAttr.m_strSumCount = IDAttributeObjectsumCount->Value();

    TiXmlElement* SignalControlerParas = docHandle \
                                        .FirstChild( "Message" ) \
                                        .FirstChild( "Body" ) \
                                        .FirstChild( "Operation" ) \
                                        .FirstChild( "Object" ) \
                                        .FirstChild( "CrossParas" ).ToElement();
        TiXmlElement *__SignalControlerID = SignalControlerParas->FirstChildElement();
        assert(__SignalControlerID);
        Msg.m_strSignalControlerID = __SignalControlerID->FirstChild()->Value();

    TiXmlElement* SignalControlerPara = docHandle \
                                        .FirstChild( "Message" ) \
                                        .FirstChild( "Body" ) \
                                        .FirstChild( "Operation" ) \
                                        .FirstChild( "Object" ) \
                                        .FirstChild( "CrossParas" ) \
                                        .FirstChild( "CrossPara" ).ToElement();
    assert(SignalControlerPara);
    while ( SignalControlerPara )
    {
        CYH1MsgCrossPara m_yhSignalControlerPara;

        TiXmlElement *SignalControlerID = SignalControlerPara->FirstChildElement();
        assert(SignalControlerID);
        //char *pVersion
        //printf("SignalControlerID:%s\n", SignalControlerID->FirstChild()->Value());
        m_yhSignalControlerPara.m_strCrossID = SignalControlerID->FirstChild()->Value();

        TiXmlElement *Supplier = SignalControlerID->NextSiblingElement();
        assert(Supplier);
        //printf("Supplier:%s\n", Supplier->FirstChild()->Value());
        m_yhSignalControlerPara.m_strCrossName = Supplier->FirstChild()->Value();

        TiXmlElement *SignalControlerParaType = Supplier->NextSiblingElement();
        assert(SignalControlerParaType);
        //printf("Type:%s\n", SignalControlerParaType->FirstChild()->Value());
        m_yhSignalControlerPara.m_strFeature = SignalControlerParaType->FirstChild()->Value();

        TiXmlElement *CrossIDList = SignalControlerParaType->NextSiblingElement();
        assert(CrossIDList);
        //printf("CrossIDList:%s\n", CrossIDList->FirstChild()->Value());
        m_yhSignalControlerPara.m_strIsKey = CrossIDList->FirstChild()->Value();

        Msg.m_v_yhCrossPara.push_back(m_yhSignalControlerPara);

        SignalControlerPara = SignalControlerPara->NextSiblingElement();
    }
    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

    return true;
}

/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
//2014-10-23 16:16:41
void CYiHe1Stack::Encode(const CMsgGetCrossStatusRequest &Msg)
{
    m_xmlDocument.Clear();

    TiXmlDeclaration * decl = new TiXmlDeclaration( "1.0", "UTF-8", ""/*standalone*/ );
    m_xmlDocument.LinkEndChild( decl );

    TiXmlElement item("Message");
    {
        AddStrElement(item,"Version"    , Msg.m_yhHeader.m_strVersion);
        AddStrElement(item,"Token"      , Msg.m_yhHeader.m_strToken  );
        AddStrElement(item,"From"       , Msg.m_yhHeader.m_strFrom   );
        AddStrElement(item,"To"         , Msg.m_yhHeader.m_strTo     );
        AddStrElement(item,"Type"       , Msg.m_yhHeader.m_strType   );
        AddStrElement(item,"Seq"        , Msg.m_yhHeader.m_strSeq    );

        TiXmlElement itemBody("Body");
        {
            TiXmlElement itemOperation("Operation");
            itemOperation.SetAttribute("order", Msg.m_yhOprAttr.m_strOrder);
            itemOperation.SetAttribute("name",  Msg.m_yhOprAttr.m_strName );
            {
                TiXmlElement itemObject ("Object");
                itemObject.SetAttribute("name"     , Msg.m_yhObjAttr.m_strName    );
                itemObject.SetAttribute("sumCount" , Msg.m_yhObjAttr.m_strSumCount);

                AddStrElement(itemObject,"CrossId"        , Msg.m_strCrossId );

                itemOperation.InsertEndChild(itemObject);
            }

            itemBody.InsertEndChild(itemOperation);
        }

        item.InsertEndChild(itemBody);
    }
    m_xmlDocument.InsertEndChild(item);
    //}

    setValid();
}

bool CYiHe1Stack::Decode(CMsgGetCrossStatusRequest &Msg)
{
    if (m_xmlDocument.Error())
    {
        return false;
    }

    TiXmlElement *RootElement = m_xmlDocument.RootElement();

    //TiXmlNode*  itemMessage = RootElement->FirstChild( "Message" );
    TiXmlElement *FirstPerson = RootElement->FirstChildElement();
    //char *pVersion
    //printf("version:%s\n", FirstPerson->FirstChild()->Value());
    Msg.m_yhHeader.m_strVersion = FirstPerson->FirstChild()->Value();

    TiXmlElement *AgeElement = FirstPerson->NextSiblingElement();
    //printf("Token:%s\n", AgeElement->FirstChild()->Value());
    Msg.m_yhHeader.m_strToken = AgeElement->FirstChild()->Value();

    TiXmlElement *From = AgeElement->NextSiblingElement();
    //printf("From:%s\n", From->FirstChild()->Value());
    Msg.m_yhHeader.m_strFrom = From->FirstChild()->Value();

    TiXmlElement *To = From->NextSiblingElement();
    //printf("To:%s\n", To->FirstChild()->Value());
    Msg.m_yhHeader.m_strTo = To->FirstChild()->Value();

    TiXmlElement *Type = To->NextSiblingElement();
    //printf("Type:%s\n", Type->FirstChild()->Value());
    Msg.m_yhHeader.m_strType = Type->FirstChild()->Value();

    TiXmlElement *Seq = Type->NextSiblingElement();
    //printf("%s:%s\n", Seq->FirstChild()->Name(), Seq->FirstChild()->Value());
    Msg.m_yhHeader.m_strSeq = Seq->FirstChild()->Value();

    TiXmlElement *Body = Seq->NextSiblingElement();
    assert(Body);
    TiXmlElement *Operation = Body->FirstChildElement(); // ????? FirstChildElement
    assert(Operation);
    TiXmlAttribute *IDAttributeOperationName = Operation->FirstAttribute();
    assert(IDAttributeOperationName);
    TiXmlAttribute *IDAttributeOperationsumCount = IDAttributeOperationName->Next();
    assert(IDAttributeOperationsumCount);
    Msg.m_yhOprAttr.m_strName = IDAttributeOperationName->Value();
    Msg.m_yhOprAttr.m_strOrder = IDAttributeOperationsumCount->Value();


    TiXmlElement *FirstObject = Operation->FirstChildElement();
    assert(FirstObject);
    TiXmlAttribute *IDAttributeObjectName = FirstObject->FirstAttribute();
    assert(IDAttributeObjectName);
    TiXmlAttribute *IDAttributeObjectsumCount = IDAttributeObjectName->Next();
    assert(IDAttributeObjectsumCount);

    //printf("ObjectName:%s\n", IDAttributeObjectName->Value());
    Msg.m_yhObjAttr.m_strName = IDAttributeObjectName->Value();
    //printf("ObjectsumCount:%s\n", IDAttributeObjectsumCount->Value());
    Msg.m_yhObjAttr.m_strSumCount = IDAttributeObjectsumCount->Value();

    TiXmlElement *FirstControler = FirstObject->FirstChildElement();
    assert(FirstControler);
    Msg.m_strCrossId = FirstControler->FirstChild()->Value();

    return true;
}

void CYiHe1Stack::Encode(const CMsgGetCrossStatusResponse &Msg)
{
    m_xmlDocument.Clear();

    TiXmlDeclaration * decl = new TiXmlDeclaration( "1.0", "UTF-8", ""/*standalone*/ );
    m_xmlDocument.LinkEndChild( decl );

    TiXmlElement item("Message");
    {
        AddStrElement(item,"Version"    , Msg.m_yhHeader.m_strVersion);
        AddStrElement(item,"Token"      , Msg.m_yhHeader.m_strToken  );
        AddStrElement(item,"From"       , Msg.m_yhHeader.m_strFrom   );
        AddStrElement(item,"To"         , Msg.m_yhHeader.m_strTo     );
        AddStrElement(item,"Type"       , Msg.m_yhHeader.m_strType   );
        AddStrElement(item,"Seq"        , Msg.m_yhHeader.m_strSeq    );

        TiXmlElement itemBody("Body");
        {
            TiXmlElement itemOperation("Operation");
            itemOperation.SetAttribute("order", Msg.m_yhOprAttr.m_strOrder);
            itemOperation.SetAttribute("name",  Msg.m_yhOprAttr.m_strName );
            {
                TiXmlElement itemObject ("Object");
                itemObject.SetAttribute("name"     , Msg.m_yhObjAttr.m_strName    );
                itemObject.SetAttribute("sumCount" , Msg.m_yhObjAttr.m_strSumCount);
                {
//                    TiXmlElement itemSignalControlerParas("CrossParas");
//                    {
//                        // will be many
//                        for (size_t ii=0; ii<Msg.m_v_yhCrossStatus.size(); ii++)
//                        {

                            TiXmlElement itemSignalControlerPara("CrossState");
                            {

                                AddStrElement(itemSignalControlerPara,"CrossID"   , Msg.m_v_yhCrossStatus.m_strCrossID    );
                                AddStrElement(itemSignalControlerPara,"Value" , Msg.m_v_yhCrossStatus.m_strValue  );

                                //itemSignalControlerParas.InsertEndChild(itemSignalControlerPara);
                            }
//                        }
                        itemObject.InsertEndChild(itemSignalControlerPara);
//                    }

                }

                itemOperation.InsertEndChild(itemObject);
            }

            itemBody.InsertEndChild(itemOperation);
        }

        item.InsertEndChild(itemBody);
    }
    m_xmlDocument.InsertEndChild(item);
    //}

    setValid();
}

bool CYiHe1Stack::Decode(CMsgGetCrossStatusResponse &Msg)
{
    if (m_xmlDocument.Error())
    {
        return false;
    }
    TiXmlHandle  docHandle( &m_xmlDocument );

    TiXmlElement *RootElement = m_xmlDocument.RootElement();

    //TiXmlNode*  itemMessage = RootElement->FirstChild( "Message" );
    TiXmlElement *FirstPerson = RootElement->FirstChildElement();
    //char *pVersion
    //printf("version:%s\n", FirstPerson->FirstChild()->Value());
    Msg.m_yhHeader.m_strVersion = FirstPerson->FirstChild()->Value();

    TiXmlElement *AgeElement = FirstPerson->NextSiblingElement();
    //printf("Token:%s\n", AgeElement->FirstChild()->Value());
    Msg.m_yhHeader.m_strToken = AgeElement->FirstChild()->Value();

    TiXmlElement *From = AgeElement->NextSiblingElement();
    //printf("From:%s\n", From->FirstChild()->Value());
    Msg.m_yhHeader.m_strFrom = From->FirstChild()->Value();

    TiXmlElement *To = From->NextSiblingElement();
    //printf("To:%s\n", To->FirstChild()->Value());
    Msg.m_yhHeader.m_strTo = To->FirstChild()->Value();

    TiXmlElement *Type = To->NextSiblingElement();
    //printf("Type:%s\n", Type->FirstChild()->Value());
    Msg.m_yhHeader.m_strType = Type->FirstChild()->Value();

    TiXmlElement *Seq = Type->NextSiblingElement();
    //printf("Seq:%s\n", Seq->FirstChild()->Value());
    Msg.m_yhHeader.m_strSeq = Seq->FirstChild()->Value();

    TiXmlElement* Operation = docHandle.FirstChild( "Message" ).FirstChild( "Body" ).FirstChild( "Operation" ).ToElement();
    assert(Operation);
    TiXmlAttribute *IDAttributeOperationName = Operation->FirstAttribute();
    assert(IDAttributeOperationName);
    TiXmlAttribute *IDAttributeOperationsumCount = IDAttributeOperationName->Next();
    assert(IDAttributeOperationsumCount);
    Msg.m_yhOprAttr.m_strName = IDAttributeOperationName->Value();
    Msg.m_yhOprAttr.m_strOrder = IDAttributeOperationsumCount->Value();


    //    TiXmlElement *Object = Operation->FirstChildElement();
    TiXmlElement* Object = docHandle.FirstChild( "Message" ).FirstChild( "Body" ).FirstChild( "Operation" ).FirstChild( "Object" ).ToElement();
    assert(Object);
    TiXmlAttribute *IDAttributeObjectName = Object->FirstAttribute();
    assert(IDAttributeObjectName);
    TiXmlAttribute *IDAttributeObjectsumCount = IDAttributeObjectName->Next();
    assert(IDAttributeObjectsumCount);
    //printf("ObjectName:%s\n", IDAttributeObjectName->Value());
    Msg.m_yhObjAttr.m_strName = IDAttributeObjectName->Value();
    //printf("ObjectsumCount:%s\n", IDAttributeObjectsumCount->Value());
    Msg.m_yhObjAttr.m_strSumCount = IDAttributeObjectsumCount->Value();

    TiXmlElement* SignalControlerPara = docHandle \
                                        .FirstChild( "Message" ) \
                                        .FirstChild( "Body" ) \
                                        .FirstChild( "Operation" ) \
                                        .FirstChild( "Object" ) \
                                        .FirstChild( "CrossState" ).ToElement();
    assert(SignalControlerPara);
//    while ( SignalControlerPara )
//    {
        //CYH1MsgCrossStatus m_yhSignalControlerPara;

        TiXmlElement *SignalControlerID = SignalControlerPara->FirstChildElement();
        assert(SignalControlerID);
        //char *pVersion
        //printf("SignalControlerID:%s\n", SignalControlerID->FirstChild()->Value());
        Msg.m_v_yhCrossStatus.m_strCrossID = SignalControlerID->FirstChild()->Value();

        TiXmlElement *Supplier = SignalControlerID->NextSiblingElement();
        assert(Supplier);
        Msg.m_v_yhCrossStatus.m_strValue = Supplier->FirstChild()->Value();


        //Msg.m_v_yhCrossStatus.push_back(m_yhSignalControlerPara);

//        SignalControlerPara = SignalControlerPara->NextSiblingElement();
//    }
    /*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

    return true;
}

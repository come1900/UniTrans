/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * FileNode.cpp - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: FileNode.cpp 5884 2018-03-19 02:49:09Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2018-03-19 02:49:09  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
#include <stdio.h>
#include <stdlib.h>

#include "FileNode.h"

CFileNode::CFileNode()
{
    strName = "";
    strPath = "";
    mTime = 0;
    iSize = -1;

}

CFileNode::~CFileNode()
{}

bool CFileNode::operator==(const CFileNode& obj) const
{
    return this->strName==obj.strName \
           && this->strName == obj.strName \
           && this->strPath == obj.strPath \
           && this->mTime   == obj.mTime \
           && this->iSize   == obj.iSize  ;
}

bool CFileNode::operator==(const std::string & strName) const
{
    return this->strName==strName;
}

CFileNode & CFileNode::operator=(const CFileNode &tt)
{
    strName  = tt.strName     ;
    strPath  = tt.strPath     ;
    mTime    = tt.mTime       ;
    iSize    = tt.iSize       ;

    return *this;
}

bool CFileNode::isValid() const
{
    return strName.length()>0 \
           && strPath.length()>0 \
           && mTime>0 \
           && iSize>0 \
           ;
}


void CFileNode::dump(int mode)
{
    if (mode==0)
    {
        printf("%38s %10s, %10s\n"
               , "Name"
               , "mTime"
               , "iSize"
              );
    }
    else
    {
        //std::cout << strName << std::endl;
        printf("%38s %10lu, %10lu\n"
               , strName.c_str()
               , mTime
               , iSize
              );
    }
}

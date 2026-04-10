/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * FileNode.h - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: FileNode.h 5884 2018-03-19 02:49:05Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2018-03-19 02:49:05  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
#include <string>
#include <list>

#if !defined(_FileNode_H)
#define _FileNode_H

class CFileNode// : public CEZObject
{
public:

    CFileNode();
    virtual ~CFileNode();

public:
    std::string strName;// 名字 名字为索引
    std::string strPath;// 全路径
    time_t      mTime;  /* time of last modification */
    size_t      iSize;  /* total size, in bytes */


    bool operator==(const CFileNode& obj) const;
    bool operator==(const std::string & strName) const;
    CFileNode & operator=(const CFileNode &tt);

    void dump(int mode=1);

    //
    bool isValid() const;

private:

};


#endif // !defined(_FileNode_H)

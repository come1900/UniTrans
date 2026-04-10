#!/bin/sh
#输出ver文件当前值并+1

Version=build_ver
expr `cat $Version 2>/dev/null` + 1 > $Version
cat $Version


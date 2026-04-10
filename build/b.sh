#!/bin/sh
# 2016-08-29 15:25:17, joy.woo@ezlibs.com
# ÁÐ±íÎ¬»¤

#2017-04-30 16:08:37
#135
TheRtspList="
09.Makefile.LiveStreamming
09.02.Makefile.LiveStreamming.newMedia
09.03.Makefile.LiveStreamming.Wone
11.Makefile.LiveRecord
11.03.Makefile.LiveRecord.Wone
"
#min len of TheRtspList
DevidLength_Min=10
DevidLength_Max=50

for DevID in $TheRtspList
do
	if [ ${#DevID} -gt ${DevidLength_Max} ]
	then
		echo "  ..."
		continue;
	fi

	if [ ${#DevID} -lt ${DevidLength_Min} ]
	then
		echo -e "\n--------------------------------------------:"${DevID}
	else
		make -f ${DevID} clean
		make -f ${DevID}
	fi;
done


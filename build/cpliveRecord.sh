#!/bin/sh
ver_liveStreamming="1.5.4"
fileName="liveRecord-uniTrans-linux"

#M01-JF-XH-TZ
scp -P51054 ${fileName} pss@ca7ad6e72a6.ezlibs.com:~/lrs/${fileName}_${ver_liveStreamming}

exit;

cd ~/liveRecord; ./stop.sh ; sleep 1; cp liveRecord-uniTrans-linux liveRecord-uniTrans-linux.re; cp liveStreamming-uniTrans-linux_1.5.4 liveStreamming-uniTrans-linux ; ./start.sh


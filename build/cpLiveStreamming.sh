#!/bin/sh
ver_liveStreamming="1.5.4"
fileName="liveStreamming-uniTrans-linux"

# Group-01(HongxinY).General.Pss
scp -P122 ${fileName} pss@61.160.52.11:~/liveStreamming/${fileName}_${ver_liveStreamming}
scp -P122 ${fileName} pss@61.147.2.13:~/liveStreamming/${fileName}_${ver_liveStreamming}
scp -P122 ${fileName} pss@61.147.2.20:~/liveStreamming/${fileName}_${ver_liveStreamming}
scp -P122 ${fileName} pss@61.147.2.34:~/liveStreamming/${fileName}_${ver_liveStreamming}
# ##### Group-02(External).General.Pss ...
# nt159
scp -P51036 ${fileName} pss@ca7ad6e72a6.ezlibs.com:~/liveStreamming/${fileName}_${ver_liveStreamming}
#nt164
scp -P51038 ${fileName} pss@ca7ad6e72a6.ezlibs.com:~/liveStreamming/${fileName}_${ver_liveStreamming}
#gts219AtHXYun
scp -P22 ${fileName} pss@202.102.101.219:~/liveStreamming/${fileName}_${ver_liveStreamming}
scp -P122 ${fileName} pss@61.160.52.71:~/liveStreamming/${fileName}_${ver_liveStreamming}

#M01-JF-XH-TZ
scp -P51054 ${fileName} pss@ca7ad6e72a6.ezlibs.com:~/lss/${fileName}_${ver_liveStreamming}

exit;

cd ~/liveStreamming; ./stop.sh ; sleep 1; cp liveStreamming-uniTrans-linux liveStreamming-uniTrans-linux_Pre; cp liveStreamming-uniTrans-linux_1.5.4 liveStreamming-uniTrans-linux ; ./start.sh


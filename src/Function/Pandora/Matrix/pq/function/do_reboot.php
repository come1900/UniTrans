<?php

$_Action = $_REQUEST["a"];// 1-reboot, 2-setwma, 3-clearwma
$_DevID = $_REQUEST["d"];

$__urlReq = '';
$__bodyReq = '';

if (strlen($_DevID) == 0) {
    $_DevID = 'G1664065432';
}

if (strlen($_Action) > 0) {

	if ($_Action == '1'){//reboot
		$__urlReq = 'http://vsa.pq.ezlibs.com:18900/pq/pub/task/start';
		$__bodyReq = '{ "DevID" : "' . $_DevID . '", "TaskID" : "reboot" }';
	}
	else if ($_Action == '2'){ // Wma
		$__urlReq = 'http://vsa.pq.ezlibs.com:18900/pq/shp/set';
		//$__urlReq = 'http://127.0.0.1:18900/pq/shp/set';
		$__bodyReq = '{"DevID":"' . $_DevID . '","Func":"\/sbin\/shpc.wma.ini","Shpcs":[{"cHost":"127.0.0.1","cItem":"","cPort":22,"cRemontePort":50001,"cType":"tcp"},{"cHost":"127.0.0.1","cItem":"","cPort":80,"cRemontePort":50002,"cType":"tcp"}],"sHost":"9e05c825f2e.ezlibs.com","sPort":50000,"sTocken":"9e05c825f2e1365fe638a09a597b362e5d111d12"}';
	}
	else if ($_Action == '3'){ // deWma, set wrong
		$__urlReq = 'http://vsa.pq.ezlibs.com:18900/pq/shp/set';
		$__bodyReq = '{"DevID":"' . $_DevID . '","Func":"\/sbin\/shpc.wma.ini","Shpcs":[{"cHost":"127.0.0.1","cItem":"","cPort":220,"cRemontePort":500,"cType":"tcp"},{"cHost":"127.0.0.1","cItem":"","cPort":220,"cRemontePort":550,"cType":"tcp"}],"sHost":"who.are.you.galaxy.cosmos","sPort":55550,"sTocken":"e05c825f2e1365fe638a09"}';

	}
	else{
		echo "Your input:" . $_Action . ", 1 or 0 needed";
	}

    if (strlen($__bodyReq) > 3 && strlen($__urlReq) > 3) {
        $ch = curl_init();
        $__headerReq = array("Content-Type: application/json", "PssSid: G1751453539", "AppKey: 701818788", "Nonce: d7acd6d9-7321-4535-8c54-a572fee5216c", "CurTime: 1472451456", "CheckSum: 3cb4003be9f0c7eb9bb6849b5927b93133abd23b", "Content-Length:" . strlen($__bodyReq));
        curl_setopt($ch, CURLOPT_URL, $__urlReq);
        curl_setopt($ch, CURLOPT_POST, 1);
        curl_setopt($ch, CURLOPT_CONNECTTIMEOUT, 60);
        curl_setopt($ch, CURLOPT_HTTPHEADER, $__headerReq);
        curl_setopt($ch, CURLOPT_POSTFIELDS, $__bodyReq);
        curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);
        $output = curl_exec($ch);
        //echo $output;

        curl_close($ch);
    }
}
else
{
    echo "Input 1 or 0";
}

?>


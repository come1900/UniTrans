<?php
	// Point to where you downloaded the phar
	include('./httpful.phar');

	// 
	$secondsTimeout=3;
	$url = 'http://vsa.pq.ezlibs.com:18900/UniTrans/hello';
	$response = \Httpful\Request::post($url)
		->timeoutIn(  $secondsTimeout )
		->addHeader('PssSid', 'G1758103565')
		->addHeader('AppKey', '701818788')
		->body('{"DevID":"G1758103565"}')
		->send();

	echo $url . '::' . $response->body->Result  . '::' .  $response->body->ResultMsg . "\n";

?>

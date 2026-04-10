<?php

//------------------------------------------
// Start Session
session_start();
// check user login
if(empty($_SESSION['user_id']))
{
    header("Location: login_system/login.php");
    exit;
}
//------------------------------------------
	include ('./pq_pages_common.php');
	// Point to where you downloaded the phar
	include('./httpful.phar');
	include('./pss_ServicesArray.php');

	function restfulReqSnd($HostPort, $Method)
	{
		// 
		$secondsTimeout=3;
		//$url = 'http://vsa.pq.ezlibs.com:18900/UniTrans/hello';
		//$url = $HostPort . '/' . $Method;
		$url = $HostPort  . $Method;
		$response = \Httpful\Request::post($url)
			->timeoutIn(  $secondsTimeout )
			->addHeader('PssSid', 'G1758103565')
			->addHeader('AppKey', '701818788')
			->body('{"DevID":"G1758103565"}')
			->send();

		return $response;
	}		
	pq_top("chkPssServices", 60);

	echo "<h1>chk_PssServices</h1>\n";
	echo "<hr>\n";
	flush();

	echo "<table  id=\"customers\">\n";
	//echo "<tr> <th>No</th><th>URL</th> <th>Result</th> <th>ResultMsg</th> </tr>\n";

	$NoOfItems=1;
	$count_pssServicesArray = count($pssServicesArray);
	for ($i = 0; $i < $count_pssServicesArray; $i++) {

		if ($pssServicesArray[$i][0] == 2){
			$NoOfItems=1;
			//echo '<tr> <th colspan="4"><span class="pq_bond">' . $pssServicesArray[$i][1] . '</span></td>' . "</tr>\n";
			echo '<tr> <th colspan="4">' . $pssServicesArray[$i][1] . "</td></tr>\n";
			continue;
		}
		else if ($pssServicesArray[$i][0] == 3){
			continue;
		}

		$response = restfulReqSnd($pssServicesArray[$i][1]/*URL*/, '/UniTrans/hello');

		if ($NoOfItems%2==0){
			echo '<tr class="alt">';
		}
		else{
			echo '<tr>';
		}

		if ($response->hasErrors()) {
			echo '<td>' . $NoOfItems . '</td> <td>' . $pssServicesArray[$i][1] . '</td> <td>' . -1  . '</td> <td>' . 'Failed' . "</td> </tr>\n";
		}
		else
		{
			echo '<td>' . $NoOfItems . '</td> <td>' . $pssServicesArray[$i][1] . '</td> <td>' . $response->body->Result  . '</td> <td>' . $response->body->ResultMsg . "</td> </tr>\n";
		}

		flush();
		$NoOfItems++;
	}//for

	echo "</table>";

	pq_bottom(__FILE__);

?>

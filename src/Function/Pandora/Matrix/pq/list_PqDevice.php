<script>
function showHint(str, strCh) {
    if (str.length == 0 || strCh.length == 0) { 
        document.getElementById("txtHint").innerHTML = "";
        return;
    } else {

        var con;
        con=confirm("Sure?");

        var xmlhttp = new XMLHttpRequest();
        xmlhttp.onreadystatechange = function() {
            if (this.readyState == 4 && this.status == 200) {
                document.getElementById("txtHint").innerHTML = this.responseText;
            }
        };

        if(con==true){
                xmlhttp.open("GET", "function/do_reboot.php?a=" + str + "&d=" + strCh, true);
                xmlhttp.send();

                alert("command has been sended.");
        }
        else {
                alert("Do Nothing.");
        }
    }
}
</script>

<?php
	include ('./pq_pages_common.php');
	// Point to where you downloaded the phar
	include('./httpful.phar');
	include('./pq_vendorArray.php');

	function restfulReqSnd($HostPort, $Method, $GroupID)
	{
		// 
		$secondsTimeout=3;
		//$url = 'http://vsa.pq.ezlibs.com:18900/UniTrans/hello';
		//$url = $HostPort . '/' . $Method;
		$url = $HostPort . $Method;
		$response = \Httpful\Request::post($url)
			->timeoutIn(  $secondsTimeout )
			->addHeader('PssSid', 'G1758103565')
			->addHeader('AppKey', '701818788')
			->addHeader('Nonce', 'd7acd6d9-7321-4535-8c54-a572fee5216c')
			->addHeader('CurTime', '1472451456')
			->addHeader('CheckSum', '3cb4003be9f0c7eb9bb6849b5927b93133abd23b')
			->body("{  \"GroupID\": \"" . $GroupID . "\",  \"PageSize\": 9999,  \"Pages\": 1  }")
			->send();

		return $response;
	}		
	pq_top("listPqDevice", 70);

	date_default_timezone_set("Asia/Shanghai");

	echo "<h1>listPqDevice</h1>\n";
	echo "<hr>\n";
	//flush();

	echo "<table  id=\"customers\">\n";
	echo "<tr> <th>No</th><th>DevID</th> <th>Name</th> <th>Mac</th> <th>LastOn</th> <th>Version</th> <th>Operation</th> </tr>\n";

	$totalDevices=0;
	$count_vendorArray = count($vendorArray);
	for ($i = 0; $i < $count_vendorArray; $i++) {

		if ($vendorArray[$i][0] == 2){
			echo '<tr> <th colspan="7">' . $vendorArray[$i][1] . "</td></tr>\n";
			continue;
		}
		else if ($vendorArray[$i][0] == 3){
			continue;
		}

		$response = restfulReqSnd($vendorArray[$i][1]/*URL*/, '/pq/get/device/list', $vendorArray[$i][2]);

		if (empty($response->body->Devices)) {
			$count_Devices=0;
		}
		else{
			$count_Devices = count($response->body->Devices);
		}

		$totalDevices=$totalDevices+$count_Devices;

		for ($iDevs = 0; $iDevs < $count_Devices; $iDevs++) {
			if ($iDevs%2==0){
				echo '<tr class="alt">';
			}
			else{
				echo '<tr>';
			}

			$NoShow=$iDevs+1;
			
			echo '<td>' . $NoShow . '</td>';
			echo '<td>' . $response->body->Devices[$iDevs]->DevID . '</td>';
			echo '<td>' . $response->body->Devices[$iDevs]->Name . '</td>';
			echo '<td>' . $response->body->Devices[$iDevs]->Mac . '</td>';
			echo '<td>' . date("m-d H:i:s", $response->body->Devices[$iDevs]->LastOnTime) . '</td>';
			echo '<td>' . $response->body->Devices[$iDevs]->Version . '</td>';
			echo "<td><button type=\"button\" onclick=\"showHint('1', '" . $response->body->Devices[$iDevs]->DevID . "')\">Reboot</button>";
			echo "<button type=\"button\" onclick=\"showHint('2', '" . $response->body->Devices[$iDevs]->DevID . "')\">Wma</button>";
			echo "<button type=\"button\" onclick=\"showHint('3', '" . $response->body->Devices[$iDevs]->DevID . "')\">deWma</button></td>";

			echo "</tr>\n";
		}

		//flush();
	}//for

	echo "</table>\n";
	echo "<p>->> Total:$totalDevices</p>\n";

	pq_bottom(__FILE__);
?>

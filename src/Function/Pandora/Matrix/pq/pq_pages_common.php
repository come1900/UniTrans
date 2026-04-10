<?php
//
function pq_top($Title, $refresh_content=0)
{
	echo '<!DOCTYPE html>  <html>';
	if ($refresh_content>0)
	{
		echo '<meta http-equiv=\'refresh\' content=\''; echo $refresh_content; echo '\'/>';
	}

	echo '<head> <title> ' . $Title . ' </title>
		<link rel="shortcut icon" href="/favicon.ico" />
		<link rel="bookmark" href="/favicon.ico" type="image/x-icon" />
		<style>
		    body {
		        //width: 35em;
		        margin: 0 auto;
		        font-family: Tahoma, Verdana, Arial, sans-serif;
		    }
#customers
{
	font-family:"Trebuchet MS", Arial, Helvetica, sans-serif;
	width:100%;
	border-spacing: 5px;
	border-collapse:collapse;
}
#customers td, #customers th 
{
	font-size:1em;
	border:1px solid #98bf21;
	padding:3px 7px 2px 7px;
}
#customers th 
{
	font-size:1.1em;
	text-align:left;
	padding-top:5px;
	padding-bottom:4px;
	background-color:#A7C942;
	color:#ffffff;
}
#customers tr.alt td 
{
	color:#000000;
	background-color:#EAF2D3;
}
		        .pq_bond{
		        	color:#304FFE; 
		        	font-weight:bold
		        }

		</style>
		</head>
		<body>
	';
}//pq_top
//
function pq_bottom($FilePathName)
{
	echo '<hr>';
	date_default_timezone_set("Asia/Shanghai");
	$a=filemtime($FilePathName);
	echo "<p><em>Update:" . date("Y-m-d H:i:s", $a) . ", PageFresh:" . date('Y-m-d H:i:s')."(CST)</em></p>";
}

?>

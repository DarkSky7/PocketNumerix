<?php
include_once( "my_parse_str.php" );
# include_once( "../Crypt/KeyExpansion.php" );
# include_once( "../Crypt/Rijndael256.php" );					# defines $blockSize
# include_once( "../Crypt/MimeDecode.php" );
# include_once( "../Crypt/MimeEncode.php" );

/*
 function echoBinary( $fname )
{	$fp = fopen( $fname, "rb" );	
	while ( 1 )						# while ( ! feof($fp) ) would cause one byte too many to be written
	{	$str = fread( $fp, 1 );		# not buffering due to a lack of trust in behavior of ASCII NULL
		if ( feof( $fp ) )
			break;
		echo $str;					# no need to use chr() from a file (!)
	}
	fclose( $fp );
}
*/
$debug = false;
$skipDecrypt = false;
$useSantaFe = false;					# false --> BlueHost (pmmx.com)
$useSantaFeForOpPort = false;
# sample request for YieldCurveFitter:
# "http://pnmx.com/Install_Process/pnmxSubscribe.php?ds=DAjxsUD/4g5eP4LP1kYIPdDzG8UeisQj0T25ra0clIaT7zAJNmBGuiP/7L+3ZLYpUXvUJglMyqCDIHriVa40zapavV8s2JsXvjcuFZT5jSQtq+PY3BcE"
# sample request for StrategyExplorer:
# "http://localhost/Install_Process/Request.php?ds=/0XL6gmeA0e3lWr43NOtbJp1yn7sNCWVuMY55cvdVQR1k8LB4UngvtFxNYCA6D2TrfjCNPSAmxGe21aV/rjTLic6yUiwy7pGEkSdirYUnV7BZpx+mXwmXd52br35N4dqlWx3o5KCaTIUy7Oi+r9uS9XnjXa6asV5j9juWEO/S/lo1w=="
#-------------------------------------------------------------------------------+
#								params retrieval								|
#-------------------------------------------------------------------------------+
if ( $skipDecrypt )
{
/*
  	$bm_ds = "?in=2?pn=BondManager?os=5.1.1702?dv=618754147?uu=0127001E0705252128000050BFE45CE5?hu=EDCE46DA483C3F6705DFD35BF3DD63D3077656C5";
	$nh_ds = "?in=2?pn=NillaHedge?os=5.1.1702?dv=618754147?uu=0127001E0705252128000050BFE45CE5?hu=470137269D06F18B851982DC9097341D5F5996E8";
	$sx_ds = "?in=2?pn=StrategyExplorer?os=5.1.1702?dv=618754147?uu=0127001E0705252128000050BFE45CE5?hu=5340C7BDF65200D1FE5542E1B2E6C75B4E3BEDAF";
	$ycf_dx = "?in=2?pn=YieldCurveFitter?os=5.1.1702?dv=618754147?uu=0127001E0705252128000050BFE45CE5?hu=FCCD1C511473DDE99C4DDB1195BD06E502181B3B";
	# the BondManager target as a test case
  	$decryptParams = "?in=2?pn=BondManager?os=5.1.1702?dv=618754147?uu=0127001E0705252128000050BFE45CE5?hu=EDCE46DA483C3F6705DFD35BF3DD63D3077656C5";
*/
}
else
{	# $queryString = $HTTP_SERVER_VARS[ 'QUERY_STRING' ];		# doesn't work
	$queryString = $_SERVER[ 'QUERY_STRING' ];					# works
	
	# expecting to find:  ds parameter which ultimately contains:  in os pn dv uu hu
	$pos = strpos( $queryString, "ds=" );
	$ds = substr( $queryString, $pos+3 );						# chop off 'ds=', keep the rest, e.g.
	if ( $debug )
		printf( "ds=%s<br>", $ds );

	$dslen = strlen( $ds );
	if ( $debug )
		printf( "dslen=%d<br>", $dslen );
	if ( $dslen == 0 )
		die;
	#---------------------------------------------------------------------------+
	#							decode the ds argument							|
	#---------------------------------------------------------------------------+
	include_once( "MakeRanName.php" );
	$ranName = "./tmp/" . MakeRanName();
	$dsFile = $ranName . ".n";

	$fp = fopen( $dsFile, "w" );
	if ( $fp == NULL )
	{   if ( $debug )
			printf( "Couldn't open(w) %s<br>", $dsFile );
		die();
	}
	$bytesWritten = fwrite( $fp, $ds, $dslen );
	if ( $bytesWritten != $dslen )
	{	if ( $debug )
				printf( "wrote %d bytes to %s, should have been=%d<br>", $bytesWritten, $dsFile, $dslen );
		die();
	}
	fclose( $fp );

	$crpFile = $ranName . ".o";
	$lastline = system( "../cgi-bin/med d $dsFile $crpFile" );
	if ( $lastLine != "" )
	{   if ( $debug )
			printf( "system(med) returned...<br>%s<br>", $lastLine );
		die();
	}
	#---------------------------------------------------------------------------+
	#							decrypt param string	 						|
	#---------------------------------------------------------------------------+
	$clrFile = $ranName . ".p";
	$lastline = system( "../cgi-bin/rc d ./cfg/pnSubDs.cfg $crpFile $clrFile" );
	if ( $lastLine != "" )
	{   if ( $debug )
			printf( "system(rc d) returned...<br>%s<br>", $lastLine );
		die();
	}
	$decryptParams = file_get_contents( $clrFile );

	# cleanup temp files
	$lastline = system( "rm $dsFile $crpFile $clrFile" );
	if ( $lastLine != "" )
	{   if ( $debug )
			printf( "system(rm) returned...<br>%s<br>", $lastLine );
		die();
	}
	if ( $debug )
		printf( "decryptParams=%s<br>", $decryptParams );
}
#-------------------------------------------------------------------------------+
#							parse the decrypted params							|
#-------------------------------------------------------------------------------+
# $paramsArr should contain the results of parsing a string generated with:
#    "?in=%d?pn=%s?os=%s?dv=%d?uu=%s?hu=%s", thus: in, pn, os, dv, uu, hu
my_parse_str( $decryptParams, $paramsArr );
if ( $debug )
{	echo "<br>";
	foreach ( $paramsArr as $key => $val )
		printf( "%s => %s<br>", $key, $val );
}

$osVer = $paramsArr['os'];				if ( $debug )	echo "osVer=$osVer<br>";			# first field keying the item record
$shortVer = $osVer[0];
if ( $shortVer < '3'  or $shortVer > '5' )
	die( "shortVer=$shortVer, $osVer=$osVer" );
# gather intel on the request
$dateFormat = "Y-m-d H:i:s";
$accessDateTime = date( $dateFormat );	if ( $debug )	echo "accessDateTime=$accessDateTime<br>";
$ipAddress = $_SERVER['REMOTE_ADDR'];	if ( $debug )	echo "ipAddress=$ipAddress<br>";
$orderid = $paramsArr['in'];			if ( $debug )	echo "orderid=$orderid<br>";
$progName = $paramsArr['pn'];			if ( $debug )	echo "progName=$progName<br>";		# second field keying the item record
$osVer = $paramsArr['os'];				if ( $debug )	echo "osVer=$osVer<br>";
# $deviceid = $paramsArr['dv'];			if ( $debug )	echo "deviceid=$deviceid<br>";		# the hardware deviceid (per PpcInstall)
$uuid = $paramsArr['uu'];				if ( $debug )	echo "uuid=$uuid<br>";				# UUID for the device (could be "0")
$huid = $paramsArr['hu'];				if ( $debug )	echo "huid=$huid<br>";				# hashed UUID for this progName (could be "0")
#-------------------------------------------------------------------------------+
#						setup database access and authenticate					|
#-------------------------------------------------------------------------------+
# downloads
$sql_host = 'localhost';				# same on SantaFe and BlueHost
if ( $useSantaFe )
{	$sql_db = 'pnmxSS';					# was pnmxss
	$sql_user = 'SSadmin';				# was SSordersReader
	$sql_password = 'SSpw57';			# was kn2Gw7xT13a0pI5
}
else									# BlueHost
{	$sql_user ='pnmxcom_SSadmin';
	$sql_db ='pnmxcom_pnmxss';
	$sql_password ='jN5U2v7WmPa83R';
}

$link = mysql_connect( $sql_host, $sql_user, $sql_password ) or die( "Couldn't connect: " .  mysql_error() );
if ( ! mysql_select_db( $sql_db, $link ) ) die( "Couldn't select database" );
#-------------------------------------------------------------------------------+
#						impute productid from progName							|
#-------------------------------------------------------------------------------+
$nh_huid = "";
$ycf_huid = "";
if ( $debug ) echo "B: huid ==> '$huid'<br>";
if (	 strcmp( $progName, "OptionChains")	== 0 )
{	$nh_huid  = $huid;
	$productid = 5;
}
elseif ( strcmp( $progName, "TBillRates") == 0 )
{	$ycf_huid = $huid;
	$productid = 6;
}
if ( $debug ) echo "productid=$productid<br>";
#-------------------------------------------------------------------------------+
#						get id from devices as devicesid						|
#-------------------------------------------------------------------------------+
# $devicesSelectQuery = "SELECT id FROM pnmx_devices WHERE osVer = '$osVer' AND deviceid = '$deviceid' AND uuid = '$uuid'";
$devicesSelectQuery = "SELECT id FROM pnmx_devices WHERE osVer = '$osVer' AND uuid = '$uuid'";
if ( $debug ) printf( "query=$devicesSelectQuery<br>" );
$result = mysql_query( $devicesSelectQuery, $link )	or die( "Devices Q1 failed: " . mysql_error() );
$numRes = mysql_num_rows( $result );
if ( $numRes < 1 )
# 	$query = "INSERT INTO pnmx_devices ( osVer, deviceid, uuid, nh_huid, ycf_huid ) VALUES ( '$osVer', '$deviceid', '$uuid', '$nh_huid', '$ycf_huid')";
	$query = "INSERT INTO pnmx_devices ( osVer, uuid, nh_huid, ycf_huid ) VALUES ( '$osVer', '$uuid', '$nh_huid', '$ycf_huid')";
else
{	if ( $debug ) echo "defining an UPDATE query<br>";
	$row = mysql_fetch_assoc( $result );
	if ( $debug )
	{	echo "the devices row:  ";
		print_r( $row );
		echo "<br>";
	}
	$devicesid = $row['id'];						# so we can update the devices and download_requests records
	if ( $debug ) printf( "devicesid=%d<br>", $devicesid );
	if ( strcmp( $progName, "OptionChains") == 0   &&  strlen( $row['nh_huid'] ) < 40 )
		$query = "UPDATE pnmx_devices SET nh_huid = '$huid' WHERE id = '$devicesid'";
	elseif ( strcmp( $progName, "TBillRates") == 0  &&  strlen( $row['ycf_huid'] ) < 40 )
		$query = "UPDATE pnmx_devices SET ycf_huid = '$huid' WHERE id = '$devicesid'";
	else
		$query = "";
}
if ( $debug )
	echo "devices UI query=$query<br>";
if ( $query != "" )
{	$result = mysql_query( $query, $link ) or die( "Devices UI failed: " . mysql_error() );		# run the INSERT or UPDATE devices query
	if ( $numRes < 1 )																			# still looking for devicesid
	{	# the previous query was an INSERT, so we still don't know the devicesid
		$result = mysql_query( $devicesSelectQuery, $link ) or die( "Devices Q2 failed: " . mysql_error() );
		$numRes = mysql_num_rows( $result );
		if ( $numRes > 0 )
		{	$row = mysql_fetch_assoc( $result );
			$devicesid = $row['id'];						# so we can insert a subscription record
		}
	}
}
#-------------------------------------------------------------------------------+
#						log 'request' in download_requests						|
#-------------------------------------------------------------------------------+
$query = "INSERT INTO pnmx_download_requests ( ipAddress, accessDateTime, orderid, productid, devicesid, status ) VALUES ( '$ipAddress', '$accessDateTime', '$orderid', '$productid', '$devicesid', 'request' )";
if ( $debug )	printf( "query=$query<br>" );
$result = mysql_query( $query, $link ) or die( "Requests I failed: " . mysql_error() );
#-------------------------------------------------------------------------------+
#				retrieve the request id for subsequent update					|
#-------------------------------------------------------------------------------+
$query = "SELECT id FROM pnmx_download_requests WHERE orderid = '$orderid' AND accessDateTime = '$accessDateTime'";
if ( $debug )	printf( "query=$query<br>" );
$result = mysql_query( $query, $link ) or die( "Requests Q failed: " . mysql_error() );
$row = mysql_fetch_assoc( $result );
$requestid = $row['id'];						# so we can update the record later
#-------------------------------------------------------------------------------+
#				block same order-item downloads by dissimilar devices			|
#-------------------------------------------------------------------------------+
$query = "SELECT accessDateTime, devicesid FROM pnmx_download_requests WHERE orderid = '$orderid' AND productid = '$productid' AND status = 'complete' ORDER BY accessDateTime ASC";
if ( $debug ) printf( "query=$query<br>" );
$result = mysql_query( $query, $link ) or die( "Download requests Q failed: " . mysql_error() );
$numRes = mysql_num_rows( $result );
if ( $numRes > 0 )
{	$row = mysql_fetch_assoc( $result );
	if ( $row['devicesid'] != $devicesid )
		die( "devicesid=?<br>" );									# already allocated to another device
	$completeStartDate = strtotime( $row['accessDateTime'] );		# 'ORDER BY accessDateTime ASC' assures us that the original entry is first (restore that)
	if ( $debug ) echo "completeStartDate=$row[accessDateTime]<br>";
}
# duplicate 'complete' request gate passed (the subscription has not been assigned to another device)
#-------------------------------------------------------------------------------+
#					compute the end date for the subsccription					|
#-------------------------------------------------------------------------------+
$query = "SELECT amount FROM xcart_order_details where orderid = '$orderid' AND productid = '$productid'";
if ( $debug )	printf( "query=$query<br>" );
$result = mysql_query( $query, $link ) or die( "Details Q failed: " . mysql_error() );
$numRes = mysql_num_rows( $result );
if ( $numRes < 1 )
	die("no results for amount Q");							# no productid subscription on this order

# all gates passed
$row = mysql_fetch_assoc( $result );
$years = intval( $row['amount'] );							# an integer?
if ( $debug )
	echo "years = $years<br>";
if ( $completeStartDate )
{	if ( $debug ) echo "completeStartDate is defined<br>";
	$yr = intval( date("Y", $completeStartDate ) );			# defined above
	$moDa = date( "m-d", $completeStartDate );				# mm-dd (both w/leading zeroes)
}
else
{	if ( $debug ) echo "completeStartDate is undefined<br>";
	$yr = intval( date( "Y" ) );							# unknown above
	$moDa = date( "m-d" );									# mm-dd (both w/leading zeroes)
}
$endMoDa = $moDa;
if ( $years != 4  &&  strcmp($moDa, "02-29") == 0 )
	$endMoDa = "02-28";
$startDate = sprintf( "%4d-%s", $yr, $moDa );
if ( $debug )
	echo "startDate=$startDate<br>";
$endDate = sprintf( "%4d-%s", $yr+$years, $endMoDa );
if ( $debug )
	echo "endDate=$endDate<br>";
#-------------------------------------------------------------------------------+
#								decide what to send								|
#-------------------------------------------------------------------------------+
if ( $productid == 5 )
	$fname = "Supplies/OCR.crp";
elseif ( $productid == 6 )
	$fname = "Supplies/TBR.crp";
else
	die("Unexpected productid=$productid<br>");
#--------------------------
if ( $completeStartDate )
{	# we have trapped a previously registered subscription
#	echoBinary( $fname );
	passthru( "cat $fname" );
}
else
{	# if reset, we didn't capture it above, thus we can assume that OptimalPortfolio hasn't been notified
	#-------------------------------------------------------------------------------+
	#					update the download_request to 'download'					|
	#	(representing the beginning up the OptimalPortfolio notification process)	|
	#-------------------------------------------------------------------------------+
	$query = "UPDATE pnmx_download_requests SET status = 'download' WHERE id = '$requestid'";
	if ( $debug ) printf( "query=$query<br>" );
	$result = mysql_query( $query, $link ) or die( "Requests U1 failed: " . mysql_error() );
	#-------------------------------------------------------------------------------+
	#	create a params string for OptimalPortfolio	INSERT subscription query		|
	#-------------------------------------------------------------------------------+
#	$params = sprintf( "?in=%s?pn=%s?dvs=%s?os=%s?dv=%s?uu=%s?hu=%s?sd=%s?ed=%s",
#					  $orderid, $productid, $devicesid, $osVer, $deviceid, $uuid, $huid, $startDate, $endDate );
	$params = sprintf( "?in=%s?pn=%s?dvs=%s?os=%s?uu=%s?hu=%s?sd=%s?ed=%s",
					  $orderid, $productid, $devicesid, $osVer, $uuid, $huid, $startDate, $endDate );
	if ( $debug ) printf( "params=$params<br>" );
	$paramsLen = strlen( $params );
	if ( $debug ) echo "paramsLen=$paramsLen<br>";
	#-------------------------------------------------------------------------------+
	#		encrypt the params string to OptimalPorftolio's opSubscribe.php			|
	#-------------------------------------------------------------------------------+
	$clrFile = $ranName . ".q";
	if ( $debug ) printf( "clrFile='%s'<br>", $clrFile );
	$fp = fopen( $clrFile, "w" );
	if ( $fp == NULL )
	{   if ( $debug )
			printf( "Couldn't open(w) %s<br>", $clrFile );
		die();
	}
	$bytesWritten = fwrite( $fp, $params, $paramsLen );
	if ( $debug ) printf( "bytesWritten=%d<br>", $bytesWritten );
	if ( $bytesWritten != $paramsLen )
	{	if ( $debug )
			printf( "wrote %d bytes to %s, should have been=%d<br>", $bytesWritten, $clrFile, $paramsLen );
		die();
	}
	fclose( $fp );
	
	$crpFile = $ranName . ".r";
	if ( $debug ) printf( "crpFile='%s'<br>", $crpFile );
	$lastline = system( "../cgi-bin/rc e ./cfg/opSubDs.cfg $clrFile $crpFile" );
	if ( $lastLine != "" )
	{	if ( $debug )
			printf( "system(rc e) returned...<br>%s<br>", $lastLine );
		die();
	}
	# echo the crypt file to the browser stream
	$mimeFile = $ranName . ".s";
	if ( $debug ) printf( "mimeFile='%s'<br>", $mimeFile );
	$lastLine = system( "../cgi-bin/med e $crpFile $mimeFile" );
	if ( $lastLine != "" )
	{	if ( $debug )
			printf( "system(med e) returned...<br>%s<br>", $lastLine );
		die();
	}
	$ds = file_get_contents( $mimeFile );
	if ( $debug ) printf( "ds='%s'<br>", $ds );

	# cleanup temp files
	$lastline = system( "rm $clrFile $crpFile $mimeFile" );
	if ( $lastLine != "" )
	{	if ( $debug )
			printf( "system(rm) returned...<br>%s<br>", $lastLine );
		die();
	}

	if ( $useSantaFeForOpPort )
		$url = "http://localhost/Install_Process/opSubscribe.php?ds=" . $ds;
	else
		$url = "http://OptimalPortfolio.net/Install_Process/opSubscribe.php?ds=" . $ds;  
	if ( $debug ) echo "url=$url<br>";
	if ( $debug ) echo "-----------------------------------------------------------------------------------------<br>";

	$contents = file_get_contents( $url );
	if ( $debug ) echo "contents=...<br>$contents<br>";
	if ( strcmp( $contents, "Okay" ) != 0 )
		die;

	# OptimalPortfolio will decrypt the params and do something like this:
	## $query = "INSERT INTO {ocr,tbr}_subscriptions ( orderid, devicesid, deviceid, uuid, huid, startDate, endDate ) VALUES ( '$ipAddress', '$orderid', '$devicesid', '$deviceid', '$uuid', '$huid', '$startDate', '$endDate' )";
	# $query = "INSERT INTO {ocr,tbr}_subscriptions ( orderid, devicesid, uuid, huid, startDate, endDate ) VALUES ( '$ipAddress', '$orderid', '$devicesid', '$uuid', '$huid', '$startDate', '$endDate' )";
	# $result = mysql_query( $query, $link )	or die( "Requests I failed: " . mysql_error() );

#	echoBinary( $fname );
	passthru( "cat $fname" );

	#-------------------------------------------------------------------------------+
	#						log 'complete' in download_requests						|
	#-------------------------------------------------------------------------------+
	$query = "UPDATE pnmx_download_requests SET status = 'complete' WHERE id = '$requestid'";
	if ( $debug ) printf( "query=$query<br>" );
	$result = mysql_query( $query, $link ) or die( "Requests U1 failed: " . mysql_error() );
}
mysql_close( $link );
?>

<?php
include_once( "my_parse_str.php" );

$debug = false;
$skipDecrypt = false;
$useSantaFe = false;				# false --> BlueHost

# sample request for StrategyExplorer:
# "http://localhost/Install_Process/Request.php?ds=/0XL6gmeA0e3lWr43NOtbJp1yn7sNCWVuMY55cvdVQR1k8LB4UngvtFxNYCA6D2TrfjCNPSAmxGe21aV/rjTLic6yUiwy7pGEkSdirYUnV7BZpx+mXwmXd52br35N4dqlWx3o5KCaTIUy7Oi+r9uS9XnjXa6asV5j9juWEO/S/lo1w=="
# sample request for PpcInstall:
# "http://pnmx.com/Install_Process/Request.php?ds=WguritKkp9zdxxwnSTt2eGYZ20iuIbOdSy0xVvc38o3EaApbTfFiOuODebIEAn/9r2IAtrrWGAk="
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
*/
  	$decryptParams = "?in=2?pn=BondManager?os=5.1.1702?dv=618754147?uu=0127001E0705252128000050BFE45CE5?hu=EDCE46DA483C3F6705DFD35BF3DD63D3077656C5";
}
else
{	# $queryString = $HTTP_SERVER_VARS[ 'QUERY_STRING' ];		# doesn't work
	$queryString = $_SERVER[ 'QUERY_STRING' ];					# works
	
	# expecting to find:  ds parameter which ultimately contains:  in os pn dv uu hu
	$pos = strpos( $queryString, "ds=" );
	$ds = substr( $queryString, $pos+3 );			# chop off 'ds=', keep the rest, e.g.
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
	$dsFile = $ranName . ".f";

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

	$crpFile = $ranName . ".g";
	$lastline = system( "../cgi-bin/med d $dsFile $crpFile" );
	if ( $lastLine != "" )
	{   if ( $debug )
			printf( "system(med) returned...<br>%s<br>", $lastLine );
		die();
	}
	#---------------------------------------------------------------------------+
	#							decrypt param string	 						|
	#---------------------------------------------------------------------------+
	$clrFile = $ranName . ".h";
	$lastline = system( "../cgi-bin/rc d ./cfg/ReqDs.cfg $crpFile $clrFile" );
	if ( $lastLine != "" )
	{   if ( $debug )
			printf( "system(rc) returned...<br>%s<br>", $lastLine );
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
# $deviceid = $paramsArr['dv'];			if ( $debug )	echo "deviceid=$deviceid<br>";		# the hardware deviceid (per PpcInstall)
$uuid = $paramsArr['uu'];				if ( $debug )	echo "uuid=$uuid<br>";				# UUID for the device (could be "0")
$huid = $paramsArr['hu'];				if ( $debug )	echo "huid=$huid<br>";				# hashed UUID for this progName (could be "0")
#-------------------------------------------------------------------------------+
#						setup database access and authenticate					|
#-------------------------------------------------------------------------------+
# downloads
$tracking = ( strcmp( $progName, "PpcInstall" ) != 0  &&  strcmp( $progName, "DLLs" ) != 0 );
if ( $tracking )
{	$sql_host = 'localhost';				# same on SantaFe and BlueHost
	if ( $useSantaFe )
	{	$sql_db = 'pnmxSS';
		$sql_user = 'SScompCRPfetch';
		$sql_password = 'c3-9v%3ry$3Q9zP';
	}
	else									# BlueHost
	{	$sql_user ='pnmxcom_SSadmin';
		$sql_db ='pnmxcom_pnmxss';
		$sql_password ='jN5U2v7WmPa83R';
	}

	$link = mysql_connect( $sql_host, $sql_user, $sql_password ) or die( "Couldn't connect: " .  mysql_error() );
	if ( ! mysql_select_db( $sql_db, $link ) ) die( "Couldn't select database" );
	#-------------------------------------------------------------------------------+
	#						get productid from order_details						|
	#-------------------------------------------------------------------------------+
	$query = "SELECT productid FROM xcart_order_details WHERE orderid = '$orderid' and product='$progName'";
	if ( $debug )		printf( "A:query=$query<br>" );
	$result = mysql_query( $query, $link )	or die( "Details Q failed: " . mysql_error() );
	$numRes = mysql_num_rows( $result );	if ( $debug )	printf( "Number of %s items on order %d is %d<br>", $progName, $orderid, $numRes );
	if ( $numRes < 1 )
		die( "Details Q - no results: orderid=" . $orderid );
	$row = mysql_fetch_assoc( $result );
	$productid = $row['productid'];			if ( $debug )	printf( "productid=%s<br>", $productid );
	#-------------------------------------------------------------------------------+
	#						get id from devices as devicesid						|
	#-------------------------------------------------------------------------------+
 	$bm_huid = "";
	$nh_huid = "";
	$sx_huid = "";
	$ycf_huid = "";
	if ( $debug )
		echo "B: huid ==> '$huid'<br>";
	if (	 strcmp( $progName, "BondManager")		== 0 )	$bm_huid  = $huid;
	elseif ( strcmp( $progName, "NillaHedge")		== 0 )	$nh_huid  = $huid;
	elseif ( strcmp( $progName, "StrategyExplorer") == 0 )	$sx_huid  = $huid;
	elseif ( strcmp( $progName, "YieldCurveFitter") == 0 )	$ycf_huid = $huid;

#	$devicesSelectQuery = "SELECT id FROM pnmx_devices WHERE osVer = '$osVer' AND deviceid = '$deviceid' AND uuid = '$uuid'";
	$devicesSelectQuery = "SELECT id FROM pnmx_devices WHERE osVer = '$osVer' AND uuid = '$uuid'";
	if ( $debug )	printf( "query=$devicesSelectQuery<br>" );
	$result = mysql_query( $devicesSelectQuery, $link )	or die( "Devices Q1 failed: " . mysql_error() );
	$numRes = mysql_num_rows( $result );
	if ( $numRes < 1 )
#		$query = "INSERT INTO pnmx_devices ( osVer, deviceid, uuid, bm_huid, nh_huid, sx_huid, ycf_huid ) VALUES ( '$osVer', '$deviceid', '$uuid', '$bm_huid', '$nh_huid', '$sx_huid', '$ycf_huid')";
		$query = "INSERT INTO pnmx_devices ( osVer, uuid, bm_huid, nh_huid, sx_huid, ycf_huid ) VALUES ( '$osVer', '$uuid', '$bm_huid', '$nh_huid', '$sx_huid', '$ycf_huid')";
	else
	{	if ( $debug )
			echo "defining an UPDATE query<br>";
		$row = mysql_fetch_assoc( $result );
		if ( $debug )
		{	echo "the devices row:  ";
			print_r( $row );
			echo "<br>";
		}
		$devicesid = $row['id'];						# so we can update the devices and download_requests records
		if ( $debug )
			printf( "devicesid=%d<br>", $devicesid );
		if (	 strcmp( $progName, "BondManager") == 0  &&  strlen( $row['bm_huid'] ) < 40  )
			$query = "UPDATE pnmx_devices SET bm_huid = '$huid' WHERE id = '$devicesid'";
		elseif ( strcmp( $progName, "NillaHedge") == 0   &&  strlen( $row['nh_huid'] ) < 40 )
			$query = "UPDATE pnmx_devices SET nh_huid = '$huid' WHERE id = '$devicesid'";
		elseif ( strcmp( $progName, "StrategyExplorer") == 0  &&  strlen( $row['sx_huid'] ) < 40 )
			$query = "UPDATE pnmx_devices SET sx_huid = '$huid' WHERE id = '$devicesid'";
		elseif ( strcmp( $progName, "YieldCurveFitter") == 0  &&  strlen( $row['ycf_huid'] ) < 40 )
			$query = "UPDATE pnmx_devices SET ycf_huid = '$huid' WHERE id = '$devicesid'";
		else
			$query = "";
	}
	if ( $debug )
		echo "devices UI query=$query<br>";
	if ( $query != "" )
	{	$result = mysql_query( $query, $link )	or die( "Devices UI failed: " . mysql_error() );		# run the INSERT or UPDATE devices query
		if ( $numRes < 1 )																				# still looking for devicesid
		{	# the previous query was an INSERT, so we still don't know the devicesid
			$result = mysql_query( $devicesSelectQuery, $link ) or die( "Devices Q2 failed: " . mysql_error() );
			$numRes = mysql_num_rows( $result );
			if ( $numRes > 0 )
			{	$row = mysql_fetch_assoc( $result );
				$devicesid = $row['id'];						# so we can insert a download_requests record
			}
		}
	}
	#-------------------------------------------------------------------------------+
	#						log 'request' in download_requests						|
	#-------------------------------------------------------------------------------+
	$query = "INSERT INTO pnmx_download_requests ( ipAddress, accessDateTime, orderid, productid, devicesid, status ) VALUES ( '$ipAddress', '$accessDateTime', '$orderid', '$productid', '$devicesid', 'request' )";
	if ( $debug )	printf( "query=$query<br>" );
	$result = mysql_query( $query, $link )	or die( "Requests I failed: " . mysql_error() );
	#-------------------------------------------------------------------------------+
	#				retrieve the request id for subsequent update					|
	#-------------------------------------------------------------------------------+
	$query = "SELECT id FROM pnmx_download_requests WHERE orderid = '$orderid' AND accessDateTime = '$accessDateTime'";
	if ( $debug )	printf( "query=$query<br>" );
	$result = mysql_query( $query, $link )	or die( "Requests Q failed: " . mysql_error() );
	$row = mysql_fetch_assoc( $result );
	$requestid = $row['id'];				# so we can update the record later
	# authentication consists of finding a record for progName associated with orderid,
	# UNLESS the request is for PpcInstall or DLLs, which we'll let sail on through authentication
	#-------------------------------------------------------------------------------+
	#				block same order-item downloads by dissimilar devices			|
	#-------------------------------------------------------------------------------+
	$query = "SELECT devicesid FROM pnmx_download_requests WHERE orderid = '$orderid' AND productid = '$productid' AND status = 'complete' ORDER BY accessDateTime ASC";
	if ( $debug )		printf( "query=$query<br>" );
	$result = mysql_query( $query, $link )	or die( "Download requests Q failed: " . mysql_error() );
	$numRes = mysql_num_rows( $result );
	if ( $numRes > 0 )
	{	$row = mysql_fetch_assoc( $result );
		if ( $row['devicesid'] != $devicesid )
			die;					# already allocated to another device
	}
	# duplicate 'complete' gate passed
}
#-------------------------------------------------------------------------------+
#							get product from order								|
#-------------------------------------------------------------------------------+
if ( $useSantaFe )
{	$dirSeparator = "\\";
	$rootDir = "c:\\\\htdocs\\";
}
else
{	$dirSeparator = "/";
	$rootDir = "/home/pnmxcom/public_html/";			# BlueHost is not based at / however
}
$suppliesDir = $rootDir . "Install_Process" . $dirSeparator . "Supplies" . $dirSeparator;

if ( $shortVer == "5")
	$prefix = "WM5_";
else if ( $shortVer == "4" )
	$prefix = "PPC03_";
else							# $ver == 3
	$prefix = "PPC02_";
if ( $debug )
	printf( "prefix = $prefix<br>" );
$srcPath = $suppliesDir . $prefix . $progName . ".crp";
if ( ! file_exists( $srcPath ) )
{	if ( $debug )
		printf( "Unable to locate %s<br>", $srcPath );
	die();
}
#-------------------------------------------------------------------------------+
#						log 'download' in download_requests						|
#-------------------------------------------------------------------------------+
if ( $tracking )
{	$query = "UPDATE pnmx_download_requests SET status = 'download' WHERE id = '$requestid'";
	if ( $debug ) printf( "query=$query<br>" );
	$result = mysql_query( $query, $link )	or die( "Requests U1 failed: " . mysql_error() );
}
#-------------------------------------------------------------------------------+
#						write binary output to the stream						|
#-------------------------------------------------------------------------------+
# echo the crypt file to the browser stream
passthru( "cat $srcPath" );
#-------------------------------------------------------------------------------+
#						log 'complete' in download_requests						|
#-------------------------------------------------------------------------------+
if ( $tracking )
{	$query = "UPDATE pnmx_download_requests SET status = 'complete' WHERE id = '$requestid'";
	$result = mysql_query( $query, $link ) or die( "Requests U2 failed: " . mysql_error() );
	mysql_close( $link );
}
?>

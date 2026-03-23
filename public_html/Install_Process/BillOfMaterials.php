<?php
include_once( "my_parse_str.php" );

$debug = false;
$skipDecrypt = false;		# NOTE: the RoundKeys generated for ds decryption are needed later, in normal use
$useSantaFe = false;
$downloadHost = "pnmx.com";
# include_once( "make_base62_names.php" );		# for MakeName()

function AddToBOM( $suppliesDir, $mimeDir, $ver, $displayName, $binaryName, &$result, &$aggSize )
{	# we only need $suppliesDir and $dirSeparator to determine the size of the target file ($tgtPath)
	$debug = false;
	if ( $debug )
		printf( "Adding: %s to BOM<br>", $productName );

	if ( $ver >= "5")
		$prefix = "WM5_";
	else if ( $ver == "4" )
		$prefix = "PPC03_";
	else													# $ver == 3
		$prefix = "PPC02_";
	if ( $debug )
		printf( "prefix = $prefix<br>" );
	$crpFile = $suppliesDir . $prefix . $binaryName . ".crp";
	if ( $debug )
		echo "AddToBOM: crpFile=$crpFile<br>";

#	$b1 = file_exists( $crpFile );									# on BlueHost, filesize() fails unless file_exists() call precedes it
#	if ( $debug )
#		printf( "file_exists(%s) --> %d<br>", $crpFile, $b1 );

	$size = filesize( $crpFile );
	if ( $size === false)
	{	echo "filesize is false!<br>";
		die;
	}
	if ( $debug )
		printf( "%s is %d bytes long<br>", $crpFile, $size );

	$mimeFile = $mimeDir . $prefix . $binaryName . ".mime";
	$mime = file_get_contents( $mimeFile );
	$result .= "$displayName $size $mime\n";
	$aggSize = $aggSize + $size;							# update $aggSize
}		# AddToBOM()

#-------------------------------------------------------------------------------+
#								params retrieval								|
#-------------------------------------------------------------------------------+
if ( $skipDecrypt )
{	# ?os=%s?dv=%d?em=%s?in=%s?dt=%4d-%02d-%02d?ln=%s?fn=%s
	# BOMds clear:	"?os=3.0.11171?dv=1214788427?em=mlandis@pnmx.com?in=2?dt=2007-04-25?ln=Landis?fn=Mike"
	# BOMds mime	"PkQzMbzjz+VoiKyUNRk+nQqtY17x6fQUD92CFILBUdP06GsJ9m/+IiPK6Rp+RrYxgxSuUPJJa3Kyxq+llfiFF8Ss786+SZXa5z9aEnSbkDVRANAR"
	$decryptParams = "?os=5.0.13?dv=104892745?em=mlandis001@comcast.net?fn=Mike?ln=Landis?in=1?dt=2007-02-21";		# SantaFe
}
else
{	# $queryString = $HTTP_SERVER_VARS[ 'QUERY_STRING' ];	# doesn't work
	$queryString = $_SERVER[ 'QUERY_STRING' ];				# works

	# expecting to find:  ds parameter which ultimately contains:  os, dv, em, in, dt, ln, fn
	$pos = strpos( $queryString, "ds=" );
	$ds = substr( $queryString, $pos+3 );					# chop off 'ds=', keep the rest, e.g.
	if ( $debug )
		printf( "ds=%s<br>", $ds );

	# for testing purposes
	#	$ds = "acPNJ059H#VKdS7XDp44pbCsM33pkcnhfGJMFsHp0XqKx9S+dNoTy+hqP9Lqg7erOmD7NO3LYfY2AhyTpwCQYl3ftAir9T7QUBrpF3U8pKjkTlQFpy1lg==";
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
	$dsFile = $ranName . ".a";

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

	$crpFile = $ranName . ".b";
	$lastline = system( "../cgi-bin/med d $dsFile $crpFile" );
	if ( $lastLine != "" )
	{   if ( $debug )
			printf( "system(med) returned...<br>%s<br>", $lastLine );
		die();
	}
	#---------------------------------------------------------------------------+
	#							decrypt param string	 						|
	#---------------------------------------------------------------------------+
	$clrFile = $ranName . ".c";
	$lastline = system( "../cgi-bin/rc d ./cfg/BOMds.cfg $crpFile $clrFile" );
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
#							authenticate the order								|
#-------------------------------------------------------------------------------+
# $paramsArr should contain the results of parsing a string generated with:
# "?os=%s?dv=%d?em=%s?in=%s?dt=%4d-%02d-%02d?ln=%s?fn=%s"
my_parse_str( $decryptParams, $paramsArr );
# if ( $debug )
# {	echo "<br>";
#	foreach ( $paramsArr as $key => $val )
#		printf( "%s => %s<br>", $key, $val );
# }
$dateFormat = "Y-m-d H:i:s";
$accessDateTime = date( $dateFormat );
$ipAddress = $_SERVER['REMOTE_ADDR'];			# with the dots
# $Octets = explode( ".", $ipAddress );			# string representation of the octets
# $ipAddress = ( intval( $Octets[0] ) << 24 )		# leftmost byte encountered
#		   | ( intval( $Octets[1] ) << 16 )
#		   | ( intval( $Octets[2] ) << 8  )
#		   |   intval( $Octets[3] );			# now an integer

$orderid  = $paramsArr['in'];	if ( $debug ) printf( "orderid=$orderid<br>/r/n" );
$deviceid = $paramsArr['dv'];	if ( $debug ) printf( "deviceid=deviceid<br>/r/n" );
$osVer = $paramsArr['os'];		if ( $debug ) printf( "osVer=$osVer<br>/r/n" );
#-------------------------------------------------------------------------------+
#								connect to the database							|
#-------------------------------------------------------------------------------+
$sql_host = 'localhost';				# same on SantaFe and BlueHost
if ( $useSantaFe )
{	$sql_db = 'pnmxSS';
	$sql_user = 'SSordersReader';
	$sql_password = 'kn2Gw7xT13a0pI5';
}
else									# BlueHost
{	$sql_user ='pnmxcom_SSadmin';
	$sql_db ='pnmxcom_pnmxss';
	$sql_password ='jN5U2v7WmPa83R';
}
$link = mysql_connect( $sql_host, $sql_user, $sql_password );
if ( $link === false ) die( "Couldn't connect: " .  mysql_error() );
if ( ! mysql_select_db( $sql_db ) ) die( "Couldn't select database" );
#-------------------------------------------------------------------------------+
#							log 'request' in bom_requests						|
#-------------------------------------------------------------------------------+
$query = "INSERT INTO pnmx_bom_requests (ipAddress, accessDateTime, orderid, deviceid, osVer, status) VALUES ('$ipAddress', '$accessDateTime', '$orderid', '$deviceid', '$osVer', 'request')";
if ( mysql_query( $query ) === false )
	die( "bom_requests I failed: . mysql_error()" );
#-------------------------------------------------------------------------------+
#				retrieve the request id for subsequent update					|
#-------------------------------------------------------------------------------+
$query = "SELECT id FROM pnmx_bom_requests WHERE orderid = '$orderid' AND accessDateTime = '$accessDateTime'";
$result = mysql_query( $query )	or die( "Requests Q failed: " . mysql_error() );
$row = mysql_fetch_assoc( $result );
$requestid = $row['id'];						# so we can update the record later
#-------------------------------------------------------------------------------+
#						gather information from the order						|
#-------------------------------------------------------------------------------+
$query = "SELECT firstname, lastname, email, date FROM xcart_orders WHERE orderid = '$orderid'";
if ( $debug ) printf( "query=$query<br>" );
$result = mysql_query( $query ) or die( "Orders: Q failed: " . mysql_error() );
$numRes = mysql_num_rows( $result );
if ( $debug ) printf( "Number of orders with orderid=%d is %d<br>", $orderid, $numRes );
if ( $numRes < 1 ) die( "No order" . $orderid );

$row = mysql_fetch_assoc( $result );
$dateSt = date( "Y-m-d", $row['date'] );	# e.g. 2007-12-31 format

if ( $debug )
{	printf( "firstname=%s<br>", $row['firstname'] );
	printf( "lastname=%s<br>", $row['lastname'] );
	printf( "email=%s<br>", $row['email'] );
	printf( "dateSt=%s<br>", $dateSt );
}

if (	 strcmp( $row['firstname'], $paramsArr['fn'] ) != 0
	 ||  strcmp( $row['lastname'],  $paramsArr['ln'] ) != 0
	 ||  strcmp( $row['email'],	    $paramsArr['em'] ) != 0
	 ||  strcmp( $dateSt,		    $paramsArr['dt'] ) != 0		)
{	mysql_close( $link );
	die( "Modified" );
}
# valid install token
#-------------------------------------------------------------------------------+
#							retrieve order details rows							|
#-------------------------------------------------------------------------------+
# $query = "SELECT productid, product, amount FROM xcart_order_details where orderid = '$orderid'";
$query = "SELECT productid, product FROM xcart_order_details where orderid = '$orderid'";
if ( $debug ) printf( "query=%s<br>", $query );
$result = mysql_query( $query )	or die( 'Order Q failed: ' . mysql_error() );
$numRes = mysql_num_rows( $result );
if ( $numRes < 1 )
{	mysql_close( $link );
	die;
}
if ( $debug ) printf( "num products on order=%d<br>", $numRes );
# we're cleared to go
#-------------------------------------------------------------------------------+
#								get products from order							|
#-------------------------------------------------------------------------------+
$msg = "";									# contents of the BOM file
$shortVer = $osVer[0];						# is this definitive?
if ( $useSantaFe )
{	$dirSeparator = "\\";
	$rootDir = "c:\\\\htdocs\\";
}
else										# Linux
{	$dirSeparator = "/";
	$rootDir = "/home/pnmxcom/public_html/";				# BlueHost doesn't base <htdocs> at /
}
$suppliesDir = $rootDir . "Install_Process" . $dirSeparator . "Supplies" . $dirSeparator;
$mimeDir     = $rootDir . "Install_Process" . $dirSeparator . "mime" . $dirSeparator;
#  PpcInstall (display name == download name)
AddToBOM( $suppliesDir, $mimeDir, $shortVer, 'PpcInstall', 'PpcInstall', $msg, $aggSize );
# we're not going to count PpcInstall's size as part of memory required
$aggSize = 0;								# reset aggSize to eliminate the effect of PpcInstall

$nPPCproducts = 0;							# number of software products
for ( $ii = 0; $ii < $numRes; $ii++ )
{	$row = mysql_fetch_assoc( $result );
	if ( $row === false )
	{	printf( "fetch_assoc --> false for ii = %d<br>", $ii );
		continue;
	}
	$productid = $row['productid'];
	$prodName = $row['product'];
#	$amount = $row['amount'];				# for subscriptions
	$crpName = $productName . '. crp ';
	if ( $debug )
		printf( "AddToBOM: productid=%d<br>", $productid );
	switch ( $productid )
	{	case 1:					# NillaHedge (display name == download name)
			AddToBOM( $suppliesDir, $mimeDir, $shortVer, $prodName, $prodName, $msg, $aggSize );
			$nPPCproducts++;
			break;
		case 2:					# BondManager (display name == download name)
			AddToBOM( $suppliesDir, $mimeDir, $shortVer, $prodName, $prodName, $msg, $aggSize );
			$nPPCproducts++;
			break;
		case 3:					# StrategyExplorer (display name == download name)
			AddToBOM( $suppliesDir, $mimeDir, $shortVer, $prodName, $prodName, $msg, $aggSize );
			$nPPCproducts++;
			break;
		case 4:					# YieldCurveFitter (display name == download name)
			AddToBOM( $suppliesDir, $mimeDir, $shortVer, $prodName, $prodName, $msg, $aggSize );
			$nPPCproducts++;
			break;
		case 5:					# 5 - an (NH) OptionChainSubscription (how many years?)
			$mimeFile = $mimeDir . "OCR.mime";
			$mime = file_get_contents( $mimeFile );		# ckiv to decrypt the registry { key, value } pair
			$msg .= "OptionChains $mime\n";
			break;
		case 6:					# 6 - a (YCF) T-Bill Rate Subscription (how many years?)
			$mimeFile = $mimeDir . "TBR.mime";
			$mime = file_get_contents( $mimeFile );		# ckiv to decrypt the registry { key, value } pair
			$msg .= "TBillRates $mime\n";
			break;
		default:
			break;
	}
}
# add any necessary MFC8.0 DLLs to the BOM
if ( $nPPCproducts > 0  &&  $shortVer > '3'  &&  $shortVer < '6' )
{	# PPC02 ('3') doesn't need libraries, but
	# PPC03 ('4') and WM5 ('5') targets need MFC8.0 DLLs
	AddToBOM( $suppliesDir, $mimeDir, $shortVer, 'mfcDLLs', 'DLLs', $msg, $aggSize );	# add WM5_DLLs.CAB (encrypted as .crp)
}

# provide the current location of the encrypted binaries
$msg = "$downloadHost $aggSize\n" . $msg;
$msgLen = strlen( $msg );
# everything to be included in the response is now in $msg
if ( $debug )
	printf( "BOM...<br>%s<br>", $msg );
#-------------------------------------------------------------------------------+
#						log 'download' in bom_requests							|
#-------------------------------------------------------------------------------+
$query = "UPDATE pnmx_bom_requests SET status = 'download' WHERE id = '$requestid'";
if ( $debug ) printf( "query=$query<br>" );
$result = mysql_query( $query )	or die( "Requests U1 failed: " . mysql_error() );
#-------------------------------------------------------------------------------+
#							enrypt the BOM response								|
#-------------------------------------------------------------------------------+
$srcFile = $ranName . ".d";
$fp = fopen( $srcFile, "w" );
if ( $fp == NULL )
{   if ( $debug )
		printf( "Couldn't open(w) %s<br>", $srcFile );
	die();
}
$bytesWritten = fwrite( $fp, $msg, $msgLen );
if ( $bytesWritten != $msgLen )
{	if ( $debug )
		printf( "wrote %d bytes to %s, should have been=%d<br>", $bytesWritten, $srcFile, $msgLen );
	die();
}
fclose( $fp );

$tgtFile = $ranName . ".e";
$lastline = system( "../cgi-bin/rj e ./cfg/BOMresp.cfg $srcFile $tgtFile" );
if ( $lastLine != "" )
{	if ( $debug )
		printf( "system(rj) returned...<br>%s<br>", $lastLine );
	die();
}
# echo the crypt file to the browser stream
passthru( "cat $tgtFile" );

# cleanup temp files
$lastline = system( "rm $srcFile $tgtFile" );
if ( $lastLine != "" )
{	if ( $debug )
		printf( "system(rm) returned...<br>%s<br>", $lastLine );
	die();
}
#-------------------------------------------------------------------------------+
#						log 'complete' in download_requests						|
#-------------------------------------------------------------------------------+
$query = "UPDATE pnmx_bom_requests SET status = 'complete' WHERE id = '$requestid'";
if ( $debug ) printf( "query=$query<br>" );
$result = mysql_query( $query ) or die( "Requests U2 failed: " . mysql_error() );
mysql_close( $link );
?>

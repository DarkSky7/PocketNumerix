<?php
if ( !defined('XCART_START') ) { header("Location: ../"); die("Access denied"); }

function NumToBase62( $num )
{		# convert non-negative integers to base 62 strings
#    global $debug;
	$num = abs( $num );
    if ( $debug )
        printf( "NumToBase62:num=%d<br>", $num );
	$str = "";
	do
	{	$quo = intval( $num / 62 );
        if ( $debug )
            printf( "quo=%d<br>", $quo );
		$rem = $num - (62 * $quo);
        if ( $debug )
            printf( "rem=%d<br>", $rem );

		if ( $rem < 10 )
			$str .= chr( $rem + 48 );			# -0 + 48 ('0')
		else if ( 10 <= $rem  &&  $rem < 36 )
			$str .= chr( $rem + 87 );			# -10 + 97 ('a')
		else									# hopefully,  36 <= $num  &&  $num < 62
			$str .= chr( $rem  + 29 );			# -36 + 65 ('A')
		$num = $quo;
	} while ( $num > 0 );
    if ( $debug )
        printf( "NumToBase62:str='%s'<br>", $str );
	return	$str;
}			# NumToBase62()
#--------------------------------------------------------------------------------------------
function MakeRanName()
{	$rndy = mt_rand();
	$n1 = NumToBase62( $rndy );

	$rndy = mt_rand();
	$n2 = NumToBase62( $rndy );

	$rndy = mt_rand();
	$n3 = NumToBase62( $rndy );

	$rndy = mt_rand();
	$n4 = NumToBase62( $rndy );

	return	$n1 . $n2 . $n3 . $n4;
}			# MakeRanName()
#--------------------------------------------------------------------------------------------
function make_invoice_header( $installKeyDir, $orderid )
{	# returns the filename containing the invoice header unless something failed, then false
	$useBrowser = false;			# running directly in the browser, so we have to open and close the database ourselves
	$debug = false;					# write debug info to "debug_info.html" in the same directory as this script
	$useSantaFe = false;			# defines the directory separator string appropriate to the platform

	$tgtFile = $installKeyDir . $orderid . ".dat";
	
	if ( $debug )
	{	$fp_dbg = fopen( "make_invoice_header_debug.txt", "w" );
		if ( $fp_dbg ) fprintf( $fp_dbg, "%s\r\n", $tgtFile );
	}
	if ( file_exists($tgtFile) )
		return	$tgtFile;			# do not recrypt what we already know

	if ( $fp_dbg ) fprintf( $fp_dbg, "entered make_invoice_header\r\n" );
	if ( $useBrowser )
	{	# connect with database and extract order info
		$sql_host ='localhost';
		$sql_db ='pnmxss';
		$sql_user ='SSordersReader';
		$sql_password ='kn2Gw7xT13a0pI5';
	
		$link = mysql_connect( $sql_host, $sql_user, $sql_password )
			or die( 'Could not connect: ' . mysql_error() );
	
		mysql_select_db( $sql_db )
			or die('Could not select database');
	}

	# Given an orderid, get the customer's name & email address
	$query = "SELECT firstname, lastname, email, date FROM xcart_orders WHERE orderid = '$orderid'";
	$result = mysql_query( $query )
		or die( 'Order Query failed: ' . mysql_error() );
	$numRes = mysql_num_rows( $result );
	if ( $numRes < 1 )
	{	if ( $useBrowser )
			mysql_close( $link );
		return	false;
	}
	if ( $fp_dbg ) fprintf( $fp_dbg, "numRes=%d\r\n", $numRes );
	$row = mysql_fetch_assoc( $result );

	$dateString = date( "Y-M-d", $row['date'] );			# in 2007-Mar-05 format
	if ( $fp_dbg ) fprintf( $fp_dbg, "%s\r\n", $dateString );
	$clearText = sprintf( "%s %s %s %d %s",
		$row['firstname'], $row['lastname'], $row['email'], $orderid, $dateString );
	mysql_free_result( $result );
	if ( $fp_dbg ) fprintf( $fp_dbg, "ClearText...\r\n%s\r\n", $clearText );
	#-------------------------------------------------------------------------------+
	#								encrypt the clearText							|
	#-------------------------------------------------------------------------------+
	# this function is called from <htdocs>/Store/include/func/func.order.php, so
	#    the tmp directory is located in ../../../Install_Process/tmp
	$tmpFile = "../../../Install_Process/tmp/" . MakeRanName() . ".z";
	$fp = fopen( $tmpFile, "w" );
	if ( $fp == NULL )
	{   if ( $debug )
			printf( "Couldn't open(w) %s<br>", $tmpFile );
		die();
	}

	$slen = strlen( $clearText );
	$bytesWritten = fwrite( $fp, $clearText, $slen );
	if ( $bytesWritten != $slen )
	{	if ( $debug )
			printf( "wrote %d bytes to %s, should have been=%d<br>", $bytesWritten, $tmpFile, $slen );
		die();
	}
	fclose( $fp );
	
	$lastline = system( "../../../cgi-bin/rj e ../../../InstallProcess/cfg/token.cfg $tmpFile $tgtFile" );  
	if ( $lastLine != "" )
	{	if ( $debug )
			printf( "system(rc) returned...<br>%s<br>", $lastLine );
		die();
	}

	# cleanup temp file
	$lastline = system( "rm $tmpFile" );
	if ( $lastLine != "" )
	{	if ( $debug )
			printf( "system(rm) returned...<br>%s<br>", $lastLine );
		die();
	}

	if ( $useBrowser )
		mysql_close( $link );
	if ( $fp_dbg )
		fclose( $fp_dbg );

	return  $tgtFile;									# was returning $cryptText, now just the filename
}
#--------------------------------------------------------------------------------------------
# only needed when running in the browser
function my_parse_str( $str, &$arr )
{	# work son strings of the form "?var1=<value1>?var2=<value2>..."
	$debug = false;
	# works like parse_str(), but doesn't substitute spaces for '+' signs
	$slen = strlen( $str );
	if ( $debug )
		printf( "<br>slen=%d<br>", $slen );
	$arr = array();							# the return value
	$nn = 0;
	while ( $nn < $slen )
	{
		if ( $debug )
			printf( "nn=%d<br>", $nn );
		$jj = stripos( $str, "?", $nn );
		if ( $jj === FALSE )
			break;
		$jj++;								# just past the '?'

		if ( $debug )
			printf( "jj=%d<br>", $jj );
		$kk = stripos( $str, "=", $jj );
		if ( $kk === FALSE)
			break;

		if ( $debug )
			printf( "kk=%d<br>", $kk );
		$len = $kk - $jj;

		if ( $debug )
		{	printf( "len=%d<br>...key chars: ", $len );
			for ( $ii = $jj; $ii < $kk; $ii++ )
				printf( "%s", $str[$ii] );
		}
		$key = substr( $str, $jj, $len );
		if ( $debug )
			printf( "<br>key=%s<br>", $key );

		# now for the value
		$mm = $kk + 1;					# just past the '='
		if ( $debug )
			printf( "mm=%d<br>", $mm );
		$nn = stripos( $str, "?", $mm );
		if ( $debug )
			printf( "nn=%d<br>", $nn );
		if ( $nn === FALSE )
		{	# no more '?'s in $str
			$val = substr( $str, $mm );			# to EOS
			if ( $debug )
				printf( "...0_val=%d<br>", $val );
			$nn = $slen;				# terminates the while
		}
		else						# there are more keys
		{	$len = $nn - $mm;
			if ( $debug )
				printf( "len=%d<br>", $len );
			$val = substr( $str, $mm, $len );
			if ( $debug )
				printf( "...1_val=%d<br>", $val );
		}
		$arr[$key] = $val;
	}
}
#--------------------------------------------------------------------------------------------
# if testing in a browser, uncomment the following...
$debug = 0;
if ( $debug )
{	# put back the initial "?"
	$queryString = "?" . $_SERVER[ 'QUERY_STRING' ];
	echo "queryString='$queryString'<br>";
	# expecting to find:  '?in=<orderid>'
#	include_once( "my_parse_str.php" );
	my_parse_str( $queryString, $paramsArr );
	$orderid = $paramsArr['in'];
	echo "<br>orderid=$orderid<br>";
	$fname = make_invoice_header( "/home/pnmxcom/public_html/Install_Process/attach/", $orderid );
	echo "fname='$fname'<br>";
}
?>

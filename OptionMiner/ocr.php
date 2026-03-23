<?php
include_once( "my_parse_str.php" );
include_once( "TradingDate.php" );
include_once( "GetYahooOptions.php" );
# include_once( "isDaylightSavingsTime.php" );

function MakeOptionXML( $optRow, $optPriceRow )
{		# option definition information
	$optXML  = "<option>";
	$optXML .= "<sym>$optRow[optionSymbol]</sym>";				# symbol
	$optXML .= " <strk>$optRow[strikePrice]</strk>";			# strike price
		# option price information
	$optXML .= " <last>$optPriceRow[last]</last>";
	$optXML .= " <chng>$optPriceRow[oneDayChange]</chng>";

	if ( $optPriceRow['bid'] == -1 )
		$optXML .= " <bid>N/A</bid>";
	else
		$optXML .= " <bid>$optPriceRow[bid]</bid>";

	if ( $optPriceRow['ask'] == -1 )
		$optXML .= " <ask>N/A</ask>";
	else
		$optXML .= " <ask>$optPriceRow[ask]</ask>";

	$optXML .= " <vol>$optPriceRow[volume]</vol>";
	$optXML .= " <opnInt>$optPriceRow[openInterest]</opnInt>";
	$optXML .= "</option><br>\r\n";
	return	$optXML;
}
#################################################################################

$debug = false;
# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# Is this request from a registered device?
# the following works on Windows:
#		echo "queryString=$QUERY_STRING<br>\r\n";
# but the Linux version has to be:
# $queryString = $HTTP_SERVER_VARS['QUERY_STRING'];
$queryString = $_SERVER[ 'QUERY_STRING' ];				# works
if ( $debug )
	printf( "OCR: queryString='%s'<br>\r\n", $queryString );
if ( strlen( $queryString ) < 3 )
	die( "x1" );

# expecting to find:  ds parameter which ultimately contains: os pn dv uu hu
$pos = strpos( $queryString, "ds=" );
$ds = substr( $queryString, $pos+3 );			# chop off 'ds=', keep the rest, e.g.
if ( $debug )
	printf( "ds='%s'<br>/r/n", $ds );

$dslen = strlen( $ds );
if ( $debug )
	printf( "dslen=%d<br>/r/n", $dslen );
if ( $dslen == 0 )
	die( "x2" );
#---------------------------------------------------------------------------+
#							decode the ds argument							|
#---------------------------------------------------------------------------+
include_once( "MakeRanName.php" );
$ranName = "./tmp/" . MakeRanName();
if ( $debug )
	printf( "ranName=%s<br>/r/n", $ranName );
$dsFile = $ranName . ".a";

$fp = fopen( $dsFile, "w" );
if ( $fp == NULL )
{   if ( $debug )
		printf( "Couldn't open(w) '%s'<br>/r/n", $dsFile );
	die( "x3" );
}
$bytesWritten = fwrite( $fp, $ds, $dslen );
if ( $debug )
	printf( "wrote %d (should be %d) bytes to '%s'<br>/r/n", $bytesWritten, $dslen, $dsFile );
if ( $bytesWritten != $dslen )
	die( "x4" );
fclose( $fp );

$crpFile = $ranName . ".b";
$lastline = system( "./cgi-bin/med d $dsFile $crpFile" );
if ( $lastLine != "" )
{   if ( $debug )
		printf( "system(med) returned...<br>%s<br>/r/n", $lastLine );
	die( "x5" );
}
#---------------------------------------------------------------------------+
#							decrypt param string	 						|
#---------------------------------------------------------------------------+
$clrFile = $ranName . ".c";
$lastline = system( "./cgi-bin/rc d ./cfg/OCRds.cfg $crpFile $clrFile" );
if ( $lastLine != "" )
{   if ( $debug )
		printf( "system(rc) returned...<br>%s<br>/r/n", $lastLine );
	die( "x6" );
}
$decryptParams = file_get_contents( $clrFile );
if ( $debug )
	printf( "decryptParams=%s<br>/r/n", $decryptParams );

# cleanup temp files
$lastline = system( "rm $dsFile $crpFile $clrFile" );
if ( $lastLine != "" )
{   if ( $debug )
		printf( "system(rm) returned...<br>%s<br>/r/n", $lastLine );
	die( "x7" );
}
#-------------------------------------------------------------------------------+
#							parse the decrypted params							|
#-------------------------------------------------------------------------------+
# $paramsArr should contain the results of parsing a string generated with:
#    "?os=%s?dv=%d?uu=%s?hu=%s", thus: os, dv, uu, hu

my_parse_str( $decryptParams, $paramsArr );
if ( $debug )
{	foreach ( $paramsArr as $key => $val )
		printf( "%s => %s<br>/r/n", $key, $val );
}
if ( count( $paramsArr ) < 4 )
	die( "x8" );

# gather intel on the request
$dateTimeFormat = "Y-m-d H:i:s";
$accessDateTime = date( $dateTimeFormat );	if ( $debug ) echo "OCR: accessDateTime=$accessDateTime<br>/r/n";
$ipAddress = $_SERVER['REMOTE_ADDR'];		if ( $debug ) echo "OCR: ipAddress=$ipAddress<br>/r/n";
$osVer = $paramsArr['os'];					if ( $debug ) echo "OCR: osVer=$osVer<br>/r/n";			# one thing that identifies the client device
# $deviceid = $paramsArr['dv'];				if ( $debug ) echo "OCR: deviceid=$deviceid<br>/r/n";	# the hardware deviceid (per PpcInstall)
$uuid = $paramsArr['uu'];					if ( $debug ) echo "OCR: uuid=$uuid<br>/r/n";			# UUID for the device (could be "0")
$huid = $paramsArr['hu'];					if ( $debug ) echo "OCR: huid=$huid<br>/r/n";			# hashed UUID for this client application that uses the subscription
$stk = $paramsArr['stk'];					if ( $debug ) echo "OCR: stk=$stk<br>/r/n";				# extra param assoc. with OCR requests
$exp = $paramsArr['exp'];					if ( $debug ) echo "OCR: exp=$exp<br>/r/n";				# ditto
#-------------------------------------------------------------------------------+
#						setup database access and authenticate					|
#-------------------------------------------------------------------------------+
$sql_host = 'localhost';
if ( $useSantaFe )
{	$sql_db = 'FiveInchStilettos';
	$sql_user = 'FiveInchStiletto';
	$sql_password = 'Pm3Vb7r9dL65wz';
}
else		# on HostMonster.com
{	$sql_db = 'optimal1_DeadPresidents';
	$sql_user = 'optimal1_120mmSt';
	$sql_password = 'rqKeI8nPdO9mB3z';
}

$link = mysql_connect( $sql_host, $sql_user, $sql_password ) or die( "x9: Couldn't connect: " .  mysql_error() );
if ( ! mysql_select_db( $sql_db, $link ) )
	die( "x10" );		# 	die( "Couldn't select database" );

# log the OCR request in the accessLog
$query = "INSERT INTO accessLog (ipAddress, accessDateTime, page, status) VALUES ('$ipAddress', '$accessDateTime', 'OC', 'request')";
if ( $debug )
	printf( "query=%s<br>/r/n", $query );
$result = mysql_query( $query )	or die( "x11" );

#------------------------

# retrieve the id of the accessLog record
$query = "SELECT id FROM accessLog WHERE ipAddress = '$ipAddress' AND accessDateTime = '$accessDateTime' AND page = 'OC'";
if ( $debug )
	printf( "query=%s<br>/r/n", $query );
$result = mysql_query( $query )	or die( "x12" );
$numRows = mysql_num_rows( $result );
if ( $debug )
	printf( "numRows = %d<br>/r/n", $numRows );
if ( $numRows < 1 )
	die( "x13" );
$row = mysql_fetch_assoc( $result );
$accessLogId = $row['id'];										# have accessLogId
#-------------------------------------------------------------------------------+
#								device authentication							|
#-------------------------------------------------------------------------------+
if ( $osVer[0] == '5' )
#	$query = "SELECT devicesid FROM pnmx_devices WHERE osVer = '$osVer' AND deviceid = '$deviceid' AND uuid = '$uuid' AND nh_huid = '$huid'";
	$query = "SELECT devicesid FROM pnmx_devices WHERE osVer = '$osVer' AND uuid = '$uuid' AND nh_huid = '$huid'";
else		# presumably $osVer[0] is a '3' or a '4'
#	$query = "SELECT devicesid FROM pnmx_devices WHERE osVer = '$osVer' AND deviceid = '$deviceid' AND uuid = '$uuid'";
	$query = "SELECT devicesid FROM pnmx_devices WHERE osVer = '$osVer' AND uuid = '$uuid'";
if ( $debug )
	echo "OCR: query = $query<br>/r/n";

$result = mysql_query( $query )	or die( "x14" );
#		or die( 'Query failed: ' . mysql_error() );
$numRows = mysql_num_rows( $result );
if ( $debug )
	echo "OCR: numRows = $numRows<br>/r/n";
if ( $numRows < 1 )
	die( "x15" );
$row = mysql_fetch_assoc( $result );
$devicesid = $row['devicesid'];									# have devicesid
if ( $debug )
	echo "OCR: Device authenticated, devicesid=$devicesid<br>\r\n";
$query = "UPDATE accessLog SET devicesid = '$devicesid', status = 'deviceOk' WHERE id = '$accessLogId'";
$result = mysql_query( $query )	or die( "x16" );
#-------------------------------------------------------------------------------+
#							subscription verification							|
#-------------------------------------------------------------------------------+
$query = "SELECT startDate, endDate FROM ocr_subscriptions WHERE devicesid = '$devicesid' ORDER BY endDate DESC";
if ( $debug )
	echo "OCR: query=$query<br>\r\n";
$result = mysql_query( $query ) or die( "x17" );
#		or die( 'Query failed: ' . mysql_error() );
$numRows = mysql_num_rows( $result );
if ( $debug )
	echo "OCR: Device($devicesid) has $numRows OptionChain subscription record(s)...<br>\r\n";

$today = date( "Y-m-d" );			# returns a formatted string
if ( $debug )
	echo "OCR: today=$today<br>\r\n";
	
$activeSubs = false;
for ( $ii = 0; $ii < $numRows; $ii++ )
{	$row = mysql_fetch_assoc( $result );
	$startDate = $row['startDate'];
	$endDate = $row['endDate'];
	if ( $debug )
		echo "OCR: startDate=$startDate, endDate=$endDate<br>\r\n";
	$activeSubs = ( $startDate <= $today  &&  $today <= $endDate );
	if ( $activeSubs )
		break;
}

# log subscription status
$status = ( $activeSubs )  ?  "active"  :  "expired";
$query = "UPDATE accessLog SET status = '$status' WHERE id = '$accessLogId'";
mysql_query( $query ) or die( "x18" );			# couldn't update accessLog to 'active' or 'expired'
if ( ! $activeSubs ) die( "x19" );				# expired, so we're done

# subscription is active
if ( $debug )
	echo "OCR: Device($devicesid) has an active OptionChain subscription.<br>\r\n";
#-------------------------------------------------------------------------------+
#								Option Chain retrieval							|
#-------------------------------------------------------------------------------+
	# do we have a stock record?
$timeZoneOffset = 0;							# will eventually have to figure this out
$timeNow = getdate();							# an associative array

$ymd = date( "Y-m-d" );							# a date string, e.g. 2007-03-15, for the Ides of March
if ( $debug )
	echo "OCR: ymd=$ymd<br>\r\n";

	# need the stock's id
$stockSymbol = strtoupper( $stk );
$stockQuery = "SELECT * FROM stocks WHERE stockSymbol = '$stockSymbol'";
if ( $debug )
	echo "OCR: stockQuery=$stockQuery<br>\r\n";
$stockResult = mysql_query( $stockQuery ) or die( "x20: Stocks Q failed: " . mysql_error() );

	# check out all options on that stock with the given expiry (if any)
$hrB4dtSt = OneHourBeforeOrLastClose( $timeZoneOffset );			# betting that we'll need it inside the if()
$numStockResults = mysql_num_rows( $stockResult );
if ( $debug )
	echo "OCR: numStockResults=$numStockResults<br>\r\n";
$needYahoo = true;
$stkId = NULL;									# a sentinal value.  if we find a stock, this will become valid
$companyName = "";								# we'll also make this valid
$stockPrice = 0;								# and this
if ( $numStockResults > 0 )
{		# we found a stock record for the symbol
	$stkRow = mysql_fetch_assoc( $stockResult );
	$stkId = $stkRow['id'];
	if ( $debug )
		echo "OCR: stkId=$stkId<br>\r\n";
	$companyName = $stkRow['companyName'];
	if ( $debug )
		echo "OCR: companyName=$companyName<br>\r\n";

	if ( $exp == "" )
	{		# the OCR dialog was just opened or the stock symbol has changed in the OCR dialog
		$exp = ActiveThirdFriday( $timeNow );				// now a mySQL date string	2006-05-19
	}
	else
	{		# the request is from the list of expirations in the OCR dialog
			# need to onvert incoming YYYY-MM format --> YYYY-MM-DD (mySQL format)
		sscanf( $exp, "%04d-%02d", $expYr, $expMo );
		$expMonths = $expYr * 12 + $expMo;
		$nowMonths = $timeNow['year'] * 12 + $timeNow['mon'];
		if ( $expMonths < $nowMonths )
			$exp = ActiveThirdFriday( $timeNow );
		else
			$exp = MakeExpiry( $expYr, $expMo );		// a mySQL date string including day of month (third Friday)
	}
	if ( $debug ) echo "OCR: exp=$exp<br>\r\n";

		# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -	
	$optQuery = "SELECT * FROM options WHERE stock_id = '$stkId' AND expiryDate = '$exp'";
	if ( $debug ) echo "OCR: optQuery='$optQuery'<br>\r\n";
	$optResult = mysql_query( $optQuery ) or die( "x21: Option Query failed: " . mysql_error() );

	$numOpts = mysql_num_rows( $optResult );
	if ( $debug ) echo "OCR: numOpts=$numOpts<br>\r\n";
	if ( $numOpts > 0 )
	{		# found at least one option dependent on the given underlying
		$optRow = mysql_fetch_assoc( $optResult );
		if ( $optRow === FALSE )
			continue;
		$optId = $optRow['id'];
		if ( $debug )
			echo "OCR: optId=$optId<br>\r\n";

			# see if the first option's prices are current (ascecding is the default ordering)
		$optPriceQuery = "SELECT * FROM optionPrices WHERE id = '$optId' AND priceDateTime >= '$hrB4dtSt' ORDER BY priceDateTime DESC";
		if ( $debug )
			echo "OCR: optPriceQuery='$optPriceQuery'<br>\r\n";
		$optPriceResult = mysql_query( $optPriceQuery ) or die( "x22: Option Price Query failed: " . mysql_error() );

		$numOptPrices = mysql_num_rows( $optPriceResult );
		if ( $debug )
			echo "OCR: numOptPrices=$numOptPrices<br>\r\n";
		if ( $numOptPrices > 0 )
		{
			if ( $debug )
				echo "OCR: Sourcing from server cache (not asking Yahoo)!<br>\r\n";
				# option prices are current, don't need to ask Yahoo!

				# postpone output of Company Name until we have an expiration string
			$stockPriceQuery = "SELECT * FROM stockPrices WHERE id = '$stkId' AND priceDateTime >= '$hrB4dtSt' ORDER BY priceDateTime DESC";
			if ( $debug )
				echo "OCR: stockPriceQuery='$stockPriceQuery'<br>\r\n";
			$stockPriceResult = mysql_query( $stockPriceQuery )	or die( "x23: Stock Price Query failed: " . mysql_error() );
			$numStockPriceResults = mysql_num_rows( $stockPriceResult );
			if ( $debug )
				echo "OCR: numStockPriceResults=$numStockPriceResults<br>\r\n";
			if ( $numStockPriceResults > 0 )
			{		# postpone output of Stock Price until after we have an expiration string
				$stockPriceRow = mysql_fetch_assoc( $stockPriceResult );
				$stockPrice = $stockPriceRow['last'];
			}
			mysql_free_result( $stockPriceResult );

				# now that expiration is in hand, we can let the title out of the bag
			$mmmYY = MakeMmmYYFromMySqlDate( $exp );
			echo "<title>$stockSymbol: Options for $companyName expiring in $mmmYY</title>\r\n";
			echo "<CoName>$companyName</CoName><br>\r\n";
			echo "<StkPrc>$stockPrice</StkPrc><br>\r\n";
			echo "<exp>$mmmYY</exp><br>\r\n";						// "MmmYY" format

				# what option expiration dates are available?
			$expiryQuery = "SELECT * FROM optionExpiry WHERE stock_id = '$stkId' ORDER BY expDate";
			if ( $debug )
				echo "OCR: expiryQuery='$expiryQuery'<br>\r\n";
			$expDateResult = mysql_query( $expiryQuery );
			$numExpDateRows = mysql_num_rows( $expDateResult );
			if ( $debug )
				echo "OCR: numExpDateRows=$numExpDateRows<br>\r\n";
			$Dates = "";
			$obsoleteExpDates = 0;
			for ( $ii = 0; $ii < $numExpDateRows; $ii++ )
			{	$expDateRow = mysql_fetch_assoc( $expDateResult );
				$expDate = $expDateRow['expDate'];
				if ( $debug )
					echo "OCR: expDate=$expDate<br>\r\n";
				if ( $expDate >= $ymd )
				{
					if ( $debug )
						echo "OCR: expDate=$expDate > ymd=$ymd<br>\r\n";
					if ( $expDate != $exp )				// $exp has it's own special spot
					{	$mmmYY = MakeMmmYYFromMySqlDate( $expDate );
						if ( $debug )
							echo "OCR: mmmYY=$mmmYY<br>\r\n";
						$Dates .= "<date>$mmmYY</date><br>\r\n";
					}
					$obsoleteExpDates += 1;
				}
			}
			echo "<Dates><br>\r\n$Dates</Dates><br>\r\n";
#				mysql_free_result( $expiryDateResult );				// supposedly not a valid MySQL result resource

				# get rid of obsolete option expiration dates for this stock (all at once)
			if ( $obsoleteExpDates > 0 )
			{	$expDeleteQuery = "DELETE LOW_PRIORITY FROM optionExpiry WHERE stock_id = '$stkId' AND expDate < '$ymd'";
				if ( $debug )
					echo "OCR: expDeleteQuery='$expDeleteQuery'<br>\r\n";
				$expDeleteResult = mysql_query( $expDeleteQuery );
			}

				# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
				# collect puts and calls
			$optPriceRow = mysql_fetch_assoc( $optPriceResult );
			$putsXML = "<Puts><br>\r\n";
			$callsXML = "<Calls><br>\r\n";
			$numCalls = 0;
			$numPuts = 0;
			if ( $optRow['isCall'] )
			{	$callsXML .= MakeOptionXML( $optRow, $optPriceRow );
				$numCalls++;
			}
			else
			{	$putsXML .= MakeOptionXML( $optRow, $optPriceRow );
				$numPuts++;
			}
			mysql_free_result( $optPriceResult );

				# now for the rest
			for ( $ii = 1; $ii < $numOpts; $ii++ )
			{
				$optRow = mysql_fetch_assoc( $optResult );
				$optPriceQuery = "SELECT * FROM optionPrices WHERE id = '$optRow[id]' AND priceDateTime >= '$hrBrdtSt' ORDER BY priceDateTime DESC";
				if ( $debug )
					echo "OCR: $optPriceQuery<br>\r\n";
				$optPriceResult = mysql_query( $optPriceQuery ) or die( "x24: Option Price Query failed: " . mysql_error() );

				$optPriceRow = mysql_fetch_assoc( $optPriceResult );
				if ( $optRow['isCall'] )
				{	$callsXML .= MakeOptionXML( $optRow, $optPriceRow );
					$numCalls++;
				}
				else
				{	$putsXML .= MakeOptionXML( $optRow, $optPriceRow );
					$numPuts++;
				}
				mysql_free_result( $optPriceResult );
			}
				# if there were calls, write them out now
			if ( $numCalls > 0 )
				echo "$callsXML</Calls><br>\r\n";
				# if there were puts, write them out now
			if ( $numCalls > 0 )
				echo "$putsXML</Puts><br>\r\n";

			$numOpts = $numCalls + $numPuts;
			$needYahoo = false;
		}				// option prices were found
	}					// options were found
	mysql_free_result( $optResult );
}						// stocks were found
mysql_free_result( $stockResult );

if ( $needYahoo )
{		# ask yahoo!
	if ( $debug )
		printf( "Ask Yahoo! (stk=%s, exp=%s, stkId=%d, accessDateTime=%s)<br>\r\n",
               $stockSymbol, $exp, $stkId, $accessDateTime );         # $companyName could be empty
	if ( strlen( $exp ) > 7 )
		$exp = substr( $exp, 0, 7 );		// GetYahooOptions wants only the year and the numerical month
	$numOpts = GetYahooOptions( $stockSymbol, $exp, $stkId, $accessDateTime );
	if ( $debug )
		printf( "OCR: numOptions=%d<br>\r\n", $numOpts );
}
	# Log options delivered
$query = "UPDATE accessLog SET status = 'delivered' WHERE id = '$accessLogId'";
mysql_query( $query ) or die( "x25" );			# couldn't update accessLog to 'active' or 'expired'
?>

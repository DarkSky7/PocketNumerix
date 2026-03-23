<?php
include_once( 'GetContents.php' );
include_once( 'LoadLinks.php' );
include_once( 'LoadOptions.php' );
include_once( 'TradingDate.php' );

function GetYahooOptions( $stk, $exp, $stkId, $ymdhms )
{		# returns the number of options found on Yahoo!
		# $stk is a stock symbol
		# $exp is a requested expiration date in YYYY-MM (e.g. 2006-06) format
		# $stkId is the stock's database identifier
		# $ymdhms is a date/time string for today
	global $debug;
	if ( $debug )
		echo "GetYahooOptions: (stk=$stk, exp=$exp, stkId=$stkId, ymdhms=$ymdhms)<br>\r\n";
	$expStr = "";
	$slen = strlen( $exp );
	if ( $slen == 7 )
		$expStr = "&m=$exp";									# e.g. $exp = 2006-05 for May-2006 options
	$fileName = "/q/op?s=$stk$expStr";
	if ( $debug )
		echo "$fileName<br>\r\n";

#	$url = "http:#finance,yahoo.com/q/op?s=$stk$expStr";
#	echo "GetYahooOptions: requesting: $url<br>";				# debug only
#	$fileContents = file_get_contents( $fileName );				# complains that getaddrinfo() fails
#	if ( $debug )												# today's test case
#		$fileContents = file_get_contents( "intel_2007-07_options.htm" );
#	else

	$fileContents = GetContents( "finance.yahoo.com", 80, $fileName );
		# parse the URL requested
	$pos1 = strpos( $fileContents, "<title>" );					# pass up the preamble (up to "<title>")
	if ( $pos1 === FALSE )
		return	-1;
	$pos1 += 7;													# pass up "<title>"

	$pos2 = strpos( $fileContents, "</title>", $pos1 );
	if ( $pos2 === FALSE )
		return	-2;

	$title = substr( $fileContents, $pos1, $pos2 - $pos1 );
    if ( $debug )
        echo "GetYahooOptions: title='$title'<br>\r\n";
	$invalidTickerSymbol = "Invalid Ticker Symbol";
	$pos3 = strpos( $title, $invalidTickerSymbol );
    if ( $debug )
		echo "GetYahooOptions($invalidTickerSymbol): pos3=$pos3<br>\r\n";
	if ( $pos3 !== FALSE )
	{		# found "Invalid Ticker Symbol", so we're done
		echo "<title>$invalidTickerSymbol</title>\r\n";
		return	-3;
	}
    $noSuchURL = "404 Not Found";
	$pos3 = strpos( $title, $noSuchURL );
    if ( $debug )
		echo "GetYahooOptions($noSuchURL): pos3=$pos3<br>\r\n";
	if ( $pos3 !== FALSE )
	{		# found "404 Not Found", so we're done
        $mmmYY = MakeMmmYYFromMySqlDate( $exp );
        echo "<title>$stk: Options for Not Found expiring in $mmmYY</title>\r\n";
        echo "<CoName>Not Found</CoName><br>\r\n";
        echo "<StkPrc>N/A</StkPrc><br>\r\n";
        echo "<exp>$mmmYY</exp><br>\r\n";						// "MmmYY" format
        echo "<Dates></Dates><br>\r\n";
        echo "<Calls></Calls><br>\r\n";
        echo "<Puts></Puts><br>\r\n";
		return	-4;
	}

	$gecko = "indexOf('gecko')";						# a signpost
	$pos1 = strpos( $fileContents, $gecko, $pos1 );		# pass up the metadata
	if ( $pos1 === FALSE )
		return	-5;

	$pSym = "(" . strtoupper( $stk ) . ")";			            	# e.g. '(INTC)', another signpost
	$pos1 = strpos( $fileContents, $pSym, $pos1 );
	if ( $pos1 === FALSE )
		return	-6;
	$pos1 -= 64;								            		# rewind a little

	$pos1 = strpos( $fileContents, "<b>", $pos1 );
	if ( $pos1 === FALSE )
		return	-7;
	$pos1 += 3;										            	# pass up "<b>"

	$pos2 = strpos( $fileContents, "</b>", $pos1 );
	if ( $pos2 === FALSE )
		return	-8;

		# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		# get the company name
	$companyName = substr( $fileContents, $pos1, $pos2 - $pos1 );
	$pos3 = strpos( $companyName, $pSym );
	$companyName = substr( $companyName, 0, $pos3 );
	$companyName = trim( $companyName );
		# convert occurances of "&amp;" to "&" in the companyName
	$companyName = str_replace( "&amp;", "&", $companyName );
    if ( $debug )
        printf( "GetYahooOptions: companyName='%s', pos2=%d<br>/r/n", $companyName, $pos2 );

		# the following is being deferred until we have an expiration date from the file
		# we still don't have a title tag.  Insert one before the company name
#		echo "<title>$stk: Options for $companyName with expiration: $exp</title><br>\r\n";
#		echo "<CoName>$companyName</CoName><br>\r\n";				# XML output

		# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		# make sure the stock is in the database (and get it's id)
#		echo "GetYahooOptions: incoming stkId=$stkId<br>\r\n";
	if ( $stkId == NULL )
	{		# we never found a stock, so we'll Insert one now
		$stockQuery = "INSERT INTO stocks ( stockSymbol, companyName ) VALUES ( '$stk', '$companyName' )";
#			echo "GetYahooOptions: $stockQuery<br>\r\n";
		$stockResult = mysql_query( $stockQuery )
			or die ('Stock Insert failed: ' . mysql_error() );		# does not fail if the stock is already present
		$stockQuery = "SELECT * FROM stocks WHERE stockSymbol = '$stk'";
#			echo "GetYahooOptions: $stockQuery<br>\r\n";
		$stockResult = mysql_query( $stockQuery )
			or die ('Stock Query failed: ' . mysql_error() );		# does not fail if the stock is already present

		$stkRow = mysql_fetch_assoc( $stockResult );
		$stkId = $stkRow['id'];
#			echo "GetYahooOptions: outgoing stkId=$stkId<br>\r\n";
		mysql_free_result( $stockResult );
	}
    
		# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		# get the stock price
    $fileContentsPos = $pos2;                                       # remember this position within the file
	$str = substr( $fileContents, $pos2, 512 );                     # capture a subset of the file, $pos2 == $fileContentsPos
	$pos1 = strpos( $str, "<big><b>", 0 );                          # relative to the beginning of $str
	if ( $pos1 === FALSE )
	   $pos1 = strpos( $str, "<b><big>", $pos2 );
    if ( $pos1 === FALSE )
        return	-9;
	if ( $debug )
        printf( "GetYahooOptions: pos1=%d, buf=%s<br>/r/n", $pos1, $str );
	$pos1 += 8;														# pass up "<big><b>"

    $pos3 = strpos( $str, "span", $pos1 );
    if ( $pos3 === FALSE )
    {   # the original non-obfuscated approach
    	$pos2 = strpos( $str, "</b>", $pos1 );              # the beginning of the end of <big><b>...</b></big>
        $pos3 = strpos( $str, "</big>", $pos1 );
        if ( $pos3 < $pos2 )
            $pos2 = $pos3;
    }
    else
    {   # Yahoo's new obfuscated version (e.g. <big><b><span id="yfs_110_g">stkPrice</span></b></big>
        $pos3 += 4;                                         # length of "span"
        $pos1 = strpos( $str, ">", $pos3 );                 # end of the <span ...> tag
        if ( $pos1 !== FALSE )
        {   $pos1 += 1;
            $pos2 = strpos( $str, "</span>", $pos1 );
        }
    }
	if ( $pos2 === FALSE )
		return	-10;
	$stockPrice = substr( $str, $pos1, $pos2 - $pos1 );
		# the following output is being deferred until we have an expiration date from the file
	if ( $debug )
        printf( "GetYahooOptions: stockPrice=%s<br>/r/n", $stockPrice );
    # echo "<StkPrc>$stockPrice</StkPrc><br>\r\n";

		# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		# insert the current stock price
	$stockPriceQuery = "INSERT INTO stockPrices ( id, priceDateTime, last ) VALUES ( '$stkId', '$ymdhms', '$stockPrice' )";
#		echo "OCR: $stockPriceQuery<br>\r\n";
	$stockPriceResult = mysql_query( $stockPriceQuery )
		or die( 'Stock Price Insert failed: ' . mysql_error() );

		# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		# find the Expiration Date (other available expiration dates are written to the browser)
	$pos2 = $fileContentsPos + $pos2;       # $pos2 currently being the offset within the $fileContents which was extracted beginning at $fileContentsPos
	$pos1 = strpos( $fileContents, "View By Expiration:", $pos2 );
	if ( $pos1 === FALSE )
	{		# no options found
        $mmmYY = MakeMmmYYFromMySqlDate( $exp );
        echo "<title>$stockSymbol: Options for $companyName expiring in $mmmYY</title>\r\n";
        echo "<CoName>$companyName</CoName><br>\r\n";
        echo "<StkPrc>$stockPrice</StkPrc><br>\r\n";
        echo "<exp>$mmmYY</exp><br>\r\n";						            # "MmmYY" format
        echo "<Dates></Dates><br>\r\n";
        echo "<Calls></Calls><br>\r\n";
        echo "<Puts></Puts><br>\r\n";
		return	-11;
    }
    $pos1 += 19;														    # pass up "View By Expiration:"

	$pos2 = strpos( $fileContents, "<table", $pos1 );						# note that this is NOT "<table>"
	if ( $pos2 === FALSE )
		return	-12;
	$dateCache = substr( $fileContents, $pos1, $pos2 - $pos1 );
	if ( $debug )
		echo "dateCache = $dateCache<br>\r\n";								# debug only
		# LoadLinks does its own output to the browser, including:
		# <title>, <CoName>, <StkPrc>, <exp>, and <date> lines
		# LoadLinks returns the number of dates found (including $expiration)
	$expiration = "";						# we're going to overwrite this in LoadLinks
	$numDates = LoadLinks( $dateCache, $stk, $stkId, $companyName, $stockPrice, $expiration );
		# make a mySQL date out of  $expiration
#		echo "GetYahooOptions: expiration=$expiration<br>\r\n";
	$exp_ymd = MakeExpiryFromMmmYY( $expiration );
#		echo "GetYahooOptions: exp_ymd=$exp_ymd<br>\r\n";

		# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		# isolate the Call cache
	$pos1 = strpos( $fileContents, "CALL OPTIONS", $pos2 );				# beginning of Calls signpost
	if ( $pos1 === FALSE )
		return	-13;
	$pos1 += 12;														# pass up "CALL OPTIONS"

	$pos2 = strpos( $fileContents, "PUT OPTIONS", $pos1 );				# end of Calls signpost
	if ( $pos2 === FALSE )
		return	-14;
	$callOptionsSt = substr( $fileContents, $pos1, $pos2 - $pos1 );
#		if ( $debug )
#			echo "GetYahooOptions: callOptionSt=$callOptionSt<br>\r\n";
	echo "<Calls><br>\r\n";												# start of the calls

		# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		# parse and load the Calls
	$numCalls = LoadOptions( $callOptionsSt, $stkId, $ymdhms, $companyName, $exp_ymd, 1 );		# calls
	echo "</Calls><br>\r\n";											# end of the calls
	if ( $debug )
		echo "GetYahooOptions: numCalls=$numCalls<br>\r\n";

		# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		# isolate the Put cache
	$pos2 += 11;														# pass up "PUT OPTIONS"
	$pos1 = strpos( $fileContents, "Expand to Straddle View", $pos2 );	# end of Puts signpost
	if ( $pos1 === FALSE )
		return	-15;
	$putOptionsSt = substr( $fileContents, $pos2, $pos1 - $pos2 );
#		if ( $debug )
#			echo "GetYahooOptions: putOptionSt=$putOptionSt<br>\r\n";
	echo "<Puts><br>\r\n";												# start of the puts

		# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		# parse and load the Puts
	$numPuts = LoadOptions( $putOptionsSt, $stkId, $ymdhms, $companyName, $exp_ymd, 0 );		# not calls
	echo "</Puts><br>\r\n";												# end of the puts
	if ( $debug )
		echo "GetYahooOptions: numPuts=$numPuts<br>\r\n";
	return	$numCalls + $numPuts;
}
?>

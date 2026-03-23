<?php
include_once("TradingDate.php");            # DayOfWeek()
include_once("MoreTradingDate.php");		# DayAfter(), DayAfter(), YearBefore()
include_once("Tokens.php");                 # gettoken() and getline()

function YahooFetchHistoricalPrices( $sym, $yrFrom, $monFrom, $dayFrom )
{   # fetch one year of historical prices for the given stock symbol: $sym
    # beginning with date specified by $yrFrom-$monFrom-$dayFrom
    global $debug, $yahooLive;
    $dayTo = date("d");             # day of month (01 to 31)
    $monTo = date("n");             # numeric month (1 to 12)
    $yrTo  = date("Y");             # four digit representation of year
    $dow = DayOfWeek( $yrTo, $monTo, $dayTo );          # numeric 0 (Sunday) .. 6 (Saturday)
    DayAfter( $yrTo, $monTo, $dayTo, $dow );            # the toDate that yahoo wants
    
    # yahoo uses zero-based months for the a-argument, and one-based months for the d-argument
    $yahooMonFrom = $monFrom - 1;
    $arr = array( "&a=$yahooMonFrom", "&b=$dayFrom", "&c=$yrFrom", "&d=$monTo", "&e=$dayTo", "&f=$yrTo", "&g=d", "&ignore=.csv" );
    shuffle( $arr );            # randomize the arguments (hopefully sidestepping yahoo anti-bot protection)
    $args = "";                 # superfluous in PHP
    for ( $ii = 0; $ii < count($arr); $ii++ )
    {   $args .= "$arr[$ii]";
    }
    if ( $yahooLive )
    {   $url = "http://ichart.finance.yahoo.com/table.csv?s=$sym$args";
        $contents = file_get_contents( $url );
		# for some reason, this isn't working...
#        $fname = "YahooHistoricalPricesFor" . $sym . ".csv";
#        $fp = fopen( $fname, "w" );
#        fwrite( $fp, $contents );
#        fclose( $fp );
    }
    else
    {   $url = "YahooHistoricalPricesFor" . $sym . ".csv";
        $contents = file_get_contents( $url );
    }
    if ( $debug )
        echo "YahooFetchHistoricalPrices: url=$url<br>\r\n";
    return  $contents;
}           # YahooFetchHistoricalPrices()
#-------------------------------------------------------------------------------
function ParseCSV( $link, $stkId, $contents )
{   # parse $contents for Date, Open, High, Low, Close, Volume, Adj Close
    # the first line contains the column names
    # insert records into the database for each line of prices
    global $debug;
    $start = 0;
    $line = getline( $contents, $start );
    $cnt = 0;
    while ( $line )
    {   $pos = strpos( $line, "html>" );
        if ( $pos )
        {   $line = getline( $contents, $start );
            continue;
        }
        $pos = strpos( $line, "body>" );
        if ( $pos )
        {   $line = getline( $contents, $start );
            continue;
        }
        $pos = stripos( $line, "High" );
        if ( $pos )
        {   $line = getline( $contents, $start );
            continue;
        }
        
        $pos = stripos( $line, "Dividend" );
        if ( $pos )
        {   $line = getline( $contents, $start );
            continue;
        }
    
        # this is a line with price data on it
        
        # Date
        $pos = 0;
        $date = gettoken( $line, ",", $pos );
        if ( ! $date )
            break;
    
        # Open
        $open = gettoken( $line, ",", $pos );
        if ( ! $open )
            break;
    
        # High
        $high = gettoken( $line, ",", $pos );
        if ( ! $high )
            break;
    
        # Low
        $low = gettoken( $line, ",", $pos );
        if ( ! $low )
            break;
    
        # Close
        $close = gettoken( $line, ",", $pos );
        if ( ! $close )
            break;
        
        # Volume
        $volume = gettoken( $line, ",", $pos );
        if ( ! $volume )
            break;
    
        # AdjClose
        $adjClose = substr( $line, $pos );
        if ( ! $adjClose )
            break;
    
#        if ( $debug )
#            echo "($date, $open, $high, $low, $close, $volume, $adjClose)<br>\r\n";

        $query = "INSERT INTO HistoricalPrices ( stock_id, date, open, high, low, close, volume, adjClose ) VALUES ('$stkId', '$date', '$open', '$high', '$low', '$close', '$volume', '$adjClose' )";
#        if ( $debug )
#            echo "$query<br>\r\n";
        $result = mysql_query( $query, $link );
		if ( $result === FALSE )
			echo "ParseCSV: Couldn't insert prices for '$stkId' on '$date'<br>\r\n";

        # done with that line
        $line = getline( $contents, $start );
    }
}           # ParseCSV()
#-------------------------------------------------------------------------------
function	GetMostRecentPriceDate( $link, $stkId )
{	global $debug;
    $date = FALSE;
	$query = "SELECT date FROM HistoricalPrices WHERE stock_id = '$stkId' ORDER BY date DESC LIMIT 1";
    # if ( $debug )
    #    echo "query=$query<br>\r\n";
    $result = mysql_query( $query, $link ) or die( "q6: Couldn't select dates for $stkId" );
    $numRows = mysql_num_rows( $result );
    if ( $debug )
		echo "GetMostRecentPriceDate: numRows=$numRows<br>\r\n";
	if ( $numRows > 0 )
	{	$row = mysql_fetch_assoc( $result );
        $date = $row['date'];
		if ( $debug )
			echo "GetMostRecentPriceDate: date=$date<br>\r\n";
	}
    mysql_free_result( $result );
	return	$date;
}			# GetMostRecentPriceDate()
#-------------------------------------------------------------------------------
function	GetIssueId( $link, $symbol )
{	global $debug;
	$stkId = FALSE;
	$query = "SELECT symbol, id FROM Issues WHERE symbol = '$symbol'";
	if ( $debug )
		echo "GetStockId: query=$query<br>\r\n";
	$result = mysql_query( $query, $link ) or die( "GetStockId: Couldn't select issue '$symbol'" );
	$numRows = mysql_num_rows( $result );
	if ( $debug )
		echo "GetStockId: numrows=$numRows<br>\r\n";
	if ( $numRows > 0 )
	{   $row = mysql_fetch_assoc( $result );
		$stkId = $row['id'];
		if ( $debug )
			echo "GetStockId: stkId=$stkId<br>\r\n";
	}
	mysql_free_result( $result );
	return	$stkId;
}			# GetIssueId()
#-------------------------------------------------------------------------------
function	CacheHistoricalPrices( $link, $symbol, $issueName )
{   global $debug, $yahooLive;
	# returns the symbol's stock_id
	$stkId = GetIssueId( $link, $symbol );
	if ( $debug )
		echo "CacheHistoricalPrices: stkId=$stkId<br>\r\n";
	if ( $stkId === FALSE )
	{   # insert a row?
        $query = "INSERT INTO Issues ( symbol, name ) VALUES ('$symbol', '$issueName')";
        if ( $debug )
            echo "query=$query<br>\r\n";
        $result = mysql_query( $query ) or die( "q3: Couldn't insert issue '$symbol'" );		
		$stkId = GetIssueId( $link, $symbol );
	}
	$date = GetMostRecentPriceDate( $link, $stkId );	# MySQL format date string
	if ( $debug )
		echo "CacheHistoricalPrices: MostRecentPriceDate=$date<br>\r\n";

	if ( $date === FALSE )
    {   # no historical prices found, use six months previous to today
		$date = YearAgoDate( $yrFrom, $monFrom, $dayFrom );
		if ( $debug )
			echo "CacheHistoricalPrices: YearAgoDate=$date<br>\r\n";
	}
	else
	{   sscanf( $date, "%4d-%02d-%02d", $yrFrom, $monFrom, $dayFrom );
        $wday = DayOfWeek( $yrFrom, $monFrom, $dayFrom );
        DayAfter( $yrFrom, $monFrom, $dayFrom, $wday );
		if ( $debug )
			echo "CacheHistoricalPrices: DayAfter date=$date<br>\r\n";
	}
 	if ( $yahooLive )
	{	if ( $debug )
			echo "CacheHistoricalPrices: asking Yahoo for $symbol with yrFrom=$yrFrom, monFrom=$monFrom, dayFrom=$dayFrom<br>";
		$contents = YahooFetchHistoricalPrices( $symbol, $yrFrom, $monFrom, $dayFrom );    
		# if ( $debug )
		#     echo "$contents<br>-----------<br>";
		ParseCSV( $link, $stkId, $contents );
	}
	return	$stkId;
}           # CacheHistoricalPrices()
#-------------------------------------------------------------------------------

?>
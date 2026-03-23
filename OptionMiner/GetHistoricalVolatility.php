<?php
include_once( "GetYahooHistoricalPrices.php" );		# CacheHistoricalPrices
include_once( "SharpeLogin.php" );
include_once( "RandomNumbers.php" );

function	GetRecentPrices( $stkId, $link )
{	global $debug;
	$date = YearAgoDate( &$yr, &$mon, &$day );
	if ( $debug )
		echo "GetRecentPrices: YearAgoDate=$date<br>\r\n";
	# the default order is ascending
	# XXX: eliminate the date from the result set once result set order has been verified?
	$query = "SELECT date, adjClose FROM HistoricalPrices WHERE stock_id = '$stkId' AND date > '$date' ORDER BY date";
	# the following query produces stock_ids, dates, and prices for two stocks.  The order of the stock_ids varies with each date
#	$query = "SELECT stock_id, date, adjClose FROM `HistoricalPrices` WHERE date > '$date' AND (stock_id = '$stkId1' OR stock_id = '$stkId2') ORDER BY date";
#    if ( $debug )
#        echo "query=$query<br>\r\n";
    $result = mysql_query( $query, $link ) or die( "GetRecentPrices: Couldn't select prices for $stkId" );
	return	$result;
}			# GetRecentPrices()
#-------------------------------------------------------------------------------
function	GetCorrAndVol( $stkId1, $stkId2, $link, &$ewmaVol1, &$ewmaVol2, &$corr, $volLambda = 0.94, $rejThresh = 3.0 )
{	global $debug;
	# rejThresh is a rejection threshold for a more robust EWMA volatility estimate
	$res1 = GetRecentPrices( $stkId1, $link );
	$res2 = GetRecentPrices( $stkId2, $link );
	$numRows1 = mysql_num_rows( $res1 );
	if ( $debug )
		echo "GetCorrAndVol: numRows=$numRows<br>\r\n";
	if ( $numRows >= 164 )										# two issues, at least 82 records each - where is $numRows defined?
	{	$one_meanLambda = 1.0 - $meanLambda;
		$one_volLambda = 1.0 - $volLambda;

		# for all rows in the result set
		$rejCnt = 0;
		$row1 = mysql_fetch_assoc( $res1 );
		$row2 = mysql_fetch_assoc( $res2 );

		for ( $ii = 1; $ii < $numRows; $ii++ )
		{	$date = $row1['date'];
			$price1 = $row1['adjClose'];
			$price2 = $row2['adjClose'];
			$dailyGain1 = $price1 - $prevPrice1;
			$dailyReturn1 = $dailyGain1 / $prevPrice1;

			$dailyGain2 = $price2 - $prevPrice2;
			$dailyReturn2 = $dailyGain2 / $prevPrice2;

			# robust EWMA volatility ignores outlier returns
			$thresh1 = $rejThresh * sqrt($vol1);
			if ( $thresh1 < 0.001  ||  abs($dailyReturn1) <= $thresh1 )
			{	$sqDailyReturn1 = $dailyReturn1 * $dailyReturn1;
				$vol1 = $volLambda * $vol1 + $one_volLambda * $sqDailyReturn1;
			}
			else
			{	# $ewmaVol remains unchanged
				$rejCnt1++;				# track how often we reject the daily return
			}
#			if ( $debug )
#				echo "$date, $price, $dailyGain, $dailyReturn, $sqDailyReturn, $thresh, $ewmaReturn, $ewmaVol\r\n";
			$row1 = mysql_fetch_assoc( $res1 );
			$row2 = mysql_fetch_assoc( $res2 );
			$prevPrice1 = $price1;								# zero on the first pass
			$prevPrice2 = $price2;								# zero on the first pass
			}
	}
	if ( $debug )
	{	$rejectRatio = 100.0 * (float)$rejCnt / ((float)$numRows - 1.0);
		echo "GetCorrAndEWMAvol: reject ratio=$rejectRatio %<br>\r\n";
	}
	$ewmaVol = $ewmaVol * 365;				# annualize the EWMA volatility
	mysql_free_result( $result );
}			# GetCorrAndVol()
#-------------------------------------------------------------------------------
# for the Dow Jones Wilshire 5000 on the 16th of Feb 2008 (a Saturday)...
# a,b,c -> fromDate; d,e,f -> toDate.  Both in this order:
#    month (zero based),
#    day (toDate is rounded up to next trading day, so appears to be non-inclusive),
#    year
# http://ichart.finance.yahoo.com/table.csv?s=%5EDWC&a=9&b=9&c=2000&d=1&e=17&f=2008&g=d&ignore=.csv
# for GM...
# http://ichart.finance.yahoo.com/table.csv?s=GM&a=01&b=16&c=2007&d=01&e=17&f=2008&g=d&ignore=.csv

$link = mysql_connect( $sql_host, $sql_user, $sql_password )
    or die( "q0: Couldn't connect: " .  mysql_error() );
if ( ! mysql_select_db( $sql_db, $link ) )
    die( "q1: Couldn't select database '$sql_db'" );

# make sure ^DWC is up to date
# $dwcId = CacheHistoricalPrices( "^DWC", "Dow Jones Wilshire 5000 Tot", $link );

# create a delay that is Normally distributed with
# mean 35 seconds and StdDev 8 seconds, but no less than 15 seconds
# StochasticDelay( 35, 8, 20, 50 );

# make sure symbol is up to date
$symbol = "MSFT";
$issueName = "Microsoft Corporation";
$stkId = CacheHistoricalPrices( $symbol, $issueName, $link );
GetCorrAndVol( $stkId1, $stkId2, $link, $vol1, $vol2, $corr );
echo "$issueName's EWMA EWMA volatility=$vol1<br>\r\n";
?>
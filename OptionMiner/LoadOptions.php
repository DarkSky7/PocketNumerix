<?php
function LoadOptions( $cache, $stkId, $ymdhms, $companyName, $expiration, $isCall )
	{	$numOpts = 0;
		$startA = "<a href=";
		$endA = "</a>";
		$startBold = "<b>";
		$endBold = "</b>";
		$startTd = "<td";
		$endTd = "</td>";
		$colorColonPound = "color:#";
		$jj = 0;
		while ( $cache[$jj] != '\0' )
		{		# get the Strike price
			$ii = strpos( $cache, $startA, $jj );			if ( $ii === FALSE ) break;
			$ii += 8;																	# skip "<a href="
			$ii = strpos( $cache, ">", $ii );				if ( $ii === FALSE ) break;
			$ii += 1;																	# skip ">"
			$jj = strpos( $cache, $endA, $ii );				if ( $jj === FALSE ) break;
			$strikePrice = substr( $cache, $ii, $jj - $ii );
			$jj += 4;																	# skip "</a>"

				# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
				# get the option Symbol
			$ii = strpos( $cache, $startA, $jj );			if ( $ii === FALSE ) break;
			$ii += 8;																	# skip "<a href="
			$ii = strpos( $cache, ">", $ii );				if ( $ii === FALSE ) break;
			$ii += 1;																	# skip ">"
			$jj = strpos( $cache, $endA, $ii );				if ( $jj === FALSE ) break;
			$symbol = substr( $cache, $ii, $jj - $ii );
			$jj += 4;																	# skip "</a>"

				# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
				# get the Last transacted price
			$ii = strpos( $cache, $startBold, $jj );		if ( $ii === FALSE ) break;
			$ii += 3;																	# skip "<b>"
			$jj = strpos( $cache, $endBold, $ii );			if ( $jj === FALSE ) break;
			$last = substr( $cache, $ii, $jj - $ii );
			$jj += 4;																	# skip "</b>"

				# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
				# get the Change in price from previous trading day
				# we're parsing a portion of the cache that looks something like:
				# <b style="color:#cc0000;">0.10</b>
				# scan for the colorspec preamble ("color:#")

			$ii = strpos( $cache, $colorColonPound, $jj );	if ( $ii === FALSE ) break;
			$ii += 7;																	# skip "color:#"

			$jj = strpos( $cache, ";", $ii );				if ( $jj === FALSE ) break;
			$colorSpec = substr( $cache, $ii, $jj - $ii );
			$jj += 1;																	# skip ";"
			$colorIsRed = ( $colorSpec[0] != '0'  );

				# get the numerical value of the price change
			$ii = strpos( $cache, ">", $jj );				if ( $ii === FALSE ) break;
			$ii += 1;																	# skip ">"
			$jj = strpos( $cache, $endBold, $ii );			if ( $jj === FALSE ) break;
			$change = substr( $cache, $ii, $jj - $ii );
			if ( $colorIsRed )
				$change = - $change;
			$jj += 4;																	# skip "</b>"

				# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
				# get the Bid price
			$ii = strpos( $cache, $startTd, $jj );			if ( $ii === FALSE ) break;
			$ii += 3;																	# skip "<td"
			$ii = strpos( $cache, ">", $ii );				if ( $ii === FALSE ) break;
			$ii += 1;																	# skip ">"
			$jj = strpos( $cache, $endTd, $ii );			if ( $jj === FALSE ) break;
			$bid = substr( $cache, $ii, $jj - $ii );
			if ( strpos( $bid, "N/A" ) !== FALSE )
				$bid = -1;
			$jj += 5;																	# skip "</td>"

				# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
				# get the Ask price
			$ii = strpos( $cache, $startTd, $jj );			if ( $ii === FALSE ) break;
			$ii += 3;																	# skip "<td"
			$ii = strpos( $cache, ">", $ii );				if ( $ii === FALSE ) break;
			$ii += 1;																	# skip ">"
			$jj = strpos( $cache, $endTd, $ii );			if ( $jj === FALSE ) break;
			$ask = substr( $cache, $ii, $jj - $ii );
			if ( strpos( $ask, "N/A" ) !== FALSE )
				$ask = -1;
			$jj += 5;																	# skip "</td>"

				# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
				# get today's Volume
			$ii = strpos( $cache, $startTd, $jj );			if ( $ii === FALSE ) break;
			$ii += 3;																	# skip "<td"
			$ii = strpos( $cache, ">", $ii );				if ( $ii === FALSE ) break;
			$ii += 1;																	# skip ">"
			$jj = strpos( $cache, $endTd, $ii );			if ( $jj === FALSE ) break;
			$volume = substr( $cache, $ii, $jj - $ii );
			$jj += 5;																	# skip "</td>"
			$volume = str_replace( ",", "", $volume );									# kill commas

				# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
				# get today's Open Interest
			$ii = strpos( $cache, $startTd, $jj );			if ( $ii === FALSE ) break;
			$ii += 3;																	# skip "<td"
			$ii = strpos( $cache, ">", $ii );				if ( $ii === FALSE ) break;
			$ii += 1;																	# skip ">"
			$jj = strpos( $cache, $endTd, $ii );			if ( $jj === FALSE ) break;
			$openInt = substr( $cache, $ii, $jj - $ii );
			$jj += 5;																	# skip "</td>"
			$openInt = str_replace( ",", "", $openInt );								# kill commas

				# write to the browser
			echo "<option>";
			echo "<sym>$symbol</sym> ";
			echo "<strk>$strikePrice</strk> ";
			echo "<last>$last</last> ";
			echo "<chng>$change</chng> ";
			if ( $bid == -1 )	echo "<bid>N/A</bid> ";
			else				echo "<bid>$bid</bid> ";
			if ( $ask == -1 )	echo "<ask>N/A</ask> ";
			else				echo "<ask>$ask</ask> ";
			echo "<vol>$volume</vol> ";
			echo "<opnInt>$openInt</opnInt>";
			echo "</option><br>\r\n";

				# create an entry in the reference argument
			$optQuery = "INSERT INTO options ( stock_id, optionSymbol, strikePrice, expiryDate, isCall ) VALUES ( '$stkId', '$symbol', '$strikePrice', '$expiration', '$isCall' )";
			if ( $debug )		echo "LoadOptions: $optQuery<br>\r\n";
			mysql_query( $optQuery );							# insert fails when there's an existing record

			$optQuery = "SELECT * FROM options WHERE optionSymbol = '$symbol'";
			if ( $debug )	echo "LoadOptions: $optQuery<br>\r\n";
			$optResult = mysql_query( $optQuery )
				or die( 'Option Query failed ' . mysql_error() );

			$optRow = mysql_fetch_assoc( $optResult );
			$optId = $optRow['id'];
			mysql_free_result( $optResult );

				# finally, insert the option prices
			$optPriceQuery = "INSERT INTO optionPrices ( id, priceDateTime, last, oneDayChange, bid, ask, volume, openInterest ) VALUES ( '$optId', '$ymdhms', '$last', '$change', '$bid', '$ask', '$volume', '$openInt' )";
#			echo "LoadOptions: $optPriceQuery<br>\r\n";
			$optPriceResult = mysql_query( $optPriceQuery )
				or die( 'Option Price Insert failed: ' . mysql_error() );

			$numOpts += 1;
		}
		return	$numOpts;
	}
?>
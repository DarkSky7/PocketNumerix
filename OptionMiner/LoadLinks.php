<?php
	function LoadLinks( $cache, $sym, $stkId, $coName, $stkPrc, &$expiration )
	{		# Sample cache (carriage returns inserted to improve readability)
			# $cache won't actually include "View By Expiration" - it was trimmed off by the caller
			# this sample is for Caterpiller Inc., hence the stock symbol "CAT"
			# View By Expiration:
			# <a href="/q/op?s=CAT&amp;m=2006-04">Apr 06</a> |
			# <b>May 06</b> |
			# <a href="/q/op?s=CAT&amp;m=2006-08">Aug 06</a> |
			# <a href="/q/op?s=CAT&amp;m=2006-11">Nov 06</a> |
			# <a href="/q/op?s=CAT&amp;m=2007-01">Jan 07</a> |
			# <a href="/q/op?s=CAT&amp;m=2008-01">Jan 08</a>
			# <table cellpadding="0" cellspacing="0" border="0">
			# $sym is a stock symbol
			# $stkId is the stock's database identifier
			# $coName is the corresponding company name string for exchange symbol $sym
			# $stkPrc is the stock's current market price
			# $expiration is the expiration date for all of the options found in the Yahoo file
#		echo "LoadLinks: cache=$cache, sym=$sym, stkId=$stkId, coName=$coName, stkPrc=$stkPrc<br>\r\n";
		$pos1 = strpos( $cache, "<b>" );
		if ( $pos1 == FALSE )
			return	0;
		$pos1 += 3;										# pass up "<b>"
		$pos2 = strpos( $cache, "</b>", $pos1 );
		if ( $pos2 == FALSE )
			return	0;
		$expiration = substr( $cache, $pos1, $pos2 - $pos1 );
		$expiration = str_replace( " ", "", $expiration );						# kill spaces
#		echo "LoadLinks: initial expiration string = '$expiration'<br>\r\n";	# debug only

			# insert into the database
		$expirySt = MakeExpiryFromMmmYY( $expiration );
		$expiryQuery = "INSERT INTO optionExpiry (stock_id, expDate) VALUES ( '$stkId', '$expirySt' )";
		$expDateResult = mysql_query( $expiryQuery );				# okay if it fails (Uniqueness contraint)

			# let the caller do the output
		echo "<title>$sym: Options for $coName expiring in $expiration</title><br>\r\n";
		echo "<CoName>$coName</CoName><br>\r\n";
		echo "<StkPrc>$stkPrc</StkPrc><br>\r\n";
		echo "<exp>$expiration</exp><br>\r\n";

			# now for the actual links
		$startA = "<a href=";
		$endA = "</a>";
		$ii = 0;
		$expDates = "";
		$datesFound = 0;									# here, not including the expiration above
		while ( $cache[$ii] != '\0' )
		{	$ii = strpos( $cache, $startA, $ii );
			if ( $ii == FALSE )
				break;
			$ii = strpos( $cache, ">", $ii );
			$ii += 1;													# pass up ">"

			$jj = strpos( $cache, $endA, $ii );
			if ( $jj == FALSE )
				break;
			$date = substr( $cache, $ii, $jj - $ii );
			$date = str_replace( " ", "", $date );						# kill spaces
#			echo "LoadLinks: date=$date<br>\r\n";						# debug only
				# stuff it in the database
			$expirySt = MakeExpiryFromMmmYY( $date );
			$expiryQuery = "INSERT INTO optionExpiry (stock_id, expDate) VALUES ( '$stkId', '$expirySt' )";
#			echo "LoadLinks: expiryQuery = $expiryQuery<br>\r\n";
			$expDateResult = mysql_query( $expiryQuery );				# okay if it fails (Uniqueness contraint)
				# add it to the $Dates string
			$expDates .= "<date>$date</date><br>\r\n";				   	# XML output
			$ii = $jj + 4;												# pass up endA
			$datesFound++;
		}
		if ( $datesFound > 0 )
			echo "<Dates><br>\r\n$expDates</Dates><br>\r\n";
		return	$datesFound + 1;								# now we're counting expiration above too
	}
/*
	$cache = 'leading junk <a href="/q/op?s=CAT&amp;m=2006-04">Apr 06</a> | <b>May 06</b> | <a href="/q/op?s=CAT&amp;m=2006-08">Aug 06</a> | <a href="/q/op?s=CAT&amp;m=2006-11">Nov 06</a> | <a href="/q/op?s=CAT&amp;m=2007-01">Jan 07</a> | <a href="/q/op?s=CAT&amp;m=2008-01">Jan 08</a> <table cellpadding="0" cellspacing="0" border="0">';
	$res = LoadLinks( $cache, $expiration );
	echo "res = $res<br>\r\n";
	echo "expiration = $expiration<br>\r\n";
*/
?>
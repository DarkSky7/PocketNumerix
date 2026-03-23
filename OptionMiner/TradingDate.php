<?php
$debug = true;
#	$timeZoneOffset = 0;				// 1 from Chicago, 0 from Ft. Myers, and 3 on GoDady.com

		//					  Jan  Feb  Mar  Apr  May  Jun  Jul	 Aug  Sep  Oct  Nov  Dec
	$daysInMonth = array( 1 => 31,  28,  31,  30,  31,  30,  31,  31,  30,  31,  30,  31 );

	$indexToThreeCharMonth = array( 1 => "Jan", "Feb", "Mar", "Apr", "May", "Jun",
										 "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"	);

	function DayOfWeek( $year, $month, $dayOfMonth )
	{		# $month is expected to be on the range 1..12 (i.e. not zero-based)
			# refer to:  http://ScienceWorld.wolfram.com/astronomy/Weekday.html
            # return value is between 0 (Sunday) and 6 (Saturday)
		$yPrime = $year;
		$mm = $month - 2;
		if ( $mm <= 0 )
		{	$mm += 12;
			$yPrime--;
		}
		$cc = (int)floor( $yPrime / 100 );		// the century
		$yy = $yPrime - 100 * $cc;				// year within the century

		$dayOfWeek = $dayOfMonth
				   + (int)floor( 2.6 * $mm - 0.2 ) + $yy
				   + (int)floor( $yy / 4 )
				   + (int)floor( $cc / 4 ) - 2 * $cc;
		$dayOfWeek = $dayOfWeek % 7;
		if ( $dayOfWeek < 0 )
			$dayOfWeek += 7;
		return	$dayOfWeek;
	}

	function MyGetDate( $timestamp, $GMToffset )
	{		// $GMToffset is in hours
			// returns an array with the following members:
			//	[seconds], [minutes], [hours], [mday], [wday],
			//	[mon], [year], [yday], [weekday], [month], [0]
			// expect incoming $GMTtimestamp to be an integer encoding a UNIX timestamp
			// with zero point at 00:00:00 GMT January 1, 1970
			// the intent is to replace PHP's getdate( $timestamp ) which
			// only works when no argument is provided
			// the $GMToffset is -6 in Chicago
		if ( $timestamp == NULL )
			return	getdate();
		$GMTseconds = $GMToffset * 3600;				// the actual shift is in seconds
		$timestamp += $GMTseconds;
		$days = (int)floor( $timestamp / 86400 );		// seconds in a day --> totsl days
		$secs = $timestamp - $days * 86400;				// total remaining seconds
		$mins = (int)floor( $secs / 60 );

		$arr = array();
		$arr['hours'] = (int)floor( $mins / 60 );
		$arr['minutes'] = $mins - 60 * $arr['hours'];
		$arr['seconds'] = $secs - 60 * $mins;

		$leaps = (int)floor( $days / 1461 );			// complete four year periods
		$postLeapsDays = $days - $leaps * 1461;			// days beyond full leaps

			// we started on Jan 1, 1970, in the middle of a leap cycle,
			// so it's possible that we crossed another leap day
		$oneMoreLeap = false;
		if ( $postLeapsDays > 789 )				// 365*2 + 31 + 28 (crossed another leap day)
		{	$oneMoreLeap = true;
		}
		$yrs = (int)floor( $postLeapsDays / 365 );		// years short of a full leap
		$arr['year'] = 1970 + 4 * $leaps + $yrs;
		$ydays = $postLeapsDays - $yrs * 365;
		$arr['yday'] = $ydays;

			// now for the month and day of month
			// the following would not be a good idea if multiple threads
			// wanted to access $daysInMonth
		$dim = array( 1 => 31,  28,  31,  30,  31,  30,  31,  31,  30,  31,  30,  31 );
		if ( $oneMoreLeap )
			$dim[2] = 29;
		$its = array( 1 => "Jan", "Feb", "Mar", "Apr", "May", "Jun",
						   "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"	);
		$monDays = 0;
		for ( $ii = 1; $ii <= 12; $ii++ )
		{	$tmp = $monDays + $dim[$ii];
			if ( $tmp > $ydays )
				break;
			$monDays = $tmp;
		}
		$arr['mon'] = $ii;
		$arr['month'] = $its[$ii];
		$arr['mday'] = $ydays - $monDays + 1;		// because months start at 1
		$arr['wday'] = DayOfWeek( $arr['year'], $arr['mon'], $arr['mday'] );
			// note, we're not setting the [0] element (cause we're not using it)
		return	$arr;
	}

	function IsLeapYear( $yr )
	{
		return	!($yr % 400)  ||  ( !($yr % 4)  &&  ($yr % 100) );
	}

	function ThirdFriday( $timeArr )
	{		// you can get a $timeArr from getdate()
			// return a mySQL formatted date string which is the
			// third Friday in the month and year specified by $date
			// $date is expected to be a timestamp variable
			// wday starts with Sunday = 0, so Friday = 5
		$deltaFriday = $timeArr['mday'] - $timeArr['wday'] + 5;
		$thirdFriday = 14 + $deltaFriday % 7;
		if ( $thirdFriday < 15 )
			$thirdFriday += 7;
		return	$thirdFriday;
	}

	function ActiveThirdFriday( $timeArr )
	{		// expecting a date array from getdate() for today,
			// i.e. we're presuming that the year and month are today's
			// returning a mySQL date string for a ThirdFriday which has not yet passed
		$thirdFriday = ThirdFriday( $timeArr );
		if ( $thirdFriday < $timeArr['mday'] )
		{		// this month's options have expired
			$timeArr['mon'] += 1;
			if ( $timeArr['mon'] > 12 )
			{	$timeArr['mon'] = 1;
				$timeArr['year'] += 1;
			}
			$timestamp = mktime( 12,0,0, $timeArr['mon'], $thirdFriday, $timeArr['year'] );
			$timeArr = MyGetDate( $timestamp, -6 );
			$thirdFriday = ThirdFriday( $timeArr );
		}
		$timeArr['mday'] = $thirdFriday;
		$timeArr['wday'] = 5;
		return	ThirdFridayMySqlSt( $timeArr );
	}

	function ThirdFridayMySqlSt( $timeArr )
	{		// you can get a $timeArr from getdate()
			// return a mySQL formatted date string which is the
			// third Friday in the month and year specified by $date
			// $date is expected to be a timestamp variable
			// wday starts with Sunday = 0, so Friday = 5
		$thirdFriday = ThirdFriday( $timeArr );
		return	sprintf( "%04d-%02d-%02d",
						 $timeArr['year'], $timeArr['mon'], $thirdFriday );
	}

	function ThirdFridayFromMmmYY( $MmmYY )
	{	$sti = array( "Jan" =>  1, "Feb" =>  2, "Mar" =>  3, "Apr" =>  4,
					  "May" =>  5, "Jun" =>  6, "Jul" =>  7, "Aug" =>  8,
					  "Sep" =>  9, "Oct" => 10, "Nov" => 11, "Dec" => 12  );
		$Mmm = substr( $MmmYY, 0, 3 );
		$monIdx = $sti[ $Mmm ];
		$yr = substr( $MmmYY, 3, 2 );
		$yr += 2000;
		$time = mktime( 0, 0, 0, $monIdx, 20, $yr );
		$date = MyGetDate( $time, -6 );
		return	ThirdFridayMySqlSt( $date );
	}

	function MakeExpiry( $yr, $mo )
	{		// create a MySQL date string for an option expiration date
			// given only the year and month formated as YYYY-MM
			// example input is ( 2007, 08 )
			//            hr, min, sec, mon, day, year
		$timestamp = mktime( 23, 59, 59, $mo, 15, $yr );	// unix timestamp
		$timeThen = MyGetDate( $timestamp, -6 );			// an associative array
		return	ThirdFridayMySqlSt( $timeThen );			// mySQL format YYYY-MM-DD
	}

	function MakeExpiryFromMmmYY( $MmmYY )
	{		// example input is:  "May06"
			// for some reason, the following associative array cannot be external
			// results will not be correct if the array is global
		$sti = array( "Jan" =>  1, "Feb" =>  2, "Mar" =>  3, "Apr" =>  4,
					  "May" =>  5, "Jun" =>  6, "Jul" =>  7, "Aug" =>  8,
					  "Sep" =>  9, "Oct" => 10, "Nov" => 11, "Dec" => 12  );
			// create a MySQL date string for an option expiration date
			// given only the year and month formated as YYYY-MM
		$moSt = substr( $MmmYY, 0, 3 );
		$yrSt = substr( $MmmYY, 3, 2 );
		sscanf( $yrSt, "%d", $yr );
		$yr += 2000;
		$monIdx = $sti[$moSt];
		return	MakeExpiry( $yr, $monIdx );
	}

	function MakeMmmYYFromMySqlDate( $mySqlDate )
	{
		$its = array( 1 => "Jan", "Feb", "Mar", "Apr", "May", "Jun",
						   "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"	);
		sscanf( $mySqlDate, "%4d-%2d-%2d", $yr, $mon, $da );
		return	sprintf( "%3s%02d", $its[$mon], $yr - 2000 );
	}

	function DayBefore( &$yr, &$mon, &$mday, &$wday )
	{   global $daysInMonth;
        $mday--;
		$wday--;
		if ( $wday < 0 )
			$wday += 7;
		if ( $mday < 1 )
		{	$mon--;
			if ( $mon < 1 )
			{	$mon += 12;
				$yr--;
			}
            $mday = $daysInMonth[ $mon ];
            if ( $mon == 2  &&  IsLeapYear( $yr ) )
			{	$mday++;
				$wday++;
			}
		}
	}
		
	function OneHourBeforeOrLastClose( $hoursFromEasternTime )
	{		// returns a dateTime string suitable for mySQL Queries and Inserts
			// the dateTime will be an hour before current local (server) time
			// but shifted to East Coast time, on a trading day within
			// trading hours.  The previous day's close is used if the corrected
			// time is outside of trading hours.
			//
			// In Chicago, hoursFromEasternTime = +1; in SanFrancisco it's +3.
			// If local time in Chicago is 2:30PM, NY time is 3:30PM
		$dt = getDate();					// essentially 'now'
		$yr = $dt['year'];
		$mon= $dt['mon'];
		$mday = $dt['mday'];
		$hour = $dt['hours'];
		$min = $dt['minutes'];
		$hrMin = 100 * $hr + $min;
		$wday = $dt['wday'];					// Sunday :== 0
		$hour += $hoursFromEasternTime;			// adjust for time zone changes

			// (assumes that dateTimes in the database are NYSE based)
		if ( $hour > 23 )
		{		// we don't really need to flip days unless the
				// OCR server is really remote from the NYSE
				// because we're just going to
				// revert back to today's close anyway
				// obviously things would change if the OCR server
				// were located East of NY (e.g. Iceland, London, Paris ?)
			$hour = 23;
		}
			// generate the most recent trading day close
		$hrMin = 100 * $hour + $min;
		if ( 0 == $wday  ||  $wday == 6  ||  $hrMin < 930 )
		{		// it's a weekend day or sometime in the wee hours
				// before the opening bell ...
				// back up to the close of the previous trading day
			DayBefore( $yr, $mon, $mday, $wday );
			while ( $wday == 0  ||  $wday == 6 )
				DayBefore( $yr, $mon, $mday, $wday );
			$hour = 18;
			$min = 30;
		}
		else
		{	if ( $hrMin >= 1830 )
			{		// after the close of trading - revert to the close
				$hour = 18;		// 6PM
				$min = 30;		// end of trading
			}
			else if ( $hrMin < 1030 )
			{		// within an hour of the opening bell
					// revert to the open
				$hour = 9;
				$min = 30;
			}
			else
			{		// must be during trading hours, but after 10:30
				$hour -= 1;
			}
		}
		$ret = sprintf( "%04d-%02d-%02d %02d:%02d:00",
						 $yr, $mon, $mday, $hour, $min );
		return	$ret;
	}
?>

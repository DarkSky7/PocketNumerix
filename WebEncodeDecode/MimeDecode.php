<?php
function Base64ToNum( $ain )
{		// map Base64 codes to numbers on the range 0x00 .. 0x3F
	if ( 'A' <= $ain  &&  $ain <= 'Z' )	return	ord($ain) - ord('A');
	if ( 'a' <= $ain  &&  $ain <= 'z' )	return	ord($ain) - ord('a') + 26;
	if ( '0' <= $ain  &&  $ain <= '9' )	return	ord($ain) - ord('0') + 52;
	if ( $ain == '+' )					return	62;
										return	63;			// a '/'
}			// Base64ToNum()
//--------------------------------------------------------------------------------------------
/*	// the test case for Base64ToNum()
$str = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
for ( $ii = 0; $ii < 64; $ii++ )
	printf( "<br>%s --> %d", $str[$ii], Base64ToNum($str[$ii]) );
*/
//--------------------------------------------------------------------------------------------
function PackSixBitCodesInThreeBytes( $ip, $ip_offset, $nValidCodes )
{		// expecting $ip to be a character array
		// want to perform integer operations and write results to $op, also a character array
		// ignore the top two bits of incoming bytes --> outgoing bytes are full of data
	if ( $nValidCodes <= 0 )
		return;
	$debug = 0;
	if ( $debug )			// &&  $ip_offset > 25
	{	echo " -->";
		for ( $ii = $ip_offset; $ii < $ip_offset+4; $ii++ )
			printf( " %02x", ord($ip[$ii]) );
		echo "<br>";
	}
		// nValidCodes prevents buffer overrun on the input
	$i0 = $ip_offset + 0;
//	echo "<br>";
//	for ( $ii = $i0; $ii < ($i0+4); $ii++ )		// '1', '2', '3', '4' -->
//		printf( " %02x", ord($ip[$ii]) );					// b0011 0001, b0011 0010, b0011 0011, b0011 0100
//	echo "<br>";								// expect: 110001 110010 110011 110100 --> 1100 0111, 0010 1100, 1111 0100 (C7 2C F4)
//	for ( $ii = $i0; $ii < $i0 + 4; $ii++ )
//		printf( " %02x", ord($ip[$ii]) );
	$o0 = (ord($ip[$i0]) << 2) & 0xFC;			// 00's in the least significant bits
	if ( $debug )
		printf( "0: %02x<br>", $o0 );			// ... '1' 0x31 (0011 0001) --> 0xc4 (1100 0100)
	if ( $nValidCodes == 1 )	return	pack( "C", $o0 );

	$i1 = $ip_offset + 1;
	$o0 |= (ord($ip[$i1]) >> 4) & 0x03;			// <6 bits in[0]><2 bits in[1]>
	$o1  = (ord($ip[$i1]) << 4) & 0xFF;			// 0000's in the least significant bits
	if ( $debug )
	{	printf( "0: %02x<br>", $o0 );			// ...
		printf( "1: %02x<br>", $o1 );
	}
	if ( $nValidCodes == 2 )	return	pack( "CC", $o0, $o1 );

	$i2 = $ip_offset + 2;
	$o1 |= (ord($ip[$i2]) >> 2) & 0x0F;			// <4 bits in[1]><4 bits in[2]>
	$o2  = (ord($ip[$i2]) << 6) & 0xFF;			// 000000 in the least significant bits
	if ( $debug )
	{	printf( "1: %02x<br>", $o1 );
		printf( "2: %02x<br>", $o2 );
	}
	if ( $nValidCodes == 3 )	return	pack( "CCC", $o0, $o1, $o2 );

	$i3 = $ip_offset + 3;
	$o2 |= ord($ip[$i3])		& 0x3F;			// <2 bits in[2]><6 bits in[3]>
	if ( $debug )
		printf( "2: %02x<br>", $o2 );
	return	pack( "CCC", $o0, $o1, $o2 );
}			// PacbkSixBitCodesInThreeBytes()
//--------------------------------------------------------------------------------------------
/*
	$str = "ABCD";					// 1, 2, 3, 4, i.e., xx00 0001 xx00 0010 xx00 0011 xx00 0100
									// drop the top two bits of each word and merge...
									//					 00 0001 00  0010  00 00  11 00 0100
									//					 0x04        0x20         0xC4
	$res = PackSixBitCodesInThreeBytes( $str, 0, 4 );
	for ( $ii = 0; $ii < 3; $ii++ )
		printf( " %d: %02x '%s'<br>", $ii, ord($res[$ii]), $res[$ii] );
*/
//--------------------------------------------------------------------------------------------
function	MIMEdecode( $ibuf, $bytesLeft, &$obuf )
{		// returns the number of output bytes written
		// expects a string
	$debug = 0;
	$ibuf_p = 0;
	$obuf_p = 0;
	while ( $bytesLeft > 0 )
	{		// 4 MIME codes into 3 binary bytes
		$nPadChars = 0;
		if ( $bytesLeft > 4 )
		{		// decode four bytes of input in place
			$iLimit = $ibuf_p + 4;
			for ( $ii = $ibuf_p; $ii < $iLimit; $ii++ )
			{
//				$val = ord( $ibuf[$ii] );
//				printf( " %02x", $val );
//				$ibuf[$ii] = chr( Base64ToNum( $ibuf[$ii] ) );			// this doesn't work
				$ibuf[$ii] = pack("C", Base64ToNum( $ibuf[$ii] ));		// but this does
//				printf( " --> %02x<br>", ord($ibuf[$ii])  );
			}
//			echo "<br>";
				// pack the decoded six bit binary segments into three (binary) bytes
			if ( $debug )
			{	for ( $ii = $ibuf_p; $ii < $ibuf_p + 4; $ii++ )
					printf( " %02x", ord($ibuf[$ii]) );
			}
			$threeBytes = PackSixBitCodesInThreeBytes( $ibuf, $ibuf_p, 4 );
			if ( $debug )
				printf( "%d:%s<br>------<br>", $obuf_p, $threeBytes );
			$obuf .= $threeBytes;
				// echo results of unpacking
//			echo " -->";
			$oLimit = $obuf_p + 3;
//			for ( $oo = $obuf_p; $oo < $oLimit; $oo++ )
//				printf( " %02x", $obuf[$oo] );
		}
		else	// we found four MIME codes (hopefully, no less).  The possibilities are:
		{		// f(00xxxxxx), f(00xxyyyy), f(00yyyyzz), f(00zzzzzz)	- 4 valid codes, write three bytes
				// f(00xxxxxx), f(00xxyyyy), f(00yyyy00),	'='			- 3 valid codes, write two bytes
				// f(00xxxxxx), f(00xx0000),	 '=',		'='			- 2 valid codes, write one bytes
			$sub = $ibuf_p + 3;
			if ( $ibuf[$sub] == '=' ) $nPadChars++;
			$sub -= 1;
			if ( $ibuf[$sub] == '=' ) $nPadChars++;
			$nValidCodes = 4 - $nPadChars;								// thus 4, 3, or 2
				// decode <nValidCodes> of input in place, zero out the rest
			$iLimit = $ibuf_p + $nValidCodes;
			for ( $ii = $ibuf_p; $ii < $iLimit; $ii++ )
				$ibuf[$ii] = pack("C", Base64ToNum( $ibuf[$ii] ));		// decode to binary in place
			if ( $debug )
			{	for ( $ii = $ibuf_p; $ii < $ibuf_p + 4; $ii++ )
					printf( " %02x", ord($ibuf[$ii]) );
			}
			$threeBytes = PackSixBitCodesInThreeBytes( $ibuf, $ibuf_p, $nValidCodes );
			if ( $debug )
				printf( "%d:%s<br>------<br>", $obuf_p, $threeBytes );
			$obuf .= $threeBytes;
//			$oLimit = $obuf_p + $nValidCodes - 1;						// we don't process $obuf[$oLimit]
//			for ( $oo = $obuf_p; $oo < $oLimit; $oo++ )
//				printf( " %02x", $obuf[$oo] );
		}
		$ibuf_p += 4;
		$obuf_p += 3 - $nPadChars;
		$bytesLeft -= 4;
	}
	return	$obuf_p;				// output length
}			// MIMEdecode()
//---------------------------------------------------------------------------------
/*
// expecting: "?os=5.1.1702?dv=618754147?em=mlandis@acm.org?in=90234023?dt=2005-10-13?ln=Landis?fn=Mike"
$str="P29zPTUuMS4xNzAyP2R2PTYxODc1NDE0Nz9lbT1tbGFuZGlzQGFjbS5vcmc/aW49OTAyMzQwMjM/ZHQ9MjAwNS0xMC0xMz9sbj1MYW5kaXM/Zm49TWlrZQ==";
$slen = strlen( $str );
echo $str . "<br>";
echo "slen=" . $slen . "<br>";
$resLen = MIMEdecode( $str, $slen, $res );

echo "<br>resLen=" . $resLen . "<br>";
for ( $ii = 0; $ii < $resLen; $ii++ )
	printf( " %02x", ord($res[$ii]) );
echo "<br>... as a string<br>  ";
for ( $ii = 0; $ii < $resLen; $ii++ )
	printf( "%s", $res[$ii] );
echo "<br>?os=5.1.1702?dv=618754147?em=mlandis@acm.org?in=90234023?dt=2005-10-13?ln=Landis?fn=Mike";
echo "<br>123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 ";
*/
?>

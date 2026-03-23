<?php
include_once( "my_ord.php" );

function NumToBase64( $ain )
{		// map 0 .. 63 onto { 'A' .. 'Z', 'a' .. 'z', '+', '/' }
	if		( $ain <  26 )		$yy = ord('A') + $ain;			// 0 --> 'A' .. 25 -> 'Z'
	else if ( $ain <  52 )		$yy = ord('a') + $ain - 26;		// 26 -> 'a' .. 51 -> 'z'
	else if ( $ain <  62 )		$yy = ord('0') + $ain - 52;		// 52 -> '0' .. 61 -> '9'
	else if ( $ain == 62 )		$yy = ord('+');
	else						$yy = ord('/');
	return	 chr( $yy );
}			// NumToBase64()
//--------------------------------------------------------------------------------------------
/*
		// the test case for Base64ToNum()
	for ( $ii = 0; $ii < 64; $ii++ )
//		printf( "%d --> %s<br>",   $ii, NumToBase64($ii) );	// works when NumToBase64 returns chr($yy)
		printf( "%d --> %02x<br>", $ii, NumToBase64($ii) );	// works when NumToBase64 returns $yy
*/
//--------------------------------------------------------------------------------------------
function ThreeBytesIntoMIMEcodes( $ibuf, $ii )
{		// incoming bytes are fully packed --> outgoing bytes contain 00's in the most significant two bits
	$debug = 0;
	if ( $debug )
	{	echo "<br>";
		for ( $jj = $ii; $jj < ($ii + 3); $jj++ )
			printf( " %02x", my_ord($ibuf[$jj]) );
		echo " --> ";
	}
	$i0 = my_ord( $ibuf[$ii] );											// ord() is necessary to success in characcter mode
	$o0 = NumToBase64(						    ($i0 >> 2) & 0x3F  );	//					   <00><   top 6 bits of in[0]>

	$ii++;
	$i1 = my_ord( $ibuf[$ii] );											// ord() is necessary to success in characcter mode
	$o1 = NumToBase64( (($i0 << 4) & 0x30)  |  (($i1 >> 4) & 0x0F) );	// <bottom 2 bits of in[0]><   top 4 bits of in[1]>

	$ii++;
	$i2 = my_ord( $ibuf[$ii] );											// ord() is necessary to success in characcter mode
	$o2 = NumToBase64( (($i1 << 2) & 0x3C)  |  (($i2 >> 6) & 0x03) );	// <bottom 4 bits of in[1]><   top 2 bits of in[2]>

	$o3 = NumToBase64(							 $i2       & 0x3F  );	//					   <00><bottom 6 bits of in[3]>
	if ( $debug )
		echo	$o0 . $o1 . $o2 . $o3;
	return	$o0 . $o1 . $o2 . $o3;
}			// ThreeBytesIntoMIMEcodes()
//--------------------------------------------------------------------------------------------
/*
		// the test case for ThreeBytesIntoMIMEcodes
//	$pkStr = "ABC";										// -> ABC -> QUjD
	$pkStr = pack( "CCCC", 0x9F, 0x57, 0xF3, 0 );		// -> 'n1fz'
	$b64 = ThreeBytesIntoMIMEcodes( $pkStr, 0 );
	for ( $oo = 0; $oo < 4; $oo++ )
		printf( "%d: %02x (%s)<br>", $oo, ord($b64[$oo]), $b64[$oo] );	// i.e. 0x4D 0x56 0x66 0x7A
*/
//--------------------------------------------------------------------------------------------
function	MIMEencode( $ibuf, $bytesLeft, &$obuf )
{		// return value is the length of $obuf
		// convert binary 'string' $ibuf into an array of integers
//	$ibuf = str_split( $ibufStr );				// this is necessary when input is a string
//	for ( $ii = 0; $ii < $bytesLeft; $ii++ )	// cfb_encrypt --> an array, not a string
//		printf( "%s", $ibuf[$ii] );
//	printf( "<br>bytesLeft=%d<br>", $bytesLeft );
	$ib_p = 0;
	$ob_p = 0;
	while ( $bytesLeft > 0 )
	{		// three bytes into 4 MIME codes
		if ( $bytesLeft >= 3 )
		{	$obuf .= ThreeBytesIntoMIMEcodes( $ibuf, $ib_p );			// returns a four character packed binary string
			$ib_p += 3;
			$bytesLeft -= 3;
		}
		else	// we came up short of three binary bytes, implying end of the buffer or EOF.
		{		// since buffer size is a multiple of 3, we conclude that it's EOF
			$ii = $ib_p + 0;											// definitely have one pad character
			$i0 = my_ord( $ibuf[$ii] );									// ord() is necessary to success in character mode
			$obuf .= NumToBase64( ($i0 >> 2) & 0x3F );
			$tmp =								($i0 << 4) & 0x30;		// needed in both cases
			if ( $bytesLeft == 1 )
			{		// 1 left, out[3] = {	f(00xxxxxx), f(00xx0000), =, =				}
				$obuf .= NumToBase64( $tmp ) . '=';						// was using ord('=')
			}
			else	// $bytesLeft == 2
			{		// 2 left, out[3] = {	f(00xxxxxx), f(00xxyyyy>, f(00yyyy00), =	}
				$ii = $ib_p + 1;
				$i1 = my_ord( $ibuf[$ii] );								// ord() is necessary to success in character mode
				$obuf .= NumToBase64( $tmp  |  (($i1 >> 4) & 0x0F) );
				$obuf .= NumToBase64(			($i1 << 2) & 0x3C  );
			}
			$obuf .= '=';			// need at least one pad character when bytesLeft < 3
			$bytesLeft = 0;
		}
		$ob_p += 4;
	}
	return	$ob_p;
}			// MIMEencode()
//--------------------------------------------------------------------------------------------
/*		// this is broken right now so the encrypt/encode/decode/decrypt cycle will work
		// encode
	$params = "?os=5.1.1702?dv=618754147?ml=mlandis@acm.org?in=90234023?dt=2005-10-13?ln=Landis?fn=Mike";
	$paramsLen = strlen( $params );
	$b64paramsLen = MIMEencode( $params, $paramsLen, $b64params );
	echo "<br>b64paramsLen=" . $b64paramsLen;
	echo "<br>b64params...<br>";
	for ( $ii = 0; $ii < $b64paramsLen; $ii++ )
		printf( " %02x", ord($b64params[$ii]) );
	echo "<br>... as a string ...<br>" . $b64params;

		// decode
	include_once( "MimeDecode.php" );
//	$b64params = "P29zPTUuMS4xNzAyP2R2PTYxODc1NDE0Nz9tbD1tbGFuZGlzQGFjbS5vcmc/aW49OTAyMzQwMjM/ZHQ9MjAwNS0xMC0xMz9sbj1MYW5kaXM/Zm49TWlrZQ==";
	$decodeLen = MIMEdecode( $b64params, $b64paramsLen, $decode );
	echo "<br>decodeLen=" . $decodeLen;
	echo "<br>decode...<br>";
	for ( $ii = 0; $ii < $decodeLen; $ii++ )
		printf( " %02x", ord($decode[$ii]) );
	echo "<br>... as a string ...<br>" . $decode;
*/
?>

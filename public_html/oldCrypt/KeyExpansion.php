<?php
function	SubWord( $word )
{	$Sbox = array(
		 99,	124,	119,	123,	242,	107,	111,	197, 	 48,	  1,	103,	 43,	254,	215,	171,	118,
		202,	130,	201,	125,	250,	 89,	 71,	240,	173,	212,	162,	175,	156,	164,	114,	192,
		183,	253,	147,	 38,	 54,	 63,	247,	204,	 52,	165,	229,	241,	113,	216,	 49,	 21,
		  4,	199,	 35,	195,	 24,	150,	  5,	154,	  7,	 18,	128,	226,	235,	 39,	178,	117,
		  9,	131,	 44,	 26,	 27,	110,	 90,	160,	 82,	 59,	214,	179,	 41,	227,	 47,	132,
		 83,	209,	  0,	237,	 32,	252,	177,	 91,	106,	203,	190,	 57,	 74,	 76,	 88,	207,
		208,	239,	170,	251,	 67,	 77,	 51,	133,	 69,	249,	  2,	127,	 80,	 60,	159,	168,
		 81,	163,	 64,	143,	146,	157,	 56,	245,	188,	182,	218,	 33,	 16,	255,	243,	210,
		205,	 12,	 19,	236,	 95,	151,	 68,	 23,	196,	167,	126,	 61,	100,	 93,	 25,	115,
		 96,	129,	 79,	220,	 34,	 42,	144,	136,	 70,	238,	184,	 20,	222,	 94,	 11,	219,
		224,	 50,	 58,	 10,	 73,	  6,	 36,	 92,	194,	211,	172,	 98,	145,	149,	228,	121,
		231,	200,	 55,	109,	141,	213,	 78,	169,	108,	 86,	244,	234,	101,	122,	174,	  8,
		186,	120,	 37,	 46,	 28,	166,	180,	198,	232,	221,	116,	 31,	 75,	189,	139,	138,
		112,	 62,	181,	102,	 72,	  3,	246,	 14,	 97,	 53,	 87,	185,	134,	193,	 29,	158,
		225,	248,	152,	 17,	105,	217,	142,	148,	155,	 30,	135,	233,	206,	 85,	 40,	223,
		140,	161,	137,	 13,	191,	230,	 66,	104,	 65,	153,	 45,	 15,	176,	 84,	187,	 22	);

		// perform Sbox substitutions on integer $word
//	printf( "SubWord <-- 0x%08x<br>", $word );
	$ww = $Sbox[ $word & 0xFF ];			// low byte substitution
	$word = $word >> 8;
	$xx = $Sbox[ $word & 0xFF ] << 8;		// second byte substitution
	$word = $word >> 8;
	$yy = $Sbox[ $word & 0xFF ] << 16;		// third byte substitution
	$word = $word >> 8;
	$zz = $Sbox[ $word & 0xFF ] << 24;		// high byte substitution
	$res = $zz | $yy | $xx | $ww;
//	printf( "SubWord --> 0x%08x<br>", $res );
	return	$res;
}			// SubWord()
//------------------------------------------------------------------------------
function	RotateWord( $word )
{		// circularly shift $word one byte to the right
//	printf( "RotateWord <-- 0x%08x<br>", $word );
	$cc = ($word & 0xFF) << 24;			// $cc = low order byte shifted into high position
	$dd = ($word  >> 8 ) & 0xFFFFFF;	// the mask is necessary because PHP integers are signed
	$res = $cc | $dd;
//	printf( "RotateWord --> 0x%08x<br>", $res );
	return	$res;					// put $cc into $word in the high position
}			// RotateWord()
//------------------------------------------------------------------------------
// refer to Brian Gladman's April 2003 v3.6 paper on Rijndael
// Brian Gladman shows construction of a word from bytes in the key
// the addresses of the key bytes are highest on the left, lowest on the right, thus little Endian
// so we can just copy the cipher key a word at a time.  we can simply address a word as 4 contiguous
// bytes
// with the same address as the lowest address byte, so his first while loop isn't necessary
function	KeyExpansion( $cipherKey, $nCipherKeys )
{		// for our purposes, nKeys is eq. to nColumns, thus 4, 6, or 8
	$nRounds = $nCipherKeys + 6;
	$nRoundKeys = $nCipherKeys * ( $nRounds + 1 );			// Nc(Nr+1):  4(10), 6(12), 8(14) -> 44, 78, 120
	$blockSize = $nCipherKeys * 4;

		// input: cipherKey, a character array containing nKeys integers
		// output: keySchedule, an integer array with nKeys*(nRounds+1) elements
		// pack the (integers) cipherKeys into the base of the KeySchedule
	for ( $kk = 0; $kk < $nCipherKeys; $kk++ )
	{	$tmp = 0;
		for ( $rr = 0; $rr < 4; $rr++ )
		{	$t1 = ord( $cipherKey[$kk*4+$rr] ) << (8 * $rr);
			$tmp |= $t1;
//			printf( "tmp=%08x, t1=%02x<br>", $tmp, $t1 );
		}
		$KeySchedule[$kk] = $tmp;
	}
/*
	echo "KeyExpansion: KeySchedule=<br>";
	for ( $ii = 0; $ii < $nCipherKeys; $ii++ )
		printf( " %08x", $KeySchedule[$ii] );
	echo "<br>";
*/
		// According to:  http://www.quadibloc.com/crypto/co040401.htm, the first 36 round constants are:
		//  1   2   4   8  16  32  64 128  27  54 108 216 171  77 154  47  94 188
		// 99 198 151  53 106 212 179 125 250 239 197 145  57 114 228 211 189  97 ...
		// we only need the first 15 to cover 15 encryption rounds (nRounds + 1)
	$Rcon = array( 0 => 1, 2, 4, 8, 16, 32, 64, 128, 27, 54, 108, 216, 171, 77, 154 );
	for ( $kk = $nCipherKeys; $kk < $nRoundKeys; $kk++ )
	{	$temp = $KeySchedule[ $kk - 1 ];
		if ( ($kk % $nCipherKeys ) == 0 )
		{	$rconVal = $Rcon[ $kk / $nCipherKeys - 1 ];
			$substWord = SubWord( RotateWord($temp) );
//			printf( "temp=0x%08x, rconVal=0x%08x, substWord=0x%08x<br>",
//				$temp, $rconVal, $substWord );
			$temp = $substWord ^ $rconVal;
		}
		else if ( $nCipherKeys > 6  &&  ($kk % $nCipherKeys) == 4 )
			$temp = SubWord( $temp );
//		printf( "KeySchedule[%d] = 0x%08x, KeySchedule[%d]= 0x%08x, temp = 0x%08x<br>",
//				$kk, $KeySchedule[$kk], $kk-$nCipherKeys, $KeySchedule[$kk-$nCipherKeys], $temp );
		$KeySchedule[ $kk ] = $KeySchedule[ $kk - $nCipherKeys ] ^ $temp;
//		printf( "KeySchedule[%d] = 0x%08x<br>", $kk, $KeySchedule[$kk] );
	}
	return	$KeySchedule;
}			// KeyExpansion()
//------------------------------------------------------------------------------
?>
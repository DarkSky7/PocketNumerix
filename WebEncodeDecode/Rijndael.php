<?php
require_once( "GFmul.php" );

function SubBytes( $state, $nCells )
{		// byte oriented state...
		// not sure why, but SubBytes doesn't work when
		// defined to pass by reference:  SubBytes( &$state, $nCells)
	$Sbox = array( 
		 99,	124,	119,	123,	242,	107,	111,	197,	 48,	  1,	103,	 43,	254,	215,	171,	118,
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
		140,	161,	137,	 13,	191,	230,	 66,	104,	 65,	153,	 45,	 15,	176,	 84,	187,	 22
	);
/*
	printf( "SubBytes incoming state:<br>" );
	for ( $cc = 0; $cc < $nCells; $cc++ )
		printf( " %x", $state[$cc] );
	echo "<br>";
*/
	for ( $cc = 0; $cc < $nCells; $cc++ )
		$state[$cc] = $Sbox[ $state[$cc] ];
/*
	printf( "SubBytes outgoing state:<br>" );
	for ( $cc = 0; $cc < $nCells; $cc++ )
		printf( " %x", $state[$cc] );
	echo "<br>";
*/
	return	$state;
}			// SubBytes()
//------------------------------------------------------------------------------
// see page 12 of Brian Gladman's Apr-03 v3.6 Specification for Rijndael, the AES Algorithm
// pp.11-12 of the following presentation is succinct:
// http://eee.ucc.ie/staff/marnanel/Files/handoutee5251/lec-AES2.pdf, but has a different matrix
// for for the forward cipher and no matrix for the inverse cipher.
//	A is a column of state; and C is the polynomial 03 x^3 + 01 x^2 + 01 x^1 + 02
// so (03 x^3 + 01 x^2 + 01 x^1 + 02) mod ( x^4 + 1 ) in a GF(2^8) Galois Field
// becomes the coefficient matrix below
// | b0 |	  | 02 01 01 03 | | a0 |
// | b1 |  =  | 03 02 01 01 | | a1 |
// | b2 |     | 01 03 02 01 | | a2 |
// | b3 |     | 01 01 03 02 | | a3 |
function	MixColumns( $state, $nColumns )
{		// byte oriented state ...
		// make a copy of state
/*
	printf( "MixColumns: incoming state=<br>" );
	for ( $ii = 0; $ii < $nColumns*4; $ii++ )
		printf( " %x", $state[$ii] );
	echo "<br>";
*/
	for ( $cc = 0; $cc < $nColumns; $cc++ )
	{	$xx = array_slice( $state, 4*$cc, 4 );
//		for ( $ii = 0; $ii < 4; $ii++ )
//			printf( " %x", $xx[$ii] );
		$state[4*$cc  ] =											// multiply by | 02 03 01 01 |
			GFmul(2,$xx[0]) ^ $xx[1] ^ $xx[2] ^ GFmul(3,$xx[3]);	// and add (xor in a Galois Field)
		$state[4*$cc+1] =											// multiply by | 01 02 03 01 |
			GFmul(3,$xx[0]) ^ GFmul(2,$xx[1]) ^ $xx[2] ^ $xx[3];	// and add (xor in a Galois Field)
		$state[4*$cc+2] =											// multiply by | 01 01 02 03 |
			$xx[0] ^ GFmul(3,$xx[1]) ^ GFmul(2,$xx[2]) ^ $xx[3];	// and add (xor in a Galois Field)
		$state[4*$cc+3] =											// multiply by | 03 01 01 02 |
			$xx[0] ^ $xx[1] ^ GFmul(3,$xx[2]) ^ GFmul(2,$xx[3]);	// and add (xor in a Galois Field)
	}
//	echo "<br>";
/*
	printf( "MixColumns: outgoing state=<br>" );
	for ( $ii = 0; $ii < $nColumns*4; $ii++ )
		printf( " %x", $state[$ii] );
	echo "<br>";
*/
	return	$state;
}			// MixColumns()
//------------------------------------------------------------------------------
// XOR each column of state with the keySchedule (a subset of the round key schedule)
// an example call looks like:
//		XorRoundKey( (unsigned long*)state, (unsigned long*)keySchedule+colNum );
// where rn is the round number
// XorRoundRey is its own inverse
function	XorRoundKey( $state, $KeySchedule, $roundNum, $nColumns )
{		// keySchedule is an array of integers
		// $keyIdx is the starting index of interest
		// state is an array of bytes
/*
	printf( "XorRoundKey: incoming state=<br>" );
	for ( $ii = 0; $ii < $nColumns*4; $ii++ )
		printf( " %x", $state[$ii] );
	echo "<br>";
*/
	$nRounds = ($nColumns + 6) + 1;
	$baseIdx = $roundNum * $nColumns;
	$ii = 0;
	for ( $cc = 0; $cc < $nColumns; $cc++ )
	{	$key = $KeySchedule[ $baseIdx + $cc ];
//		printf( "KeySchedule[%d]=0x%x<br>", $baseIdx + $cc, $key );
		for ( $rr = 0; $rr < 4; $rr++ )
		{	$mask = $key & 0xFF;
			$stii = $state[ $ii ];
			$state[ $ii ] = $stii ^ $mask;
//			printf( "%d: %x <-- %x ^ %x<br>", $ii, $state[$ii], $stii, $mask );
			$key = $key >> 8;			// for the next pass
			$ii++;
		}
	}
/*
	printf( "XorRoundKey: outgoing state=<br>" );
	for ( $ii = 0; $ii < $nColumns*4; $ii++ )
		printf( " %x", $state[$ii] );
	echo "<br>";
*/
	return	$state;
}			// XorRoundKey()
//------------------------------------------------------------------------------
?>
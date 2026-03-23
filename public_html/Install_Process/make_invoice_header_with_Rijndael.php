<?php
function GFmul( $aa, $bb )
{		// the logarithms (L) table from http://www.cs.utsa.edu/~wagner/laws/FFM.html
	$Log = array(
	 		// the 0,0 entry is irrelevant (no log of 0), but the code works around it
		0x00, 0x00, 0x19, 0x01, 0x32, 0x02, 0x1a, 0xc6, 0x4b, 0xc7, 0x1b, 0x68, 0x33, 0xee, 0xdf, 0x03,
		0x64, 0x04, 0xe0, 0x0e, 0x34, 0x8d, 0x81, 0xef, 0x4c, 0x71, 0x08, 0xc8, 0xf8, 0x69, 0x1c, 0xc1,
		0x7d, 0xc2, 0x1d, 0xb5, 0xf9, 0xb9, 0x27, 0x6a, 0x4d, 0xe4, 0xa6, 0x72, 0x9a, 0xc9, 0x09, 0x78,
		0x65, 0x2f, 0x8a, 0x05, 0x21, 0x0f, 0xe1, 0x24, 0x12, 0xf0, 0x82, 0x45, 0x35, 0x93, 0xda, 0x8e,
		0x96, 0x8f, 0xdb, 0xbd, 0x36, 0xd0, 0xce, 0x94, 0x13, 0x5c, 0xd2, 0xf1, 0x40, 0x46, 0x83, 0x38,
		0x66, 0xdd, 0xfd, 0x30, 0xbf, 0x06, 0x8b, 0x62, 0xb3, 0x25, 0xe2, 0x98, 0x22, 0x88, 0x91, 0x10,
		0x7e, 0x6e, 0x48, 0xc3, 0xa3, 0xb6, 0x1e, 0x42, 0x3a, 0x6b, 0x28, 0x54, 0xfa, 0x85, 0x3d, 0xba,
		0x2b, 0x79, 0x0a, 0x15, 0x9b, 0x9f, 0x5e, 0xca, 0x4e, 0xd4, 0xac, 0xe5, 0xf3, 0x73, 0xa7, 0x57,
		0xaf, 0x58, 0xa8, 0x50, 0xf4, 0xea, 0xd6, 0x74, 0x4f, 0xae, 0xe9, 0xd5, 0xe7, 0xe6, 0xad, 0xe8,
		0x2c, 0xd7, 0x75, 0x7a, 0xeb, 0x16, 0x0b, 0xf5, 0x59, 0xcb, 0x5f, 0xb0, 0x9c, 0xa9, 0x51, 0xa0,
		0x7f, 0x0c, 0xf6, 0x6f, 0x17, 0xc4, 0x49, 0xec, 0xd8, 0x43, 0x1f, 0x2d, 0xa4, 0x76, 0x7b, 0xb7,
		0xcc, 0xbb, 0x3e, 0x5a, 0xfb, 0x60, 0xb1, 0x86, 0x3b, 0x52, 0xa1, 0x6c, 0xaa, 0x55, 0x29, 0x9d,
		0x97, 0xb2, 0x87, 0x90, 0x61, 0xbe, 0xdc, 0xfc, 0xbc, 0x95, 0xcf, 0xcd, 0x37, 0x3f, 0x5b, 0xd1,
		0x53, 0x39, 0x84, 0x3c, 0x41, 0xa2, 0x6d, 0x47, 0x14, 0x2a, 0x9e, 0x5d, 0x56, 0xf2, 0xd3, 0xab,
		0x44, 0x11, 0x92, 0xd9, 0x23, 0x20, 0x2e, 0x89, 0xb4, 0x7c, 0xb8, 0x26, 0x77, 0x99, 0xe3, 0xa5,
		0x67, 0x4a, 0xed, 0xde, 0xc5, 0x31, 0xfe, 0x18, 0x0d, 0x63, 0x8c, 0x80, 0xc0, 0xf7, 0x70, 0x07
	);
		// the exponentials (E) table from http://www.cs.utsa.edu/~wagner/laws/FFM.html
	$Antilog = array(
		0x01, 0x03, 0x05, 0x0f, 0x11, 0x33, 0x55, 0xff, 0x1a, 0x2e, 0x72, 0x96, 0xa1, 0xf8, 0x13, 0x35,
		0x5f, 0xe1, 0x38, 0x48, 0xd8, 0x73, 0x95, 0xa4, 0xf7, 0x02, 0x06, 0x0a, 0x1e, 0x22, 0x66, 0xaa,
		0xe5, 0x34, 0x5c, 0xe4, 0x37, 0x59, 0xeb, 0x26, 0x6a, 0xbe, 0xd9, 0x70, 0x90, 0xab, 0xe6, 0x31,
		0x53, 0xf5, 0x04, 0x0c, 0x14, 0x3c, 0x44, 0xcc, 0x4f, 0xd1, 0x68, 0xb8, 0xd3, 0x6e, 0xb2, 0xcd,
		0x4c, 0xd4, 0x67, 0xa9, 0xe0, 0x3b, 0x4d, 0xd7, 0x62, 0xa6, 0xf1, 0x08, 0x18, 0x28, 0x78, 0x88,
		0x83, 0x9e, 0xb9, 0xd0, 0x6b, 0xbd, 0xdc, 0x7f, 0x81, 0x98, 0xb3, 0xce, 0x49, 0xdb, 0x76, 0x9a,
		0xb5, 0xc4, 0x57, 0xf9, 0x10, 0x30, 0x50, 0xf0, 0x0b, 0x1d, 0x27, 0x69, 0xbb, 0xd6, 0x61, 0xa3,
		0xfe, 0x19, 0x2b, 0x7d, 0x87, 0x92, 0xad, 0xec, 0x2f, 0x71, 0x93, 0xae, 0xe9, 0x20, 0x60, 0xa0,
		0xfb, 0x16, 0x3a, 0x4e, 0xd2, 0x6d, 0xb7, 0xc2, 0x5d, 0xe7, 0x32, 0x56, 0xfa, 0x15, 0x3f, 0x41,
		0xc3, 0x5e, 0xe2, 0x3d, 0x47, 0xc9, 0x40, 0xc0, 0x5b, 0xed, 0x2c, 0x74, 0x9c, 0xbf, 0xda, 0x75,
		0x9f, 0xba, 0xd5, 0x64, 0xac, 0xef, 0x2a, 0x7e, 0x82, 0x9d, 0xbc, 0xdf, 0x7a, 0x8e, 0x89, 0x80,
		0x9b, 0xb6, 0xc1, 0x58, 0xe8, 0x23, 0x65, 0xaf, 0xea, 0x25, 0x6f, 0xb1, 0xc8, 0x43, 0xc5, 0x54,
		0xfc, 0x1f, 0x21, 0x63, 0xa5, 0xf4, 0x07, 0x09, 0x1b, 0x2d, 0x77, 0x99, 0xb0, 0xcb, 0x46, 0xca,
		0x45, 0xcf, 0x4a, 0xde, 0x79, 0x8b, 0x86, 0x91, 0xa8, 0xe3, 0x3e, 0x42, 0xc6, 0x51, 0xf3, 0x0e,
		0x12, 0x36, 0x5a, 0xee, 0x29, 0x7b, 0x8d, 0x8c, 0x8f, 0x8a, 0x85, 0x94, 0xa7, 0xf2, 0x0d, 0x17,
		0x39, 0x4b, 0xdd, 0x7c, 0x84, 0x97, 0xa2, 0xfd, 0x1c, 0x24, 0x6c, 0xb4, 0xc7, 0x52, 0xf6, 0x01
	);
	if ( $aa == 0  ||  $bb == 0 )
		return	0;
	$tt = $Log[$aa] + $Log[$bb];
	if ( $tt > 255 )
		$tt -= 255;
	return	$Antilog[$tt];
}				// GFmul()
#-----------------------------------------------------------------------------------------------------------------------------
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

	# input: cipherKey, a character array containing nKeys integers
	# output: keySchedule, an integer array with nKeys*(nRounds+1) elements
	# pack the (integers) cipherKeys into the base of the KeySchedule
	for ( $kk = 0; $kk < $nCipherKeys; $kk++ )
	{	$tmp = 0;
		for ( $rr = 0; $rr < 4; $rr++ )
		{	$t1 = ord( $cipherKey[$kk*4+$rr] ) << (8 * $rr);
			$tmp |= $t1;
#			printf( "tmp=%08x, t1=%02x<br>", $tmp, $t1 );
		}
		$KeySchedule[$kk] = $tmp;
	}
/*
	echo "KeyExpansion: KeySchedule=<br>";
	for ( $ii = 0; $ii < $nCipherKeys; $ii++ )
		printf( " %08x", $KeySchedule[$ii] );
	echo "<br>";
*/
	# According to:  http://www.quadibloc.com/crypto/co040401.htm, the first 36 round constants are:
	#  1   2   4   8  16  32  64 128  27  54 108 216 171  77 154  47  94 188
	# 99 198 151  53 106 212 179 125 250 239 197 145  57 114 228 211 189  97 ...
	# we only need the first 15 to cover 15 encryption rounds (nRounds + 1)
	$Rcon = array( 0 => 1, 2, 4, 8, 16, 32, 64, 128, 27, 54, 108, 216, 171, 77, 154 );
	for ( $kk = $nCipherKeys; $kk < $nRoundKeys; $kk++ )
	{	$temp = $KeySchedule[ $kk - 1 ];
		if ( ($kk % $nCipherKeys ) == 0 )
		{	$rconVal = $Rcon[ $kk / $nCipherKeys - 1 ];
			$substWord = SubWord( RotateWord($temp) );
#			printf( "temp=0x%08x, rconVal=0x%08x, substWord=0x%08x<br>",
#				$temp, $rconVal, $substWord );
			$temp = $substWord ^ $rconVal;
		}
		else if ( $nCipherKeys > 6  &&  ($kk % $nCipherKeys) == 4 )
			$temp = SubWord( $temp );
#		printf( "KeySchedule[%d] = 0x%08x, KeySchedule[%d]= 0x%08x, temp = 0x%08x<br>",
#				$kk, $KeySchedule[$kk], $kk-$nCipherKeys, $KeySchedule[$kk-$nCipherKeys], $temp );
		$KeySchedule[ $kk ] = $KeySchedule[ $kk - $nCipherKeys ] ^ $temp;
#		printf( "KeySchedule[%d] = 0x%08x<br>", $kk, $KeySchedule[$kk] );
	}
	return	$KeySchedule;
}			// KeyExpansion()
//------------------------------------------------------------------------------
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
#------------------------------------------------------------------------------
# see page 12 of Brian Gladman's Apr-03 v3.6 Specification for Rijndael, the AES Algorithm
# pp.11-12 of the following presentation is succinct:
# http://eee.ucc.ie/staff/marnanel/Files/handoutee5251/lec-AES2.pdf, but has a different matrix
# for for the forward cipher and no matrix for the inverse cipher.
#	A is a column of state; and C is the polynomial 03 x^3 + 01 x^2 + 01 x^1 + 02
# so (03 x^3 + 01 x^2 + 01 x^1 + 02) mod ( x^4 + 1 ) in a GF(2^8) Galois Field
# becomes the coefficient matrix below
# | b0 |	  | 02 01 01 03 | | a0 |
# | b1 |  =  | 03 02 01 01 | | a1 |
# | b2 |     | 01 03 02 01 | | a2 |
# | b3 |     | 01 01 03 02 | | a3 |
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
#		for ( $ii = 0; $ii < 4; $ii++ )
#			printf( " %x", $xx[$ii] );
		$state[4*$cc  ] =											# multiply by | 02 03 01 01 |
			GFmul(2,$xx[0]) ^ $xx[1] ^ $xx[2] ^ GFmul(3,$xx[3]);	# and add (xor in a Galois Field)
		$state[4*$cc+1] =											# multiply by | 01 02 03 01 |
			GFmul(3,$xx[0]) ^ GFmul(2,$xx[1]) ^ $xx[2] ^ $xx[3];	# and add (xor in a Galois Field)
		$state[4*$cc+2] =											# multiply by | 01 01 02 03 |
			$xx[0] ^ GFmul(3,$xx[1]) ^ GFmul(2,$xx[2]) ^ $xx[3];	# and add (xor in a Galois Field)
		$state[4*$cc+3] =											# multiply by | 03 01 01 02 |
			$xx[0] ^ $xx[1] ^ GFmul(3,$xx[2]) ^ GFmul(2,$xx[3]);	# and add (xor in a Galois Field)
	}
#	echo "<br>";
/*
	printf( "MixColumns: outgoing state=<br>" );
	for ( $ii = 0; $ii < $nColumns*4; $ii++ )
		printf( " %x", $state[$ii] );
	echo "<br>";
*/
	return	$state;
}			# MixColumns()
#------------------------------------------------------------------------------
# XOR each column of state with the keySchedule (a subset of the round key schedule)
# an example call looks like:
#		XorRoundKey( (unsigned long*)state, (unsigned long*)keySchedule+colNum );
# where rn is the round number
# XorRoundRey is its own inverse
function	XorRoundKey( $state, $KeySchedule, $roundNum, $nColumns )
{	# keySchedule is an array of integers
	# $keyIdx is the starting index of interest
	# state is an array of bytes
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
#		printf( "KeySchedule[%d]=0x%x<br>", $baseIdx + $cc, $key );
		for ( $rr = 0; $rr < 4; $rr++ )
		{	$mask = $key & 0xFF;
			$stii = $state[ $ii ];
			$state[ $ii ] = $stii ^ $mask;
#			printf( "%d: %x <-- %x ^ %x<br>", $ii, $state[$ii], $stii, $mask );
			$key = $key >> 8;			# for the next pass
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
function	ShiftRows8( $state )
{	# use with 256 bit blocks
	# bytes are stored column first, row second, e.g.
	# before...				afterwards, the array looks like...
	# 0  4  8 12 16 20 24 28		 0  4  8 12	16 20 24 28		leave the first row alone
	# 1  5  9 13 17 21	25 29		 5  9 13 17 21 25 29  1		circularly 'rotate' one byte left
	# 2  6 10 14 18 22 26 30  -->	14 18 22 26 30  2  6 10		circularly 'rotate' three bytes left
	# 3  7 11 15 19 23 27 31		19 23 27 31  3  7 11 15		circularly 'rotate' four bytes
	# refer to:  http://www.quadibloc.com/crypto/co040401.htm
/*
	printf( "ShiftRows8: incoming state=<br>" );
	for ( $cc = 0; $cc < 32; $cc++ )
		printf( " %x", $state[$cc] );
	echo "<br>";
*/														//  0
	$s1  = $state[ 1];	$state[ 1] = $state[ 5];		//  5
	$s2  = $state[ 2];	$state[ 2] = $state[14];		// 14
	$s3  = $state[ 3];	$state[ 3] = $state[19];		// 19
														//  4
						$state[ 5] = $state[ 9];		//  9
	$s6  = $state[ 6];	$state[ 6] = $state[18];		// 18
	$s7  = $state[ 7];	$state[ 7] = $state[23];		// 23
														//  8
						$state[ 9] = $state[13];		// 13
	$s10 = $state[10];	$state[10] = $state[22];		// 22
	$s11 = $state[11];	$state[11] = $state[27];		// 27
														// 12
						$state[13] = $state[17];		// 17
						$state[14] = $state[26];		// 26
	$s15 = $state[15];	$state[15] = $state[31];		// 31
														// 16
						$state[17] = $state[21];		// 21
						$state[18] = $state[30];		// 30
						$state[19] = $s3;				//  3
														// 20
						$state[21] = $state[25];		// 25
						$state[22] = $s2;				//  2
						$state[23] = $s7;				//  7
														// 24
						$state[25] = $state[29];		// 29
						$state[26] = $s6;				//  6
						$state[27] = $s11;				// 11
														// 28
						$state[29] = $s1;				//  1
						$state[30] = $s10;				// 10
						$state[31] = $s15;				// 15
/*
	printf( "ShiftRows8: outgoing state=<br>" );
	for ( $cc = 0; $cc < 32; $cc++ )
		printf( " %x", $state[$cc] );
	echo "<br>";
*/
	return	$state;
}			// ShiftRows8()
//------------------------------------------------------------------------------------------------
//$blockSize = $nCipherKeys * 4;
//$nRounds = $nCipherKeys + 6;
function	Rijndael256( $ibuf, $KeySchedule )
{	$nColumns = 8;						// Nc - could be 4, 6, or 8 columns for {128, 192, 256 bits}
	$nRounds = ( $nColumns + 6 );		// Nn - when key and block sizes are both 128 bits, i.e. 40 bytes
										// nRounds could be 10, 12, or 14
	$blockSize = $nColumns * 4;
	$unmixedRound = $nRounds - 1;		// where nRounds = 10, 12, or 14
	$ibuf = XorRoundKey( $ibuf, $KeySchedule, 0, $nColumns );
	$debug = 0;
	if ( $debug )
	{	printf( "Rijndael256: 1st XorRoundKey(0)-->" );
		for ( $cc = 0; $cc < $nColumns; $cc++ )
			for ( $rr = 0; $rr < 4 ; $rr++ )
				printf( " %02x", $ibuf[$cc*4+$rr] );
		echo "<br>";
	}
	for ( $rn = 1; $rn < $nRounds; $rn++ )			// note that round 0 already happened
	{	$ibuf = SubBytes( $ibuf, $blockSize );
		if ( $debug )
		{	printf( "Rijndael256: SubBytes(%d)-->", $rn );
			for ( $cc = 0; $cc < $nColumns; $cc++ )
				for ( $rr = 0; $rr < 4 ; $rr++ )
					printf( " %02x", $ibuf[$cc*4+$rr] );
			echo "<br>";
		}
		$ibuf = ShiftRows8( $ibuf );
		if ( $debug )
		{	printf( "Rijndael256: ShiftRows8(%d)-->", $rn );
			for ( $cc = 0; $cc < $nColumns; $cc++ )
				for ( $rr = 0; $rr < 4 ; $rr++ )
					printf( " %02x", $ibuf[$cc*4+$rr] );
			echo "<br>";
		}
		$ibuf = MixColumns( $ibuf, $nColumns );
		if ( $debug )
		{	printf( "Rijndael256: MixColumns(%d)-->", $rn );
			for ( $cc = 0; $cc < $nColumns; $cc++ )
				for ( $rr = 0; $rr < 4 ; $rr++ )
					printf( " %02x", $ibuf[$cc*4+$rr] );
			echo "<br>";
		}
		$ibuf = XorRoundKey( $ibuf, $KeySchedule, $rn, $nColumns );
		if ( $debug )
		{	printf( "Rijndael256: XorRoundKey(%d)-->", $rn );
			for ( $cc = 0; $cc < $nColumns; $cc++ )
				for ( $rr = 0; $rr < 4 ; $rr++ )
					printf( " %02x", $ibuf[$cc*4+$rr] );
			echo "<br>";
		}
	}
	$ibuf = SubBytes( $ibuf, $blockSize );
	if ( $debug )
	{	printf( "Rijndael256: SubBytes(%d)-->", $nRounds );
		for ( $cc = 0; $cc < $nColumns; $cc++ )
			for ( $rr = 0; $rr < 4 ; $rr++ )
				printf( " %02x", $ibuf[$cc*4+$rr] );
		echo "<br>";
	}
	$ibuf = ShiftRows8( $ibuf );
	if ( $debug )
	{	printf( "Rijndael256: ShiftRows8(%d)-->", $nRounds );
		for ( $cc = 0; $cc < $nColumns; $cc++ )
			for ( $rr = 0; $rr < 4 ; $rr++ )
				printf( " %02x", $ibuf[$cc*4+$rr] );
		echo "<br>";
	}
	$ibuf = XorRoundKey( $ibuf, $KeySchedule, $nRounds, $nColumns );
	if ( $debug )
	{	printf( "Rijndael256: XorRoundKey(%d)-->", $nRounds );
		for ( $cc = 0; $cc < $nColumns; $cc++ )
			for ( $rr = 0; $rr < 4 ; $rr++ )
				printf( " %02x", $ibuf[$cc*4+$rr] );
		echo "<br>";
	}
	return	$ibuf;
}			// Rijndael256()
//------------------------------------------------------------------------------
function	cfb256_encrypt( $ibuf, $bytesToGo, $iv, $KeySched )
{	$debug = true;
	# Refer to: http://en.wikipedia.org/wiki/Block_cipher_modesrequire_once('cryptest.php');require_once('GFmul.php');require_once('KeyExpansion.php');require_once('Rijndael128.php');require_once('Rijndael192.php');require_once('Rijndael256.php');require_once('Rijndael.php');require_once('Rijndael_modes.php');require_once('sbox.php');_of_operation
	$nColumns = 8;						# 256 bits --> 8 columns of 4 bytes
	$nRounds = ($nColumns + 6) + 1;		# Nn - when key and block sizes are both 128 bits, i.e. 40 bytes
										# nRounds could be 10, 12, or 14
	$blockSize = 4 * $nColumns;			# so, 32 bytes
	if ( $debug )
	{	printf( "cfb256_encrypt: ibuf=<br>" );
		for ( $ii = 0; $ii < $bytesToGo; $ii++ )
			printf( " %02x", ord($ibuf[$ii]) );
		echo "<br>";
	
		printf( "cfb256_encrypt: iv=<br>" );
		for ( $ii = 0; $ii < $blockSize; $ii++ )
			printf( " %x", $iv[$ii] );
		echo "<br>";
	
		$nRoundKeys = $nRounds * $nColumns;
		printf( "cfb256_encrypt: KeySched=<br>" );
		for ( $ii = 0; $ii < $nRoundKeys; $ii++ )
			printf( " %x", $KeySched[$ii] );
		echo "<br>";
	}
	$baseIndex = 0;						# in 'C', we'd use a pointer
#	$obuf = "";
	while ( $bytesToGo > 0 )
	{	$iv = Rijndael256( $iv, $KeySched );
		if ( $debug )
			printf( "baseIndex=%d<br>", $baseIndex );
		$loopLimit = $blockSize;
		if ( $bytesToGo < $blockSize )
			$loopLimit = $bytesToGo;
		if ( $debug )
			printf( "cfb_encrypt:" );
		for ( $ii = 0; $ii < $loopLimit; $ii++ )
		{	$tt = ord( $ibuf[ $baseIndex + $ii ] );		# unpack("C") didn't perform as expected, but ord() does !?
			$iv[ $ii ] = $iv[ $ii ] ^ $tt;
			$obuf[ $baseIndex + $ii ] = $iv[ $ii ];
			if ( $debug )
				printf( " %02x", $obuf[$baseIndex + $ii] );
		}
		if ( $debug )
			printf( "<br>" );
		$baseIndex += $loopLimit;
		$bytesToGo -= $loopLimit;
	}
	return	$obuf;
}			// cfb256_encrypt()
//------------------------------------------------------------------------------
function make_invoice_header( $installKeyDir, $orderid )
{	# returns the filename containing the invoice header unless something failed, then false
	$useBrowser = false;			# running directly in the browser, so we have to open and close the database ourselves
	$debug = false;					# write debug info to "debug_info.html" in the same directory as this script
#	$useSantaFe = true;				# defines the directory separator string appropriate to the platform

	$fname = $installKeyDir . $orderid . ".dat";
	
	if ( $debug )
	{	$fp_dbg = fopen( "make_invoice_header_debug.txt", "w" );
		if ( $fp_dbg ) fprintf( $fp_dbg, "%s\r\n", $fname );
	}
	if ( file_exists($fname) )
		return	$fname;				# do not recrypt what we already know

	if ( $fp_dbg ) fprintf( $fp_dbg, "entered make_invoice_header\r\n" );
	if ( $useBrowser )
	{	# connect with database and extract order info
		$sql_host ='localhost';
		$sql_db ='pnmxss';
		$sql_user ='SSordersReader';
		$sql_password ='kn2Gw7xT13a0pI5';
	
		$link = mysql_connect( $sql_host, $sql_user, $sql_password )
			or die( 'Could not connect: ' . mysql_error() );
	
		mysql_select_db( $sql_db )
			or die('Could not select database');
	}

	# Given an orderid, get the customer's name & email address
	$query = "SELECT firstname, lastname, email, date FROM xcart_orders WHERE orderid = '$orderid'";
	$result = mysql_query( $query )
		or die( 'Order Query failed: ' . mysql_error() );
	$numRes = mysql_num_rows( $result );
	if ( $numRes < 1 )
	{	if ( $useBrowser )
			mysql_close( $link );
		return	false;
	}
	if ( $fp_dbg ) fprintf( $fp_dbg, "numRes=%d\r\n", $numRes );
	$row = mysql_fetch_assoc( $result );

	$dateString = date( "Y-M-d", $row['date'] );			# in 2007-Mar-05 format
	if ( $fp_dbg ) fprintf( $fp_dbg, "%s\r\n", $dateString );
	$clearText = sprintf( "%s %s %s %d %s",
		$row['firstname'], $row['lastname'], $row['email'], $orderid, $dateString );
	mysql_free_result( $result );
	if ( $fp_dbg ) fprintf( $fp_dbg, "ClearText...\r\n%s\r\n", $clearText );
	
	$tokenCk = pack( "VVVVVVVV",
				0x9EE736CE, 0x63B1C14E, 0x85481852, 0x7BFC6C17,
				0xFBCB5D88, 0x15083952, 0x24E2E978, 0x7610035A  );
	$nCipherKeys = 8;

	# set up to encrypt
	if ( $fp_dbg )
	{	fprintf( $fp, "tokenCk...\r\n" );
		for ( $ii = 0; $ii < $blockSize; $ii++ )
			fprintf( $fp_dbg, " %02x", ord($tokenCk[$ii]) );
	}
	$textLen = strlen( $clearText );
	if ( $fp_dbg )
		fprintf( $fp_dbg, "\r\ntextLen = %d\r\n", $textLen );

	if ( $fp_dbg )
		fprintf( $fp_dbg, "nCipherKeys = %d\r\n", $nCipherKeys );		# just checking	
	$blockSize = 4 * $nCipherKeys;
	$nRounds = $nCipherKeys + 6;
	$nRoundKeys = $nCipherKeys * ( $nRounds + 1 );
	$KeySched = KeyExpansion( $tokenCk, $nCipherKeys );
	if ( $fp_dbg )
	{	fprintf( $fp_dbg, "KeySched...\r\n" );
		for ( $ii = 0; $ii < $nRoundKeys; $ii++ )
			fprintf( $fp_dbg, " %08x", $KeySched[$ii] );
	}
	$tokenIv =  pack( "VVVVVVVV",
					0xB25C1F15, 0x8BC0DDCF, 0x9FAE8C66, 0xA58EC640,
					0x94633916, 0x12FCCDE6, 0xDB2BA8F0, 0x5EAADB21	);
	for ( $kk = 0; $kk < $blockSize; $kk++ )			# create wideIv
		$wideIv[$kk] = ord( $tokenIv[$kk] );

	# encrypt the clear text
	$cryptText = cfb256_encrypt( $clearText, $textLen, $wideIv, $KeySched );
	if ( $fp_dbg )
	{	fprintf( $fp_dbg, "\r\ncryptText...\r\n" );
		for ( $ii = 0; $ii < $textLen; $ii++ )
			fprintf( $fp_dbg, " %02x", $cryptText[$ii] );
		fprintf( $fp_dbg, "\r\n" );
	}

	$fp = fopen( $fname, "w" );
	for ( $ii = 0; $ii < $textLen; $ii++ )
	   fwrite( $fp, chr($cryptText[$ii]), 1 );			# the encrypted message
	fclose( $fp );

	if ( $useBrowser )
		mysql_close( $link );
	if ( $fp_dbg )
		fclose( $fp_dbg );

	return  $fname;										# were returning $cryptText
}
#-----------------
# if testing in a browser, uncomment the following...
$debug = 0;
if ( $debug )
{	# put back the initial "?"
	$queryString = "?" . $_SERVER[ 'QUERY_STRING' ];
	echo "queryString='$queryString'<br>";
	# expecting to find:  '?in=<orderid>'
	include_once( "my_parse_str.php" );
	my_parse_str( $queryString, $paramsArr );
	$orderid = $paramsArr['in'];
	echo "<br>orderid=$orderid<br>";
	$fname = make_invoice_header( "c:\\htdocs\\Install_Process\\attach\\", $orderid );
	echo "fname='$fname'<br>";
}
?>

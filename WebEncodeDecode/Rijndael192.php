<?php
$nColumns = 6;					// 256 bits --> 8 (columns) 32 bit words
$blockSize = 4 * $nColumns;		// so, 32 bytes

//require_once( "sbox.php" );
require_once( "GFmul.php" );
require_once( "Rijndael.php" );

function ShiftRows6( $state )
{		// use with 192 bit blocks
		// bytes are stored column first, row second, e.g.
		// before...			afterwards, the array looks like...
		// 0  4  8 12 16 20			 0  4  8 12	16 20	leave the first row alone
		// 1  5  9 13 17 21			 5  9 13 17 21  1	circularly 'rotate' one byte left
		// 2  6 10 14 18 22  --->	10 14 18 22  2  6	circularly 'rotate' two bytes left
		// 3  7 11 15 19 23			15 19 23  3  7 11	circularly 'rotate' three bytes
		// refer to:  http://www.quadibloc.com/crypto/co040401.htm
														//  0
	$s1  = $state[ 1];	$state[ 1] = $state[ 5];		//  5
	$s2  = $state[ 2];	$state[ 2] = $state[10];		// 10
	$s3  = $state[ 3];	$state[ 3] = $state[15];		// 15
														//  4
						$state[ 5] = $state[ 9];		//  9
	$s6  = $state[ 6];	$state[ 6] = $state[14];		// 14
	$s7  = $state[ 7];	$state[ 7] = $state[19];		// 19
														//  8
						$state[ 9] = $state[13];		// 13
						$state[10] = $state[18];		// 18
	$s11 = $state[11];	$state[11] = $state[23];		// 23
														// 12
						$state[13] = $state[17];		// 17
						$state[14] = $state[22];		// 22
						$state[15] = $s3;				//  3
														// 16
						$state[17] = $state[21];		// 21
						$state[18] = $s2;				//  2
						$state[19] = $s7;				//  7
														// 20
						$state[21] = $s1;				//  1
						$state[22] = $s6;				//  6
						$state[23] = $s11;				// 23
	return	$state;
}			// ShiftRows6()
//------------------------------------------------------------------------------
// requires generation of a keySchedule
function	Rijndael192( $ibuf, $keySchedule )
{	$nColumns = 6;						// Nc - could be 4, 6, or 8 columns for {128, 192, 256 bits}
	$nCells = $nColumns * 4;
	$nRounds = ($nColumns + 6) + 1;		// Nn - when key and block sizes are both 128 bits, i.e. 40 bytes
										// nRounds could be 10, 12, or 14
	$unmixedRound = $nRounds - 1;		// where nRounds = 10, 12, or 14
	$ibuf = XorRoundKey( $ibuf, $keySchedule, 0, $nColumns );
	for ( $rn = 1; $rn < $unmixedRound; $rn++ )		// note that round 0 already happened
	{	$ibuf = SubBytes( $ibuf, $nCells );
		$ibuf = ShiftRows6( $ibuf );
		$ibuf = MixColumns( $ibuf, $nColumns );
		$ibuf = XorRoundKey( $ibuf, $keySchedule, $rn, $nColumns );
	}
	$ibuf = SubBytes( $ibuf, $nCells );
	$ibuf = ShiftRows6( $ibuf );
	$ibuf = XorRoundKey( $ibuf, $keySchedule, $unmixedRound, $nColumns );
	return	$ibuf;
}			// Rijndael192()
//------------------------------------------------------------------------------
?>
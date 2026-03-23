<?php
$nColumns = 8;					// 256 bits --> 8 (columns) 32 bit words
$blockSize = 4 * $nColumns;		// so, 32 bytes

require_once( "GFmul.php" );
require_once( "Rijndael.php" );

function	ShiftRows8( $state )
{		// use with 256 bit blocks
		// bytes are stored column first, row second, e.g.
		// before...				afterwards, the array looks like...
		// 0  4  8 12 16 20 24 28		 0  4  8 12	16 20 24 28		leave the first row alone
		// 1  5  9 13 17 21	25 29		 5  9 13 17 21 25 29  1		circularly 'rotate' one byte left
		// 2  6 10 14 18 22 26 30  -->	14 18 22 26 30  2  6 10		circularly 'rotate' three bytes left
		// 3  7 11 15 19 23 27 31		19 23 27 31  3  7 11 15		circularly 'rotate' four bytes
		// refer to:  http://www.quadibloc.com/crypto/co040401.htm
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
{		// Refer to: http://en.wikipedia.org/wiki/Block_cipher_modesrequire_once('cryptest.php');require_once('GFmul.php');require_once('KeyExpansion.php');require_once('Rijndael128.php');require_once('Rijndael192.php');require_once('Rijndael256.php');require_once('Rijndael.php');require_once('Rijndael_modes.php');require_once('sbox.php');_of_operation
	$nColumns = 8;						// 256 bits --> 8 columns of 4 bytes
	$nRounds = ($nColumns + 6) + 1;		// Nn - when key and block sizes are both 128 bits, i.e. 40 bytes
										// nRounds could be 10, 12, or 14
	$blockSize = 4 * $nColumns;			// so, 32 bytes
/*
	printf( "cfb256_encrypt: ibuf=<br>" );
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
*/
	$baseIndex = 0;					// in 'C', we'd use a pointer
	while ( $bytesToGo > 0 )
	{	$iv = Rijndael256( $iv, $KeySched );

//		printf( "baseIndex=%d<br>", $baseIndex );
		$loopLimit = $blockSize;
		if ( $bytesToGo < $blockSize )
			$loopLimit = $bytesToGo;
//		printf( "cfb_encrypt:" );
		for ( $ii = 0; $ii < $loopLimit; $ii++ )
		{	$tt = ord( $ibuf[ $baseIndex + $ii ] );		// unpack("C") didn't perform as expected, but ord() does !?
			$iv[ $ii ] = $iv[ $ii ] ^ $tt;
			$obuf[ $baseIndex + $ii ] = $iv[ $ii ];
//			printf( " %02x", $obuf[$baseIndex + $ii] );
		}
//		printf( "<br>" );
		$baseIndex += $loopLimit;
		$bytesToGo -= $loopLimit;
	}
	return	$obuf;
}			// cfb256_encrypt()
//------------------------------------------------------------------------------
function	cfb256_decrypt( $ibuf, $bytesToGo, $iv, $KeySched )
{		// Refer to: http://en.wikipedia.org/wiki/Block_cipher_modes_of_operation
	$nColumns = 8;					// 256 bits --> 8 columns of 4 bytes
	$nRounds = ($nColumns + 6) + 1;	// Nn - when key and block sizes are both 128 bits, i.e. 40 bytes
									// nRounds could be 10, 12, or 14
	$blockSize = 4 * $nColumns;		// so, 32 bytes
/*
	printf( "<br>cfb256_decrypt: ibuf=<br>" );
	for ( $ii = 0; $ii < $bytesToGo; $ii++ )
		printf( " %02x", $ibuf[$ii] );
	echo "<br>";

	printf( "cfb256_decrypt: iv=<br>" );
	for ( $ii = 0; $ii < $blockSize; $ii++ )
		printf( " %x", $iv[$ii] );
	echo "<br>";

	$nRoundKeys = $nRounds * $nColumns;
	printf( "cfb256_decrypt: KeySched=<br>" );
	for ( $ii = 0; $ii < $nRoundKeys; $ii++ )
		printf( " %x", $KeySched[$ii] );
	echo "<br>";
*/
	$baseIndex = 0;					// in 'C', we'd use a pointer
	while ( $bytesToGo > 0 )
	{	$iv = Rijndael256( $iv, $KeySched );
		$loopLimit = $blockSize;
		if ( $bytesToGo < $blockSize )
			$loopLimit = $bytesToGo;
		for ( $ii = 0; $ii < $loopLimit; $ii++ )
		{		// ord() isn't necessary when cfb256_encrypt creates input
				// but must be used if MIMEdecode creates it...
			$tt = ord( $ibuf[$baseIndex+$ii] );
			$obuf[$baseIndex+$ii] = $tt ^ $iv[$ii];
			$iv[$ii] = $tt;
		}
		$baseIndex += $loopLimit;
		$bytesToGo -= $loopLimit;
	}
	return	$obuf;
}			// cfb256_decrypt()
//------------------------------------------------------------------------------
?>
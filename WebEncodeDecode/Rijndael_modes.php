<?php
require_once( "Rijndael256.php" );			// defines $blockSize

function cfb256_encrypt( $ibuf, &$obuf, $iv, $keySched )
{		// Refer to: http://en.wikipedia.org/wiki/Block_cipher_modesrequire_once('cryptest.php');require_once('GFmul.php');require_once('KeyExpansion.php');require_once('Rijndael128.php');require_once('Rijndael192.php');require_once('Rijndael256.php');require_once('Rijndael.php');require_once('Rijndael_modes.php');require_once('sbox.php');_of_operation
	$bytesToGo = strlen( $ibuf );
	while ( $bytesToGo > 0 )
	{	Rijndael256( $iv, $iv, $keySched );
		$loopLimit = $blockSize;
		if ( $bytesToGo < $blockSize )
			$loopLimit = $bytesToGo;
		for ( $ii = 0; $ii < $loopLimit; $ii++ )
		{	$iv[$ii] = $iv[$ii] ^ $ibuf[$ii];
			$obuf[$ii] = $iv[$ii];
		}
		$bytesToGo -= $loopLimit;
	}
}			// cfb256_encrypt()
//-----------------------------------------------------------------------
function cfb256_decrypt( $ibuf, &$obuf, $iv, $keySched )
{		// Refer to: http://en.wikipedia.org/wiki/Block_cipher_modes_of_operation
	$bytesToGo = strlen( $ibuf );
	while ( $bytesToGo > 0 )
	{	Rijndael256( $iv, $iv, $keySched );
		$loopLimit = $blockSize;
		if ( $bytesToGo < $blockSize )
			$loopLimit = $bytesToGo;
		for ( $ii = 0; $ii < $loopLimit; $ii++ )
		{	$tt = $ibuf[$ii];
			$obuf[$ii] = $tt ^ $iv[$ii];
			$iv[$ii] = $tt;
		}
		$bytesToGo -= $loopLimit;
	}
}			// cfb256_decrypt()
//-----------------------------------------------------------------------
?>
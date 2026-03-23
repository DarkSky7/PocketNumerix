<?php
	include_once( "Rijndael256.php" );		    	// defines $blockSize
	include_once( "KeyExpansion.php" );
	include_once( "InitializationVector.php" );		// defines $InitializationVector
	include_once( "CipherKey.php" );
	include_once( "MimeDecode.php" );
	include_once( "MimeEncode.php" );

	echo "<br>CipherKey=<br>";
	for ( $ii = 0; $ii < $blockSize; $ii++ )
		printf( " %02x", ord($CipherKey[$ii]) );					// works!

	$params = "?os=5.1.1702?dv=618754147?em=mlandis@acm.org?in=90234023?dt=2005-10-13?ln=Landis?fn=Mike";
	echo "<br>params...<br>";
	$paramsLen = strlen( $params );
	for ( $ii = 0; $ii < $paramsLen; $ii++ )
		printf( "%s", $params[$ii] );

	$nRounds = $nCipherKeys + 6;
	$nRoundKeys = $nCipherKeys * ( $nRounds + 1 );
	$KeySched = KeyExpansion( $CipherKey, $nCipherKeys );
	echo "<br>KeySched...<br>";
	for ( $ii = 0; $ii < $nRoundKeys; $ii++ )
		printf( " %08x", $KeySched[$ii] );

	for ( $kk = 0; $kk < $blockSize; $kk++ )						// create wideIv
		$wideIv[$kk] = ord( $InitializationVector[$kk] );

		// encrypt
	$cryptParams = cfb256_encrypt( $params, $paramsLen, $wideIv, $KeySched );
	echo "<br>encrypted params...<br>";
	for ( $ii = 0; $ii < $paramsLen; $ii++ )
		printf( " %02x", $cryptParams[$ii] );

		// encode
	$paramsLen = strlen( $params );
	$b64CryptParamsLen = MIMEencode( $cryptParams, $paramsLen, $b64CryptParams );
	echo "<br>encode: b64 encrypted params...<br>";
	for ( $ii = 0; $ii < $b64CryptParamsLen; $ii++ )
		printf( " %02x", ord($b64CryptParams[$ii] ) );
	echo "<br>... as a string ...<br>" . $b64CryptParams;

		// decode
	$decodeB64CryptParamsLen = MIMEdecode( $b64CryptParams, $b64CryptParamsLen, $decodeB64CryptParams );
	echo "<br>decode: decoded b64 encrypted params...<br>";
	for ( $ii = 0; $ii < $decodeB64CryptParamsLen; $ii++ )
		printf( " %02x", ord($decodeB64CryptParams[$ii]) );

		// decrypt the decoded string/array(?)
	$decryptParams = cfb256_decrypt( $decodeB64CryptParams, $paramsLen, $wideIv, $KeySched );
//	$decryptParams = cfb256_decrypt( $cryptParams, $paramsLen, $wideIv, $KeySched );		// -> array of integer
	echo "<br>decryptParams...<br>";
		// this following loop converts the array returned by cfb_decrypt above into a string.
		// doing so isn't necessary when cfb_encrypt creates our input (go figure)
	$str = "";
	for ( $ii = 0; $ii < $paramsLen; $ii++ )
	{	printf( " %02x", $decryptParams[$ii] );
		$str .= chr( $decryptParams[$ii] );
	}
	echo "<br>... as a string ...<br>" . $str;
//--------------------------------------------------------------------------------------------
/*
	echo "CipherKey...<br>";						// . $CipherKey . "<br>";
	for ( $ii = 0; $ii < $blockSize; $ii++ )
		printf( " %02x", ord($CipherKey[$ii]) );

	$nRounds = $nCipherKeys + 6;
	$nRoundKeys = $nCipherKeys * ( $nRounds + 1 );
	$KeySched = KeyExpansion( $CipherKey, $nCipherKeys );
	echo "<br>KeySched...<br>";
	for ( $ii = 0; $ii < $nRoundKeys; $ii++ )
		printf( " %08x", $KeySched[$ii] );
		// the $ds value below is from the C++ MimeEncodeDecode project and agrees with our output at that stage
//		   123456789 123456789 123456789 123456789 123456789 123456789
	$ds = "acPNJ059H//VKdS7XDp44pbCsM33pkcnhfGJMFsHp0XqKx9S+dNoTy+hqP9Lqg7erOmD7NO3LYfY2AhyTpwCQYl3ftAir9T7QUBrpF3U8pKjkTlQFpy1lg==
	$dsLen = strlen( $ds );
	echo "<br>dsLen=" . $dsLen;
	echo "<br>ds...<br>" . $ds;

	$b64dsLen = MimeDecode( $ds, $dsLen, $b64ds );
	echo "<br>b64dsLen=" . $b64dsLen;
	echo "<br>b64ds...<br>";
	for ( $ii = 0; $ii < $b64dsLen; $ii++ )
		printf( " %02x", $b64ds[$ii] );

		// decrypt the decoded MIME string
	for ( $kk = 0; $kk < $blockSize; $kk++ )						// create wideIv
		$wideIv[$kk] = ord( $InitializationVector[$kk] );
	echo "<br>wideIv=";
	for ( $ii = 0; $ii < $blockSize; $ii++ )
		printf( " %02x", $wideIv[$ii] );
	echo "<br>";

	$params = cfb256_decrypt( $ds, $dsLen, $wideIv, $KeySched );	// wideIv gets modified internally
	echo "<br>params=<br>";
	for ( $ii = 0; $ii < $dsLen; $ii++ )
		printf( " %02x", $params[$ii] );
	echo "<br>... as a string ...<br>";
	for ( $ii = 0; $ii < $dsLen; $ii++ )
		printf( "%s", chr($params[$ii]) );
*/
?>
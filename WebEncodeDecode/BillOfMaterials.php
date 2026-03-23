<?php
	function NumToBase62( $num )
	{		// convert non-negative integers to base 62
		$result = "";
		if ( $num >= 0 )
		{	do
			{	$quo = (int)( $num / 62 );
				$rem = $num - 62 * $quo;
				if ( $rem < 10  )
					$result = $rem  . $result;					// -0 + 48 ('0') ... chr(0..9) is broken
				else if ( 10 <= $rem  &&  $rem < 36 )
					$result = chr( $rem + 87 ) . $result;		// -10 + 97 ('a')
				else											// hopefully,  36 <= $num  &&  $num < 62
					$result	= chr( $rem  + 29 ) . $result;		// -36 + 65 ('A')
				$num = $quo;
			} while ( $num > 0 );
		}
		return	$result;
	}

	function MakeName()
	{
		$rndy = mt_rand();
		$name = NumToBase62( $rndy );
		$rndy = mt_rand();
		$name = $name . NumToBase62( $rndy );
		$rndy = mt_rand();
		$name = $name . NumToBase62( $rndy );
		$rndy = mt_rand();
		$name = $name . NumToBase62( $rndy );
		return	$name;
	}

	function AddToBOM( $productName, $ver, $downDirSuffix, $cabName, &$result, &$aggSize )
	{	$downloadDir = 'Downloads/' . $downDirSuffix . '/';
		if ( ! file_exists( $downloadDir ) )
			mkdir( $downloadDir );

		$suppliesDir = 'Supplies/';
		if ( $ver == '5')
			$prefix = 'WM5_';
		else if ( $ver == '4' )
			$prefix = 'PPC03_';
		else						// $ver == 3
			$prefix = 'PPC02_';
//		echo "prefix=" . $prefix . "\n";

			// set up to copy the hidden file into the Downloads/$rndy directory
		$hiddenCab = $suppliesDir . $prefix . $cabName;
//		echo "hiddenCab=" . $hiddenCab . "\n";

		$tgtFilename = MakeName();
//		echo "tgtFilename=" . $tgtFilename . "\n";

		$downloadPath = $downloadDir . $tgtFilename;
//		echo "downloadPath=" . $downloadPath . "\n";

		$okay = copy( $hiddenCab, $downloadPath );
//		echo "okay=" . $okay . "\n";

		$stats = stat( $downloadPath );
		if ( ! $stats )
		{	echo "stats is false!\n";
			exit;
		}
		$size = $stats['size'];
//		echo $size;
		$aggSize = $aggSize + $size;
//		echo $aggSize;

			// for each item on the invoice ...
			// generate a random number representing the location of the file to download
		$hiddenFileName = $prefix . $productName;
//		echo $hiddenFileName;
			// the format of the BOM response is...
			// product name     fileSize   location (not incl. 'Downloads/' or $rndy)
//		echo "productName=$productName\n";
//		echo "size=$size\n";
//		echo "exeName=$exeName\n";
		$result .= "$productName $size $tgtFilename\n";		// 'Downloads' directory is assumed
	}

		//----------------------------------------------------------------------------------//
		// 								  params retrieval									//
		//----------------------------------------------------------------------------------//
	$debug = 1;
	$queryString = $HTTP_SERVER_VARS[ 'QUERY_STRING' ];
	parse_str( $queryString, $paramsArr );
//	foreach( $paramsArr as $key => $val )
//		printf( "%s => %s<br>", $key, $val );

		// expecting to find:  ds parameter which ultimately contains:  os, dv, em, in, dt, ln, fn
	$ds = $paramsArr['ds'];
//	$ds = "acPNJ059H//VKdS7XDp44pbCsM33pkcnhfGBMVsHp0XFVpXAmfKEdf144G8WImwf4e03u3qT+Dp/F/Agp44Rytc0ms2s85SkzVFU3ot4qaxf2CZsLg/oGw==";
	echo "ds=" . $ds . "<br>";
	$dslen = strlen( $ds );
	include_once("MimeDecode.php");
	$cryptParamsLen = MIMEdecode( $ds, $dslen, $cryptParams );
	if ( $debug )
	{	echo "cryptParams(" . $cryptParamsLen . ")...<br>";
		for ( $ii = 0; $ii < $cryptParamsLen; $ii++ )
			printf( " %02x",  ord( $cryptParams[$ii] ) );
		echo "<br>";
	}

	include_once("CipherKey.php");									// defines $CipherKey, $nCipherKeys
	$nRounds = $nCipherKeys + 6;
	$nRoundKeys = $nCipherKeys * ( $nRounds + 1 );
	include_once("KeyExpansion.php");
	$KeySched = KeyExpansion( $CipherKey, $nCipherKeys );

		// create a byte oriented version of initialization vector
	include_once("Rijndael256.php");								// defines $blockSize
	include_once("InitializationVector.php");						// defines $InitializationVector
	for ( $kk = 0; $kk < $blockSize; $kk++ )
		$wideIv[$kk] = ord( $InitializationVector[$kk] );			// appears to work

	$decryptParamsArr = cfb256_decrypt( $cryptParams, $cryptParamsLen, $wideIv, $KeySched );
	$decryptParams = "";
	if ( $debug )
	{	echo "decryptParams(" . $cryptParamsLen  . ") ...<br>" . $decryptParams;
		for ( $ii = 0; $ii < $cryptParamsLen; $ii++ )
		{	printf( " %02x", $decryptParamsArr[$ii] );
			$decryptParams .= chr( $decryptParamsArr[$ii] );
		}
		echo "<br>... as string ...<br>" . $decryptParams;
	}
	die();

		//----------------------------------------------------------------------------------//
		// 									authentication									//
		//----------------------------------------------------------------------------------//
//	$shortVer = $ver[0];						// is this definitive?

		// pretend that we just queried the database for invoice number, and
		// verified that the other four fields match with the data we have

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// create a directory name for the downloads
	$downDirSuffix = MakeName();		// the target download directory
//	echo "downDirSuffix=" . $downDirSuffix . "<br>";

		// copy the appropriate PpcInstall.EXE to the Downloads directory
	$msg = "";
	$aggSize = 0;
	AddToBOM( 'PpcInstall', $shortVer, $downDirSuffix, 'PpcInstall.CRP', $msg, $aggSize );

		// we're not going to count PpcInstall's size as part of memory required
	$aggSize = 0;				// reset aggSize

		// for all items on the invoice...
	$nItems = 0;

	AddToBOM( 'BondManager',	   $shortVer, $downDirSuffix, 'BondManager.CRP',	  $msg, $aggSize );
	$nItems++;

	AddToBOM( 'NillaHedge', 	   $shortVer, $downDirSuffix, 'NillaHedge.CRP', 	  $msg, $aggSize );
	$nItems++;

	AddToBOM( 'StrategyExplorer',  $shortVer, $downDirSuffix, 'StrategyExplorer.CRP', $msg, $aggSize );
	$nItems++;

	AddToBOM( 'YieldCurveFitter',  $shortVer, $downDirSuffix, 'YieldCurveFitter.CRP', $msg, $aggSize );
	$nItems++;

		// copy any necessary MFC8.0 DLLs into the Downloads directory
	if ( $nItems > 0  &&  $shortVer > '3' )
	{		// PPC02 ('3') doesn't need libraries, but
			// PPC03 ('4') and WM5 ('5') targets need MFC8.0 DLLs

		AddToBOM( 'mfcDLLs', $shortVer, $downDirSuffix, 'DLLs.CRP', $msg, $aggSize );		// add WM5_DLLs.CAB (encrypted as .CRP)
	}
	$msg = $downDirSuffix . " " . $aggSize . "\n" . $msg;
	$msgLen = strlen( $msg );
		// everything to be included in the response is now in $outBuf

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// set up an encryption run
include_once("CipherKey.php");								// defines $CipherKey and $nCipherKeys
include_once("InitializationVector.php");					// defines $InitializationVector

		// build the round key schedule
	$nRounds = $nCipherKeys + 8;
	$nRoundKeys = $nCipherKeys * ( $nRounds + 1 );
	$KeySched = KeyExpansion( $CipherKey, $nCipherKeys );

		// encrypt the message
	for ( $kk = 0; $kk < $blockSize; $kk++ )
		$wideIv[$kk] = ord($InitializationVector[$kk] );
	$CipherText = cfb256_encrypt( $msg, $msgLen, $wideIv, $KeySched );

		// echo the binary to the stream...
	for ( $cc = 0; $cc < $msgLen; $cc++ )
		printf( "%s", chr( $CipherText[$cc]) );

		//  That's all folks!

		// for a man readable HEX view of $CipherText, use this:
//	echo "<br>CipherText=<BR>";
//	for ( $cc = 0; $cc < $msgLen; $cc++ )
//		printf( " %02x", $CipherText[$cc] );
//	echo "<br>";


/*		// the mCrypt way...
		// Open the cipher, set up its iv, and send it
	$td = mcrypt_module_open('rijndael-256', '', 'cfb', '');
	$iv = mcrypt_create_iv(mcrypt_enc_get_iv_size($td), MCRYPT_DEV_RANDOM );

		// set up a key and send it
	$key = sha1( mt_rand() );
	$key = substr( $key, 0, mcrypt_enc_get_key_size($td) );

		// encrypt the message and send it
	mcrypt_generic_init( $td, $key, $iv );
	$ciphertext = mcrypt_generic( $td, $outBuf );
	mcrypt_generic_deinit( $td );
	mcrypt_module_close( $td );
*/
?>

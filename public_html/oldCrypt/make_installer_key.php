<?php
include_once( "KeyExpansion.php" );

function make_installer_key( $installKeyDir, $orderid )
{
    include_once( "Rijndael256.php" );		    	// defines $blockSize
    include_once( "InitializationVector.php" );		// defines $InitializationVector
    include_once( "CipherKey.php" );

    # returns the cryptKey unless something failed, then false
    # connect with database and extract order info
/*
    $sql_host ='localhost';
    $sql_user ='SSordersReader';
    $sql_db ='pnmxss';
    $sql_password ='kn2Gw7xT13a0pI5';

    $link = mysql_connect( $sql_host, $sql_user, $sql_password )
	or die( 'Could not connect: ' . mysql_error() );

    mysql_select_db( $sql_db )
	or die('Could not select database');
*/
    # Given an orderid, get the customer's name & email address
    $query = "SELECT firstname, lastname, email, date FROM xcart_orders WHERE orderid = '$orderid'";
    $result = mysql_query( $query )
	or die( 'Order Query failed: ' . mysql_error() );
    $numRes = mysql_num_rows( $result );
    if ( $numRes < 1 )
	return	false;
#    printf( "numRes=%d<br>", $numRes );
    $row = mysql_fetch_assoc( $result );

    $dateString = date( "Y-m-d", $row['date'] );
#   printf( "%s<br>", $dateString );
    $clearText = sprintf( "%s %s %s %d %s",
	$row['firstname'], $row['lastname'], $row['email'], $orderid, $dateString );
    mysql_free_result( $result );
#    printf( "ClearText...<br>%s<br>", $clearText );
    
    # set up to encrypt
#    echo "CipherKey...<br>";
#    for ( $ii = 0; $ii < $blockSize; $ii++ )
#       printf( " %02x", ord($CipherKey[$ii]) );					// works!
    $textLen = strlen( $clearText );
#    printf( "<br>textLen = %d<br>", $textLen );

#    printf( "nCipherKeys = %d<br>", $nCipherKeys );				// just checking    
    $nRounds = $nCipherKeys + 6;
    $nRoundKeys = $nCipherKeys * ( $nRounds + 1 );
    $KeySched = KeyExpansion( $CipherKey, $nCipherKeys );
#    echo "KeySched...<br>";
#    for ( $ii = 0; $ii < $nRoundKeys; $ii++ )
#	printf( " %08x", $KeySched[$ii] );

    for ( $kk = 0; $kk < $blockSize; $kk++ )						// create wideIv
	$wideIv[$kk] = ord( $InitializationVector[$kk] );

    # encrypt the clear text
    $cryptText = cfb256_encrypt( $clearText, $textLen, $wideIv, $KeySched );
#    echo "<br>cryptText...<br>";
#    for ( $ii = 0; $ii < $textLen; $ii++ )
#	printf( " %02x", $cryptText[$ii] );
#    echo "<br>";

    $fname = $installKeyDir . "/" . $orderid . ".dat";
    $fp = fopen( $fname, "w" );
    for ( $ii = 0; $ii < $textLen; $ii++ )
        fwrite( $fp, chr($cryptText[$ii]), 1 );
    fclose( $fp );

    return  $cryptText;
}
//    $arr = make_installer_key( "install_key", 1 );
?>

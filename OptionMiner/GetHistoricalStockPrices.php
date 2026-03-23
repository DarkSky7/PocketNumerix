<?php
inlude_once("SharpeLogin.php");

$debug = true;

$link = mysql_connect( $sql_host, $sql_user, $sql_password ) or die( "q1: Couldn't connect: " .  mysql_error() );
if ( ! mysql_select_db( $sql_db, $link ) )
	die( "q2" );		                                        # 	die( "Couldn't select database" );

# NASDAQ:CSCO 

$sym = $_REQUEST['StockSymbol'];


# for each line that starts with a date (lines between the <body> and </body> tags):
# break the line into tokens separate by commas,
# specifically: date, open, high, low, close, volume
# 
?>

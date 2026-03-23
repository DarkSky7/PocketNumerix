<?php
function NumToBase62( $num )
{		# convert non-negative integers to base 62 strings
#    global $debug;
	$num = abs( $num );
    if ( $debug )
        printf( "NumToBase62:num=%d<br>", $num );
	$str = "";
	do
	{	$quo = intval( $num / 62 );
        if ( $debug )
            printf( "quo=%d<br>", $quo );
		$rem = $num - (62 * $quo);
        if ( $debug )
            printf( "rem=%d<br>", $rem );

		if ( $rem < 10 )
			$str .= chr( $rem + 48 );			# -0 + 48 ('0')
		else if ( 10 <= $rem  &&  $rem < 36 )
			$str .= chr( $rem + 87 );			# -10 + 97 ('a')
		else									# hopefully,  36 <= $num  &&  $num < 62
			$str .= chr( $rem  + 29 );			# -36 + 65 ('A')
		$num = $quo;
	} while ( $num > 0 );
    if ( $debug )
        printf( "NumToBase62:str='%s'<br>", $str );
	return	$str;
}			# NumToBase62()
#--------------------------------------------------------------------------------------------
function MakeRanName()
{	$rndy = mt_rand();
	$n1 = NumToBase62( $rndy );

	$rndy = mt_rand();
	$n2 = NumToBase62( $rndy );

	$rndy = mt_rand();
	$n3 = NumToBase62( $rndy );

	$rndy = mt_rand();
	$n4 = NumToBase62( $rndy );

	return	$n1 . $n2 . $n3 . $n4;
}			# MakeRanName()
#--------------------------------------------------------------------------------------------
?>
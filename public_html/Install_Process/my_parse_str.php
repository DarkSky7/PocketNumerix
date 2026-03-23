<?php
function my_parse_str( $str, &$arr )
{	# work son strings of the form "?var1=<value1>?var2=<value2>..."
	$debug = false;
	# works like parse_str(), but doesn't substitute spaces for '+' signs
	$slen = strlen( $str );
	if ( $debug )
		printf( "<br>slen=%d<br>", $slen );
	$arr = array();							# the return value
	$nn = 0;
	while ( $nn < $slen )
	{
		if ( $debug )
			printf( "nn=%d<br>", $nn );
		$jj = stripos( $str, "?", $nn );
		if ( $jj === FALSE )
			break;
		$jj++;								# just past the '?'

		if ( $debug )
			printf( "jj=%d<br>", $jj );
		$kk = stripos( $str, "=", $jj );
		if ( $kk === FALSE)
			break;

		if ( $debug )
			printf( "kk=%d<br>", $kk );
		$len = $kk - $jj;

		if ( $debug )
		{	printf( "len=%d<br>...key chars: ", $len );
			for ( $ii = $jj; $ii < $kk; $ii++ )
				printf( "%s", $str[$ii] );
		}
		$key = substr( $str, $jj, $len );
		if ( $debug )
			printf( "<br>key=%s<br>", $key );

		# now for the value
		$mm = $kk + 1;					# just past the '='
		if ( $debug )
			printf( "mm=%d<br>", $mm );
		$nn = stripos( $str, "?", $mm );
		if ( $debug )
			printf( "nn=%d<br>", $nn );
		if ( $nn === FALSE )
		{	# no more '?'s in $str
			$val = substr( $str, $mm );			# to EOS
			if ( $debug )
				printf( "...0_val=%d<br>", $val );
			$nn = $slen;				# terminates the while
		}
		else						# there are more keys
		{	$len = $nn - $mm;
			if ( $debug )
				printf( "len=%d<br>", $len );
			$val = substr( $str, $mm, $len );
			if ( $debug )
				printf( "...1_val=%d<br>", $val );
		}
		$arr[$key] = $val;
	}
}
?>
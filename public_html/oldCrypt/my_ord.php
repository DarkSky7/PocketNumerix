<?php
function my_ord( $arg )
{
//	return	ord( $arg );
//	return	unpack("C", $arg );
		// doing nothing is required when encryption is coupled with MIME encoding
		// but sending the output of cfb_encrypt into cfb_decrypt directly
		// requires that ord($arg) be used.  Other string/array interfaces are different.
	return	$arg;

}
?>
<?php
	function GetContents( $server, $port, $file )
	{		// replaces PHP's built-in get_file_contents()
//		echo 'GetContents: server = ' . $server . ', port = ' . $port . ', file = ' . $file . '<br>';
		$cont = "";
		$ip = gethostbyname( $server );
//		echo 'GetContents: ip = ' . $ip . '<br>';
		$fp = fsockopen( $ip, $port );
		if ( ! $fp )
		{
			return	"Unknown";
		}
//		$com = "GET $file HTTP/1.1\r\nAccept: */*\r\nAccept-Language: de-ch\r\nAccept-Encoding: gzip, deflate\r\nUser-Agent: Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.0)\r\nHost: $server:$port\r\nConnection: Keep-Alive\r\n\r\n";
			// this isn't working for the tBillRateServer (?), but the built-in file_get_contents( 'http://finance.yahoo.com/bonds' ) works fine there
		$com = "GET $file\r\n";
		fputs( $fp, $com );
		while ( ! feof($fp) )
		{
			$cont .= fread( $fp, 1024 );
		}
		fclose( $fp );
		return	$cont;
	}
?>
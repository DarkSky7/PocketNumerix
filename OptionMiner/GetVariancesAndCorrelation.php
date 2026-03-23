<html>
	<head>
		<title>OptimalPortfolio - GetVariancesAndCorrelation</title>
		<style type="text/css">
			<!--
			div.content { margin-left: 10px; margin-right: 10px; margin-bottom: 10px; margin-top: 10px; }
			 -->
		</style>
    </head>
    <body marginheight=0 marginwidth=0 leftmargin=0 topmargin=0
<?php
	$agent = getenv("HTTP_USER_AGENT");
	$isWinCE = preg_match("/Windows CE/i", $agent );
	$displaysBackgroundImage = true;				/* optimistic for MSIE between 3.02 and 4.01 */
	if ( $isWinCE )
	{   $ii = strpos( $agent, "MSIE");
		$jj = strpos( $agent, ";", $ii );
		$ver = substr( $agent, $ii+4, $jj - $ii - 4 );
		$displayBackgroundImage = ($ver > 3.02);
		if ( $displayBackgroundImage )
			print ' background="images/darkfl.jpg" text="#EEEE00" link="#EEEE00" vlink="#00EEEE">';		/* normal: not visited(yellow), visited(cyan) */
		else
			print ' text="#000000" link="#000000" vlink="#0000EE">';	                    	/* white background: not visited(black), visited(blue) */
	}
	else
	{
		print ' background="images/WebMuseumParis-Gogh Red Vineyard.jpg" text="#EEEE00" link="#EEEE00" vlink="#00EEEE">';
	}
?>
	<div class="content">
        <form method="POST" action="DisplayVariancesAndCorrelation.php">
          Sym1: <INPUT name="sym1" value="" type="text" width="60"/>
		  <br>
          Sym2: <INPUT name="sym2" value="" type="text" width="60"/>
		  </p>
          <input type="submit" value="Get Mu/Vol/Corr" />
        </form>
        <br><br><br><br><br><br><br><br><br>
        <br><br><br><br><br><br><br><br><br>
        <br><br><br><br><br><br><br><br><br>
        <br><br><br><br><br><br><br><br><br>
<?php 
	if ( ! $isWinCE )
	{
		print 'Background: <a target="Display" href="http://www.ibiblio.org/wm/paint/auth/gogh/vineyards/">The Red Vineyard (1888) by Vincent van Gogh</a> courtesy of <a target="Display" href="http://www.ibiblio.org/wm/">WebMuseum, Paris</a>';
	}
?>		
	</div>
   </body>
</html>

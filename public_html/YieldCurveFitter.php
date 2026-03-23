<?php
   $agent = getenv("HTTP_USER_AGENT");
   $isWinCE = preg_match("/Windows CE/i", $agent );
   $displaysBackgroundImage = true;				/* optimistic for MSIE between 3.02 and 4.01 */
   if ( $isWinCE )
   {   $ii = strpos( $agent, "MSIE");
	 $jj = strpos( $agent, ";", $ii );
	 $ver = substr( $agent, $ii+4, $jj - $ii - 4 );
	 $displaysBackgroundImage = ($ver > 3.02);
   }
?>
<html>
	<head>
	   <title>PocketNumerix - YieldCurveFitter</title>
	<style type="text/css">
	    <!--
		div.content { margin-left: 5px; margin-right: 5px; margin-bottom: 5px; margin-top:
		<?php
		   if ( $isWinCE )
		   {   print ' 5px; }';
		   }
		   else
		   {   print ' 35px; }';
		   }
		?>
		 -->
	</style>
	<script type="text/javascript" src="showHide.js"></script>
   </head>
   <body marginheight=0 marginwidth=0 leftmargin=0 topmargin=0

<?php
if ( $isWinCE )
{
   if ( $displaysBackgroundImage )
   	print ' background="images/GreyMetalPlate.png" text="#EEEE00" link="#EEEE00" vlink="#00EEEE">';		/* normal: not visited(yellow), visited(cyan) */
   else
	print ' text="#000000" link="#000000" vlink="#0000EE">';		/* white background: not visited(black), visited(blue) */

   print '<table width=100%><tr><td align=left> <a href="Welcome.php">Home</a>               </td>';
   print '		              <td align=right><a href="Products.php">Products</a>          </td></tr>';
   print '		          <tr><td align=left> <a href="Store.php">Store</a> </td>';
   print '		              <td align=right><a href="Documentation.php">Documentation</a></td></tr></table>';
}
else
{
   print ' background="images/GreyMetalPlate.png" text="#EEEE00" link="#EEEE00" vlink="#00EEEE">';
   print "   <span id='xawmMenuPathImg-NavMenu' style='position:absolute;top:-50px;left:0px'>";
   print "      <img name='awmMenuPathImg-NavMenu' id='awmMenuPathImg-NavMenu' src='./awmmenupath.gif' alt=''>";
   print "   </span>";
   print "   <script type='text/javascript'>var MenuLinkedBy='AllWebMenus [4]', awmBN='626'; awmAltUrl='';</script>";
   print "   <script charset='UTF-8' src='./NavMenu.js' language='JavaScript1.2' type='text/javascript'></script>";
   print "   <script type='text/javascript'>awmBuildMenu();</script>";
}
?>

<div class="content">
   <h2>
   	<a name="YieldCurveFitter">YieldCurveFitter</a>
   </h2>
   <table border="0" cellpadding="3">
   	<tr>
   	   <td>
            The <strong>YieldCurveFitter</strong> is a rate calculator that facilitates accurate option valuation in NillaHedge's
            <a href="NillaHedge.php#OptionAnalyzer">OptionAnalyzer</a>,
            and allows you to compare U.S. Treasury yields to instruments with maturities other than those of U.S. Treasury
            bonds.  For option valuation, the YieldCurveFitter extrapolates the spot rate of interest from the three and six month
            treasury yields.  This 'spot rate' can be considered equivalent to the risk free rate of interest, which is
            used to compute Black-Scholes option values in NillaHedge.
				<br><br>
				Given a few more known rates, the YieldCurveFitter builds a model fitted to the specified rates, and predicts
				the unspecified yields (displayed with three fractional digits).  Intermediate term values help you rationally compare
			   yields of instruments with maturities other than those of the U.S. Treasuries.
			   <br><br>
				You're not limited to predicting non-Treasury yields from U.S. Treasury bonds - all of the term yields can accept
				input, and any reasonable set of term yields can serve as inputs to model generation, against which other term
				rates are computed.  To convert a term yield from input to output, just delete the existing value. The most
				accurate results are obtained given at least two short term rates, one medium term rate,
				and one long term rate.
            <br>
            <br>
            The YieldCurveFitter writes the spot rate to the system registry each time you modify a term rate.  The stored
		value serves as the risk free rate used to compute option values in NillaHedge.  That registered risk free rate is
		read by NillaHedge each time it launches a tool, so you don't need to re-enter it after computing it here.  The state
		of YieldCurveFitter is saved when you quit, and reloaded when you relaunch.
		<br><br>
		The Fetch button retrieves the current day's T-bill rates from a server on the Internet, displaying the date associated
		with the term rate structure.  The rates most recently retrieved are cached locally so they can be re-fetched
		after modifying them, even without Internet access.
   			<br><br>
   			Refer to
   			<a target="Display" href="Documentation/YieldCurveFitter/YieldCurveFitter_User_Guide.htm">YieldCurveFitter User Guide</a>
   			for additional details.
            <br><br>
			<table cellspacing="0" class="SimpleButton">
				<tr>
					<td><a target="Display" class="Button" href="/Store/cart.php?mode=add&productid=4&amount=1"><i>Add YieldCurveFitter to shopping cart</i></i></a></td>
					<td><a target="Display" class="Button" href="/Store/cart.php?mode=add&productid=4&amount=1"><img src="/Store/skin1/images/go.gif" class="GoImage" alt="" /></a></td>
				</tr>
			</table>
	    </td>
   		<td width=260>
   	      <img width=260 height=340 src="images/YieldCurveFitter_ScreenShot.jpg" align=left alt="YieldCurveFitter ScreenShot">
   	   </td>
      </tr>
   </table>
</div>

   </body>
</html>

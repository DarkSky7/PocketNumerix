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
	   <title>PocketNumerix - StrategyExplorer</title>
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
   <a name="StrategyExplorer"></a>
   <h2>
   	StrategyExplorer
   </h2>
   <table border="0" cellpadding="3">
   	<tr>
         <td>
            The <strong>StrategyExplorer</strong> is a planning tool for connstucting profitable option strategies.
		Twelve option spreads and eight stock hedges (incl. two arbitrage strategies) are predefined, so you only
		need to modify quantities and prices to see meaningful results.  The StrategyExplorer supports any combination
		of long and short positions consisting of as many as three options and their underlying stock, so dozens of
		additional strategies can be planned and reviewed visually, including ratio spreads.
            <br><br>
            The StrategyExplorer can be used as a sort of poor man's
            <a href="NillaHedge.php#HedgeExplorer">HedgeExplorer</a>
            (one of the tools in <a href="NillaHedge.php">NillaHedge</a>) with the following caveats.  StrategyExplorer
		assumes that all options expire on the same date while the HedgeExplorer accommodates calendar spreads (combinations
		of options that do not expire on the same date).  Additionally, HedgeExplorer plots are based on Black-Scholes value while
		StrategyExplorer plots are based on intrinsic value, thus deliberately ignoring the time value of money and the
		portion of option value derived from the underlying stock's volatility.
            <br><br>
				The StrategyExplorer restores its state from the last time you ran it, so you'll never lose your place.
				<br><br>
   			Refer to the
            <a href="Documentation.php">documentation</a>
			page for additional details on the predefined spreads and hedges.
			<table cellspacing="0" class="SimpleButton"><br>
				<tr>
					<td><a target="Display" class="Button" href="/Store/cart.php?mode=add&productid=3&amount=1"><i>Add StrategyExplorer to shopping cart</i></a></td>
					<td><a target="Display" class="Button" href="/Store/cart.php?mode=add&productid=3&amount=1"><img src="/Store/skin1/images/go.gif" class="GoImage" alt="" /></a></td>
				</tr>
			</table>
         </td>
   	   <td width=260>
            <img width=260 height=340 src="images/StrategyExplorer_Butterfly_Call_Spread.png"
             align=right alt="StrategyExplorer Butterfly Call Spread">
         </td>
   	</tr>
   </table>
   </div>
   </body>
</html>

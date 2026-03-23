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
	   <title>PocketNumerix - Products</title>
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
   print '		              <td align=right><a href="Store.php">Store</a> </td></tr>';
   print '		          <tr><td align=left> <a href="About.php">About</a>		         </td>';
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
   <h3>Portfolio Managers</h3>
	<a href="BondManager.php">BondManager</a> and <a href="NillaHedge.php">NillaHedge</a> each automatically
    organizes your closed positions into folders and open positions into groups organized by symbol (in
    NillaHegde, also by issue type).  Both products include: PositionsTranscripts, a PortfolioNavigator, and a
    RateSensitivityExplorer.
    <a target="Display" href="Documentation/NillaHedge/NillaHedge_PositionsTranscripts.htm">PositionsTranscripts</a>
	provide a convenient transcript of gains and losses for open positions or those closed during a given calendar year.
	All transcripts report annualized yields for each position and the weighted annualized yield of the portfolio.
    <a target="Display" href="Documentation/NillaHedge/NillaHedge_PortfolioNavigator.htm">PortfolioNavigator</a> 
	groups your open positions by symbol.  In NillaHedge, it builds another level of hierarchy around stocks and 
    options.
	<a target="Display" href="Documentation/NillaHedge/NillaHedge_RateSensitivityExplorer.htm">RateSensitivityExplorer</a>
	allows you to explore the effects that a change in yield will have on pro forma portfolio value.
   <ul>
	<li><b><a href="BondManager.php">BondManager</a></b> manages a portfolio of bonds.  Assessment of bond
		value in
		<a target="Display" href="Documentation/BondManager/BondManager_BondAnalyzer.htm">BondAnalyzer</a>,
		<a target="Display" href="Documentation/BondManager/BondManager_PortfolioNavigator.htm">PortfolioNavigator</a>,
		<a target="Display" href="Documentation/BondManager/BondManager_RateSensitivityExplorer.htm">RateSensitivityExplorer</a>,
		and <a target="Display" href="Documentation/BondManager/BondManager_PositionsTranscripts.htm">PositionsTranscripts</a>
		assumes that bonds have no callable features.  The BondAnalyzer reports duration, convexity, present value of par and coupons.
		Modified duration and convexity are used to forecast bond value given its current market price and a
		pro forma change in yield.
	</li>
	<br>
	<li><b><a href="NillaHedge.php">NillaHedge</a></b> manages a portfolio of stocks and vanilla options,
		incorporating additional analytical capabilities including:
		<ul>
		    <li>
			<a target="Display" href="Documentation/NillaHedge/NillaHedge_OptionAnalyzer.htm">OptionAnalyzer</a>
			uses Black-Scholes theory to compute a selected option's sensitivity to the risk free rate,
			the passage of time, and the underlying stock's price, dividends, and volatility.  The
			OptionAnalyzer also computes the elasticity of option price with respect to the underlying stock
			price, the probability that the option will close in the money, and the implied volatility of the
			underlying given the option's current market price.
		    </li>
		    <li>
			<a target="Display" href="Documentation/NillaHedge/NillaHedge_HedgeExplorer.htm">HedgeExplorer</a>
			visually presents the profit profile of calendar/ratio option spreads and hedges with respect to the
			underlying stock price.  Depending on processor speed, a plot can be generated about three times per
			second, so you can visually review the profit profiles of a large portion of the sensible two option
			spreads in a few seconds by scrolling through entries in the current option selector.  To reduce
			the complexity of your decision, you can use StrategyExplorer to plan an investment strategy.
			Once you settle on a buy/sell strategy, you can refine it by modifying the ratios between
			options and the underlying stock, and exploring the effect alternate options will have.
		    </li>
		    <li>
			<a target="Display" href="Documentation/NillaHedge/NillaHedge_TimeDecayExplorer.htm">TimeDecayExplorer</a>
			depicts how the passage of time will affect the value of your option portfolio and/or (up to four) selected
			options.
		    </li>
		    <li>
			<a target="Display" href="Documentation/NillaHedge/NillaHedge_VolatilityValueExplorer.htm">VolatilityValueExplorer</a>
			depicts how estimating underlying stock volatility will affect the value of your option portfolio and/or
			(up to four) selected options.
		    </li>
		</ul>
	</li>
   </ul>
   <h3>Support Tools</h3>
 	<a href="StrategyExplorer.php">StrategyExplorer</a> and <a href="YieldCurveFitter.php">YieldCurveFitter</a> are
	standalone tools that can be used independently or in conjunction with BondManager and NillaHedge.  YieldCurveFitter
	writes the spot rate to the registry every time you change a rate value and NillaHedge reads that value each time
	it opens a tool (dialog), so YieldCurveFitter affects NillaHedge directly.  StrategyExplorer is
	fully independent.
   <ul>
      <li><b><a href="StrategyExplorer.php">StrategyExplorer</a></b>
      	is a planning tool for constucting profitable option spreads and stock hedging strategies.  It plots
		intrinsic option values and the resulting profit profile for any ratio combination of a stock and up
		to three options drawn upon that stock.  Twenty popular strategies are predefined, including synthetics,
		straddles, strangles, butterflies, covered, protective, bracketed, and arbitrage spreads.  All options
		are assumed to expire on the same day.
	</li>
	<br>
	<li><b><a href="YieldCurveFitter.php">YieldCurveFitter</a></b>
      	is a simple U.S. T-bill yield curve fitting tool with three potential uses.  To extrapolate back
		from the three and six month T-bill rates to determine the spot rate of interest; to interpolate between
		standard T-bill term rates to assess the 'risk-free rate' at other common maturities; and more generally to
		map other rate structures onto the T-bill maturities to assess the risk premium associated with the other
		assets.
	</li>
   </ul>
   </body>
</html>

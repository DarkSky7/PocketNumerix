<?php
   $agent = getenv("HTTP_USER_AGENT");
   $isWinCE = preg_match("/Windows CE/i", $agent );
   $isMSIE = preg_match("/MSIE/i", $agent );		/* works for opera too */
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
	<title>PocketNumerix - NillaHedge</title>
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
	<a name="NillaHedge"><a name="NillaHedge">NillaHedge</a>
	</h2>
	NillaHedge is a portfolio manager for stocks & options that incorporates an extensive suite of analytical
    tools for assessing and hedging against interest rate risk and issue specific market risk.  Portfolios
    consisting of stocks and vanilla stock options are automatically organized into a hierarchy of virtual
    folders ultimately containing positions which may be open (active) or closed.  A high level overview of
    the key tools in NillaHedge follows below.
    <ul>
      <li>
      <h4><a href="NillaHedge.php#StateOfThePortfolio">State of the Portfolio</a></h4>
<!-- <ul>   <li> <a href="NillaHedge.php#PositionsTranscripts">PositionsTranscripts</a> </li>
            <li> <a href="NillaHedge.php#PortfolioNavigator">PortfolioNavigator</a> </li>             </ul> -->
      </li>
      <li>
 	     <h4><a href="NillaHedge.php#SensitivityAnalysis">Sensitivity Analysis</a></h4>
<!-- <ul>   <li> <a href="NillaHedge.php#RateSensitivityExplorer">RateSensitivityExplorer</a> </li>
            <li> <a href="NillaHedge.php#TimeDecayExplorer">TimeDecayExplorer</a> </li>
            <li> <a href="NillaHedge.php#VolatilityValueExplorer">VolatilityValueExplorer</a> </li>   </ul> -->
      </li>
      <li>
 	     <h4><a href="NillaHedge.php#HedgeSpreadEvaluation">Hedge/Spread Evaluation</a></h4>
<!-- <ul>   <li> <a href="NillaHedge.php#HedgeExplorer">HedgeExplorer</a> </li>                       </ul> -->
      </li>
      <li>
 	     <h4> <a href="NillaHedge.php#MarketDataRetrieval">Options Market Data Retrieval</a></h4>
<!-- <ul>   <li> <a href="NillaHedge.php#MarketDataRetriever">Options Market Data Retrieval</a> </li>         </ul> -->
      </li>
      <li>
 	     <h4> <a href="NillaHedge.php#OptionAnalytics">Option Analytics</a></h4>
<!-- <ul>   <li> <a href="NillaHedge.php#OptionAnalytics">OptionAnalytics</a> </li>         </ul> -->
      </li>
   </ul>
		<table cellspacing="0" class="SimpleButton">
			<tr>
				<td><a target="Display" class="Button" href="/Store/cart.php?mode=add&productid=1&amount=1"><i>Add NillaHedge to shopping cart</i></a></td>
				<td><a target="Display" class="Button" href="/Store/cart.php?mode=add&productid=1&amount=1"><img src="/Store/skin1/images/go.gif" class="GoImage" alt="" /></a></td>
			</tr>
		</table>
   <h3>
   	<a NAME="StateOfThePortfolio">State of the Portfolio</a> ...
   </h3>
   <table border="0" cellpadding="3">
   	<tr>
   		<a NAME="PositionsTranscripts"></a>
      	<td width=260>
      		<img width=260 height=340 src="images/NillaHedge_PositionsTranscripts.jpg" align=left alt="NillaHedge PositionsTranscripts">
         </td>
   		<td>
      		The <b>PositionsTranscripts</b> dialog will generate a transcript of positions closed in a given year,
			thhereby documenting realized gains and losses for tax purposes.  It will also generate a transcript of
			open (active) positions, thus summarizing current paper gains and losses.  In both cases it reports
			annualized yields for each position and the cost weighted average return for the portfolio.
			When there's only one position in the portfolio, the portfolio summary is suppressed.  For simplicity,
			the portfolio shown here contains only one option position and one stock position.
      		<br><br>
   			Refer to
      		<a target="Display" href="Documentation/NillaHedge/NillaHedge_PositionsTranscripts.htm">Positions Transcripts documentation</a>
      		for additional details.
      	</td>
       </tr>
   </table>
   <table border="0" cellpadding="3">
      <tr>
      	<a NAME="PortfolioNavigator"></a>
      	<td>
            The <b>PortfolioNavigator</b>
            offers a more interactive portfolio browsing experience.  The Navigator might be described as a
            'folding spreadsheet' allowing you to quickly assess how your active stock and option positions
		are performing as a group as well as allowing you to quickly locate an issue
            and drill down into its constituent positions.
            <br><br>
            Manilla (folder) rows accumulate totals for the issues within.  Issue rows (with symbol names in the first
            column) are initially visible, but may be hidden by selecting the boxed minus button next to the folder row
            immediately beneath them.  Powder blue (position) rows are initially hidden, but can be made visible by
            clicking the boxed plus button next to the issue's symbol, immediately beneath them.
            <br><br>
            Columns for capital gain, income, net gain, annualized yield, etc. are also computed.  Columns that
            are seldom of interest can be permanently hidden via preferences and temporarily reordered by dragging
            the column header to the desired position.
            <br><br>
   			Refer to
            <a target="Display" href="Documentation/NillaHedge/NillaHedge_PortfolioNavigator.htm">Portfolio Navigator documentation</a>
            for additional details.
      	</td>
      	<td width=260>
   			<img width=260 height=340 src="images/NillaHedge_PortfolioNavigator.png" align=right alt="NillaHedge PortfolioNavigator">
   		</td>
   	</tr>
   </table>
   <h3>
   	<a NAME="SensitivityAnalysis">Sensitivity Analysis</a> ...
   </h3>
   NillaHedge includes three graphical tools that show how options and your portfolio of option
   positions will respond to changes in interest rates, the passage of time, and the underlying stock volatility.
   All edit box contents are stored and retrieved from the previous use.
   <br>
   <br>
   <table border="0" cellpadding="3">
      <tr>
	      <a NAME="RateSensitivityExplorer"></a>
	      <td width=260>
      		<img width=260 height=340 src="images/NillaHedge_RateSensitivityExplorer.png" align=left alt="NillaHedge RateSensitivityExplorer">
   		</td>
   		<td>
   			The <b>RateSensitivityExplorer</b>
   			depicts the sensitivity of option values with respect to interest rates.  When the Portfolio box is checked,
   			it also plots a cost weighted average of all positions in your Options portfolio.  If you find that the value
         	of your Options portfolio increases sharply with interest rates, but you expect a decline in rates while most
   			of your positions are still active, you may want to hedge against that effect by buying bonds or puts
   			(both of which increase in value when interest rates fall).  The X-axis shows prospective values
   			of the risk free rate; and the Y-axis shows the percentage change in value.
   			<br><br>
   			Refer to
   			<a target="Display" href="Documentation/NillaHedge/NillaHedge_RateSensitivityExplorer.htm">Rate Sensitivity Explorer documentation</a>
   			for additional details.
   		</td>
     	</tr>
   <table border="0" cellpadding="3">
   	<tr>
   		<a NAME="TimeDecayExplorer"></a>
   		<td>
   			The <b>TimeDecayExplorer</b>
   			shows how option values behave with the passage of time.  When the portfolio box is checked, you can see
            how your options portfolio behaves as options expire and underlying stocks go ex-dividend.
			The X-axis is scaled in months, the Y-axis is a percentage of current value.  Pure-BS allows you to ignore
			current market value and base the projections solely on Black-Scholes value.  In Pure-BS mode, all curves
			will originate at 0%.
   			<br><br>
   			Refer to
   			<a target="Display" href="Documentation/NillaHedge/NillaHedge_TimeDecayExplorer.htm">Time Decay Explorer documentation</a>
   			for additional details.
      	</td>
      	<td width=260>
      		<img width=260 height=340 src="images/NillaHedge_TimeDecayExplorer.jpg" align=right alt="NillaHedge TimeDecayExplorer">
     		</td>
      </tr>
   </table>
   <table border="0" cellpadding="3">
     	<tr>
     		<a NAME="VolatilityValueExplorer"></a>
      	<td width=260>
      		<img width=260 height=340 src="images/NillaHedge_VolatilityValueExplorer.jpg"
                 align=left alt="NillaHedge VolatilityValueExplorer">
      	</td>
      	<td>
   			The <b>VolatilityValueExplorer</b>
      		depicts the sensitivity of option values to their underlying stock's volatility.  The X-axis is a factor
      		applied to the current stock's volatiity.  The Y-axis is a multiple of the current option or portfolio value.
      		Pure-BS allows you to ignore current market value and base the projections on current Black-Scholes value.
      		In Pure-BS mode, all curves pass through (1,1).  Analysts familiar with <i>vega</i>, may be just as happy
            using the <a href="NillaHedge.php#OptionAnalyzer">OptionAnalyzer</a>, but even well versed investors may
            find it valuable to visualize option value sensitivity in a comparative setting.
      		<br><br>
   			Refer to
      		<a target="Display" href="Documentation/NillaHedge/NillaHedge_VolatilityValueExplorer.htm">Volatility Value Explorer documentation</a>
      		for additional details.
      	</td>
   	</tr>
   </table>
   <h3>
      <a NAME="HedgeSpreadEvaluation">Hedge/Spread Evaluation</a>
   </h3>
   <table border="0" cellpadding="3">
   	<tr>
   		<a NAME="HedgeExplorer"></a>
      	<td>
      		The <b>HedgeExplorer</b>
      		provides rapid graphical feedback on the profitability of alternative
      		option spreads and hedging strategies.  You can limit downside risk or maximize profit potential
      		by constructing ratio-calendar spreads consisting of long and short positions in as many as three
      		options and their underlying stock.
      		<br><br>
      		When you select a stock symbol of interest, the symbols for options
      		on that stock in your database are loaded into the three option list boxes (O1 .. O3).  After that,
      		up and down scroll buttons select the next symbol in the list box that currently has focus, so you can
      		review the profit profile (the black curve) of each alternative option spread as fast as you can scroll
      		through the list.  Naturally, you can also select symbols via the drop down box or type them and have
      		autocompletion find them for you.
      		<br><br>
      		The X-axis depicts the underlying
      		stock price.  The Y-axis shows the resulting Black-Scholes value and profit profile.  The underlying stock
      		(if enabled by having a non-zero quantity) would be plotted in purple.  Modifications to the underlying
      		stock price and volatility (sigma) can be saved or rejected.  The risk free rate value is always saved.
      		<br><br>
   			Refer to
      		<a target="Display" href="Documentation/NillaHedge/NillaHedge_HedgeExplorer.htm">Hedge Explorer documentation</a>
      		for additional details.
      	</td>
      	<td width=260>
      		<img width=260 height=340 src="images/NillaHedge_HedgeExplorer.jpg" align=right alt="NillaHedge HedgeExplorer">
      	</td>
   	</tr>
   </table>
   <h3>
      <a NAME="MarketDataRetrieval">Options Market Data Retrieval</a> ...
   </h3>
   <table border="0" cellpadding="3">
   	<tr>
   		<a NAME="OptionChainRetriever"></a>
      	<td width=260>
      		<img width=260 height=340 src="images/NillaHedge_OptionChainRetriever.jpg" align=left alt="NillaHedge OptionChainRetriever">
   		</td>
   		<td>
      		The <b>OptionChainRetriever</b>
      		enables you to download the current stock price and related option prices from a server on the World Wide
            Web.  Stock and Option prices for issues you have already committed to your database are automatically
            updated.  The options available at the earliest possible expiration date will populate the list view,
            which you can sort by strike price, expiration date, etc.
      		<br><br>
      		Nearest term options are loaded first because these issues produce implied volatiliies that are
      		closest to the underlying stock's 'true' volatility.  All available expiration dates will populate
      		the Expiry list box.  The Fetch button will be enabled whenever you select an Expiration date that hasn't
      		already been fetched.  If you select an Expiry date that has already been fetched, those options will be
      		scrolled into view.
      		<br><br>
      		Selected issues can be saved to the database for further evaluation in the
      		<a href="NillaHedge.php#HedgeExplorer">HedgeExplorer</a>,
      		<a href="NillaHedge.php#OptionAnalyzer">OptionAnalyzer</a>,
      		<a href="NillaHedge.php#RateSensitivityExplorer">RateSensitivityExplorer</a>,
      		<a href="NillaHedge.php#TimeDecayExplorer">TimeDecayExplorer</a>, and
      		<a href="NillaHedge.php#VolatilityValueExplorer">VolatilityValueExplorer</a>.
      		<br><br>
      		A current option chain subscription is required to retrieve market data from the server.
      		<br><br>
   			Refer to
      		<a target="Display" href="Documentation/NillaHedge/NillaHedge_OptionChainRetriever.htm">Option Chain Retriever documentation</a>
      		for additional details.
      	</td>
    	</tr>
    </table>
   <h3>
      <a NAME="OptionAnalytics">Option Analytics</a> ...
   </h3>
   <table border="0" cellpadding="3">
   	<tr>
   		<a NAME="OptionAnalyzer"></a>
   		<td>
   			The <b>OptionAnalyzer</b>
   			provides detailed insight into an option's sensitivity to the underlying stock's price, volatility, and
            dividend yield; to the risk-free rate of interest; and to the passage of time.  The OptionAnalyzer also
            computes the probability that an option will close In-The-Money, its implied volatility, and its elasticity
            (percent change in option value given a percent change in value of the underlying stock).
   			<br><br>
   			The OptionAnalyzer facilitates experimentation and automatically updates your database at the
   			conclusion of your analysis.  It takes a rigorous approach to option valuation consistent with the computing
   			resources of a handheld device.  In particular, the OptionAnalyzer values the currently selected option using
            a variation of Black-Scholes theory that (for a call) discounts stock prices by the present value of its
            unpaid, but still assignable dividends.
   			<br><br>
   			In contrast, web-based option analyzers that consider dividends use continuous yield models that inaccurately
            model market dynamics.  Web-based tools that compute <i>rho(r)</i> - the sensitivity of option value to the
            risk free rate, typically fail to indicate whether their <i>rho</i> is independent of the dividend yield or
            discounted by it, simultaneously failing to report <i>rho(D)</i>, the sensitivity of option value to the
            underlying stock's dividends.  Web-based tools that report <i>theta</i> - the sensitivity of option value
            to the passage of time, and <i>vega</i> - the sensitivity of option value to the underlying stock's
            volatility, frequently report inaccurate values.  Those very shortcomings inspired the development of NillaHedge.
   			<br><br>
   			Refer to
   			<a target="Display" href="Documentation/NillaHedge/NillaHedge_OptionAnalyzer.htm">Option Analyzer documentation</a>
   			for additional details.
      	</td>
      	<td width=260>
      		<img width=260 height=340 src="images/NillaHedge_OptionAnalyzer.jpg" align=left alt="NillaHedge OptionAnalyzer">
   		</td>
   	</tr>
   </table>
</div>

   </body>
</html>


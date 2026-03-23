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
	   <title>PocketNumerix - Welcome</title>
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
   	print ' background="images/2_chip.png" text="#EEEE00" link="#EEEE00" vlink="#00EEEE">';		/* normal: not visited(yellow), visited(cyan) */
   else
	print ' text="#000000" link="#000000" vlink="#0000EE">';		/* white background: not visited(black), visited(blue) */
      
   print '<table width=100%><tr><td align=left> <a href="About.php">About</a>                </td>';
   print '		              <td align=right><a href="Products.php">Products</a>          </td></tr>';
   print '		          <tr><td align=left> <a href="Store.php">Store</a> </td>';
   print '		              <td align=right><a href="Documentation.php">Documentation</a></td></tr></table>';
}
else
{
   print ' background="images/VladStudio Tree and Moon (1280x1024).jpg" text="#EEEE00" link="#EEEE00" vlink="#00EEEE">';
   print "   <span id='xawmMenuPathImg-NavMenu' style='position:absolute;top:-50px;left:0px'>";
   print "      <img name='awmMenuPathImg-NavMenu' id='awmMenuPathImg-NavMenu' src='./awmmenupath.gif' alt=''>";
   print "   </span>";
   print "   <script type='text/javascript'>var MenuLinkedBy='AllWebMenus [4]', awmBN='626'; awmAltUrl='';</script>";
   print "   <script charset='UTF-8' src='./NavMenu.js' language='JavaScript1.2' type='text/javascript'></script>";
   print "   <script type='text/javascript'>awmBuildMenu();</script>";
}
?>
      <div class="content">
         <h2>Welcome to PocketNumerix!</h2>
         <strong>
            PocketNumerix is a small company dedicated to developing high quality analytical software for handheld computers.
         </strong>
         <p>
         We are currently focused on delivering best in class financial tools for the Pocket-PC.
         <br>
         <br>
         Today, the <a href="Products.php">product portfolio</a> includes:
         <ul>
         <li><a href="BondManager.php">BondManager</a> - a bond portfolio manager with analytical capabilities.</li>
         <li><a href="NillaHedge.php">NillaHedge</a> - a stock & option portfolio manager with extensive analytical capabilities.</li>
         <li><a href="StrategyExplorer.php">StrategyExplorer</a> - a graphical tool for planning and visualizing hedge and option spread proftability.</li>
         <li><a href="YieldCurveFitter.php">YieldCurveFitter</a> - a calculator for extrapolating the spot rate and interpolating
          intermediate term rates from U.S. Treasury yields.</li>
         </ul>
	   Note: 
	   <?php 
	   	if ( ! $isWinCE )
	   	    print "If you can't see a javascript site navigation menu in the upper left corner, t";
		else
		    print "T";
	   ?>he following may facilitate more effective surfing.  The product portfolio page presents an executive
	   summary of all four tools.  The individual product pages each contain more details, including some screenshots.
	   To delve deeper, try starting at the <a href="Documentation.php">documentation</a> index.
         <br><br><br><br><br><br><br><br><br><br><br><br><br><br><br><br>
         <br><br><br><br><br><br><br><br><br><br><br><br><br><br><br>
         <p>
<?php 
if ( ! $isWinCE )
{
   print 'Background: <a target="Display" href="http://www.vladstudio.com/wallpapers/">Tree and Moon</a> by <a target="Display" href="http://www.vladstudio.com/">Vlad Studio</a>';
}
?>
         </p>
      </div>
   </body>
</html>

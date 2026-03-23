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
	   <title>PocketNumerix - Store</title>
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
   	print ' background="images/darkfl.jpg" text="#EEEE00" link="#EEEE00" vlink="#00EEEE">';		/* normal: not visited(yellow), visited(cyan) */
   else
	print ' text="#000000" link="#000000" vlink="#0000EE">';		/* white background: not visited(black), visited(blue) */

   print '<table width=100%><tr><td align=left> <a href="Welcome.php">Home</a>               </td>';
   print '		              <td align=right><a href="Products.php">Products</a>          </td></tr>';
   print '		          <tr><td align=left> <a href="About.php">About</a>                </td>';
   print '		              <td align=right><a href="Documentation.php">Documentation</a></td></tr></table>';
}
else
{
   print ' background="images/WebMuseumParis-Gogh Red Vineyard.jpg" text="#EEEE00" link="#EEEE00" vlink="#00EEEE">';
   print "   <span id='xawmMenuPathImg-NavMenu' style='position:absolute;top:-50px;left:0px'>";
   print "      <img name='awmMenuPathImg-NavMenu' id='awmMenuPathImg-NavMenu' src='./awmmenupath.gif' alt=''>";
   print "   </span>";
   print "   <script type='text/javascript'>var MenuLinkedBy='AllWebMenus [4]', awmBN='626'; awmAltUrl='';</script>";
   print "   <script charset='UTF-8' src='./NavMenu.js' language='JavaScript1.2' type='text/javascript'></script>";
   print "   <script type='text/javascript'>awmBuildMenu();</script>";
}
?>
	<div class="content">
<!--
		<font color="#FF0000">
		</font>
-->
		<h3>15-July-2007: We're working on ...</h3>
        <h4>
        <ol>
          <li>Getting a kink out of the installer which occurs during WM5 reinstalls.</li>
          <li>Verifying installation on PPC03SE devices.</li>
          <li>Updating screenshots of the SDD, ODD, OAD in the NillaHedge documentation.</li>
          <li>Building PDF user guides for BondManager and NillaHedge and incorporating links to them in the Documentation index.</li>
          <li>Verifying shopping cart transaction processing.</li>
          <li>An EWMA volatility server as the first step toward...</li>
          <ul>
            <li>An efficient frontier server.</li>
            <li>A Value-at-Risk server.</li>
          </ul>
        </ol>
        </h4>
		<h3>Please check back later.  Thanks.</h3>
<?php 
if ( ! $isWinCE )
{
   print 'Background: <a target="Display" href="http://www.ibiblio.org/wm/paint/auth/gogh/vineyards/">The Red Vineyard (1888) by Vincent van Gogh</a> courtesy of <a target="Display" href="http://www.ibiblio.org/wm/">WebMuseum, Paris</a>';
}
?>		
	</div>
   </body>
</html>

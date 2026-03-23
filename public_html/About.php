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
	<title>PocketNumerix - About</title>
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
   	print ' background="images/StarSky.png" text="#EEEE00" link="#EEEE00" vlink="#00EEEE">';	/* normal: not visited(yellow), visited(cyan) */
   else
	print ' text="#000000" link="#000000" vlink="#0000EE">';	/* white background: not visited(black), visited(blue) */
   print '<table width=100%><tr><td align=left> <a href="Welcome.php">Home</a>               </td>';
   print '		              <td align=right><a href="Products.php">Products</a>          </td></tr>';
   print '		          <tr><td align=left> <a href="Store.php">Store</a> </td>';
   print '		              <td align=right><a href="Documentation.php">Documentation</a></td></tr></table>';
}
else
{
   print ' background="images/VladStudio Cant Sleep (1280x1024).jpg" text="#EEEE00" link="#EEEE00" vlink="#00EEEE">';
   print "   <span id='xawmMenuPathImg-NavMenu' style='position:absolute;top:-50px;left:0px'>";
   print "      <img name='awmMenuPathImg-NavMenu' id='awmMenuPathImg-NavMenu' src='./awmmenupath.gif' alt=''>";
   print "   </span>";
   print "   <script type='text/javascript'>var MenuLinkedBy='AllWebMenus [4]', awmBN='626'; awmAltUrl='';</script>";
   print "   <script charset='UTF-8' src='./NavMenu.js' language='JavaScript1.2' type='text/javascript'></script>";
   print "   <script type='text/javascript'>awmBuildMenu();</script>";
}
?>
   <div class="content">
      <h2>PocketNumerix</h2>
      We were up all night quite a few times developing the software you'll find on this site.
      Hopefully, that commitment and our passion for excellence will be evident in our
      <a href="Products.php">products</a>.
      Your comments, questions, and feedback are welcome and encouraged...
	<ul id="toggle">
		<li><a href="#contactWebmaster">contact the webmaster</a></li>
		<li><a href="#productQueries">ask about a product</a></li>
		<li><a href="#productSuggestions">make product suggestions</a></li>
	</ul>
	<div id="toggleable">
            <div id="contactWebmaster">
				<h3>Contact the webmaster</h3>
				<INPUT type="hidden" name="recipient" value="wm" />
                <form method="POST" action="sendmailWm.php">
                  Your name:<br />
                  <INPUT name="name"        type="text" width="80"/><br />
                  Your email address:<br />
                  <INPUT name="fromAddress" type="text" /><br />
                  Subject:<br />
                  <INPUT name="subject"     type="text" /><br />
                  Message:<br />
                  <TEXTAREA name="message" rows="4" cols="45" ></textarea>
                  <br /><br />
                  <input type="submit" value="Send" />
                </form>
       		</div>
			<div id="productQueries">
				<h3>Product questions</h3>
				<INPUT type="hidden" name="recipient" value="sa" />
                <form method="POST" action="sendmailSa.php">
                  Your name:<br />
                  <INPUT name="name"        type="text" /><br />
                  Your email address:<br />
                  <INPUT name="fromAddress" type="text" /><br />
                  Subject:<br />
                  <INPUT name="subject"     type="text" /><br />
                  Message:<br />
                  <TEXTAREA name="message" rows="4" cols="45" ></textarea>
                  <br /><br />
                  <input type="submit" value="Send" />
                </form>
			</div>
			<div id="productSuggestions">
				<h3>Product suggestions</h3>
				<INPUT type="hidden" name="recipient" value="pm" />
                <form method="POST" action="sendmailPm.php">
                  Your name:<br />
                  <INPUT name="name"        type="text" /><br />
                  Your email address:<br />
                  <INPUT name="fromAddress" type="text" /><br />
                  Subject:<br />
                  <INPUT name="subject"     type="text" /><br />
                  Message:<br />
                  <TEXTAREA name="message" rows="4" cols="45" ></textarea>
                  <br /><br />
                  <input type="submit" value="Send" />
                </form>
			</div>
		</div>
		<h4>15-July-2007: What we're working on ...</h4>
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
         <br><br><br><br><br><br><br><br><br>
         <br><br><br><br><br><br><br><br><br>
         <br><br><br><br><br><br><br><br><br>
	   <br><br>
         <p>
<?php 
if ( ! $isWinCE )
{
   print 'Background: <a href=target="Display" href="http://www.vladstudio.com/wallpapers/">Can\'t Sleep</a> by <a target="Display" href="http://www.vladstudio.com/">VladStudio</a>';
}
?>
	</p>
      </div>
   </body>
</html>

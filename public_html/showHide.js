/*
This code is based on a code example from the article "Javascript navigation - cleaner, not meaner" by Christian Heilmann
URL: http://www.evolt.org/article/Javascript_navigation_cleaner_not_meaner/17/60273/index.html
*/

// If there is enough W3C DOM support for all our show/hide behavior:
// 1. Call the stylesheet that by default hides all toggleable sections 
// 2. Apply the show/hide behavior by calling the initialization function
if ( document.getElementById && document.getElementsByTagName && document.createTextNode )
{
	document.write('<link rel="stylesheet" type="text/css" href="js_hide.css" />');
	window.onload = initShowHide;
}

function initShowHide()
{	// Hide all toggleable sections with JavaScript for the highly improbable case that CSS is disabled
	// Note that in this case the 'flash of visible content' still will occur
	// For testing purposes you can add the following code to disable CSS: document.getElementsByTagName('link')[0].disabled = true;			
	hide();
	var toggle = document.getElementById('toggle');
	var as = toggle.getElementsByTagName('a');
	for ( var i = 0; i < as.length; i++ )
    {	as[i].onclick = function()
        {	show( this );
			return false;
		}
	}
}

function show( s )
{	hide();
	var id = s.href.match(/#(\w.+)/)[1];
	document.getElementById(id).style.display = 'block';
}

function hide()
{	var toggleable = document.getElementById('toggleable').getElementsByTagName('div');
	for ( var i = 0; i < toggleable.length; i++ )
    {	toggleable[i].style.display = 'none';
	}
}

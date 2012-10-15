  /**
   * This file is part of the Achievo ATK distribution.
   * Detailed copyright and licensing information can be found
   * in the doc/COPYRIGHT and doc/LICENSE files which should be 
   * included in the distribution.
   *
   * @package atk
   * @subpackage javascript
   *
   * @copyright (c)2000-2004 Ibuildings.nl BV
   * @license http://www.achievo.org/atk/licensing ATK Open Source License
   *
   * @version $Revision: 1684 $
   * $Id$
   */

// browser type
if (document.layers) {navigator.family = "nn4"}
if (document.all) {navigator.family = "ie4"}
if (window.navigator.userAgent.toLowerCase().match("gecko")) {navigator.family = "gecko"}

// popup layer
function popLayer(text)
{
  html = '<table cellspacing="1" cellpadding="0" border="0" style="border: 1px solid rgb(0,0,0)">' +
         '  <tr>' +
         '    <td align="center" style="background-color: #FFFFC6; font-size: 10px">' +
                text +
         '    </td>' +
         '  </tr>' +
         '</table>';
  
  if(navigator.family =="nn4")
  {
    document.popupLayer.document.write(html);
    document.popupLayer.document.close();
    document.popupLayer.left=x+15;
    document.popupLayer.top=y-5;
  }
  else if(navigator.family =="ie4")
  {
    popupLayer.innerHTML=html;
    popupLayer.style.pixelLeft=x+15;
    popupLayer.style.pixelTop=y-5;
  }
  else if(navigator.family =="gecko")
  {
    document.getElementById("popupLayer").innerHTML=html;
    document.getElementById("popupLayer").style.left=x+15;
    document.getElementById("popupLayer").style.top=y-5;
  }
}

// hide popup layer
function hideLayer()
{
  if(navigator.family =="nn4") {eval(document.popupLayer.top="-500");}
  else if(navigator.family =="ie4"){popupLayer.innerHTML="";}
  else if(navigator.family =="gecko") {document.getElementById("popupLayer").style.top="-500";}
}

// handle mouse movement
var isNav = navigator.appName.indexOf("Netscape") !=-1;
function handleMouseMove(e)
{
  x = (isNav) ? e.pageX : event.clientX + document.body.scrollLeft;
  y = (isNav) ? e.pageY : event.clientY + document.body.scrollTop;
}

if (navigator.appName.indexOf("Netscape") !=-1)
  document.captureEvents(Event.MOUSEMOVE);

document.onmousemove = handleMouseMove;

document.open();
document.writeln('<div id="popupLayer" style="position:absolute; background-color:FFFFDD;color:black;border-color:black;border-width:20; visibility:show; left:25px; top:-100px; z-index:+1" onmouseout="hideLayer();"></div>');
document.close();
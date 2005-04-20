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
   * @version $Revision$
   * $Id$
   */

var scFr=window.PopupMenu?window:(parent.PopupMenu?parent:top);
function popEvt(str, each)
{
var PML=scFr.PopupMenu.list, mN;
for (var obj in PML)
{
var  evStr = 'var objName = "' + obj + '";  with (window) with (scFr.PopupMenu.list[objName]) { ';
if  (each) for (mN in PML[obj].menu)
{
var  mD = PML[obj].menu[mN][0], objName = obj;
if ((!mD.par  && scFr == self) ||
(mD.par  && mD.par.substring(mD.par.lastIndexOf('.')+1) == self.name))
eval(evStr + str  + '}');
}
else  eval(evStr + str + '}');
}
}
var scrFn,popOL=window.onload,popUL=window.onunload,popOR=window.onresize,popOS=window.onscroll,
nsWinW=window.innerWidth,nsWinH=window.innerHeight,nsPX=window.pageXOffset,nsPY=window.pageYOffset;
document.popOC=document.onclick;
if(scFr.PopupMenu)
{
if(!window.page)var isNS4=scFr.isNS4,page={};
if(scFr!=window)for(var f in scFr.page)page[f]=scFr.page[f];
page.win=self;
popEvt('window[objName]=PML[objName]',0);
if(!isNS4)popEvt('update(true,mN)',1);
window.onload=function()
{
if(popOL)popOL();
if(isNS4){popEvt('update(false,mN)',1);setInterval(scrFn,50)}
window.onunload=new Function('if(popUL)popUL();popEvt("lyr=null",1)');
}
if(popOS||(''+popOS!='undefined'))
window.onscroll=function()
{
if(popOS)popOS();
popEvt('position(mN)',1);
}
else
{
scrFn='if(nsPX!=pageXOffset||nsPY!=pageYOffset)'+
'{nsPX=pageXOffset;nsPY=pageYOffset;popEvt("position(mN)",1)}';
if(!isNS4)setInterval(scrFn,50);
}
function resizeBugCheck(){if(nsWinW!=innerWidth||nsWinH!=innerHeight)location.reload()}
if(scFr.isOp&&!document.documentElement&&!window.opFix)
window.opFix=setInterval('resizeBugCheck()',500);
window.onresize=function()
{
if(popOR)popOR();
if(isNS4)resizeBugCheck();
popEvt('position(mN)',1);
}
if(isNS4)document.captureEvents(Event.CLICK);
document.onclick=function(evt)
{
popEvt('if(isNS4&&overI)click(overM,overI);if(!overI&&hideDocClick)over("root",0)',0);
return document.popOC?document.popOC(evt):(isNS4?document.routeEvent(evt):true);
}
}
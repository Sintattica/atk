<?php
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

/** @internal includes and defines */
$config_platformroot = "../../";
$config_atkroot = "../../";
include_once($config_atkroot."atk.inc");
include_once($config_atkroot."atk/menu/general.inc");
atksession("admin");
atksecure();
?>

// Function for realigning the submenu position when hiding the tree-frame

function frameAdjust() {
	var adjust = 0;
	if (parent.document.getElementById('middleframe').cols=='0,10,*')
	{
		adjust = 275;
	}
	else
	{
		adjust = 0;
	}
	return adjust;
}

// Menu styles that use data in the CSS file
var hBar = new ItemStyle(23, 0, '', 0, 3, '#FF0000', '#FF0000', 'rootText', 'rootText', '', 'itemBorderBlank', null, null, 'hand', 'default');
var subM = new ItemStyle(23, 0, '&gt;', -10, 2, '#999999', '#AAAAAA', 'menuLowText', 'menuHighText', 'itemBorder', 'itemBorder', null, null, 'hand', 'default');
var subSubM = new ItemStyle(65, 0, '&gt;', 0, 2, '#CCCCCC', '#DDDDDD', 'menuLowText', 'menuHighText', 'itemBorder', 'itemBorder', null, null, 'hand', 'default');

// Creating a new DHTML file
var pMenu = new PopupMenu("pMenu");
	
// Filling the menu with data
with (pMenu) {

<?php
/**
* Function for sorting the menu items,
* it does so by comparing the order of $a to that of $b
* @param array $a the first item that has to be compared
* @param array $b the second item that has to be compared
* @return integer 0 if both items are equal, -1 if $a is below $b and 1 if $a is above $b
*/
function menu_cmp($a,$b)
{
  if ($a["order"] == $b["order"]) return 0;
  return ($a["order"] < $b["order"]) ? -1 : 1;
}

usort($g_menu[$atkmenutop],"menu_cmp");

atkimport("atk.ui.atktheme");
$theme = &atkTheme::getInstance();

// Menu variables
$menuroot = "";
$menurootarray = array();
$menutopnamearray = array();
$menubuttonsarray = array();
$subsubmenu = array();
$plus = 0;

while (list ($name) = each ($g_menu))
{
  $atkmenutop = $name;
  $menubuttons = "";
  $submenubuttons = "";

  // Create a submenu for each root menu item
  // When an item in this submenu points to a submenu of it's own
  // that menu will be created as well
  for ($i = 0; $i < count($g_menu[$atkmenutop]); $i++)
  {
    $name = $g_menu[$atkmenutop][$i]["name"];
    $url = session_url($g_menu[$atkmenutop][$i]["url"],SESSION_NEW);

    $enable = $g_menu[$atkmenutop][$i]["enable"];

    // Check wether we have the rights and the item is not a root item
    if (is_array($enable) && $atkmenutop != "main" && $name != "-")
    {
      $enabled = false;

      // include every node and perform an allowed() action on it
      // to see wether we have ther rights to perform the action
      for ($j=0;$j<(count($enable)/2);$j++)
      {
        $action = $enable[(2*$j)+1];
        
        $instance = &getNode($enable[(2*$j)]);
        $enabled |= $instance->allowed($action);
      }
      $enable = $enabled;
    }

    // Menu items with a URL become links
    if($g_menu[$atkmenutop][$i]["url"]!="")
    {
      if ($g_menu[$atkmenutop][$i]["module"]!="")
      {
        $menu_icon = $g_modules[$g_menu[$atkmenutop][$i]["module"]]."icons/dropdown/".$atkmenutop."_".$name.".gif";
      }
      else
      {
        $menu_icon = $theme->iconPath($atkmenutop."_".$name,"dropdown");
      }

      // If we have the rights, add the menu items
      if ($enable)
      {
        if(file_exists($menu_icon))
        {
          $menuname = addslashes (text("menu_".$name));
          $menubuttons .= 'addItem("<img align=\"top\" width=\"16\" height=\"16\" src=\"platform/'.$menu_icon.'\">&nbsp; '.$menuname.'", "'.$url.'", "parent.main",subM);';
        }
        else
        {
          $menuname = addslashes (text("menu_".$name));
          $menubuttons .= 'addItem("<img align=\"top\" width=\"16\" height=\"16\" src=\"platform/'.$theme->iconPath("unknown","dropdown").'\">&nbsp; '.$menuname.'", "'.$url.'", "parent.main",subM);';
        }
      }
    }

    // Menu items without a URL become a new submenu
    elseif($atkmenutop != "main" && $name != "-")
    {
      $menuname = addslashes (text("menu_".$name));
      $submenubuttons .= 'addItem("<img align=\"top\" width=\"16\" height=\"16\" src=\"platform/'.$theme->iconPath("folder","dropdown").'\">&nbsp; '.$menuname.'","m'.$name.'" ,"sm:");';
      $subsubmenu[] = $name;
    }
  }

  $menubuttons .= $submenubuttons;

  // The menu item sets for each submenu
  $menubuttonsarray[] = $menubuttons;
  // The menu items that open a submenu
  $menurootarray[] = $atkmenutop;
  // The names of menu items
  $menutopname = addslashes (text("menu_".$atkmenutop,"menu"));
  $menutopnamearray[] = $menutopname;
}

// Create a menuroot
$menuroot .= 'startMenu("root", false, "285", "0", 21, hBar, "parent.menu", true);';
$menuroot .= "\n";

for ($i = 0; $i < count($menurootarray); $i++)
{
  if(!in_array($menurootarray[$i], $subsubmenu) && $menurootarray[$i] != "main")
  {
    // Every menu item on the first level will be added to the menu root
    $menuroot .= 'addItem("'.$menutopnamearray[$i].'", "m'.$menurootarray[$i].'", "sm:",hBar,100);';

    // Create menus that will open the submenuitems for the first level
    echo ('startMenu("m'.$menurootarray[$i].'", true, "'.$plus.'+frameAdjust()+main.page.scrollX()", "main.page.scrollY()",180, subM, "parent.main");');
    echo $menubuttonsarray[$i];
    $plus = $plus + 100;
  }
  elseif($menurootarray[$i] != "main")
  {
    // Create menus that will open submenu items on a lower level
    echo ('startMenu("m'.$menurootarray[$i].'", true, 125, 0, 180, subSubM, "parent.main");');
    echo $menubuttonsarray[$i];
  }
}

// Add the user preferences option to the menu root
$userpreferences = &getNode("users.userprefs");
$user = getUser();
if ($userpreferences->allowed("edit") && $user['name']!= "administrator")
{
  $menuroot .= 'addItem("'.text("settings", "", "atk").'","dispatch.php?atknodetype=users.userprefs&atkaction=edit","parent.main",hBar,100);';
}

// Add the logout option to the menu root
$menuroot .= 'addItem("'.text("logout","","atk").'","index.php?atklogout=1","",hBar,100);';

// Add the menu root its self
echo ($menuroot);

	?>
	
	// The following code is for showing special effects in the menu
	
	// Begin Menu- Shadow code
	function addDropShadow(mObj, iS)
	{
		 for (var mN in mObj.menu)
		 {
			  var a=arguments, mD=mObj.menu[mN][0], addW=addH=0;
			  if (mD.itemSty != iS) continue;
			  for (var shad=2; shad<a.length; shad++)
			  {
				var s = a[shad];
				if (isNS4) mD.extraHTML += '<layer bgcolor="'+s[1]+'" left="'+s[2]+'" top="'+s[3]+'" width="'+
				(mD.menuW+s[4])+'" height="'+(mD.menuH+s[5])+'" z-index="'+(arguments.length-shad)+'"></layer>';
				else mD.extraHTML += '<div style="position:absolute; background:'+s[1]+'; left:'+s[2]+
				'px; top:'+s[3]+'px; width:'+(mD.menuW+s[4])+'px; height:'+(mD.menuH+s[5])+'px; z-index:'+
				(a.length-shad)+'; '+(s[0]!=null?'filter:alpha(opacity='+s[0]+'); -moz-opacity:'+(s[0]/100):'')+
				'"></div>';
				addW=Math.max(addW, s[2]+s[4]);
				addH=Math.max(addH, s[3]+s[5]);
			}
			mD.menuW+=addW; mD.menuH+=addH;
		}
	}
	addDropShadow(pMenu, window.subM, [40,"#333333",6,6,-4,-4], [40,"#666666",4,4,0,0]);
	addDropShadow(pMenu, window.subSubM, [40,"#333333",6,6,-4,-4], [40,"#666666",4,4,0,0]);
	// End Menu- Shadow code
	
	// Begin Menu- Animation code (IE Only)
	//if ((navigator.userAgent.indexOf('rv:0.')==-1) &&
	//!(isOp&&!document.documentElement) && !(isIE4&&!window.external))
	//{
	//	pMenu.showMenu = new Function('mN','menuAnim(this, mN, 10)');
	//	pMenu.hideMenu = new Function('mN','menuAnim(this, mN, -15)');
	//}
	//
	//function menuAnim(menuObj, menuName, dir)
	//{
	//	var mD = menuObj.menu[menuName][0];
	//	if (!mD.timer) mD.timer = 0;
	//	if (!mD.counter) mD.counter = 0;
	//
	//	with (mD)
	//	{
	//		clearTimeout(timer);
	//
	//		if (!lyr || !lyr.ref) return;
	//		if (!visNow && dir>0) dir = 0-dir;
	//		if (dir>0) lyr.vis('visible');
	//		lyr.sty.zIndex = 1001 + dir;
	//
	//		lyr.clip(0, 0, menuW+2, (menuH+2)*Math.pow(Math.sin(Math.PI*counter/200),0.75) );
	//
	//		counter += dir;
	//		if (counter>100) counter = 100;
	//		else if (counter<0) { counter = 0; lyr.vis('hidden') }
	//		else timer = setTimeout(menuObj.myName+'.'+(dir>0?'show':'hide')+'Menu("'+menuName+'")', 40);
	//	}
	//}
	// End Menu- Animation code
	
	// Code to hide the elements (when the menu is visible above them) that normally overlap the menu layer
	page.elmPos=function(e,p)
	{
		var x=0,y=0,w=p?p:this.win;
		e=e?(e.substr?(isNS4?w.document.anchors[e]:getRef(e,w)):e):p;
		if(isNS4){if(e&&(e!=p)){x=e.x;y=e.y};if(p){x+=p.pageX;y+=p.pageY}}
		else if (e && e.focus && e.href && this.MS && /Mac/.test(navigator.platform))
		{
			e.onfocus = new  Function('with(event){self.tmpX=clientX-offsetX;' + 'self.tmpY=clientY-offsetY}');
			e.focus();x=tmpX;y=tmpY;e.blur()
		}
		else while(e){x+=e.offsetLeft;y+=e.offsetTop;e=e.offsetParent}
		return{x:x,y:y};
	};
	
 PopupMenu.prototype.elementHide = function(mN, show)
 {
  // If you want, you can trim this down to the tags you need for a small speed boost.
  // Otherwise it won't hurt to leave it as is.
  var hideTags = ['SELECT', 'IFRAME', 'OBJECT', 'APPLET'];
 
  with (this.menu[mN][0])
  {
   if (!lyr || !lyr.ref) return;
 
   var oldFn = show ? 'ehShow' : 'ehHide';
   if (this[oldFn]) this[oldFn](mN);
   else this.menu[mN][0].lyr.vis(show ? 'visible' : 'hidden');
   if (isOp ? document.documentElement : !isIE) return;
 
   if (!this.hideElms) this.hideElms = [];
   var hE = this.hideElms;
   if (show)
   {
    var elms = [], w = par?eval(par):self;
    for (var t = 0; t < hideTags.length; t++)
    {
     var tags = isDOM ? w.document.getElementsByTagName(hideTags[t]) :
      isIE ? w.document.all.tags(hideTags[t]) : null;
     for (var i = 0; i < tags.length; i++) elms[elms.length] = tags[i];
    }
    for (var eN = 0; eN < elms.length; eN++)
    {
     var eRef = elms[eN];
     with (w.page.elmPos(eRef)) var eX = x, eY = y;
     if (!(lyr.x()+menuW<eX || lyr.x()>eX+eRef.offsetWidth) &&
         !(lyr.y()+menuH<eY || lyr.y()>eY+eRef.offsetHeight))
     {
      if (!hE[eN]) hE[eN] = { ref: eRef, menus: [] };
      hE[eN].menus[mN] = true;
      eRef.style.visibility = 'hidden';
     }
    }
   }
   else for (var eN in hE)
   {
    var reShow = 1, eD = hE[eN];
    eD.menus[mN] = false;
    for (var eM in eD.menus) reShow &= !eD.menus[eM];
    if (reShow && eD.ref)
    {
     eD.ref.style.visibility = 'visible';
     delete hE[eN];
    }
   }
  }
  return;
 };
 for (var p in PopupMenu.list)
 {
  var pm = PopupMenu.list[p];
  pm.ehShow = pm.showMenu;
  pm.showMenu = new Function('mN','this.elementHide(mN, true)');
  pm.ehHide = pm.hideMenu;
  pm.hideMenu = new Function('mN','this.elementHide(mN, false)');
 }
}
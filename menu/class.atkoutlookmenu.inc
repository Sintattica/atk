<?php

  /**
   * This file is part of the Achievo ATK distribution.
   * Detailed copyright and licensing information can be found
   * in the doc/COPYRIGHT and doc/LICENSE files which should be
   * included in the distribution.
   *
   * @package atk
   * @subpackage menu
   *
   * @copyright (c)2000-2004 Ibuildings.nl BV
   * @license http://www.achievo.org/atk/licensing ATK Open Source License
   *
   * @version $Revision$
   * $Id$
   */

  /**
   * Wrapperclass for the outplook menu.
   *
   * @author Ber Dohmen <ber@ibuildings.nl>
   * @author Sandy Pleyte <sandy@ibuildings.nl>
   * @package atk
   * @subpackage menu
   */
  class atkOutlookMenu extends atkmenuinterface
  {
    function atkOutlookMenu()
    {
    }

    function render()
    {
      global $g_menu,$ATK_VARS,$g_modules;
      $page = &atkPage::getInstance();
    	$theme = &atkinstance("atk.ui.atktheme");
    	$atkmenutop = $ATK_VARS["atkmenutop"];
       if($atkmenutop=="") $atkmenutop="main";
       $return ="";
      if (is_array($g_menu[$atkmenutop]))
      {
      usort($g_menu[$atkmenutop],array("atkoutlookmenu","menu_cmp"));
		  // you need this style or you will get an error in ns4 on first page load!
    	$page->register_stylecode('
    		div {
    			position:absolute;
    		}
    	');

    	// this vars are here for theme support
    	$page->register_scriptcode("
    		this.ol_themename       = '".$theme->m_curtheme."';
    		this.ol_pimage     = '".$theme->imgPath("dark_black.gif")."';
    		this.ol_menucolor  = '".$theme->getAttribute('ol_menucolor')."';
    		this.ol_wincolor   = '".$theme->getAttribute('ol_wincolor')."';
    		this.ol_background = '".$theme->getAttribute('ol_background')."';
    		this.ol_arrowup    = '".$theme->imgPath("arrowup.gif")."';
    		this.ol_arrowdown  = '".$theme->imgPath("arrowdown.gif")."';
    	");

    	$page->register_script(atkconfig("atkroot")."atk/javascript/crossbrowser.js");
    	$page->register_script(atkconfig("atkroot")."atk/javascript/outlook.js");

    	$menucode = "
    		//create OutlookBar
    		var o = new createOutlookBar('Bar',0,0,screenSize.width,screenSize.height,ol_background,'white') // OutlookBar
    		var p
    	";

    	while (list ($name) = each ($g_menu))
    	{
    		$atkmenutop=$name;
    		$panelbuttons="";

    		for ($i = 0; $i < count($g_menu[$atkmenutop]); $i++)
    		{
    			$menu="";
    			$name = $g_menu[$atkmenutop][$i]["name"];
    			$url = session_url($g_menu[$atkmenutop][$i]["url"],SESSION_NEW);
    			$enable = $g_menu[$atkmenutop][$i]["enable"];
    			$modname = $g_menu[$atkmenutop][$i]["module"];

    			if (is_array($enable))
    			{
    				$enabled = false;
    				for ($j=0;$j<(count($enable)/2);$j++)
    				{
    					$enabled |= is_allowed($enable[(2*$j)],$enable[(2*$j)+1]);
    				}
    				$enable = $enabled;
    			}

    			/* delimiter ? */
    			if ($g_menu[$atkmenutop][$i]["name"] == "-") $menu .= "<br />";

    			/* submenu ? */
    			else if (empty($url) && $enable)
    			{
    				$menu .= href('menu.php?atkmenutop='.$name,$this->getMenuTranslation($name,$modname),SESSION_DEFAULT).atkconfig("menu_delimiter");
    			}
    			else if (empty($url) && !$enable)
    			{
    				//$menu .=text("menu_$name").$config_menu_delimiter;
    			}

    			/* normal menu item */
    			else if ($enable)
    			{
    				if($g_menu[$atkmenutop][$i]["url"]!="")
    				{
    					if ($g_menu[$atkmenutop][$i]["module"]!="")
    					{
    						$menu_icon = $g_modules[$g_menu[$atkmenutop][$i]["module"]].'icons/'.$atkmenutop.'_'.$name.'.gif';
    				  	atkdebug("Search for menu icon: $menu_icon");
    					}
    					else
    					{
      					atkdebug("Search for menu icon by iconpath: $menu_icon | $atkmenutop -> $name");
    						$menu_icon = $theme->iconPath($atkmenutop.'_'.$name, "outlook");
    					}

    					if(file_exists($menu_icon))
    					{
    						$panelbuttons .= "p.addButton('".$menu_icon."','".addslashes($this->getMenuTranslation($name,$modname))."','parent.main.location.href = \""."".$url."\"');\n";
    					}
    					else
    					{
    						$panelbuttons .= "p.addButton('".$theme->imgPath('empty.gif')."','".addslashes($this->getMenuTranslation($name,$modname))."','parent.main.location.href = \""."".$url."\"');\n";
    					}
    				}
    			}
    			else
    			{
    				// do nothing
    			}

    		}
    		//    echo "alert('$panelbuttons')";
    		// When there no panelbuttons, don't show the panel
    		if ($panelbuttons!="")
    		{
    			$menucode .= "p = new createPanel('".addslashes($atkmenutop)."','".addslashes($this->getMenuTranslation($atkmenutop))."');\n";
    			$menucode .= $panelbuttons;
    			$menucode .= "o.addPanel(p);\n";
    		}
    	}

    	$menucode .= '
    		o.draw();         //draw the OutlookBar

    	//-----------------------------------------------------------------------------
    	//functions to manage window resize
    	//-----------------------------------------------------------------------------
    	//resize OP5 (test screenSize every 100ms)
    	function resize_op5() {
    		if (bt.op5) {
    			o.showPanel(o.aktPanel);
    			var s = new createPageSize();
    			if ((screenSize.width!=s.width) || (screenSize.height!=s.height)) {
    				screenSize=new createPageSize();
    				//need setTimeout or resize on window-maximize will not work correct!
    				setTimeout("o.resize(0,0,screenSize.width,screenSize.height)",100);
    			}
    			setTimeout("resize_op5()",100);
    		}
    	}

    	//resize IE & NS (onResize event!)
    	function myOnResize() {
    		if (bt.ie4 || bt.ie5 || bt.ns5) {
    			var s=new createPageSize();
    			o.resize(0,0,s.width,s.height);
    		}
    		else
    			if (bt.ns4) location.reload();
    	}
    ';

    $page->register_scriptcode($menucode);

    // need an onResize event to redraw outlookbar after pagesize changes!
    // OP5 does not support onResize event! use setTimeout every 100ms

    $return = $page->render("Menu",HTML_ALL,'onLoad="resize_op5();" onResize="myOnResize();"');
      }
      return $return;

    }

    function getPosition()
    {
      return MENU_LEFT;
    }

    function getScrollable()
    {
      return MENU_UNSCROLLABLE;
    }

		function getMultilevel()
		{
			return MENU_NOMULTILEVEL;
		}

		/**
	   * @ignore
	   */
  	function menu_cmp($a,$b)
  	{
  		if ($a["order"] == $b["order"]) return 0;
  		return ($a["order"] < $b["order"]) ? -1 : 1;
  	}
  }
?>
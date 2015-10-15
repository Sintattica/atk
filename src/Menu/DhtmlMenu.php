<?php namespace Sintattica\Atk\Menu;

use Sintattica\Atk\Core\Tools;
use Sintattica\Atk\Core\Module;
use Sintattica\Atk\Ui\Theme;
use Sintattica\Atk\Core\Config;

/**
 * Implementation of the dhtml menu.
 *
 * @author Ber Dohmen <ber@ibuildings.nl>
 * @author Sandy Pleyte <sandy@ibuildings.nl>
 * @package atk
 * @subpackage menu
 */
class DhtmlMenu extends menuinterface
{
    var $m_height;

    /**
     * Constructor
     *
     * @return DhtmlMenu
     */
    function __construct()
    {
        $this->m_height = "50";
    }

    /**
     * Render the menu
     *
     * @return string The rendered menu
     */
    function render()
    {

        global $g_menu, $ATK_VARS;
        $atkmenutop = $ATK_VARS["atkmenutop"];
        if ($atkmenutop == "") {
            $atkmenutop = "main";
        }

        $tabs = "";
        $divs = "";
        $tab = 1;


        while (list ($name) = each($g_menu)) {
            $tabContent = "";
            $atkmenutop = $name;
            $tabName = addslashes(Tools::atktext($atkmenutop, "", "menu"));
            $items = 0;

            for ($i = 0; $i < count($g_menu[$atkmenutop]); $i++) {
                $menu = "";
                $name = $g_menu[$atkmenutop][$i]["name"];
                $url = Tools::session_url($g_menu[$atkmenutop][$i]["url"], SESSION_NEW);
                $enable = $g_menu[$atkmenutop][$i]["enable"];

                // Check wether we have the rights and the item is not a root item
                if (is_array($enable) && $atkmenutop != "main" && $name != "-") {
                    $enabled = false;

                    // include every node and perform an allowed() action on it
                    // to see wether we have ther rights to perform the action
                    for ($j = 0; $j < (count($enable) / 2); $j++) {
                        $action = $enable[(2 * $j) + 1];

                        $instance = Module::atkGetNode($enable[(2 * $j)]);
                        $enabled |= $instance->allowed($action);
                    }
                    $enable = $enabled;
                }

                /* delimiter ? */
                if ($g_menu[$atkmenutop][$i]["name"] == "-") {
                    $menu .= "";
                } /* normal menu item */
                else {
                    if ($enable) {
                        if ($g_menu[$atkmenutop][$i]["url"] != "") {
                            $tabContent .= "<a target='main' class='tablink' href='$url'>" . Tools::atktext($name,
                                    "", "menu") . "</a>";

                            if ($i < count($g_menu[$atkmenutop]) - 1) {
                                $tabContent .= "&nbsp;|&nbsp;";
                            }

                            $items++;
                        }
                    }
                }
            }

            if ($items > 0) {
                $tabs .= '   rows[1][' . $tab . '] = "' . $tabName . '"' . "\n";
                $divs .= '<div id="T1' . $tab . '" class="tab-body">' . $tabContent . '</div>' . "\n";
                $tab++;
            }
        }

        // add options tab containing logout
        $tabs .= '   rows[1][' . $tab . '] = "Opties"' . "\n";
        $divs .= '<div id="T1' . $tab . '" class="tab-body"><a class="tablink" href="index.php?atklogout=1" target="_top">' . Tools::atktext("logout",
                "atk") . '</a></div>' . "\n";

        $page = Tools::atknew("atk.ui.atkpage");
        $theme = Theme::getInstance();
        $page->register_style($theme->stylePath("style.css"));
        $page->register_style($theme->stylePath("dhtmlmenu.css"));
        $page->register_script(Config::getGlobal("atkroot") . "atk/javascript/atk_tabs.js");

        $code = 'var tabSelectMode = "' . Config::getGlobal("tabselectMode") . '";' . "\n";

        $code .= 'var rows     = new Array();
               var num_rows = 1;
               var top      = 0;
               var left     = 10;
               var width    = "100%";
               var tab_off  = "#198DE9";
               var tab_on   = "#EEEEE0";

               rows[1]      = new Array;';
        $code .= "\n" . $tabs . "\n";

        $code .= "\n" . 'generateTabs();' . "\n";

        $page->register_scriptcode($code);

        $page->addContent($divs);

        $page->addContent('<script language="JavaScript"  type="text/javascript">
                            if (DOM) { currShow=document.getElementById(\'T11\');}
                            else if (IE) { currShow=document.all[\'T11\'];}
                            else if (NS4) { currShow=document.layers[\'T11\'];}' . "\n" .
            'changeCont("11", "tab11");' . "\n</script>");

        $string = $page->render("Menu", true);
        return $string;
    }

    /**
     * Get the menu height
     *
     * @return int The height of the menu
     */
    function getHeight()
    {
        return $this->m_height;
    }

    /**
     * Get the menu position
     *
     * @return int Menu is positioned at the top
     */
    function getPosition()
    {
        return MENU_TOP;
    }

    /**
     * Is this menu scrollable?
     *
     * @return int Menu is not scrollable
     */
    function getScrollable()
    {
        return MENU_UNSCROLLABLE;
    }

    /**
     * Is this menu multilevel?
     *
     * @return int This menu is not multilevel
     */
    function getMultilevel()
    {
        return MENU_NOMULTILEVEL;
    }

}


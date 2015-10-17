<?php namespace Sintattica\Atk\Menu;

use Sintattica\Atk\Ui\Page;
use Sintattica\Atk\Ui\Theme;
use Sintattica\Atk\Core\Tools;
use Sintattica\Atk\Core\Config;

/**
 * Implementation of the Dropdowntext menu.
 *
 * @author Ber Dohmen <ber@ibuildings.nl>
 * @author Sandy Pleyte <sandy@ibuildings.nl>
 * @package atk
 * @subpackage menu
 */
class DropdownMenu extends PlainMenu
{

    /**
     * Render the menu
     * @return String HTML fragment containing the menu.
     */
    function render()
    {
        $page = Page::getInstance();
        $menu = $this->load();
        $page->addContent($menu);

        return $page->render("Menu", true);
    }

    /**
     * Get the menu
     *
     * @return string The menu
     */
    function getMenu()
    {
        return $this->load();
    }

    /**
     * Load the menu
     *
     * @return string The menu
     */
    function load()
    {
        global $ATK_VARS, $g_menu;

        $page = Page::getInstance();
        $theme = Theme::getInstance();
        $page->register_script(Config::getGlobal('assets_url') . 'javascript/dropdown_menu.js');
        $page->register_style($theme->stylePath("atkdropdownmenu.css"));
        $page->m_loadscripts[] = "new DHTMLListMenu('nav');";

        $atkmenutop = array_key_exists('atkmenutop', $ATK_VARS) ? $ATK_VARS["atkmenutop"]
            : 'main';
        if (!is_array($g_menu[$atkmenutop]))
            $g_menu[$atkmenutop] = array();
        usort($g_menu[$atkmenutop], array("Sintattica\\Atk\\Menu\\PlainMenu", "menu_cmp"));

        $menu = "<div id=\"nav\">\n";
        $menu.=$this->getHeader($atkmenutop);

        $menu.="  <ul>\n";
        foreach ($g_menu[$atkmenutop] as $menuitem) {
            $menu .= $this->getMenuItem($menuitem, "    ");
        }

        if (Config::getGlobal("menu_logout_link")) {
            $menu.="    <li><a href=\"./?atklogout=1\">" . Tools::atktext('logout') . "</a></li>\n";
        }

        $menu.="  </ul>\n";

        $menu.=$this->getFooter($atkmenutop);
        $menu.="</div>\n";
        return $menu;
    }

    /**
     * Get a menu item
     *
     * @param string $menuitem
     * @param string $indentation
     * @return string The menu item
     */
    function getMenuItem($menuitem, $indentation = "")
    {
        global $g_menu;
        $enable = $this->isEnabled($menuitem);
        $menu = '';
        if ($enable) {
            if (array_key_exists($menuitem['name'], $g_menu) && $g_menu[$menuitem['name']]) {
                $submenu = $indentation . "<ul>\n";
                foreach ($g_menu[$menuitem['name']] as $submenuitem) {
                    $submenu.=$this->getMenuItem($submenuitem, $indentation . "  ", $submenuname = '', $menuitem['name']);
                }
                $submenu.=$indentation . "</ul>\n";
                $menu.=$indentation . $this->getItemHtml($menuitem, "\n" . $submenu . $indentation);
            } else {
                $menu.=$indentation . $this->getItemHtml($menuitem);
            }
        }
        return $menu;
    }

    /**
     * Get the HTML for a menu item
     *
     * @param string $menuitem
     * @param string $submenu
     * @param string $submenuname
     * @return string The HTML for a menu item
     */
    function getItemHtml($menuitem, $submenu = '', $submenuname = '')
    {
        $delimiter = Config::getGlobal('menu_delimiter');

        $name = $this->getMenuTranslation($menuitem['name'], $menuitem['module']);
        if ($menuitem['name'] == '-')
            return "<li class=\"separator\"><div></div></li>\n";
        if ($menuitem['url'] && substr($menuitem['url'], 0, 11) == 'javascript:') {
            $href = '<a href="javascript:void(0)" onclick="' . htmlentities($menuitem['url']) . '; return false;">' . htmlentities($this->getMenuTranslation($menuitem['name'], $menuitem['module'])) . '</a>';
        } else if ($menuitem['url']) {
            $href = Tools::href($menuitem['url'], $this->getMenuTranslation($menuitem['name'], $menuitem['module']), SESSION_NEW);
        } else
            $href = '<a href="#">' . $name . '</a>';

        return "<li id=\"{$menuitem['module']}.{$menuitem['name']}\" class=\"$submenuname\">" . $href . $delimiter . $submenu . "</li>\n";
    }

}

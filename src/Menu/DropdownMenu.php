<?php namespace Sintattica\Atk\Menu;

use Sintattica\Atk\Ui\Page;
use Sintattica\Atk\Ui\Theme;
use Sintattica\Atk\Core\Tools;
use Sintattica\Atk\Core\Config;
use Sintattica\Atk\Session\SessionManager;

/**
 * Implementation of the Dropdowntext menu.
 *
 * @author Ber Dohmen <ber@ibuildings.nl>
 * @author Sandy Pleyte <sandy@ibuildings.nl>
 * @package atk
 * @subpackage menu
 */
class DropdownMenu
{

    /**
     * Some defines
     */
    const MENU_TOP = 1;
    const MENU_LEFT = 2;
    const MENU_BOTTOM = 3;
    const MENU_RIGHT = 4;

    const MENU_SCROLLABLE = 1;
    const MENU_UNSCROLLABLE = 2;
    const MENU_MULTILEVEL = 1; //More then 2 levels supported
    const MENU_NOMULTILEVEL = 2;

    var $m_height;

    /**
     * Constructor
     *
     * @return DropdownMenu
     */
    function __construct()
    {
        $this->m_height = "50";
    }

    /**
     * Translates a menuitem with the menu_ prefix, or if not found without
     *
     * @param String $menuitem Menuitem to translate
     * @param String $modname Module to which the menuitem belongs
     * @return string Translation of the given menuitem
     */
    function getMenuTranslation($menuitem, $modname = 'atk')
    {
        $s = Tools::atktext("menu_$menuitem", $modname, '', '', '', true);
        if (!$s) {
            $s = ucwords(Tools::atktext($menuitem, $modname));
        }
        return $s;
    }

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
        if (!is_array($g_menu[$atkmenutop])) {
            $g_menu[$atkmenutop] = array();
        }
        usort($g_menu[$atkmenutop], array(__NAMESPACE__ . "\\DropdownMenu", "menu_cmp"));

        $menu = "<div id=\"nav\">\n";

        $menu .= "  <ul>\n";
        foreach ($g_menu[$atkmenutop] as $menuitem) {
            $menu .= $this->getMenuItem($menuitem, "    ");
        }

        if (Config::getGlobal("menu_logout_link")) {
            $menu .= "    <li><a href=\"./?atklogout=1\">" . Tools::atktext('logout') . "</a></li>\n";
        }

        $menu .= "  </ul>\n";

        $menu .= "</div>\n";
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
                    $submenu .= $this->getMenuItem($submenuitem, $indentation . "  ", $submenuname = '',
                        $menuitem['name']);
                }
                $submenu .= $indentation . "</ul>\n";
                $menu .= $indentation . $this->getItemHtml($menuitem, "\n" . $submenu . $indentation);
            } else {
                $menu .= $indentation . $this->getItemHtml($menuitem);
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
        if ($menuitem['name'] == '-') {
            return "<li class=\"separator\"><div></div></li>\n";
        }
        if ($menuitem['url'] && substr($menuitem['url'], 0, 11) == 'javascript:') {
            $href = '<a href="javascript:void(0)" onclick="' . htmlentities($menuitem['url']) . '; return false;">' . htmlentities($this->getMenuTranslation($menuitem['name'],
                    $menuitem['module'])) . '</a>';
        } else {
            if ($menuitem['url']) {
                $href = Tools::href($menuitem['url'], $this->getMenuTranslation($menuitem['name'], $menuitem['module']),
                    SessionManager::SESSION_NEW);
            } else {
                $href = '<a href="#">' . $name . '</a>';
            }
        }

        return "<li id=\"{$menuitem['module']}.{$menuitem['name']}\" class=\"$submenuname\">" . $href . $delimiter . $submenu . "</li>\n";
    }


    /**
     * Compare two menuitems
     *
     * @param array $a
     * @param array $b
     * @return int
     */
    function menu_cmp($a, $b)
    {
        if ($a["order"] == $b["order"]) {
            return 0;
        }
        return ($a["order"] < $b["order"]) ? -1 : 1;
    }

    /**
     * Get the height for this menu
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
     * @return int The menu position (Menu::MENU_RIGHT, Menu::MENU_TOP, Menu::MENU_BOTTOM or Menu::MENU_LEFT)
     */
    function getPosition()
    {
        switch (Config::getGlobal("menu_pos", "left")) {
            case "right":
                return self::MENU_RIGHT;
            case "top":
                return self::MENU_TOP;
            case "bottom":
                return self::MENU_BOTTOM;
        }
        return self::MENU_LEFT;
    }

    /**
     * Is this menu scrollable?
     *
     * @return int MENU_SCROLLABLE or MENU_UNSCROLLABLE
     */
    function getScrollable()
    {
        return self::MENU_SCROLLABLE;
    }

    /**
     * Is this menu multilevel?
     *
     * @return int MENU_MULTILEVEL or MENU_NOMULTILEVEL
     */
    function getMultilevel()
    {
        return self::MENU_MULTILEVEL;
    }

    /**
     * Recursively checks if a menuitem should be enabled or not.
     *
     * @param array $menuitem menuitem array
     * @return bool enabled?
     */
    function isEnabled($menuitem)
    {
        global $g_menu;

        $enable = $menuitem['enable'];
        if ((is_string($enable) || (is_array($enable) && count($enable) == 2 && is_object(@$enable[0]))) &&
            is_callable($enable)
        ) {
            $enable = call_user_func($enable);
        } else {
            if (is_array($enable)) {
                $enabled = false;
                for ($j = 0; $j < (count($enable) / 2); $j++) {
                    $enabled = $enabled || SecurityManager::is_allowed($enable[(2 * $j)], $enable[(2 * $j) + 1]);
                }
                $enable = $enabled;
            } else {
                if (array_key_exists($menuitem['name'], $g_menu) && is_array($g_menu[$menuitem['name']])) {
                    $enabled = false;
                    foreach ($g_menu[$menuitem['name']] as $item) {
                        $enabled = $enabled || $this->isEnabled($item);
                    }
                    $enable = $enabled;
                }
            }
        }

        return $enable;
    }

}

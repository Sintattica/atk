<?php namespace Sintattica\Atk\Core;


use Sintattica\Atk\Ui\Theme;
use Sintattica\Atk\Ui\Page;
use Sintattica\Atk\Session\SessionManager;
use Sintattica\Atk\Security\SecurityManager;

/**
 * Menu class.
 *
 * @author Ber Dohmen <ber@ibuildings.nl>
 * @author Sandy Pleyte <sandy@ibuildings.nl>
 * @package atk
 * @subpackage menu
 */
class Menu
{

    protected $menuItems = array();

    /**
     * Get new menu object
     *
     * @return Menu class object
     */
    public static function &getInstance()
    {
        static $s_instance = null;
        if ($s_instance == null) {
            Tools::atkdebug("Creating a new menu instance");
            $s_instance = new self();
        }

        return $s_instance;
    }


    /**
     * Translates a menuitem with the menu_ prefix, or if not found without
     *
     * @param string $menuitem Menuitem to translate
     * @param string $modname Module to which the menuitem belongs
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
        global $ATK_VARS;

        $page = Page::getInstance();
        $theme = Theme::getInstance();
        $page->register_script(Config::getGlobal('assets_url') . 'javascript/dropdown_menu.js');
        $page->register_style($theme->stylePath("atkdropdownmenu.css"));
        $page->m_loadscripts[] = "new DHTMLListMenu('nav');";

        $atkmenutop = array_key_exists('atkmenutop', $ATK_VARS) ? $ATK_VARS["atkmenutop"]
            : 'main';
        if (!is_array($this->menuItems[$atkmenutop])) {
            $this->menuItems[$atkmenutop] = array();
        }
        usort($this->menuItems[$atkmenutop], array($this, "menu_cmp"));

        $menu = "<div id=\"nav\">\n";

        $menu .= "  <ul>\n";
        foreach ($this->menuItems[$atkmenutop] as $menuitem) {
            $menu .= $this->getMenuItem($menuitem, "    ");
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
        $enable = $this->isEnabled($menuitem);
        $menu = '';
        if ($enable) {
            if (array_key_exists($menuitem['name'], $this->menuItems) && $this->menuItems[$menuitem['name']]) {
                $submenu = $indentation . "<ul>\n";
                foreach ($this->menuItems[$menuitem['name']] as $submenuitem) {
                    $submenu .= $this->getMenuItem($submenuitem, $indentation);
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
        $delimiter = '<br />';

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
     * Recursively checks if a menuitem should be enabled or not.
     *
     * @param array $menuitem menuitem array
     * @return bool enabled?
     */
    function isEnabled($menuitem)
    {
        $secManager = SecurityManager::getInstance();

        $enable = $menuitem['enable'];
        if ((is_string($enable) || (is_array($enable) && count($enable) == 2 && is_object(@$enable[0]))) &&
            is_callable($enable)
        ) {
            $enable = call_user_func($enable);
        } else {
            if (is_array($enable)) {
                $enabled = false;
                for ($j = 0; $j < (count($enable) / 2); $j++) {
                    $enabled = $enabled || $secManager->allowed($enable[(2 * $j)], $enable[(2 * $j) + 1]);
                }
                $enable = $enabled;
            } else {
                if (array_key_exists($menuitem['name'], $this->menuItems) && is_array($this->menuItems[$menuitem['name']])) {
                    $enabled = false;
                    foreach ($this->menuItems[$menuitem['name']] as $item) {
                        $enabled = $enabled || $this->isEnabled($item);
                    }
                    $enable = $enabled;
                }
            }
        }

        return $enable;
    }


    /**
     * Create a new menu item
     *
     * Both main menu items, separators, submenus or submenu items can be
     * created, depending on the parameters passed.
     *
     * @param string $name The menuitem name. The name that is displayed in the
     *                     userinterface can be influenced by putting
     *                     "menu_something" in the language files, where 'something'
     *                     is equal to the $name parameter.
     *                     If "-" is specified as name, the item is a separator.
     *                     In this case, the $url parameter should be empty.
     * @param string $url The url to load in the main application area when the
     *                    menuitem is clicked. If set to "", the menu is treated
     *                    as a submenu (or a separator if $name equals "-").
     *                    The dispatch_url() method is a useful function to
     *                    pass as this parameter.
     * @param string $parent The parent menu. If omitted or set to "main", the
     *                       item is added to the main menu.
     * @param mixed $enable This parameter supports the following options:
     *                      1: menuitem is always enabled
     *                      0: menuitem is always disabled
     *                         (this is useful when you want to use a function
     *                         call to determine when a menuitem should be
     *                         enabled. If the function returns 1 or 0, it can
     *                         directly be passed to this method in the $enable
     *                         parameter.
     *                      array: when an array is passed, it should have the
     *                             following format:
     *                             array("node","action","node","action",...)
     *                             When an array is passed, the menu checks user
     *                             privileges. If the user has any of the
     *                             node/action privileges, the menuitem is
     *                             enabled. Otherwise, it's disabled.
     * @param int $order The order in which the menuitem appears. If omitted,
     *                   the items appear in the order in which they are added
     *                   to the menu, with steps of 100. So, if you have a menu
     *                   with default ordering and you want to place a new
     *                   menuitem at the third position, pass 250 for $order.
     * @param $module string The name of the module that added this menuitem. It is usually
     *                not necessary to pass this parameter, but is present for
     *                backwardscompatibility reasons.
     */
    public function addMenuItem($name = "", $url = "", $parent = "main", $enable = 1, $order = 0, $module = "")
    {

        static $order_value = 100, $s_dupelookup = array();
        if ($order == 0) {
            $order = $order_value;
            $order_value += 100;
        }

        $item = array(
            "name" => $name,
            "url" => $url,
            "enable" => $enable,
            "order" => $order,
            "module" => $module
        );

        if (isset($s_dupelookup[$parent][$name]) && ($name != "-")) {
            $this->menuItems[$parent][$s_dupelookup[$parent][$name]] = $item;
        } else {
            $s_dupelookup[$parent][$name] = isset($this->menuItems[$parent]) ? count($this->menuItems[$parent])
                : 0;
            $this->menuItems[$parent][] = $item;
        }
    }
}



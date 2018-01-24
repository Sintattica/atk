<?php

namespace Sintattica\Atk\Core;

use Sintattica\Atk\Security\SecurityManager;
use Sintattica\Atk\Ui\Page;

class Menu
{
    protected $menuItems = [];

    protected $format_submenuparent = '
            <li>
                <a href="#">%s <span class="caret"></span></a>
                <ul class="dropdown-menu">%s</ul>
            <li>
        ';

    protected $format_submenuchild = '
            <li>
                <a href="#">%s <span class="caret"></span></a>
                <ul class="dropdown-menu">%s</ul>
            <li>
        ';

    protected $format_menu_left = '<ul class="main-nav nav navbar-nav navbar-left">%s</ul>';
    protected $format_menu_right = '<ul class="main-nav nav navbar-nav navbar-right">%s</ul>';

    protected $format_single = '<li><a href="%s" %s>%s</a></li>';

    protected $format_single_text = '<li><p class="navbar-text">%s</p></li>';

    /**
     * Get new menu object.
     *
     * @return Menu class object
     */
    public static function getInstance()
    {
        static $s_instance = null;
        if ($s_instance == null) {
            Tools::atkdebug('Creating a new menu instance');
            $s_instance = new static();
        }

        return $s_instance;
    }

    /**
     * Translates a menuitem with the menu_ prefix, or if not found without.
     *
     * @param string $menuitem Menuitem to translate
     * @param string $modname Module to which the menuitem belongs
     *
     * @return string Translation of the given menuitem
     */
    public function getMenuTranslation($menuitem, $modname = 'atk')
    {
        $s = Tools::atktext("menu_$menuitem", $modname, '', '', '', true);
        if (!$s) {
            $s = Tools::atktext($menuitem, $modname);
        }

        return $s;
    }

    /**
     * Render the menu.
     *
     * @return string HTML fragment containing the menu.
     */
    public function render()
    {
        $page = Page::getInstance();
        $menu = $this->load();
        $page->addContent($menu);

        return $page->render('Menu', true);
    }

    /**
     * Get the menu.
     *
     * @return string The menu
     */
    public function getMenu()
    {
        return $this->load();
    }

    /**
     * Load the menu.
     *
     * @return string The menu
     */
    public function load()
    {
        $html_items = $this->parseItems($this->menuItems['main']);

        $html_items_left = array_filter($html_items, function ($el) {
            return $el['navbar'] == 'left';
        });
        $left = '';
        $content = $this->processMenu($html_items_left);
        if ($content) {
            $left = sprintf($this->format_menu_left, $content);
        }

        $html_items_right = array_filter($html_items, function ($el) {
            return $el['navbar'] == 'right';
        });
        $right = '';
        $content = $this->processMenu($html_items_right);
        if ($content) {
            $right = sprintf($this->format_menu_right, $this->processMenu($html_items_right));
        }

        return $left.$right;
    }

    protected function processMenu($menu, $child = false)
    {
        $html = '';
        if (is_array($menu)) {
            foreach ($menu as $item) {
                if ($this->isEnabled($item)) {
                    if ($this->_hasSubmenu($item)) {
                        $a_content = $this->_getMenuTitle($item);
                        $childHtml = $this->processMenu($item['submenu'], true);
                        if ($child) {
                            $html .= sprintf($this->format_submenuchild, $a_content, $childHtml);
                        } else {
                            $html .= sprintf($this->format_submenuparent, $a_content, $childHtml);
                        }
                    } else {
                        $a_content = $this->_getMenuTitle($item);

                        $attrs = '';
                        if ($item['target']) {
                            $attrs .= ' target="'.$item['target'].'"';
                        }

                        if ($item['url']) {
                            $html .= sprintf($this->format_single, $item['url'], $attrs, $a_content);
                        } else {
                            $html .= sprintf($this->format_single_text, $a_content);
                        }
                    }
                }
            }
        }

        return $html;
    }

    /**
     * Recursively checks if a menuitem should be enabled or not.
     *
     * @param array $menuitem menuitem array
     *
     * @return bool enabled?
     */
    public function isEnabled($menuitem)
    {
        $secManager = SecurityManager::getInstance();

        $enable = $menuitem['enable'];
        if ((is_string($enable) || (is_array($enable) && Tools::count($enable) == 2 && is_object(@$enable[0]))) && is_callable($enable)) {
            $enable = call_user_func($enable);
        } else {
            if (is_array($enable)) {
                $enabled = false;
                for ($j = 0; $j < (Tools::count($enable) / 2); ++$j) {
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
     * Create a new menu item.
     *
     * Both main menu items, separators, submenus or submenu items can be
     * created, depending on the parameters passed.
     *
     * @param string $name The menuitem name. The name that is displayed in the
     *                       userinterface can be influenced by putting
     *                       "menu_something" in the language files, where 'something'
     *                       is equal to the $name parameter.
     *                       If "-" is specified as name, the item is a separator.
     *                       In this case, the $url parameter should be empty.
     * @param string $url The url to load in the main application area when the
     *                       menuitem is clicked. If set to "", the menu is treated
     *                       as a submenu (or a separator if $name equals "-").
     *                       The dispatch_url() method is a useful function to
     *                       pass as this parameter.
     * @param string $parent The parent menu. If omitted or set to "main", the
     *                       item is added to the main menu.
     * @param mixed $enable This parameter supports the following options:
     *                       1: menuitem is always enabled
     *                       0: menuitem is always disabled
     *                       (this is useful when you want to use a function
     *                       call to determine when a menuitem should be
     *                       enabled. If the function returns 1 or 0, it can
     *                       directly be passed to this method in the $enable
     *                       parameter.
     *                       array: when an array is passed, it should have the
     *                       following format:
     *                       array("node","action","node","action",...)
     *                       When an array is passed, the menu checks user
     *                       privileges. If the user has any of the
     *                       node/action privileges, the menuitem is
     *                       enabled. Otherwise, it's disabled.
     * @param int $order The order in which the menuitem appears. If omitted,
     *                       the items appear in the order in which they are added
     *                       to the menu, with steps of 100. So, if you have a menu
     *                       with default ordering and you want to place a new
     *                       menuitem at the third position, pass 250 for $order.
     * @param string $module The module name. Used for translations
     * @param string $target The link target (_self, _blank, ...)
     * @param string $navbar The navbar (left or right) where the menu will be put in
     * @param bool $raw If true, the $name will be rendered as is
     *
     */
    public function addMenuItem($name = '', $url = '', $parent = 'main', $enable = 1, $order = 0, $module = '', $target = '', $navbar = 'left', $raw = false)
    {
        static $order_value = 100, $s_dupelookup = [];
        if ($order == 0) {
            $order = $order_value;
            $order_value += 100;
        }

        $item = array(
            'name' => $name,
            'url' => $url,
            'enable' => $enable,
            'order' => $order,
            'module' => $module,
            'target' => $target,
            'navbar' => $navbar,
            'raw' => $raw,
        );

        if (isset($s_dupelookup[$parent][$name]) && ($name != '-')) {
            $this->menuItems[$parent][$s_dupelookup[$parent][$name]] = $item;
        } else {
            $s_dupelookup[$parent][$name] = isset($this->menuItems[$parent]) ? Tools::count($this->menuItems[$parent]) : 0;
            $this->menuItems[$parent][] = $item;
        }
    }

    protected function parseItems(&$items)
    {
        if (is_array($items)) {
            foreach ($items as &$item) {
                $this->parseItem($item);
            }
        }

        return $items;
    }

    protected function parseItem(&$item)
    {
        if ($item['enable'] && array_key_exists($item['name'], $this->menuItems)) {
            $item['submenu'] = $this->parseItems($this->menuItems[$item['name']]);

            return $item;
        }
    }

    public function _hasSubmenu($item)
    {
        return isset($item['submenu']) && Tools::count($item['submenu']);
    }

    public function _getMenuTitle($item, $append = '')
    {
        if ($item['raw'] == true) {
            return $item['name'];
        }

        return (string)$this->getMenuTranslation($item['name'], $item['module']).$append;
    }
}

<?php

namespace Sintattica\Atk\Core\Menu;

use Sintattica\Atk\Core\Tools;
use Sintattica\Atk\Security\SecurityManager;
use Sintattica\Atk\Ui\Page;

abstract class MenuBase
{

    public const MENU_SIDEBAR = 'sidebar';
    public const MENU_NAV_LEFT = 'left';
    public const MENU_NAV_RIGHT = 'right';

    //All menu items (sidebar and navbar)
    private array $items = [];
    private array $menu = [];


    //-------- Sidebar Menu  ------------
    //The submenu works by exploring all the children and then appending data from bottom up.

    //1) If the item has no children then it will be formatted as a simple item (formatSimpleItemSidebar)
    //where it can have a link or can be text only.
    //2) If the item has subitems it will be classified as a complex menu
    //      -> the two vars $formatSubmenuParentSidebar and $formatSubmenuChildSidebar are needed
    //   The recursive call sets up a variable called $childs that takes into account if we are on the root father
    //   that has subitems or a child that has subitems and depending from that decides to format:
    //      a) If top level father -> $formatSubmenuParentSidebar will be used
    //      b) If a child of the top level father -> $formatSubmenuChildSidebar will be used.
    //NB: The recursive call is working on dfs mode:
    //    1) Get the data from htmlItems
    //    2) Format in the following mode: Children -> SubmenuChildSidebar -> SubMenuParentSidebar
    //       Including the html in each step.
    //    3) Return a string that contains all the formatted sidebar menus.

    //These variables should be itended as formatting templates
    private $SIDEBAR_PARENT_TEMPLATE = '
              <li class="nav-item">
                <a href="#" class="nav-link">
                  <i class="far fa-circle nav-icon"></i>
                  <p>
                    %s
                    <i class="fas fa-angle-left right"></i>
                  </p>
                </a>
                <ul class="nav nav-treeview">%s</ul>
               </li>
            ';

    private $SIDEBAR_COMPLEX_ITEM_TEMPLATE = '
              <li class="nav-item">
                <a href="#" class="nav-link">
                  <i class="far fa-circle nav-icon"></i>
                  <p>
                    %s
                    <i class="fas fa-angle-left right"></i>
                  </p>
                </a>
                <ul class="nav nav-treeview">%s</ul>
               </li>
            ';

    private $SIDEBAR_ITEM_TEMPLATE = '
            <li class="nav-item">
              <a href="%s" %s class="nav-link">
               <i class="nav-icon fas fa-th"></i>
               <p>%s</p>
              </a>
            </li>';

    private $SIDEBAR_TEXT_ITEM_TEMPLATE = '
            <li class="nav-item">
              <a href="#" %s class="nav-link" style="cursor:default;">
               <i class="nav-icon fas fa-th"></i>
               <p>%s</p>
              </a>
            </li>';


    //-------- Navbar Menu  ------------

    private $NAVBAR_PARENT_TEMPLATE = '
        <li class="nav-item dropdown">
            <a id="dropdownSubMenu1" href="#" data-toggle="dropdown" 
                aria-haspopup="true" aria-expanded="false" 
                class="nav-link dropdown-toggle"
                >
                %s
            </a>
            <ul aria-labelledby="dropdownSubMenu1" class="dropdown-menu border-0 shadow">%s</ul>
        </li>';


    private $NAVBAR_COMPLEX_ITEM_TEMPLATE = '
        <li class="dropdown-submenu dropdown-hover">
            <a id="dropdownSubMenu2" href="#" role="button" 
                data-toggle="dropdown" aria-haspopup="true" aria-expanded="false" 
                class="dropdown-item dropdown-toggle"
                >
                %s
            </a>
                <ul aria-labelledby="dropdownSubMenu2" class="dropdown-menu border-0 shadow">%s</ul>
        </li>';


    private $NAVBAR_SINGLE_ITEM_NAV_MODE = '<li class="nav-item">
                                    <a class="nav-link" href="%s" %s>%s</a>
                                </li>';


    private $NAVBAR_SINGLE_ITEM_DROP_MODE = '<li class="nav-item">
                                    <a class="dropdown-item" href="%s" %s>%s</a>
                                </li>';

    private $NAVBAR_SINGLE_TEXT_ITEM_DROP_MODE = '<li class="nav-item">
                                        <span class="dropdown-item" style="cursor:default;">%s</span>
                                      </li>';

    private $NAVBAR_SINGLE_TEXT_ITEM_NAV_MODE = '<li class="nav-item">
                                        <span class="nav-link" style="cursor:default;">%s</span>
                                      </li>';


    public abstract function appendMenuItems();


    /**
     * Get new menu object.
     *
     * @return MenuBase class object
     */
    public static function getInstance()
    {
        static $s_instance = null;
        if ($s_instance == null) {
            Tools::atkdebug('Creating a new menu instance');
            $s_instance = new static();
            $s_instance->appendMenuItems();
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
    public function getMenuTranslation($menuitem, $modname = 'atk'): string
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
    public function render(): string
    {
        $page = Page::getInstance();
        $page->addContent($this->getMenu());

        return $page->render('Menu', true);
    }


    /**
     * Load a cached version of the menu.
     * The menu gets constructed only if is not present.
     * @return array The menu
     */
    public function getMenu(): array
    {
        if(!$this->menu) {
            $this->menu = $this->load();
        }

        return $this->menu;
    }


    /**
     * The same as getMenu() but without caching.
     * Load the complete menu independently if it was cached or not.
     * The function is used by Atk to load all the formatted menu complete with all parts (sidebar, navbar left and navbar right)
     * @return array This is a map with three keys:
     *               sidebar -> containing the html formatted sidebar menu
     *               left -> containning the html formatted items of the left navbar menu
     *               right -> same as the left but contains the items of the right part of the nav menu.
     */
    public function load(): array
    {

        $html_items = $this->parseItems($this->items['main']);

        $itemsLeftHtml = array_filter($html_items, fn($el): bool => $el['position'] === self::MENU_NAV_LEFT);
        $itemsRightHtml = array_filter($html_items, fn($el): bool => $el['position'] === self::MENU_NAV_RIGHT);
        $itemsSidebarHtml = array_filter($html_items, fn($el): bool => $el['position'] === self::MENU_SIDEBAR);

        return [
            self::MENU_NAV_LEFT => $this->processMenu($itemsLeftHtml) ?: '',
            self::MENU_NAV_RIGHT => $this->processMenu($itemsRightHtml) ?: '',
            self::MENU_SIDEBAR => $this->processMenu($itemsSidebarHtml, false, true) ?: ''
        ];
    }


    /**
     * @param array $menu - An array containing the all the menu (provided by the configuration of all the modules)
     * @param bool $child - Is this called from the recursive function to explore the child or is this the first call of this method.
     * @param bool $sidebar - Needed for the recursive function to understand if it was exploring
     *                        the nav or the sidebar part on the parent
     * @return string - A complete html formatted menu of the selected type (sidebar, navbar left or navbar right)
     */
    private function processMenu(array $menu, bool $child = false, bool $sidebar = false): string
    {
        $html = '';
        if (is_array($menu)) {
            foreach ($menu as $item) {
                if ($this->isEnabled($item)) {
                    if ($sidebar) {
                        $html .= $this->formatSidebar($item, $child);
                    } else {
                        $html .= $this->formatNavBar($item, $child);
                    }
                }
            }
        }

        return $html;
    }


    /**
     * The navbar part. This is very similar to the formatSidebarMenu(...)
     * The main differences are the templates to be used for the the html part.
     * @param array $item - The menu Item containing all the submenus
     * @param bool $child - Decides it called from the recursive function or is this the main call.
     * @return string - Containing the generated html for this part of the menu.
     */
    private function formatNavBar(array $item, bool $child): string
    {
        $html = '';
        $menuTitle = $this->getMenuTitle($item);

        if ($this->hasSubmenu($item)) {
            $childHtml = $this->processMenu($item['submenu'], true, false);

            if ($child) {
                $html .= sprintf($this->NAVBAR_COMPLEX_ITEM_TEMPLATE, $menuTitle, $childHtml);
            } else {
                $html .= sprintf($this->NAVBAR_PARENT_TEMPLATE, $menuTitle, $childHtml);
            }

        } else {
            $attrs = '';
            if ($item['target']) {
                $attrs .= ' target="' . $item['target'] . '"';
            }


            if ($child) {
                if ($item['url']) {
                    $html .= sprintf($this->NAVBAR_SINGLE_ITEM_DROP_MODE, $item['url'], $attrs, $menuTitle);
                } else {
                    $html .= sprintf($this->NAVBAR_SINGLE_TEXT_ITEM_DROP_MODE, $menuTitle);
                }
            } else {
                if ($item['url']) {
                    $html .= sprintf($this->NAVBAR_SINGLE_ITEM_NAV_MODE, $item['url'], $attrs, $menuTitle);
                } else {
                    $html .= sprintf($this->NAVBAR_SINGLE_TEXT_ITEM_NAV_MODE, $menuTitle);
                }
            }
        }

        return $html;

    }


    /**
     * @param array $item - The menu Item containing all the submenus
     * @param bool $child - Decides it called from the recursive function or is this the main call.
     * @return string - Containing the generated html for this part of the menu.
     */
    private function formatSidebar(array $item, bool $child): string
    {
        $html = '';
        $menuTitle = $this->getMenuTitle($item);

        if ($this->hasSubmenu($item)) {

            //explore the child before formatting the parent (depth-first)
            $childHtml = $this->processMenu($item['submenu'], true, true);

            if ($child) {
                $html .= sprintf($this->SIDEBAR_COMPLEX_ITEM_TEMPLATE, $menuTitle, $childHtml);
            } else {
                $html .= sprintf($this->SIDEBAR_PARENT_TEMPLATE, $menuTitle, $childHtml);
            }

            //Caso Simple Item -> No submenu
        } else {
            $attrs = '';
            if ($item['target']) {
                $attrs .= ' target="' . $item['target'] . '"';
            }

            if ($item['url']) {
                $html .= sprintf($this->SIDEBAR_ITEM_TEMPLATE, $item['url'], $attrs, $menuTitle);
            } else {
                $html .= sprintf($this->SIDEBAR_TEXT_ITEM_TEMPLATE, $attrs, $menuTitle);
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
    public function isEnabled(array $menuitem): bool
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
                if (array_key_exists($menuitem['name'], $this->items) && is_array($this->items[$menuitem['name']])) {
                    $enabled = false;
                    foreach ($this->items[$menuitem['name']] as $item) {
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
     * @param string $position The destination (sidebar, navbar left or navbar right) where the menu will be put in
     * @param bool $raw If true, the $name will be rendered as is
     *
     */
    public function addMenuItem($name = '', $url = '', $parent = 'main', $enable = 1, $order = 0, $module = '', $target = '', $position = self::MENU_SIDEBAR, $raw = false)
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
            'position' => $position,
            'raw' => $raw,
        );

        if (isset($s_dupelookup[$parent][$name]) && ($name != '-')) {
            $this->items[$parent][$s_dupelookup[$parent][$name]] = $item;
        } else {
            $s_dupelookup[$parent][$name] = isset($this->items[$parent]) ? Tools::count($this->items[$parent]) : 0;
            $this->items[$parent][] = $item;
        }
    }


    private function parseItems(array &$items): array
    {

        foreach ($items as &$item) {
            $this->parseItem($item);
        }

        return $items;
    }


    private function parseItem(array &$item): ?array
    {
        if ($item['enable'] && array_key_exists($item['name'], $this->items)) {
            $item['submenu'] = $this->parseItems($this->items[$item['name']]);
            return $item;
        }

        return null;
    }


    private function hasSubmenu(array $item): bool
    {
        return isset($item['submenu']) && Tools::count($item['submenu']);
    }


    private function getMenuTitle(array $item, string $append = ''): string
    {
        if ($item['raw'] == true) {
            return $item['name'];
        }

        return (string)$this->getMenuTranslation($item['name'], $item['module']) . $append;
    }

}

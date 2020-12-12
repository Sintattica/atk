<?php

namespace Sintattica\Atk\Core\Menu;

use Sintattica\Atk\Core\Language;
use Sintattica\Atk\Core\Tools;
use Sintattica\Atk\Security\SecurityManager;
use Sintattica\Atk\Ui\Page;
use Sintattica\Atk\Ui\SmartyProvider;
use SmartyException;

abstract class MenuBase
{

    public const MENU_SIDEBAR = 'sidebar';
    public const MENU_NAV_LEFT = 'left';
    public const MENU_NAV_RIGHT = 'right';

    private const TYPE_MENU_SIDEBAR = 'sidebar';
    private const TYPE_MENU_NAVBAR = 'navbar';

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

    //-------- Sidebar Menu  ------------
    private const SIDEBAR_PARENT_ITEM_TPL = "menu/sidebar/parent_item.tpl";
    private const SIDEBAR_CHILD_ITEM_TPL = "menu/sidebar/child_item.tpl";
    private const SIDEBAR_HEADER_ITEM_TPL = "menu/sidebar/header_item.tpl";

    //-------- Navbar Menu  ------------
    private const NAVBAR_PARENT_ITEM_TPL = 'menu/navbar/parent_item.tpl';
    private const NAVBAR_SUBPARENT_ITEM_TPL = 'menu/navbar/subparent_item.tpl';
    private const NAVBAR_CHILD_ITEM_TPL = 'menu/navbar/child_item.tpl';


    public abstract function appendMenuItems();


    /**
     * Get new menu object.
     *
     * @return MenuBase class object
     */
    public static function getInstance(): ?MenuBase
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
     * @throws SmartyException
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
     * @throws SmartyException
     */
    public function getMenu(): array
    {
        if (!$this->menu) {
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
     *               left -> containing the html formatted items of the left navbar menu
     *               right -> same as the left but contains the items of the right part of the nav menu.
     * @throws SmartyException
     */
    public function load(): array
    {

        $html_items = $this->parseItems($this->items['main']);

        $itemsLeftHtml = array_filter($html_items, fn($el): bool => $el['position'] === self::MENU_NAV_LEFT);
        $itemsRightHtml = array_filter($html_items, fn($el): bool => $el['position'] === self::MENU_NAV_RIGHT);
        $itemsSidebarHtml = array_filter($html_items, fn($el): bool => $el['position'] === self::MENU_SIDEBAR);

        return [
            self::MENU_NAV_LEFT => $this->processMenu($itemsLeftHtml, false, self::TYPE_MENU_NAVBAR) ?: '',
            self::MENU_NAV_RIGHT => $this->processMenu($itemsRightHtml, false, self::TYPE_MENU_NAVBAR) ?: '',
            self::MENU_SIDEBAR => $this->processMenu($itemsSidebarHtml) ?: ''
        ];
    }


    /**
     * @param array $menu - An array containing the all the menu (provided by the configuration of all the modules)
     * @param bool $child - Is this called from the recursive function to explore the child or is this the first call of this method.
     * @param string $type - Needed for the recursive function to understand if it was exploring
     *                        the nav or the sidebar part on the parent
     * @return string - A complete html formatted menu of the selected type (sidebar, navbar left or navbar right)
     * @throws SmartyException
     */
    private function processMenu(array $menu, bool $child = false, string $type = self::TYPE_MENU_SIDEBAR): string
    {
        $html = '';
        if (is_array($menu)) {
            foreach ($menu as $item) {
                if ($this->isEnabled($item)) {
                    switch ($type) {
                        case self::TYPE_MENU_SIDEBAR:
                            $html .= $this->formatSidebar($item);
                            break;
                        case self::TYPE_MENU_NAVBAR:
                            $html .= $this->formatNavBar($item, $child);
                            break;
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
     * @throws SmartyException
     */
    private function formatNavBar(array $item, bool $child): string
    {
        $html = '';
        $title = $this->getMenuTitle($item);
        $classes = isset($item['classes']) ? $item['classes'] : '';

        if ($this->hasSubmenu($item)) {
            $submenu = $this->processMenu($item['submenu'], true, self::TYPE_MENU_NAVBAR);
            $template = $child ? self::NAVBAR_SUBPARENT_ITEM_TPL : self::NAVBAR_PARENT_ITEM_TPL;
            $html .= SmartyProvider::render($template, ['title' => $title, 'submenu' => $submenu, "classes" => $classes]);

        } else {
            $attrs = '';
            if ($item['target']) {
                $attrs .= ' target="' . $item['target'] . '"';
            }

            $link = isset($item['url']) ? $item['url'] : '';
            $classes .= $child ? ' dropdown-item' : ' nav-link';
            $html .= SmartyProvider::render(self::NAVBAR_CHILD_ITEM_TPL, ['title' => $title, 'link' => $link, "classes" => $classes, "attributes" => $attrs]);
        }

        return $html;

    }


    /**
     * @param array $item - The menu Item containing all the submenus
     * @return string - Containing the generated html for this part of the menu.
     * @throws SmartyException
     */
    private function formatSidebar(array $item): string
    {
        $html = '';
        $title = $this->getMenuTitle($item);

        $classes = isset($item['classes']) ? $item['classes'] : '';

        if ($this->hasSubmenu($item)) {

            //explore the child before formatting the parent (depth-first)
            $subMenu = $this->processMenu($item['submenu'], true);
            $html .= SmartyProvider::render(self::SIDEBAR_PARENT_ITEM_TPL, ['title' => $title, 'submenu' => $subMenu, "classes" => $classes]);


        } else {
            //Caso Simple Item -> No submenu
            switch ($item['type']) {
                case MenuItem::TYPE_HEADER:
                    $template = self::SIDEBAR_HEADER_ITEM_TPL;
                    break;
                default:
                    $template = self::SIDEBAR_CHILD_ITEM_TPL;
                    break;
            }

            $attrs = '';
            if ($item['target']) {
                $attrs .= ' target="' . $item['target'] . '"';
            }

            $link = isset($item['url']) ? $item['url'] : '';
            $html .= SmartyProvider::render($template, ['title' => $title, 'attributes' => $attrs, 'link' => $link, "classes" => $classes]);
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
     * @param bool $raw If true, the $name will be rendered as is
     * @param string $position The destination (sidebar, navbar left or navbar right) where the menu will be put in
     * @param string $type
     */
    public function addMenuItem($name = '', $url = '', $parent = 'main', $enable = 1, $order = 0, $module = '', $target = '', $raw = false, $position = self::MENU_SIDEBAR, $type = MenuItem::TYPE_LINK)
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
            'type' => $type,
            'raw' => $raw,
        );

        if (isset($s_dupelookup[$parent][$name]) && ($name != '-')) {
            $this->items[$parent][$s_dupelookup[$parent][$name]] = $item;
        } else {
            $s_dupelookup[$parent][$name] = isset($this->items[$parent]) ? Tools::count($this->items[$parent]) : 0;
            $this->items[$parent][] = $item;
        }
    }

    // todo: Check how we can provide those with the last param.
    //Currently not exposed
    private function addMenuItem2(string $parent = '', string $name = '', string $nodeUri = '', string $action = '', string $position = self::MENU_SIDEBAR, array $extraParams = [])
    {
        $parent = $parent ?: 'main'; //Default parent is name

        //Default name is the translation of the node name
        if (!$name) {
            list($modulo, $nodo) = explode('.', $nodeUri);
            $name = Language::text($nodo, $modulo);
        }

        //todo: Set default filters in urlParams!
        $urlParams = [];
        $url = Tools::dispatch_url($nodeUri, $action, $urlParams);

        $enable = isset($extraParams['enable']) ?: 1;
        $order = isset($extraParams['order']) ?: 0;
        $module = isset($extraParams['module']) ?: '';
        $target = isset($extraParams['target']) ?: '';
        $raw = isset($extraParams['raw']) ?: false;
        $type = isset($extraParams['type']) ?: MenuItem::TYPE_LINK;

        $this->addMenuItem($name, $url, $parent, $enable, $order, $module, $target, $raw, $position, $type);
    }

    /**
     * Add a new MenuItem to the Menu
     * @param MenuItem $menuItem
     * @return MenuItem
     */
    public function add(MenuItem $menuItem): MenuItem
    {
        $this->addMenuItem(
            $menuItem->getName(),
            $menuItem->getUrl(),
            $menuItem->getParent(),
            $menuItem->getEnable(),
            $menuItem->getOrder(),
            $menuItem->getModule(),
            $menuItem->getTarget(),
            $menuItem->isRaw(),
            $menuItem->getPosition(),
            $menuItem->getType(),
        );

        return $menuItem;
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

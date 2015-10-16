<?php namespace Sintattica\Atk\Menu;

use Sintattica\Atk\Session\SessionManager;
use Sintattica\Atk\Ui\Page;
use Sintattica\Atk\Ui\Theme;
use Sintattica\Atk\Core\Config;
use Sintattica\Atk\Ui\Ui;

/**
 * Modern menu implementation
 * @package atk
 * @subpackage menu
 */
class ModernMenu extends PlainMenu
{

    /**
     * Constructor
     *
     * @return ModernMenu
     */
    function __construct()
    {
        parent::__construct();
        $this->m_height = "130";
    }

    /**
     * Render the menu
     *
     * @return string
     */
    function render()
    {
        $page = Page::getInstance();
        $page->addContent($this->getMenu());
        return $page->render("Menu", true);
    }

    /**
     * Get the menu
     *
     * @return string The menu
     */
    function getMenu()
    {
        $theme = Theme::getInstance();
        $sm = SessionManager::getInstance();

        global $g_menu;
        $atkmenutop = (isset($_REQUEST['atkmenutop']) ? $_REQUEST['atkmenutop']
            : $sm::sessionLoad('atkmenutop'));
        $sm::sessionStore('atkmenutop', $atkmenutop);

        $menuitems = $this->getMenuItems($g_menu, 'main');

        $page = Page::getInstance();
        $page->register_style($theme->stylePath("style.css"));
        $page->register_style($theme->stylePath("menu.css"));
        $page->register_script(Config::getGlobal("atkroot") . "atk/javascript/menuload.js");

        $ui = Ui::getInstance();
        $atkmenutop = (isset($_REQUEST['atkmenutop']) ? $_REQUEST['atkmenutop']
            : $atkmenutop);

        $box = $ui->renderBox(array(
            "title" => $this->getMenuTranslation(($atkmenutop
                ? $atkmenutop : 'main')),
            "menuitems" => $menuitems,
            'menutop' => $atkmenutop,
            'g_menu' => $g_menu,
            'atkmenutop' => $atkmenutop,
            'atkmenutopname' => $this->getMenuTopName($menuitems, $atkmenutop)
        ), "menu");

        return $box;
    }

    /**
     * Get the menu top name
     *
     * @param array $menuitems
     * @param string $menutop
     * @return string The name of the menu top item
     */
    function getMenuTopName($menuitems, $menutop)
    {
        foreach ($menuitems as $menuitem) {
            if ($menuitem['id'] == $menutop) {
                return $menuitem['name'];
            }
        }
        return '';
    }

    /**
     * Get menuitems
     *
     * @param array $menu
     * @param string $menutop
     * @return array Array with menu items
     */
    function getMenuItems($menu, $menutop)
    {
        $menuitems = array();

        if (isset($menu[$menutop]) && is_array($menu[$menutop])) {
            usort($menu[$menutop], array("atkplainmenu", "menu_cmp"));
            foreach ($menu[$menutop] as $menuitem) {
                $menuitem['id'] = $menuitem['name'];
                $menuitem['enable'] = $menuitem['name'] != '-' && $this->isEnabled($menuitem);

                $this->addSubMenuForMenuitem($menu, $menuitem);

                if ($menuitem['name'] !== '-') {
                    $menuitem['name'] = $this->getMenuTranslation($menuitem['name'], $menuitem['module']);
                }

                $theme = Theme::getInstance();
                $menu_icon = $theme->iconPath($menutop . '_' . $menuitem['id'], "menu", $menuitem['module']);
                if ($menu_icon) {
                    $menuitem['image'] = $menu_icon;
                }

                if (!empty($menuitem['url'])) {
                    $menuitem['url'] = SessionManager::sessionUrl($menuitem['url'] . "&amp;atkmenutop={$menuitem['id']}", SESSION_NEW);
                }
                $menuitem['header'] = $this->getHeader($menuitem['id']);
                $menuitems[] = $menuitem;
            }
        }
        return $menuitems;
    }

    /**
     * Add submenu for menu item
     *
     * @param array $menu
     * @param array $menuitem
     */
    function addSubMenuForMenuItem($menu, &$menuitem)
    {
        // submenu
        if (!isset($menu[$menuitem['name']])) {
            return;
        }

        $menuitem['submenu'] = $menu[$menuitem['name']];
        foreach ($menuitem['submenu'] as $submenukey => $submenuitem) {
            $menuitem['submenu'][$submenukey]['id'] = $menuitem['submenu'][$submenukey]['name'];
            if ($menuitem['submenu'][$submenukey]['name'] !== '-') {
                $menuitem['submenu'][$submenukey]['name'] = $this->getMenuTranslation($submenuitem['name'],
                    $submenuitem['module']);
            }
            if (!empty($submenuitem['url'])) {
                if (strpos($submenuitem['url'], "?") !== false) {
                    $start = "&amp;";
                } else {
                    $start = "?";
                }
                $url = $submenuitem['url'] . $start . "atkmenutop={$menuitem['id']}";

                $menuitem['submenu'][$submenukey]['enable'] = $menuitem['submenu'][$submenukey]['name'] != '-' &&
                    $this->isEnabled($menuitem['submenu'][$submenukey]);
                $menuitem['submenu'][$submenukey]['url'] = SessionManager::sessionUrl($url, SESSION_NEW);
            }
            $theme = Theme::getInstance();
            $menu_icon = $theme->iconPath($menuitem['id'] . '_' . $submenuitem['name'], "menu", $submenuitem['module']);
            if ($menu_icon) {
                $menuitem['submenu'][$submenukey]['image'] = $menu_icon;
            }
        }
    }

    /**
     * Get the header
     *
     * @param string $atkmenutop
     * @return string Empty string
     */
    function getHeader($atkmenutop)
    {
        return '';
    }

}
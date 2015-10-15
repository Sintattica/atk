<?php

use Sintattica\Atk\Menu\PlainMenu;
use Sintattica\Atk\Core\Tools;

/**
 * Implementation of the Bootstrap menu.
 *
 * @author Michele Rota <michele.rota@me.com>
 * @package atk
 * @subpackage menu
 */
class BootstrapMenu extends PlainMenu
{
    private $format_submenuparent = '
            <li class="dropdown">
                <a tabindex="0" data-toggle="dropdown">
                    %s <span class="caret"></span>
                </a>
                <ul class="dropdown-menu" role="menu">
                    %s
                </ul>
            <li>
        ';


    private $format_submenuchild = '
            <li class="dropdown-submenu">
                <a tabindex="0" data-toggle="dropdown">
                    %s
                </a>
                <ul class="dropdown-menu" role="menu">
                    %s
                </ul>
            <li>
        ';
    private $format_menu = '<ul class="nav navbar-nav"> %s </ul>';


    private $format_single = '
        <li> <a href="%s">%s</a> </li>
    ';

    /**
     * Render the menu
     * @return String HTML fragment containing the menu.
     */
    function render()
    {
        /** @var Page $page */
        $page = Page::getInstance();
        /** @var Theme $theme */
        $theme = Tools::atkinstance('atk.ui.atktheme');
        $page->register_style($theme->absPath("atk/themes/bootstrap/lib/bootstrap-submenu/css/bootstrap-submenu.min.css"));
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
        global $g_menu;
        /** @var Page $page */
        $page = Page::getInstance();
        /** @var atkTheme $theme */
        $theme = Tools::atkinstance('atk.ui.atktheme');
        $page->register_style($theme->absPath("atk/themes/bootstrap/lib/bootstrap-submenu/css/bootstrap-submenu.min.css"));
        $page->register_script($theme->absPath("atk/themes/bootstrap/lib/bootstrap-submenu/js/bootstrap-submenu.min.js"));
        $page->register_script($theme->absPath("atk/themes/bootstrap/js/menu.js"));

        $html_items = $this->parseItems($g_menu['main']);
        $html_menu = $this->processMenu($html_items);
        return sprintf($this->format_menu, $html_menu);
    }

    private function processMenu($menu, $child = false)
    {
        $html = '';

        foreach ($menu as $item) {
            if ($this->isEnabled($item)) {
                $url = $item['url'] ? $item['url'] : '#';
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
                    $html .= sprintf($this->format_single, $url, $a_content);
                }
            }
        }
        return $html;
    }


    private function parseItems(&$items)
    {
        foreach ($items as &$item) {
            $this->parseItem($item);
        }
        return $items;
    }

    private function parseItem(&$item)
    {
        global $g_menu;
        if ($item['enable'] && array_key_exists($item['name'], $g_menu)) {
            $item['submenu'] = $this->parseItems($g_menu[$item['name']]);
            return $item;
        }
    }

    function _hasSubmenu($item)
    {
        return isset($item['submenu']) && count($item['submenu']);
    }

    function _getMenuTitle($item, $append = "")
    {
        return htmlentities($this->getMenuTranslation($item['name'], $item['module'])) . $append;
    }

}

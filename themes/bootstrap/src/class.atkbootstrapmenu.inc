<?php
/**
 * This file is part of the ATK distribution on GitHub.
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
 * @version $Revision: 6347 $
 * $Id$
 */
atkimport('atk.menu.atkplainmenu');

/**
 * Implementation of the Bootstrap menu.
 *
 * @author Michele Rota <michele.rota@me.com>
 * @package atk
 * @subpackage menu
 */
class atkBootstrapMenu extends atkPlainMenu
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
        /** @var atkPage $page */
        $page = & atkinstance("atk.ui.atkpage");
        /** @var atkTheme $theme */
        $theme = atkinstance('atk.ui.atktheme');
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
        /** @var atkPage $page */
        $page = & atkinstance("atk.ui.atkpage");
        /** @var atkTheme $theme */
        $theme = &atkinstance('atk.ui.atktheme');
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
                    if($child)
                        $html .= sprintf($this->format_submenuchild, $a_content, $childHtml );
                    else
                        $html .= sprintf($this->format_submenuparent, $a_content, $childHtml );
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
        return (string)$this->getMenuTranslation($item['name'], $item['module']) . $append;
    }

}

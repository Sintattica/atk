<?php

/**
 * Implementation of "SmartMenus" jQuery menu plugin (http://www.smartmenus.org)
 *
 * In themedef, you can optionally define:
 * - sm_theme: one of the four SmartMenus themes available ('clean' (dafault), 'blue', 'mint', 'simple')
 * - sm_logo: the logo for this menu, e.g. 'images/logo.png'
 * - sm_options: a json string of SmartMenus options (see http://www.smartmenus.org/docs#options)
 * - sm_atk_usernode: node to be used (instead of the default auth_usernode) for the user/s links in the menu
 *
 * @package atk
 * @subpackage menu
 */
class atkSmartMenu extends atkDropdownMenu
{
    /**
     * Load the menu
     *
     * @return string The menu
     */
    function load()
    {
        global $ATK_VARS, $g_menu;

        /** @var atkPage $page */
        $page = &atkinstance('atk.ui.atkpage');
        /** @var atkTheme $theme */
        $theme = &atkinstance("atk.ui.atktheme");


        /** CONFIGURATION */

        // SmartMenus options
        $smOptions = $theme->getAttribute('sm_options');

        // SmartMenus theme (default: 'clean')
        $smTheme = $theme->getAttribute('sm_style');
        $smTheme = $smTheme && in_array($smTheme, ['clean', 'blue', 'mint', 'simple']) ? $smTheme : 'clean';

        // logo
        $logo = $theme->getAttribute('basepath') . $theme->getAttribute('sm_logo');

        // usernode
        if ($theme->getAttribute('sm_atk_usernode')) {
            $userNode = atkGetNode($theme->getAttribute('sm_atk_usernode'));
        } else if (atkconfig('auth_usernode')) {
            $userNode = atkGetNode(atkconfig('auth_usernode'));
        }


        /** STYLES */

        // SmartMenus core style
        $page->register_style(atkconfig('atkroot') . 'atk/menu/smartmenus/css/sm-core-css.css');

        // SmartMenus theme style
        $page->register_style(atkconfig('atkroot') . "atk/menu/smartmenus/css/sm-$smTheme/sm-$smTheme.css");

        // SmartMenus custom style
        $page->register_style(atkconfig('atkroot') . 'atk/menu/smartmenus/css/sm-custom.css');

        // ovverridable style in atk theme
        $page->register_style($theme->stylePath('atksmartmenu.css'));

        // font awesome
        $page->register_style(atkconfig('atkroot') . 'atk/themes/bootstrap/lib/font-awesome-4.2.0/css/font-awesome.min.css');


        /** SCRIPTS */

        // SmartMenus core
        $page->register_script(atkconfig('atkroot') . 'atk/menu/smartmenus/jquery.smartmenus.min.js');

        // script for collapsable menu
        $page->register_loadscript("
        jQuery('#collapse-menu-button').click(function (e) {
            var item = jQuery(this),
                menu = jQuery('#sm-menu');
            if (menu.is(':animated')) {
                return false;
            }
            if (!item.hasClass('collapsed')) {
                menu.slideUp(250, function () {
                    jQuery(this).addClass('collapsed').css('display', '');
                });
                item.addClass('collapsed');
            } else {
                menu.slideDown(250, function () {
                    jQuery(this).removeClass('collapsed');
                });
                item.removeClass('collapsed');
            }
            e.preventDefault();
        });");

        // push page content to bottom on resize
        $page->register_loadscript("
        jQuery(window).on('resize', function() {
            if(!jQuery('#collapse-menu-button').is(':visible')){
                jQuery('#topSpacer').css('height', jQuery('#sm-menu').height() + 4);
            } else {
                jQuery('#topSpacer').css('height', 45);
            }
        }).resize();");

        // enable SmartMenus
        $page->register_loadscript("jQuery('#sm-menu').smartmenus($smOptions);");


        /** RENDERING */

        $atkmenutop = array_key_exists('atkmenutop', $ATK_VARS) ? $ATK_VARS['atkmenutop'] : 'main';
        if (!is_array($g_menu[$atkmenutop])) {
            $g_menu[$atkmenutop] = array();
        }
        usort($g_menu[$atkmenutop], array('atkplainmenu', 'menu_cmp'));

        $menu = '<div id="nav">';

        // logo and menu header in collapsed mode
        $menu .= '
            <div id="collapse-menu-wrapper">
                <a href="index.php" id="collapse-menu-logo" class="collapsed"><img src="' . $logo . '" style="height:24px; padding: 0; margin:0;"></a>
                <a href="#" id="collapse-menu-button" class="collapsed"><i class="fa fa-bars"></i>&nbsp;&nbsp;Menu</a>
            </div>';

        // menu
        $menu .= "
            <ul id=\"sm-menu\" class=\"sm sm-$smTheme collapsed\">";

        if ($logo) { // logo in desktop mode
            $menu .= '
                <li id="smartmenu-logo">
                    <a href="index.php" style="top:-4px; padding-bottom:0; margin-bottom: 0">
                        <img src="' . $logo . '" style="height:24px; padding: 0; margin:0;">
                    </a>
                </li>';
        }

        // add menu items
        foreach ($g_menu[$atkmenutop] as $menuitem) {
            $menu .= $this->getMenuItem($menuitem);
        }

        // add logout link
        $menu .= '<li id="logout-link"><a href="index.php?atklogout=1"><i class="fa fa-sign-out fa-lg"></i></a></li>';

        // add current user name and view/edit profile link
        $user = atkGetUser();
        $userName = $user['name'];
        $userLink = '#';
        if (atkGetUserId() && $userNode) {
            $userName = $userNode->descriptor($user, 'menu');
            if ($userNode->allowed('edit')) {
                $userLink = dispatch_url($userNode, 'edit', ['atkselector' => $userNode->primaryKey($user)]);
            } else if ($userNode->allowed('view')) {
                $userLink = dispatch_url($userNode, 'view', ['atkselector' => $userNode->primaryKey($user)]);
            }
        }
        $menu .= '<li id="user-link"><a href="' . $userLink . '"><i class="fa fa-user fa-lg"></i> ' . $userName . '</a></li>';

        // add users list link
        if ($userNode && $userNode->allowed('admin')) {
            $usersLink = dispatch_url($userNode, 'admin');
            $menu .= '<li id="users-link"><a href="' . $usersLink . '"><i class="fa fa-users fa-lg"></i></a></li>';
        }

        $menu .= '</ul>';

        $menu .= '</div>';

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
                $submenu = '';
                foreach ($g_menu[$menuitem['name']] as $submenuitem) {
                    $submenu .= $this->getMenuItem($submenuitem, '', $submenuname = '', $menuitem['name']);
                }
                if ($submenu) {
                    $submenu = "<ul>$submenu</ul>";
                }
                $menu .= $this->getItemHtml($menuitem, "" . $submenu . $indentation);
            } else {
                $menu .= $this->getItemHtml($menuitem);
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
    function getItemHtml($menuitem, $submenu = "", $submenuname = '')
    {
        $delimiter = '';// atkconfig("menu_delimiter");

        $name = $this->getMenuTranslation($menuitem['name'], $menuitem['module']);
        if ($menuitem['name'] == '-') {
            return '<li class="separator"><div></div></li>';
        }
        if ($menuitem['url'] && substr($menuitem['url'], 0, 11) == 'javascript:') {
            $href = sprintf('<a href="javascript:void(0)" onclick="%s; return false;">%s</a>',
                atk_htmlentities($menuitem['url']), atk_htmlentities($this->getMenuTranslation($menuitem['name'], $menuitem['module'])));
        } else if ($menuitem['url']) {
            $href = href($menuitem['url'], $this->getMenuTranslation($menuitem['name'], $menuitem['module']), SESSION_NEW);
        } else {
            $href = '<a href="#">' . $name . '</a>';
        }

        return "<li id=\"{$menuitem['module']}.{$menuitem['name']}\" class=\"$submenuname\">" . $href . $delimiter . $submenu . "</li>";
    }
}
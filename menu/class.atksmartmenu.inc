<?php

/**
 * Smartmenu implementation - http://www.smartmenus.org/
 * In themedef, you can optionally define:
 * - sm_style: one of the four styles available (mint, blue, simple, clean)
 * - sm_logo: the logo for this menu, e.g. 'images/logo.png'
 * - sm_atk_user_node: if you don't want to use the default auth_usernode
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

        $page = &atkinstance('atk.ui.atkpage');
        $theme = &atkinstance("atk.ui.atktheme");

        // set the smartmenu style
        $style = $theme->getAttribute('sm_style');
        $smStyles = ['clean', 'mint', 'blue', 'simple'];
        $type = $style && in_array($style, $smStyles) ?
            $style : $smStyles[0];

        // smartmenu scripts and styles
        // the default atk banner (#banner) is hidden in sm-core-css
        $page->register_style(atkconfig('atkroot') . 'atk/menu/smartmenus/css/sm-core-css.css');
        $page->register_style(atkconfig('atkroot') . "atk/menu/smartmenus/css/sm-$type/sm-$type.css");
        $page->register_script(atkconfig('atkroot') . 'atk/menu/smartmenus/jquery.smartmenus.min.js');

        // font awesome
        $page->register_style(atkconfig('atkroot') . 'atk/themes/bootstrap/lib/font-awesome-4.2.0/css/font-awesome.min.css');

        // ovverridable smartmenu style sheet
        $page->register_style($theme->stylePath("atksmartmenu.css"));

        // script for collapsable smartmenu
        $page->m_loadscripts[] = "jQuery('#collapse-menu-button').click(function (e) {
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
        });";

        // push page content to bottom on resize
        $page->m_loadscripts[] = "
              jQuery(window).on('resize', function() {
                    if(!jQuery('#collapse-menu-button').is(':visible')){
                        jQuery('#topSpacer').css('height', jQuery('#sm-menu').height() + 4);
                    } else {
                        jQuery('#topSpacer').css('height', 45);
                    }
              }).resize();
        ";

        // enable smartmenu
        $page->m_loadscripts[] = "jQuery('#sm-menu').smartmenus({hideOnClick: false});";


        $atkmenutop = array_key_exists('atkmenutop', $ATK_VARS) ? $ATK_VARS["atkmenutop"]
            : 'main';
        if (!is_array($g_menu[$atkmenutop]))
            $g_menu[$atkmenutop] = array();
        usort($g_menu[$atkmenutop], array("atkplainmenu", "menu_cmp"));


        $menu = "<div id='nav'> <a href='#' id='collapse-menu-button' class='collapsed'><i class='fa fa-bars'></i>&nbsp;&nbsp;Menu</a>";
        $menu .= $this->getHeader($atkmenutop);
        $menu .= "<ul id='sm-menu' class='sm sm-$type collapsed'>";
        // add logo
        $logo = $theme->getAttribute("sm_logo");
        if ($logo) {
            $logo = $theme->getAttribute("basepath") . "$logo";
            $menu .= "<li id='smartmenu-logo'>
                        <a href='index.php' style='top:-4px; padding-bottom:0; margin-bottom: 0'>
                            <img src='$logo' style='height:24px; padding: 0; margin:0;'>
                        </a>
                    </li>";
        }
        // add menu items
        foreach ($g_menu[$atkmenutop] as $menuitem) {
            $menu .= $this->getMenuItem($menuitem, "    ");
        }
        // add logout link and view profile link
        $user = atkGetUser();
        $userLink = $usersLink = '';
        $userNode = '';
        if ($theme->getAttribute('sm_atk_user_node')) {
            $userNode = atkGetNode($theme->getAttribute('sm_atk_user_node'));
        } else if (atkconfig('auth_usernode')) {
            $userNode = atkGetNode(atkconfig('auth_usernode'));
        }
        if (atkGetUserId()) {
            if ($userNode) {
                $username = $userNode->descriptor($user);
                if ($userNode->allowed('view')) {
                    $userLink = dispatch_url($userNode, 'view', ['atkselector' => $userNode->primaryKey($user)]);
                }
            } else {
                $username = $user['name'];
            }
        } else {
            // administrator
            $username = $user['name'];
            $userLink = '#';
        }
        if ($userNode && $userNode->allowed('admin')) {
            $usersLink = dispatch_url($userNode, 'admin');
        }

        $menu .= "<li id='logout-link'><a href=\"index.php?atklogout=1\"><i class='fa fa-sign-out fa-lg'></i></a></li>";
        if ($userLink) {
            $menu .= "<li id='user-link'><a href=\"./$userLink\"><i class='fa fa-user fa-lg'></i> $username</a></li>";
        }
        if ($usersLink) {
            $menu .= "<li id='users-link'><a href=\"./$usersLink\"><i class='fa fa-users fa-lg'></i></a></li>";
        }

        $menu .= "</ul>";
        $menu .= $this->getFooter($atkmenutop);
        $menu .= "</div>";
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
        if ($menuitem['name'] == '-')
            return "<li class=\"separator\"><div></div></li>";
        if ($menuitem['url'] && substr($menuitem['url'], 0, 11) == 'javascript:') {
            $href = '<a href="javascript:void(0)" onclick="' . atk_htmlentities($menuitem['url']) . '; return false;">' . atk_htmlentities($this->getMenuTranslation($menuitem['name'], $menuitem['module'])) . '</a>';
        } else if ($menuitem['url']) {
            $href = href($menuitem['url'], $this->getMenuTranslation($menuitem['name'], $menuitem['module']), SESSION_NEW);
        } else
            $href = '<a href="#">' . $name . '</a>';

        return "<li id=\"{$menuitem['module']}.{$menuitem['name']}\" class=\"$submenuname\">" . $href . $delimiter . $submenu . "</li>";
    }
}

?>

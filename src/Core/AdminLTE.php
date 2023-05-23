<?php


namespace Sintattica\Atk\Core;


use Sintattica\Atk\AdminLte\UIStateColors;

class AdminLTE
{

    private const SKINS_NAV_DARK_PRIMARY = 'navbar-primary';
    private const SKINS_NAV_DARK_SECONDARY = 'navbar-secondary';
    private const SKINS_NAV_DARK_INFO = 'navbar-info';
    private const SKINS_NAV_DARK_SUCCESS = 'navbar-success';
    private const SKINS_NAV_DARK_DANGER = 'navbar-danger';
    private const SKINS_NAV_DARK_INDIGO = 'navbar-indigo';
    private const SKINS_NAV_DARK_PURPLE = 'navbar-purple';
    private const SKINS_NAV_DARK_PINK = 'navbar-pink';
    private const SKINS_NAV_DARK_NAVY = 'navbar-navy';
    private const SKINS_NAV_DARK_LIGHTBLUE = 'navbar-lightblue';
    private const SKINS_NAV_DARK_TEAL = 'navbar-teal';
    private const SKINS_NAV_DARK_CYAN = 'navbar-cyan';
    private const SKINS_NAV_DARK_DARK = 'navbar-dark';
    private const SKINS_NAV_DARK_GRAY_DARK = 'navbar-gray-dark';
    private const SKINS_NAV_DARK_GRAY = 'navbar-gray';

    private const SKINS_SIDEBAR_DARK_PRIMARY = 'sidebar-dark-primary';
    private const SKINS_SIDEBAR_DARK_WARNING = 'sidebar-dark-warning';
    private const SKINS_SIDEBAR_DARK_INFO = 'sidebar-dark-info';
    private const SKINS_SIDEBAR_DARK_DANGER = 'sidebar-dark-danger';
    private const SKINS_SIDEBAR_DARK_SUCCESS = 'sidebar-dark-success';
    private const SKINS_SIDEBAR_DARK_INDIGO = 'sidebar-dark-indigo';
    private const SKINS_SIDEBAR_DARK_LIGHT_BLUE = 'sidebar-dark-lightblue';
    private const SKINS_SIDEBAR_DARK_NAVY = 'sidebar-dark-navy';
    private const SKINS_SIDEBAR_DARK_PURPLE = 'sidebar-dark-purple';
    private const SKINS_SIDEBAR_DARK_FUCHSIA = 'sidebar-dark-fuchsia';
    private const SKINS_SIDEBAR_DARK_PINK = 'sidebar-dark-pink';
    private const SKINS_SIDEBAR_DARK_MAROON = 'sidebar-dark-maroon';
    private const SKINS_SIDEBAR_DARK_ORANGE = 'sidebar-dark-orange';
    private const SKINS_SIDEBAR_DARK_LIME = 'sidebar-dark-lime';
    private const SKINS_SIDEBAR_DARK_TEAL = 'sidebar-dark-teal';
    private const SKINS_SIDEBAR_DARK_OLIVE = 'sidebar-dark-olive';
    private const SKINS_SIDEBAR_LIGHT_PRIMARY = 'sidebar-light-primary';
    private const SKINS_SIDEBAR_LIGHT_WARNING = 'sidebar-light-warning';
    private const SKINS_SIDEBAR_LIGHT_INFO = 'sidebar-light-info';
    private const SKINS_SIDEBAR_LIGHT_DANGER = 'sidebar-light-danger';
    private const SKINS_SIDEBAR_LIGHT_SUCCESS = 'sidebar-light-success';
    private const SKINS_SIDEBAR_LIGHT_INDIGO = 'sidebar-light-indigo';
    private const SKINS_SIDEBAR_LIGHT_LIGHT_BLUE = 'sidebar-light-lightblue';
    private const SKINS_SIDEBAR_LIGHT_NAVY = 'sidebar-light-navy';
    private const SKINS_SIDEBAR_LIGHT_PURPLE = 'sidebar-light-purple';
    private const SKINS_SIDEBAR_LIGHT_FUCHSIA = 'sidebar-light-fuchsia';
    private const SKINS_SIDEBAR_LIGHT_PINK = 'sidebar-light-pink';
    private const SKINS_SIDEBAR_LIGHT_MAROON = 'sidebar-light-maroon';
    private const SKINS_SIDEBAR_LIGHT_ORAGE = 'sidebar-light-orange';
    private const SKINS_SIDEBAR_LIGHT_LIME = 'sidebar-light-lime';
    private const SKINS_SIDEBAR_LIGHT_TEAL = 'sidebar-light-teal';
    private const SKINS_SIDEBAR_LIGHT_OLIVE = 'sidebar-light-olive';
    private const SKINS_SIDEBAR_DARK_ATK = 'sidebar-dark-atk';

    private const SKINS_NAV_LIGHT_LIGHT = 'navbar-light';
    private const SKINS_NAV_LIGHT_WARNING = 'navbar-warning';
    private const SKINS_NAV_LIGHT_WHITE = 'navbar-white';
    private const SKINS_NAV_LIGHT_ORANGE = 'navbar-orange';

    private const SIDEBAR_VARIANT_PRIMARY = 'bg-primary';
    private const SIDEBAR_VARIANT_WARNING = 'bg-warning';
    private const SIDEBAR_VARIANT_INFO = 'bg-info';
    private const SIDEBAR_VARIANT_DANGER = 'bg-danger';
    private const SIDEBAR_VARIANT_SUCCESS = 'bg-success';
    private const SIDEBAR_VARIANT_INDIGO = 'bg-indigo';
    private const SIDEBAR_VARIANT_LIGHT_BLUE = 'bg-lightblue';
    private const SIDEBAR_VARIANT_NAVY = 'bg-navy';
    private const SIDEBAR_VARIANT_PURPLE = 'bg-purple';
    private const SIDEBAR_VARIANT_FUCHSIA = 'bg-fuchsia';
    private const SIDEBAR_VARIANT_PINK = 'bg-pink';
    private const SIDEBAR_VARIANT_MAROON = 'bg-maroon';
    private const SIDEBAR_VARIANT_ORANGE = 'bg-orange';
    private const SIDEBAR_VARIANT_LIME = 'bg-lime';
    private const SIDEBAR_VARIANT_TEAL = 'bg-teal';
    private const SIDEBAR_VARIANT_OLIVE = 'bg-olive';

    private const ACCENT_PRIMARY = 'accent-primary';
    private const ACCENT_PRIMARY_WARNING = 'accent-warning';
    private const ACCENT_INFO = 'accent-info';
    private const ACCENT_DANGER = 'accent-danger';
    private const ACCENT_SUCCESS = 'accent-success';
    private const ACCENT_INDIGO = 'accent-indigo';
    private const ACCENT_LIGHT_BLUE = 'accent-lightblue';
    private const ACCENT_NAVY = 'accent-navy';
    private const ACCENT_PURPLE = 'accent-purple';
    private const ACCENT_FUCHSIA = 'accent-fuchsia';
    private const ACCENT_PINK = 'accent-pink';
    private const ACCENT_MAROON = 'accent-maroon';
    private const ACCENT_ORANGE = 'accent-orange';
    private const ACCENT_LIME = 'accent-lime';
    private const ACCENT_TEAL = 'accent-teal';
    private const ACCENT_OLIVE = 'accent-olive';

    private $loginBodyClasses = ['login-page'];

    /**
     * Set the default skin combination for navbar and sidebar
     */
    private $currentSidebarSkinsBundle = [self::SKINS_SIDEBAR_DARK_ATK];

    private $defaultNavBarSkinsBundle = [
        self::SKINS_NAV_DARK_DARK,
        self::SKINS_NAV_DARK_LIGHTBLUE
    ];

    private $devEnvSkinsBundle = [
        self::SKINS_NAV_LIGHT_LIGHT,
        self::SKINS_NAV_LIGHT_WARNING
    ];

    private $currentNavBarSkinsBundle = [];


    /* start config fields */

    //These properties are necessary only if a config item doesn't exist
    //If these properties are set in atk configuration the fields are not needed for anything.

    private $expandSidebarOnHover = true;
    private $indentSidebarChildren = true;
    private $flatNavStyle = false;
    private $compactSidebarStyle = false;
    private $legacyNavStyle = false;
    private $bodySmallText = true;
    private $navSmallText = false;
    private $sidebarSmallText = false;
    private $collapsedSidebar = false;
    private $footerSmallText = false; //Todo: Yet to do this!
    private $brandSmallText = false;
    private $sidebarElevation = 2;
    private $holdTransition = false; //Transit o min-menu mode with a linear transition
    private $fixedNavHeader = true;
    private $menuAccordion = true;

    /* end of config fields */


    private static $adminLTEInstance = null;


    public static function getInstance(): AdminLTE
    {
        if (self::$adminLTEInstance == null) {
            self::$adminLTEInstance = new self();

            //Assign current skin bundle on creation
            self::$adminLTEInstance->currentNavBarSkinsBundle = self::$adminLTEInstance->defaultNavBarSkinsBundle;
            Tools::atkdebug('Created a new AdminLte instance');
        }

        return self::$adminLTEInstance;
    }


    public function getGeneralBodyClasses(): string
    {
        $bodyClasses = "";

        if (Config::getGlobal('menu_hide_sidebar') === true) {
            $bodyClasses .= "layout-top-nav";

        } else {
            $bodyClasses .= "sidebar-mini layout-fixed";

            if ($this->isCollapsedSidebar()) {
                $bodyClasses .= " sidebar-collapse";
            }

            $bodyClasses .= $this->getFixedNavHeaderClass();
        }

        if ($this->isBodySmallText()) {
            $bodyClasses .= " text-sm";
        }

        if ($this->isTransitionHold()) {
            $bodyClasses .= " hold-transition";
        }

        return $bodyClasses;
    }

    public function getLoginClasses(): string
    {
        return implode(' ', $this->loginBodyClasses);
    }

    public function getSidebarClasses(): string
    {

        $classes = implode(' ', $this->currentSidebarSkinsBundle);

        if ($this->isSidebarSmallText()) {
            $classes .= " text-sm";
        }

        if (!$this->isExpandSidebarOnHover()) {
            $classes .= " sidebar-no-expand";
        }


        $classes .= " elevation-" . $this->getSidebarElevation();

        return $classes;
    }


    public function getNavSidebarClasses(): string
    {
        $classes = $this->isIndentSidebarChildren() ? " nav-child-indent" : "";
        $classes .= $this->isLegacyNavStyle() ? " nav-legacy" : "";
        $classes .= $this->isCompactSidebarStyle() ? " nav-compact" : "";
        $classes .= $this->isFlatNavStyle() ? " nav-flat" : "";

        return $classes;
    }

    public function getMainHeaderClasses(): string
    {

        $classes = implode(" ", $this->currentNavBarSkinsBundle);

        if ($this->isNavSmallText()) {
            $classes .= " text-sm";
        }

        return $classes;
    }

    public function getFixedNavHeaderClass(): string
    {
        return $this->isFixedNavHeader() ? " layout-navbar-fixed" : "";
    }

    public function setNavBarSkinBundle(array $skinBundle)
    {
        $this->currentNavBarSkinsBundle = $skinBundle;
    }

    public function setNavBarDevMode($enable)
    {
        $this->currentNavBarSkinsBundle = $enable && Config::getGlobal('enableDevModeNavbar', true)
            ? $this->devEnvSkinsBundle
            : $this->defaultNavBarSkinsBundle;

    }


    public function getFooterClasses(): string
    {
        return $this->isFooterSmallText() ? 'text-sm' : "";
    }


    public function getBrandTextStyle(): string
    {
        return $this->isBrandSmallText() ? 'text-sm' : "";
    }


    public function getBoxTemplate(string $icon, UIStateColors $stateColors)
    {

    }

    public function getSidebarIconsSize(): string
    {
        return $this->shouldFixSmallSidebarIcons() ? 'small-txt' : '';
    }

    private function shouldFixSmallSidebarIcons(): bool
    {
        return $this->isSidebarSmallText() || $this->isBodySmallText();
    }


    /**
     * @return bool
     */
    public function isFixedNavHeader(): bool
    {
        return Config::getGlobal('fixedNavHeader', $this->fixedNavHeader);
    }

    /**
     * @return bool
     */
    public function isBodySmallText(): bool
    {
        return Config::getGlobal('bodySmallText', $this->bodySmallText);
    }

    /**
     * @return bool
     */
    public function isSidebarSmallText(): bool
    {
        return Config::getGlobal('sidebarSmallText', $this->sidebarSmallText);
    }


    public function isTransitionHold(): bool
    {
        return Config::getGlobal('holdTransition', $this->holdTransition);
    }

    /**
     * @return bool
     */
    public function isNavSmallText(): bool
    {
        return Config::getGlobal('navSmallText', $this->navSmallText);
    }

    /**
     * @return bool
     */
    public function isFooterSmallText(): bool
    {
        return Config::getGlobal('footerSmallText', $this->footerSmallText);
    }

    /**
     * @return bool
     */
    public function isIndentSidebarChildren(): bool
    {
        return Config::getGlobal('indentSidebarChildren', $this->indentSidebarChildren);
    }

    /**
     * @return bool
     */
    public function isLegacyNavStyle(): bool
    {
        return Config::getGlobal('legacyNavStyle', $this->legacyNavStyle);
    }

    /**
     * @return bool
     */
    public function isCompactSidebarStyle(): bool
    {
        return Config::getGlobal('compactSidebarStyle', $this->compactSidebarStyle);
    }

    /**
     * @return bool
     */
    public function isFlatNavStyle(): bool
    {
        return Config::getGlobal('flatNavStyle', $this->flatNavStyle);
    }

    /**
     * @return bool
     */
    public function isBrandSmallText(): bool
    {
        return Config::getGlobal('brandSmallText', $this->brandSmallText);
    }

    /**
     * @return bool
     */
    public function isCollapsedSidebar(): bool
    {
        return Config::getGlobal('collapsedSidebar', $this->collapsedSidebar);
    }


    public function isMenuAccordion(): bool
    {
        return Config::getGlobal('menuAccordion', $this->menuAccordion);
    }

    /**
     * @return bool
     */
    public function isExpandSidebarOnHover(): bool
    {
        return Config::getGlobal('expandSidebarOnHover', $this->expandSidebarOnHover);
    }

    /**
     * @return string
     */
    public function getSidebarElevation(): string
    {
        return (string)Config::getGlobal('sidebarElevation', $this->sidebarElevation);
    }


}

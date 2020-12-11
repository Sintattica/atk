<?php


namespace Sintattica\Atk\Core;


use Sintattica\Atk\AdminLte\UIStateColors;
use Sintattica\Atk\Ui\Page;

class AdminLTE
{


    private array $loginBodyClasses;
    private array $generalBodyClasses;

    private string $expandSidebarOnHover;
    private string $indendSidebarChildren;
    private bool $compactSidebar;
    private bool $flatNavStyle;
    private bool $legacyNavStyle;
    private bool $smallerFont;


    private static $adminLTEInstance;

    private $atk;
    private $page;


    /**
     * AdminLTE Singleton constructor.
     * The class configures only one time adminLte
     * for the current webApp
     * @param Atk $atk
     * @param Page $page
     */
    private function __construct()
    {
        if (!self::$adminLTEInstance) {
            //$this->atk = Atk::getInstance();
          //  $this->page = Page::getInstance();
            self::$adminLTEInstance = $this;
        }

    }

    public static function getInstance()
    {
        return self::$adminLTEInstance ?: new self();
    }


    public function getBoxTemplate(string $icon, UIStateColors $stateColors)
    {

    }

}

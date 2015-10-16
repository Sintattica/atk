<?php namespace Sintattica\Atk\Menu;

use Sintattica\Atk\Core\Controller;
use Sintattica\Atk\Core\Tools;
use Sintattica\Atk\Ui\Theme;
use Sintattica\Atk\Core\Config;
use Sintattica\Atk\Core\Module;

/**
 * Some defines
 */
define("MENU_TOP", 1);
define("MENU_LEFT", 2);
define("MENU_BOTTOM", 3);
define("MENU_RIGHT", 4);
define("MENU_SCROLLABLE", 1);
define("MENU_UNSCROLLABLE", 2);
define("MENU_MULTILEVEL", 1); //More then 2 levels supported
define("MENU_NOMULTILEVEL", 2);

/**
 * Menu utility class.
 *
 * This class is used to retrieve the instance of an atkMenuInterface-based
 * class, as defined in the configuration file.
 *
 * @author Ber Dohmen <ber@ibuildings.nl>
 * @author Sandy Pleyte <sandy@ibuildings.nl>
 * @package atk
 * @subpackage menu
 */
class Menu
{

    /**
     * Convert the layout name to a classname
     *
     * @param string $layout The layout name
     * @return string The classname
     */
    function layoutToClass($layout)
    {
        // Check if the menu config is one of the default atk menus (deprecated)
        if (in_array($layout, array("plain", "frames", "outlook", "dhtml", "modern", "cook", "dropdown"))) {
            $classname = "atk.menu.atk" . $layout . "menu";
        } // Application root menu directory (deprecated)
        elseif (strpos($layout, '.') === false) {
            $classname = "menu." . $layout;
        } // Full class name with packages.
        else {
            $classname = $layout;
        }
        return $classname;
    }


    /**
     * Get new menu object
     *
     * @return Menu class object
     */
    public static function &getMenu()
    {
        static $s_instance = null;
        if ($s_instance == null) {
            Tools::atkdebug("Creating a new menu instance");
            $s_instance = new PlainMenu();

            // Set the dispatchfile for this menu based on the theme setting, or to the default if not set.
            // This makes sure that all calls to dispatch_url will generate a url for the main frame and not
            // within the menu itself.
            $theme = Theme::getInstance();
            $dispatcher = $theme->getAttribute('dispatcher',
                Config::getGlobal("dispatcher", "dispatch.php")); // do not use atkSelf here!
            $c = Controller::getInstance();
            $c->setPhpFile($dispatcher);

            Module::atkHarvestModules("getMenuItems");
        }

        return $s_instance;
    }

}



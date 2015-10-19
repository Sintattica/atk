<?php namespace Sintattica\Atk\Menu;

use Sintattica\Atk\Core\Controller;
use Sintattica\Atk\Core\Tools;
use Sintattica\Atk\Ui\Theme;
use Sintattica\Atk\Core\Config;
use Sintattica\Atk\Core\Module;

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
     * Get new menu object
     *
     * @return MenuInterface class object
     */
    public static function &getMenu()
    {
        static $s_instance = null;
        if ($s_instance == null) {
            $theme = Theme::getInstance();
            Tools::atkdebug("Creating a new menu instance");
            $class = $theme->getAttribute('menu_class', Config::getGlobal('menu_class'));
            $s_instance = new $class();

            // Set the dispatchfile for this menu based on the theme setting, or to the default if not set.
            // This makes sure that all calls to dispatch_url will generate a url for the main frame and not
            // within the menu itself.

            $dispatcher = $theme->getAttribute('dispatcher',
                Config::getGlobal("dispatcher", "index.php")); // do not use atkSelf here!
            $c = Controller::getInstance();
            $c->setPhpFile($dispatcher);

            Module::atkHarvestModules("getMenuItems");
        }

        return $s_instance;
    }
}



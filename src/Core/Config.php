<?php namespace Sintattica\Atk\Core;

use Sintattica\Atk\Session\SessionManager;

/**
 * Config class for loading config files and retrieving config options.
 * Also contains misc. methods for use in config files.
 *
 * @author Ivo Jansch <ivo@achievo.org>
 * @package atk
 */
class Config
{

    /**
     * Load global configuration variables.
     */
    public static function loadGlobals()
    {
        $overrides = array();


        // Put all "config_" globals as variables in the function scope
        foreach ($GLOBALS as $key => $value) {
            if (substr($key, 0, 7) === 'config_') {

                $$key = $value;

                // Store the current value, so that we can restore it later. Since our includes below here, depend on
                // some of these variables, we can't simply change the ordering of this process. Also we can't use
                // $GLOBALS, due to the use of the global keyword later on.
                $overrides[$key] = $GLOBALS[$key];
            }
        }

        // Include the defaults
        require_once "defaultconfig.inc.php";

        // Get the application config, this is leading and will override all previously defined configuration values.
        $applicationConfig = self::getApplicationConfig();

        // Merge everything we got, including variables defined in our application config and configuration defined
        // prior to constructing atkConfig
        $allVars = array_merge(
            get_defined_vars(), $applicationConfig
        );

        // Get all defined config variables, make then global and update their value
        foreach ($allVars as $key => $value) {
            if (substr($key, 0, 7) === 'config_') {
                // Reference the variable to the global scope
                global $$key;

                // If a key was previously defined, use that instead of the default value.
                if (array_key_exists($key, $applicationConfig)) {
                    $$key = $applicationConfig[$key];
                } else {
                    if (array_key_exists($key, $overrides)) {
                        $$key = $overrides[$key];
                    } else {
                        $$key = $value;
                    }
                }
            }
        }
    }

    /**
     * Get the application configuration.
     *
     * @static
     * @param  string $path The path to 'config.inc.php in the application directory
     * @return array
     */
    static public function getApplicationConfig()
    {
        global $config_application_config;
        require_once $config_application_config;
        return get_defined_vars();
    }

    /**
     * Returns the value for a global configuration variable.
     *
     * @param string $name configuration variable name (without the config_ prefix)
     * @param mixed $default default (fallback) value
     * @return mixed config value
     */
    public static function getGlobal($name, $default = null)
    {
        $fullName = 'config_' . $name;
        return isset($GLOBALS[$fullName]) ? $GLOBALS[$fullName] : $default;
    }

    /**
     * Sets the value of a global configuration variable.
     *
     * Only works for configuration variables where no function for exists.
     *
     * @param string $name configuration variable name (without the config_ prefix)
     * @param mixed $value new value
     */
    public static function setGlobal($name, $value)
    {
        $GLOBALS['config_' . $name] = $value;
    }

    /**
     * Get a configuration value for a section (typically a module)
     *
     * Can be overridden with a global function config_$section_$tag.
     * Relies on your configurations being in configs/ (or wherever $config_configdir says).
     * Also gets $section.*.inc.php.
     * If the section is a module and has a skel/configs/ it will get those configs too
     * and use them as defaults.
     *
     * <b>Example:</b>
     *        Config::get('color','mymodule','FF0000');
     *
     * @param string $section Section to check (typically a module)
     * @param string $tag Name of configuration to get
     * @param mixed $default Default to use if configuration value does not exist
     * @return mixed Configuration value
     */
    public static function get($section, $tag, $default = "")
    {
        static $s_configs = array();

        $fn = 'config_' . $section . '_' . $tag;
        if (function_exists($fn)) {
            return $fn();
        }

        if (!isset($s_configs[$section])) {
            $config = self::getConfigForSection($section);
            if (!is_array($config)) {
                $config = array();
            }
            $s_configs[$section] = $config;
        }

        if (isset($s_configs[$section][$tag]) && $s_configs[$section][$tag] !== "") {
            return $s_configs[$section][$tag];
        } else {
            return $default;
        }
    }

    /**
     * Get the configuration values for a section and if the section
     * turns out to be a module, try to get the module configs
     * and merge them as fallbacks.
     *
     * @param string $section Name of the section to get configs for
     * @return array Configuration values
     */
    public static function getConfigForSection($section)
    {
        $config = self::getDirConfigForSection(Config::getGlobal('configdir'), $section);
        if (Module::moduleExists($section)) {
            $dir = Module::moduleDir($section) . 'skel/configs/';
            if (file_exists($dir)) {
                $module_configs = self::getDirConfigForSection($dir, $section);
                $config = array_merge($module_configs, $config);
            }
        }
        return $config;
    }

    /**
     * Get all configuration values from all configuration files for
     * a specific directory and a specific section.
     *
     * @param string $dir Directory where the configuration files are
     * @param string $section Section to get configuration values for
     * @return array Configuration values
     */
    protected static function getDirConfigForSection($dir, $section)
    {
        Tools::atkdebug("Loading config file for section $section");
        $config = array();
        @include($dir . $section . '.php');

        $other = glob(Config::getGlobal("configdir") . "{$section}.*.php");
        if (is_array($other)) {
            foreach ($other as $file) {
                include($file);
            }
        }
        return $config;
    }

    /**
     * Is debugging enabled for client IP?
     *
     * @param array $params
     * @return bool
     */
    function ipDebugEnabled($params)
    {
        $ip = Tools::atkGetClientIp();
        return in_array($ip, $params["list"]);
    }

    /**
     * Is debugging enabled by special request variable?
     *
     * @param array $params
     * @return bool
     */
    function requestDebugEnabled($params)
    {
        $session = &SessionManager::getSession();

        if (isset($_REQUEST["atkdebug"]["key"])) {
            $session["debug"]["key"] = $_REQUEST["atkdebug"]["key"];
        } else {
            if (isset($_COOKIE['ATKDEBUG_KEY']) && !empty($_COOKIE['ATKDEBUG_KEY'])) {
                $session["debug"]["key"] = $_COOKIE['ATKDEBUG_KEY'];
            }
        }

        return (isset($session["debug"]["key"]) && $session["debug"]["key"] == $params["key"]);
    }

    /**
     * Returns a debug level based on the given options for
     * dynamically checking/setting the debug level. If nothing
     * found returns the default level.
     *
     * @param int $default The default debug level
     * @param array $options
     * @return int
     */
    static public function smartDebugLevel($default, $options = array())
    {
        $session = &SessionManager::getSession();

        $enabled = $default > 0;

        foreach ($options as $option) {
            $method = $option["type"] . "DebugEnabled";
            if (is_callable(array("atkconfig", $method))) {
                $enabled = $enabled || config::$method($option);
            }
        }

        global $config_debug_enabled;
        $config_debug_enabled = $enabled;

        if ($enabled) {
            if (isset($_REQUEST["atkdebug"]["level"])) {
                $session["debug"]["level"] = $_REQUEST["atkdebug"]["level"];
            } else {
                if (isset($_COOKIE['ATKTools::DEBUG_LEVEL'])) {
                    $session["debug"]["level"] = $_COOKIE['ATKTools::DEBUG_LEVEL'];
                }
            }

            if (isset($session["debug"]["level"])) {
                return $session["debug"]["level"];
            } else {
                return max($default, 0);
            }
        }

        return $default;
    }

    /**
     * Restrict access to an attribute to a certain entity (group or level)
     *
     * When $config_authorization is set to "config", this method can be used
     * to restrict access to certain attributes for a given entity.
     * This means that certain users can not edit or even view some attributes
     * in a node. This is called "attribute level security".
     *
     * If this method is called on a node/attrib combination, only those users
     * who match the level/group can view/edit the attribute. If no calls are
     * made for an attribute, the attribute is considered unrestricted and every
     * user has access.
     *
     * @param string $node The node on which access is restricted.
     * @param string $attrib The name of the attribute that is to be restricted.
     * @param string $mode The action to restrict ("edit" or "view")
     * @param mixed $entity The level/group that has access to the attribute.
     */
    function attribRestrict($node, $attrib, $mode, $entity)
    {
        $GLOBALS["config_attribrestrict"][$node][$attrib][$mode] = $entity;
    }

    /**
     * Grants acces to an entity (group or level)
     *
     * When $config_authorization is set to "config", this method can be used
     * in the configfile to grant privileges.
     *
     * @param string $node The node on which to grant a privilege.
     * @param string $action The action (privilege) that is granted.
     * @param mixed $entity The entity (securitylevel or group) to which the
     *                      privilege is granted.
     */
    function grant($node, $action, $entity)
    {
        $GLOBALS["config_access"][$node][] = Array($action => $entity);
    }

    /**
     * Translate pop3 server responses to user readable error messages.
     *
     * This function is only of use when using pop3 as authentication method.
     * Some pop3 servers give specific error messages that may be of interest
     * to the user. If you use this function (in the config file) and atk
     * encounters the specified substring in a server response, the specified
     * message is displayed.
     *
     * @param string $substring The substring to look for in the server
     *                          response.
     * @param string $message The message to display to the user upon encounter
     *                        of the substring.
     */
    function addPop3Response($substring, $message)
    {
        global $g_pop3_responses;
        $g_pop3_responses[$substring] = $message;
    }

    /**
     * Create a new user.
     *
     * When $config_authentication is set to "config", this method can be used
     * in the configfile to create users. Mind you that anybody who has read
     * access on the config file, can read the passwords. It is advisable to
     * use a more secure authentication method like "db" or "pop3".
     *
     * @param string $name The login name.
     * @param string $password The password of the user.
     * @param mixed $securitylevel The securitylevel or group of the user.
     *                             Permissions are granted on level/group basis,
     *                             depending on the setting of
     *                             $config_security_scheme
     */
    public static function addUser($name, $password, $securitylevel = 0)
    {
        $GLOBALS["config_user"][$name] = Array("password" => $password, "level" => $securitylevel);
    }
}


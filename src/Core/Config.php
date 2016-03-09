<?php namespace Sintattica\Atk\Core;

/**
 * Config class for loading config files and retrieving config options.
 */
class Config
{

    static $s_globals = [];

    /**
     * Load global configuration variables.
     */
    public static function init()
    {

        // Include the defaults
        $defaultConfig = self::getConfigValues(__DIR__ . '/../Resources/config/atk.php');
        foreach ($defaultConfig as $key => $value) {
            self::$s_globals[$key] = $value;
        }

        // Get the application config
        $applicationConfig = self::getConfigValues(self::$s_globals['application_config']);
        if (is_array($applicationConfig) && count($applicationConfig)) {
            self::$s_globals = Tools::atk_array_merge_recursive(self::$s_globals, $applicationConfig);
        }
    }

    public static function env($key, $default = false) {
        $value = getenv($key);
        if($value === false && $default){
            return $default;
        }
        return $value;
    }

    public static function getConfigValues($file) {
        if(is_file($file)){
            $values = include($file);
            if(is_array($values)){
                return $values;
            }
        }
        return [];
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
        return isset(self::$s_globals[$name]) ? self::$s_globals[$name] : $default;
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
        self::$s_globals[$name] = $value;
    }

    /**
     * Get a configuration value for a section (typically a module)
     *
     * If the section is a module and has a config/config.php it will get those configs too
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
        static $s_configs = [];

        if (!isset($s_configs[$section])) {
            $config = self::getConfigForSection($section);
            if (!is_array($config)) {
                $config = array();
            }
            $s_configs[$section] = $config;
        }

        if (!isset($s_configs[$section])) {
            $config = self::getConfigValues(self::getGlobal('application_config_dir') . $section . '.php');
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
        $config = self::getConfigValues(self::getGlobal('application_config_dir') . $section . '.php');

        $app = Atk::getInstance();
        if ($app->isModule($section)) {
            $dir = $app->moduleDir($section) . self::getGlobal('configdirname') . '/';
            if (is_dir($dir)) {
                $module_configs = self::getConfigValues($dir . $section . '.php');
                $config = array_merge($module_configs, $config);
            }
        }
        return $config;
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
        self::$s_globals["attribrestrict"][$node][$attrib][$mode] = $entity;
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
        self::$s_globals["access"][$node][] = Array($action => $entity);
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
        self::$s_globals["user"][$name] = Array("password" => $password, "level" => $securitylevel);
    }
}

<?php

namespace Sintattica\Atk\Core;

use Sintattica\Atk\Security\SecurityManager;
use Sintattica\Atk\Session\SessionManager;

/**
 * Class that handles user interface internationalization.
 *
 * This class is used to retrieve the proper translations for any string
 * displayed in the user interface. It includes only those language files
 * that are actually used, and has several fallback systems to find
 * translations if they can be found in the correct module.
 *
 * @author Boy Baukema <boy@ibuildings.nl>
 */
class Language
{
    private static ?Language $s_instance = null;

    private static ?array $s_supportedLanguages = null;

    /*
     * Directory where language files are stored.
     * @access private
     * @var String
     */
    public $LANGDIR = 'languages/';

    /*
     * Contains all currently loaded language strings.
     * @access private
     * @var array
     */
    public $m_cachedlang = [];

    /*
     * List of currently loaded language files
     * @access private
     * @var array
     */
    public $m_cachedlangfiles = [];

    /*
     * List of fallback modules
     * @access private
     * @var array
     */
    public $m_fallbackmodules = [];

    /*
     * List of override modules
     * @access private
     * @var array
     */
    public $m_overridemodules = ['langoverrides'];

    /*
     * List of custum language string overrides
     * @access private
     * @var array
     */
    public $m_customStrings = [];

    /**
     * Default Constructor.
     */
    public function __construct()
    {
        Tools::atkdebug('New instance made of atkLanguage');
    }

    /**
     * Gets an instance of the Language class.
     *
     * Using this function will ensure that only 1 instance ever exists
     * (singleton).
     *
     * @return Language|null Instance of the atkLanguage class
     */
    public static function getInstance(): ?Language
    {
        if (self::$s_instance == null) {
            self::$s_instance = new self();
        }

        return self::$s_instance;
    }

    /**
     * Add a module that serves as an override for language strings.
     *
     * @param string $module Name of the module to add.
     */
    public function addOverrideModule(string $module): void
    {
        array_unshift($this->m_overridemodules, $module);
    }

    /**
     * Add a module that servers as a fallback for language strings.
     *
     * @param string $module Name of the module to add.
     */
    public function addFallbackModule(string $module): void
    {
        $this->m_fallbackmodules[] = $module;
    }

    /**
     * Calculate the list of fallbackmodules.
     *
     * @param bool $modulefallback Wether or not to use all the modules of the application in the fallback,
     *                             when looking for strings
     * @return array Array of fallback modules
     */
    protected function _getFallbackModules(bool $modulefallback): array
    {
        static $s_fallbackmodules = [];
        $key = $modulefallback ? 1 : 0; // we can be called with true or false, cache both results

        if (!array_key_exists($key, $s_fallbackmodules)) {
            $modules = [];

            if ($modulefallback || Config::getGlobal('language_modulefallback', false)) {
                $atk = Atk::getInstance();
                foreach ($atk->g_modules as $modname => $modpath) {
                    $modules[] = $modname;
                }
            }

            $modules[] = 'atk';

            $s_fallbackmodules[$key] = array_merge($this->m_fallbackmodules, $modules);
        }

        return $s_fallbackmodules[$key];
    }

    /**
     * Text function, retrieves a translation for a certain string.
     *
     * @static
     *
     * @param array|string $string string or array of strings containing the name(s) of the string to return
     *                               when an array of strings is passed, the second will be the fallback if
     *                               the first one isn't found, and so forth
     * @param string|null $module module in which the language file should be looked for,
     *                               defaults to core module with fallback to ATK
     * @param string|null $node the node to which the string belongs
     * @param string|null $lng ISO 639-1 language code, defaults to config variable
     * @param string|null $firstfallback the first module to check as part of the fallback
     * @param bool $nodefaulttext if true, then it doesn't return false when it can't find a translation
     * @param bool $modulefallback Wether or not to use all the modules of the application in the fallback,
     *                               when looking for strings
     *
     * @return string the string from the languagefile
     */
    public static function text(
        array|string $string,
        ?string       $module,
        ?string       $node = '',
        ?string       $lng = '',
        ?string       $firstfallback = '',
        ?bool         $nodefaulttext = false,
        ?bool         $modulefallback = false
    ): string
    {
        // We don't translate nothing
        if ($string == '') {
            return '';
        }
        if ($lng == '') {
            $lng = self::getLanguage();
        }
        $lng = strtolower($lng);
        $atklanguage = self::getInstance();

        // If only one string given, process it immediatly
        if (!is_array($string)) {
            return $atklanguage->_getString($string, $module, $lng, $node, $nodefaulttext, $firstfallback, $modulefallback);
        }

        // If multiple strings given, iterate through all strings and return the translation if found
        for ($i = 0, $_i = Tools::count($string); $i < $_i; ++$i) {
            // Try to get the translation
            $keyCurr = $string[$i] ?? null;
            if ($keyCurr) {
                $translation = $atklanguage->_getString($string[$i], $module, $lng, $node, $nodefaulttext || ($i < ($_i - 1)), $firstfallback, $modulefallback);
                if ($translation != '') {
                    return $translation;
                }
            }
        }

        return '';
    }

    /**
     * Returns all strings for the given modulename.
     *
     * The returned struct will contain key-value pairs for the translation
     * keys, and their respective translation.
     *
     * @param string $module Module in which the language file should be
     * @param string $lng ISO 639-1 language code, defaults to config
     * @return array Translations
     */
    public static function getStringsForModule(string $module, string $lng = ''): array
    {
        if ($lng == '') {
            $lng = self::getLanguage();
        }
        $atklanguage = self::getInstance();

        $atklanguage->_includeLanguage($module, $lng);

        if (isset($atklanguage->m_cachedlang[$module]) && is_array($atklanguage->m_cachedlang[$module][$lng])) {
            return $atklanguage->m_cachedlang[$module][$lng];
        }

        return [];
    }

    /**
     * Get the current language, either from url, or if that's not present, from what the user has set.
     *
     * @static
     *
     * @return string current language.
     */
    public static function getLanguage(): string
    {
        global $ATK_VARS;

        if (isset($ATK_VARS['atklng']) && in_array($ATK_VARS['atklng'], self::getSupportedLanguages())) {
            $lng = $ATK_VARS['atklng'];
        } else {
            $lng = self::getUserLanguage();
        }

        return strtolower($lng);
    }

    /**
     * Change the current language.
     * Note that his only remains set for the current request, it's not
     * session based.
     *
     * @static
     *
     * @param string $lng The language to set
     */
    public static function setLanguage(string $lng): void
    {
        global $ATK_VARS;
        $ATK_VARS['atklng'] = $lng;
    }

    /**
     * Get the selected language of the current user if he/she set one,
     * otherwise we try to get it from the browser settings and if even THAT
     * fails, we return the default language.
     *
     * @static
     *
     * @return string
     */
    public static function getUserLanguage(): string
    {
        $supported = self::getSupportedLanguages();
        $sessionmanager = SessionManager::getInstance();
        if (!empty($sessionmanager)) {
            $userinfo = SecurityManager::atkGetUser();
            $fieldname = Config::getGlobal('auth_languagefield');
            if (isset($userinfo[$fieldname]) && in_array($userinfo[$fieldname], $supported)) {
                return $userinfo[$fieldname];
            }
        }

        // Otherwise we check the headers
        if (Config::getGlobal('use_browser_language', false)) {
            $headerlng = self::getLanguageFromHeaders();
            if ($headerlng && in_array($headerlng, $supported)) {
                return $headerlng;
            }
        }

        // We give up and just return the default language
        return Config::getGlobal('language');
    }

    /**
     * Get the primary languagecode that the user has set in his/her browser.
     *
     * @static
     *
     * @return string|null The languagecode
     */
    public static function getLanguageFromHeaders(): ?string
    {
        $autolng = null;
        if (isset($_SERVER['HTTP_ACCEPT_LANGUAGE'])) {
            $langs = preg_split('/[,;]/', $_SERVER['HTTP_ACCEPT_LANGUAGE']);
            if ($langs[0] != '') {
                $elems = explode('-', $langs[0]); // lng might contain a subset after the dash.
                $autolng = $elems[0];
            }
        }

        return $autolng;
    }

    /**
     * Explicitly sets the supported languages.
     *
     * @param array $languages supported languages
     */
    public static function setSupportedLanguages(array $languages): void
    {
        self::$s_supportedLanguages = $languages;
    }

    /**
     * Get the languages supported by the application.
     *
     * @return array|null An array with the languages supported by the application.
     */
    public static function getSupportedLanguages(): ?array
    {
        if (self::$s_supportedLanguages === null) {
            self::$s_supportedLanguages = Config::getGlobal('supported_languages');
        }

        return self::$s_supportedLanguages;
    }

    /**
     * Determine the list of modules we need to go through to check
     * language strings.
     *
     * @param string|null $module manually passed module
     * @param string|null $firstfallback an additional module in which the
     *                               translation will be searched first, if not found in the
     *                               module itself.
     * @param bool $modulefallback If true, *all* modules are checked.
     * @return array List of modules to use to find the translations
     */
    protected function _getModules(?string $module, ?string $firstfallback = '', ?bool $modulefallback = false): array
    {
        $arr = [];
        if ($module) {
            $arr[] = $module;
        }
        if ($firstfallback != '') {
            $arr[] = $firstfallback;
        }
        if ($module == 'atk') {
            // overrides have precedence, then the passed module (finally the fallbacks)
            $modules = array_merge($this->m_overridemodules, $arr, $this->_getFallbackModules($modulefallback));
        } else {
            // the passed module has precedence, then the overrides (finally the fallbacks)
            $modules = array_merge($arr, $this->m_overridemodules, $this->_getFallbackModules($modulefallback));
        }

        return $modules;
    }

    /**
     * This function takes care of the fallbacks when retrieving a string ids.
     * It is as following:
     * First we check for a string specific to both the module and the node
     * (module_node_key).
     * If that isn't found we check for a node specific string (node_key).
     * And if all that fails we look for a general string in the module.
     *
     *
     * @param string|null $key the name of the string to return
     * @param string|null $module module in which the language file should be looked for,
     *                               defaults to core module with fallback to ATK
     * @param string|null $lng ISO 639-1 language code, defaults to config variable
     * @param string|null $node the node to which the string belongs
     * @param bool $nodefaulttext wether or not to pass a default text back
     * @param string|null $firstfallback the first module to check as part of the fallback
     * @param bool $modulefallback Wether or not to use all the modules of the application in the fallback,
     *                               when looking for strings
     *
     * @return string the name with which to call the string we want from the languagefile
     */
    protected function _getString(
        ?string $key,
        ?string $module,
        ?string $lng,
        ?string $node = '',
        ?bool   $nodefaulttext = false,
        ?string $firstfallback = '',
        ?bool $modulefallback = false
    ): string
    {
        // First find node specific string.
        $modules = $this->_getModules($module, $firstfallback, $modulefallback);

        // Second check custom Strings
        if (isset($this->m_customStrings[$lng]) && isset($this->m_customStrings[$lng][$key])) {
            return $this->m_customStrings[$lng][$key];
        }

        if ($node != '') {
            foreach ($modules as $modname) {
                $text = $this->_getStringFromFile($module.'_'.$node.'_'.$key, $modname, $lng);
                if ($text != '') {
                    return $text;
                }
            }

            foreach ($modules as $modname) {
                $text = $this->_getStringFromFile($node.'_'.$key, $modname, $lng);
                if ($text != '') {
                    return $text;
                }
            }
        }

        // find generic module string
        foreach ($modules as $modname) {
            $text = $this->_getStringFromFile($key, $modname, $lng);
            if ($text != '') {
                return $text;
            }
        }

        if (!$nodefaulttext) {
            if (Config::getGlobal('debug_translations', false)) {
                Tools::atkdebug("atkLanguage: translation for '$key' with module: '$module' and node: '$node' and language: '$lng' not found, returning default text");
            }

            // Still nothing found. return default string
            return $this->defaultText($key);
        }

        return '';
    }

    /**
     * Checks wether the language is set or not.
     *
     * If set, it does nothing and return true
     * otherwise it sets it
     *
     * @param string $module the module to import the language file from
     * @param string $lng language of file to import
     * @return bool true if everything went okay
     */
    protected function _includeLanguage(string $module, string $lng): bool
    {
        if (!isset($this->m_cachedlangfiles[$module][$lng]) || $this->m_cachedlangfiles[$module][$lng] != 1) {
            $this->m_cachedlangfiles[$module][$lng] = 1;
            $path = $this->getLanguageDirForModule($module);

            $file = $path.$lng.'.php';

            if (file_exists($file)) {
                $this->m_cachedlang[$module][$lng] = $this->getLanguageValues($file);

                return true;
            }

            return false;
        }

        return true;
    }

    protected function getLanguageValues($file): array
    {
        if (is_file($file)) {
            $values = include $file;
            if (is_array($values)) {
                return $values;
            }
        }

        return [];
    }

    /**
     * Method for getting the relative path to the languagedirectory
     * of a module.
     * Supports 2 special modules:
     * - atk (returns the path of the atk languagedir)
     * - langoverrides (returns the path of the languageoverrides dir).
     *
     * Special method in that it can run both in static and non-static
     * mode.
     *
     * @param string $moduleName The module to get the languagedir for
     * @return string The relative path to the languagedir
     */
    public function getLanguageDirForModule(string $moduleName): string
    {
        if ($moduleName == 'atk') {
            $path = __DIR__.'/../Resources/'.$this->LANGDIR;
        } else {
            if ($moduleName == 'langoverrides') {
                $path = Config::getGlobal('language_basedir', $this->LANGDIR);
            } else {
                $atk = Atk::getInstance();
                $path = $atk->moduleDir($moduleName).$this->LANGDIR;
            }
        }

        return $path;
    }

    /**
     * A function to change the original "$something_text" string to
     * "Something text"
     * This is only used when we really can't find the "$something_text" anywhere.
     *
     * @param string $string the name of the string to return
     * @return string the changed string
     */
    public function defaultText(string $string): string
    {
        return ucfirst(str_replace('_', ' ', str_replace('title_', '', $string)));
    }

    /**
     * Gets the string from the languagefile or, if we failed, returns "".
     *
     * @param string $key the name which was given when the text function was called
     * @param string $module the name of the module to which the text function belongs
     * @param string $lng the current language
     *
     * @return string the true name by which the txt is called or "" if we can't find any entry
     */
    protected function _getStringFromFile(string $key, string $module, string $lng): string
    {
        if (isset($key)) {
            $this->_includeLanguage($module, $lng);

            if (isset($this->m_cachedlang[$module]) && is_array($this->m_cachedlang[$module][$lng]) && isset($this->m_cachedlang[$module][$lng][$key])) {
                return $this->m_cachedlang[$module][$lng][$key];
            }
        }

        return '';
    }

    /**
     * Set a custom language string.
     *
     * @param string $code The code of the custom string
     * @param string $text Text
     * @param string $lng Language
     */
    public function setText(string $code, string $text, string $lng): void
    {
        if (!isset($this->m_customStrings[$lng])) {
            $this->m_customStrings[$lng] = [];
        }
        $this->m_customStrings[$lng][$code] = $text;
    }
}

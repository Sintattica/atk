<?php namespace Sintattica\Atk\Ui;

use Sintattica\Atk\Core\Config;
use Sintattica\Atk\Utils\TmpFile;
use Sintattica\Atk\Core\Tools;
use Sintattica\Atk\Core\Module;
use Sintattica\Atk\Utils\DirectoryTraverser;

/**
 * Compiles cache for current theme.
 *
 * The compiler scans the theme directory and file structure and builds a
 * compiled file that contains the exact location of every themeable
 * element.
 *
 * If a theme is derived from another theme, the compiled theme contains the
 * sum of the parts, so a single compiled theme file contains every
 * information that ATK needs about the theme.
 *
 * @author Boy Baukema <boy@ibuildings.nl>
 * @author Ivo Jansch <ivo@achievo.org>
 * @package atk
 * @subpackage ui
 *
 */
class ThemeCompiler
{

    /**
     * Compile a theme file for a certain theme.
     *
     * @param String $name The name of the theme to compile.
     */
    function compile($name)
    {
        // Process theme directory structure into data array.
        $data = $this->readStructure($name);

        // Write it to the compiled theme file
        if (count($data)) {
            if (!file_exists(Config::getGlobal("atktempdir") . "themes/")) {
                mkdir(Config::getGlobal("atktempdir") . "themes/");
            }

            $tmpfile = new TmpFile("themes/$name.php");
            $tmpfile->writeAsPhp("theme", $data);
            return true;
        }
        return false;
    }

    /**
     * Parse theme structure.
     *
     * This method parses the themes directory and file structure and
     * converts it to a dataset containing all theme attributes and the
     * exact location of all themable files.
     *
     * @param String $name The name of the theme
     * @param String $location The location of the theme ("atk", "app" or "auto")
     * @return array Theme dData structure
     */
    function readStructure($name, $location = "auto")
    {
        $data = array();

        $path = $this->findTheme($name, $location);

        $abspath = Theme::absPath($path, $location);

        // First parse the themedef file for attributes
        if ($path != "" && file_exists($abspath . "themedef.php")) {
            include($abspath . "themedef.php");

            if (isset($theme)) {
                foreach ($theme as $key => $value) {
                    $data["attributes"][$key] = $value;
                }
            }

            // Second scan all files in the theme path
            $this->scanThemePath($path, $abspath, $data);
            $this->scanModulePath($name, $data);

            $data["attributes"]["basepath"] = $path;
        }
        return $data;
    }

    /**
     * Find the location on disk of a theme with a certain name.
     *
     * @param String $name Name of the theme
     * @param String $location The location of the theme ("atk", "app" or "auto")
     *                         If set to auto, the method changes the $location
     *                         value to the actual location.
     * @return String The path relative to atk root, where the theme is located
     */
    function findTheme($name, &$location)
    {
        if (strpos($name, ".") !== false) {
            list ($module, $name) = explode(".", $name);
            $path = Module::moduleDir($module) . "themes/" . $name . "/";
            if (file_exists($path . "themedef.php")) {
                $location = "module";
                return "module/$module/themes/$name/";
            }
        } else {
            if ($location != "atk" && file_exists(Config::getGlobal("application_dir") . "themes/$name/themedef.php")) {
                $location = "app";
                return "themes/$name/";
            } else {
                if ($location != "app" && file_exists(Config::getGlobal("atkroot") . "atk/themes/$name/themedef.php")) {
                    $location = "atk";
                    return "atk/themes/$name/";
                }
            }
        }
        Tools::atkerror("Theme $name not found");
        $location = "";
        return "";
    }

    /**
     * Traverse theme path.
     *
     * Traverses the theme path and remembers the physical location of all theme files.
     *
     * @param String $path The path of the theme, relative to atkroot.
     * @param String $abspath The absolute path of the theme
     * @param String $data Reference to the data array in which to report the file locations
     */
    function scanThemePath($path, $abspath, &$data)
    {
        $traverser = new DirectoryTraverser();
        $subitems = $traverser->getDirContents($abspath);
        foreach ($subitems as $name) {
            if (in_array($name,
                array("images", "styles", "templates"))) { // images, styles and templates are compiled the same
                $files = $this->_dirContents($abspath . $name);
                foreach ($files as $file) {
                    $key = $file;
                    if (substr($key, -8) == '.tpl.php') {
                        $key = substr($key, 0, -4);
                    }

                    $data["files"][$name][$key] = $path . $name . "/" . $file;
                }
            } else {
                if ($name == "icons") { // New ATK5 style icon theme dirs
                    $subs = $this->_dirContents($abspath . $name);
                    foreach ($subs as $type) {
                        $files = $this->_dirContents($abspath . $name . "/" . $type);
                        foreach ($files as $file) {
                            $data["files"]["icons"][$type][$file] = $path . $name . "/" . $type . "/" . $file;
                        }
                    }
                } else {
                    if (in_array($name,
                        array("tree_icons", "recordlist_icons", "toolbar_icons"))) { // Old ATK5 style icon theme dirs
                        $type = substr($name, 0, -6);
                        $files = $this->_dirContents($abspath . $name);
                        foreach ($files as $file) {
                            $data["files"]["icons"][$type][$file] = $path . $name . "/" . $file;
                        }
                    }
                }
            }
        }
    }

    /**
     * Traverse module path.
     *
     * Traverses the module path and remembers the physical location of all theme files.
     *
     * @param String $theme The name of the theme
     * @param String $data Reference to the data array in which to report the file locations
     */
    function scanModulePath($theme, &$data)
    {
        global $g_modules;

        $traverser = new DirectoryTraverser();
        foreach ($g_modules as $module => $modpath) {
            $abspath = $modpath . "themes/" . $theme . "/";

            if (is_dir($abspath)) {
                $subitems = $traverser->getDirContents($abspath);
                foreach ($subitems as $name) {
                    if (in_array($name,
                        array("images", "styles", "templates"))) { // images, styles and templates are compiled the same
                        $files = $this->_dirContents($abspath . $name);
                        foreach ($files as $file) {
                            $data["modulefiles"][$module][$name][$file] = $theme . "/" . $name . "/" . $file;
                        }
                    } else {
                        if ($name == "icons") { // New ATK5 style icon theme dirs
                            $subs = $this->_dirContents($abspath . $name);
                            foreach ($subs as $type) {
                                $files = $this->_dirContents($abspath . $name . "/" . $type);
                                foreach ($files as $file) {
                                    $data["modulefiles"][$module]["icons"][$type][$file] = $theme . "/" . $name . "/" . $type . "/" . $file;
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    /**
     * Get files for a directory
     *
     * @param string $path The directory to traverse
     * @return Array with files from the traversed directory
     */
    function _dirContents($path)
    {
        $traverser = new DirectoryTraverser();
        $traverser->addExclude('/^\.(.*)/'); // ignore everything starting with a '.'
        $traverser->addExclude('/^CVS$/');   // ignore CVS directories
        $files = $traverser->getDirContents($path);
        return $files;
    }

}
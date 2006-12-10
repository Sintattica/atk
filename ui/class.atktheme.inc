<?php
  /**
   * This file is part of the Achievo ATK distribution.
   * Detailed copyright and licensing information can be found
   * in the doc/COPYRIGHT and doc/LICENSE files which should be
   * included in the distribution.
   *
   * @package atk
   * @subpackage ui
   *
   * @copyright (c)2000-2006 Ivo Jansch
   * @copyright (c)2000-2006 Ibuildings.nl BV
   * @license http://www.achievo.org/atk/licensing ATK Open Source License
   *
   * @version $Revision$
   * $Id$
   */

  /**
   * Theme loader
   *
   * @author Ivo Jansch <ivo@achievo.org>
   * @author Boy Baukema <boy@ibuildings.nl>
   * @package atk
   * @subpackage ui
   *
   */
  class atkTheme
  {
    var $m_name ="";
    var $m_theme = array();

    /**
     * Function to get an Instance of the atkTheme class,
     * ensures that there is never more than one instance (Singleton pattern)
     * @return obj atkTheme instance
     */
    function &getInstance($reset=false)
    {
      static $s_instance = NULL;
      if ($s_instance == NULL || $reset)
      {
        $s_instance = new atkTheme();
      }
      return $s_instance;
    }

    /**
     * Constructor, initializes class and certain values
     * @access private
     */
    function atkTheme()
    {
      global $g_theme;
      atkdebug("Created a new atkTheme instance");
      if (isset($g_theme["Name"]) && $g_theme["Name"]!="")
      {
        $this->m_name = $g_theme["Name"];
      }
      else
      {
        $this->m_name = atkconfig("defaulttheme");
      }
      $this->_loadTheme();
    }

    /**
     * Convert a relative theme path to an absolute path.
     *
     * If a relative path starts with 'module/something' this method converts
     * the start of the path to the location where the module 'something' is
     * actually installed.
     *
     * @static
     * @param String $relpath The relative path to convert
     * @return String The absolute path
     */
    function absPath($relpath)
    {
      if ($relpath=="") return "";

      if (preg_match("!module/(.*?)/(.*)!", $relpath, $matches))
      {
        return moduleDir($matches[1]).$matches[2];
      }

      return atkconfig("atkroot").$relpath;
    }

    /**
     * Load the theme information into memory.
     *
     * If a cached file with theme information doesn't exist, it is compiled
     * from the theme dir.
     */
    function _loadTheme()
    {
      if (!count($this->m_theme))
      {
        $filename = atkconfig("atktempdir")."themes/".$this->m_name.".inc";
        if (!file_exists($filename) || atkconfig("force_theme_recompile"))
        {
           $compiler = &atknew("atk.ui.atkthemecompiler");
           $compiler->compile($this->m_name);
        }
        include($filename);
        $this->m_theme = $theme; // $theme is set by compiled file
      }
    }

    /**
     * Returns the value for themevalue
     * Example: getAttribute("highlight");
     *          returns "#eeeeee"
     * @param string $attribname the name of the attribute in the themedefinition
     * @return var the value of the attribute in the themedefinition
     */
    function getAttribute($attribname,$default="")
    {
      return (isset($this->m_theme["attributes"][$attribname])?$this->m_theme["attributes"][$attribname]:$default);
    }

    /**
     * Retrieve the location of a file
     * @access private
     * @param string $module the name of the module requesting the file
     */
    function getFileLocation($type, $name, $module="")
    {
      if ($module!="" & isset($this->m_theme["modulefiles"][$module][$type][$name]))
      {
        return moduleDir($module)."themes/".$this->m_theme["modulefiles"][$module][$type][$name];
      }
      if (isset($this->m_theme["files"][$type][$name]))
      {
        return atkTheme::absPath($this->m_theme["files"][$type][$name]);
      }
      return "";
    }


    /**
     * Returns full path for themed template file
     * @param string $tpl the template name
     * @param string $module the name of the module requesting the file
     * @return string the full path of the template file
     */
    function tplPath($tpl, $module="")
    {
      return $this->getFileLocation("templates", $tpl, $module);
    }

    /**
     * Returns full path for themed image file
     * @param string $img the image name
     * @param string $module the name of the module requesting the file
     * @return string the full path of the image file
     */
    function imgPath($img, $module="")
    {
      return $this->getFileLocation("images", $img, $module);
    }

    /**
     * Returns full path for themed style file
     * @param string $style the name of the CSS file
     * @param string $module the name of the module requesting the file
     * @return string the full path of the style file
     */
    function stylePath($style, $module="")
    {
      return $this->getFileLocation("styles", $style, $module);
    }

    /**
     * Returns full path for themed icon file
     * @param string $icon   the icon name (no extension)
     * @param string $type   the icon type (example: "recordlist")
     * @param string $module the name of the module requesting the file
     * @return string the full path of the icon file
     */
    function iconPath($icon, $type, $module="")
    {
      // Check module themes for icon
      $iconfile = $this->getIconFileFromModuleTheme($icon,$type);
      if ($module!="" && $iconfile) return moduleDir($module)."themes/".$iconfile;

      // Check the default theme for icon
      $iconfile = $this->getIconFileFromTheme($icon,$type, $this->m_theme['files']);
      if ($iconfile) return atkTheme::absPath($iconfile);

      // Check the default theme for default icon
      $iconfile = $this->getIconFileFromTheme('default',$type,$this->m_theme['files']);
      if ($iconfile) return atkTheme::absPath($iconfile);

      return false;
    }

    function getIconFileFromModuleTheme($icon,$type)
    {
      if (!isset($this->m_theme['modulefiles']))
        return false;
      $modules = atkGetModules();
      $modulenames = array_keys($modules);
      foreach ($modulenames as $modulename)
      {
        if (isset($this->m_theme['modulefiles']))
        {
          $iconfile = $this->getIconFileFromTheme($icon,$type, $this->m_theme['modulefiles'][$modulename]);
          if ($iconfile) return $iconfile;
        }
      }
      return false;
    }

    function getIconFileFromTheme($iconname, $type, $theme)
    {
      $allowediconext=array('png','gif','jpg');
      foreach ($allowediconext as $ext)
      {
        if (isset($theme['icons'][$type][$iconname.".".$ext]))
        {
          $iconfile = $theme['icons'][$type][$iconname.".".$ext];
          if ($iconfile) return $iconfile;
        }
      }
      return false;
    }

    /**
     * Gets the directory of the current theme
     * @return string full path of the current theme
     */
    function themeDir()
    {
      return atkTheme::absPath($this->getAttribute("basepath"));
    }
  }
?>

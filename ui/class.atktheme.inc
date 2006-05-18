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
    function getAttribute($attribname)
    {
      return (isset($this->m_theme["attributes"][$attribname])?$this->m_theme["attributes"][$attribname]:"");
    }

    function getFileLocation($type, $name)
    {
      return (isset($this->m_theme["files"][$type][$name])?atkconfig("atkroot").$this->m_theme["files"][$type][$name]:"");
    }


    /**
     * Returns full path for themed template file
     * @param string $tpl the template name
     * @return string the full path of the template file
     */
    function tplPath($tpl)
    {
      return $this->getFileLocation("templates", $tpl);
    }

    /**
     * Returns full path for themed image file
     * @param string $img the image name
     * @return string the full path of the image file
     */
    function imgPath($img)
    {
      return $this->getFileLocation("images", $img);
    }

    /**
     * Returns full path for themed style file
     * @param string $style the name of the CSS file
     * @return string the full path of the style file
     */
    function stylePath($style)
    {
      return $this->getFileLocation("styles", $style);
    }

    /**
     * Returns full path for themed icon file
     * @param string $icon      the icon name (no extension)
     * @param string $type      the icon type (example: "recordlist")
     * @param string $extension the icon extension, defaults to .gif
     * @return string the full path of the icon file
     */
    function iconPath($icon, $type, $extension=".gif")
    {

      return (isset($this->m_theme["files"]["icons"][$type][$icon.$extension])?atkconfig("atkroot").$this->m_theme["files"]["icons"][$type][$icon.$extension]:"");
    }

    /**
     * Gets the directory of the current theme
     * @return string full path of the current theme
     */
    function themeDir()
    {
      return atkconfig("atkroot").$this->getAttribute("basepath");
    }
  }
?>

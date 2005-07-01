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
   * @copyright (c)2000-2004 Ivo Jansch
   * @copyright (c)2000-2004 Ibuildings.nl BV
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
   /**
    * Flag for initialisation of atkTheme object
    * @var bool
    * @access private
    */
    var $m_initialized = false;
    
    /**
    * Constant with the directoryname where the themes can be found
    * @var string
    * @access public
    */
    var $THEMEDIRNAME = "themes";   
    
    /**
    * Constant with the filename where themedefinitions are stored
    * @var string
    * @access public
    */
    var $THEMEFILE = "themedef.inc";
    
    /**
    * Variable containing the name of the current theme
    * @var string
    * @access private
    */
    var $m_curtheme;
    
    /**
    * Directoryname where the current theme can be found
    * @var string
    * @access private
    */
    var $m_curthemedir;
    
    /**
    * Array with the entire current themedefinition
    * @var array
    * @access private
    */
    var $m_def = array();
    
    /**
     * Function to get an Instance of the atkTheme class,
     * ensures that there is never more than one instance (Singleton pattern)
     * @return obj atkTheme instance
     */
    function &getInstance()
    {
      static $s_instance = NULL;
      if ($s_instance == NULL)
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
      atkdebug("Created a new atkTheme instance");
      $this->m_curtheme = atkconfig("defaulttheme");
      $this->_setCurThemedir();
      $this->_initTheme();
    }
    
    /**
     * Initialiser for the atkTheme class
     * @access private
     */
    function _initTheme()
    {
      if (!$this->m_initialized)
      {
        // Include layout.inc file (oldstyle, soon to be deprecated)
        if (file_exists($this->m_curthemedir."layout.inc"))
        {
          global $g_theme;
          include_once($this->m_curthemedir."layout.inc");
        }
        
        $this->_getCachedTheme();
        $this->m_initialized = true;
      }
    }
    
    /**
     * Checks wether the current theme is cached yet, if not it calls the compiler and compiles it
     * @access private
     */
    function _getCachedTheme()
    {
      if (!atkconfig("force_theme_recompile")) $this->_loadThemedef();
      if(!$this->m_def)
      {
        atkdebug("Cached themedefinition was not found, compiling one");
        $compiler = &atknew("atk.ui.atkthemecompiler");
        $compiler->compile();
        $this->_loadThemedef();
      }
    }
    
    /**
     * Includes and loads the current themedefinition
     * @access private
     */
    function _loadThemedef()
    {
      $filelocation = atkconfig("atktempdir").$this->THEMEDIRNAME."/".$this->m_curtheme;
      
      if (file_exists($filelocation))
      {
        include_once($filelocation);
        $this->m_def = $theme;
      }
    }
    
    /**
     * Gets the location of the themedefinition for the current theme
     * @access private
     */
    function _setCurThemedir()
    {
      $themedefs = array(
      atkconfig("atkroot").$this->THEMEDIRNAME."/".$this->m_curtheme,
      atkconfig("atkroot")."atk/".$this->THEMEDIRNAME."/".$this->m_curtheme,
      atkconfig("atkroot")."atk/".$this->THEMEDIRNAME."/default",
      );
      
      foreach ($themedefs as $themedef)
      {
        if (file_exists($themedef."/".$this->THEMEFILE))
        {
          $this->m_curthemedir = $themedef."/";
          return;
        }
      }
    }
    
    /**
     * Gets an attribute from the stacked array with all the themedefinitions
     * @access private
     * @param string $name      the name of the stack to look in
     * @param string $searchfor the item in the attribute to search for
     * @return array returns array("dir"=>"directory where we found it","item"=>"the item") if found, void otherwise
     */
    function _getStackedItem($name, $searchfor)
    {
      for($counter=0;$counter <= count($this->m_def[$name]);$counter++)
      {
        if (isset($this->m_def[$name])&&isset($this->m_def[$name][$counter])&&$this->m_def[$name][$counter])
        {
          foreach($this->m_def[$name][$counter] as $from => $itemarray)
          {
            foreach($itemarray as $item)
            {
              if ($item == $searchfor)
              {
                return array("dir" => $from, "item" => isset($itemarray[$searchfor])?$itemarray[$searchfor]:NULL);
              }
            }
          }
        }
      }
     atkdebug("Couldn't find $searchfor in stack: $name");
    }
    
    /**
     * Gets an attribute from the current themedefinition
     * example: getAttribute("images", "teknoman.jpg", "/images");
     * @param string $attrib            the name of the attribute
     * @param var $attribvalue          the value of the attribute in the theme
     * @param string $returnpath        the path where the attribute can be found (like /templates/), always begins with a /
     * @param string $defaultreturnpath the path where the attribute can be found in the default ATK theme, will default to the normal returnpath
     */
    function _getAttribute($attribname, $attribvalue="", $returnpath="", $defaultreturnpath="")
    { 
      global $PHP_SELF;
      
      if (!$defaultreturnpath) $defaultreturnpath = $returnpath;
      
      if(is_array($this->m_def[$attribname]))
      {
        $attrib = $this->_getStackedItem($attribname,$attribvalue);
        if ($attrib)
        {
          return atkconfig("atkroot").$attrib["dir"].$returnpath;
        }
        return atkconfig("atkroot")."atk/themes/default/".$defaultreturnpath;
      }
      else
      {
        return $this->m_def[$attribname];
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
      return $this->_getAttribute($attribname);
    }
    
    /**
     * Returns full path for themed template file, interface to the _getAttribute function
     * @param string $tpl the template name
     * @return string the full path of the template file
     */
    function tplPath($tpl)
    {
      return $this->_getAttribute("customtpls",$tpl, "/templates/".$tpl);
    }
    
    /**
     * Returns full path for themed image file, interface to the _getAttribute function
     * @param string $img the image name
     * @return string the full path of the image file
     */
    function imgPath($img)
    {
      return $this->_getAttribute("customimgs", $img, "/images/".$img);
    }
    
    /**
     * Returns full path for themed stylefile file, interface to the _getAttribute function
     * @param string $style the name of the CSS file
     * @return string the full path of the style file
     */
    function stylePath($style)
    { 
      return $this->_getAttribute("customstyles", $style, "/styles/".$style);
    }
    
    /**
     * Returns full path for themed icon file, interface to the _getAttribute function
     * @param string $icon      the icon name (no extension)
     * @param string $type      the icon type (example: "recordlist")
     * @param string $extension the icon extension, defaults to .gif
     * @return string the full path of the icon file
     */
    function iconPath($icon, $type, $extension=".gif")
    {
      $iconpath = $this->_getAttribute($type."_icons", $icon.$extension, "/icons/$type/$icon".$extension);
      if (!file_exists($iconpath))
      {
        $iconpath = $this->_getAttribute($type."_icons", "default".$extension, "/icons/$type/default".$extension);
      }
      return $iconpath;
    }
      
    /**
     * Gets the directory of the current theme
     * @return string full path of the current theme
     */
    function themeDir()
    {
      return $this->m_curthemedir;
    }
  }
?>

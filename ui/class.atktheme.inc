<?php  
/**
* atkTheme class
*/
class atkTheme
{
  var $atkroot;                      //root of the ATK application
  var $m_initialized = false;        //boolean, true if atkTheme object is initialised
  
  var $m_themedirname = "themes";    //directoryname where the themes can be found... this shouldn't change often
  var $m_themefile = "themedef.inc"; //filename where themedefinitions are stored
  
  var $m_curtheme;                   //the current theme
  var $m_curthemedir;                //gets the directory where the current theme can be found
  
  var $m_def = array();              //the current themedefinition
  
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
  */
  function atkTheme()
  {
    atkdebug("Created a new atkTheme instance");
    $this->atkroot = atkconfig("atkroot");
    $this->m_curtheme = atkconfig("defaulttheme");
    $this->getCurThemedir();
    $this->initTheme();
  }
  
  /**
  * Initialiser for the atkTheme class
  */
  function initTheme()
  {
    if (!$this->m_initialized)
    {
      // Include layout.inc file (oldstyle, soon to be deprecated)
      if (file_exists($this->m_curthemedir."layout.inc"))
      {
        global $g_theme;
        include_once($this->m_curthemedir."layout.inc");
      }
      
      $this->getCachedTheme();
      $this->m_initialized = true;
    }
  }
  
  /*
  * Checks wether the current theme is cached yet, if not it calls the compiler and compiles it
  */
  function getCachedTheme()
  {
    $this->getThemedef();
    if(!$this->m_def)
    {
      atkdebug("Cached themedefinition was not found, compiling one");
      $compiler = &atknew("atk.ui.atkthemecompiler");
      $compiler->compile();
      $this->getThemedef();
    }
  }
  
  /**
  * Includes and loads the themedefinition
  */
  function getThemedef()
  {
    $filelocation = atkconfig("atktempdir").$this->m_themedirname."/".$this->m_curtheme;
    
    if (file_exists($filelocation))
    {
      include_once($filelocation);
      $this->m_def = $theme;
    }
  }
  
  /**
  * Gets the location of the themedefinition for the current theme
  * @return void
  */
  function getCurThemedir()
  {
    $themedefs = array(
    $this->atkroot.$this->m_themedirname."/".$this->m_curtheme,
    $this->atkroot."atk/".$this->m_themedirname."/".$this->m_curtheme,
    $this->atkroot."atk/".$this->m_themedirname."/default",
    );
    
    foreach ($themedefs as $themedef)
    {
      if (file_exists("$themedef/$themefile"))
      {
        $this->m_curthemedir = $themedef."/";
        return;
      }
    }
  }
  
  /**
  * Gets an attribute from a stacked array from the themedefinitions
  * @param string $name      the name of the attribute
  * @param string $searchfor the item in the attribute to search for
  * @return array returns array("dir"=>"directory where we found it","item"=>"the item") if found, void otherwise
  */
  function getStackedItem($name, $searchfor)
  {
    for($counter=0;$counter <= count($this->m_def[$name]);$counter++)
    {
      if ($this->m_def[$name][$counter])
      {
        foreach($this->m_def[$name][$counter] as $from => $itemarray)
        {
          foreach($itemarray as $item)
          {
            if ($item == $searchfor)
            {
              return array("dir" => $from, "item" => $itemarray[$searchfor]);
            }
          }
        }
      }
    }
   atkdebug("Couldn't find $searchfor in stack: $name");
  }
  
  /**
  * Gets an attribute from the current themedefinition
  * example: getAttribute("highlight");
  * @param string $attrib the name of the attribute
  * @param var the value of the attribute in the theme
  */
  function getAttribute($attribname, $attribvalue="", $returnpath="", $defaultreturnpath="")
  {
    if(is_array($this->m_def[$attribname]))
    {
      $attrib = $this->getStackedItem($attribname,$attribvalue);
      if ($attrib)
      {
        return $this->atkroot."../".$attrib["dir"].$returnpath;
      }
      return $this->atkroot."atk/themes/default/".$defaultreturnpath;
    }
    else
    {
      //atkdebug("<H1>getattribute called with $attribname</H1>");
      //atk_var_dump($this->m_def);
      return $this->m_def[$attribname];
    }
  }
  
  /**
  * Returns full path for themed template file, interface to the getAttribute function
  * @param string $tpl the template name
  * @return string the full path of the template file
  */
  function tplPath($tpl)
  {
    return $this->getAttribute("customtpls",$tpl, "/templates/".$tpl,"templates/".$tpl);
  }
  
  /**
  * Returns full path for themed image file, interface to the getAttribute function
  * @param string $img the image name
  * @return string the full path of the image file
  */
  function imgPath($img)
  {
    return $this->getAttribute("customimgs", $img, "/images/".$img, "images/".$img);
  }
  
  /**
  * Returns full path for themed stylefile file, interface to the getAttribute function
  * @param string $style the name of the CSS file
  * @return string the full path of the style file
  */
  function stylePath($style)
  { 
    return $this->getAttribute("customstyles", $style, "/styles/".$style,"styles/".$style);
  }
  
  /**
  * Returns full path for themed icon file, interface to the getAttribute function
  * @param string $icon the icon name
  * @param string $type the icon type (example: "recordlist")
  * @return string the full path of the icon file
  */
  function iconPath($icon, $type)
  {
    return $this->getAttribute($type."_icons", $icon, "/icons/$type/$icon.gif","icons/$type/$icon.gif");
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
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
   * @copyright (c)2000-2004 Ibuildings.nl BV
   * @license http://www.achievo.org/atk/licensing ATK Open Source License
   *
   * @version $Revision$
   * $Id$
   */
   
  /**
   * Compiles cache for current theme.
   *
   * Can be seen as two parts, everything above the compile function takes
   * care of assembling the themedefinition and everything below takes care
   * of actually compiling it.
   *
   * @author Boy Baukema <boy@ibuildings.nl>
   * @package atk
   * @subpackage ui
   *
   */
  class atkThemeCompiler extends atkTheme
  {
    /**
     * Stack with all (fallback) directories to check for themefiles
     * @var array
     * @access private
     */
    var $m_themedefs; 
  
    /**
     * String with the indentation to use for the cache file
     * @var string
     * @access private
     */
    var $indentation;
  
    /**
     * Directory where we make a template directory and store the cachefile
     * @var bool
     * @access private
     */
    var $m_cachedir;
    
    /**
    * Constructor, initialises several values
    */
    function atkThemeCompiler()
    {
      $this->m_cachedir = atkconfig("atktempdir");
      $this->m_curtheme = atkconfig("defaulttheme");
      $this->_fillThemedefs();
    }
    
    /**
     * Fills a stack with all fallback locations for themedefinitions
     */
    function _fillThemedefs()
    {
      global $g_modules;
      
      $this->m_themedefs[] = $this->THEMEDIRNAME."/".$this->m_curtheme."/".$this->THEMEFILE;
      $this->m_themedefs[] = $this->THEMEDIRNAME."/default/".$this->THEMEFILE;    
      
      foreach ($g_modules as $modname => $modpath)
      {
        $this->m_themedefs[] = "modules/".$modname."/".$this->THEMEDIRNAME."/".$this->m_curtheme."/".$this->THEMEFILE;
        $this->m_themedefs[] = "modules/".$modname."/".$this->THEMEDIRNAME."/default/".$this->THEMEFILE;
      }
      
      $this->m_themedefs[] = "atk/".$this->THEMEDIRNAME."/".$this->m_curtheme."/".$this->THEMEFILE;
      $this->m_themedefs[] = "atk/".$this->THEMEDIRNAME."/default/".$this->THEMEFILE;    
    }
        
    /**
     * Function responsible for getting the themedefinitions
     * @return void
     */
    function getThemedef()
    {
      $file = array_shift($this->m_themedefs);
      if (file_exists(atkconfig("atkroot").$file))
      {
        include(atkconfig("atkroot").$file);
        atkdebug("Found $file, going to compile it");
        $fromlength = strrpos($file, "/");
        $from = substr($file, 0, $fromlength);
        
        $this->_checkForBase($theme, $from);      
        $this->_checkForAutodetect($theme, $from);
        
        $this->_addThemedef($theme, $from);
      }
      if ($this->m_themedefs[0]) { $this->getThemedef();}
    }
    
    /**
     * If the theme has a base, we go back and do that one first
     * @param array $theme the current themedefinition
     * @param string $from the directory of the current themedefinition
     * @return void
     */
    function _checkForBase($theme, $from)
    {
      if ($theme["base"])
      {
        array_unshift($this->m_themedefs, "$from/../".$theme["base"]."/".$this->THEMEFILE);
      }
    }
    
    /**
     * If a theme has attributes with a "scan 'somedir/'" 
     * it will scan the files in that dir
     * @param array &$theme the current themedefinition
     * @param string $from the directory of the current themedefinition
     */
    function _checkForAutodetect(&$theme, $from)
    {      
      foreach ($theme as $themename => $themedata)
      {
        if (!is_array($themedata))
        {
          if (substr($themedata, 0, 4) == "scan")
          {
            $directories = $this->_getAutodetectdirectories($themedata);
            foreach ($directories as $dir)
            {
              $files = $this->_scanAutodetectdirectory($dir, $from);
              $theme[$themename] = $files;
            }
          }
        }
      }
    }

    /**
     * Extracts the director(y/ies) we need to scan, checks wether the user set the slashes
     * Multiple directories are seperated with a ;
     * @param array &$theme the current themedefinition
     * @param string $from the directory of the current themedefinition
     * @return array an array with the directories
     */
    function _getAutodetectdirectories($themedata)
    {
      $begindirs = strpos($themedata, "'");
      $enddirs = strrpos($themedata, "'");
      $dirnamelength = $enddirs - $begindirs;
      $directories = substr($themedata, $begindirs+1, $dirnamelength-1);
      $directories = explode(";", $directories);
      foreach ($directories as $dirid => $directory)
      {
        if (substr($directory, 0, 1) !== "/") $directories[$dirid] = "/$directory";
        if (substr($directory, -1, 1) !== "/") $directories[$dirid] .= "/";
      }
      return $directories;  
    }
    
    /**
     * Scans the directory given to it and returns the files it finds
     * @param string $directory the directory we need to scan
     * @param string $from the directory of the current themedefinition
     * @return array an array with the files we found
     */
    function _scanAutodetectdirectory($directory, $from)
    {
      $handle=opendir(atkconfig("platformroot").$from.$directory);
      while (($file = readdir($handle))!==false) 
      {
        if ($file != "." && $file != ".." && $file != "CVS") 
        {
          $detectedfiles[] = $file;
        }
      }
	  closedir($handle); 
      return $detectedfiles;
    }
    
    
    /**
     * This function adds themedefitems from initialisation to the central themedef
     * If the item is an array it unshifts in on a queue.
     * @param array $themedef
     * @param string $from
     */
    function _addThemedef($themedef, $from)
    {
      foreach ($themedef as $themedefitemname => $themedefitem)
      {
        if (!is_array($themedef[$themedefitemname]))
        {
          $this->m_def[$themedefitemname]=$themedefitem;
        }
        else
        {
          if (!is_array($this->m_def[$themedefitemname]))
          {
            $this->m_def[$themedefitemname] = array();
          }
          $this->m_def[$themedefitemname][]= array($from => $themedefitem);
        }
      }
    }
    
    /**
     * The compiler, compiles the cache for the current theme
     */
    function compile()
    {
      $this->getThemedef();
      
      $compiled = $this->_compileThemedef();
      $this->_checkLocation();
      $this->_writeCache($compiled);
    }
    
    /**
     * Writes the cache to a file
     * @param string $compiled the cache
     */
    function _writeCache($compiled)
    {
      $filename = $this->m_cachedir.$this->THEMEDIRNAME."/".$this->m_curtheme;
      
      if($filepointer = fopen($filename,"w"))
      {
        fwrite($filepointer, $compiled);
        fclose($filepointer);
      } else atkerror("Unable to write theme cache file! Permission denied!");
    }
    
    /**
     * Takes care of indentation, though not absolutely necessary for the system,
     * it makes the compiled cachefile easier to read for humans
     * @return string the indentation, a string with spaces
     */
    function _getIndentation()
    {
      $dent = "";
      for ($counter = 0; $counter <= $this->indentation; $counter++)
      {
        $dent .= " ";
      }
      return $dent;
    }
    
    /**
     * Transforms the themedefinition to a php file compatible format
     * @return string the compiled themedefinition
     */
    function _compileThemedef()
    {
      $this->_compileBegin($file);
      
      foreach ($this->m_def as $themedefitemname => $themedefitem)
      {
        $this->_compileItem($themedefitemname, $themedefitem, $file);
      }
      $this->_compileEnd($file);
      return $file;
    }
    
    /**
     * Adds a beginning to the compiling file
     * @param string &$file the compiled cache so far
     */
    function _compileBegin(&$file)
    {
      $file.="<"."?php\n";
      $this->indentation += 2;
      $file.=$this->_getIndentation()."\$theme = array(\n";
    }
    
    /**
     * Parses an array for the themecompiler
     * @param string $themedefitemname the name of the item
     * @param var $themedefitem        the value of the item
     * @param string &$file            the compiled cache so far
     */
    function _compileItem($themedefitemname, $themedefitem, &$file)
    {
      if (!is_array($themedefitem))
      {
        $file.=$this->_getIndentation().'"'.$themedefitemname.'" => "'.$themedefitem."\",\n";
      }
      else
      {
        $this->_compileArray($themedefitem, $themedefitemname, $file);
      }
    }
    
    /**
     * Compiles an array
     * @param var $array        the array to be compiled
     * @param string $arrayname the item name
     * @param string &$file     the compiled cache so far
     */
    function _compileArray($array, $arrayname, &$file)
    {
      $file.=$this->_getIndentation().'"'.$arrayname."\" => array(\n";
      
      foreach ($array as $themedefitemname => $themedefitem)
      {
        $this->indentation += 2;
        $this->_compileItem($themedefitemname, $themedefitem, $file);
        $this->indentation -= 2;
      }
      
      $file .= $this->_getIndentation()."),\n";
    }
    
    /**
     * Adds an ending to the compiling file
     * @param string &$file the compiled cache so far
     */
    function _compileEnd(&$file)
    {
      $file .= $this->_getIndentation().");\n";
      $this->indentation -= 2;
      $file .= "?".">";
    }
    
    /**
     * Checks if the directory and file we want to write to actually exist,
     * if not we create them and check again.
     * Please note that this function is not exited until the file and directory exist
     * this might create trouble if we do not have the rights,
     * but we should have the rights anyway.
     * @return void
     */
    function _checkLocation()
    {
      $dir = $this->m_cachedir.$this->THEMEDIRNAME;
      $file = $dir."/".$this->m_curtheme;
      
      if (is_dir($dir))
      {
        if (is_file($file))
        {
          return;
        }
        if (!touch($file)){echo(text("cannot make file", "", "atk").$file.text("check rights to", "", "atk").$this->m_cachedir.$this->THEMEDIRNAME); exit;};
        return $this->_checkLocation();
      }
      if (!mkdir($this->m_cachedir.$this->THEMEDIRNAME)){echo(text("cannot create directory", "", "atk").$this->m_cachedir.$this->THEMEDIRNAME." ".text("check rights to", "", "atk").$this->m_cachedir);exit;}
      return $this->_checkLocation();
    }
  }
  
?>
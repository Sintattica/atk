<?php 
/**
* Compiles cache for current theme
* Can be seen as two seperate classes, everything above the compile function takes care
* of assembling the themedefinition and everything below takes care of actually compiling it.
*/
class atkThemeCompiler extends atkTheme
{
  var $m_themedefs; // stack with all (fallback) directories to check for themefiles
  var $indentation; // indentation for the cache file
  var $m_cachedir;  // directory where we make a template directory and store the cachefile
  
  /**
  * Constructor, initialises several values
  */
  function atkThemeCompiler()
  {
    $this->m_cachedir = atkconfig("atktempdir");
    $this->m_curtheme = atkconfig("defaulttheme");
    $this->fillThemedefs();
  }
  
  /**
  * Fills a stack with all fallback locations for themedefinitions
  */
  function fillThemedefs()
  {
    global $g_modules;
    
    $this->m_themedefs[] = $this->m_themedirname."/".$this->m_curtheme."/".$this->m_themefile;
    $this->m_themedefs[] = $this->m_themedirname."/default/".$this->m_themefile;    
    
    foreach ($g_modules as $modname => $modpath)
    {
      $this->m_themedefs[] = "modules/".$modname."/".$this->m_themedirname."/".$this->m_curtheme."/".$this->m_themefile;
      $this->m_themedefs[] = "modules/".$modname."/".$this->m_themedirname."/default/".$this->m_themefile;
    }
    
    $this->m_themedefs[] = "atk/".$this->m_themedirname."/".$this->m_curtheme."/".$this->m_themefile;
    $this->m_themedefs[] = "atk/".$this->m_themedirname."/default/".$this->m_themefile;    
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
      
      $this->checkForBase($theme, $from);      
      
      $this->addThemedef($theme, $from);
    }
    if ($this->m_themedefs[0]) { $this->getThemedef();}
  }
  
  /**
  * If the theme has a base, we go back and do that one first
  * @param array $theme the current themedefinition
  * @param string $from the directory of the current themedefinition
  * @return void
  */
  function checkForBase($theme, $from)
  {
    if ($theme["base"])
    {
      array_unshift($this->m_themedefs, atkconfig("atkroot")."$from/../".$theme["base"]."/".$this->m_themefile);
      return $this->getThemedef();
    }
  }
  
  /**
  * This function adds themedefitems from initialisation to the central themedef
  * If the item is an array it unshifts in on a queue.
  * @param array $themedef
  * @param string $from
  */
  function addThemedef($themedef, $from)
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
    
    $compiled = $this->compileThemedef();
    $this->checkLocation();
    $this->writeCache($compiled);
  }
  
  /**
  * Writes the cache to a file
  * @param string $compiled the cache
  */
  function writeCache($compiled)
  {
    $filename = $this->m_cachedir.$this->m_themedirname."/".$this->m_curtheme;
    
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
  function getIndentation()
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
  function compileThemedef()
  {
    $this->compileBegin($file);
    
    foreach ($this->m_def as $themedefitemname => $themedefitem)
    {
      $this->compileItem($themedefitemname, $themedefitem, $file);
    }
    $this->compileEnd($file);
    return $file;
  }
  
  /**
  * Adds a beginning to the compiling file
  * @param string &$file the compiled cache so far
  */
  function compileBegin(&$file)
  {
    $file.="<"."?php\n";
    $this->indentation += 2;
    $file.=$this->getIndentation()."\$theme = array(\n";
  }
  
  /**
  * Parses an array for the themecompiler
  * @param string $themedefitemname the name of the item
  * @param var $themedefitem        the value of the item
  * @param string &$file            the compiled cache so far
  */
  function compileItem($themedefitemname, $themedefitem, &$file)
  {
    if (!is_array($themedefitem))
    {
      $file.=$this->getIndentation().'"'.$themedefitemname.'" => "'.$themedefitem."\",\n";
    }
    else
    {
      $this->compileArray($themedefitem, $themedefitemname, $file);
    }
  }
  
  /**
  * Compiles an array
  * @param var $array        the array to be compiled
  * @param string $arrayname the item name
  * @param string &$file     the compiled cache so far
  */
  function compileArray($array, $arrayname, &$file)
  {
    $file.=$this->getIndentation().'"'.$arrayname."\" => array(\n";
    
    foreach ($array as $themedefitemname => $themedefitem)
    {
      $this->indentation += 2;
      $this->compileItem($themedefitemname, $themedefitem, $file);
      $this->indentation -= 2;
    }
    
    $file .= $this->getIndentation()."),\n";
  }
  
  /**
  * Adds an ending to the compiling file
  * @param string &$file the compiled cache so far
  */
  function compileEnd(&$file)
  {
    $file .= $this->getIndentation().");\n";
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
  function checkLocation()
  {
    $dir = $this->m_cachedir.$this->m_themedirname;
    $file = $dir."/".$this->m_curtheme;
    
    if (is_dir($dir))
    {
      if (is_file($file))
      {
        return;
      }
      touch($file);
      return $this->checkLocation();
    }
    mkdir($this->m_cachedir.$this->m_themedirname);
    return $this->checkLocation();
  }
}
?>
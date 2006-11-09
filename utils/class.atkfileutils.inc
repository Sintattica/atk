<?php
/**
 * This file is part of the Achievo ATK distribution.
 * Detailed copyright and licensing information can be found
 * in the doc/COPYRIGHT and doc/LICENSE files which should be
 * included in the distribution.
 *
 * @package atk
 * @subpackage utils
 *
 * @author Dennis-Jan Broerse <dennisjan@ibuildings.nl>
 *
 * @copyright (c) 2006 Ibuildings.nl BV
 * @license see doc/LICENSE
 *
 * @version $Revision$
 * $Id$
 */


/**
 * This class provide functions to create, copy or check files or directories. 
 *
 * @author dennisjan <dennisjan@ibuildings.nl>
 * @package ATK
 * @subpackage Utils
 *
 */
class atkFileUtils
{

  /**
   * This function copies everything what is in the source directory
   * to the destination directory. 
   * 
   * @static
   * @staticvar $orgdest to     store the first original destination.
   * @param string $source      path to the skell to copy
   * @param string $dest        path to where the skell has to be copied to
   * @param string $dirname     unique name for the first directory
   * @param octal  $privileges  octal number for the rights of the written
   * @return bool returns true when skell is copied to the destination.
   */   
  function copyDirRecursive($source, $dest, $dirname='', $privileges=0777)
  {
    static $orgdest = null;  
      
    if (is_null($orgdest))
      $orgdest = $dest;
     
    atkdebug("Checking write permission for ".$orgdest);    

    if (!atkFileUtils::is_writable($orgdest))
    {
      atkdebug("Error no write permission!");
      return false;
    }

    atkdebug("Permission granted to write.");
    
    if ($dest == $orgdest && $dirname != '')
    {
      mkdir($orgdest . "/" . $dirname,$privileges);
      return atkFileUtils::copyDirRecursive($source,$orgdest."/".$dirname,'',$privileges);
    }
    
    // Simple copy for a file
    if (is_file($source))
    {
      $c = copy($source, $dest);

      chmod($dest, $privileges);

      return $c;
    }

    // Make destination directory
    if (!is_dir($dest))
    {
      if ($dest != $orgdest && !is_dir($orgdest.'/'.$dirname) && $dirname != '')
      $dest = $orgdest.'/'.$dirname;

      $oldumask = umask(0);

      mkdir($dest, $privileges);

      umask($oldumask);
    }

    // Loop through the folder
    $dir = dir($source);

    while (false !== $entry = $dir->read())
    {
      // Skip pointers
      if ($entry == '.' || $entry == '..')
        continue;

      // Deep copy directories
      if ($dest !== "$source/$entry")
        atkFileUtils::copyDirRecursive("$source/$entry", "$dest/$entry", $dirname, $privileges);
    }

    // Clean up
    $dir->close();
    
    return true;
  }

  /**
   * This function checks if the root of the destination is writeable.
   * The difference with php native function is that this functions accepts
   * non-existing directories.
   *
   * @static 
   * @param string $orgdest document parh
   * @return bool returns true if the destination is writeable.
   */   
  function is_writable($orgdest)
  {
    if ($orgdest{0} == '/')
    {
      if (count($orgdest) == 1)
        $testdest = $orgdest;
      else 
        $testdest= substr($orgdest, 0, strpos($orgdest, '/', 1));    
    }
    else 
    {
      if ($orgdest{strlen($orgdest)-1} != '/' && !is_file($orgdest))
      $orgdest .= '/';

      $testdest = $orgdest;

      if (!is_dir($orgdest))
      {
        $orgdestArray = explode('/', $orgdest);

        $testdest = $orgdestArray[0].'/';
      }
    }

    atkdebug("Checking with: ".$testdest);

    return is_writable($testdest);    
  }
  
  /**
   * This function creates recursively a destination. This fuction accepts
   * a full path ../dir/subdir/subdir2/subdir3 etc. It checks if the path is writeable 
   * and replace mis typed slashes with forward slashes. 
   *
   * @static 
   * @param string $dir the fullpath
   * @param octal  $privileges  octal number for the rights of the written
   * @return bool returns true if the destination is written.
   */   
  function mkdirRecursive($dir, $privileges=0777, $recursive=true)
   {
     $dir = preg_replace('/(\/){2,}|(\\\){1,}/','/',$dir); //only forward-slash     
     
     if (!atkFileUtils::is_writable($dir))
     {
       atkdebug("Error no write permission!");
       return false;
     }

     atkdebug("Permission granted to write.");
     
     if( is_null($dir) || $dir === "" ){
       return FALSE;
     }
     if( is_dir($dir) || $dir === "/" ){
       return TRUE;
     }
     if( atkFileUtils::mkdirRecursive(dirname($dir), $privileges, $recursive) ){
       return mkdir($dir, $privileges);
     }
     return FALSE;
   }
   
   /**
    * This function parse a templatestring with the data and returns 
    * a string with the data parsed in the template.
    *
    * @static 
    * @param string $template the template to parse
    * @param array $data array which contains the data for the template
    * @return string returns the parsed string
    */
   function parseDirectoryName($template, $data)
   {
     atkimport("atk.utils.atkstringparser");
     $stringparser = new atkStringParser($template);
     return $stringparser->parse($data);
   }
}

?>
<?php
/**
 * This file is part of the Ibuildings E-business Platform.
 * Detailed copyright and licensing information can be found
 * in the doc/COPYRIGHT and doc/LICENSE files which should be 
 * included in the distribution.
 *
 * @package Atk
 * @subpackage Utils
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
   * Constructor
   *
   */
  function atkFileUtils()
  {

  }

  /**
   * Gets an instance of the atkFileUtils class
   *
   * Using this function will ensure that only 1 instance ever exists
   * (singleton).
   *
   * @return atkFileUtils Instance of the atkFileUtils class
   */
  function &getInstance()
  {
    static $s_atkFileUtils;
    if (!$s_atkFileUtils)
      $s_atkFileUtils = new atkFileUtils();
    
    return $s_atkFileUtils;
  }

  /**
   * This function copies everything what is in the source directory
   * to the destination directory. 
   * 
   * @static
   * @staticvar $orgdest to store the first original destination.
   * @param string $source  path to the skell to copy
   * @param string $dest    path to where the skell has to be copied to
   * @param string $dirname unique name for the first directory
   * @param octal  $rights  octal number for the rights of the written
   * @return bool returns true when skell is copied to the destination.
   */   
  function atkCopyDirRec($source, $dest, $dirname='', $rights=0777)
  {
    static $orgdest = null;  
      
    if (is_null($orgdest))
      $orgdest = $dest;
     
    atkdebug("Checking write permission for ".$orgdest);    

    if (!atkFileUtils::is__writable($orgdest))
    {
      atkdebug("Error no write permission!");
      return false;
    }

    atkdebug("Permission granted to write.");    
    
    // Simple copy for a file
    if (is_file($source))
    {
      $c = copy($source, $dest);

      chmod($dest, $rights);

      return $c;
    }

    // Make destination directory
    if (!is_dir($dest))
    {
      if ($dest != $orgdest && !is_dir($orgdest.'/'.$dirname) && $dirname != '')
      $dest = $orgdest.'/'.$dirname;

      $oldumask = umask(0);

      mkdir($dest, $rights);

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
        atkFileUtils::atkCopyDirRec("$source/$entry", "$dest/$entry", $dirname, $rights);
    }

    // Clean up
    $dir->close();
    
    return true;
  }

  /**
   * This function checks if the root of the destination is writeable.
   *
   * @static 
   * @param string $orgdest document parh
   * @return bool returns true if the destination is writeable.
   */   
  function is__writable($orgdest)
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
   * @param string $orgdest the fullpath
   * @param octal  $rights  octal number for the rights of the written
   * @return bool returns true if the destination is written.
   */   
  function atkCreateDest($orgdest, $rights=0777)
   {
     $dirs=array();     
     
     if (!atkFileUtils::is__writable($orgdest))
     {
       atkdebug("Error no write permission!");
       return false;
     }

     atkdebug("Permission granted to write.");
     
     $orgdest=preg_replace('/(\/){2,}|(\\\){1,}/','/',$orgdest); //only forward-slash
     $dirs=explode("/",$orgdest);
     
     $orgdest="";
     
     foreach ($dirs as $element)
     {
       $orgdest .= $element."/";
       
       if(!is_dir($orgdest))
       {
         $oldumask = umask(0);
         atkdebug("Making directory: ".$orgdest);
         mkdir($orgdest, $rights);

         umask($oldumask);
       }
     }
     
     return true;
   }
}

?>
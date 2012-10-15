<?php
  /**
   * This file is part of the Achievo ATK distribution.
   * Detailed copyright and licensing information can be found
   * in the doc/COPYRIGHT and doc/LICENSE files which should be 
   * included in the distribution.
   *
   * Class used by the application level record locking mechanism.
   * If the given lock is (still) valid we try to extend the lease time.
   *
   * @package atk
   * @subpackage lock
   * @author Peter C. Verhage <peter@ibuildings.nl>
   * @access private
   *
   * @copyright (c)2000-2004 Ibuildings.nl BV
   * @license http://www.achievo.org/atk/licensing ATK Open Source License
   *
   * @version $Revision: 1684 $
   * $Id$
   */
   
  /** @internal includes, defines etc. */
  global $ATK_VARS;
  $id = (int)$ATK_VARS["id"];
  $type = ($ATK_VARS["type"] == "xml") ? "xml" : "image";
  
  $lock = &atkLock::getInstance();
  
  /* extend lock lease */
  if ($lock->extend($id))
  {
    // xml
    if ($type == "xml")
    {
      header("Content-type: text/xml");  
      echo "<response><success/></response>";
    }
    
    // image
    else
    {
      header("Content-type: image/gif");
      readfile(atkconfig("atkroot").'atk/images/dummy.gif');
    }
  }
  
  /* failure */
  else
  {
    // xml
    if ($type == "xml")
      echo "<response><failure/></response>";
    
    // image
    else
      header("HTTP/1.0 404 Not Found");
  }
?>
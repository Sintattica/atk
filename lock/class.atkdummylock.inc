<?php
  /**
   * This file is part of the Achievo ATK distribution.
   * Detailed copyright and licensing information can be found
   * in the doc/COPYRIGHT and doc/LICENSE files which should be 
   * included in the distribution.
   *
   * @package atk
   * @subpackage lock
   *
   * @copyright (c)2000-2004 Ibuildings.nl BV
   * @license http://www.achievo.org/atk/licensing ATK Open Source License
   *
   * @version $Revision$
   * $Id$
   */
   
  /**
   * Dummy lock driver. Fakes locking functionality.
   *
   * @author Peter C. Verhage <peter@ibuildings.nl>
   * @package atk
   * @subpackage lock
   *
   */
  class atkDummyLock extends atkLock
  {
    /**
     * Dummy method.
     */
    function lock($selector, $table)
    {
      atkdebug("atkDummyLock::lock -- faking lock functionality");
      return TRUE;
    }
  
    /**
     * Dummy method.
     */
    function unlock($selector, $table)
    {
      atkdebug("atkDummyLock::unlock -- faking lock functionality");
    }
  
    /**
     * Dummy method.
     */
    function extend($identifier)
    {
      atkdebug("atkDummyLock::extend -- faking lock functionality");
      return TRUE;
    }
  
    /**
     * Dummy method.
     */
    function isLocked($selector, $table)
    {
      atkdebug("atkDummyLock::isLocked -- faking lock functionality");
      return NULL;
    }
  }
?>
<?php
/**
 * $Id$
 *
 * The ATK dummy lock fakes lock functionality.
 *
 * @author Peter C. Verhage <peter@ibuildings.nl>
 * @version $Revision$
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
<?php
  global $ATK_VARS;
  $id = (int)$ATK_VARS["id"];
  $message = text("lock_expired");
  $stack = $ATK_VARS["stack"];
?>
/**
 * $Id$
 *
 * The ATK lock javascript. This script allows us to extend the lock lease
 * for a certain record / item.
 *
 * @author Peter C. Verhage <peter@ibuildings.nl>
 * @version $Revision$
 */
var atkLock = new Object();

/**
 * Initialize the lock.
 * @param identifier the lock ID
 */
function atkLockInit(identifier)
{
  /* initialize */
  atkLock.theIdentifier = identifier;
  atkLock.theSequence   = 1;
  atkLock.isLocked      = true;

  /* start the timer */
  atkLockTimer();
}

/**
 * The lock timer recursively calls itself until the lock
 * lease has expired. Every run it checks if the lock is
 * still valid and increments the lock sequence.
 */
function atkLockTimer()
{
  if (!atkLock.isLocked) return;
  atkLockCheck();
  atkLock.theSequence++;
  setTimeout('atkLockTimer()', 30000);
}

/**
 * Fetch a new lock image, which extends the lock, or if
 * the lock lease has expired triggers an error.
*/
function atkLockCheck()
{
  var image = new Image();
  image.onerror = atkLockUnlock;
  image.src = 'include.php?file=atk/lock/lock.php&stack=<?=$stack?>&id=' + atkLock.theIdentifier + '&sequence=' + atkLock.theSequence;
}

/**
 * When the lock lease has expired we notify the user.
 */
function atkLockUnlock()
{
  atkLock.isLocked = false;
  if (typeof(document.images['_lock_']) != 'undefined')
    document.images['_lock_'].src='<?=atkconfig("atkroot")?>atk/images/lock_expired.gif';
  alert('<?=addslashes($message)?>');
}

atkLockInit('<?=$id?>');
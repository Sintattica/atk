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
  atkLock.type          = (window.XMLHttpRequest || window.ActiveXObject) ? 'xml' : 'image';

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
 * Check the DOM XML document response.
 * @param XMLDocument the DOM XML document
 */
function atkLockCheckResponse(XMLdocument)
{
  try
  {
    var root = XMLdocument.getElementsByTagName("response");
    if (root.length == 0) throw Error("Invalid lock response document.");
    var response = root.item(0);

    // success?
    if (response.getElementsByTagName("success").length > 0)
    {
      // lock lease extended, do nothing at this time
    }

    // failure?
    else if (response.getElementsByTagName("failure").length > 0)
    {
      throw Error("Lock has expired");
    }
      
    // invalid response?
    else
    {
      throw Error("Invalid lock response document.");
    }
  }  
  
  // failure
  catch (exception)
  {
    // for now ignore the exception messages
    atkLockUnlock();
  }
}

/**
 * Fetch a new lock image, which extends the lock, or if
 * the lock lease has expired triggers an error.
*/
function atkLockCheck()
{
  var sURI = '<?php echo session_url('include.php?file=atk/lock/lock.php&type=xml&stack='.$stack);?>&id=' + atkLock.theIdentifier + '&sequence=' + atkLock.theSequence;

  if (atkLock.type == 'xml')
  {
    var xmlHttp = XmlHttp.create();
    xmlHttp.open("GET", sURI, true);
    xmlHttp.onreadystatechange = function ()
    {
      if (xmlHttp.readyState == 4)
      {
        atkLockCheckResponse(xmlHttp.responseXML);
      }
    }
    
    xmlHttp.send(null);  
  }  
  
  else
  {
    var image = new Image();
    image.onerror = atkLockUnlock;
    image.src = '<?php echo session_url('include.php?file=atk/lock/lock.php&type=image&stack='.$stack);?>&id=' + atkLock.theIdentifier + '&sequence=' + atkLock.theSequence;
  }
}

/**
 * When the lock lease has expired we notify the user.
 */
function atkLockUnlock()
{
  atkLock.isLocked = false;
  if (typeof(document.images['_lock_']) != 'undefined')
    document.images['_lock_'].src='<?php echo atkconfig("atkroot");?>atk/images/lock_expired.gif';
  alert('<?=addslashes($message)?>');
}

atkLockInit('<?=$id?>');
<?php
/**
 * $Id$
 *
 * If the given lock is (still) valid we try to extend the lease time.
 *
 * @author Peter C. Verhage <peter@ibuildings.nl>
 * @version $Revision$
 */
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
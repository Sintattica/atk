<?php
/**
 * $Id$
 *
 * If the given lock is (still) valid we try to extend the lease time, if not
 * we return a 404 error not found header.
 *
 * @author Peter C. Verhage <peter@ibuildings.nl>
 * @version $Revision$
 */
global $ATK_VARS;
$id = (int)$ATK_VARS["id"];

$lock = &atkLock::getInstance();

/* extend lock lease */
if ($lock->extend($id))
{
  header("Content-type: image/png");
  $image = imagecreate(1, 1);
  $color = imagecolorallocate($image, 255, 255, 255);
  imagefill($image, 0, 0, $color);
  imagepng($image);
  imagedestroy($image);
}

/* not found header */
else header("HTTP/1.0 404 Not Found");
?>
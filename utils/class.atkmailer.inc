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
 * @license http://www.achievo.org/atk/licensing ATK Open Source License
 *
 * @version $Revision:
 * $Id$
 */

include_once(atkconfig("atkroot").'atk/ext/phpmailer/class.phpmailer.php');

/**
 * ATK mailer.
 * 
 * Simple extension of phpmailer class, to override 
 * some basic settings etc.
 * 
 * @author Peter C. Verhage <peter@ibuildings.nl>
 * @version $Revision$
 * 
 * @package atk
 * @subpackage utils
 */
class atkMailer extends phpmailer
{
  var $Mailer   = "mail";
  var $WordWrap = 75;

  /**
   * Override error handler.
   *
   * @param string $msg error message
   */
  function error_handler($msg) 
  {
    atkerror($msg);
  }   
  
  /**
   * Send.
   */
  function Send()
  {
    if (atkconfig("mail_enabled", true))
      return parent::Send();
    else return true;
  }
}
?>
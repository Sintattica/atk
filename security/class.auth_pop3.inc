<?php
/**
 * This class does authentication through a POP3 server
 *
 * @author Peter Verhage <peter@ibuildings.nl>
 * @version $Revision$
 * 
 * <b>Changes</b>:
 * $Id$
 * $Log$
 * Revision 4.3  2001/07/09 15:40:46  ivo
 * Authentication classes can now indicate whether they support md5 encoding
 * of passwords.
 *
 * Revision 4.2  2001/07/09 12:58:28  ivo
 * Small fixes to pop3 authentication.
 *
 * Revision 4.1  2001/06/18 10:24:58  peter
 * Split POP3/IMAP authentication support (mainly a new POP3 auth class).
 * Authentication and authorization can now be done by different auth classes.
 *
 */
class auth_pop3 extends auth_interface
{
  /**
   * Validate user with $user and $passwd
   * @param $user the username
   * @param $passwd the password
   * @return login true / false
   */
  function validateUser($user, $passwd)
  { 
    global $g_securityManager; // used for logging..
    
    /* if it's a virtual mail server add @<domain> to the username */
    if(atkconfig("auth_mail_virtual") == true)
      $user = $user . "@" . atkconfig("auth_mail_suffix");
 
    /* connect */
    $port = atkconfig("auth_mail_port");
    $link_id = fsockopen(atkconfig("auth_mail_server"), $port, $errno, $errstr, 30);
    if (!$link_id) 
    {
      $g_securityManager->log(1,"Error connecting to server".atkconfig("auth_mail_server").": $errstr");
      atkdebug("Error connecting to server ".atkconfig("auth_mail_server").": $errstr");
      return false;
    }

    /* authenticate */
    $void = fgets($link_id, 1000);
    fputs($link_id, "USER ".$user."\r\n");
    $void = fgets($link_id, 1000);
    fputs($link_id, "PASS ".$passwd."\r\n");
    $auth = fgets($link_id, 1000);    
    fputs($link_id, "QUIT\r\n");    
    fclose($link_id);    
    
    $g_securityManager->log(1, "pop3auth response for user $user: ".trim($auth));

    /* login ok? */
    if (!stristr($auth, "ERR")) return true;
    else return false;
  }
  
  /**
   * Dummy method...
   */   
  function getUser($user)
  {
    return array("name"=>"","level"=>-1);
  }
  
  /**
   * Dummy method...
   */   
  function getEntity($node, $action)
  {
    return array();
  }
  
  /** 
   * Dummy method...
   */   
  function getAttribEntity($node, $attrib, $mode)
  {
    return array();
  }
  
  /** 
   * pop3 can't handle md5 passwords since they must be sent to the server
   * as plain text.
   */
  function canMd5()
  {
    return false;
  }
}
?>

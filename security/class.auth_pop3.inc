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
    /* if it's a virtual mail server add @<domain> to the username */
    if(atkconfig("authentication_mail_login_type") == "vmailmgr")
      $user = $user . "@" . atkconfig("authentication_mail_suffix");
 
    /* connect */
    $port = atkconfig("authentication_mail_port") == "" ? "110" : atkconfig("authentication_mail_port");
    $link_id = fsockopen(atkconfig("authentication_mail_server"), $port, $errno, $errstr, 30);
    if (!$link_id) return false;

    /* authenticate */
    $void = fgets($link_id, 1000);
    fputs($link_id, "USER ".$user."\r\n");
    $void = fgets($link_id, 1000);
    fputs($link_id, "PASS ".$passwd."\r\n");
    $auth = fgets($link_id, 1000);    
    fputs($link_id, "QUIT\r\n");
    fclose($link_id);    

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
}
?>

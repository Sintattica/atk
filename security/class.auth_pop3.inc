<?php
/**
 * This class does authentication through a POP3 server
 *
 * @author Peter Verhage <peter@ibuildings.nl>
 * @version $Revision$
 * 
 * $Id$ 
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
    global $g_securityManager; // we need the securitymanager for logging.
    global $g_db, $g_pop3_responses; 
    
    /* if it's a virtual mail server add @<domain> to the username */
    if(atkconfig("auth_mail_virtual") == true)
      $user = $user . "@" . atkconfig("auth_mail_suffix");
      
    $server = atkconfig("auth_mail_server");
    
    // Special feature
    if ($server=="[db]")
    {
      // if server is set to [db], that means we have a different server per
      // user. We lookup in the database what server we need to call.
      $res = $g_db->getrows("SELECT auth_server 
                             FROM ".atkconfig("auth_usertable")." 
                            WHERE ".atkconfig("auth_userfield")."='".$user."'");
      if (count($res)==0)
      {
        // User not found. 
        return 0;
      }
      $server = $res[0]["auth_server"];      
    }
    
    if ($server=="")
    {
      $g_securityManager->log(1,"pop3auth error: No server specified");
      atkdebug("pop3auth error: No server specified");
      $this->m_fatalError = text("auth_no_server");
      return -1;
    }
 
    /* connect */
    $port = atkconfig("auth_mail_port");
    $link_id = fsockopen($server, $port, $errno, $errstr, 30);
    if (!$link_id) 
    {
      $g_securityManager->log(1,"pop3auth serverconnect error $server: $errstr");
      atkdebug("Error connecting to server $server: $errstr");
      $this->m_fatalError = text("auth_unable_to_connect");
      return -1;
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
    
    // search application specified pop3 responses.. 
    if (is_array($g_pop3_responses))
    {
      foreach($g_pop3_responses as $substring => $message)
      {
        if (stristr($auth, $substring)!=false)
        {
          $this->m_fatalError = $message;
          return -1;
        }
      }
    }

    /* login ok? */
    if (!stristr($auth, "ERR")) return 1;
    else return 0;
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
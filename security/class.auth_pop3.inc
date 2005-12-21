<?php
  /**
   * This file is part of the Achievo ATK distribution.
   * Detailed copyright and licensing information can be found
   * in the doc/COPYRIGHT and doc/LICENSE files which should be
   * included in the distribution.
   *
   * @package atk
   * @subpackage security
   *
   * @copyright (c)2000-2004 Ibuildings.nl BV
   * @license http://www.achievo.org/atk/licensing ATK Open Source License
   *
   * @version $Revision$
   * $Id$
   */

  /**
   * Driver for authentication using pop3.
   *
   * Does not support authorization.
   *
   * @author Ivo Jansch <ivo@achievo.org>
   * @package atk
   * @subpackage security
   *
   */
  class auth_pop3 extends auth_interface
  {
    /**
     * Validate user.
     * @param String $user the username
     * @param String $passwd the password
     * @return boolean True if login is valid, false if not.
     */
    function validateUser($user, $passwd)
    {
      if ($user=="") return AUTH_UNVERIFIED; // can't verify if we have no userid

      global $g_pop3_responses;

      /* if it's a virtual mail server add @<domain> to the username */
      if(atkconfig("auth_mail_virtual") == true)
        $user = $user . "@" . atkconfig("auth_mail_suffix");

      $server = atkconfig("auth_mail_server");

      // Special feature
      if ($server=="[db]")
      {
        // if server is set to [db], that means we have a different server per
        // user. We lookup in the database what server we need to call.
        $db = &atkGetDb();
        $res = $db->getrows("SELECT auth_server
                               FROM ".atkconfig("auth_usertable")."
                              WHERE ".atkconfig("auth_userfield")."='".$user."'");
        if (count($res)==0)
        {
          // User not found.
          return AUTH_MISMATCH;
        }
        $server = $res[0]["auth_server"];
      }

      $secMgr = &atkGetSecurityManager();

      if ($server=="")
      {
        $secMgr->log(1,"pop3auth error: No server specified");
        atkdebug("pop3auth error: No server specified");
        $this->m_fatalError = atktext("auth_no_server");
        return AUTH_ERROR;
      }

      /* connect */
      $port = atkconfig("auth_mail_port");
      $link_id = fsockopen($server, $port, $errno, $errstr, 30);
      if (!$link_id)
      {
        $secMgr->log(1,"pop3auth serverconnect error $server: $errstr");
        atkdebug("Error connecting to server $server: $errstr");
        $this->m_fatalError = atktext("auth_unable_to_connect");
        return AUTH_ERROR;
      }

      /* authenticate */
      $void = fgets($link_id, 1000);
      fputs($link_id, "USER ".$user."\r\n");
      $void = fgets($link_id, 1000);
      fputs($link_id, "PASS ".$passwd."\r\n");
      $auth = fgets($link_id, 1000);
      fputs($link_id, "QUIT\r\n");
      fclose($link_id);

      $secMgr->log(1, "pop3auth response for user $user: ".trim($auth));

      // search application specified pop3 responses..
      if (is_array($g_pop3_responses))
      {
        foreach($g_pop3_responses as $substring => $message)
        {
          if (stristr($auth, $substring)!=false)
          {
            $this->m_fatalError = $message;
            return AUTH_ERROR;
          }
        }
      }

      /* login ok? */
      if (!stristr($auth, "ERR")) return AUTH_SUCCESS;
      else return AUTH_MISMATCH;
    }

    /**
     * Pop3 can't handle md5 passwords since they must be sent to the server
     * as plain text.
     * @return boolean False
     */
    function canMd5()
    {
      return false;
    }
  }

?>
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
   * Driver for authentication using an imap server. 
   *
   * Does not support authorization.
   *
   * @author Sandy Pleyte <sandy@ibuildings.nl>
   * @package atk
   * @subpackage security
   *
   */   
  class auth_imap extends auth_interface
  {
    function validateUser($user, $passwd)
    { 
      // if it's a virtual mail server add @<domain> to the username
      if(atkconfig("auth_mail_login_type") == "vmailmgr")
      {
        $user = $user . "@" . atkconfig("auth_mail_suffix");
      }

      if (atkconfig("auth_mail_server")=="")
      {
        $this->m_fatalError = text("auth_no_server");
        return AUTH_ERROR;
      }
      
      $mailauth = @imap_open("{".atkconfig("auth_mail_server")
 		                       .":".atkconfig("auth_mail_port")."}", $user , $passwd);
       // TODO/FIXME: return AUTH_ERROR when connection fails..
      if ($mailauth == 0) 
      {
        return AUTH_MISMATCH;
      } 
      else
      {
        imap_close($mailauth);
        return AUTH_SUCCESS;
      }         
    }          
    
    function canMd5()
    {
      return false;
    }
  }

?>

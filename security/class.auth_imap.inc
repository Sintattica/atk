<?php
  /**
   * This class does authentication through a set of tables in a db
   *
   * <b>Todo's</b>   : - everything
   *
   * @author Sandy Pleyte (sandy@achievo.com)
   *
   * <b>Changes</b>:
   *    
   * $Id$
   * $Log$
   * Revision 4.3  2001/07/19 11:40:07  ivo
   * Updated authentication classes with an additional returncode for fatal
   * errors. Added a feature to pop3 authentication to handle server-specific
   * response codes.
   *
   * Revision 4.2  2001/07/09 15:40:46  ivo
   * Authentication classes can now indicate whether they support md5 encoding
   * of passwords.
   *
   * Revision 4.1  2001/06/18 10:24:58  peter
   * Split POP3/IMAP authentication support (mainly a new POP3 auth class).
   * Authentication and authorization can now be done by different auth classes.
   *
   * Revision 1.2  2001/05/21 13:38:18  ivo
   * Added a cookie that can remember your login if you close your browser.
   * (disabled by default, enable in your configfile if you need it)
   *
   * Revision 1.1  2001/05/21 13:09:56  sandy
   * added 2 validation classes, 1 for mail (POP3 / IMAP) and 1 for LDAP
   * both are untested at this moment.
   *
   */
   
   class auth_mail extends auth_interface
   {
     function validateUser($user, $passwd)
     { 
       // if it's a virtual mail server add @<domain> to the username
       if(atkconfig("authentication_mail_login_type") == "vmailmgr")
       {
         $user = $user . "@" . atkconfig("authentication_mail_suffix");
       }
 
      if (atkconfig("authentication_mail_server")=="")
      {
        $this->m_fatalError = text("auth_no_server");
        return -1;
      }
      $mailauth = imap_open("{".atkconfig("authentication_mail_server")
			     .":".atkconfig("authentication_mail_port")."}INBOX", $user , $passwd);

      // TODO/FIXME: return -1 when connection fails..
      if ($mailauth == False) 
      {
        return 0;
      } 
      else
      {
        imap_close($mailauth);
        return 1;
      }    
       
     }          
     
     function canMd5()
     {
       return false;
     }
   }

?>

<?php
  /**
   * This class does authentication through an IMAP server.
   *
   * @author Sandy Pleyte (sandy@achievo.org)
   * @version $Revision$
   *
   * $Id$
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

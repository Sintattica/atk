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
       if(atkconfig("mail_login_type") == "vmailmgr")
       {
         $user = $user . "@" . atkconfig("mail_suffix");
       }
 
      $mailauth = imap_open("{".atkconfig("mail_server")
			     .":".atkconfig("mail_port")."}INBOX", $user , $passwd);

      if ($mailauth == False) {
        return False;
      } else {
        imap_close($mailauth);
        return True;
      }    
       
     }
     
     function getUser($user)
     {
       return Array("name"=>"","level"=>-1);
     }
     
     function getEntity($node, $action)
     {
       return Array();
     }
     
     function getAttribEntity($node, $attrib, $mode)
     {
       return Array();
     }
   }

?>

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
   * Revision 1.4  2001/07/19 11:40:07  ivo
   * Updated authentication classes with an additional returncode for fatal
   * errors. Added a feature to pop3 authentication to handle server-specific
   * response codes.
   *
   * Revision 1.3  2001/07/09 15:40:46  ivo
   * Authentication classes can now indicate whether they support md5 encoding
   * of passwords.
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
   
   class auth_ldap extends auth_interface
   {
     function validateUser($user, $passwd)
     { 
     
        $ldap = ldap_connect(atkconfig("authentication_ldap_host"));
      
        // find the dn for this uid, the uid is not always in the dn
        $sri = ldap_search($ldap, atkconfig("authentication_ldap_context"), "uid=$user");
        $allValues = ldap_get_entries($ldap, $sri);
        if($allValues["count"] > 0)
        {
        	// we only care about the first dn
      	  $userDN = $allValues[0]["dn"];

      	  // generate a bogus password to pass if the user doesn't give us one 
	        // this gets around systems that are anonymous search enabled 
          if (empty($passwd)) $passwd = crypt(microtime()); 
      	  // try to bind as the user with user suplied password
      	  if (ldap_bind($ldap,$userDN, $passwd)) return True;
        }

        // dn not found or password wrong TODO/FIXME: return -1 if dn not found
       return 0;
     }
                         
     function canMd5()
     {
       return atkconfig("authentication_md5"); // ?? Is this correct? can we store passwords as md5 in ldap?
     }
   }

?>
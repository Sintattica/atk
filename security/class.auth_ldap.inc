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
   
   class auth_ldap extends auth_interface
   {
     function validateUser($user, $passwd)
     { 
     
        $ldap = ldap_connect(atkconfig("ldap_host"));
      
        // find the dn for this uid, the uid is not always in the dn
        $sri = ldap_search($ldap, atkconfig("ldap_context"), "uid=$user");
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

        // dn not found or password wrong
       return False; 
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

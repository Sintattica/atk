<?php
  /**
   * This class does authentication through an ldap server.
   *
   * @author Sandy Pleyte (sandy@achievo.org)
   * @version $Revision$
   *
   * $Id$
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
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
   * Driver for authentication using an ldap server. 
   *
   * Does not support authorization.
   *
   * @author Sandy Pleyte <sandy@achievo.org>
   * @package atk
   * @subpackage security
   *
   */
  class auth_ldap extends auth_interface
  {
    function validateUser($user, $passwd)
    { 
       $ldap = ldap_connect(atkconfig("authentication_ldap_host"));

       // find the dn for this uid, the uid is not always in the dn
       $query     = atkconfig("authentication_ldap_field")."=".$user;
       $sri       = ldap_search($ldap, atkconfig("authentication_ldap_context"), $query);
       $allValues = ldap_get_entries($ldap, $sri);

       if ($allValues["count"] > 0)
       {
         // we only care about the first dn
         $userDN = $allValues[0]["dn"];

         // generate a bogus password to pass if the user doesn't give us one
         // this gets around systems that are anonymous search enabled 
         if (empty($passwd)) $passwd = crypt(microtime()); 

         // try to bind as the user with user suplied password
         $bound  = @ldap_bind($ldap, $userDN, $passwd);
 
         if ($bound)
         {
           ldap_close($ldap);
           return AUTH_SUCCESS;
         }
         else
         {
           ldap_close($ldap);
           return AUTH_MISMATCH;
         }
       }
       else
       {
         ldap_close($ldap);
         return AUTH_MISMATCH;
       }
    }
                         
    function canMd5()
    {
      return false; // ?? Is this correct? can we store passwords as md5 in ldap?
    }
  }
?>
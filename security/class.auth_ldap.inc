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
    /**
     * Authenticate a user. 
     *
     * @param String $user The login of the user to authenticate.
     * @param String $passwd The password of the user. Note: if the canMd5 
     *                       function of an implementation returns true,      
     *                       $passwd will be passed as an md5 string.
     *
     * @return int AUTH_SUCCESS - Authentication succesful
     *             AUTH_MISMATCH - Authentication failed, wrong 
     *                             user/password combination
     *             AUTH_LOCKED - Account is locked, can not login
     *                           with current username.
     *             AUTH_ERROR - Authentication failed due to some 
     *                          error which cannot be solved by 
     *                          just trying again. If you return 
     *                          this value, you *must* also 
     *                          fill the m_fatalError variable.
     */
    function validateUser($user, $passwd)
    {
       if ($user=="") return AUTH_UNVERIFIED; // can't verify if we have no userid

       if($ldap = ldap_connect(atkconfig("auth_ldap_host")))
       {
        atkdebug("successful connection to ".atkconfig("auth_ldap_host"));
        if(atkconfig("auth_ldap_bind_tree"))
        {
          if ($bindID = @ldap_bind($ldap, atkconfig("auth_ldap_bind_dn") , atkconfig("auth_ldap_bind_pw")))
          {
            atkdebug("Succesfully bound to ". atkconfig("auth_ldap_bind_dn") . " with id: ".$bindID . " conn_id ". $ldap);
          }
          else
          {
            atkdebug("<b>Error binding to</b> ".atkconfig("auth_ldap_bind_dn") ." ". atkconfig("auth_ldap_bind_pw"));
            return AUTH_ERROR;
          }
        }

        // find the dn for this uid, the uid is not always in the dn
        $filter  = (atkconfig("auth_ldap_search_filter") != "" ? atkconfig("auth_ldap_search_filter") : "uid");
        $pattern = atkconfig("auth_ldap_context");

        // Add support for searching in multiple DN's
        if (!is_array($pattern))
        {
          $pattern = array($pattern);
        }

        foreach ($pattern AS $searchPattern)
        {
          $filterCmd = $filter."=".$user;
          $sri       = @ldap_search($ldap, $searchPattern, $filterCmd);

          if ($sri === false)
          {
            atkdebug("Invalid searchpattern: ".$searchPattern);
          }
          else
          {
            $allValues = ldap_get_entries($ldap, $sri);

            if($allValues["count"] > 0)
            {
              // we only care about the first dn
              $userDN = $allValues[0]["dn"];

      	      // generate a bogus password to pass if the user doesn't give us one
	      // this gets around systems that are anonymous search enabled
              if (empty($passwd)) $passwd = crypt(microtime());

      	      // try to bind as the user with user suplied password
      	      if (@ldap_bind($ldap, $userDN, $passwd)) return AUTH_SUCCESS;
            }
          }
        }
        atkdebug("LDAP did not successfully authenticate $user");

        // dn not found or password wrong TODO/FIXME: return -1 if dn not found
        return AUTH_MISMATCH;
       }
       else return AUTH_ERROR;
    }

    /**
     * Ldap can't handle passwords as md5
     *
     * @return boolean false
     */
    function canMd5()
    {
      return false; // ?? Is this correct? can we store passwords as md5 in ldap?
    }
  }
?>
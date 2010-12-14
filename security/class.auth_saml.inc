<?php

  /**
   * This file is part of the ATK Framework distribution.
   * Detailed copyright and licensing information can be found
   * in the doc/COPYRIGHT and doc/LICENSE files which should be 
   * included in the distribution.
   *
   * @package atk
   * @subpackage security
   *
   * @copyright (c)2010 Ivo Jansch / Egeniq
   */

  /**
   * Driver for SAML authentication, for SAML based Single Sign On systems.
   *
   * It uses the SimpleSAMLphp library for the actual SAML handling
   * To use this driver you need to specify a number of config options:
   *
   * Required:
   *
   * $config_auth_saml_simplesaml_path = path to your simplesamlphp
   * installation, relative to your application, with trailing slash.
   * Example:
   * $config_auth_saml_simplesaml_path = "../simplesamlphp/";
   *
   * Optional:
   *
   * $config_auth_userid_field = "uid"; The name of the field in the SSO system that 
   * contains the id of the logged in user. Defaults to 'uid'.
   *
   * $config_auth_saml_authsource = "default-sp"; The name of the authsource in 
   * simplesamlphp. Defaults to 'default-sp'.
   * 
   * @author Ivo Jansch        <ivo@egeniq.com>
   * @package atk
   * @subpackage security
   *
   */   
class auth_saml extends auth_interface
{
    /**
     * @internal This value is static because atkSecurityManager has 2 instances of this class
     * if authorization and authentication are both saml. 
     */
    protected static $_samlUser = NULL;

    /**
     * Authenticate a user. 
     * $user and $passwd are ignored by the saml plugin since we perform 
     * login elsewhere (in the SSO system).
     * Note: if the user is not logged in in the SSO system, this method
     * will never return as we redirect to the SSO system.
     *
     * @return int AUTH_SUCCESS - Authentication succesful
     */
    public function validateUser($user, $passwd)
    {       
        $simplesamlPath = atkconfig("auth_saml_simplesaml_path");
        require_once(atkconfig('atkroot').$simplesamlPath.'lib/_autoload.php');
        $as = new SimpleSAML_Auth_Simple(atkconfig('auth_saml_authsource', 'default-sp'));

        $as->requireAuth();

        $logoutUrl = $as->getLogoutURL();

        $attributes = $as->getAttributes();

        // We transport the logoutUrl as part of the user session data so we can logout at any
        // time later when the user clicks the logout button.
        self::$_samlUser = array("name" => $attributes[atkconfig("auth_userid_field", "uid")][0],
                                 "attributes" => $attributes,
                                 "_logoutUrl" => $logoutUrl);

        return AUTH_SUCCESS;
    }          

    /**
     * Logout through the SSO system. The default logout handler destroys the session, but
     * we can't do that, since simplesamlphp needs the session to be able to perform the 
     * logout.
     */
    public function logout($user)
    {
        header("Location: ".$user["_logoutUrl"]);
    }
     
    /**
     * Does this authentication method support md5 encoding of passwords?
     *
     * @return boolean false
     */
    public function canMd5()
    {
       return false;
    }

    /** 
     * Retrieve the user. We ignore $user since a saml user is managed in a 
     * separate system. We return the user as we retrieved it from the SSO 
     * system during login in validateUser.
     * All the user data we retrieved from the SSO system is present as sub
     * array $user["attributes"];
     */
    public function getUser($user)
    {
        return self::$_samlUser;
    }
}


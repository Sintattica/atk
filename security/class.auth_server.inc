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
   * @copyright (c)2005 Ibuildings.nl BV
   * @license http://www.achievo.org/atk/licensing ATK Open Source License
   *
   * @version $Revision: 6280 $
   * $Id$
   */
   
  /**
   * Driver for external authentication, such as Apache .htaccess files.
   *
   * With this driver, the webserver is supposed to handle the authentication. 
   * Use with care. ATK will not validate anything so if the server
   * authentication is not set-up properly, this may be a security risk
   * The only check ATK makes is whether the webserver has put a valid
   * username in $_SERVER['PHP_AUTH_USER'].
   *
   * @author Ivo Jansch        <ivo@achievo.org>
   * @author Gabriele Gallacci <infouser@gallacci.com>
   * @package atk
   * @subpackage security
   *
   */   
   class auth_server extends auth_interface
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
       if ($_SERVER['PHP_AUTH_USER']) { return AUTH_SUCCESS; } else { return AUTH_MISMATCH; }
     }          
     
     /**
     * Does this authentication method support md5 encoding of passwords?
     *
     * @return boolean false
     */
     function canMd5()
     {
       return false;
     }
   }

?>
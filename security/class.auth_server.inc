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
   * @version $Revision$
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
     function validateUser($user, $passwd)
     {       
       if ($_SERVER['PHP_AUTH_USER']) { return AUTH_SUCCESS; } else { return AUTH_MISMATCH; }
     }          
     
     function canMd5()
     {
       return false;
     }
   }

?>
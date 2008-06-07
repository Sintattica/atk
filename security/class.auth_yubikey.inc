<?php
  /**
   * @package atk
   * @subpackage security
   *
   * @copyright (c)2008 Gerco Dries
   * @license
   *
   * @version $Revision: 1.0 $
   */

  /**
   * Driver for authentication using yubikeys.
   *
   * Does not support authorization.
   * To get this working you need to install the PEAR module from Yubico
   * and request an API key.
   *
   * @author Gerco Dries <gerco@gdries.com>
   * @package atk
   * @subpackage security
   *
   */

  require_once('Yubico.php');

  class auth_yubikey extends auth_interface
  {
    /**
     * Validate user.
     * @param String $user the username
     * @param String $otp the Yubikey OTP
     * @return boolean True if login is valid, false if not.
     */
    function validateUser($user, $otp)
    {
      if ($user=="") return AUTH_UNVERIFIED; // can't verify if we have no userid

      // Find the Yubikey identity for the user in the database
      $db = &atkGetDb();
      $res = $db->getrows("SELECT ".atkconfig("auth_yubikeyfield","yubikey")."
   		           FROM ".atkconfig("auth_usertable")."
			           WHERE ".atkconfig("auth_userfield")."='".$user."'");

      if (count($res)==0)
      {
        // User not found.
	      atkdebug("auth_yubikey::validateUser() -> error: User $user not found");
        return AUTH_MISMATCH;
      }

      $keyidentity = $res[0]["auth_yubikey"];
      $secMgr = &atkGetSecurityManager();

      if ($keyidentity=="")
      {
        $secMgr->log(1,"Yubikey_auth error: No yubikey identity for this user");
        atkdebug("auth_yubikey::validateUser() -> error: No yubikey identity for this user");
        $this->m_fatalError = "auth_no_yubikey";
        return AUTH_ERROR;
      }

      // Check the Yubikey identity
      if($keyidentity != substr($otp, 0, 12))
      {
        atkdebug("auth_yubikey::validateUser() -> error: identity does not match OTP");
        return AUTH_MISMATCH;
      }

      // Perform call to Yubico
      $yubi = &new Auth_Yubico(atkconfig("auth_yubikey_id", atkconfig("auth_yubikey_key")));
      $auth = $yubi->verify($otp);
      if (PEAR::isError($auth))
      {
        $this->m_fatalError = $auth->getMessage();
        return AUTH_MISMATCH;
      }
      else
      {
	      return AUTH_SUCCESS;
      }

      return AUTH_MISMATCH;
    }

    /**
     * Yubikey OTPs must not be md5()ed, it would be pointless and also break the scheme
     * @return boolean False
     */
    function canMd5()
    {
      return false;
    }
  }

?>
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
   * @copyright (c)2004 Ibuildings.nl BV
   * @license http://www.achievo.org/atk/licensing ATK Open Source License
   *
   */

  /**
   * Class for encrypting and decrypting data with the openssl algorithm
   * This class uses the functions openssl_public_encrypt and openssl_private_decrypt
   * which are experimental function. see for more details: http://nl3.php.net/manual/en/function.openssl-public-encrypt.php
   *
   * @author Mark Baaijens <mark@ibuildings.nl>
   *
   * @package atk
   * @subpackage security
   *
   */
  class atkOpenSSLEncryption extends atkEncryption
  {
    var $config_default;
    var $config_nokey;

    /**
     * The constructor of this class makes two configurations which are used by other functions
     */
    function atkOpenSSLEncryption()
    {
      $this->config_default = array('config'=>"c:\php\openssl\openssl.cnf");
      $this->config_nokey = array_merge($this->config_default,array('encrypt_key'=>false));
    }


    /**
     * The encryption method for encrypting data with the openssl algorithm
     *
     * @param mixed $input  the data we want to encrypt
     * @param mixed $key    the key we want to encrypt the data with
     * @return mixed        the encrypted data
     */
    function encrypt($input, $key)
    {
      $keys = $this->getKeys($key);
    
      $key = openssl_get_publickey($keys['public']);

      if(!$key){
        atkerror("atkOpenSSLEncryption::encrypt << not a valid key passed");
        return "";
      }

      openssl_public_encrypt($input,$encrypted,$key);
      
      //atkerror("encrypt $input with ".$keys['public']." to $encrypted");

      return $encrypted;
    }

    /**
     * The decryption method for decrypting data with the bajus algorithm
     * @param mixed $input  the data we want to encrypt
     * @param mixed $key    the key we want to encrypt the data with
     * @return mixed        the encrypted data
     */
    function decrypt($input, $key)
    {
      $keys = $this->getKeys($key);

      $key = openssl_get_privatekey($keys['private']);

      if(!$key){
        atkerror("atkOpenSSLEncryption::decrypt << not a valid key passed");
        return "";
      }
      
      openssl_private_decrypt($input,$decrypted,$key);
      
      //atkerror("decrypt $input with ".$keys['public']." to $decrypted");

      return $decrypted;
    }

    /**
     * This function copies a private key with a new password
     *
     * @param string $key             The private key which must be copied
     * @param string $privkeypass     The passphrase of the key
     * @param string $newprivkeypass  The passphrase of the new key
     * @return string                 The new key
     */
    function copyPrivateKey($key,$privkeypass,$newprivkeypass)
    {
      if($privkeypass != "")
        $priv = openssl_get_privatekey($key,$privkeypass);
      else
        $priv = openssl_get_privatekey($key);
        
      if($priv && $newprivkeypass!="")
        openssl_pkey_export($priv,$newprivatekey,$newprivkeypass,$this->config_default);
      else if($priv)
        openssl_pkey_export($priv,$newprivatekey,"",$this->config_nokey);

      return $newprivatekey;
    }
    
    /**
      * This function makes a public key, a private key and a certificate from a single key
      *
      * @param string $key   an key on which the information is based
      * @returns array       a array with 'privatekey','publickey' and
      */
    function getNewKeys($privkeypass)
    {
      $dn = array(
       "countryName" => "NL",
       "stateOrProvinceName" => "Zeeland",
       "localityName" => "Vlissingen",
       "organizationName" => "Ibuildings.nl",
       "organizationalUnitName" => "IBS-Vlissingen",
       "commonName" => "Mark Baaijens",
       "emailAddress" => "mark@ibuildings.nl"
      );

      //generate a private key
      $privkey = openssl_pkey_new($this->config_default);
      //generate a certificate signing request
      $csr = openssl_csr_new($dn,$privkey,$this->config_default);
      //This creates a self-signed cert that is valid for 365 days
      $sscert = openssl_csr_sign($csr,null,$privkey,365,$this->config_default);
      //now we store the public key, private key and the certificate in variables
      //private key is encrypted with privkeypass
      $result = array();
      openssl_x509_export($sscert, $result['public']);
      openssl_pkey_export($privkey, $result['private'], $privkeypass,$this->config_default);

      return $result;
    }
    
    /**
     * Creates a random key for tableencryption
     * This implentation of this function returns a public and private key pair together in one string
     */
    function getRandomKey($pass)
    {
      $keys = $this->getNewKeys($pass);
      return $this->putKeys($keys);
    }
    
    /**
     * Decryptionmethod for a key.
     * This implementation get the private key with a password and export it without password
     * @param string key      The encrypted key
     * @param string password The password to decrypt de key
     * @return string         The decrypted key
     */
    function decryptKey($key,$pass)
    {
      $keys = $this->getKeys($key);
      $keys['private'] = $this->copyPrivateKey($keys['private'],$pass,"");
      return $this->putKeys($keys);
    }
    
    /**
     * Encryptionmethod for a key.
     * This implementation get the private key without a password and export is with a password
     * @param string key      The decrypted key
     * @param string password The password to encrypt de key
     * @return string         The encrypted key
     */
    function encryptKey($key,$pass)
    {
      $keys = $this->getKeys($key);
      $keys['private'] = $this->copyPrivateKey($keys['private'],"",$pass);
      return $this->putKeys($keys);
    }
    
    /**
     * This is a help function to get the private and the public key from one string in an array
     * @param string key    The string containing the private, and the public key
     * @return array        The privatekey (private) and the public key (public)
     */
    function getKeys($key)
    {
      //the string have the following format:
      //-----BEGIN RSA PRIVATE KEY-----...-----END RSA PRIVATE KEY----------BEGIN CERTIFICATE-----...-----END CERTIFICATE-----

      //break this in
      //-----BEGIN RSA PRIVATE KEY-----...-----END RSA PRIVATE KEY-----
      //and
      //-----BEGIN CERTIFICATE-----...-----END CERTIFICATE-----
      if(($begin = strpos($key,"-----BEGIN CERTIFICATE-----")) != false)
      {
        $keys['private'] = substr($key,0,$begin);
        $keys['public'] = substr($key,$begin,strlen($key)-$begin);
        return $keys;
      }
    }
    
    /**
     * This help function make one string from an array with private an public key in it
     * @param array keys  The private and public keys in one array
     * @return string     The private and public keys in one string
     */
    function putKeys($keys)
    {
      return $keys['private'].$keys['public'];
    }
    
    
  }
?>

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
   * Base class for all ATK encryption methods
   *
   * @author Mark Baaijens <mark@ibuildings.nl>
   *
   * @package atk
   * @subpackage security
   *
   */
  class atkEncryption
  {
    /**
     * Get function for encryption
     *
     * Gets a new instance of an encryption class with the type we passed 
     * along
     * @param string $type The type of encryption we want, 
     *                     defaults to $config)_encryption_defaultmethod
     * @return obj the node with which to encrypt or decrypt your data
     */
    function &getEncryption($type="")
    {
      if ($type=="") $type = atkconfig("encryption_defaultmethod");
      $encryptionclass = "atk".strtolower($type)."encryption";
      
      if (atkimport("atk.utils.".$encryptionclass))
      {
        return new $encryptionclass();
      }
      else return $this;
    }

    /**
     * Encryptionmethod, encrypts your input with a key
     * @param mixed $input  the data we want to encrypt
     * @param mixed $key    the key we want to encrypt the data with
     * @return mixed        the encrypted data
     */
    function encrypt($input, $key)
    {
      // dummy implementation
      return $input;
    }
    
    /**
     * Decryptionmethod, decrypts your input with a key
     * @param mixed $input  the encrypted data that we want to decrypt
     * @param mixed $key    the key with which to decrypt the data
     * @return mixed        the decrypted data
     */  
    function decrypt($input, $key)
    {
      // dummy implementation
      return $input;
    }
    
    /**
     * Creates a hash of a random number and returns the amount of characters you pass along (<=32)
     * or the entire (32 characters) string
     * @param int $length the amount of characters we want, can't be more than 32
     * @return string the random string
     */
    function getRandomString($length=null)
    {
      $str = md5(rand(1,100));
    
      if($length > 32)
        return $str;
    
      $begin = rand(0,32-$length);
      return substr($str,$begin,$length);
    }
  }
?>

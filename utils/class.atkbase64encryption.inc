<?php
  /**
   * This file is part of the Achievo ATK distribution.
   * Detailed copyright and licensing information can be found
   * in the doc/COPYRIGHT and doc/LICENSE files which should be 
   * included in the distribution.
   * 
   * @package atk
   * @subpackage utils
   *
   * @copyright (c)2000-2004 Ibuildings.nl BV
   * @license http://www.achievo.org/atk/licensing ATK Open Source License
   *
   */

  /**
   * Class for encrypting and decrypting data with the base64 algorithm
   *
   * @author Mark Baaijens <mark@ibuildings.nl>
   *
   * @package atk
   * @subpackage utils
   *
   */
  class atkBase64Encryption extends atkEncryption
  {
    /**
     * The encryption method for encrypting data with the base64 algorithm
     * @param mixed $input  the data we want to encrypt
     * @param mixed $key    the key we want to encrypt the data with
     * @return mixed        the encrypted data
     */
    function encrypt($input, $key)
    {
      $key = md5($key);
      $number = ord($key{6}) % 5 + 1;
    
      $output = $input;
    
      for($i=0; $i<$number; $i++)
        $output = base64_encode($output);
      
      return $output;
    }

    /**
     * The decryption method for decrypting data with the base64 algorithm
     * @param mixed $input  the data we want to encrypt
     * @param mixed $key    the key we want to encrypt the data with
     * @return mixed        the encrypted data
     */
    function decrypt($input, $key)
    {
      $key = md5($key);
      $number = ord($key{6}) % 5 + 1;

      $output = $input;

      for($i=0; $i<$number; $i++)
        $output = base64_decode($output);

      return $output;
    }
  }
?>
<?php namespace Sintattica\Atk\Security\Encryption;

/**
 * Class for encrypting and decrypting data with the base64 algorithm
 *
 * This isn't strong encryption. It is mainly used for testing purposes.
 *
 * @author Mark Baaijens <mark@ibuildings.nl>
 *
 * @package atk
 * @subpackage security
 *
 */
class Base64Encryption extends Encryption
{

    /**
     * The encryption method for encrypting data with the base64 algorithm
     * @param mixed $input the data we want to encrypt
     * @param mixed $key the key we want to encrypt the data with
     * @return mixed        the encrypted data
     */
    function encrypt($input, $key)
    {
        $key = md5($key);
        $number = ord($key{6}) % 5 + 1;

        $output = $input;

        for ($i = 0; $i < $number; $i++) {
            $output = base64_encode($output);
        }

        return $output;
    }

    /**
     * The decryption method for decrypting data with the base64 algorithm
     * @param mixed $input the data we want to encrypt
     * @param mixed $key the key we want to encrypt the data with
     * @return mixed        the encrypted data
     */
    function decrypt($input, $key)
    {
        $key = md5($key);
        $number = ord($key{6}) % 5 + 1;

        $output = $input;

        for ($i = 0; $i < $number; $i++) {
            $output = base64_decode($output);
        }

        return $output;
    }

    /**
     * Decryptionmethod for a key. This implementation decrypt the key with de base64 algoritm
     * @param string $key The encrypted key
     * @param string $pass The password to decrypt de key
     * @return string      The decrypted key
     */
    function decryptKey($key, $pass)
    {
        return $this->decrypt($key, $pass);
    }

    /**
     * Encryptionmethod for a key. This implementation encrypt the key with de base64 algoritm
     * @param string $key The decrypted key
     * @param string $pass The password to encrypt de key
     * @return string      The encrypted key
     */
    function encryptKey($key, $pass)
    {
        return $this->encrypt($key, $pass);
    }

}


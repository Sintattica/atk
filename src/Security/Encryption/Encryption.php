<?php

namespace Sintattica\Atk\Security\Encryption;

use Sintattica\Atk\Core\Config;

/**
 * Base class for all ATK encryption methods.
 *
 * @todo Currently, 2 weak encryption implementations are available for
 *       testing purposes. Strong encryption using the mcrypt() php
 *       extension is yet to be implemented.
 *
 * @author Mark Baaijens <mark@ibuildings.nl>
 */
class Encryption
{
    /**
     * Get function for encryption.
     *
     * Gets a new instance of an encryption class with the type we passed
     * along
     *
     * @param string $type The type of encryption we want,
     *                     defaults to $config)_encryption_defaultmethod
     *
     * @return Encryption the node with which to encrypt or decrypt your data
     */
    public function getEncryption($type = '')
    {
        if ($type == '') {
            $type = Config::getGlobal('encryption_defaultmethod');
        }
        $encryptionclass = $type.'Encryption';
        if (class_exists($encryptionclass)) {
            return new $encryptionclass();
        } else {
            return $this;
        }
    }

    /**
     * Encryptionmethod, encrypts your input with a key.
     *
     * @param mixed $input the data we want to encrypt
     * @param mixed $key the key we want to encrypt the data with
     *
     * @return mixed the encrypted data
     */
    public function encrypt($input, $key)
    {
        // dummy implementation
        return $input;
    }

    /**
     * Decryptionmethod, decrypts your input with a key.
     *
     * @param mixed $input the encrypted data that we want to decrypt
     * @param mixed $key the key with which to decrypt the data
     *
     * @return mixed the decrypted data
     */
    public function decrypt($input, $key)
    {
        // dummy implementation
        return $input;
    }

    /**
     * Decryptionmethod for a key. This implementation returns simple the input.
     *
     * @param string $key The encrypted key
     * @param string $pass The password to decrypt de key
     *
     * @return string The decrypted key
     */
    public function decryptKey($key, $pass)
    {
        // dummy implementation
        return $key;
    }

    /**
     * Encryptionmethod for a key. This implementation returns simple the input.
     *
     * @param string $key The decrypted key
     * @param string $pass The password to encrypt de key
     *
     * @return string The encrypted key
     */
    public function encryptKey($key, $pass)
    {
        //dummy implementation
        return $key;
    }

    /**
     * Creates a hash of a random number and returns the amount of characters you pass along (<=32)
     * or the entire (32 characters) string.
     *
     * @param int $length the amount of characters we want, can't be more than 32
     *
     * @return string the random string
     */
    public function getRandomString($length = null)
    {
        $str = md5(rand(1, 100));

        if ($length > 32) {
            return $str;
        }

        $begin = rand(0, 32 - $length);

        return substr($str, $begin, $length);
    }

    /**
     * Creates a random key for tableencryption
     * The default implementation of this function returns a string with 6 random characters.
     *
     * @param string $pass This implementation does nothing with this param
     *
     * @return string A random key
     */
    public function getRandomKey($pass)
    {
        return $this->getRandomString(6);
    }

    /**
     * For use of strimslashes function
     * This implementation does nothing.
     *
     * @param string $value The original string
     *
     * @return string The original string
     * */
    public function stripbackslashes($value)
    {
        return $value;
    }

    /**
     * For use of strimslashes function
     * This implementation does nothing.
     *
     * @param string $value The original string
     *
     * @return string The original string
     * */
    public function addbackslashes($value)
    {
        return $value;
    }
}

<?php namespace Sintattica\Atk\Security\Encryption;

/**
 * Class for encrypting and decrypting data with the bajus algorithm
 *
 * @author Mark Baaijens <mark@ibuildings.nl>
 *
 * @package atk
 * @subpackage security
 *
 */
class BajusEncryption extends Encryption
{

    /**
     * The encryption method for encrypting data with the bajus algorithm
     *
     * This isn't strong encryption, it is meant mainly for testing
     * purposes.
     * @param mixed $input the data we want to encrypt
     * @param mixed $key the key we want to encrypt the data with
     * @return mixed        the encrypted data
     */
    public function encrypt($input, $key)
    {
        $key = md5($key);

        for ($i = 0; $i < strlen($input); $i++) {
            $char = substr($input, $i, 1);
            $keychar = substr($key, $i, 1);
            $charvalue = ord($char) + ord($keychar);
            if ($charvalue > 255) {
                $charvalue -= 255;
            }
            $output .= chr($charvalue);
        }

        return $output;
    }

    /**
     * The decryption method for decrypting data with the bajus algorithm
     * @param mixed $input the data we want to encrypt
     * @param mixed $key the key we want to encrypt the data with
     * @return mixed        the encrypted data
     */
    public function decrypt($input, $key)
    {
        $key = md5($key);

        for ($i = 0; $i < strlen($input); $i++) {
            $char = substr($input, $i, 1);
            $keychar = substr($key, $i, 1);
            $charvalue = ord($char) - ord($keychar);
            if ($charvalue < 0) {
                $charvalue += 255;
            }
            $output .= chr($charvalue);
        }

        return $output;
    }

    /**
     * Decryptionmethod for a key. This implementation decrypt the key with de bajus algoritm
     * @param string $key The encrypted key
     * @param string $pass The password to decrypt de key
     * @return string      The decrypted key
     */
    public function decryptKey($key, $pass)
    {
        return $this->decrypt($key, $pass);
    }

    /**
     * Encryptionmethod for a key. This implementation encrypt the key with de bajus algoritm
     * @param string $key The decrypted key
     * @param string $pass The password to encrypt de key
     * @return string      The encrypted key
     */
    public function encryptKey($key, $pass)
    {
        return $this->encrypt($key, $pass);
    }
}

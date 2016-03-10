<?php namespace Sintattica\Atk\Utils;

use Sintattica\Atk\Core\Tools;

/**
 * atkIpUtils class. Contains static methods to allow numeric and string ip validation
 * and conversion.
 *
 * @author guido <guido@ibuildings.nl>
 * @package atk
 * @subpackage utils
 */
class IpUtils
{

    /**
     * Checks if the given ip (string or long numeric) is a valid ip
     *
     * A string will be valid if it contains 4 integer values between 0 and 255, glued together by dots (0.1.2.3).
     * A number will be valid if it is between 0 and 4294967295.
     *
     * @static This function may be used statically
     * @param mixed $ip String or long numeric.
     * @return boolean True if the ip is valid, False if not.
     */
    public function ipValidate($ip)
    {
        if (is_numeric($ip)) {
            return ($ip >= 0 && $ip <= 4294967295 && !is_null($ip));
        } elseif (is_string($ip)) {
            $num = '(\d|[1-9]\d|1\d\d|2[0-4]\d|25[0-5])';

            return (preg_match("/^$num\\.$num\\.$num\\.$num$/", $ip, $matches) > 0);
        }

        return false;
    }

    /**
     * Converts an ip (in number or string format) to a string
     *
     * The supplied ip must be a valid ip. If the given ip is not
     * valid, then atkerror will be called.
     *
     * @static This function may be used statically
     * @param mixed $ip String or long numeric IP address.
     * @return boolean True if the ip is valid, False if not.
     */
    public static function ipStringFormat($ip)
    {
        if (!IpUtils::ipValidate($ip)) {
            Tools::atkdebug("IpUtils::ipStringFormat() Invalid ip given");

            return null;
        }
        $long = is_numeric($ip) ? $ip : IpUtils::ipLongFormat($ip);
        $string = "";
        for ($i = 3; $i >= 0; $i--) {
            $string .= (int)($long / pow(256, $i));
            $long -= (int)($long / pow(256, $i)) * pow(256, $i);
            if ($i > 0) {
                $string .= ".";
            }
        }

        return $string;
    }

    /**
     * Converts an ip (in number or string format) to a long number
     *
     * The supplied ip must be a valid ip. If the given ip is not
     * valid, then atkerror will be called.
     *
     * @static This function may be used statically
     * @param mixed $ip String or long numeric IP address.
     * @return boolean True if the ip is valid, False if not.
     */
    public static function ipLongFormat($ip)
    {
        if (!IpUtils::ipValidate($ip)) {
            Tools::atkdebug("IpUtils::ipLongFormat() Invalid ip given");

            return null;
        }
        if (is_numeric($ip)) {
            return $ip;
        }
        $array = explode(".", $ip);

        return $array[3] + 256 * $array[2] + 256 * 256 * $array[1] + 256 * 256 * 256 * $array[0];
    }
}

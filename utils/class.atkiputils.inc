<?php

  class atkIpUtils
  {

    function ipValidate($ip)
    {
      if (is_numeric($ip))
      {
        return ($ip >= 0 && $ip <= 4294967295 && !is_null($ip));
      }
      elseif (is_string($ip))
      {
        $num='(\d|[1-9]\d|1\d\d|2[0-4]\d|25[0-5])';
        return (preg_match("/^$num\\.$num\\.$num\\.$num$/", $ip, $matches) > 0);
      }
      return false;
    }

    function ipStringFormat($ip)
    {
      if (!atkIpUtils::ipValidate($ip))
      {
        atkError("atkIpUtils::ipStringFormat() Invalid ip given");
        return null;
      }
      $long = is_numeric($ip) ? $ip : atkIpUtils::ipLongFormat($ip);
      $string = "";
      for ($i=3;$i>=0;$i--)
      {
        $string .= (int)($long / pow(256, $i));
        $long -= (int)($long / pow(256, $i)) * pow(256, $i);
        if ($i > 0)
          $string .= ".";
      }
      return $string;
    }

    function ipLongFormat($ip)
    {
      if (!atkIpUtils::ipValidate($ip))
      {
        atkError("atkIpUtils::ipLongFormat() Invalid ip given");
        return null;
      }
      if (is_numeric($ip)) return $ip;
      $array = explode(".",$ip);
      return $array[3]+256*$array[2]+256*256*$array[1]+256*256*256*$array[0];
    }

  }
?>
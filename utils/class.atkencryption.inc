<?php

class atkEncryption
{
  function &getEncryption($type)
  {
    $encryptionclass = "atk".strtolower($type)."encryption";
    if (atkimport("atk.utils.".$encryptionclass))
    {
      return new $encryptionclass();
    }
    else return $this;
  }
  
  function getRandomString($length)
  {
    $str = md5(rand(1,100));
    
    if($length > 32)
      return $str;
    
    $begin = rand(0,32-$length);
    return substr($str,$begin,$length);
  }
  
  function encrypt($input, $key)
  {
    return $input;
  }
  
  function decrypt($input, $key)
  {
    return $input;
  }
}

?>

<?php

class atkBase64Encryption extends atkEncryption
{
  function encrypt($input, $key)
  {
    $key = md5($key);
    $number = ord($key{6}) % 5 + 1;
    
    $output = $input;
    
    for($i=0; $i<$number; $i++)
      $output = base64_encode($output);
      
    return $output;
  }

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

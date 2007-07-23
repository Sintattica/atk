<?php

  function smarty_modifier_atkvardump($data,$name='')
  {
    atk_var_dump($data,$name);
    return $data;
  }

?>
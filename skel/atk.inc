<?php
  /**
   * ATK Include file.. If you strip out stuff you don't need, you 
   * might have some performance improvement.
   * Somethings can't be stripped out.
   */
   
   // This variable indicates where to get all the includes. Usually, this is 
   // the current pc.
   if ($config_atkroot =="")  $config_atkroot = "./";      
  
  // Atknode is required. Don't remove it.
  include_once($config_atkroot."atk/class.atknode.inc");
   
?>
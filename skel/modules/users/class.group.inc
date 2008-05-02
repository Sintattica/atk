<?php

class group extends atkMetaNode
{
  protected $descriptor = '[name]';

  public static function meta($policy)
  {
    $policy->add('accessrights', 'atkProfileAttribute', array(), AF_HIDE_ADD|AF_HIDE_LIST|AF_BLANK_LABEL);
  }

}

?>
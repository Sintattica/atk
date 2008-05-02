<?php

atkimport('modules.users.user');

class customer extends user
{

  public static function meta($policy)
  {
    parent::meta($policy);

    $policy->addFlag('type_id', AF_HIDE|AF_FORCE_LOAD);
    $policy->setDefaultValue('type_id', 2);
  }

  public function postMeta()
  {
    parent::postMeta();

    $this->addFilter("user.type_id = 2");
  }

}

?>
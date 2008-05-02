<?php

atkimport('modules.users.user');

class supplier extends user
{

  public static function meta($policy)
  {
    parent::meta($policy);

    $policy->addFlag('type_id', AF_HIDE|AF_FORCE_LOAD);
    $policy->setDefaultValue('type_id', 3);
  }

  public function postMeta()
  {
    parent::postMeta();

    $this->addFilter("user.type_id = 3");
  }

}

?>
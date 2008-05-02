<?php

atkimport('modules.users.user');

class employee extends user
{

  public static function meta($policy)
  {
    /* @var $policy atkmetapolicy */

    parent::meta($policy);

    // remove excludes
    $policy->setExcludes(array());

    $policy->addFlags('address', 'zipcode', 'city', 'state', 'country', AF_HIDE_ADD|AF_HIDE_LIST);
    $policy->addFlag('type_id', AF_HIDE|AF_FORCE_LOAD);
    $policy->setDefaultValue('type_id', 1);
  }

  public function postMeta()
  {
    parent::postMeta();

    $this->addFilter("user.type_id = 1");
  }

}

?>
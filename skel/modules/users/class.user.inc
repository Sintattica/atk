<?php

class User extends atkMetaNode
{
  protected $descriptor = '[name]';

  public static function meta($policy)
  {
    /* @var $policy atkmetapolicy */
    // basic user node does not contain these extra fields
    $policy->setExcludes('address', 'zipcode', 'city', 'state', 'country', 'functionlevel_id', 'department_id');

    $policy->addFlags('title', 'initials', 'phonenumber', 'cellular', 'faxnumber', 'jobtitle', 'remark', 'birthdate', 'status', 'language', AF_HIDE_ADD|AF_HIDE_LIST);

    $policy->hasOne('usertype', array('source' => 'type_id'));
    $policy->hasOne('group');
    $policy->hasOne('status');
    $policy->hasOne('language');
    $policy->hasOne('functionlevel');
    $policy->hasOne('department');
  }
}

?>
<?php
  /**
   * Node for business card fields
   * 
   * @package businesscards
   * @subpackage -
   * 
   * @author Jeroen van Sluijs <jeroenvs@ibuildings.nl>
   * 
   * @copyright (c) 2008 Ibuildings.nl BV
   * @version $Revision: 1.0 $
   */
  
  atkimport("atk.atkmetanode");
  
  class bc_fields extends atkMetaNode
  {
    var $table = "bc_fields";
    var $flags = NF_NO_ADD;
    
    public function meta(&$policy)
    {
      $policy->setOrder("page_nr","field_nr","field_blockname","field_desc","field_multiline","obligatory","additional_informatie");
      $policy->setType('field_multiline', 'atkBoolAttribute');
      $policy->setType('obligatory', 'atkBoolAttribute');
      
      $policy->setFlags('page_nr', 'field_nr', 'field_blockname', 'field_multiline', 'field_desc', AF_READONLY);
      $policy->setFlags('business_card_id', AF_HIDE);
    }
  }
?>
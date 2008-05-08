<?php
  /**
   * Module for business cards
   * 
   * @package businesscards
   * @subpackage -
   * 
   * @author Jeroen van Sluijs <jeroenvs@ibuildings.nl>
   * 
   * @copyright (c) 2008 Ibuildings.nl BV
   * @version $Revision: 1.0 $
   */
  
  class mod_businesscards extends atkModule 
  {
    function getNodes()
    {
      /* register nodes */
      registerNode('businesscards.businesscards', array('admin', 'add', 'edit', 'delete'));
      registerNode('businesscards.bc_fields', array('admin', 'add', 'edit', 'delete'));
    }
  
    function getMenuItems()
    {
      /**
       * The next line adds a submenu called 'users' to the main menu.
       */
      $this->menuitem('businesscards');
      $this->menuitem('businesscards', dispatch_url('businesscards.businesscards', 'admin'), "businesscards", array("businesscards.businesscard",  "admin"));
    }    
//    
//    function users_cmsusers_modifier(&$node)
//    {
//      userelation("atkmanytoonerelation");
//      $node->add(new atkManyToOneRelation("cms_customer_id", 
//                                          "businesscards.cms_customers", 
//                                          AF_OBLIGATORY));
//                                          
//      useattrib("atkboolattribute");
//      $node->add(new atkBoolAttribute("csv_upload_allowed"));
//    }    
//    
//    function users_extranetusers_modifier(&$node)
//    {
//      userelation("atkmanytoonerelation");
//      $node->add(new atkManyToOneRelation("business_card_company_id", 
//                                          "businesscards.companies", 
//                                          AF_OBLIGATORY));
//    }        
  }
?>
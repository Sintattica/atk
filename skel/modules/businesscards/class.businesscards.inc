<?php
  /**
   * Node for business cards
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
  atkimport("modules.businesscards.utils.pdf");
  
  class businesscards extends atkMetaNode
  {
    var $table = "bc_business_cards"; 
    var $flags = NF_MULTI_RECORD_ACTIONS;   
    
    /**
     * Each businesscard belongs to a company.
     * 
     * Only usertype 'employee' can:
     *  - see businesscards of all companies
     *  - change companyname
     *
     * @param atkMetaPolicy $policy
     */
    public function meta(&$policy)
    {      
      $user = &atkGetUser();
      if (in_array($user['type_id'], array('2','3'))) // 2=customer, 3=supplier
      {
        $company_id = $user['company_id'];
        atkdebug("businesscards::meta | customer or supplier logged in (usertype_id = {$user['type_id']}) | filter on company_id = {$company_id}");
        
        $policy->setNodeFilter("company_id={$company_id}");
        $policy->hasOne("users.company", array('source'=>'company_id', 'dest'=>'id', 'filter'=>"id={$company_id}"), array(), AF_OBLIGATORY|AF_READONLY);
      }
      else 
      {
        $policy->hasOne("users.company", array('source'=>'company_id', 'dest'=>'id'));
      }
            
      $policy->hasMany("bc_fields", array('dest'=>'business_card_id'), array(), AF_CASCADE_DELETE);
      
      $policy->setType('business_card_pdf_path', 'atkFileAttribute', atkConfig::get('businesscards', 'pdf_template_path'), AF_READONLY_EDIT);
    }
    
    public function postAdd($record)
    {
      $this->processPDFTemplate($record);
      return true;
    }
    
    /**
     * Read fields from PDF template
     *
     * @param array $record
     * @todo move loading lib to php.ini; for now, .so file is in '/usr/local/Zend/Core/lib/php/20060613'
     */
    private function processPDFTemplate($record)
    {
      dl("libpdf_php.so");
      
      $pdf_filename = atkConfig::get('businesscards', 'pdf_template_path').$record['business_card_pdf_path']['filename'];
      
      $pdf = new PDF($pdf_filename);
      $fields = $pdf->getFields();
      $pdf->close();
      
      atk_var_dump($fields, "Businesscards::processPDFTemplate | File: $pdf_filename");
      
      $bc_fields_node = &atkGetNode("businesscards.bc_fields");
      
      foreach ($fields as $blockname => $field)
      {
        $field_record['business_card_id'] = $record['id'];
        $field_record['field_blockname']  = $blockname;
        
        $field_record['page_nr']          = $field["page_nr"];
        $field_record['field_nr']         = $field["field_nr"];
        $field_record['field_multiline']  = $field["field_multiline"];
        $field_record['field_desc']       = $field["field_desc"];
        
        $bc_fields_node->addDb($field_record);
      }
    }
    
    public function initial_values()
    {
      $user = &atkGetUser();
      if (in_array($user['type_id'], array('2','3'))) // 2=customer, 3=supplier
      {
        return array("company_id"=>$user['company_id']);      
      }
    }
    
    public function descriptor_def()
    {
      return "[name]";
    }    
  }
?>
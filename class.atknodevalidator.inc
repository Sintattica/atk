<?php

  /**
   * This file is part of the Achievo ATK distribution.
   * Detailed copyright and licensing information can be found
   * in the doc/COPYRIGHT and doc/LICENSE files which should be 
   * included in the distribution.
   *
   * @package atk
   *
   * @copyright (c)2000-2004 Ibuildings.nl BV
   * @license http://www.achievo.org/atk/licensing ATK Open Source License
   *
   * @version $Revision$
   * $Id$
   */
   
  /**
   * Validator for records, based on node definition. 
   * 
   * The class takes a node, and based on the attribute definitions,
   * validation can be performed on records.
   *
   * @author Kees van Dieren <kees@ibuildings.nl>
   * @package atk
   *
   */
  class atkNodeValidator 
  {

    /**     
     * @var atkNode The node which needs to get validated
     * @access private
     */
    var $m_nodeObj = null;

    /**    
     * @var array the record of the node which will get validated
     * @access private
     */
    var $m_record = array();

    /**
     * @var String the mode in which the validate will get runned     
     * @access private
     */
    var $m_mode = "";

    /**
     * constructor
     *
     */
    function atkNodeValidator() 
    {
    }

    /**
     * set the list of fields which will get ignored 
     * @param array $fieldArr List of fields to ignore during validation
     */
    function setIgnoreList($fieldArr)
    {	
      $this->m_ignoreList = $fieldArr;
    }

    /**
     * set the mode in which to validate
     * @param String $mode The mode ("edit"/"add")
     */
    function setMode($mode)
    {	
      $this->m_mode = $mode;
    }

    /**
     * Set the Node which should be validated
     *
     * @param atkNode $nodeObj The node for validation
     */
    function setNode(&$nodeObj)
    {	
      $this->m_nodeObj = &$nodeObj;
    }

    /**
     * set the record which should get validated.
     * @param array $record The record to validate. The record is passed by
     *                      reference, because errors that are found are
     *                      stored in the record.
     */
    function setRecord(&$record)
    {	
      $this->m_record = &$record;
    }

    /**
     * Validate a record
     *
     * @param string $mode Override the mode
     * @param array $ignoreList Override the ignoreList
     */
    function validate($mode="", $ignoreList=array())
    {	
      // check overrides
      if(count($ignoreList))
        $this->setIgnoreList($ignoreList);

      if($mode != "")
        $this->setMode($mode);

       atkdebug("validate() with mode ".$this->m_mode." for node ".$this->m_nodeObj->atkNodeType());
      
      // set the record
      $record = &$this->m_record;

      // First we check if the values for the attributes are correct
      foreach (array_keys($this->m_nodeObj->m_attribList) as $attribname)
      {
        if (!atk_in_array($attribname, $this->m_ignoreList))
        {
          $p_attrib = &$this->m_nodeObj->m_attribList[$attribname];

          if (!( $p_attrib->isEmpty($record)))
          {
            $funcname = $p_attrib->m_name."_validate";
            if (method_exists($this->m_nodeObj, $funcname))
            {
              $this->m_nodeObj->$funcname($record, $this->m_mode);
            }
            else
            {
              $p_attrib->validate($record, $this->m_mode);
            }
          }
        }
      }
      
      if(isset($record['atkerror'])&&count($record['atkerror']) > 0)
      {
        for ($i = 0, $_i = count($record["atkerror"]); $i < $_i; $i++)
          $record["atkerror"][$i]["node"] = $this->m_nodeObj->m_type;        
        return false;
      }

      // After the values check, we check the Flags.
      $db = &$this->m_nodeObj->getDb();
      foreach (array_keys($this->m_nodeObj->m_attribList) as $attribname)
      {
        if (!atk_in_array($attribname, $this->m_ignoreList))
        {
          $p_attrib = &$this->m_nodeObj->m_attribList[$attribname];

          //if(count($record['atkerror']) > 0) return false;
          //Validate if ne
          if ($p_attrib->hasFlag(AF_PRIMARY) && !$p_attrib->hasFlag(AF_AUTO_INCREMENT))
          {                        
            $atkorgkey = $record["atkprimkey"];
            if(($atkorgkey == '' // no orgkey, so adding this record
                || $atkorgkey != $this->m_nodeObj->primaryKey($record)) // key has changed, so check is necessary
               &&  count($db->getrows("SELECT ".$p_attrib->fieldName()." FROM ".$this->m_nodeObj->m_table." WHERE ".$this->m_nodeObj->primaryKey($record)))>0
              )
            {
              triggerError($record, $attribname, 'error_primarykey_exists', '', $p_attrib->m_tabs[0]);
            }
          }

          // if no root elements may be added to the tree, then every record needs to have a parent!
          if ($p_attrib->hasFlag(AF_PARENT) && $this->m_nodeObj->hasFlag(NF_TREE_NO_ROOT_ADD) && $this->m_nodeObj->m_action == "save")
            $p_attrib->m_flags |= AF_OBLIGATORY;

          // validate obligatory fields (but not the auto_increment ones, because they don't have a value yet)
          if ($p_attrib->hasFlag(AF_OBLIGATORY) && !$p_attrib->hasFlag(AF_AUTO_INCREMENT) && $p_attrib->isEmpty($record))
          {
            triggerError($record, $attribname, 'error_obligatoryfield', '', $p_attrib->m_tabs[0]);
          }
          // if flag is primary
          else if ($p_attrib->hasFlag(AF_UNIQUE) && !$p_attrib->hasFlag(AF_PRIMARY) &&
                   count($db->getrows("SELECT ".$p_attrib->fieldName()." FROM ".$this->m_nodeObj->m_table." WHERE $attribname='".$db->escapeSQL($p_attrib->value2db($record))."' AND NOT (".$this->m_nodeObj->primaryKey($record).")"))>0
                  )
          {
            triggerError($record, $attribname, 'error_uniquefield', '', $p_attrib->m_tabs[0]);
          }
        }
      }
      
      $this->validateUniqueFieldSets($record);

      if (isset($record['atkerror']))
      {
        for ($i = 0, $_i = count($record["atkerror"]); $i < $_i; $i++)
          $record["atkerror"][$i]["node"] = $this->m_nodeObj->m_type;        
        return false;
      }
      
      return true;
    }

    /**
     * Check unique field combinations.
     * The function is called by the validate() method automatically. It is
     * not necessary to call this manually in a validation process.
     * Errors that are found are stored in the $record parameter
     *
     * @param array $record The record to validate
     */
    function validateUniqueFieldSets(&$record)
    {
      $db = &$this->m_nodeObj->getDb();
      atkimport('atk.db.atkquery');
      foreach($this->m_nodeObj->m_uniqueFieldSets as $uniqueFieldSet)
      {
        $query = &$db->createQuery();
        $query->addField('*');
        $query->addTable($this->m_nodeObj->m_table);
        foreach($uniqueFieldSet as $field)
        { 
          $attrib = &$this->m_nodeObj->m_attribList[$field];
          if ($attrib)
          {
            if (strstr(strtolower(get_class($attrib)),'attribute'))
            {
              $query->addCondition($query->quoteField($field)." = '".$attrib->value2db($record)."'");
            }
            else if (strstr(strtolower(get_class($attrib)),'manytoonerelation') && $attrib->createDestination())
            {
              foreach ($attrib->m_refKey as $refkey)
              {
                $query->addCondition($query->quoteField($refkey)." = '".$db->escapeSQL($record[$attrib->fieldName()][$refkey])."'");
              }
            }
            else $query->addCondition($query->quoteField($field)." = '".$db->escapeSQL($record[$field])."'");
          }
          else 
            atkerror("Field $field is mentioned in uniquefieldset but does not exist in ".$this->m_nodeObj->atknodetype());
        }
        $query->addCondition("NOT (".$this->m_nodeObj->primaryKey($record).")");
        
        if (count($db->getRows($query->buildSelect()))> 0)
        {	
          triggerError($record, $uniqueFieldSet, 'error_uniquefieldset', '', $this->m_nodeObj->m_type);
        }
      }
    }
  }
?>
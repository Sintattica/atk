<?php
  /**
   * This file is part of the Achievo ATK distribution.
   * Detailed copyright and licensing information can be found
   * in the doc/COPYRIGHT and doc/LICENSE files which should be 
   * included in the distribution.
   * 
   * @package atk
   * @subpackage relations
   *
   * @copyright (c)2000-2004 Ivo Jansch
   * @license http://www.achievo.org/atk/licensing ATK Open Source License
   *
   * @version $Revision$
   * $Id$
   */

  /**
   * The atkRelation class defines a relation to another node. 
   *
   * @author Ivo Jansch <ivo@achievo.org>
   * @package atk
   * @subpackage relations
   * @abstract
   *
   */
  class atkRelation extends atkAttribute
  {
    /**
     * @var String Destination node.
     */
    var $m_destination;

    /**
     * @var atkNode Destination instance. 
     */
    var $m_destInstance="";

    /**
     * @var String Filter for destination records. 
     */
    var $m_destinationFilter="";

    /**
     * Constructor
     * @param String $name The name of the relation. 
     * @param String $destination The destination node (in module.name notation)
     * @param int $flags Flags for the relation
     */
    function atkRelation($name, $destination, $flags=0)
    {
      $this->atkAttribute($name, $flags);
      $this->m_destination = $destination;

    }

    /**
     * Create the instance of the destination. 
     *
     * If succesful, the instance is stored in the m_destInstance member variable.
     * 
     * @return boolean true if succesful, false if something went wrong.
     */
    function createDestination()
    {
      if ($this->m_destInstance == "")
      {
        $cache_id = $this->m_owner.".".$this->m_name;
        $this->m_destInstance = &getNode($this->m_destination, $cache_id);

        // Validate if destination was created succesfully
        if (!is_object($this->m_destInstance))
	      {
	        atkerror("Relation with unknown nodetype '".$this->m_destination."' (in node '".$this->m_owner."')");
	        $this->m_destInstance = NULL;
	        return false;
	      }
	
        if ($this->hasFlag(AF_NO_FILTER)) $this->m_destInstance->m_flags |= NF_NO_FILTER;
        foreach (array_keys($this->m_destInstance->m_attribList) as $key)
        {
          $attribute = &$this->m_destInstance->m_attribList[$key];
          if (is_subclass_of($attribute, "atkrelation") && is_object($this->m_ownerInstance) && $attribute->m_destination == $this->m_ownerInstance->atkNodeType())
            $attribute->m_destInstance = &$this->m_ownerInstance;
        }
      }
      return true;
    }

    /**
     * Return a displayable string for a record. 
     * @param array $record The record that contains the information to display.
     * @return String a displayable string for this value.
     */
    function display($record)
    {
      return $record[$this->fieldName()];
    }

    /**
     * Validation method. Empty implementation. Derived classes may override
     * this function.
     * @abstract     
     */
    function validate(&$record, $mode)
    {
    }

    /**
     * Check if the relation is empty
     * @param array $record The record to check
     * @return boolean true if a destination record is present. False if not.
     */
    function isEmpty($record)
    {
      if ($this->createDestination())
      {
        $keyval = $record[$this->fieldName()][$this->m_destInstance->primaryKeyField()];        
        return ($keyval == "" || $keyval == NULL || $keyval === 0);
        // mindyou, we must check with '===', because if keyval is  a string, the comparison
        // is always true. (for some reason 'test'==0 is true in php.)
      }
      return true; // always empty if error.
    }

    /**
     * Retrieve the searchmodes supported by the relation.
     * @return array A list of supported searchmodes.
     */
    function getSearchModes()
    {
      // exact match and substring search should be supported by any database.
      // (the LIKE function is ANSI standard SQL, and both substring and wildcard
      // searches can be implemented using LIKE)
      // Possible values
      //"regexp","exact","substring", "wildcard","greaterthan","greaterthanequal","lessthan","lessthanequal"
      return array("exact");
    }
    
    /**
     * Since most relations do not store anything in a field, the default
     * fieldtype for relations is "". Exceptions (like the many2oone relation,
     * which stores a foreign key) can implement their own dbFieldType().
     * @abstract
     * @return String
     */
    function dbFieldType()
    {
      return "";
    }
    
    /**
    * Returns an array to pass as limit parameter to the selectdb function
    *
    * @return array an array to pass to the selectdb function
    */
    function fetchLimitArray()
    {
      global $ATK_VARS;

      // limitVarPostfix was introduced to make a difference when
      // there is more than one item with pagenumbers on the page
      $postfix = $this->fieldName();

      $retArray = array();

      if ($ATK_VARS["atkstartat" . $postfix] == "" )
      {
        $retArray["offset"] = 0;
      }
      else
      {
        $retArray["offset"] = $ATK_VARS["atkstartat" . $postfix];
      }

      // If we didn't pass along our own limit (atklimit var with postfix)
      // we'll use the recordsperpage config value and if THAT fails,
      // we use 25
      if ($ATK_VARS["atklimit" . $postfix] == "")
      {
        $retArray["limit"] = atkconfig("recordsperpage", 25);
      }
      else
      {
        $retArray["limit"] = $ATK_VARS["atklimit" . $postfix];
      }
      return $retArray;
    }
    
    /**
     * Returns the condition (SQL) that should be used when we want to join a relation's
     * owner node with the parent node.
     * @return String SQL string for joining the owner with the destination. 
     *                Defaults to false.
     */
    function getJoinCondition(&$query, $tablename="",$fieldalias="")
    {
      return false; 
    }
  }
  
?>
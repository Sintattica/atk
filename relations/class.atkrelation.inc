<?php
  /**
   * The atkRelation class defines a relation to another node
   *
   * Note        : A relation is a (very) special kind of attribute.
   *
   * TODO: relations with a referential key that consist of more than one field don't work.
   *
   * @author Ivo Jansch (ivo@achievo.org)
   *
   * @version $Revision$
   *
   * $Id$
   *
   */
  class atkRelation extends atkAttribute
  {

   /**
    * Destination
    */
    var $m_destination;

   /**
    * Destination Instance
    */
    var $m_destInstance="";

   /**
    * Filter
    */
   var $m_destinationFilter="";

   /**
    * Constructor
    * @param $name ?
    * @param $destination ?
    * @param $flags Flags for the relation
    */
    function atkRelation($name, $destination, $flags=0)
    {
      $this->atkAttribute($name, $flags);
      $this->m_destination = $destination;

    }

    /**
    * Create Destination
    */
    function createDestination()
    {
      if ($this->m_destInstance == "")
      {
        $this->m_destInstance = &newNode($this->m_destination);

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
    * Return a displayable string for this value.
    * @return a displayable string for this value.
    */
    function display($record)
    {
      return $record[$this->fieldName()];
    }

    /**
    * Dummy function
    */
    function validate(&$record, $mode)
    {
    }

    /**
     * Check if the attribute is empty
     * @return true if it's empty
     */
    function isEmpty($postvars)
    {
      if ($this->createDestination())
      {
        $keyval = $postvars[$this->fieldName()][$this->m_destInstance->primaryKeyField()];
        return ($keyval == "" || $keyval == NULL || $keyval === 0);
        // mindyou, we must check with '===', because if keyval is  a string, the comparison
        // is always true. (for some reason 'test'==0 is true in php.)
      }
      return true; // always empty if error.
    }

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
     */
    function dbFieldType()
    {
      return "";
    }

  }
?>

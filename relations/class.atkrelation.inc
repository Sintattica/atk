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
        if ($this->hasFlag(AF_NO_FILTER)) $this->m_destInstance->m_flags |= NF_NO_FILTER;
//        if ($this->m_destinationFilter!="")
  //      {
    //      $this->m_destInstance->addFilter($this->m_destinationFilter);
      //  }
      }      
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
      $this->createDestination();
      $keyval = $postvars[$this->fieldName()][$this->m_destInstance->primaryKeyField()];
      return ($keyval == "" || $keyval == NULL || $keyval === 0);
      // mindyou, we must check with '===', because if keyval is  a string, the comparison
      // is always true. (for some reason 'test'==0 is true in php.)
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
    
    
  }  
?>
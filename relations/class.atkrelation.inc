<?php
  /**
   * The atkRelation class defines a relation to another node
   *   
   * Note        : A relation is a (very) special kind of attribute.
   *
   * TODO: relations with a referential key that consist of more than one field don't work.
   *
   * @author Ivo Jansch (ivo@achievo.com)
   *
   * @version 0.6
   *
   * <b>Changes</b>:
   * 
   * 0.6    - NF_NO_FILTER of destination is now set if attribute has AF_NO_FILTER (Ivo)
   *
   * $Id$
   * $Log$
   * Revision 1.1  2001/04/23 10:17:14  ivo
   * Initial revision
   *
   * Revision 1.4  2001/04/05 15:10:12  ivo
   * AAAAAAAAARG. previous fix was wrong. now it's right. I hope. Finally.
   *
   * Revision 1.3  2001/04/05 15:06:54  ivo
   * undid the last modification (caused stack overflows)
   *
   * Revision 1.2  2001/04/05 15:02:19  peter
   * Optimised relations.
   *
   * Revision 1.1.1.1  2001/01/10 13:58:00  sandy
   * Achievo Tool Kit
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
    * Owner Instance
    */
    var $m_ownerInstance="";


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
// the next line would be efficient, but leads to stack overflows.. :(
    //  $this->createDestination();
    }

    /**
    * Create Destination
    */
    function createDestination()
    {
      $classname = $this->m_destination;
      include "class.".$classname.".inc";      
      if ($this->m_destInstance == "")
      {
        $this->m_destInstance = new $classname();
        if ($this->hasFlag(AF_NO_FILTER)) $this->m_destInstance->m_flags |= NF_NO_FILTER;
      }
    }
    
    /**
    * create an instance of the same class as the owner.. (normally, an attribute
    * would have a pointer to its owner, but since php lacks pointers, we have to
    * do it like this)
    */
    function createOwner()
    {
      $classname = $this->m_owner;
      include "class.".$classname.".inc";      
      if ($this->m_ownerInstance == "")
      {
        $this->m_ownerInstance = new $classname();
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
    function validate($value)
    {
      return "";
    }

  }

 

  include "atk/relations/class.atkmanytoonerelation.inc";
  include "atk/relations/class.atkmanytomanyrelation.inc";  
  include "atk/relations/class.atkmanyboolrelation.inc";    
  include "atk/relations/class.atkonetomanyrelation.inc";    
  include "atk/relations/class.atkmatrixrelation.inc";
?>

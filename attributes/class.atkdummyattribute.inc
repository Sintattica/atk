<?php

  /**
   * With the atkDummyAttribute class you can place comments between the edit boxes.
   * Use the flag AF_NOLABEL if you want to start at the begin of the line.
   * 
   * @author	 : Sandy Pleyte <sandy@ibuildings.nl>
   * @version	 : $Revision$
   *
   * $Id$
   *
   */

  class atkDummyAttribute extends atkAttribute
  {
    var $m_text;

    /**
         * Constructor
         * @param $text text field
         * @param $flags Flags for this attribute
         */
    function atkDummyAttribute($name, $text="", $flags=0)
    {
      $flags=$flags|AF_BLANKLABEL;
      $this->atkAttribute($name,$flags); // base class constructor
      $this->m_text = $text;
    }
   
    /**
    * Edit  record
    * Here it will only return the text, no edit box.
    * @param $record Array with fields
    * @return Parsed string
    */
    function edit($record="", $fieldprefix="")
    {
      return $this->m_text;
    }
    
   /**
    * VOID implementation.. parserAttribute has no data associated with it, so you can't search it.
    * @param $record Array with fields
    */    
    function search($record="", $size=0, $maxsize=0)
    {
      return "&nbsp;";
    }

   /**
    * Display a record
    * Here it will only returns the text.
    * @param $record  Array with fields
    * @return Parsed string
    */
    function display($record)
    {
      return $this->m_text;
    }
 
    /**
    * No function, but is neccesary
    */
    function store($a, $b)
    {
    }

    /**
    * No function, but is neccesary
    */
    function addToQuery(&$query, $tablename="", $fieldaliasprefix="",$level)
    {
    }
    
    function getSearchModes()
    {
      // exact match and substring search should be supported by any database.
      // (the LIKE function is ANSI standard SQL, and both substring and wildcard
      // searches can be implemented using LIKE)
      // Possible values
      //"regexp","exact","substring", "wildcard","greaterthan","greaterthanequal","lessthan","lessthanequal"
      return array();
    }
  }
?>

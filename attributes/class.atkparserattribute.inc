<?php
  /**
   * The atkParserAttribute can be used to create links or texts that 
   * contain values, by supplying a template as parameter.   
   *
   * @author	 : Ivo Jansch <ivo@achievo.org>
   * @version	 : $Revision$
   *
   * $Id$
   *
   */
  class atkParserAttribute extends atkAttribute
  {
    var $m_text;

    /**
     * Constructor
     * @param $name Name of the attribute
     * @param $text text field
     * @param $flags Flags for this attribute
     */
    function atkParserAttribute($name, $text, $flags=0)
    {
      $this->atkAttribute($name,$flags); // base class constructor
      $this->m_text = $text;
    }

    /**
     * Parses a record
     * @param $record Array with fields
     * @return Parsed string
     */
    function edit($record="", $fieldprefix="")
    {
      return stringparse($this->m_text,$record);
    }

    /**
     * VOID implementation.. parserAttribute has no data associated with it, so you can't search it.
     * @param $record Array with fields
     */
    function search($record="")
    {
      return "&nbsp;";
    }

    /**
     * Parses a record
     * @param $record  Array with fields
     * @return Parsed string
     */
    function display($record)
    {
      return stringparse($this->m_text,$record);
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
    function addToQuery(&$query, $tablename="", $fieldaliasprefix="", $rec, $level, $mode)
    {
    }
    
    /**
     * Dummy imp
     */
    function dbFieldType()
    {
      return "";
    }
  }
?>
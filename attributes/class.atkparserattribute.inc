<?php
  /**
   * The atkparserAttribute class represents an attribute of a node
   * ......
   * @author	 : Ivo Jansch <ivo@achievo.com>
   * @version	 : 0.5
   *
   * $Id$
   * $Log$
   * Revision 4.1  2001/05/10 08:31:00  ivo
   * Major upgrade. Changes:
   * * Deprecated the m_records/m_currentRec feature of atknode. Nodes are now
   *   singletons by default, and nodefunctions pass around recordsets.
   * + Session management for forms. If you now leave a page through a click on
   *   a link, the session remembers everything from your form and restores it
   *   when you return.
   * + New relation: oneToOneRelation
   * + Reimplemented the embedded editForm feature (forms inside forms)
   *
   * Revision 4.0  2001/04/23 10:20:39  ivo
   * revision tags.
   *
   * Revision 1.1.1.1  2001/04/23 10:17:09  ivo
   * Added atk4 to repository.
   *
   * Revision 1.3  2001/02/23 11:17:30  sandy
   * - Updated the layout class with new template engine
   * - New Dummy attribute
   * - New skel directory
   * - and some small new config vars
   *
   * Revision 1.2  2001/02/22 22:46:18  peter
   * changed edit/search API, new member vars, clean-up of code
   *
   * Revision 1.1.1.1  2001/01/10 13:57:58  sandy
   * Achievo Tool Kit
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
  }
?>
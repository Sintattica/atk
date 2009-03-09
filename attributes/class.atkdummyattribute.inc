<?php

  /**
   * This file is part of the Achievo ATK distribution.
   * Detailed copyright and licensing information can be found
   * in the doc/COPYRIGHT and doc/LICENSE files which should be
   * included in the distribution.
   *
   * @package atk
   * @subpackage attributes
   *
   * @copyright (c)2000-2004 Ibuildings.nl BV
   * @license http://www.achievo.org/atk/licensing ATK Open Source License
   *
   * @version $Revision$
   * $Id$
   */

   atkimport("atk.attributes.atkattribute");

   /**
    * Custom flags
    */
   define("AF_DUMMY_SHOW_LABEL", AF_SPECIFIC_1); // make the dummy label its fields

  /**
   * With the atkDummyAttribute class you can place comments between other
   * attributes.
   *
   * Use the flag AF_NOLABEL if you want to start at the beginning of the
   * line.
   *
   * @author Sandy Pleyte <sandy@ibuildings.nl>
   * @package atk
   * @subpackage attributes
   *
   */
  class atkDummyAttribute extends atkAttribute
  {
    var $m_text;

    /**
     * Constructor
     * @param String $name The name of the attribute
     * @param String $text The text to display
     * @param int $flags The flags for this attribute
     */
    function atkDummyAttribute($name, $text="", $flags=0)
    {
    	// A Dummy attrikbute should not be searchable and sortable
      $flags |= AF_HIDE_SEARCH | AF_NO_SORT;

      // Add the AF_BLANKLABEL flag unless the AF_DUMMY_SHOW_LABEL flag wasn't present
      if (!hasFlag($flags, AF_DUMMY_SHOW_LABEL))
    	  $flags |= AF_BLANKLABEL;

      $this->atkAttribute($name,$flags); // base class constructor
      $this->m_text = $text;
    }

    /**
     * Returns a piece of html code that can be used in a form to edit this
     * attribute's value.
     * Here it will only return the text, no edit box.
     *
     * @param array $record The record that holds the value for this attribute.
     * @param String $fieldprefix The fieldprefix to put in front of the name
     *                            of any html form element for this attribute.
     * @param String $mode The mode we're in ('add' or 'edit')
     * @return String A piece of htmlcode for editing this attribute
     */
    function edit($record="", $fieldprefix="", $mode="")
    {
      return "<div ID=\"$this->m_name\">".$this->m_text."</div>";
    }

    /**
     * Returns a piece of html code that can be used to get search terms input
     * from the user.
     * VOID implementation, dummy attributes cannot be searched
     *
     * @param array $record Array with values
     * @param boolean $extended if set to false, a simple search input is
     *                          returned for use in the searchbar of the
     *                          recordlist. If set to true, a more extended
     *                          search may be returned for the 'extended'
     *                          search page. The atkAttribute does not
     *                          make a difference for $extended is true, but
     *                          derived attributes may reimplement this.
     * @param $fieldprefix The fieldprefix of this attribute's HTML element.
     *
     * @return String A piece of html-code
     */
    public function search($record=array(), $extended=false, $fieldprefix="")
    {
      return "&nbsp;";
    }

    /**
     * Display a record
     * Here it will only return the text.
     * @param array $record  Array with fields
     * @return Text
     */
    function display($record)
    {
      return $this->m_text;
    }
    
    /**
     * Set the text of this attribute
     *
     * @param string $text
     */
    function setText($text)
    {
      $this->m_text = $text;
    }

    /**
    * No function, but is neccesary
    * 
    * @param atkDb $db Database object
    * @param array $record The record
    * @param string $mode The mode
    * @return boolean to indicate if store went succesfully
    */
    function store($db, $record, $mode)
    {
      return true;
    }

    /**
     * Convert the database value to an internally used value
     * Since dummyattrbiutes are not stored in the database this function returns NULL
     *
     * @param array $record The record
     * @return NULL
     */
    function db2value($record)
    {
      return NULL;
    }

    /**
     * Adds this attribute to database queries.
     * VOID implementation because dummy attributes are not stored in the database
     *
     * @param atkQuery $query The SQL query object
     * @param String $tablename The name of the table of this attribute
     * @param String $fieldaliasprefix Prefix to use in front of the alias
     *                                 in the query.
     * @param Array $rec The record that contains the value of this attribute.
     * @param int $level Recursion level if relations point to eachother, an
     *                   endless loop could occur if they keep loading
     *                   eachothers data. The $level is used to detect this
     *                   loop. If overriden in a derived class, any subcall to
     *                   an addToQuery method should pass the $level+1.
     * @param String $mode Indicates what kind of query is being processing:
     *                     This can be any action performed on a node (edit,
     *                     add, etc) Mind you that "add" and "update" are the
     *                     actions that store something in the database,
     *                     whereas the rest are probably select queries.
     */
    function addToQuery(&$query, $tablename="", $fieldaliasprefix="", $rec="", $level, $mode)
    {
    }

    /**
     * Retrieve the list of searchmodes supported by the attribute.
     * Since this attribute does not support searching it returns an empty array
     *
     * @return empty array
     */
    function getSearchModes()
    {
      // exact match and substring search should be supported by any database.
      // (the LIKE function is ANSI standard SQL, and both substring and wildcard
      // searches can be implemented using LIKE)
      // Possible values
      //"regexp","exact","substring", "wildcard","greaterthan","greaterthanequal","lessthan","lessthanequal"
      return array();
    }

    /**
     * Return the database field type of the attribute.
     * VOID implementation because dummy attributes are not stored in the database
     *
     * @return empty string
     */
    function dbFieldType()
    {
      return "";
    }
  }
?>

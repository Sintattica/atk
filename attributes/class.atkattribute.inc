<?php

  /** Define some flags for attributes. Use the constructor of the atkAttribute
  * class to set the flags. (concatenate multiple flags with '|')
  */
  define("AF_UNIQUE"        ,  4); // The value of the attribute must be unique.
  define("AF_OBLIGATORY"    ,  8); // A value must always be entered for the attribute.
  define("AF_HIDE_LIST"     ,  1); // If this flag is set, the attribute will not show
                                   // up in record lists (like adminpage and selectpage).
  define("AF_HIDE_ADD"      ,  2); // The attribute will be hidden in add forms.
                                   // (unless AF_OBLIGATORY is also set)
  define("AF_NOLABEL"       , 16); // The attribute has no label in edit/add forms.
  define("AF_BLANKLABEL"    , 32); // The attribute has a blank label in edit/add forms.
  define("AF_CASCADE_DELETE", 64); // If this flag is set, then the delete function of
                                   // the attributes is called when an owning node is
                                   // deleted
  define("AF_PRIMARY"       ,128|AF_OBLIGATORY); // Attribute is part of the primary key of a node.
                                 // Primary key also needs to be obligatory...
  define("AF_HIDE_EDIT"     ,256); // Attribute is not editable.
  define("AF_AUTO_INCREMENT",512); // Attribute is autoincrement..
  define("AF_AUTOINCREMENT",512);  // AF_AUTO_INCREMENT is often mistyped without undersore..

  define("AF_NO_SORT"       ,1024);    // Attribute can not be sorted in recordlists.
  define("AF_NOSORT"  ,AF_NO_SORT);  // AF_NOSORT has wrong naming convention but is still there
                                     // for compatibility reasons.    
  define("AF_READONLY_ADD",2048);   // Attribute is readonly in add.
  define("AF_SEARCHABLE"  ,4096); // Attribute is searchable in list views 
  
  // flags specific for AtkFileAttribute
  define("AF_FILE_NO_UPLOAD"     , 8192);   // Unable to upload files
  define("AF_FILE_NO_SELECT"     , 16384);  // Unable to select files
  define("AF_FILE_NO_DELETE"     , 32768);  // Unable to delete files
  define("AF_FILE_NO_AUTOPREVIEW", 65536);  // Don't try to detect the file type (shows only filename)
  define("AF_FILE_POPUP"         , 131072); // Show preview in popup instead of inline

  define("AF_POPUP"              , AF_FILE_POPUP); // Since other attributes could also use a popup
                                                   // flag, there's now an AF_POPUP flag.

  define("AF_NO_FILTER"          , 262144); // Ignore filters when selecting records (used in relations)

  define("AF_HIDE_SELECT"        , 524288);  // Record wil not be shown in select pages.
  define("AF_NO_QUOTES"          , 1048576); // No quotes are used when adding an attrib with
                                             // this flag to the database.
  define("AF_FORCE_LOAD"         , 2097152); // With this flag, an attribute is always loaded,
                                             // even if it's not displayed anywhere.
  define("AF_FILE_PHYSICAL_DELETE",4194304); // Removes the file physically

  // flags specific for AtkMatrixAttribute
  define("AF_MATRIX_ACYCLIC"	 , 8388608); // Checks if a matrix is acyclic

  // flags specific for AtkPasswordAttribute
  define("AF_PASSWORD_NOVALIDATE", 16777216); // Disables password check when editing password field

  // multilangauge flag (internal use only!)
  define("AF_MULTILANGUAGE", 33554432); // Notify's atknode that this attribute is multilanguagal

  define("AF_TOTAL", 67108864);             // The attribute will have a 'total' column in lists. 

  define("AF_PARENT", 134217728);           // the parent field for parent child relations in a treeview
  define("AF_LARGE", 268435456); // Attribute will have a large ammount of 
                                 // records.. (attribs can optimize accordingly)
  
  define("AF_HIDE_SEARCH", 536870912); // Attribute is not searchable in extended search  
  
  define("AF_READONLY_EDIT", 1073741824); // Attribute is readonly when edited.   
  
  define("AF_HIDE",AF_HIDE_EDIT|AF_HIDE_ADD|AF_HIDE_LIST|AF_HIDE_SEARCH); // Attribute is totally hidden
  define("AF_AUTOKEY", AF_PRIMARY|AF_HIDE|AF_AUTOINCREMENT); // short way of saying that an attribute
                                                             // is an autoincremented primarykey that
                                                             // is also hidden from the user.
  define("AF_READONLY",AF_READONLY_EDIT|AF_READONLY_ADD); // Attribute is always readonly.
                                       
  // FLAGS MAY NOT EXCEED 2^31 (2147483648), because that's the integer limit beyond which
  // the bitwise operators won't work anymore.
                                                             
  /**  
   * The atkAttribute class represents an attribute of an atkNode.
   * An atkAttribute has a name and a set of parameters that
   * control its behaviour, like whether an atkAttribute
   * is obligatory, etc.
   *
   * @author Ivo Jansch <ivo@achievo.com>
   *
   * @version $Revision$
   *
   * $Id$
   *
   */
  class atkAttribute
  {
    /**
     * The name of the attribute
     */
    var $m_name;

    /**
     * The flags of the attribute
     * Flags can be:
     * AF_UNIQUE             The value of the attribute must be unique.
     * AF_OBLIGATORY         A value must always be entered for the attribute.
     * AF_HIDE_LIST          If this flag is set, the attribute will not show
     *                       up in record lists (like adminpage and selectpage).
     * AF_HIDE_ADD           The attribute will be hidden in add forms.
     *                       (unless AF_OBLIGATORY is also set)
     * AF_HIDE_SELECT        The attribute is hidden in select lists.
     * AF_HIDE               Attribute is totally hidden
     * AF_NOLABEL            The attribute has no label in edit/add forms.
     * AF_BLANKLABEL         The attribute has a blank label in edit/add forms.
     * AF_CASCADE_DELETE     If this flag is set, then the delete function of
     *                       the attributes is called when an owning node is
     *                       deleted
     * AF_PRIMARY            Attribute is part of the primary key of a node.
     * AF_HIDE_EDIT          Attribute is not editable.
     * AF_AUTO_INCREMENT     Attribute is autoincrement..
     * AF_NO_SORT            Attribute can't be sorted
     * AF_READONLY           Attribute is readonly
     * AF_SEARCHABLE         Attribute is searchable in list views
     * AF_NO_FILTER          If the attribute uses records from another node,
     *                       the other node will not be filtered.
     * AF_NO_QUOTES          No quotes are used when adding an attrib with
     *                       this flag to the database.
     * AF_FORCE_LOAD         With this flag, an attribute is always loaded,
     *                       even if it's not displayed anywhere.
     * <BR>
     * Flags specific for AtkFileAttribute
     * AF_FILE_NO_UPLOAD        Unable to upload files
     * AF_FILE_NO_SELECT        Unable to select files
     * AF_FILE_NO_DELETE        Unable to delete files
     * AF_FILE_NO_AUTOPREVIEW   Don't try to detect the file type (shows only filename)
     * AF_FILE_POPUP            Show preview in popup instead of inline
     */
    var $m_flags = 0;

    /**
     * The owner of the attribute (set by atknode)
     */
    var $m_owner = "";
    
    /**
     * Owner Instance
     */
    var $m_ownerInstance="";

    /**
     * The size, searchsize and maxsize of the attribute's field.
     */
    var $m_size = 0;
    var $m_searchsize = 0;
    var $m_maxsize = 0;

    /**
     * All meta data for the table this attribute is "part off"
     */
    var $m_tableMeta = "";

    /**
       * Order of the attribute
        */
    var $m_order = 0;
    // just a var for keeping a index
    var $m_index = 0;

    /**
     * Constructor
     *
     * <b>Example:</b>
     *        $this->addAttribute(new atkAttribute("naam",AF_OBLIGATORY));
     * @param $name Name of the attribute
     * @param $flags Flags for the attribute
     */
    function atkAttribute($name, $flags = 0)
    {
      $this->m_name = $name;
      $this->m_flags = (double)$flags;
    }

    /**
     * Checks if flag exist
     * @param $flag the flag you want to check
     * @return True or False
     */
    function hasFlag($flag)
    {
      return (($this->m_flags & $flag) == $flag);
    }

    /**
     * Returns the fieldname
     * @return fieldname
     */
    function fieldName()
    {
      return $this->m_name;
    }

    /**
     * Returns the formname
     * @return formname
     */
    function formName()
    {
      return $this->m_name;
    }

    /**
     * Check if the attribute is empty
     * @return true if it's empty
     */
    function isEmpty($postvars)
    {
      return ($postvars[$this->fieldName()]=="");
    }

    /**
     * Add's slashes to the string for the database
     * @param $rec Array with values
     * @return String with slashes
     */
    function value2db($rec)
    {
      return escapeSQL($rec[$this->fieldName()]);
    }

    /**
     * Removes slashes from the string
     * @param $rec Array with values
     * @return String without slashes
     */
    function db2value($rec)
    {
      return stripslashes($rec[$this->fieldName()]);
    }


    /**
     * Fetch values
     * @param $rec Array with values
     * @return String without slashes
     */
    function fetchValue($rec)
    {
      return $rec[$this->fieldName()];
    }

    /**
     * Returns a piece of html code that can be used in a form to edit this
     * attribute's value.
     * @param $record Array with values
     * @param $fieldprefix The attribute must use this to prefix its form elements (used for
     *                     embedded forms)
     * @return Piece of htmlcode
     */
    function edit($record="", $fieldprefix="")
    {
      $result = '<input type="text" name="'.$fieldprefix.$this->formName().
                '" value="'.htmlspecialchars($record[$this->fieldName()]).'"'.
                ($this->m_size > 0 ? ' size="'.$this->m_size.'"' : '').
                ($this->m_maxsize > 0 ? ' maxlength="'.$this->m_maxsize.'"' : '').'>';
              
      return $result;
    }
    
    /**
     * Returns a piece of html code that can be used in a form to display
     * hidden values for this attribute.
     * @param $record Array with values
     * @return Piece of htmlcode
     */
    function hide($record="", $fieldprefix="")
    {
      $result = '<input type="hidden" name="'.$fieldprefix.$this->formName().
                '" value="'.$record[$this->fieldName()].'">';
      return $result;             
    }

    /**
     * Returns a piece of html code that can be used to search for an
     * attribute's value.
     * @param $record Array with values
     * @return Piece of htmlcode
     */
    function search($record="", $extended=false)
    {
      $result= '<input type="text" name="atksearch['.$this->formName().']" value="'.$record[$this->fieldName()].'"'.
                ($this->m_searchsize > 0 ? ' size="'.$this->m_searchsize.'"' : '').
                ($this->m_maxsize > 0 ? ' maxlength="'.$this->m_maxsize.'"' : '').'>';                      
      return $result;
    }        

    /**
     * Creates an search condition for a given search value
     * @param $table the table name
     * @param $value the search value
     * @return a piece of the SQL where clause
     */
    function searchCondition(&$query, $table, $value, $searchmode)
    {      
      $func = $searchmode."Condition";
      if (method_exists($query,$func))
      {
        $query->addSearchCondition($query->$func($table.".".$this->fieldName(),escapeSQL($value)));
      }
      else
      {
        atkdebug("Database doesn't support searchmode '$searchmode' for ".$this->fieldName().", ignoring condition.");
      }
    }

    /**
     * Returns a displayable string for this value.
     * Since we always display in html, but we don't expext the text
     * to be html, we have to do some conversions.
     * if you don't like this, use the atkHtmlAttribute.
     * @param $record
     * @return String ready for displaying
     * @see atkHtmlAttribute
     */
    function display($record)
    {
      // default behaviour is that we display a value 'as is'.
      return nl2br(htmlspecialchars($record[$this->fieldName()]));
    }

    /**
     * Validate's values
     * @param &$record Record that contains value to be validated.
     *                 Errors are saved in this record
     * @param $mode can be either "add" or "update"
     * @return $record
     */
    function validate(&$record, $mode)
    {
    }

    /**
     * Add's fields to query
     * @param $query The SQL query
     * @param $tablename Tablename
     * @param $fieldaliasprefix Field alias prefix
     * @param $level ?
     * @param $mode select/insert/update
     * @return New SQL Query
     */
    function addToQuery(&$query, $tablename="", $fieldaliasprefix="", $rec="", $level, $mode)
    {
      if ($mode == "select")
        $query->addField($this->fieldName(),"",$tablename,$fieldaliasprefix,!$this->hasFlag(AF_NO_QUOTES));
      else if ($mode == "add" || $mode == "edit")
        $query->addField($this->fieldName(),$this->value2db($rec),"","",!$this->hasFlag(AF_NO_QUOTES));
    }

    /**
     * delete is only of interest for special attributes like relations, or file attributes.
     * @param $record record that has to be delete
     */
    function delete($record)
    {
      // delete is only of interest for special attributes like relations, or file attributes.
    }
    
    /**
     * returns a record which is the sum of two records
     */
    function sum($rec1, $rec2)
    {
      return array($this->fieldName()=>($rec1[$this->fieldName()]+$rec2[$this->fieldName()]));
    }
    
    /**
     * Fetch the metadata about this attrib from the table metadata
     */    
    function fetchMeta($metadata)
    {
      $this->m_tableMeta = $metadata[$this->m_name];
    }
    
    /**
     * This function is called right after the attribute is added to the node.
     * Attributes may do special initialisation here..
     */
    function init()
    {
    }
    
    /**
     * This function is called to compare two values.
     *
     */
    function equal($recA, $recB)
    {
      return ($recA[$this->fieldName()] == $recB[$this->fieldName()]);
    }

    /**
     * This function is called by atkNode to determine if a field
     * needs to be saved to the database in an updateDb call. 
     *     
     * @param $record The record that is about to be saved.
     */
    function needsUpdate($record)
    {      
      return (!$this->hasFlag(AF_READONLY_EDIT) && !$this->hasFlag(AF_HIDE_EDIT));     
    }

    function getSearchModes()
    {
      // exact match and substring search should be supported by any database.
      // (the LIKE function is ANSI standard SQL, and both substring and wildcard
      // searches can be implemented using LIKE)
      // Possible values
      //"regexp","exact","substring", "wildcard","greaterthan","greaterthanequal","lessthan","lessthanequal"
      return array("exact","substring","wildcard","regexp");
    }

  }
  
?>

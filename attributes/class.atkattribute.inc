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
   * <br>
   * Note        : The atkAttribute class is not used to store data (at least not
   *               yet) It's only used to process data. Data is currently
   *               stored in a flat array in class.atknode.inc.
   * @author Ivo Jansch (ivo@achievo.com)
   *
   * @version $Revision$
   *
   * <b>Changes</b>:
   *
   * 0.11   - Sandy Pleyte   (sandy@ibuildings.nl)
   *               New flag: AF_PHYSICAL_DELETE
   * 0.10   - New flag: AF_MATRIX_ACYCLIC
   * 0.9    - Attributes are now informed of the table metainformation (Peter)
   * 0.8    - New flags: AF_NO_QUOTES and AF_FORCE_LOAD (Ivo)
   * 0.7    - New flag: AF_HIDE_SELECT (Ivo)
   *        - Renamed AF_NOSORT to AF_NO_SORT (Ivo)
   * 0.6    - New flag: AF_NO_FILTER (Ivo)
   *
   *
   * $Id$
   * $Log$
   * Revision 4.17  2001/07/18 12:10:10  ivo
   * New flags: AF_READONLY_ADD and AF_READONLY_EDIT (AF_READONLY automatically
   * selects both flags)
   *
   * Revision 4.16  2001/07/15 17:03:59  ivo
   * New feature: Alphabetical index on top of recordlists.
   * (use e.g. $this->setIndex("name"); in your class constructor to activate)
   * Fixed some bugs in extended search.
   *
   * Revision 4.15  2001/07/15 16:37:19  ivo
   * New atk.inc includefile in skel.
   * New feature: extended search.
   * Fixed a bug in session management; style.php and other files that get
   * loaded between two dispatches could corrupt a session.
   *
   * Revision 4.14  2001/07/10 14:23:09  ivo
   * Fixed a bug in atkAttribute that caused texts with "" to fail.
   *
   * Revision 4.13  2001/07/10 12:55:07  ivo
   * New flag: AF_AUTOKEY (short for AF_PRIMARY|AF_AUTOINCREMENT|AF_HIDE)
   *
   * Revision 4.12  2001/06/29 11:26:59  ivo
   * Patches to oracle support.
   *
   * Revision 4.11  2001/06/27 15:25:42  peter
   * A lot of fixes to the oci8 support, including better outer joins, and
   * limitting queries! Fields now have an asterix behind them if they are
   * required. Some other small enhancements.
   *
   * Revision 4.10  2001/06/18 10:05:51  ivo
   * New attribute: atkProfileAttribute, for editing security profiles.
   * New node flag: NF_NO_SECURITY
   *
   * Revision 4.9  2001/06/07 21:06:54  ivo
   * Added atkemailattribute
   *
   * Revision 4.8  2001/05/22 08:36:50  peter
   * Added the searchCondition method which will be used by atkNode to create
   * a proper search condition for the attribute.
   *
   * Revision 4.7  2001/05/17 10:27:12  ivo
   * Fixes to session management, new AF_LARGE feature for manytoonerelation.
   * Improved handling of GET_VARS and POST_VARS (requires a new dispatcher)
   *
   * Revision 4.6  2001/05/16 09:05:06  ivo
   * Revision tag test
   *
   * Revision 4.5  2001/05/14 10:17:54  ivo
   * Added new atkfilewriter attribute.
   *
   * Revision 4.4  2001/05/10 08:31:00  ivo
   * Major upgrade. Changes:
   * * Deprecated the m_records/m_currentRec feature of atknode. Nodes are now
   *   singletons by default, and nodefunctions pass around recordsets.
   * + Session management for forms. If you now leave a page through a click on
   *   a link, the session remembers everything from your form and restores it
   *   when you return.
   * + New relation: oneToOneRelation
   * + Reimplemented the embedded editForm feature (forms inside forms)
   *
   * Revision 4.3  2001/05/07 15:13:49  ivo
   * Put config_atkroot in all files.
   *
   * Revision 4.2  2001/05/01 09:49:49  ivo
   * Replaced all require() and include() calls by require_once() and
   * include_once() calls. The if(!DEFINED)... inclusion protection in files
   * is now obsolete.
   *
   * Revision 4.1  2001/04/25 08:46:08  ivo
   * Big optimisations: using references for attribute manipulation.
   *
   * Revision 4.0  2001/04/23 10:20:39  ivo
   * revision tags.
   *
   * Revision 1.1.1.1  2001/04/23 10:17:07  ivo
   * Added atk4 to repository.
   *
   * Revision 1.12  2001/04/19 08:56:21  ivo
   * Got sick of mistyping AUTO_INCREMENT as AUTOINCREMENT: now both are
   * allowed.
   *
   * Revision 1.11  2001/04/10 11:51:46  martin
   * ATK treeview added for parent child relations. Just use NF_TREE as flag when you call the atknode constructor.
   *
   * Revision 1.10  2001/03/21 15:02:06  ivo
   * New feature: sticky vars (variables that 'stick' to all forms and
   * urls once you've set them.
   * Stripped the m_prefix function, since embedded forms have become
   * obsolete.
   *
   * Revision 1.9  2001/03/14 15:21:54  ivo
   * New feature: AF_TOTAL flag for attributes. This results in a total row
   * at the bottom of the record list.
   *
   * Revision 1.8  2001/03/14 12:23:22  peter
   * added hide method, to reflect Ivo's changes
   *
   * Revision 1.7  2001/03/14 10:22:28  ivo
   * Attributes now have a hide() function to do their own input=hidden implementation.
   *
   * Revision 1.6  2001/03/02 15:06:45  ivo
   * New node flag: NF_ADD_LINK (instead of an addform on an adminpage, you get
   * a 'click here to add' link)
   *
   * Revision 1.5  2001/02/23 11:17:30  sandy
   * - Updated the layout class with new template engine
   * - New Dummy attribute
   * - New skel directory
   * - and some small new config vars
   *
   * Revision 1.4  2001/02/22 22:46:17  peter
   * changed edit/search API, new member vars, clean-up of code
   *
   * Revision 1.3  2001/02/14 14:14:24  peter
   * initial release of the password attribute
   *
   * Revision 1.2  2001/02/07 21:02:44  martin
   * Added multiplefile attrib for selection of multiple files in one dbfield
   * updated class attribute to include the new attrib
   *
   * Revision 1.1.1.1  2001/01/10 13:57:56  sandy
   * Achievo Tool Kit
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
                '" value="'.htmlentities($record[$this->fieldName()]).'"'.
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
        $query->addCondition($query->$func($table.".".$this->fieldName(),escapeSQL($value)));
      }
      else
      {
        atkdebug("Database doesn't support searchmode '$searchmode', ignoring condition.");
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
      return nl2br(htmlentities($record[$this->fieldName()]));
    }

    /**
     * Validate's values
     * @param $value Value to be validate
     * @return Nothing
     */
    function validate($value)
    {
      return "";
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
      return Array($this->fieldName()=>($rec1[$this->fieldName()]+$rec2[$this->fieldName()]));
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

  }
  
?>

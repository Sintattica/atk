<?php
  /**
   * Attribute-/relationflags. The following flags can be used for attributes/relations.
   * WARNING: flags may *not* exceed 2^31 (2147483648), because that's the integer limit
   *          beyond which the bitwise operators won't work anymore!
   */

  /* "database-level" processing flags */
  define("AF_OBLIGATORY",                           1); // value must be entered
  define("AF_UNIQUE",                               2); // value must be unique
  define("AF_PRIMARY",                4|AF_OBLIGATORY); // part of primary-key node
  define("AF_AUTO_INCREMENT",                       8); // auto-increment field
  define("AF_AUTOINCREMENT",        AF_AUTO_INCREMENT); // auto-increment flag is often mistyped

  /* hide flags */
  define("AF_HIDE_LIST",                           16); // don't show in record lists
  define("AF_HIDE_ADD",                            32); // don't show on add pages (unless obligatory)
  define("AF_HIDE_EDIT" ,                          64); // don't show on edit pages
  define("AF_HIDE_SELECT",                        128); // don't show on select pages.
  define("AF_HIDE_VIEW"	 ,                        256); // don't show on view pages
  define("AF_HIDE_SEARCH",                        512); // not searchable in extended search
  define("AF_FORCE_LOAD",                        1024); // load always, even if not displayed anywhere
  define("AF_HIDE",          AF_HIDE_EDIT|AF_HIDE_ADD|  // attribute is totally hidden
                          AF_HIDE_LIST|AF_HIDE_SEARCH|
                          AF_HIDE_VIEW|AF_HIDE_SELECT);

  /* readonly flags */
  define("AF_READONLY_ADD",                      2048); // readonly in add
  define("AF_READONLY_EDIT",                     4096); // readonly when edited
  define("AF_READONLY",              AF_READONLY_EDIT|
                                      AF_READONLY_ADD); // always readonly

  /* display-related processing flags */
  define("AF_NO_LABEL",                          8192); // no label in forms
  define("AF_NOLABEL",                    AF_NO_LABEL); // no label (mistyped)
  define("AF_BLANK_LABEL",                      16384); // blank label in forms
  define("AF_BLANKLABEL",              AF_BLANK_LABEL); // blank label (mistyped)
  define("AF_NO_SORT",                          32768); // cannot be sorted in recordlists.
  define("AF_NOSORT",                      AF_NO_SORT); // no-sort flag is often mistyped
  define("AF_SEARCHABLE",                       65536); // Attribute is searchable in list views
  define("AF_TOTAL",                           131072); // The attribute will have a 'total' column in lists.
  define("AF_POPUP",                           262144); // if supported, use pop-up window

  /* other processing flags */
  define("AF_CASCADE_DELETE",                  524288); // delete function is called when owning node is deleted
  define("AF_LARGE",                          1048576); // will have a large ammount of records (relation)
  define("AF_NO_FILTER",                      2097152); // ignore filters when selecting records (relation)
  define("AF_PARENT",                         4194304); // parent field for parent child relations (treeview)
  define("AF_NO_QUOTES",                      8388608); // no quotes are used when adding to database
  define("AF_MULTILANGUAGE",                 16777216); // multi-language field
  define("AF_AUTOKEY",             AF_PRIMARY|AF_HIDE|  // shortcut for hidden auto-incremented primary-key
                                     AF_AUTOINCREMENT);

  /* flags specific for the atkFileAttribute */
  define("AF_FILE_NO_UPLOAD",                33554432); // unable to upload files
  define("AF_FILE_NO_SELECT",                67108864); // unable to select files
  define("AF_FILE_NO_DELETE",               134217728); // unable to delete files
  define("AF_FILE_NO_AUTOPREVIEW",          268435456); // don't try to detect the file type (shows only filename)
  define("AF_FILE_PHYSICAL_DELETE",         536870912); // removes the file physically
  define("AF_FILE_POPUP",                    AF_POPUP); // show preview in popup instead of inline

  /* flag specific for atkMatrixRelation */
  define("AF_MATRIX_ACYCLIC",       AF_FILE_NO_UPLOAD); // checks if a matrix is acyclic

  /* flags specific for atkPasswordAttribute */
  define("AF_PASSWORD_NOVALIDATE",  AF_FILE_NO_UPLOAD); // disables password check when editing password field
  define("AF_PASSWORD_NO_VALIDATE", AF_FILE_NO_UPLOAD); // disables password check when editing password field

  /**
   * The atkAttribute class represents an attribute of an atkNode.
   * An atkAttribute has a name and a set of parameters that
   * control its behaviour, like whether an atkAttribute
   * is obligatory, etc.
   *
   * @author Ivo Jansch <ivo@achievo.org>
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
     * The attribute flags (see above)
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
                '" value="'.htmlspecialchars($record[$this->fieldName()]).'">';
      return $result;
    }

    /**
     * Adds the attribute's edit / hide HTML code to the edit array. This method
     * is called by the node if it want the data needed to create an edit form.
     * @param $mode     the edit mode ("add" or "edit")
     * @param $arr      pointer to the edit array
     * @param $defaults pointer to the default values array
     * @param $error    pointer to the error array
     * @param $prefix   the fieldprefix
     */
    function addToEditArray($mode, &$arr, &$defaults, &$error, $fieldprefix)
    {
      /* hide */
      if (($mode=="edit" && $this->hasFlag(AF_HIDE_EDIT)) || ($mode=="add" && $this->hasFlag(AF_HIDE_ADD)))
      {
        /* when adding, there's nothing to hide... */
        if ($mode=="edit" || ($mode=="add" && !$this->isEmpty($defaults)))
          $arr["hide"][] = $this->hide($defaults, $fieldprefix, $mode);
      }

      /* edit */
      else
      {
        $entry = array("name" => $this->m_name, "obligatory" => $this->hasFlag(AF_OBLIGATORY), "attribute" => &$this);

        /* label? */
        if     ($this->hasFlag(AF_NOLABEL))    $entry["label"] = NULL;
        elseif ($this->hasFlag(AF_BLANKLABEL)) $entry["label"] = "";
        else   $entry["label"] = text($this->m_name, $this->m_ownerInstance->m_type);

        /* error? */
        $entry["error"] = FALSE;
        for ($i = 0; $i < count($error) && !$entry["error"]; $i++)
          if ($error[$i]['attrib_name'] == $this->fieldName()) $entry["error"] = TRUE;

        /* readonly */
        if (($mode=="edit" && $this->hasFlag(AF_READONLY_EDIT)) || ($mode=="add" && $this->hasFlag(AF_READONLY_ADD)))
        {
          $entry["html"] = $this->hide($defaults, $fieldprefix);
          $funcname = $this->m_name."_display";
          if (method_exists($this->m_ownerInstance, $funcname))
            $entry["html"] .= $this->m_ownerInstance->$funcname($defaults);
          else $entry["html"] .= $this->display($defaults);
        }

        /* normal */
        else
        {
          $funcname = $this->m_name."_edit";
          if (method_exists($this->m_ownerInstance, $funcname))
            $entry["html"] .= $this->m_ownerInstance->$funcname($defaults, $fieldprefix, $mode);
          else $entry["html"] .= $this->edit($defaults, $fieldprefix, $mode);
        }

        $arr["fields"][] = $entry;
      }
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
     * @param $mode Indicates what kind of query is being processing:
     *              This can be any action performed on a node (edit, add, etc)
     *              Mind you that "add" and "update" are the actions that store
     *              something in the database, whereas the rest are probably
     *              select queries.
     * @return New SQL Query
     */
    function addToQuery(&$query, $tablename="", $fieldaliasprefix="", $rec="", $level, $mode)
    {
      if ($mode == "add" || $mode == "update")
        $query->addField($this->fieldName(),$this->value2db($rec),"","",!$this->hasFlag(AF_NO_QUOTES));
      else
        $query->addField($this->fieldName(),"",$tablename,$fieldaliasprefix,!$this->hasFlag(AF_NO_QUOTES));
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
      global $g_layout;

      $attribname = $this->fieldName();
      $this->m_size = min($metadata[$attribname]['len'], $g_layout->maxInputSize());
      $this->m_searchsize = min($metadata[$attribname]['len'], $g_layout->searchSize());
      $this->m_maxsize = $metadata[$attribname]['len'];

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
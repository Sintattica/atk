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
  define("AF_ML",                            16777216); // multi-language field
  define("AF_MULTILANGUAGE",                    AF_ML);
  define("AF_AUTOKEY",             AF_PRIMARY|AF_HIDE|  // shortcut for hidden auto-incremented primary-key
                                     AF_AUTOINCREMENT);

  /* flag (values) that can be used for attribute specific flags
   * NOTE: Attribute specific flags aren't good behaviour, but for 
   * compatibility reasons we support them anyway. Newly derived attributes 
   * should not use these specific flags, but work with extra parameters.
   */
  define("AF_SPECIFIC_1",                    33554432); // specific attribute flag 1
  define("AF_SPECIFIC_2",                    67108864); // specific attribute flag 2
  define("AF_SPECIFIC_3",                   134217728); // specific attribute flag 3
  define("AF_SPECIFIC_4",                   268435456); // specific attribute flag 4
  define("AF_SPECIFIC_5",                   536870912); // specific attribute flag 5  

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
    
    var $m_dbfieldtype = "";

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
     *        $this->addAttribute(new atkAttribute("name",AF_OBLIGATORY, 30));
     *
     * @param $name  Name of the attribute
     * @param $flags Flags for the attribute
     * @param $size  The size(s) of the attribute. See the $size parameter
     *               of the setAttribSize() method for more information
     *               on the possible values of this parameter.
     *
     * @Note: If you want to use the db/ddl utility class to 
     *        automatically generate the table, the $size parameter must be 
     *        set, for it will use the size specified here to determine the 
     *        field length. (Derived classes might have reasonable default
     *        values, but the standard atkAttribute doesn't.)
     *     
     */
    function atkAttribute($name, $flags = 0, $size = 0)
    {
      $this->m_name = $name;
      $this->m_flags = (double)$flags;
      $this->setAttribSize($size);      
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
     * Adds a flag to the attribute
     * Note that adding flags at any time after the constructor might not 
     * always work. There are flags that are processed only at 
     * constructor time.
     * @param $flag the flag you want to add
     */
    function addFlag($flag)
    {
      $this->m_flags |= $flag;
    }

    /**
     * Removes a flag from the attribute
     * Note that removing flags at any time after the constructor might not
     * always work. There are flags that have already been processed at 
     * constructor time, so removing them will be futile.
     * @param $flag The flag that you want to remove
     */
    function removeFlag($flag)
    {
      $this->m_flags ^= $flag;
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
      // the next if-statement is a workaround for derived attributes which do
      // not override the hide() method properly. This will not give them a 
      // working hide() functionality but at least it will not give error messages.
      if (!is_array($record[$this->fieldName()])) 
      {
        $result = '<input type="hidden" name="'.$fieldprefix.$this->formName().
                  '" value="'.htmlspecialchars($record[$this->fieldName()]).'">';
        return $result;
      }
      else atkdebug("Warning attribute ".$this->m_name." has no proper hide method!");
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
        else   $entry["label"] = $this->label($defaults);

        /* error? */
        $entry["error"] = FALSE;
        for ($i = 0; $i < count($error) && !$entry["error"]; $i++)
          if ($error[$i]['attrib_name'] == $this->fieldName() ||
             atk_in_array($this->fieldName(), $error[$i]['attrib_name']))
             $entry["error"] = TRUE;

        /* readonly */
        if (($mode=="edit" && $this->hasFlag(AF_READONLY_EDIT)) || ($mode=="add" && $this->hasFlag(AF_READONLY_ADD)))
        {
          $entry["html"] = $this->hide($defaults, $fieldprefix);
          $funcname = $this->m_name."_display";
          if (method_exists($this->m_ownerInstance, $funcname))
            $entry["html"] .= $this->m_ownerInstance->$funcname($defaults, "view");
          else $entry["html"] .= $this->display($defaults, "view");
        }

        /* normal */
        else
        {
          $funcname = $this->m_name."_edit";
          if ($this->m_name != "action" && method_exists($this->m_ownerInstance, $funcname))
            $entry["html"] .= $this->m_ownerInstance->$funcname($defaults, $fieldprefix, $mode);
          else $entry["html"] .= $this->edit($defaults, $fieldprefix, $mode);
        }

        $arr["fields"][] = $entry;
      }
    }

    /**
     * Adds the attribute / field to the list header. This includes the column name and search field.
     * @param $action the action that is being performed on the node
     * @param $arr reference to the the recordlist array
     * @param $fieldprefix the fieldprefix
     * @param $flags the recordlist flags
     * @param $atksearch the current ATK search list (if not empty)
     * @param $atkorderby the current ATK orderby string (if not empty)
     * @see atkNode::listArray
     */
    function addToListArrayHeader($action, &$arr, $fieldprefix, $flags, $atksearch, $atkorderby)
    {
      global $g_sessionManager;

      if (!$this->hasFlag(AF_HIDE_LIST) && !($this->hasFlag(AF_HIDE_SELECT) && $action == "select"))
      {
        $arr["heading"][$fieldprefix.$this->fieldName()]["title"] = text($this->fieldName(), $this->m_ownerInstance->m_type);

        if (!hasFlag($flags, RL_NO_SORT) && !$this->hasFlag(AF_NO_SORT))
        {
          $order = $this->m_ownerInstance->m_table.".".$this->fieldName();
          if ($atkorderby == $order) $order .= " DESC";
          $arr["heading"][$fieldprefix.$this->fieldName()]["url"] =
            session_url($_SERVER["PHP_SELF"].'?atknodetype='.$this->m_ownerInstance->atkNodeType().'&atkaction='.$action.'&atkorderby='.rawurlencode($order));
        }

        if (!hasFlag($flags, RL_NO_SEARCH) && $this->hasFlag(AF_SEARCHABLE))
          $arr["search"][$fieldprefix.$this->fieldName()] = $this->search($atksearch);
      }
    }

    /**
     * Adds the attribute / field to the list row. And if the row is totalisable also to the total.
     * @param $action the action that is being performed on the node
     * @param $arr reference to the the recordlist array
     * @param $nr the current row number
     * @param $fieldprefix the fieldprefix
     * @param $flags the recordlist flags
     * @see atkNode::listArray
     */
    function addToListArrayRow($action, &$arr, $nr, $fieldprefix, $flags)
    {
      if (!$this->hasFlag(AF_HIDE_LIST) && !($this->hasFlag(AF_HIDE_SELECT) && $action == "select"))
      {
        /* an <attributename>_display function may be provided in a derived class to display an attribute */
        $function = $this->m_name."_display";
        if (method_exists($this->m_ownerInstance, $function))
          $arr["rows"][$nr]["data"][$fieldprefix.$this->fieldName()] = $this->m_ownerInstance->$function($arr["rows"][$nr]["record"], "list");
        else $arr["rows"][$nr]["data"][$fieldprefix.$this->fieldName()] = $this->display($arr["rows"][$nr]["record"], "list");

        /* totalisable? */
        if ($this->hasFlag(AF_TOTAL))
        {
          $sum = $this->sum($arr["totalraw"], $arr["rows"][$nr]["record"], $fieldprefix);
          $arr["totalraw"][$fieldprefix.$this->fieldName()] = $sum[$fieldprefix.$this->fieldName()];
          if (method_exists($this->m_ownerInstance, $function))
            $arr["total"][$fieldprefix.$this->fieldName()] = $this->m_ownerInstance->$function($sum, "list");
          else $arr["total"][$fieldprefix.$this->fieldName()] = $this->display($sum, "list");
        }
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
      // the next if-statement is a workaround for derived attributes which do
      // not override the display() method properly. This will not give them a 
      // working display() functionality but at least it will not give error messages.
      if (!is_array($record[$this->fieldName()])) 
      {
        // default behaviour is that we display a value 'as is'.
        return nl2br(htmlspecialchars($record[$this->fieldName()]));
      }
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
     * @return true if successful, false otherwise
     */
    function delete($record)
    {
      // delete is only of interest for special attributes like relations, or file attributes.
      return true;
    }

    /**
     * returns a record which is the sum of two records
     */
    function sum($rec1, $rec2, $fieldprefix="")
    {
      return array($fieldprefix.$this->fieldName() => ($rec1[$fieldprefix.$this->fieldName()]+$rec2[$fieldprefix.$this->fieldName()]));
    }

    /**
     * Fetch the metadata about this attrib from the table metadata
     */
    function fetchMeta($metadata)
    {      

      $attribname = $this->fieldName();
      // Only use meta info if size is not already set manual. (constructor or setAttribSize)
      // If both are set, use the smallest value.
      if ($this->m_size > 0) $this->m_size = min($this->m_size, $metadata[$attribname]['len'], $this->maxInputSize());
      else $this->m_size = min($metadata[$attribname]['len'], $this->maxInputSize());
      
      if ($this->m_searchsize > 0) $this->m_searchsize = min($this->m_searchsize, $metadata[$attribname]['len'], 20);
      else $this->m_searchsize = min($metadata[$attribname]['len'], 20);   
      
      if ($this->m_maxsize > 0) $this->m_maxsize = min($this->m_maxsize, $metadata[$attribname]['len']);
      else $this->m_maxsize = $metadata[$attribname]['len'];
      
      // TODO FIXME: The metadata contains the real field type. $this->m_dbfieldtype should be
      // set accordingly. Currently the metadata contains database specific types, so this
      // feature is not yet implemented, until metadata contains generic field types.      
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

    /**
     * This function is called by atkNode to determine if a field
     * needs to be saved to the database in an addDb call.
     *
     * @param $record The record that is about to be saved
     */
    function needsInsert($record)
    {
      return (!$this->hasFlag(AF_HIDE_ADD) || 
              $this->hasFlag(AF_AUTO_INCREMENT) || 
              !$this->isEmpty($record));

      // If we are set to hide_add, we will only insert into the
      // db if a value has been set (for example by an initial_values
      // method). Also, autoincrement fields might be hidden, and their
      // value is still empty, but they do need to be inserted lateron.
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

    /**
     * Set the size(s) of the attribute
     *     
     * @param $size The maximum number of characters that can be entered.
     *        If not specified, or set to 0, the max. size is automatically 
     *        retrieved from the table metadata.      
     *
     *        By default, the size of the edit box is the same as the maximum 
     *        number of chars that can be entered (as long as it fits on 
     *        screen). You can however pass an array of 2 or 3 numbers instead 
     *        of a single number. In this case, the array is interpreted as 
     *        follows:
     *        - $size[0] - The maximum size that can be entered
     *        - $size[1] - The size of the input box in add/edit forms
     *        - $size[2] - The size of the search box
     *
     *        If $size[2] is not specified, $size[1] will be used instead. 
     *        If $size[1] is not specified, or the passed value is not an 
     *        array, all 3 sizes will default to the first value.     
     *
     * @note  The sizes that are actually used depend both on the specified 
     *        size and the size of the field in the database. Usually, these 
     *        are the same. In the case they differ, the smallest of the 2 
     *        will be used.
     */
    function setAttribSize($size)
    {
      if (is_array($size) && count($size)>0)
      {
        if (!empty($size[2]))
        {
          $this->m_searchsize = $size[2]; 
        }
        else
        {
          $this->m_searchsize = (empty($size[1])?$size[0]:$size[1]);
        }
        $this->m_size = (empty($size[1])?$size[0]:$size[1]);
        $this->m_maxsize = $size[0];        
      }
      else
      {
        if ($size>0)
        {
          $this->m_maxsize = $this->m_size = $this->m_searchsize = $size;
        }
      }
    }
    
    /**
     * Return the database field type of the attribute.
     * 
     * Note that the type returned is a 'generic' type. Each database
     * vendor might have his own types, therefor, the type should be
     * converted to a database specific type using $g_db->fieldType().
     *
     * If the type was read from the table metadata, that value will 
     * be used. Else, the attribute will analyze its flags to guess 
     * what type it should be. If AF_AUTO_INCREMENT is set, the field
     * is probaly "number". If not, it's probably "string".
     *
     * @return The 'generic' type of the database field for this
     *         attribute. 
     *
     * Note: Derived attributes should override this method if they 
     *       use other field types than string or number. If the 
     *       derived attribute is one that can not be stored in the
     *       database, an empty string should be returned.
     */
    function dbFieldType()
    {
      if ($this->m_dbfieldtype=="")      
      {        
        $this->m_dbfieldtype = ($this->hasFlag(AF_AUTO_INCREMENT)?"number":"string");       
      }
      return $this->m_dbfieldtype;
    }
    
    /**
     * Return the size of the field in the database.
     *
     * If 0 is returned, the size is unknown. In this case, the
     * return value should not be used to create table columns. 
     *
     * Ofcourse, the size does not make sense for every field type. 
     * So only interpret the result if a size has meaning for
     * the field type of this attribute. (For example, if the 
     * database field is of type 'date', the size has no meaning)
     * 
     * Note that derived attributes might set a dot separated size, 
     * for example to store decimal numbers. The number after the dot
     * should be interpreted as the number of decimals.
     *
     */
    function dbFieldSize()
    {
      if ($this->m_maxsize!=0) return $this->m_maxsize;
      else
      {
        if ($this->dbFieldType()=="number")
        {
          return "10"; // default for numbers.
        }
        else
        {
          return "100"; // default for strings.
        }
      }
    }
    
    /**
     * Return the label of the field.
     */
    function label($record = array())
    {    
      return text($this->fieldName(),$this->m_ownerInstance->m_type, $this->m_ownerInstance->m_module);
    }

    /**
     * This function is used to check whether a attribute has a store function or not.
     * It can be overridden to use the store function only in a certain mode.
     */
    function hasStore($mode)
    {
      if (method_exists($this,"store"))
        return true;
      else
        return false;
    }
    
    /**
     * Determine the maximum length an input field may be.
     */
    function maxInputSize()
    {
      $configsize = atkconfig("max_input_size");
      return ($configsize==""?70:$configsize);
    }
            
  }

?>
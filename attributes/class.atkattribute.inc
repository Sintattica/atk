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
   * @copyright (c)2000-2004 Ivo Jansch
   * @copyright (c)2000-2004 Ibuildings.nl BV
   * @license http://www.achievo.org/atk/licensing ATK Open Source License
   *
   * @version $Revision$
   * $Id$
   */     

  /**
   * Attributeflags. The following flags can be used for attributes
   * @internal WARNING: flags may *not* exceed 2^31 (2147483648), because
   * that's the integer limit beyond which the bitwise operators won't 
   * work anymore!
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
                                     
  /*                                     
   * flag (values) that can be used for attribute specific flags
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
   * Storage types, used by the storageType() and related methods
   */
  define("NOSTORE"   ,0);
  define("PRESTORE"  ,1);
  define("PRELOAD",   1);
  define("POSTSTORE" ,2);
  define("POSTLOAD", 2);  
  define("ADDTOQUERY",4);  
  
  /**
   * The atkAttribute class represents an attribute of an atkNode.
   * An atkAttribute has a name and a set of parameters that
   * control its behaviour, like whether an atkAttribute
   * is obligatory, etc.
   *
   * @author Ivo Jansch <ivo@achievo.org>
   * @package atk
   * @subpackage attributes
   */  
  class atkAttribute
  {
    /**
     * The name of the attribute
     * @var String 
     * @access private
     */
    var $m_name;

    /**
     * The attribute flags (see above)
     * @var int 
     * @access private
     */
    var $m_flags = 0;

    /**
     * The owner of the attribute (set by atknode)
     * @var String 
     */
    var $m_owner = "";

    /**
     * Instance that owns this attribute
     * @var atkNode
     */
    var $m_ownerInstance="";

    /**
     * The size the attribute's field.
     * @access private
     * @var int
     */
    var $m_size = 0;
    
    /**
     * The size the attribute's search input field.
     * @access private
     * @var int
     */
    var $m_searchsize = 0;
    
    /**
     * The maximum size the attribute's value may have in the database.
     * @access private
     * @var int
     */
    var $m_maxsize = 0;
    
    /**
     * The database fieldtype.
     * @access private
     * @var String 
     */
    var $m_dbfieldtype = "";

    /**
     * The order of the attribute within its node.
     * @var int 
     */
    var $m_order = 0;
    
    /**
     * Index of the attribute within its node. (I'm not sure this is still used)
     * @var int 
     * @access private
     */
    var $m_index = 0;
        
    /**
     * The tab(s) on which the attribute lives.
     * @access private
     * @var mixed 
     */
    var $m_tabs="*";

    /**
     * Constructor
     *
     * <b>Example:</b>
     *        $this->add(new atkAttribute("name",AF_OBLIGATORY, 30));
     *
     * Note: If you want to use the db/ddl utility class to 
     *       automatically generate the table, the $size parameter must be 
     *       set, for it will use the size specified here to determine the 
     *       field length. (Derived classes might have reasonable default
     *        values, but the standard atkAttribute doesn't.)
     *
     * @param String $name Name of the attribute (unique within a node, and
     *                     for most attributes, corresponds to a field in 
     *                     the database. 
     * @param int $flags Flags for the attribute.
     * @param mixed $size  The size(s) of the attribute. See the $size
     *                     parameter of the setAttribSize() method for more 
     *                     information on the possible values of this
     *                     parameter.
     *     
     */
    function atkAttribute($name, $flags = 0, $size = 0)
    {
      $this->m_name = $name;
      $this->m_flags = (double)$flags;
      $this->setAttribSize($size);      
      atkimport("atk.keyboard.atkkeyboard");
    }

    /**
     * Check if the attribute has a certain flag.
     * @param int $flag The flag you want to check
     * @return boolean
     */
    function hasFlag($flag)
    {      
      return (($this->m_flags & $flag) == $flag);
    }

    /**
     * Adds a flag to the attribute.
     * Note that adding flags at any time after the constructor might not 
     * always work. There are flags that are processed only at 
     * constructor time.
     * @param int $flag The flag to add to the attribute
     */
    function addFlag($flag)
    {
      $this->m_flags |= $flag;
    }

    /**
     * Removes a flag from the attribute.
     *
     * Note that removing flags at any time after the constructor might not
     * always work. There are flags that have already been processed at 
     * constructor time, so removing them will be futile.
     * @param int $flag The flag to remove from the attribute
     */
    function removeFlag($flag)
    {
      $this->m_flags ^= $flag;
    }

    /**
     * Returns the name of the attribute. 
     *
     * For most attributes, this corresponds to the name of the field in the
     * database. For some attributes though (like one2many relations), the
     * name is a mere identifier within a node. This method always returns
     * the attribute name, despite the 'field' prefix of the method.
     *
     * @return String fieldname
     */
    function fieldName()
    {
      return $this->m_name;
    }

    /**
     * Retrieve the name of the attribute in HTML forms.
     * @deprecated HTML formname and fieldname are equal, use fieldName 
     *                  instead.
     * @return String Name of the attribute in HTML forms
     */
    function formName()
    {
      return $this->m_name;
    }

    /**
     * Check if a record has an empty value for this attribute.
     * @param array $record The record that holds this attribute's value.
     * @return boolean
     */
    function isEmpty($record)
    {
      return ($record[$this->fieldName()]=="");
    }

    /**
     * Converts the internal attribute value to one that is understood by the
     * database.
     *
     * For the regular atkAttribute, this means escaping things like
     * quotes and slashes. Derived attributes may reimplement this for their
     * own conversion. 
     * This is the exact opposite of the db2value method.
     * 
     * @param array $rec The record that holds this attribute's value.
     * @return String The database compatible value
     */
    function value2db($rec)
    {
      return escapeSQL($rec[$this->fieldName()]);
    }

    /**
     * Converts a database value to an internal value. 
     *
     * For the regular atkAttribute, this means stripping slashes.
     * Derived attributes may reimplement this for their own conversion.
     * (In which case, the return type might be 'mixed')
     * 
     * This is the exact opposite of the value2db method.
     *
     * @param array $rec The database record that holds this attribute's value
     * @return String The internal value
     */
    function db2value($rec)
    {
      return stripslashes($rec[$this->fieldName()]);
    }

    /**
     * Convert values from an HTML form posting to an internal value for 
     * this attribute. 
     *
     * For the regular atkAttribute, this means getting the field with the
     * same name as the attribute from the html posting. 
     *
     * @param array $postvars The array with html posted values ($_POST, for 
     *                        example) that holds this attribute's value.
     * @return String The internal value
     */
    function fetchValue($postvars)
    {
      return $postvars[$this->fieldName()];
    }

    /**
     * Returns a piece of html code that can be used in a form to edit this
     * attribute's value.
     *
     * @param array $record The record that holds the value for this attribute.
     * @param String $fieldprefix The fieldprefix to put in front of the name
     *                            of any html form element for this attribute.
     * @return String A piece of htmlcode for editing this attribute
     */
    function edit($record="", $fieldprefix="")
    {
      $id = $this->getHtmlId($fieldprefix);
      $this->registerKeyListener($id, KB_CTRLCURSOR|KB_UPDOWN);  
    
      $result = '<input type="text" id="'.$id.'" name="'.$id.
                '" value="'.htmlspecialchars($record[$this->fieldName()]).'"'.
                ($this->m_size > 0 ? ' size="'.$this->m_size.'"' : '').
                ($this->m_maxsize > 0 ? ' maxlength="'.$this->m_maxsize.'"' : '').'>';

      return $result;
    }

    /**
     * Returns a piece of html code for hiding this attribute in an HTML form,
     * while still posting its value. (<input type="hidden">)
     *
     * @param array $record The record that holds the value for this attribute
     * @param String $fieldprefix The fieldprefix to put in front of the name
     *                            of any html form element for this attribute.
     * @return String A piece of htmlcode with hidden form elements that post
     *                This attribute's value without showing it.
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
     * Return the html identifier (id="") of the attribute. (unique within a
     * page)
     * @param String $fieldprefix The fieldprefix to put in front of the name
     *                            of any html form element for this attribute.
     * @return String the HTML identifier.
     */
    function getHtmlId($fieldprefix)
    {
      return $fieldprefix.$this->fieldName();
    }

    /**
     * Adds the attribute's edit / hide HTML code to the edit array.
     *
     * This method is called by the node if it wants the data needed to create
     * an edit form.
     * 
     * This is a framework method, it should never be called directly.
     *
     * @param String $mode     the edit mode ("add" or "edit")
     * @param array  $arr      pointer to the edit array
     * @param array  $defaults pointer to the default values array
     * @param array  $error    pointer to the error array
     * @param String $prefix   the fieldprefix
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
        
        $entry["id"] = $this->getHtmlId($fieldprefix);

        /* label? */        
        $entry["label"] = $this->getLabel($defaults);

        /* error? */
        $entry["error"] = $this->getError($error);

        // on which tab?
        $entry["tabs"] = $this->m_tabs;     

        /* the actual edit contents */
        $entry["html"] = $this->getEdit($mode, $defaults, $fieldprefix);

        $arr["fields"][] = $entry;
      }
    }
    
    /**
     * Put the attribute on one or more tabs.
     * @param array $tabs An array of tabs on which the attribute should
     *                    be displayed.
     */
    function setTabs($tabs)
    {      
      if ($tabs==NULL)
      {
        $this->m_tabs = array("default");
      }
      else
      {
        $this->m_tabs = $tabs;
      }
    }        
    
    /**
     * Retrieve the html code for placing this attribute in an edit page.
     *
     * The difference with the edit() method is that the edit() method just
     * generates the HTML code for editing the attribute, while the getEdit()
     * method is 'smart', and implements a hide/readonly policy based on
     * flags and/or custom override methodes in the node. 
     * (<attributename>_edit() and <attributename>_display() methods)
     *
     * Framework method, it should not be necessary to call this method
     * directly.
     *
     * @param String $mode The edit mode ("add" or "edit")
     * @param array $defaults The record holding the values for this attribute
     * @param String $fieldprefix The fieldprefix to put in front of the name
     *                            of any html form element for this attribute.
     * @return String the HTML code for this attribute that can be used in an
     *                editpage.
     */
    function getEdit($mode, &$defaults, $fieldprefix)
    {
      /* readonly */
      $result = "";
      if (($mode=="edit" && $this->hasFlag(AF_READONLY_EDIT)) || ($mode=="add" && $this->hasFlag(AF_READONLY_ADD)))
      {
        $result.= $this->hide($defaults, $fieldprefix);
        $funcname = $this->m_name."_display";
        if (method_exists($this->m_ownerInstance, $funcname))
        {
          $result.= $this->m_ownerInstance->$funcname($defaults, "view");
        }
        else 
        {
          $result.= $this->display($defaults, "view");
        }
      }
      /* normal */
      else
      {
        $funcname = $this->m_name."_edit";
        // The check for 'action' is a workaround for a weird php bug, which chokes on funcnames with 'action' in them.
        if ($this->m_name != "action" && method_exists($this->m_ownerInstance, $funcname))
        {
          $result .= $this->m_ownerInstance->$funcname($defaults, $fieldprefix, $mode);
        }
        else 
        {
          $result .= $this->edit($defaults, $fieldprefix, $mode);
        }
      }
      return $result;
    }
    
    /**
     * Check if this attribute has errors in the specified error list.     
     *
     * @param array $errors The error list is one that is stored in the 
     *                      "atkerror" section of a record, for example
     *                      generated by validate() methods.
     * @return boolean 
     */
    function getError($errors)
    {      
      for ($i = 0; $i < count($errors) && !$error; $i++)
      {
        if ($errors[$i]['attrib_name'] == $this->fieldName() ||
            atk_in_array($this->fieldName(), $errors[$i]['attrib_name']))
        {
          return true;
        }
      }
      return false;
    }

    /**
     * Adds the attribute / field to the list header. This includes the column name and search field.
     *
     * Framework method. It should not be necessary to call this method directly.
     *
     * @param String $action the action that is being performed on the node
     * @param array  $arr reference to the the recordlist array
     * @param String $fieldprefix the fieldprefix
     * @param int    $flags the recordlist flags
     * @param array  $atksearch the current ATK search list (if not empty)
     * @param String $atkorderby the current ATK orderby string (if not empty)
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
     *
     * Framework method. It should not be necessary to call this method directly.
     *
     * @param String $action the action that is being performed on the node
     * @param array  $arr reference to the the recordlist array
     * @param int    $nr the current row number
     * @param String $fieldprefix the fieldprefix
     * @param int    $flags the recordlist flags
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
     * Returns a piece of html code that can be used to get search terms input
     * from the user.
     *
     * The framework calls this method to display the searchbox
     * in the search bar of the recordlist, and to display a more extensive
     * search in the 'extended' search screen.
     * The regular atkAttributes returns a simple text input box for entering
     * a keyword to search for.
     *
     * @param array $record Array with values
     * @param boolean $extended if set to false, a simple search input is
     *                          returned for use in the searchbar of the 
     *                          recordlist. If set to true, a more extended
     *                          search may be returned for the 'extended' 
     *                          search page. The atkAttribute does not
     *                          make a difference for $extended is true, but
     *                          derived attributes may reimplement this.
     *
     * @return String A piece of html-code
     */
    function search($record="", $extended=false)
    {
      $id = "atksearch[".$this->formName()."]";
      $this->registerKeyListener($id, KB_CTRLCURSOR|KB_UPDOWN);  
      $result= '<input type="text" id="'.$id.'" name="'.$id.'" value="'.$record[$this->fieldName()].'"'.
                ($this->m_searchsize > 0 ? ' size="'.$this->m_searchsize.'"' : '').
                ($this->m_maxsize > 0 ? ' maxlength="'.$this->m_maxsize.'"' : '').'>';
      return $result;
    }

    /**
     * Creates a search condition for a given search value, and adds it to the
     * query that will be used for performing the actual search.
     *
     * @param atkQuery $query The query to which the condition will be added.
     * @param String $table The name of the table in which this attribute
     *                      is stored
     * @param mixed $value The value the user has entered in the searchbox
     * @param String $searchmode The searchmode to use. This can be any one
     *                           of the supported modes, as returned by this
     *                           attribute's getSearchModes() method.
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
     * Returns a displayable string for this value, to be used in HTML pages.
     *
     * The regular atkAttribute uses PHP's nl2br() and htmlspecialchars() 
     * methods to prepare a value for display.
     *
     * @param array $record The record that holds the value for this attribute
     * @param String $mode The display mode ("view" for viewpages, or "list" 
     *                     for displaying in recordlists). The regular
     *                     atkAttribute does not use this parameter, but
     *                     derived attributes may use it to distinguish
     *                     between the two display modes.
     * @return String HTML String 
     */
    function display($record, $mode="")
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
     * Checks if a value is valid. 
     *
     * The regular atkAttribute has no specific validation. Derived attributes
     * may override this method to perform custom validation.
     * Note that obligatory and unique fields are checked by the
     * atkNodeValidator, and not by the validate() method itself.
     *
     * @param array $record The record that holds the value for this 
     *                      attribute. If an error occurs, the error will  
     *                      be stored in the 'atkerror' field of the record.
     * @param String $mode The mode for which should be validated ("add" or
     *                     "update")
     */     
    function validate(&$record, $mode)
    {
    }

    /**
     * Adds this attribute to database queries. 
     *
     * Database queries (select, insert and update) are passed to this method
     * so the attribute can 'hook' itself into the query.
     *
     * Framework method. It should not be necessary to call this method 
     * directly. Derived attributes that consist of more than a single simple
     * database field (like relations for example), may have to reimplement
     * this method.
     *     
     * @param atkQuery $query The SQL query object
     * @param String $tablename The name of the table of this attribute
     * @param String $fieldaliasprefix Prefix to use in front of the alias
     *                                 in the query.
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
      if ($mode == "add" || $mode == "update")
        $query->addField($this->fieldName(),$this->value2db($rec),"","",!$this->hasFlag(AF_NO_QUOTES));
      else
        $query->addField($this->fieldName(),"",$tablename,$fieldaliasprefix,!$this->hasFlag(AF_NO_QUOTES));
    }

    /**
     * The delete method is called by the framework to inform the attribute
     * that a record is deleted.
     *
     * The regular atkAttribute has no implementation for this method, but
     * derived attributes may override this, to take care of cleanups, cascade
     * deletes etc.
     * Note, that the framework only calls this method if the attribute has
     * the AF_CASCADE_DELETE flag.
     *
     * @param array $record The record that is deleted.
     * @return boolean true if cleanup was successful, false otherwise. 
     */
    function delete($record)
    {
      // delete is only of interest for special attributes like relations, or file attributes.
      return true;
    }

    /**
     * Calculate the sum of 2 records. 
     *
     * This is called by the framework for the auto-totalling feature. Two
     * records are passed, and a record is returned. The reason that the
     * params are entire records instead of plain values, is that derived
     * classes or custom attributes may need information from other attributes
     * too.
     * 
     * @param array $rec1 The first record
     * @param array $rec2 The second record
     * @param String $fieldprefix The prefix that values in $rec1
     *                            and $rec2 have
     * @return array A record containing the sum of $rec1 and $rec2
     */
    function sum($rec1, $rec2, $fieldprefix="")
    {
      return array($fieldprefix.$this->fieldName() => ($rec1[$fieldprefix.$this->fieldName()]+$rec2[$fieldprefix.$this->fieldName()]));
    }

    /**
     * Fetch the metadata about this attrib from the table metadata, and
     * process it. 
     *
     * Lengths for the edit and searchboxes, and maximum lengths are retrieved
     * from the table metadata by this method.
     *
     * @param array $metadata The table metadata from the table for this
     *                        attribute.
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
     *
     * The regular atkAttribute has no implementation for this method, but
     * derived attributes may override this method to perform custom
     * initialization.
     */
    function init()
    {
    }

    /**
     * This function is called to compare if two records are considered equal
     * by this attribute. 
     *
     * The regular atkAttribute performs a simple string match; derived
     * classes may override this method to perform more complex comparisons.
     * 
     * @param array $recA The first record holding a value for this attribute.
     * @param array $recB The second record holding a value for the attribute.
     * @return boolean True if the attribute considers the records equal,
     *                 false if not.
     */
    function equal($recA, $recB)
    {
      return ($recA[$this->fieldName()] == $recB[$this->fieldName()]);
    }

    /**
     * This function is called by the framework to determine if the attribute
     * needs to be saved to the database in an updateDb call.
     *
     * The regular atkAttribute returns false if AF_READONLY_EDIT or
     * AF_HIDE_EDIT are set, but derived attributes may override this
     * behavior.
     *
     * @param array $record The record that is going to be saved.
     * @return boolean True if this attribute should participate in the update
     *                 query; false if not.
     */
    function needsUpdate($record)
    {
      return (!$this->hasFlag(AF_READONLY_EDIT) && !$this->hasFlag(AF_HIDE_EDIT));
    }

    /**
     * This function is called by the framework to determine if the attribute
     * needs to be saved to the database in an addDb call.
     *
     * The regular atkAttribute returns false if the value is empty, or if
     * AF_HIDE is set; true in other cases. Exception: when AF_AUTO_INCREMENT
     * is set, the method always returns true. Derived attributes may override
     * this behavior.
     *
     * @param array $record The record that is going to be saved
     * @return boolean True if this attribute should participate in the add
     &                 query; false if not.
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

    /**
     * Retrieve the list of searchmodes supported by the attribute. 
     *
     * Note that not all modes may be supported by the database driver.
     * Compare this list to the one returned by the databasedriver, to 
     * determine which searchmodes may be used.
     *
     * @return array List of supported searchmodes
     */
    function getSearchModes()
    {
      // exact match and substring search should be supported by any database.
      // (the LIKE function is ANSI standard SQL, and both substring and wildcard
      // searches can be implemented using LIKE)      
      return array("exact","substring","wildcard","regexp");
    }

    /**
     * Set the size(s) of the attribute
     *     
     * @param mixed $size The max. number of characters that can be entered.
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
     *        Note: The sizes that are actually used depend both on the
     *        specified size and the size of the field in the database.
     *        Usually, these are the same. In the case they differ, the
     *        smallest of the 2 will be used.
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
     * Note: Derived attributes should override this method if they 
     *       use other field types than string or number. If the 
     *       derived attribute is one that can not be stored in the
     *       database, an empty string should be returned.
     *
     * @return String The 'generic' type of the database field for this
     *                attribute.      
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
     * @return int The database field size
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
     * Return the label of the attribute. 
     *
     * The regular atkAttribute does not make use of the $record parameter; 
     * The label is based on the attribute name, but is automatically
     * translated. Derived attributes may override this behavior.
     * 
     * @param array $record The record holding the value for this attribute.
     * @return String HTML compatible label for this attribute
     */
    function label($record = array())
    {          
      return text($this->fieldName(),$this->m_ownerInstance->m_type, 
                  $this->m_ownerInstance->m_module);
    }
    
    /** 
     * Get the HTML label of the attribute.
     *
     * The difference with the label() method is that the label method always
     * returns the HTML label, while the getLabel() method is 'smart', by
     * taking the AF_NOLABEL and AF_BLANKLABEL flags into account.
     *
     * @param array $record The record holding the value for this attribute.
     * @return String The HTML compatible label for this attribute, or an 
     *                empty string if the label should be blank, or NULL if no
     *                label at all should be displayed.
     */
    function getLabel($record = array())
    {
      $label = NULL;
      if ($this->hasFlag(AF_NOLABEL)) $label = NULL;
      else if ($this->hasFlag(AF_BLANKLABEL)) $label = "";
      else 
      {
        return $this->label($record);
      }
    }

    /**
     * This function is used to check whether a attribute has a store function
     * or not.
     *
     * It can be overridden to determine when to use store() and when not to 
     * use it.
     * @return boolean true if it has a store method or false if it hasn't.
     * @deprecated Use storageType($mode) instead, which has more detailed
     *             information about storage.
     */
    function hasStore($mode)
    {
      atkdebug("Deprecated use of hasStore");
      $storagetype = $this->storageType($mode);
      return (hasFlag($storagetype, POSTSTORE)
              ||hasFlag($storagetype, PRESTORE)
              );
    }
    
    /*
     * Determine the storage type of this attribute.
     *
     * With this method, the attribute tells the framework whether it wants
     * to be stored in the main query (addToQuery) or whether the attribute
     * has its own store() implementation. The regular atkAttribute checks if
     * a store() method is present, and returns POSTSTORE in this case, or
     * ADDTOQUERY otherwise. Derived attributes may override this behavior.
     *
     * Framework method. It should not be necesary to call this method 
     * directly.
     *
     * @param String $mode The type of storage ("add" or "update")
     *
     * @return int Bitmask containing information about storage requirements.
     *             Note that since it is a bitmask, multiple storage types
     *             could be returned at once.
     *             POSTSTORE  - store() method must be called, after the
     *                          master record is saved.
     *             PRESTORE   - store() must be called, before the master 
     *                          record is saved.
     *             ADDTOQUERY - addtoquery() must be called, so the attribute 
     *                          can nest itself in the master query.
     *             NOSTORE    - nor store(), nor addtoquery() should be
     *                          called (attribute can not be stored in the
     *                          database)
     */
    function storageType($mode)
    {
      // Default backwardscompatible behaviour:
      if (method_exists($this,"store"))
        return POSTSTORE;
      else
        return ADDTOQUERY;
    }
    
    /*
     * Determine the load type of this attribute. 
     *
     * With this method, the attribute tells the framework whether it wants
     * to be loaded in the main query (addToQuery) or whether the attribute 
     * has its own load() implementation. The regular atkAttribute checks if a
     * load() method is present, and returns POSTLOAD in this case, or
     * ADDTOQUERY otherwise. Derived attributes may override this behavior.
     *
     * Framework method. It should not be necesary to call this method 
     * directly.
     *
     * @param String $mode The type of load (view,admin,edit etc)
     *
     * @return int Bitmask containing information about load requirements.
     *             Note that since it is a bitmask, multiple load types
     *             could be returned at once.
     *             POSTLOAD   - load() method must be called, after the
     *                          master record is loaded.
     *             PRELOAD    - load() must be called, before the master 
     *                          record is loaded.
     *             ADDTOQUERY - addtoquery() must be called, so the attribute 
     *                          can nest itself in the master query.
     */
    function loadType($mode)
    {
      // Default backwardscompatible behaviour:
      if (method_exists($this,"load"))      
        return POSTLOAD;
      else
        return ADDTOQUERY;
    }
    
    /**
     * Determine the maximum length an input field may be.
     * @return int
     */
    function maxInputSize()
    {
      $configsize = atkconfig("max_input_size");
      return ($configsize==""?70:$configsize);
    }
    
    /**
     * Get list of additional tabs.
     * 
     * Attributes can add new tabs to tabbed screens. This method will be
     * called to retrieve the tabs. The regular atkAttribute has no
     * implementation for this method. Derived attributes may override this.
     *
     * @param String $action The action for which additional tabs should be 
     *                       loaded.
     * @return array The list of tabs to add to the screen.
     */
    function getAdditionalTabs($action)
    {
      return array();
    }
    
    /**
     * Register keyboard navigation for this attribute.
     *
     * This method is called by the attribute itself to register the keyboard
     * navigation, usually from its edit() method. The regular atkAttribute
     * calls this method once for its default text input box. Derived classes
     * may call this for any input box or control they have.
     *
     * @param String $id The unique HTML id of the form element to which
     *                   navigation is added.
     * @param int $navkeys The mask indicating which keys to support for
     *                     navigation.
     */
    function registerKeyListener($id, $navkeys=KB_CTRLCURSOR)
    {      
      $kb = &atkKeyboard::getInstance();
      $kb->addFormElementHandler($id, $navkeys);
    }

    /**
     * Check if the attribute wants to be shown on a certain tab.
     *
     * @param String $tab The name of the tab to check.
     * @return boolean
     */
    function showOnTab($tab)
    {
      return ($this->m_tabs == "*" || atk_in_array($tab, $this->m_tabs));
    }
    
    /**
     * Check if delete of the record is allowed. 
     *
     * This method is called by the framework to check if the attribute
     * allows the record to be deleted. The default implementation always
     * returns true, but derived attributes may implement their own 
     * logic to prevent deletion of the record. 
     *
     * @return boolean True if delete is allowed, false if not.
     */
    function deleteAllowed()
    {
      return true;
    }
            
  }

?>

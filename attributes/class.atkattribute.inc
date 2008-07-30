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
   * @copyright (c)2000-2006 Ivo Jansch
   * @copyright (c)2000-2006 Ibuildings.nl BV
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

  /**
   * Value must be entered
   *
   * "database-level" processing flag
   */
  define("AF_OBLIGATORY",                           1);

  /**
   * Value must be unique
   *
   * "database-level" processing flag
   */
  define("AF_UNIQUE",                               2);

  /**
   * Part of the primary-key node, also makes it obligatory
   *
   * "database-level" processing flag
   */
  define("AF_PRIMARY",                4|AF_OBLIGATORY);

  /**
   * Auto-increment field
   *
   * "database-level" processing flag
   */
  define("AF_AUTO_INCREMENT",                       8);

  /**
   * Alias for AF_AUTO_INCREMENT (auto-increment flag is often mistyped)
   *
   * "database-level" processing flag
   */
  define("AF_AUTOINCREMENT",        AF_AUTO_INCREMENT);

  /**
   * Don't show in record lists
   *
   * hide flag
   */
  define("AF_HIDE_LIST",                           16);

  /**
   * Don't show on add pages
   *
   * hide flag
   */
  define("AF_HIDE_ADD",                            32);

  /**
   * Don't show on edit pages
   *
   * hide flag
   */
  define("AF_HIDE_EDIT" ,                          64);

  /**
   * Don't show on select pages
   *
   * hide flag
   */
  define("AF_HIDE_SELECT",                        128);

  /**
   * Don't show on view pages
   *
   * hide flag
   */
  define("AF_HIDE_VIEW"	 ,                        256);

  /**
   * Not searchable in extended search
   *
   * hide flag
   */
  define("AF_HIDE_SEARCH",                        512); // not searchable in extended search

  /**
   * Load always, even if not displayed anywhere
   *
   * hide flag
   */
  define("AF_FORCE_LOAD",                        1024); // load always, even if not displayed anywhere

  /**
   * Attribute is totally hidden
   *
   * hide flag
   */
  define("AF_HIDE",          AF_HIDE_EDIT|AF_HIDE_ADD|  // attribute is totally hidden
                          AF_HIDE_LIST|AF_HIDE_SEARCH|
                          AF_HIDE_VIEW|AF_HIDE_SELECT);

  /**
   * Readonly in add
   *
   * readonly flag
   */
  define("AF_READONLY_ADD",                      2048); // readonly in add

  /**
   * Readonly when edited
   *
   * readonly flag
   */
  define("AF_READONLY_EDIT",                     4096); // readonly when edited

  /**
   * Always readonly
   *
   * readonly flag
   */
  define("AF_READONLY",              AF_READONLY_EDIT|
                                      AF_READONLY_ADD); // always readonly

  /**
   * No label in forms
   *
   * display-related processing flag
   */
  define("AF_NO_LABEL",                          8192); // no label in forms

  /**
   * Alias for AF_NO_LABEL (mistyped)
   *
   * display-related processing flag
   */
  define("AF_NOLABEL",                    AF_NO_LABEL); // no label (mistyped)

  /**
   * Blank label in forms
   *
   * display-related processing flag
   */
  define("AF_BLANK_LABEL",                      16384); // blank label in forms

  /**
   * Alias for AF_BLANK_LABEL (mistyped)
   *
   * display-related processing flag
   */
  define("AF_BLANKLABEL",              AF_BLANK_LABEL); // blank label (mistyped)

  /**
   * Cannot be sorted in recordlists
   *
   * display-related processing flag
   */
  define("AF_NO_SORT",                          32768); // cannot be sorted in recordlists.

  /**
   * Alias for AF_NO_SORT (mistyped)
   *
   * display-related processing flag
   */
  define("AF_NOSORT",                      AF_NO_SORT); // no-sort flag is often mistyped

  /**
   * Attribute is searchable in list views
   *
   * display-related processing flag
   */
  define("AF_SEARCHABLE",                       65536); // Attribute is searchable in list views

  /**
   * The attribute will have a 'total' column in lists
   *
   * display-related processing flag
   */
  define("AF_TOTAL",                           131072); // The attribute will have a 'total' column in lists.

  /**
   * If supported, use pop-up window
   *
   * display-related processing flag
   */
  define("AF_POPUP",                           262144); // if supported, use pop-up window

  /**
   * Delete function is called when owning node is deleted
   *
   * miscellaneous processing flag
   */
  define("AF_CASCADE_DELETE",                  524288); // delete function is called when owning node is deleted

  /**
   * Will have a large amount of recors (relation)
   *
   * Instead of showing a listbox with all the records it will show an add link to a select page
   *
   * miscellaneous processing flag
   */
  define("AF_LARGE",                          1048576); // will have a large ammount of records (relation)

  /**
   * Ignore filters when selecting records (relation)
   *
   * miscellaneous processing flag
   */
  define("AF_NO_FILTER",                      2097152); // ignore filters when selecting records (relation)

  /**
   * Parent field for parent child relations (treeview)
   *
   * miscellaneous processing flag
   */
  define("AF_PARENT",                         4194304); // parent field for parent child relations (treeview)

  /**
   * No quotes are used when adding to the database
   *
   * miscellaneous processing flag
   */
  define("AF_NO_QUOTES",                      8388608); // no quotes are used when adding to database

  /**
   * Multi-language field
   *
   * miscellaneous processing flag
   */
  define("AF_ML",                            16777216); // multi-language field

  /**
   * Alias for AF_ML (spelled out)
   *
   * miscellaneous processing flag
   */
  define("AF_MULTILANGUAGE",                    AF_ML);

  /**
   * Shortcut for hidden auto-incremented primary key
   *
   * miscellaneous processing flag
   */
  define("AF_AUTOKEY",             AF_PRIMARY|AF_HIDE|  // shortcut for hidden auto-incremented primary-key
                                     AF_AUTOINCREMENT);

  /*
   * flag (values) that can be used for attribute specific flags
   * NOTE: Attribute specific flags aren't good behaviour, but for
   * compatibility reasons we support them anyway. Newly derived attributes
   * should not use these specific flags, but work with extra parameters.
   */

   /**
    * Specific attribute flag 1
    */
   define("AF_SPECIFIC_1",                    33554432); // specific attribute flag 1

   /**
    * Specific attribute flag 2
    */
   define("AF_SPECIFIC_2",                    67108864); // specific attribute flag 2

   /**
    * Specific attribute flag 3
    */
   define("AF_SPECIFIC_3",                   134217728); // specific attribute flag 3

   /**
    * Specific attribute flag 4
    */
   define("AF_SPECIFIC_4",                   268435456); // specific attribute flag 4

   /**
    * Specific attribute flag 5
    */
   define("AF_SPECIFIC_5",                   536870912); // specific attribute flag 5

  /**
   * Do not store this attribute
   *
   * Storage type flag, used by the storageType() and related methods
   */
  define("NOSTORE"   ,0);

  /**
   * Do not load this attribute
   *
   * Storage type flag, used by the storageType() and related methods
   */
  define("NOLOAD"   ,0);

  /**
   * Store before all other ('normal') attributes (?)
   *
   * Storage type flag, used by the storageType() and related methods
   */
  define("PRESTORE"  ,1);

  /**
   * Call load before selectDb()
   *
   * Storage type flag, used by the storageType() and related methods
   */
  define("PRELOAD",   1);

  /**
   * Store after all other ('normal') attributes (?)
   *
   * Storage type flag, used by the storageType() and related methods
   */
  define("POSTSTORE" ,2);

  /**
   * Call load after selectDb()
   *
   * Storage type flag, used by the storageType() and related methods
   */
  define("POSTLOAD", 2);

  /**
   * Do addToQuery() of this attribute
   *
   * Storage type flag, used by the storageType() and related methods
   */
  define("ADDTOQUERY",4);

  /**
   * Attribute is disable in view mode
   */
  define("DISABLED_VIEW",1);

  /**
   * Attribute is disable in edit mode
   */
  define("DISABLED_EDIT",2);

  /**
   * Attribute is disabled in view and edit mode
   */
  define("DISABLED_ALL", DISABLED_VIEW|DISABLED_EDIT);

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
     * @access private
     * @var String
     */
    var $m_name;

    /**
     * The attribute flags (see above)
     * @access private
     * @var int
     */
    var $m_flags = 0;

    /**
     * The name of the atkNode that owns this attribute (set by atknode)
     * @access private
     * @var String
     */
    var $m_owner = "";

    /**
     * The module of the attribute (if empty, the module from the owner node
     * should be assumed).
     * @access private
     * @var String
     *
     */
    var $m_module = "";

    /**
     * Instance of the atkNode that owns this attribute
     * @access private
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
     * @access private
     * @var int
     */
    var $m_order = 0;

    /**
     * Index of the attribute within its node.
     * @access private
     * @var int
     */
    var $m_index = 0;

    /**
     * The tab(s) on which the attribute lives.
     * @access private
     * @var mixed
     */
    var $m_tabs="*";

    /**
     * The section(s) on which the attribute lives.
     * @access private
     * @var mixed
     */
    var $m_sections="*";

    /**
     * The id of the attribute in the HTML
     * @access private
     * @var String
     */
    var $m_htmlid;

    /**
     * The css classes of the attribute
     * @access private
     * @var sttsy
     */
    var $m_cssclasses = array();

    /**
     * The label of the attribute.
     * @access private
     * @var String
     */
    var $m_label = "";

    /**
     * The searchmode for this attribute
     *
     * This var exists so that you can assign searchmodes to specific
     * attributes instead of having a general searchmode for the entire
     * search. This can be any one of the supported modes, as returned by
     * the attribute's getSearchModes() method.
     * @access private
     * @var String
     */
    var $m_searchmode = "";

    /**
     * Wether to force an insert of the attribute
     * @access private
     * @var bool
     */
    var $m_forceinsert=false;

    /**
     * Wether to force an update of the attribute
     * @access private
     * @var bool
     */
    var $m_forceupdate=false;

    /**
     * Array for containing onchange javascript code
     * @access private
     * @var Array
     */
    var $m_onchangecode = array();

    /**
     * Variable to store initialisation javascript code
     * in for the changehandler.
     * @access private
     * @var String
     */
    var $m_onchangehandler_init = "";

    /**
     * Variable to store depended attributes.
     * @access private
     * @var Array
     */
    var $m_dependees = array();

    /**
     * Attribute to store disabled modes.
     * @access private
     * @var int
     */
    var $m_disabledModes = 0;

    /**
     * Whether to hide initially or not
     * @access private
     * @var bool
     */
    var $m_initial_hidden = false;

    /**
     * Storage type.
     * @access private
     * @var int
     * @see setStorageType
     */
    var $m_storageType = array();


    /**
     * Load type.
     * @access private
     * @var int
     * @see setLoadType
     */
    var $m_loadType = array();

    /**
     * Initial value.
     * @access private
     * @var mixed
     * @see setInitialValue
     */
    var $m_initialValue = NULL;

    /**
     * Manually painted attribute. Will not be rendered. If this is set to true, you are responsible
     * for calling the attribute's edit() method to paint it on screen.
     */
    var $m_manual_paint = false;

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
      $this->setFlags((int)$flags);
      $this->setAttribSize($size);

      // default class
      $this->addCSSClass(get_class($this));
      atkimport("atk.keyboard.atkkeyboard");
    }


    /**
     * Returns the owner instance.
     *
     * @return atkNode owner instance
     */
    function &getOwnerInstance()
    {
      return $this->m_ownerInstance;
    }

    /**
     * Sets the owner instance.
     *
     * @param atkNode $instance
     */
    function setOwnerInstance(&$instance)
    {
      $this->m_ownerInstance = &$instance;
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
     * Returns the full set of flags of the attribute.
     * @return int $m_flags The full set of flags
     */
    function getFlags()
    {
      return $this->m_flags;
    }

    /**
     * Adds a flag to the attribute.
     * Note that adding flags at any time after the constructor might not
     * always work. There are flags that are processed only at
     * constructor time.
     * @param int $flag The flag to add to the attribute
     * @return atkAttribute The instance of this atkAttribute
     */
    function addFlag($flag)
    {
      $this->m_flags |= $flag;
      if (hasFlag($flag, AF_HIDE_LIST)&&!$this->hasFlag(AF_PRIMARY)&&is_object($this->m_ownerInstance)&&!in_array($this->fieldName(),$this->m_ownerInstance->m_listExcludes))
      {
        $this->m_ownerInstance->m_listExcludes[]=$this->fieldName();
      }
      if (hasFlag($flag, AF_HIDE_VIEW)&&!$this->hasFlag(AF_PRIMARY)&&is_object($this->m_ownerInstance)&&!in_array($this->fieldName(),$this->m_ownerInstance->m_viewExcludes))
      {
        $this->m_ownerInstance->m_viewExcludes[]=$this->fieldName();
      }
      return $this;
    }

    /**
     * Sets the flags of the attribute
     *
     * Note that if you assign nothing or 0, this will remove all the flags
     * from the attribute. You can assign multiple flags by using the pipe
     * symbol. Setting the flags will overwrite all previous flag-settings.
     * @param int $flags The flags to be set to the attribute.
     */
    function setFlags($flags = 0)
    {
      $this->m_flags = 0;
      $this->addFlag($flags); // always call addFlag
    }

    /**
     * Removes a flag from the attribute.
     *
     * Note that removing flags at any time after the constructor might not
     * always work. There are flags that have already been processed at
     * constructor time, so removing them will be futile.
     * @param int $flag The flag to remove from the attribute
     * @return atkAttribute The instance of this atkAttribute
     */
    function removeFlag($flag)
    {
      if($this->hasFlag($flag)) $this->m_flags ^= $flag;
      if ((hasFlag($flag, AF_HIDE_LIST)&&!$this->hasFlag(AF_PRIMARY))&&is_object($this->m_ownerInstance)&&in_array($this->fieldName(),$this->m_ownerInstance->m_listExcludes))
      {
        unset($this->m_ownerInstance->m_listExcludes[array_search($this->fieldName(), $this->m_ownerInstance->m_listExcludes)]);
      }
      if ((hasFlag($flag, AF_HIDE_VIEW)&&!$this->hasFlag(AF_PRIMARY))&&is_object($this->m_ownerInstance)&&in_array($this->fieldName(),$this->m_ownerInstance->m_viewExcludes))
      {
        unset($this->m_ownerInstance->m_viewExcludes[array_search($this->fieldName(), $this->m_ownerInstance->m_viewExcludes)]);
      }
      return $this;
    }

    /**
     * Adds a disabled mode flag to the attribute  (use DISABLED_VIEW and DISABLED_EDIT flags).
     * @param int $flag The flag to add to the attribute
     */
    function addDisabledMode($flag)
    {
      $this->m_disabledModes |= $flag;
    }
    /**
     * Check if the attribute is disabled in some mode (use DISABLED_VIEW and DISABLED_EDIT flags).
     * @param int $flag The flag you want to check
     * @return boolean
     */
    function hasDisabledMode($flag)
    {
      return (($this->m_disabledModes & $flag) == $flag);
    }
    /**
     * Sets the disabled mode flag of the attribute
     *
     * Note that if you assign nothing or 0, this will remove all the flags
     * from the attribute. You can assign multiple flags by using the pipe
     * symbol. Setting the flags will overwrite all previous flag-settings.
     * @param int $flags The flags to be set to the attribute.
     */
    function setDisabledModes($flags = 0)
    {
      $this->m_disabledModes = $flags;
    }
    /**
     * Removes a disabled mode from the attribute.
     *
     * @param int $flag The flag to remove from the attribute
     */
    function removeDisabledMode($flag)
    {
      if($this->hasDisabledMode($flag)) $this->m_disabledModes ^= $flag;
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
      return (!isset($record[$this->fieldName()])||$record[$this->fieldName()]===""||$record[$this->fieldName()]===NULL);
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
      if (is_array($rec)&&isset($rec[$this->fieldName()]))
      {
        return $this->escapeSQL($rec[$this->fieldName()]);
      }
      return NULL;
    }

    /**
     * Converts a database value to an internal value.
     *
     * For the regular atkAttribute
     * Derived attributes may reimplement this for their own conversion.
     * (In which case, the return type might be 'mixed')
     *
     * This is the exact opposite of the value2db method.
     *
     * @param array $rec The database record that holds this attribute's value
     * @return mixed The internal value
     */
    function db2value($rec)
    {
      if (isset($rec[$this->fieldName()]))
      {
        return $rec[$this->fieldName()] === NULL ? NULL : $rec[$this->fieldName()];
      }
      return NULL;
    }

    /**
     * Is there a value posted for this attribute?
     *
     * @param array $postvars
     * @return boolean posted?
     */
    function isPosted($postvars)
    {
      return is_array($postvars)&&isset($postvars[$this->fieldName()]);
    }

    /**
     * Set initial value for this attribute.
     *
     * NOTE: the initial value only works if there is no initial_values override
     *       in the node or if the override properly calls parent::initial_values!
     *
     * @param mixed $value initial value
     */
    function setInitialValue($value)
    {
      $this->m_initialValue = $value;
    }

    /**
     * Initial value. Returns the initial value for this attribute
     * which will be used in the add form etc.
     *
     * @return mixed initial value for this attribute
     */
    function initialValue()
    {
      return $this->m_initialValue;
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
      if ($this->isPosted($postvars))
      {
        return $postvars[$this->fieldName()];
      }
    }

    /**
     * Returns a piece of html code that can be used in a form to edit this
     * attribute's value.
     *
     * @param array $record The record that holds the value for this attribute.
     * @param String $fieldprefix The fieldprefix to put in front of the name
     *                            of any html form element for this attribute.
     * @param String $mode The mode we're in ('add' or 'edit')
     * @return String A piece of htmlcode for editing this attribute
     */
    function edit($record="", $fieldprefix="", $mode="")
    {
      $id = $this->getHtmlId($fieldprefix);
      $this->registerKeyListener($id, KB_CTRLCURSOR|KB_UPDOWN);

      if (count($this->m_onchangecode))
      {
        $onchange = 'onChange="'.$id.'_onChange(this);"';
        $this->_renderChangeHandler($fieldprefix);
      }
      else
      {
        $onchange = '';
      }

      $size = $this->m_size;
      if ($mode == 'list' && $size > 20)
        $size = 20;

      $value = (isset($record[$this->fieldName()]) && !is_array($record[$this->fieldName()])?htmlspecialchars($record[$this->fieldName()]):"");

      $result = '<input type="text" id="'.$id.'" name="'.$id.'" '.$this->getCSSClassAttribute().
                ' value="'.$value.'"'.
                ($size > 0 ? ' size="'.$size.'"' : '').
                ($this->m_maxsize > 0 ? ' maxlength="'.$this->m_maxsize.'"' : '').' '.$onchange.' />';

      return $result;
    }

    /**
     * Add a javascript onchange event handler.
     * @param $jscode A block of valid javascript code.
     * @return atkAttribute Returns the instance of this attribute.
     */
    function addOnChangeHandler($jscode)
    {
      if (!in_array($jscode, $this->m_onchangecode)) $this->m_onchangecode[] = $jscode;
      return $this;
    }

    /**
     * Renders the onchange code on the page.
     *
     * @access private
     * @param String $fieldprefix The prefix to the field
     */
    function _renderChangeHandler($fieldprefix)
    {
      if (count($this->m_onchangecode))
      {
        $page = &$this->m_ownerInstance->getPage();
        $page->register_scriptcode("
    function ".$this->getHtmlId($fieldprefix)."_onChange(el)
    {
      {$this->m_onchangehandler_init}
      ".implode("\n      ", $this->m_onchangecode)."
    }\n");
      }
    }

    /**
     * Returns a piece of html code for hiding this attribute in an HTML form,
     * while still posting its value. (<input type="hidden">)
     *
     * @param array $record The record that holds the value for this attribute
     * @param String $fieldprefix The fieldprefix to put in front of the name
     *                            of any html form element for this attribute.
     * @return String A piece of htmlcode with hidden form elements that post
     *                this attribute's value without showing it.
     */
    function hide($record="", $fieldprefix="")
    {
      // the next if-statement is a workaround for derived attributes which do
      // not override the hide() method properly. This will not give them a
      // working hide() functionality but at least it will not give error messages.
      if (!is_array($record[$this->fieldName()]))
      {
        $id = $id = $this->getHtmlId($fieldprefix);
        $result = '<input type="hidden" id="'.$id.'" name="'.$fieldprefix.$this->formName().'" value="'.htmlspecialchars($record[$this->fieldName()]).'">';
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
      $this->m_htmlid = $fieldprefix.$this->fieldName();
      return $this->m_htmlid;
    }

    /**
     * Returns the html identifier of the attribute without setting it
     * Created because getHtmlId would always SET the htmlid while getting it.
     * @return String The HTML id of this attribute
     */
    function getAttributeHtmlId()
    {
      if ($this->m_htmlid) return $this->m_htmlid;
      else return $this->fieldName();
    }

    /**
     * Adds the attribute's view / hide HTML code to the view array.
     *
     * This method is called by the node if it wants the data needed to create
     * a view form.
     *
     * This is a framework method, it should never be called directly.
     *
     * @param String $mode     the mode ("view")
     * @param array  $arr      pointer to the view array
     * @param array  $defaults pointer to the default values array
     */
    function addToViewArray($mode, &$arr, &$defaults)
    {
      if(!$this->hasFlag(AF_HIDE_VIEW))
      {
        $entry = array("name" => $this->m_name, "attribute" => &$this);

        /* label? */
        $entry["label"] = $this->getLabel($defaults, $mode);
        // on which tab?
        $entry["tabs"] = $this->getTabs($mode);
        //on which sections?
        $entry["sections"] = $this->getSections();
        /* the actual edit contents */
        $entry["html"] = $this->getView($mode, $defaults);
        $arr["fields"][] = $entry;
      }
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
     * @param String $fieldprefix   the fieldprefix
     */
    function addToEditArray($mode, &$arr, &$defaults, &$error, $fieldprefix)
    {
      /* Attribute manually paints itself? */
      if ($this->m_manual_paint) return;


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
        global $ATK_VARS;

        $this->_registerDependees($mode, $fieldprefix);

        $entry = array("name" => $this->m_name, "obligatory" => $this->hasFlag(AF_OBLIGATORY), "attribute" => &$this);
        $entry["id"] = $this->getHtmlId($fieldprefix);

        /* label? */
        $entry["label"] = $this->getLabel($defaults, $mode);
        /* error? */
        $entry["error"] = $this->getError($error) || (isset($ATK_VARS["atkerrorfields"]) && atk_in_array($entry['id'], $ATK_VARS['atkerrorfields']));
        // on which tab?
        $entry["tabs"] = $this->getTabs($mode);
        //on which sections?
        $entry["sections"] = $this->getSections();
        // the actual edit contents
        $entry["html"] = $this->getEdit($mode, $defaults, $fieldprefix);
        // initially hidden
        $entry["initial_hidden"] = $this->isInitialHidden($defaults);
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
      if (empty($tabs) && isset($this->m_ownerInstance) && is_object($this->m_ownerInstance))
      {
        $tabs = array($this->m_ownerInstance->m_default_tab);
      }
      else if (empty($tabs))
      {
        $tabs = array('default');
      }

      $this->m_tabs = $tabs;
    }

    /**
     * retrieve the tabs for this attribute.
     *
     * @return array
     */
    function getTabs()
    {
      return $this->m_tabs;
    }

    /**
     * Put the attribute on one or more tabs and/or sections.
     *
     * Example:
     * <code>$attribute->setSections(array('tab.section','tab.othersection));</code>
     *
     * @param array $sections An array of tabs and/or sections on which the attribute should
     *                    be displayed.
     */
    function setSections($sections)
    {
      if ($sections==NULL)
      {
        $this->m_sections = array();
      }
      else
      {
        $this->m_sections = $sections;
      }
    }

    /**
     * retrieve the tabs and/or sections for this attribute.
     *
     * @return array
     */
    function getSections()
    {
      return $this->m_sections;
    }


    /**
     * Retrieve the html code for placing this attribute in a view page.
     *
     * Method is 'smart' and can be overridden in the node using the
     * <attributename>_display() methods.
     *
     * Framework method, it should not be necessary to call this method
     * directly.
     *
     * @param String $mode The mode ("view")
     * @param array $defaults The record holding the values for this attribute
     *
     * @return String the HTML code for this attribute that can be used in a
     *                viewpage.
     */
    function getView($mode, &$defaults)
    {
      $funcname = $this->m_name."_display";
      if (method_exists($this->m_ownerInstance, $funcname))
      {
        $result = $this->m_ownerInstance->$funcname($defaults, $mode);
      }
      else
      {
        $result = $this->display($defaults, $mode);
      }
      return $result;
    }

    /**
     * Retrieve the html/javascript code for showing the tooltip for this attribute.
     *
     * @return String HTML
     */
    function getToolTip()
    {
      $module = $this->getModule();
      if (!$module) $module = "atk";

      $node = &$this->m_ownerInstance;
      $ttip = atktext($node->m_type."_".$this->fieldName()."_tooltip", $module, "", "", "", true);

      $tooltip = '';

      if ($ttip)
      {
        $theme = &atkinstance("atk.ui.atktheme");
        $tipimg = $theme->imgPath("help.gif");

        $onelinetip = preg_replace('/([\r\n])/e',"",$ttip);
        $tooltip = '<img align="top" src="'.$tipimg.'" border="0" alt="'.$onelinetip.'" onClick="javascript:alert(\''.str_replace("\n", '\n', addslashes($ttip)).
        '\')" onMouseOver="javascript:window.status=\''. addslashes($onelinetip).'\';">';
      }
      return $tooltip;
    }

    /**
     * Retrieve the HTML code for placing this attribute in an edit page.
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
        $result.= $this->getView($mode, $defaults);
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
      for ($i = 0; $i < count($errors); $i++)
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
     * @param atkColumnConfig $columnConfig Column configuration object
     * @param atkDataGrid $grid The atkDataGrid this attribute lives on.
     * @see atkNode::listArray
     */
    function addToListArrayHeader($action, &$arr, $fieldprefix, $flags, $atksearch, &$columnConfig, $grid=null)
    {
      global $g_sessionManager, $ATK_VARS;

      if (!$this->hasFlag(AF_HIDE_LIST) && !($this->hasFlag(AF_HIDE_SELECT) && $action == "select"))
      {
        $key = $fieldprefix.$this->fieldName();

        $arr["heading"][$key]["title"] = $this->label();

        if (!hasFlag($flags, RL_NO_SORT) && !$this->hasFlag(AF_NO_SORT))
        {
          $arr["heading"][$key]["order"] = $this->listHeaderSortOrder($columnConfig, $fieldprefix);
        }

        if (hasFlag($flags, RL_EXT_SORT))
        {
          $arr["sort"][$key] = $this->extendedSort($columnConfig, $fieldprefix, $grid);
        }

        if (!hasFlag($flags, RL_NO_SEARCH) && $this->hasFlag(AF_SEARCHABLE))
        {
          $fn = $this->fieldName()."_search";
          if (method_exists($this->m_ownerInstance, $fn))
          {
            $arr["search"][$key] = $this->m_ownerInstance->$fn($atksearch, false, $fieldprefix, $grid);
          }
          else
          {
            $arr["search"][$key] = $this->search($atksearch, false, $fieldprefix, $grid);
          }
          $arr["search"][$key] .= $this->searchMode(false, $fieldprefix);
        }
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
        if (in_array($this->fieldName(), $this->m_ownerInstance->m_editableListAttributes) && $this->m_ownerInstance->allowed('edit', $arr["rows"][$nr]["record"]))
        {
          $arr["rows"][$nr]["data"][$fieldprefix.$this->fieldName()] =
            $this->getEdit('list', $arr["rows"][$nr]["record"], 'atkrecordlistdata_AE_'.$arr['name'].'_AE_'.$nr.'_AE_');
        }
        else
        {
          $arr["rows"][$nr]["data"][$fieldprefix.$this->fieldName()] =
            $this->getView('list', $arr["rows"][$nr]["record"]);
        }

        /* totalisable? */
        if ($this->hasFlag(AF_TOTAL))
        {
          $sum = $this->sum($arr["totalraw"], $arr["rows"][$nr]["record"], $fieldprefix);
          $arr["totalraw"][$this->fieldName()] = $sum[$this->fieldName()];
          $arr["total"][$fieldprefix.$this->fieldName()] =
            $this->getView('list', $sum);
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
     * @todo  find a better way to search on onetomanys that does not require
     *        something evil in atkAttribute
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
      $id = $this->getSearchFieldName($fieldprefix);

      $value="";
      if (is_array($record)&&isset($record[$this->fieldName()]))
      {
        $value=$record[$this->fieldName()];
      }

      $this->registerKeyListener($id, KB_CTRLCURSOR|KB_UPDOWN);
      $result= '<input type="text" id="'.$id.'" class="'.get_class($this).'" name="'.$id.'" value="'.$value.'"'.
                ($this->m_searchsize > 0 ? ' size="'.$this->m_searchsize.'"' : '').
                ($this->m_maxsize > 0 ? ' maxlength="'.$this->m_maxsize.'"' : '').'>';

      return $result;
    }

    /**
     * Returns piece of html which is used for setting/selecting the search
     * mode for this attribute.
     *
     * It will show a pulldown if using extended search and multiple
     * searchmodes are supported otherwise the default searchmode is selected.
     *
     * @param  boolean $extended using extended search?
     * @param  string  $fieldprefix optional fieldprefix
     * @return string  html which is used for selecting searchmode
     */
    public function searchMode($extended=false, $fieldprefix='')
    {
      $searchModes = $this->getSearchModes();
      $dbSearchModes = $this->getDb()->getSearchModes();
      $searchModes = array_values(array_intersect($searchModes, $dbSearchModes));

      $searchMode = $this->getSearchMode($extended);
      // Set current searchmode to first searchmode if not searching in extended form or no searchmode is set
      if (!$extended || empty($searchMode) || !in_array($searchMode, $searchModes))
        $searchMode = $searchModes[0];

      if ($extended && count($searchModes) > 1)
      {
        $field = '<select name="'.$this->getSearchModeFieldname($fieldprefix).'">';

        foreach ($searchModes as $value)
        {
          $selected = $searchMode == $value ? ' selected="selected"' : '';
          $field .= '<option value="'.$value.'"'.$selected.'>'.htmlentities($this->text("search_".$value)).'</option>';
        }

        $field .= '</select>';
      }
      else
      {
        $field =
          '<input type="hidden" name="'.$this->getSearchModeFieldname($fieldprefix).'" value="'.$searchMode.'">'.
          ($extended ? atktext("search_".$searchMode) : '');
      }

      return $field;
    }

    /**
     * Retrieve the current set or default searchmode of this attribute
     *
     * @param boolean $extended Whether extended search is being used
     * @return String the default searchmode for this attribute.
     */
    function getSearchMode($extended=false)
    {
      $searchmode = $this->m_ownerInstance->getSearchMode();

      if (is_array($searchmode))
      {
        return $searchmode[$this->fieldName()];
      }
      return $searchmode;
    }

    /**
     * Creates a smart search condition for a given search value, and adds it
     * to the query that will be used for performing the actual search.
     *
     * @param Integer  $id         The unique smart search criterium identifier.
     * @param Integer  $nr         The element number in the path.
     * @param Array    $path       The remaining attribute path.
     * @param atkQuery $query      The query to which the condition will be added.
     * @param String   $ownerAlias The owner table alias to use.
     * @param Mixed    $value      The value the user has entered in the searchbox.
     * @param String   $mode       The searchmode to use.
     */
    function smartSearchCondition($id, $nr, $path, &$query, $ownerAlias, $value, $mode)
    {
      // default implementation doesn't supported nested paths, this method
      // should be overriden by relations etc. if they want to support this
      if (count($path) > 0)
      {
        atk_var_dump($path, 'Invalid search path for '.$this->m_ownerInstance->atkNodeType().'#'.$this->fieldName().', ignoring criterium!');
        return;
      }

      $this->searchCondition($query, $ownerAlias, $value, $mode);
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
     * @param string $fieldaliasprefix optional prefix for the fiedalias in the table
     */
    function searchCondition(&$query, $table, $value, $searchmode, $fieldaliasprefix='')
    {
      $searchCondition = $this->getSearchCondition($query, $table, $value, $searchmode, $fieldaliasprefix);
      if ($searchCondition)
      {
        $query->addSearchCondition($searchCondition);
      }
    }

    /**
     * Creates a searchcondition for the field,
     * was once part of searchCondition, however,
     * searchcondition() also immediately adds the search condition.
     *
     * @param atkQuery $query     The query object where the search condition should be placed on
     * @param String $table       The name of the table in which this attribute
     *                              is stored
     * @param mixed $value        The value the user has entered in the searchbox
     * @param String $searchmode  The searchmode to use. This can be any one
     *                              of the supported modes, as returned by this
     *                              attribute's getSearchModes() method.
     * @return String The searchcondition to use.
     */
    function getSearchCondition(&$query, $table, $value, $searchmode)
    {
      // If we are accidentally mistaken for a relation and passed an array
      // we only take our own attribute value from the array
      if (is_array($value)) $value = $value[$this->fieldName()];

      if ($this->m_searchmode) $searchmode = $this->m_searchmode;

      // @todo Is this really needed?
      if (strpos($value, "*")!==false && strlen($value)>1)
      {
        // auto wildcard detection
        $searchmode = "wildcard";
      }

      $func = $searchmode."Condition";
      if (method_exists($query,$func) && ($value || ($value == 0)))
      {
        return $query->$func($table.".".$this->fieldName(),$this->escapeSQL($value),$this->dbFieldType());
      }
      elseif(!method_exists($query,$func))
      {
        atkdebug("Database doesn't support searchmode '$searchmode' for ".$this->fieldName().", ignoring condition.");
      }
      return false;
    }

    /**
     * Sets the searchmode for an attribute
     * This will cause attributes that respect this
     * to use the attributes searchmode for that particulair attribute
     * instead of the general searchmode.
     * @param String $searchmode The searchmode we want to set on the attribute
     */
    function setAttributeSearchmode($searchmode)
    {
      $this->m_searchmode = $searchmode;
    }

    /**
     * Returns a displayable string for this value, to be used in HTML pages.
     *
     * The regular atkAttribute uses PHP's nl2br() and htmlspecialchars()
     * methods to prepare a value for display, unless $mode is "cvs".
     *
     * @param array $record The record that holds the value for this attribute
     * @param String $mode The display mode ("view" for viewpages, or "list"
     *                     for displaying in recordlists, "edit" for
     *                     displaying in editscreens, "add" for displaying in
     *                     add screens. "csv" for csv files. Applications can
     *                     use additional modes.
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
        if (($mode=="csv")||($mode=="plain")) return $record[$this->fieldName()];
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
     * Checks if this attribute is really not null in the database.
     * This method does not look at the AF_OBLIGATORY flag, it only
     * checks in the database if the attribute's column is really not null.
     *
     * @return boolean attribute's database column not null?
     */
    function isNotNullInDb()
    {
      $db = &$this->getDb();
      $meta = $db->tableMeta($this->m_ownerInstance->m_table);
      return hasFlag($meta[$this->fieldName()]['flags'], MF_NOT_NULL);
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
     * @param Array The record that contains the value of this attribute.
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
      {
        if ($this->isEmpty($rec) && !$this->hasFlag(AF_OBLIGATORY) && !$this->isNotNullInDb())
        {
          $query->addField($this->fieldName(),'NULL',"","",false,true);
        }
        else
        {
          $query->addField($this->fieldName(),$this->value2db($rec),"","",!$this->hasFlag(AF_NO_QUOTES),true);
        }
      }
      else
        $query->addField($this->fieldName(),"",$tablename,$fieldaliasprefix,!$this->hasFlag(AF_NO_QUOTES),true);
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
      $value1 = (isset($rec1[$this->fieldName()])?$rec1[$this->fieldName()]:0);
      $value2 = (isset($rec2[$this->fieldName()])?$rec2[$this->fieldName()]:0);
      return array($this->fieldName() => ($value1+$value2));
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
      $attribname = strtolower($this->fieldName());
      // Only use meta info if size is not already set manual. (constructor or setAttribSize)
      // If both are set, use the smallest value.
      if (isset($metadata[$attribname]))
      {
        if ($this->m_size > 0) $this->m_size = min($this->m_size, $metadata[$attribname]['len'], $this->maxInputSize());
        else $this->m_size = min($metadata[$attribname]['len'], $this->maxInputSize());

        if ($this->m_searchsize > 0) $this->m_searchsize = min($this->m_searchsize, $metadata[$attribname]['len'], 20);
        else $this->m_searchsize = min($metadata[$attribname]['len'], 20);

        if ($this->m_maxsize > 0) $this->m_maxsize = min($this->m_maxsize, $metadata[$attribname]['len']);
        else $this->m_maxsize = $metadata[$attribname]['len'];
      }

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
     * This function is called at the end of the node's init method.
     *
     * The regular atkAttribute has no implementation for this method, but
     * derived attributes may override this method to perform custom
     * initialization.
     */
    function postInit()
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
     * Used to force an attribute to update with every addDb() call
     * @param bool $force Wether or not to force the attribute to insert
     * @return atkAttribute Returns the instance of this attribute
     */
    function setForceInsert($force)
    {
      $this->m_forceinsert = $force;
      return $this;
    }

    /**
     * Getter for wether or not an attribute is forced to insert
     * @return bool Wether or not an attribute is forced to insert
     */
    function getForceInsert()
    {
      return $this->m_forceinsert;
    }

    /**
     * Used to force an attribute to update with every updateDb() call
     * @param bool $force Wether or not to force the attribute to update
     * @return atkAttribute Returns the instance of this attribute
     */
    function setForceUpdate($force)
    {
      $this->m_forceupdate = $force;
      return $this;
    }

    /**
     * Getter for wether or not an attribute is forced to update
     * @return bool Wether or not an attribute is forced to update
     */
    function getForceUpdate()
    {
      return $this->m_forceupdate;
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
     *                 query; false if not.
     */
    function needsInsert($record)
    {
      return (!$this->hasFlag(AF_HIDE_ADD) ||
              $this->hasFlag(AF_AUTO_INCREMENT) ||
              !$this->isEmpty($record) ||
              $this->m_forceinsert);

      // If we are set to hide_add, we will only insert into the
      // db if a value has been set (for example by an initial_values
      // method). Also, autoincrement fields might be hidden, and their
      // value is still empty, but they do need to be inserted lateron.
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
      return ((!$this->hasFlag(AF_READONLY_EDIT) && !$this->hasFlag(AF_HIDE_EDIT)) || $this->m_forceupdate);
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
      return array("substring","exact","wildcard","regexp");
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
     * converted to a database specific type using $db->fieldType().
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
      return ($this->m_label!=""?$this->m_label:$this->text($this->fieldName()));
    }

    function setLabel($label)
    {
      $this->m_label = $label;
    }

    /**
     * Get the module that this attribute originated from.
     *
     * By default, this is the module of the owning node of this attribute.
     * However, if the attribute was added using a modifier from a different
     * module, then the module that added the attribute is returned.
     *
     * @return String The name of the module of this attribute
     */
    function getModule()
    {
      if ($this->m_module!="") return $this->m_module;
      return $this->m_ownerInstance->m_module;
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
    function getLabel($record = array(), $mode='')
    {
      if ($this->hasFlag(AF_NOLABEL)) return "AF_NO_LABEL";
      else if ($this->hasFlag(AF_BLANKLABEL)) return NULL;
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

    /**
     * Sets the storage type.
     *
     * @param int $type Bitmask containg information about storage requirements.
     * @param string $mode The storage mode ("add", "update" or null for all)
     *
     * @see storageType
     */
    function setStorageType($type, $mode=null)
    {
      $this->m_storageType[$mode] = $type;
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
    function storageType($mode=null)
    {
      // Mode specific storage type.
      if (isset($this->m_storageType[$mode]) && $this->m_storageType[$mode] !== null)
        return $this->m_storageType[$mode];
      // Global storage type (key null is special!)
      else if (isset($this->m_storageType[null]) && $this->m_storageType[null] !== null)
        return $this->m_storageType[null];
      // Default backwardscompatible behaviour:
      else if (method_exists($this,"store"))
        return POSTSTORE;
      else
        return ADDTOQUERY;
    }


    /**
     * Sets the load type.
     *
     * @param int $type Bitmask containg information about load requirements.
     * @param string $mode The load mode ("view", "admin" etc. or null for all)
     *
     * @see loadType
     */
    function setLoadType($type, $mode=null)
    {
      $this->m_loadType[$mode] = $type;
    }

    /**
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
     * @param boolean $searching ???
     * @todo add documentation about the searching argument. What does it do?
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
     *             NOLOAD     - nor load(), nor addtoquery() should be
     *                          called (attribute can not be loaded from the
     *                          database)
     *
     */
    function loadType($mode, $searching=false)
    {
      if (isset($this->m_loadType[$mode]) && $this->m_loadType[$mode] !== null)
        return $this->m_loadType[$mode];
      else if (isset($this->m_loadType[null]) && $this->m_loadType[null] !== null)
        return $this->m_loadType[null];
      // Default backwardscompatible behaviour:
      else if (method_exists($this,"load"))
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
      if (atkconfig("use_keyboard_handler"))
      {
        $kb = &atkKeyboard::getInstance();
        $kb->addFormElementHandler($id, $navkeys);
      }
    }

    /**
     * Check if the attribute wants to be shown on a certain tab.
     *
     * @param String $tab The name of the tab to check.
     * @return boolean
     */
    function showOnTab($tab)
    {
      return ($this->getTabs() == "*" || atk_in_array($tab, $this->getTabs()));
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

    /**
     * Convert a String representation into an internal value.
     *
     * Used by CSV imports and the like, to convert string values to internal
     * values. This is somewhat similar to db2value, but this method should,
     * when derived in other attributes, act 'smart' and treat the
     * value as a user string.
     * The default implementation returns the string unmodified, but derived
     * classes may override this method to alter that behaviour.
     *
     * @param String $stringvalue The value to parse.
     * @return mixed Internal value
     */
    function parseStringValue($stringvalue)
    {
      return $stringvalue;
    }

    /**
    * Adds the needed searchbox(es) for this attribute to the fields array. This
    * method should only be called by the atkSearchHandler.
    *
    * @param array   $fields            The array containing fields to use in the
    *                                   extended search
    * @param atkNode $node              The node where the field is in
    * @param array   $record            A record containing default values to put
    *                                   into the search fields.
    * @param array   $fieldprefix       search / mode field prefix
    * @param array   $currentSearchMode current search mode
    */
    function addToSearchformFields(&$fields, &$node, &$record, $fieldprefix = "", $currentSearchMode=array())
    {
      $field = array();
      $defaults = $record;

      // set "widget" value:
      $funcname = $this->m_name."_search";

      if (method_exists($node,$funcname))
      {
        $field["widget"] = $node->$funcname($defaults, true, $fieldprefix);
      }
      else
      {
        $field["widget"] = $this->search($defaults, true, $fieldprefix); // second param indicates extended search.
      }

      // pre-emptive set "full" value:
      $field["full"] = $field["widget"]; // lateron, we might add more to full

      // set "searchmode" value:
      $field['searchmode'] = $this->searchmode(true, $fieldprefix);

      // set "label" value:
      $field['label'] = $this->label($record);

      // add $field to fields array
      $fields[] = $field;
    }

    /**
     * Retrieve the fieldname of the attribute in an atksearch form.
     *
     * @param String $prefix The prefix
     * @return String Name of the attribute in an atksearch
     */
    function getSearchFieldName($prefix)
    {
       return 'atksearch_AE_'.$prefix.$this->formName();
    }

    /**
     * Retrieve the searchmode name of the attribute in an atksearch form.
     *
     * @param String $prefix The prefix
     * @return String Name of the attribute in an atksearch
     */
    function getSearchModeFieldname($prefix)
    {
       return 'atksearchmode_AE_'.$prefix.$this->formName();
    }

    function extendedSort(&$columnConfig, $fieldprefix='', $grid=null)
    {
      $result = $this->sortOptions($columnConfig, $fieldprefix, $grid).' '.$this->sortOrder($columnConfig, $fieldprefix, $grid);
      return $result;
    }

    /**
     * Retrieves the sort options for this attribute which is used in recordlists
     * and search actions.
     *
     * @param atkColumnConfig $columnConfig The config that contains options for
     *                                      extended sorting and grouping to a
     *                                      recordlist.
     * @param String $fieldprefix The prefix of the attribute
     * @param atkDataGrid $grid The grid that this attribute lives on.
     * @return String HTML
     */
    function sortOptions(&$columnConfig, $fieldprefix='', $grid=null)
    {
      if (!$this->hasFlag(AF_TOTAL) && $columnConfig->totalizable())
      {
        // if we are not the sumcolumn itself, but there are totalcolumns present, we can perform subtotalling
        $cmd = ($columnConfig->hasSubTotal($this->fieldName())?"unsubtotal":"subtotal");
        if ($grid == null)
        {
          return href(getDispatchFile().'?'.$columnConfig->getUrlCommand($this->fieldName(), $cmd), atktext("column_".$cmd)).' ';
        }
        else
        {
          $call = $grid->getUpdateCall($columnConfig->getUrlCommandParams($this->fieldName(), $cmd));
          $res = '<a href="javascript:void(0)" onclick="'.htmlentities($call).'">'.$this->text("column_".$direction).'</a>';
        }
      }
      return "";
    }

    /**
     * Sets the sortorder options for this attribute which is used in recordlists
     * and search actions.
     *
     * @param atkColumnConfig $columnConfig The config that contains options for
     *                                      extended sorting and grouping to a
     *                                      recordlist.
     * @param String $fieldprefix The prefix of the attribute on HTML forms
     * @param atkDataGrid $grid The grid that this attribute lives on.
     * @return String HTML
     */
    function sortOrder(&$columnConfig, $fieldprefix='', $grid=null)
    {
      $fieldname = $this->fieldName();
      $currentOrder = $columnConfig->getOrder($fieldname);

      $res="";
      if ($currentOrder>0)
      {
        $direction = ($columnConfig->getSortDirection($this->fieldName())=="desc" ? "asc":"desc");
        if ($grid == null)
        {
          $res= href(getDispatchFile().'?'.$columnConfig->getUrlCommand($fieldname, $direction), atktext("column_".$direction)).' ';
        }
        else
        {
          $call = $grid->getUpdateCall($columnConfig->getUrlCommandParams($fieldname, $direction));
          $res = '<a href="javascript:void(0)" onclick="'.htmlentities($call).'">'.$this->text("column_".$direction).'</a>';
        }
      }

      $res.= '<select name="atkcolcmd[][setorder]['.$fieldprefix.$fieldname.']">';
      $res.= '<option value="">';
      for ($i=1; $i<6; $i++)
      {
        $selected = ($currentOrder==$i?"selected":"");
        $res.='<option value="'.$i.'" '.$selected.'>'.$i;
      }
      $res.= '</select>';

      return $res;
    }

    /**
     * Retrieve the sortorder for the listheader based on the
     * atkColumnConfig
     *
     * @param atkColumnConfig $columnConfig The config that contains options for
     *                                      extended sorting and grouping to a
     *                                      recordlist.
     * @return String Returns sort order ASC or DESC
     */
    function listHeaderSortOrder(&$columnConfig)
    {
      $order = $this->fieldName();

      if (isset($columnConfig->m_colcfg[$order]))
      {
        $direction = $columnConfig->getDirection($order);
        if ($direction=="asc") $order.=" desc";
      }

      return $order;
    }

    /**
     * Retrieves the ORDER BY statement for this attribute's node.     *
     * Derived attributes may override this functionality to implement other
     * ordering statements using the given parameters.
     *
     * @param Array $extra A list of attribute names to add to the order by
     *                     statement
     * @param String $tableprefix The tableprefix
     * @return String The ORDER BY statement for this attribute
     */
    function getOrderByStatement($extra='', $tableprefix='')
    {
      return $this->m_ownerInstance->m_table.".".$this->fieldName();
    }

    /**
     * Translate using the owner instance's module and type.
     *
     * @param String $string The string to be translated
     * @return String The translated string.
     */
    function text($string)
    {
      if (is_object($this->m_ownerInstance))
        return $this->m_ownerInstance->text($string, $this->getModule());
      else
        return atktext($string, $this->getModule());
    }

    /**
     * Get database instance for this attribute. Will return the owner
     * instance database instance unless the owner instance is not set
     * in which case the default instance will be returned.
     *
     * @return atkDb database instance
     */
    function getDb()
    {
      if (is_object($this->getOwnerInstance()))
      {
        return $this->getOwnerInstance()->getDb();
      }
      else
      {
        return atkGetDb();
      }
    }

    /**
     * Escape string for use in a query.
     *
     * @param string value value to escape
     * @param boolean escape wildcards too?
     * @return String The escaped value.
     */
    function escapeSQL($value, $wildcards=false)
    {
      $db = &$this->getDb();
      return $db->escapeSQL($value, $wildcards);
    }

    /**
     * Handle a partial request for this attribute, different attributes
     * support different partials.
     *
     * @param String $partial The name of the partial (i.e. refresh)
     * @param String $mode    The current add/edit mode
     * @return String HTML Returns the result of the call to the partial
     *                     handling method
     */
    function partial($partial, $mode)
    {
      $method = "partial_{$partial}";

      if (!method_exists($this, $method))
      {
        return '<span style="color: red; font-weight: bold">Invalid partial!</span>';
      }

      return $this->$method($mode);
    }

    /**
     * Partial method to refresh  the add/edit field for this attribute.
     *
     * @param String $mode add/edit mode
     * @return String HTML the output needed to refresh the attribute.
     */
    function partial_refresh($mode)
    {
      $record = $this->m_ownerInstance->updateRecord();
      $fieldprefix = $this->m_ownerInstance->m_postvars['atkfp'];
      $this->_registerDependees($mode, $fieldprefix);

      $arr = array('fields' => array());
      $defaults = &$record;
      $error = array();

      $this->addToEditArray($mode, $arr, $defaults, $error, $fieldprefix);

      $script = '';
      atkimport('atk.utils.atkjson');
      foreach ($arr['fields'] as $field)
      {
        //atkJSON::encode expect string in in ASCII or UTF-8 format, so convert data to UTF-8
        $value = atk_iconv(atktext("charset"),"UTF-8",$field['html']);
        $script .= "\$('".str_replace('.', '_', $this->m_ownerInstance->atkNodeType().'_'.$field['id'])."').update(".atkJSON::encode($value).");\r\n";
      }

      $this->_registerDependees($mode, $fieldprefix);

      return '<script type="text/javascript">'.$script.'</script>';
    }

    /**
     * Add the dependee for this attribute.
     * @see _registerDependees()
     * @param String $attribName The attribute you wish to update.
     * @return atkAttribute Returns the current instance of the attribute
     */
    function addDependee($attribName)
    {
      if(!in_array($attribName, $this->m_dependees)) $this->m_dependees[] = $attribName;
      return $this;
    }

    /**
     * Retrieve the dependees for this attribute.
     * @see _registerDependees()
     * @return Array $m_dependees Returns the list of dependees.
     */
    function getDependees()
    {
      return $this->m_dependees;
    }

    /**
     * Returns a JavaScript call for refreshing this attribute.
     *
     * @todo add documentation for $focusFirstFormEl.
     *
     * @param String $mode add/edit mode
     * @param String $fieldprefix The prefix for this attribute in an HTML form
     * @param bool $focusFirstFormEl ???
     * @param bool $registerScript Does the javascript for this attribute need to be registered.
     * @return String javascript Returns the javascript call to refresh this attribute.
     */
    protected function getRefreshCall($mode='edit', $fieldprefix='', $focusFirstFormEl=false, $registerScript=true)
    {
      if ($registerScript)
      {
        $this->getOwnerInstance()->getPage()->register_script(atkconfig('atkroot').'atk/javascript/class.atkattribute.js');
      }

      $url = addslashes(partial_url($this->getOwnerInstance()->atkNodeType(), $mode == "add" ? "add" : "edit", "attribute.".$this->fieldName().".refresh", array("atkfp" => $fieldprefix)));
      $code = "ATK.Attribute.refresh('$url', '".($focusFirstFormEl ? 'true' : 'false')."');";
      return $code;
    }

    /**
     * Register change handlers for dependees.
     *
     * A dependee is a different attribute (mostly part of the same node) that
     * needs to be refreshed through an AJAX call if the value of this attribute
     * changes.
     * @access private
     * @param String $mode add/edit mode
     * @param String $fieldprefix The prefix for this attribute in an HTML form
     */
    function _registerDependees($mode="", $fieldprefix="")
    {
      if(!empty($this->m_dependees))
      {
        foreach ($this->m_dependees as $attribName)
        {
          $code = $this->getOwnerInstance()->getAttribute($attribName)->getRefreshCall($mode, $fieldprefix);
          $this->addOnChangeHandler($code);

          $isPartial = $this->m_ownerInstance->m_partial != NULL;
          if (!$isPartial)
          {
            $code = $this->getOwnerInstance()->getAttribute($attribName)->getRefreshCall($mode, $fieldprefix, true);
          }

          // only register the initial refresh on attribute load if this attribute
          // itself doesn't depend on any other attribute or this is a partial request
          if ($isPartial || !$this->isDepending())
          {
            $page = &$this->m_ownerInstance->getPage();
            $page->register_loadscript($code);
          }
        }
      }
    }

    /**
     * Check wether this attribute depends on other attributes.
     *
     * @return this attribute depends on other attribute(s)?
     */
    function isDepending()
    {
      foreach (array_keys($this->m_ownerInstance->m_attribList) as $attrName)
      {
        $attr = &$this->m_ownerInstance->getAttribute($attrName);
        if (in_array($this->fieldName(), $attr->getDependees()))
          return true;
      }

      return false;
    }

    /**
     * Retrieve the CSS classes that were registered for this attribute
     *
     * @return Array A list of css classes
     */
    function getCSSClasses()
    {
      return $this->m_cssclasses;
    }

    /**
     * Add a CSS class for this attribute on an HTML form.
     *
     * @param String $classname The name of a class.
     */
    function addCSSClass($classname)
    {
      if (!in_array($classname, $this->m_cssclasses)) $this->m_cssclasses[] = $classname;
    }

    /**
     * Retrieve the attribute for the HTML-tag for this atkAttribute.
     *
     * @param mixed $additionalclasses A string or an array with classnames.
     * @return String HTML The attributes classname(s)
     */
    function getCSSClassAttribute($additionalclasses = array())
    {
      $classes = array_merge($this->getCSSClasses(), is_array($additionalclasses) ? $additionalclasses : array($additionalclasses));
      return 'class="'.implode(" ", $classes).'"';
    }

    /**
     * Set whether initially hidden or not.  A field is "hidden" by adding the class atkAttrRowHidden.
     *
     * @param boolean $bool Initially hidden?
     */
    function setInitialHidden($bool)
    {
      $this->m_initial_hidden = $bool;
    }

    /**
     * check whether initially hidden or not
     *
     * @param array the record
     *
     * @return initially hidden
     */
    function isInitialHidden($record)
    {
      return $this->m_initial_hidden;
    }

    /**
     * Set wether the edit() method of this attribute should be called
     * by the framework, or if the edit method is called manually.
     *
     * @param bool $bool TRUE if it is called manually, FALSE if ATK
     *                   should call it.
     */
    function setManualPaint($bool)
    {
      $this->m_manual_paint = $bool;
    }

    /**
     * Check wether this attribute should be manually painted (call the edit
     * method) or not.
     *
     * @return bool Manually painted?
     */
    function isManualPaint() { return $this->m_manual_paint; }

    /**
     * String representation for this attribute (PHP5 only).
     *
     * @return string attribute name prefixed with node type
     */
    function __toString()
    {
      return $this->m_ownerInstance->atkNodeType()."::".$this->fieldName();
    }
  }
?>

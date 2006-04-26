<?php

  /**
   * This file is part of the Achievo ATK distribution.
   * Detailed copyright and licensing information can be found
   * in the doc/COPYRIGHT and doc/LICENSE files which should be
   * included in the distribution.
   *
   * @package atk
   *
   * @copyright (c)2000-2004 Ivo Jansch
   * @copyright (c)2000-2004 Ibuildings.nl BV
   * @license http://www.achievo.org/atk/licensing ATK Open Source License
   *
   * @version $Revision$
   * $Id$
   */

  /**
   * some includes
   */
  atkimport("atk.attributes.atkattribute");

  /**
   * Define some flags for nodes. Use the constructor of the atkNode
   * class to set the flags. (concatenate multiple flags with '|')
   */

  /**
   * No new records may be added
   */
  define("NF_NO_ADD",                  1);

  /**
   * Records may not be edited
   */
  define("NF_NO_EDIT",                 2);

  /**
   * Records may not be deleted
   */
  define("NF_NO_DELETE",               4);

  /**
   * Immediately after you add a new record,
   * you get the editpage for that record
   */
  define("NF_EDITAFTERADD",            8);

  /**
   * Reocrds may not be searched
   */
  define("NF_NO_SEARCH",              16);

  /**
   * Ignore addFilter filters
   */
  define("NF_NO_FILTER",              32);

  /**
   * Doesn't show an add form on the admin page
   * but a link to the form
   */
  define("NF_ADD_LINK",               64);

  /**
   * Records may not be viewed
   */
  define("NF_NO_VIEW",               128);

  /**
   * Records / trees may be copied
   */
  define("NF_COPY",                  256);

  /**
   * If this flag is set and only one record is
   * present on a selectpage, atk automagically
   * selects it and moves on to the target
   */
  define("NF_AUTOSELECT",            512);

  /**
   * If set, atk stores the old values of
   * a record as ["atkorgrec"] in the $rec that
   * gets passed to the postUpdate
   */
  define("NF_TRACK_CHANGES",        1024);

  /**
   * Quick way to disable accessright checking
   * for an entire node. (Everybody may access this node)
   */
  define("NF_NO_SECURITY",          2048);

  /**
   * Extended search feature is turned off
   */
  define("NF_NO_EXTENDED_SEARCH",   4096);

  /**
   * Multi-selection of records is turned on
   */
  define("NF_MULTI_RECORD_ACTIONS", 8192);

  /**
   * Multi-priority-selection of records is turned on
   */
  define("NF_MRPA",                16384);

  /**
   * Add locking support to node, if one user is editing a record,
   * no one else may edit it.
   */
  define("NF_LOCK",                32768);

  /**
   * Multi-language support
   */
  define("NF_ML",                  65536);

  /**
   * Quick way to ensable the csv import feature
   */
  define("NF_IMPORT",             131072);

  /**
   * Add CSV export ability to the node.
   */
  define("NF_EXPORT",             262144);


  /**
   * Disable csv import feature
   * @deprecated since ATK 5.2
   */
  define("NF_NO_IMPORT",               0);

  /**
   * Enable extended sorting (multicolumn sort)
   */
  define("NF_EXT_SORT",           524288);

  /**
   * Makes a node cache it's recordlist
   */
  define("NF_CACHE_RECORDLIST",  1048576);


  /**
   * After adding a new record add another one instantaniously.
   */
  define("NF_ADDAFTERADD",       2097152);


  /**
   * Specific node flag 1
   */
  define("NF_SPECIFIC_1",       33554432);

  /**
   * Specific node flag 2
   */
  define("NF_SPECIFIC_2",       67108864);

  /**
   * Specific node flag 3
   */
  define("NF_SPECIFIC_3",      134217728);

  /**
   * Specific node flag 4
   */
  define("NF_SPECIFIC_4",      268435456);

  /**
   * Specific node flag 5
   */
  define("NF_SPECIFIC_5",      536870912);

  /**
   * Alias for NF_MULTI_RECORD_ACTIONS flag (shortcut)
   */
  define("NF_MRA",           NF_MULTI_RECORD_ACTIONS);

  /**
   * Alias for NF_ML flag (typed out)
   */
  define("NF_MULTILANGUAGE", NF_ML);

  /**
   * Aggregate flag to quickly create readonly nodes
   */
  define("NF_READONLY",      NF_NO_ADD|NF_NO_DELETE|NF_NO_EDIT);

  /**
   * action status flags
   * Note that these have binary numbers, even though an action could never have
   * two statusses at the same time.
   * This is done however, so the flags can be used as a mask in the setFeedback
   * function.
   */

  /**
   * The action is cancelled
   *
   * action status flag
   */
  define("ACTION_CANCELLED", 1);

  /**
   * The action failed to accomplish it's goal
   *
   * action status flag
   */
  define("ACTION_FAILED", 2);

  /**
   * The action is a success
   *
   * action status flag
   */
  define("ACTION_SUCCESS", 4);

  /**
   * Trigger flags
   */
  define("TRIGGER_NONE", 0);
  define("TRIGGER_AUTO", 1);
  define("TRIGGER_PRE",  2);
  define("TRIGGER_POST", 4);
  define("TRIGGER_ALL", TRIGGER_PRE|TRIGGER_POST);

  /**
   * The atkNode class represents a piece of information that is part of an
   * application. This class provides standard functionality for adding,
   * editing and deleting nodes.
   * This class must be seen as an abstract base class: For every piece of
   * information in an application, a class must be derived from this class
   * with specific implementations for that type of node.
   *
   * @author Ivo Jansch <ivo@achievo.org>
   * @package atk
   */
  class atkNode
  {

    /**
     * reference to the class which is used to validate atknodes
     * the validator is overridable by changing this variabele
     *
     * @access private
     * @var String
     */
    var $m_validate_class = "atk.atknodevalidator";

    /**
     * Unique field sets of a certain node.
     *
     * Indicates which field combinations should be unique.
     * It doesn't contain the unique fields which have been set by flag
     * AF_UNIQUE.
     *
     * @access private
     * @var array
     */
    var $m_uniqueFieldSets = array();


    /**
     * Nodes must be initialised using the init() function before they can be
     * used. This member indicated whether the node has been initialised.
     * @access private
     * @var boolean
     */
    var $m_initialised = false;

    /**
     * Check to prevent double execution of setAttribSizes on pages with more
     * than one form.
     * @access private
     * @var boolean
     */
    var $m_attribsizesset = false;

    /**
     * The list of attributes of a node. These should be of the class
     * atkAttribute or one of its derivatives.
     * @access private
     * @var array
     */
    var $m_attribList = array();

    /**
     * Index list containing the attributes in the order in which they will
     * appear on screen.
     * @access private
     * @var array
     */
    var $m_attribIndexList = array();

    /**
     * Reference to the page on which the node is rendering its output.
     * @access private
     * @var atkPage
     */
    var $m_page = NULL;

    /**
     * List of available tabs. Associative array structured like this:
     * array($action=>$arrayOfTabnames)
     * @access private
     * @var array
     */
    var $m_tabList = array();

    /**
     * Keep track of tabs per attribute.
     * @access private
     * @var array
     */
    var $m_attributeTabs = array();

    /**
     * Keep track if a tab contains attribs (checkEmptyTabs function)
     * @access private
     * @var array
     */
    var $m_filledTabs = array();

    /**
     * The nodetype.
     * @access protected
     * @var String
     */
    var $m_type;

    /**
     * The module of the node.
     * @access protected
     * @var String
     */
    var $m_module;

    /**
     * The database that the node is using for storing and loading its data.
     * @access protected
     * @var mixed
     */
    var $m_db = NULL;

    /**
     * The table to use for data storage.
     * @access protected
     * @var String
     */
    var $m_table;

    /**
     * The name of the sequence used for autoincrement fields.
     * @access protected
     * @var String
     */
    var $m_seq;

    /**
     * Name of the attribute that contains the language of a record.
     * Used with ATK's data internationalization feature.
     * @access private
     * @var String
     */
    var $m_lngfield;

    /**
     * List of names of the attributes that form this node's primary key.
     * @access protected
     * @var array
     */
    var $m_primaryKey = array();

    /**
     * The postvars (or getvars) that are passed to a page will be passed
     * to the class using the dispatch function. We store them in a member
     * variable for easy access.
     * @access protected
     * @var array
     */
    var $m_postvars = array();

    /**
     * The action that the node is currently performing.
     * @access protected
     * @var String
     */
    var $m_action;

    /**
     * The active action handler.
     * @access protected
     * @var atkActionHandler
     */
    var $m_handler = NULL;

    /**
     * Default order by statement.
     * @access protected
     * @var String
     */
    var $m_default_order = "";

    /**
     * var used for tracking relation within this node.
     * @todo Remove this member, it's using memory while it's used only in
     *       the case of multilanguage node, and even then only on one
     *       occasion.
     * @access private
     * @var array
     */
    var $m_relations = array();

    /**
     * Bitwise mask of node flags (NF_* flags).
     * @var int
     */
    var $m_flags;

    /*
     * Name of the field that is used for creating an alphabetical index in
     * admin/select pages.
     * @access private
     * @var String
     */
    var $m_index = "";

    /**
     * Default tab being displayed in add/edit mode.
     * @access private
     * @var String
     */
    var $m_default_tab = "default";

    /**
     * Record filters, in attributename/required value pairs.
     * @access private
     * @var array
     */
    var $m_filters = array();

    /**
     * Record filters, as a list of sql statements.
     * @access private
     * @var array
     */
    var $m_fuzzyFilters = array();

    /**
     * For speed, we keep track of a list of attributes that we don't have to
     * load in recordlists.
     * @access protected
     * @var array
     */
    var $m_listExcludes = array();

    /**
     * For speed, we keep track of a list of attributes that we don't have to
     * load when in view pages.
     * @todo This can probably be moved to the view handler.
     * @access protected
     * @var array
     */
    var $m_viewExcludes = array();

    /**
     * For speed, we keep track of a list of attributes that have the cascade
     * delete flag set.
     * @todo This should be moved to the delete handler, or should not be
     *       cached at all. (caching this on each load is slower than just
     *       retrieving the list when it's needed)
     * @access private
     * @var array
     */
    var $m_cascadingAttribs = array();

    /**
     * Actions are mapped to security units.
     *
     * For example, both actions "save" and "add" require access "add". If an
     * item is not in this list, it's treated 'as-is'. Derived nodes may add
     * more mappings to tell the systems that some custom actions require the
     * same privilege as others.
     * Structure: array($action=>$requiredPrivilege)
     * @access protected
     * @var array
     */
    var $m_securityMap = array("save"=>"add",
                               "update"=>"edit",
                               "copy"=>"add",
                               "import"=>"add",
                               "editcopy"=>"add",
                               "search"=>"admin",
                               "smartsearch"=>"admin");

    /**
     * The right to execute certain actions can be implied by the fact that you
     * have some other right. For example, if you have the right to access a
     * feature (admin right), you may also view that record, and don't need
     * explicit rights to view it. So the 'view' right is said to be 'implied'
     * by the 'admin' right.
     * This is a subtle difference with m_securityMap.
     * @access protected
     * @var array
     */
    var $m_securityImplied = array("view"=>"admin");

    /**
     * Name of the node that is used for privilege checking.
     *
     * If a class is named 'project', then by default, if the system needs to
     *  know whether a user may edit a record, the securitymanager searches
     * for 'edit' access on 'project'. However, if an alias is set here, the
     * securitymanger searches for 'edit' on that alias.
     * @access private
     * @var String
     */
    var $m_securityAlias = "";

    /*
     * Nodes can specify actions that require no access level
     * Note: for the moment, the "select" action is always allowed.
     * @todo This may not be correct. We have to find a way to bind the
     * select action to the action that follows after the select.
     * @access private
     * @var array
     */
    var $m_unsecuredActions = array("select", "feedback");

    /*
     *
     * Boolean that is set to true when the stacktrace is displayed, so it
     * is displayed only once.
     * @deprecated This member is as deprecated as the statusbar() method.
     * @access private
     * @var boolean
     */
    var $m_statusbarDone = false;

    /**
     * Auto search-actions; action that will be performed if only one record
     * is found.
     * @access private
     * @var array
     */
    var $m_search_action;

    /**
     * Priority actions
     * @access private
     * @todo This, and the priority_min/max members, should be moved
     *       to the recordlist
     * @var array
     */
    var $m_priority_actions = array();

    /**
     * Minimum for the mra priority select
     * @access private
     * @var int
     */
    var $m_priority_min = 1;

    /**
     * Maximum for the mra priority select
     * @access private
     * @var int
     */
    var $m_priority_max = 0;

    /**
     * The lock instance
     * @access protected
     * @var atkLock
     */
    var $m_lock = NULL;

    /**
     * List of actions that should give success/failure feedback
     * @access private
     * @var array
     */
    var $m_feedback = array();

    /**
     * Default language used by Multilanguage Nodes.
     * @access protected
     * @var String
     */
    var $m_defaultlanguage = "";

    /**
     * Number to use with numbering
     * @access protected
     * @var mixed
     */
    var $m_numbering = null;

    /**
     * Descriptor template.
     * @access protected
     * @var String
     */
    var $m_descTemplate = NULL;

    /**
     * List of action listeners
     * @access protected
     * @var Array
     */
    var $m_listeners = array();

    /**
     * Tracker variable to see if we are currently in 'modifier mode' (running inside
     * the scope of a modname_nodename_modifier() method). The variable contains the
     * name of the modifying module.
     * @access private
     * @var String
     */
    var $m_modifier = "";
    
    /**
     * Extended search action. The action which is called if the user
     * wants to perform an extended search.
     * 
     * @access private
     * @var String
     */
    var $m_extended_search_action = NULL; 

    /**
     * Constructor.
     *
     * This initialises the node. Derived classes should always call their
     * parent constructor ($this->atkNode($name, $flags), to initialize the
     * base class.
     * <br>
     * <b>Example:</b>
     * <code>$this->atkNode('test',NF_NO_EDIT);</code>
     * @param String $type The nodetype (by default equal to the classname)
     * @param int $flags Bitmask of node flags (NF_*).
     */
    function atkNode($type="", $flags=0)
    {
      if ($type=="") $type = get_class($this);
      atkdebug("Creating a new atkNode for $type");
      $this->m_type = $type;
      $this->m_flags = $flags;
    }

    /**
     * Add an atkAttribute (or one of its derivatives) to the node.
     * @param atkAttribute $attribute The attribute you want to add
     * @param mixed $tab The tab(s) on which the attribute should be
     *                   displayed. Can be a tabname (String) or a list of
     *                   tabs (array) or "*" if the attribute should be
     *                   displayed on all tabs.
     * @param int $order The order at which the attribute should be displayed.
     *                   If ommitted, this defaults to 100 for the first
     *                   attribute, and 100 more for each next attribute that
     *                   is added.
     * @return mixed the attribute just added
     */
    function &add($attribute,$tab=NULL,$order=0)
    {
      static $attrib_order = 100;
      static $attrib_index = 0;
      $attribute->m_owner = $this->m_type;

      // If we're running inside modifier scope, we have to tell the attribute
      // what module he originated from.
      if ($this->m_modifier!="") $attribute->m_module = $this->m_modifier;

      // *** API HACK ***
      // Because tabs will probably be used more than the order override option
      // the API for this method now favours the $tab argument. For backwards
      // compatibility we still support the old API ($attribute,$order=0).
      if (!atkReadOptimizer())
      {
        if (!($tab === NULL) && is_int($tab))
        {
          $order = $tab;
          $tab = $this->m_default_tab;
        }
        // If no tab is specified or tabs are disabled, we use the current default tab
        // (specified with the setDefaultTab method, or "default" otherwise)
        elseif ($tab === NULL || strlen($tab) == 0 || !atkconfig("tabs")) $tab = $this->m_default_tab;

        $secMgr = &atkGetSecurityManager();
        if (!$secMgr->attribAllowed($this->m_type, $attribute->m_name, "edit"))
        {
          $attribute->m_flags |= AF_READONLY;

          if (!$secMgr->attribAllowed($this->m_type, $attribute->m_name, "view"))
          {
            $attribute->m_flags |= AF_HIDE;
          }
        }

        // check for parent fieldname (treeview)
        if($attribute->hasFlag(AF_PARENT))
        {
          $this->m_parent = $attribute->fieldName();
        }

        // check for cascading delete flag
        if ($attribute->hasFlag(AF_CASCADE_DELETE))
        {
          $this->m_cascadingAttribs[]=$attribute->fieldName();
        }

        if ($attribute->hasFlag(AF_HIDE_LIST)&&!$attribute->hasFlag(AF_PRIMARY))
        {
          if (!in_array($attribute->fieldName(),$this->m_listExcludes))
          {
            $this->m_listExcludes[]=$attribute->fieldName();
          }
        }

        if ($attribute->hasFlag(AF_HIDE_VIEW)&&!$attribute->hasFlag(AF_PRIMARY))
        {
          if (!in_array($attribute->fieldName(),$this->m_viewExcludes))
          {
            $this->m_viewExcludes[]=$attribute->fieldName();
          }
        }
      }

      else
      {
        // when the read optimizer is enabled there is no active tab
        // we circument this by putting all attributes on all tabs
        if (!($tab === NULL) && is_int($tab))
          $order = $tab;
        $tab = "*";
      }


      // NOTE: THIS SHOULD WORK. BUT, since add() is called from inside the $this
      // constructor, m_ownerInstance ends up being a copy of $this, rather than
      // a reference. Don't ask me why, it has something to do with the way PHP
      // handles the constructor.
      // To work around this, we reassign the this pointer to the attributes as
      // soon as possible AFTER the constructor. (the dispatcher function)
      $attribute->m_ownerInstance = &$this;

      if ($attribute->hasFlag(AF_PRIMARY))
      {
        if (!in_array($attribute->fieldName(),$this->m_primaryKey))
        {
          $this->m_primaryKey[] = $attribute->fieldName();
        }
      }

      if($attribute->hasFlag(AF_MULTILANGUAGE))
      {
        $this->m_lngfield = $attribute->fieldName();
      }

      $attribute->init();

      $exist=false;
      if(isset($this->m_attribList[$attribute->fieldName()])&&is_object($this->m_attribList[$attribute->fieldName()]))
      {
        $exist=true;
        // if order is set, overwrite it with new order, last order will count
        if($order!=0)
        {
          $this->m_attribIndexList[$this->m_attribList[$attribute->fieldName()]->m_index]["order"]=$order;
        }
      }
      if(!$exist)
      {
        if($order==0)
        {
          $order=$attrib_order;
          $attrib_order+=100;
        }

        if (!atkReadOptimizer())
        {
          // add new tab(s) to the tab list ("*" isn't a tab!)
          if (!is_array($tab) && $tab != "*") $tab = array($tab);
          if ($tab != "*")
          {
            if (!$attribute->hasFlag(AF_HIDE_ADD)) $this->m_tabList["add"] = isset($this->m_tabList["add"]) ? atk_array_merge($this->m_tabList["add"], $tab) : $tab;
            if (!$attribute->hasFlag(AF_HIDE_EDIT)) $this->m_tabList["edit"] = isset($this->m_tabList["edit"]) ? atk_array_merge($this->m_tabList["edit"], $tab) : $tab;
            if (!$attribute->hasFlag(AF_HIDE_ADD|AF_HIDE_EDIT) && !$attribute->hasFlag(AF_HIDE_VIEW)) $this->m_tabList["view"] = isset($this->m_tabList["view"]) ? atk_array_merge($this->m_tabList["view"], $tab) : $tab;
          }
        }

        $attribute->m_order = $order;
        $this->m_attribIndexList[$attrib_index]=array("name"=>$attribute->fieldName(),"tabs"=>$tab,"order"=>$attribute->m_order);
        $attribute->m_index = $attrib_index;
        //$attribute->m_tabs = $tab;
        $this->m_attributeTabs[$attribute->fieldname()] = $tab;
        $attrib_index++;
      }
      $this->m_attribList[$attribute->fieldName()]=&$attribute;
      if(is_subclass_of($attribute,"atkrelation"))
      {
        $this->m_relations[strtolower(get_class($attribute))][$attribute->fieldName()]=&$attribute;
      }


      $attribute->setTabs($this->m_attributeTabs[$attribute->fieldName()]);
      return $attribute;
    }

    /**
     * Remove an attribute.
     *
     * Completely removes an attribute from a node.
     * Note: Since other functionality may already depend on the attribute
     * that you are about to remove, it's often better to just hide an
     * attribute if you don't need it.
     * @param String $attribname The name of the attribute to remove.
     */
    function remove($attribname)
    {
      if (is_object($this->m_attribList[$attribname]))
      {
        $listindex = $this->m_attribList[$attribname]->m_index;

        unset($this->m_attribList[$attribname]);
        foreach($this->m_listExcludes as $i => $name)
        {
          if ($name == $attribname) unset($this->m_listExcludes[$i]);
        }
        foreach($this->m_viewExcludes as $i => $name)
        {
          if ($name == $attribname) unset($this->m_viewExcludes[$i]);
        }
        foreach($this->m_cascadingAttribs as $i => $name)
        {
          if ($name == $attribname) unset($this->m_cascadingAttribs[$i]);
        }

        unset($this->m_attribIndexList[$listindex]);
      }
    }

    /**
     * Get an attribute by name.
     * @param String $name The name of the attribute to retrieve.
     * @return atkAttribute The attribute.
     */
    function &getAttribute($name)
    {
      if (isset($this->m_attribList[$name]))
        return $this->m_attribList[$name];
      else return NULL;
    }

    /**
     * Checks if the node has a certain flag set.
     * @param int $flag The flag to check.
     * @return boolean True if the node has the flag.
     */
    function hasFlag($flag)
    {
      return (($this->m_flags & $flag) == $flag);
    }

    /**
     * Add a flag to the node.
     * @param int $flag The flag to add.
     */
    function addFlag($flag)
    {
      $this->m_flags |= $flag;
    }

    /**
     * Removes a flag from the node.
     *
     * @param int $flag The flag to remove from the attribute
     */
    function removeFlag($flag)
    {
      if($this->hasFlag($flag)) $this->m_flags ^= $flag;
    }

    /**
     * Returns the node flags.
     * @return Integer  node flags
     */
    function getFlags()
    {
      return $this->m_flags;
    }

    /**
     * Returns the primary key sql expression of a record.
     * @param array $rec The record for which the primary key is calculated.
     * @return String the primary key of the record.
     */
    function primaryKey($rec)
    {
      $primKey="";
      $nrOfElements = count($this->m_primaryKey);
      for ($i=0;$i<$nrOfElements;$i++)
      {
        $p_attrib = &$this->m_attribList[$this->m_primaryKey[$i]];
        $primKey.=$this->m_table.".".$this->m_primaryKey[$i]."='".$p_attrib->value2db($rec)."'";
        if ($i<($nrOfElements-1)) $primKey.=" AND ";
      }

      return $primKey;
    }

    /**
     * Retrieve the name of the primary key attribute.
     *
     * Note: If a node has a primary key that consists of multiple attributes,
     * this method will retrieve only the first attribute!
     * @return String First primary key attribute
     */
    function primaryKeyField()
    {
      return $this->m_primaryKey[0];
    }

    /**
     * Returns a primary key template.
     *
     * Like primaryKey(), this method returns a sql expression, but in this
     * case, no actual data is used. Instead, template fields are inserted
     * into the expression. This is useful for rendering multiple primary
     * keys later with a record and a template parser.
     *
     * @return String Primary key template
     */
    function primaryKeyTpl()
    {
      $primKey="";
      $nrOfElements = count($this->m_primaryKey);
      for ($i=0;$i<$nrOfElements;$i++)
      {
        $primKey.=$this->m_primaryKey[$i]."='[".$this->m_primaryKey[$i]."]'";
        if ($i<($nrOfElements-1)) $primKey.=" AND ";
      }
      return $primKey;
    }


    /**
     * Set default sort order for the node.
     * @param String $orderby Default order by. Can be an attribute name or a
     *                        SQL expression.
     */
    function setOrder($orderby)
    {
      $this->m_default_order = $orderby;
    }

    /**
     * Get default sort order for the node.
     * @return String $orderby Default order by. Can be an attribute name or a
     *                        SQL expression.
     */
    function getOrder()
    {
      return $this->m_default_order;
    }


    /**
     * Set the table that the node should use.
     *
     * Note: This should be called in the constructor of derived classes,
     * after the base class constructor is called.
     * @param String $tablename The name of the table to use.
     * @param String $seq The name of the sequence to use for autoincrement
     *                    attributes.
     * @param mixed $db The database connection to use. If ommitted, this
     *                  defaults to the default database connection.
     *                  So in apps using only one database, it's not necessary
     *                  to pass this parameter.
     *                  You can pass either a connection (atkDb instance), or
     *                  a string containing the name of the connection to use.
     */
    function setTable($tablename,$seq="",$db=NULL)
    {
      $this->m_table      = $tablename;
      if ($seq=="") $seq = $tablename;
      $this->m_seq        = $seq;
      $this->m_db = $db;
    }

    /**
     * Get the database connection for this node.
     * @return atkDb Database connection instance
     */
    function &getDb()
    {
      if ($this->m_db == NULL)
      {
        return atkGetDb();
      }
      else if (is_object($this->m_db))
      {
        return $this->m_db;
      }
      else
      {
        // must be a named connection
        return atkGetDb($this->m_db);
      }
    }

    /**
     * Create an alphabetical index.
     *
     * Any string- or textbased attribute can be used to create an
     * alphabetical index in admin- and selectpages.
     * @param String $attribname The name of the attribute for which to create
     *                           the alphabetical index.
     */
    function setIndex($attribname)
    {
      $this->m_index = $attribname;
    }

    /**
     * Set default tab being displayed in view/add/edit mode.
     * After calling this method, all attributes which are added after the
     * method call without specification of tab will be placed on the default
     * tab. This means you should use this method before you add any
     * attributes to the node.
     * If you accept the default name for the first tab ("default") you do not
     * need to call this method.
     * @param String $tab the name of the default tab
     */
    function setDefaultTab($tab="default")
    {
      $this->m_default_tab = $tab;
    }

    /**
     * Get a list of tabs for a certain action.
     * @param String $action The action for which you want to retrieve the
     *                       list of tabs.
     * @return array The list of tabnames.
     *
     */
    function getTabs($action)
    {
      $list = &$this->m_tabList[$action];

      $this->checkTabRights($list);

      $tabAttribSet = array();
      $tabCode = "";

      if (!is_array($list))
      {
        // fallback to view tabs.
        $list = &$this->m_tabList["view"];
      }

      // Attributes can also add tabs to the tablist.
      foreach(array_keys($this->m_attribList) as $attribname)
      {
        $p_attrib = &$this->m_attribList[$attribname];

        // Only display the attribute if the attribute
        // resides on at least on visible tab
        $hideme = true;
        for($i=0, $_i=sizeof($p_attrib->m_tabs); $i<$_i; $i++)
        {
          if (is_array($list) && in_array($p_attrib->m_tabs[$i],$list))
          {
            $hideme = false;
            break;
          }
        }
        if ($hideme)
          $p_attrib->m_flags |= AF_HIDE_LIST;

        if(is_object($p_attrib))
        {
          $list = atk_array_merge($list, $p_attrib->getAdditionalTabs($action));

          // Keep track of the tabs that containg attribs
          // so we only display none-empty tabs
          $tabCode = $this->m_attributeTabs[$attribname][0];
          if (!isset($this->m_filledTabs[$tabCode]))
          {
            $this->m_filledTabs[$tabCode] = true;
          }
        }
        else
        {
          atkdebug("atknode::getTabs() Warning: $attribname is not an object!?");
        }
      }

      // Check if the currently known tabs all containg attributes
      // so we don't end up with empty tabs
      $tabList = $this->checkEmptyTabs($list);
      return $tabList;
    }

    /**
     * Check if the user has the rights to access existing tabs and
     * removes tabs from the list that may not be accessed
     *
     * @param array $tablist Array containing the current tablist
     */
    function checkTabRights(&$tablist)
    {
      global $g_nodes;
      for ($i=0, $_i=count($tablist); $i<$_i; $i++)
      {
        if ($tablist[$i] == "" || $tablist[$i] == "default") continue;
        $secMgr = &atkGetSecurityManager();

        // load the $g_nodes array to find out what tabs are required
        if (!is_array($g_nodes[$this->m_module][$this->m_type]))
        {
          include_once(atkconfig("atkroot") . "atk/atknodetools.inc");
          $module = &getModule($this->m_module);
          $module->getNodes();
        }

        $priv = "tab_" . $tablist[$i];
        if (atk_in_array($priv,$g_nodes[$this->m_module][$this->m_type]))
        {
          // authorisation is required
          if (!$secMgr->allowed($this->m_module.".".$this->m_type,"tab_" . $tablist[$i]))
          {
            atkdebug("Removing TAB ". $tablist[$i] . " because access to this tab was denied</b>");
            unset($tablist[$i]);
          }
        }
        else atkdebug("TAB " . $tablist[$i] . " does not need authorisation",DEBUG_WARNING);
      }

      if (is_array($tablist))
      {
        // we might have now something like:
        // [0]=>tabA,[3]=>tabD
        // we convert this to a 'normal' array:
        // [0]=>tabA,[1]=>tabD;
        $newarray = array();
        foreach($tablist as $tab)
          $newarray[] = $tab;
        $tablist = $newarray;
      }
    }

    /**
     * Remove tabs without attribs from the tablist
     * @param array $list The list of tabnames
     * @return array The list of tabnames without the empty tabs.
     *
     */
    function checkEmptyTabs($list)
    {
      $tabList = array();

      if (is_array($list))
      {
        foreach ($list AS $tabEntry)
        {
          if (isset($this->m_filledTabs[$tabEntry]))
          {
            $tabList[] = $tabEntry;
          }
          else
          {
            atkdebug("Removing TAB ".$tabEntry." because it had no attributes assigned");
          }
        }
      }

      return $tabList;
    }

    /**
     * Returns the currently active tab.
     *
     * Note that in themes which use dhtml tabs (tabs without reloads), this
     * method will always return the name of the first tab.
     * @return String The name of the currently visible tab.
     */
    function getActiveTab()
    {
      global $ATK_VARS;
      $tablist = $this->getTabs($ATK_VARS["atkaction"]);

      // Note: we may not read atktab from $this->m_postvars, because $this->m_postvars is not filled if this is
      // a nested node (in a relation for example).
      if (!empty($ATK_VARS["atktab"]) && in_array($ATK_VARS["atktab"], $tablist)) $tab = $ATK_VARS["atktab"];
      elseif (!empty($this->m_default_tab) && in_array($this->m_default_tab, $tablist)) $tab = $this->m_default_tab;
      else $tab = $tablist[0];
      return $tab;
    }

    /**
     * Add a recordset filter.
     * @param String $filter The fieldname you want to filter OR a SQL where
     *                       clause expression.
     * @param String $value Required value. (Ommit this parameter if you pass
     *                      an SQL expression for $filter.)
     */
    function addFilter($filter, $value="")
    {
      if ($value=="")
      {
        // $key is a where clause kind of thing
        $this->m_fuzzyFilters[] = $filter;
      }
      else
      {
        // $key is a $key, $value is a value
        $this->m_filters[$filter] = $value;
      }
    }

    /**
     * Returns the form buttons for a certain page.
     *
     * Can be overridden by derived classes to define custom buttons.
     * @param String $mode The action for which the buttons are retrieved.
     * @param array $record The record currently displayed/edited in the form.
     *                      This param can be used to define record specific
     *                      buttons.
     */
    function getFormButtons($mode, $record)
    {
      $result = array();

      // edit mode
      if ($mode == "edit")
      {
        // if atklevel is 0 or less, we are at the bottom of the session stack,
        // which means that 'saveandclose' doesn't close anyway, so we leave out
        // the 'saveandclose' button. Unless, a feedback screen is configured.
        if (atkLevel() >0 || hasFlag($this->m_feedback["update"], ACTION_SUCCESS))
        {
          $result[] = '<input type="submit" class="btn_saveandclose" name="atksaveandclose" value="'.atktext('saveandclose', "atk").'">';
        }
        $result[] = '<input type="submit" class="btn_save" name="atknoclose" value="'.atktext("save", "atk").'">';
        $result[] = '<input type="submit" class="btn_cancel" name="atkcancel" value="'.atktext("cancel", "atk").'">';
      }
      elseif ($mode == "add")
      {
        $result[] = '<input type="submit" class="btn_save" value="'.atktext("save", "atk").'">';
        // if action is admin, we don't show the cancelbutton or the add next button
        if ($this->hasFlag(NF_ADDAFTERADD) && !$this->hasFlag(NF_EDITAFTERADD))
          $result[] = '<input type="submit" class="btn_saveandnext" name="atksaveandnext" value="'.atktext("saveandnext", "atk").'">';
        $result[] = '<input type="submit" class="btn_cancel" name="atkcancel" value="'.atktext("cancel", "atk").'">';
      }
      elseif ($mode == "view")
      {
        if(atkLevel() > 0)
          $result[] = '<input type="submit" class="btn_cancel" name="atkback" value="&lt;&lt; '.atktext('back').'">';
        // if appropriate, display an edit button.
        if (!$this->hasFlag(NF_NO_EDIT)&&$this->allowed("edit", $record))
        {
          $result[] = '<input type="hidden" name="atkaction" value="edit">'.
                      '<input type="hidden" name="atknodetype" value="'.$this->atkNodeType().'">'.
                      '&nbsp;<input type="submit" class="btn_save" value="'.atktext('edit').'">&nbsp;';
        }
      }

      return $result;
    }

    /**
     * Generate a box displaying a message that the current record is locked.
     * @return String The HTML fragment containing a box with the message and
     *                a back-button.
     */
    function lockPage()
    {
      $output = $this->statusbar();
      $output.= '<img src="'.atkconfig("atkroot").'atk/images/lock.gif"><br><br>'.atktext("lock_locked").'<br>';
      $output.= '<br><form method="get">'.session_form(SESSION_BACK).
                        '<input type="submit" class="btn_cancel" value="&lt;&lt; '.atktext('back').'"></form>';

      $ui = &$this->getUi();
      if (is_object($ui))
      {
        $total = $ui->renderBox(array("title"=>$this->actionTitle($this->m_action),
                                      "content"=>$output));
      }
      return $total;

    }

    /**
     * Get the ui instance for drawing and templating purposes.
     *
     * @return atkUi An atkUi instance for drawing and templating.
     */
    function &getUi()
    {
      $ui = &atkinstance("atk.ui.atkui");
      return $ui;
    }

    /**
     * Generate a title for a certain action on a certain action.
     *
     * The default implementation displayes the action name, and the
     * descriptor of the current record between brackets. This can be
     * overridden by derived classes.
     * @param String $action The action for which the title is generated.
     * @param array $record The record for which the title is generated.
     * @return String The full title of the action.
     */
    function actionTitle($action, $record="")
    {
      global $g_sessionManager;
      $ui = &$this->getUi();

      $res = "";

      if ($record!="")
      {
        $descr = $this->descriptor($record);
        $g_sessionManager->pageVar("descriptor",$descr);
      }

      $descriptortrace = $g_sessionManager->descriptorTrace();
      $nomodule=false;
      if (!empty($descriptortrace))
      {
        $nomodule=true;
        $descrtrace = "";
        foreach ($descriptortrace as $desc)
        {
          $descrtrace.= htmlentities($desc, ENT_COMPAT,atktext('charset'))." - ";
        }
        $res= $descrtrace . $res;
      }
      if (is_object($ui))
      {
        $res.= $ui->title($this->m_module, $this->m_type, $action,$nomodule);
      }

      return $res;
    }

    /**
     * Place a set of tabs around content.
     * @param String $action The action for which the tabs are loaded.
     * @param String $content The content that is to be displayed within the
     *                        tabset.
     * @return String The complete tabset with content.
     */
    function tabulate($action, $content)
    {
      $this->addStyle("tabs.css");
      $list = $this->getTabs($action);
      if (count($list) > 1)
      {
        $ui = &$this->getUi();
        if (is_object($ui))
        {
          return $ui->renderTabs(array("tabs"=>$this->buildTabs($action),
                                       "content"=>$content));
        }
      }
      return $content;
    }

    /**
     * Determine the default form parameters for an action template.
     * @param boolean $locked If the current record is locked, pass true, so
     *                        the lockicon can be placed in the params too.
     * @return array Default form parameters for action forms (assoc. array)
     */
    function getDefaultActionParams($locked=false)
    {
      $params = $this->getHelp();
      $params["lockstatus"] = $this->getLockStatusIcon($locked);
      $params["formend"] = '</form>';
      return $params;
    }

    /**
     * Function outputs an array with edit fields. For each field the array
     * contains the name, edit HTML code etc. (name, html, obligatory,
     * error, label)
     *
     * @todo The editArray method should use a set of classes to build the
     *       form, instead of an array with an overly complex structure.
     * @param String $mode        The edit mode ("add" or "edit")
     * @param array $record       The record currently being edited.
     * @param array $forcelist    A key-value array used to preset certain
     *                            fields to a certain value, regardless of the
     *                            value in the record.
     * @param array $suppresslist List of attributenames that you want to hide
     * @param String $fieldprefix Of set, each form element is prefixed with
     *                            the specified prefix (used in embedded form
     *                            fields)
     * @return array List of edit fields (per field ( name, html, obligatory,
     *               error, label })
     */
    function editArray($mode="add", $record=NULL, $forceList="", $suppressList="", $fieldprefix="")
    {
      global $g_sessionData;

      /* read metadata */
      $this->setAttribSizes();

      /* default values */
      if (!empty($record)) $defaults = $record;
      else $defaults = array();

      /* formdata from session */
      if (isset($this->m_postvars["atkformdata"])&&is_array($this->m_postvars["atkformdata"]))
      {
        atkdebug("Restoring formdata from session");
        $this->modifyRecord($defaults, $this->m_postvars["atkformdata"]);
        $this->m_postvars["atkformdata"]="";
        $g_sessionData[$this->m_action]['stack'][atkStackID()][atkLevel()]['atkformdata']="";
      }

      /* edit mode */
      if ($mode == "edit")
      {
        $result["hide"][] = '<input type="hidden" name="'.$fieldprefix.'atkaction" value="update">';

        /* nodes can define edit_values */
        $overrides = $this->edit_values($defaults);
        foreach ($overrides as $varname=>$value)
        {
          $defaults[$varname]=$value;
        }
      }

      /* add mode */
      else
      {
        $result["hide"][] = '<input type="hidden" name="'.$fieldprefix.'atkaction" value="save">';

        /* nodes can define initial values, if they don't already have values. */
        $overrides = $this->initial_values();
        foreach ($overrides as $varname=>$value)
        {
          if (!isset($defaults[$varname]) || $defaults[$varname]=="") $defaults[$varname]=$value;
        }
      }

      /* check for forced values */
      if (is_array($forceList))
      {
        foreach ($forceList as $forcedvarname=>$forcedvalue)
        {
          $attribname="";
          if ($forcedvarname!="")
          {
            if (strpos($forcedvarname,'.')>0)
            {
              list($firstpart,$field) = split('\.',$forcedvarname);
              if ($firstpart==$this->m_table)
              {
                // this is a filter on the current table.
                $defaults[$field] = $forcedvalue;
                $attribname = $field;
              }
              else
              {
                // this is a filter on a field of another table (something we have a
                // relationship with.if(is_object($this->m_attribList[$table]))
                if(is_object($this->m_attribList[$firstpart]))
                {
                  $defaults[$firstpart][$field] = $forcedvalue;
                  $attribname = $firstpart;
                }
                else
                {
                  // This is not a filter for this node.
                }
              }
            }
            else
            {
              $defaults[$forcedvarname]=$forcedvalue;
              $attribname = $forcedvarname;
            }

            if($attribname!="")
            {
              $p_attrib = &$this->m_attribList[$attribname];
              if (is_object($p_attrib)&&(!$p_attrib->hasFlag(AF_NO_FILTER))) $p_attrib->m_flags |= AF_READONLY|AF_HIDE_ADD;
            }
          }
        }
      }

      /* extra submission data */
      //$result["hide"][] = '<input type="hidden" name="'.$fieldprefix.'atkescape">';
      $result["hide"][] = '<input type="hidden" name="'.$fieldprefix.'atknodetype" value="'.$this->atknodetype().'">';
      $result["hide"][] = '<input type="hidden" name="'.$fieldprefix.'atkprimkey" value="'.$record["atkprimkey"].'">';

      /* For all attributes we use the edit() method to get HTML code for editting the
       * attribute's data. If the attribute is hidden we use the hide() method method
       * to get HTML code for hideing the attribute's data. You can override the attribute's
       * edit() method by supplying an <attributename>_edit function in the derived classes.
       */
      $tab = $this->getActiveTab();

      foreach (array_keys($this->m_attribIndexList) as $r)
      {
        $attribname = $this->m_attribIndexList[$r]["name"];
        $field = array("name" => $attribname);
        $p_attrib = &$this->m_attribList[$attribname];
        if ($p_attrib!=NULL)
        {
          /* fields that have not yet been initialised may be overriden in the url */
          if (!isset($defaults[$p_attrib->fieldName()]) && isset($this->m_postvars[$p_attrib->fieldName()]))
          {
            $defaults[$p_attrib->fieldName()] = $this->m_postvars[$p_attrib->fieldName()];
          }

          /* sometimes a field is hidden although not specified by the field itself */

          $theme = &atkinstance("atk.ui.atktheme");
          if ($theme->getAttribute("tabtype") == "dhtml") { $notOnTab = FALSE; } else { $notOnTab = !$p_attrib->showOnTab($tab); }

          if ((is_array($suppressList) && count($suppressList) > 0 && in_array($attribname,$suppressList)) || $notOnTab)
          {
            $p_attrib->m_flags |= ($mode == "add" ? AF_HIDE_ADD : AF_HIDE_EDIT);
          }

          /* we let the attribute add itself to the edit array */
          $p_attrib->addToEditArray($mode, $result, $defaults, $record['atkerror'], $fieldprefix);
        }
        else
        {
          atkerror("Attribute $attribname not found!");
        }
      }

      /* check for errors */
      $result["error"] = $record['atkerror'];

      /* return the result array */
      return $result;
    }

    /**
     * Retrieve the initial values for a new record.
     *
     * The system calls this method to create a new record. By default
     * this method returns an empty record, but derived nodes may override
     * this method to perform record initialization.
     *
     * @return array Array containing an initial value per attribute.
     *               Only attributes that are initialized appear in the
     *               array.
     */
    function initial_values()
    {
      return array();
    }

    /**
     * Retrieve new values for an existing record.
     *
     * The system calls this method to override the values of a record
     * before editing the record.
     * The default implementation does not do anything to the record, but
     * derived classes may override this method to make modifications to.
     * the record.
     *
     * @param array $record The record that is about to be edited.
     * @return array The manipulated record.
     */
    function edit_values($record)
    {
      return $record;
    }


    /**
     * Get the template to use for a certain action.
     *
     * The system calls this method to determine which template to use when
     * rendering a certain screen. The default implementation always returns
     * the same template for the same action (it ignores parameter 2 and 3).
     * You can override this method in derived classes however, to determine
     * on the fly which template to use.
     * The action, the current record (if any) and the tab are passed as
     * parameter. By using these params, you can have custom templates per
     * action, and/or per tab, and even per record.
     *
     * @param String $action The action for which you wnat to retrieve the
     *                       template.
     * @param array $record  The record for which you want to return the
     *                       template (or NULL if there is no record).
     * @param String $tab    The name of the tab for which you want to
     *                       retrieve the template.
     * @return String The filename of the template (without path)
     */
    function getTemplate($action, $record=NULL, $tab="")
    {
      switch ($action)
      {
        case "add": // add and edit both use the same form.
        case "edit": return "editform.tpl";
        case "view": return "viewform.tpl";
        case "search": return "searchform.tpl";
        case "smartsearch": return "smartsearchform.tpl";        
      }
    }

    /**
     * Function outputs a form with all values hidden.
     *
     * This is probably only useful for the atkOneToOneRelation's hide method.
     *
     * @param String $mode        The edit mode ("add" or "edit")
     * @param array $record       The record that should be hidden.
     * @param array $forcelist    A key-value array used to preset certain
     *                            fields to a certain value, regardless of the
     *                            value in the record.
     * @param String $fieldprefix Of set, each form element is prefixed with
     *                            the specified prefix (used in embedded form
     *                            fields)
     * @return String HTML fragment containing all hidden elements.
     *
     */
    function hideForm($mode="add",$record = NULL, $forceList="", $fieldprefix="")
    {
      global $g_sessionData;

      /* suppress all */
      $suppressList = array();
      foreach (array_keys($this->m_attribIndexList) as $r)
        $suppressList[] = $this->m_attribIndexList[$r]["name"];

      /* get data, transform into "form", return */
      $data = $this->editArray($mode, $record, $forceList, $suppressList, $fieldprefix);
      foreach ($data["hide"] as $hide) $form .= $hide;
      return $form;
    }

    /**
     * Builds a list of tabs.
     *
     * This doesn't generate the actual HTML code, but returns the data for
     * the tabs (title, selected, urls that should be loaded upon click of the
     * tab etc).
     * @param String $action The action for which the tabs should be generated.
     * @return array List of tabs
     * @todo Make translation of tabs module aware
     */
    function buildTabs($action="")
    {
      if ($action=="")
      {
        // assume active action
        $action = $this->m_action;
      }

      $result = array();

      // which tab is currently selected
      $tab = $this->getActiveTab();

      // build navigator
      $list = &$this->getTabs($action);

      if (is_array($list))
      {
        $newtab["total"] = count($list);
        foreach ($list as $t)
        {
          $newtab["title"] = atktext(array("tab_$t", $t),$this->m_module,$this->m_type);
          $newtab["tab"] = $t;
          $url = $_SERVER["PHP_SELF"]."?atknodetype=".$this->atkNodeType()."&atkaction=".$this->m_action."&atktab=".$t;
          if ($this->m_action == "view")
          {
            $newtab["link"] = session_url($url, SESSION_DEFAULT);
          }
          else
          {
            $newtab["link"] = "javascript:atkSubmit('".atkurlencode(session_url($url, SESSION_DEFAULT))."')";
          }
          $newtab["selected"] = ($t == $tab);
          $result[] = $newtab;
        }
      }
      return $result;
    }

    /**
     * Retrieve an array with the default actions for a certain mode.
     *
     * This will return a list of actions that can be performed on records
     * of this node in an admin screen.
     * The actions may contain a [pk] template variable to reference a record,
     * so for each record you should run the stringparser on the action.
     *
     * @param String $mode The mode for which you want a list of actions.
     *                     Currently available modes for this method:
     *                     - "admin" (for actions in adminscreens)
     *                     - "relation" (for the list of actions when
     *                       displaying a recordlist in a onetomany-relation)
     *                     - "view" (for actions when viewing only)
     *                     Note: the default implementation of defaultActions
     *                     makes no difference between "relation" and "admin"
     *                     and will return the same actions for both, but you
     *                     might want to override this behaviour in derived
     *                     classes.
     * @param array $params An array of extra parameters to add to all the
     *                      action urls. You can use this to pass things like
     *                      an atkfilter for example. The array should be
     *                      key/value based.
     * @return array List of actions in the form array($action=>$actionurl)
     */
    function defaultActions($mode, $params=array())
    {
      $actions = array();
      $postfix = "";

      if (count($params)>0)
      {
        foreach ($params as $key=>$value)
        {
          $postfix.= "&$key=".rawurlencode($value);
        }
      }

      // Changed: it used to be that you could only view if you didn't have
      // edit right. This was changed because of Achievo bug #41
      // (http://www.achievo.org/bug/41)
      $actionbase = $_SERVER["PHP_SELF"].'?atknodetype='.$this->atknodetype().'&atkselector=[pk]'.$postfix;
      if (!$this->hasFlag(NF_NO_VIEW)&&$this->allowed("view"))
      {
        $actions["view"] = $actionbase.'&atkaction=view';
      }

      if ($mode!="view")
      {
        if (!$this->hasFlag(NF_NO_EDIT)&&$this->allowed("edit"))
        {
          $actions["edit"] = $actionbase.'&atkaction=edit';
        }

        if (!$this->hasFlag(NF_NO_DELETE)&&$this->allowed("delete"))
        {
          $actions["delete"] = $actionbase.'&atkaction=delete';
        }
        if($this->hasFlag(NF_COPY)&&$this->allowed("copy"))
        {
          $actions["copy"] = $actionbase.'&atkaction=copy';
        }
      }

      return $actions;
    }

    /**
     * Sets the priority range, for multi-record-priority actions.
     * @param int $min the minimum priority
     * @param int $max the maximum priority (0 for auto => min + record count)
     */
    function setPriorityRange($min=1, $max=0)
    {
      $this->m_priority_min = (int)$min;
      if ($max < $this->m_priority_min) $max = 0;
      else $this->m_priority_max = $max;
    }

    /**
     * Sets the possible multi-record-priority actions.
     * @param array $actions list of actions
     */
    function setPriorityActions($actions)
    {
      if (!is_array($actions)) $this->m_priority_actions = array();
      else $this->m_priority_actions = $actions;
    }
    
    /**
     * Get extended search action.
     * 
     * @return extended search action
     */
    function getExtendedSearchAction()
    {
      if (empty($this->m_extended_search_action))
        return atkconfig('extended_search_action');
      else return $this->m_extended_search_action;
    }
    
    /**
     * Set extended search action.
     * 
     * @param $action extended search action
     */
    function setExtendedSearchAction($action)
    {
      $this->m_extended_search_action = $action;
    }

    /**
     * Function returns a page in which the user is asked if he really wants
     * to perform a certain action.
     * @param mixed $atkselector Selector of current record on which the
     *                           action will be performed (String), or an
     *                           array of selectors when multiple records are
     *                           processed at once. The method uses the
     *                           selector(s) to display the current record(s)
     *                           in the confirmation page.
     * @param String $action The action for which confirmation is needed.
     * @param boolean $locked Pass true if the current record is locked.
     * @param boolean $checkoverride If set to true, this method will try to
     *                               find a custom method named
     *                               "confirm".$action."()" (e.g.
     *                               confirmDelete() and call that method
     *                               instead.
     * @return String Complete html fragment containing a box with the
     *                confirmation page, or the output of the custom
     *                override if $checkoverride was true.
     */
    function confirmAction($atkselector, $action, $locked=false, $checkoverride=true)
    {
      $method = 'confirm'.$action;
      if ($checkoverride && method_exists($this, $method))
        return $this->$method($atkselector, $locked);

      $ui = &$this->getUi();

      $this->addStyle("style.css");

      if (is_array($atkselector))
        $atkselector_str = '(('. implode($atkselector, ') OR (').'))';
      else $atkselector_str = $atkselector;

      $formstart ='<form action="'.$_SERVER["PHP_SELF"].'?"'.SID.' method="post">';
      $formstart.=session_form();
      $formstart.='<input type="hidden" name="atkaction" value="'.$action.'">';
      $formstart.='<input type="hidden" name="atknodetype" value="'.$this->atknodetype().'">';
      $formstart.='<input type="hidden" name="atkselector" value="'.$atkselector_str.'">';

      $buttons = array();
      $buttons[] = '<input name="confirm" class="btn" type="submit" value="'.atktext('yes').'">';
      $buttons[] = '<input name="cancel" type="submit" class="btn_cancel" value="'.atktext('no').'">';

      $content = "";
      $recs = $this->selectDb($atkselector_str, "", "", "", $this->descriptorFields());
      if (count($recs)==1)
      {
        // 1 record, put it in the page title (with the actionTitle call, a few lines below)
        $record = $recs[0];
      }
      else
      {
        // we are gonna perform an action on more than one record
        // show a list of affected records, at least if we can find a
        // descriptor_def method
        if ($this->m_descTemplate != NULL || method_exists($this,"descriptor_def"))
        {
          $record = "";
          $content.= "<ul>";
          for ($i=0, $_i=count($recs); $i<$_i; $i++)
          {
            $content.="<li> ".$this->descriptor($recs[$i]);
          }
          $content.= "</ul>";
        }
      }

      $content.= '<br>'.$this->confirmActionText($atkselector, $action, $checkoverride);

      $output=$ui->renderAction($action, array("content"=>$content,
                                                "formstart"=>$formstart,
                                                "formend"=>'</form>',
                                                "buttons"=>$buttons));
      return $ui->renderBox(array("title"=>$this->actionTitle('delete', $record),
                                  "content"=>$output));
    }

    /**
     * Determine the confirmation message.
     * @param String $atkselector The record(s) on which the action is
     *                            performed.
     * @param String $action The action being performed.
     * @param boolean $checkoverride If true, returns the output of a custom
     *                               method named "confirm".$action."text()"
     * @return String The confirmation text.
     */
    function confirmActionText($atkselector="", $action="delete", $checkoverride=TRUE)
    {
      $method = 'confirm'.$action.'text';
      if ($checkoverride && method_exists($this, $method)) return $this->$method($atkselector);
      else return atktext("confirm_$action".(is_array($atkselector) && count($atkselector) > 1 ? '_multi' : ''), $this->m_module, $this->m_type);
    }

    /**
     * Small compare function for sorting attribs on order field
     * @access private
     */
    function attrib_cmp($a,$b)
    {
      if ($a["order"] == $b["order"]) return 0;
      return ($a["order"] < $b["order"]) ? -1 : 1;
    }

    /**
     * This function initialises certain elements of the node.
     *
     * This must be called right after the constructor. The function has a
     * check to prevent it from being executed twice. If you construct a node
     * using 'new', you have to call this method. If you construct it with the
     * getNode or newNode method, you don't have to call this method.
     */
    function init()
    {
      atkdebug("init for ".$this->m_type);
      global $g_modifiers;

      // Check if initialisation is not already done.
      if ($this->m_initialised == true) return;

      // We assign the $this reference to the attributes at this stage, since
      // it fails when we do it in the add() function.
      // See also the comments in the add() function.
      foreach (array_keys($this->m_attribList) as $attribname)
      {
        $p_attrib = &$this->m_attribList[$attribname];
        $p_attrib->m_ownerInstance = &$this;
      }

      // See if there are modules active that modify this node, and apply the
      // modifiers if found.
      if (isset($g_modifiers[$this->atknodetype()]))
      {
        foreach ($g_modifiers[$this->atknodetype()] as $modulename)
        {
          $module = &getModule($modulename);
          $module->modifier($this);
        }
      }

      $this->_addListeners();

      // We set the tabs for the attributes
      foreach (array_keys($this->m_attribList) as $attribname)
      {
        $p_attrib = &$this->m_attribList[$attribname];
        $p_attrib->setTabs($this->m_attributeTabs[$attribname]);
      }

      $this->attribSort();

      $lockType = atkconfig("lock_type");
      if (!empty($lockType) && $this->hasFlag(NF_LOCK))
      {
        $this->m_lock = &atkinstance("atk.lock.atklock");
      }
      else $this->removeFlag(NF_LOCK);

      $this->m_defaultlanguage = strtoupper(atkconfig("defaultlanguage"));

      $this->m_initialised = true;
    }

    /**
     * Add the listeners for the current node
     * A listener can be defined either by placing an instantiated object
     * or the full location in atkimport style notation, in a global array
     * called $g_nodeListeners (useful for example for adding listeners
     * to nodes from another module's module.inc file. in module.inc files,
     * $listeners can be used to add listeners to a node.
     * @access private
     */
    function _addListeners()
    {
      global $g_nodeListeners;
      if (isset($g_nodeListeners[$this->atknodetype()]))
      {
        foreach ($g_nodeListeners[$this->atknodetype()] as $listener)
        {
          if (is_object($listener))
          {
            $this->addListener($listener);
          }
          else
          {
            if (is_string($listener))
            {
              $listenerobj = &atknew($listener);
              if (is_object($listenerobj))
              {
                $this->addListener($listenerobj);
              }
              else
                atkdebug("We couldn't find a classname for listener with supposed nodetype: '$listener'");
            }
            else
            {
              atkdebug("Failed to add listener with supposed nodetype: '$listener'");
            }
          }
        }
      }
    }

    /**
     * This function reads meta information from the database and initialises
     * its attributes with the metadata.
     *
     * This method should be called before rendering a form, if you want the
     * sizes of all the inputs to match the fieldlengths from the database.
     */
    function setAttribSizes()
    {
      if ($this->m_attribsizesset) return true;

      $db = &$this->getDb();
      $metainfo = $db->tableMeta($this->m_table);

      foreach (array_keys($this->m_attribList) as $attribname)
      {
        $p_attrib = &$this->m_attribList[$attribname];
        if (is_object($p_attrib))
        {
          $p_attrib->fetchMeta($metainfo);
        }
      }
      $this->m_attribsizesset = true;
    }

    /**
     * This is the wrapper method for all http requests on a node.
     *
     * The method looks at the atkaction from the postvars and determines what
     * should be done. If possible, it instantiates actionHandlers for
     * handling the actual action.
     *
     * @param array $postvars The request variables for the node.
     * @param boolean $fullpage If true (default), the dispatch function
     *                          renders a complete html page, including
     *                          <html><head><body> etc. If you set this to
     *                          false, plain output is generated. Warning: If
     *                          you set this to false, you are responsible for
     *                          loading all the required scripts, stylesheets
     *                          etc. (When $fullpage is true, these are
     *                          automatically included in the page header.
     */
    function dispatch($postvars, $fullpage=true)
    {
      $this->loadDispatchPage($postvars);

      $ui = &$this->getUi();
      $page = &$this->getPage();
      if (!$page->isEmpty()) // Only output an html output if there is anything to output.
      {
        $screen = $page->render(atktext('app_shorttitle')." - ".$ui->title($this->m_module, $this->m_type,$this->m_postvars['atkaction']), $fullpage);

        $output = &atkOutput::getInstance();
        $output->output($screen);
      }

      // This is the end of all things for this page..
      // so we clean up some resources..
      $db = &$this->getDb();
      if (is_object($db)) $db->disconnect();
      atkdebug("disconnected from the database");
    }

    /**
     * Does the actual loading of the dispatch page
     * And adds it to the page for the dispatch() method to render.
     * @param array $postvars The request variables for the node.
     */
    function loadDispatchPage($postvars)
    {
      $this->m_postvars = $postvars;

      $this->m_action = $postvars['atkaction'];

      /** DIRTY HACK
          if action is something else than edit, we clear the atkformdata sessionstuff **/
      if ($this->m_action!="edit"&&$this->m_action!="add"&&$this->m_action!="admin")
      {
        atkdebug("Clearing stored formdata");
        $this->m_postvars["atkformdata"]="";
      }

      $page = &$this->getPage();

      if ($this->allowed($this->m_action))
      {
        $secMgr = &atkGetSecurityManager();
        $secMgr->logAction($this->m_type, $this->m_action);
        $this->callHandler($this->m_action);

        list($selector,$id) = explode("=", $this->m_postvars["atkselector"]);
        $page->register_hiddenvars(array("atknodetype"=>$this->m_module.".".$this->m_type, "atkselector"=>str_replace("'", "", $id)));
      }
      else
      {
        $page->addContent($this->genericPage(atktext('access_denied'), "<br><br>".atktext("error_node_action_access_denied", "", $this->m_type)."<br><br><br>"));
      }
    }

    /**
     * Render a generic page, with a box, title, stacktrace etc.
     * @param String $title The title of the page. Appears in the box title
     *                      and in the page header.
     * @param String $content The content to display in the box.
     * @return String A complete html page with the desired content.
     */
    function genericPage($title, $content)
    {
       $ui = &$this->getUi();
       $this->addStyle("style.css");
       return $this->renderActionPage($title, $ui->renderBox(array("title"=>$title,
                                                                   "content"=>$content)));
    }

    /**
     * Render a generic action.
     *
     * Renders actionpage.tpl for the desired action. This doesn't include a
     * box. The output can be rendered in a box with atkUi's renderBox method
     * if desired.
     * @param String $action The action for which the page is rendered.
     * @param mixed $blocks Pieces of html content to be rendered. Can be a
     *                      single string with content, or an array with
     *                      multiple content blocks.
     * @param String HTLM fragment for the action.
     * @param String $content The content to display in the box.
     * @return String A complete html page with the desired content.
     */
    function renderActionPage($action, $blocks=array())
    {
      if(!is_array($blocks))
      {
        $blocks = ($blocks==""?array():array($blocks));
      }

      $ui = &$this->getUi();

      // todo: overridable action templates
      return $ui->render("actionpage.tpl", array("blocks"=>$blocks,
                                                 "title"=>$ui->title($this->m_module, $this->m_type, $action)));
    }

    /**
     * Use this function to enable feedback for one or more actions.
     *
     * When feedback is enabled, the action does not immediately return to the
     * previous screen, but first displays a message to the user. (e.g. 'The
     * record has been saved').
     *
     * @param mixed $action The action for which feedback is enabled. You can
     *                      either pass one action or an array of actions.
     * @param int $statusmask The status(ses) for which feedback is enabled.
     *                        If for example this is set to ACTION_FAILED,
     *                        feedback is enabled only when the specified
     *                        action failed. It is possible to specify more
     *                        than one status by concatenating with '|'.
     */
    function setFeedback($action, $statusmask)
    {
      if (is_array($action))
      {
        for ($i=0, $_i=count($action); $i<$_i; $i++)
        {
          $this->m_feedback[$action[$i]] = $statusmask;
        }
      }
      else
      {
        $this->m_feedback[$action] = $statusmask;
      }
    }

    /**
     * Get the page instance of the page on which the node can render output.
     * @return atkPage The page instance.
     */
    function &getPage()
    {
      $page = &atkinstance("atk.ui.atkpage");
      return $page;
    }

    /**
     * Redirect the browser to a different location.
     *
     * This is usually used at the end of actions that have no output. An
     * example: when the user clicks 'save and close' in an edit screen, the
     * action 'save' is executed. If the save is succesful, this method is
     * called to redirect the user back to the adminpage.
     * When $config_debug is set to 2, redirects are paused and you can click
     * a link to execute the redirect (useful for debugging the action that
     * called the redirect).
     * Note: this method should be called before any output has been sent to
     * the browser, i.e. before any echo or before the call to
     * atkOutput::outputFlush().
     *
     * @param String $location The url to which you want to redirect the user.
     *                         If ommitted, the call automatically redirects
     *                         to the previous screen of the user. (one level
     *                         back on the session stack).
     * @param array $record If you pass a record here, the record is passed
     *                      as 'atkpkret' to the redirected url. Usually it's
     *                      not necessary to pass this parameter.
     */
    function redirect($location="", $record=array())
    {
      global $g_returnurl;

      atkdebug("atknode::redirect()");

      if ($g_returnurl!="") $location = $g_returnurl;

      if ($location=="")
      {
        $location = session_url($_SERVER["PHP_SELF"],SESSION_BACK,1);
      }

      if (count($record))
      {
        if (isset($this->m_postvars["atkpkret"]))
        {
          $location.="&".$this->m_postvars["atkpkret"]."=".rawurlencode($this->primaryKey($record));
        }
      }

      // The actual redirect.
      if (atkconfig("debug")>=2)
      {
        atkdebug('nondebug version would have redirected to <a href="'.$location.'">'.$location.'</a>');
      }
      else
      {
        atkdebug('redirecting to: '.$location);

        if (substr($location,-1)=="&")
        {
          $location=substr($location,0,-1);
        }
        if (substr($location,-1)=="?")
        {
          $location=substr($location,0,-1);
        }

        header('Location: '.$location);
      }
    }

    /**
     * Parse a set of url vars into a valid record structure.
     *
     * When attributes are posted in a formposting, the values may not be
     * valid yet. After posting, a call to updateRecord should be made to
     * translate the html values into the internal values that the attributes
     * work with.
     * @param array $vars The request variables that were posted from a form.
     * @return array A valid record.
     */
    function updateRecord($vars ="")
    {
      if ($vars=="") $vars = $this->m_postvars;
      $record = array();
      foreach (array_keys($this->m_attribList) as $attribname)
      {
        $p_attrib = &$this->m_attribList[$attribname];
        $record[$p_attrib->fieldName()]=$p_attrib->fetchValue($vars);
      }
      $record["atkprimkey"] = $vars["atkprimkey"];
      return $record;
    }

    /**
     * Update a record with variables from a form posting.
     *
     * Similar to updateRecord(), but here you can pass an existing record
     * (for example loaded from the db), and update it with the the variables
     * from the request. Instead of returning a record, the record you pass
     * is modified directly.
     *
     * @param array $record The record to update.
     * @param array $vars The request variables that were posted from a form.
     */
    function modifyRecord(&$record, $vars)
    {
      foreach (array_keys($this->m_attribList) as $attribname)
      {
        $p_attrib = &$this->m_attribList[$attribname];
        $record[$p_attrib->fieldName()]=$p_attrib->fetchValue($vars);
      }
    }

    /**
     * Returns the descriptor template for this node.
     * @return String The descriptor Template
     */
    function getDescriptorTemplate()
    {
      return $this->m_descTemplate;
    }

    /**
     * Sets the descriptor template for this node.
     * @param String $template The descriptor template.
     */
    function setDescriptorTemplate($template)
    {
      $this->m_descTemplate = $template;
    }

    /**
     * Retrieve the list of attributes that are used in the descriptor
     * definition.
     * @return array The names of the attributes forming the descriptor.
     */
    function descriptorFields()
    {
      $fields = array();

      // See if node has a custom descriptor definition.
      if ($this->m_descTemplate != NULL || method_exists($this,"descriptor_def"))
      {
        if ($this->m_descTemplate != NULL)
          $descriptordef = $this->m_descTemplate;
        else $descriptordef = $this->descriptor_def();

        // parse fields from descriptordef
        atkimport("atk.utils.atkstringparser");
        $parser = new atkStringParser($descriptordef);
        $fields = $parser->getFields();

        // There might be fields that have a '.' in them. These fields are
        // a concatenation of an attributename (probably a relation), and a subfield
        // (a field of the destination node).
        // The actual field is the one in front of the '.'.
        for ($i=0, $_i=count($fields);$i<$_i; $i++)
        {
          $elems = explode(".", $fields[$i]);
          if (count($elems)>1)
          {
            // dot found. attribute is the first item.
            $fields[$i] = $elems[0];

          }
        }
      }
      else
      {
        // default descriptor.. (default is first attribute of a node)
        $keys = array_keys($this->m_attribList);
        $fields[0]=$keys[0];
      }
      return $fields;
    }

    /**
     * Determine a descriptor of a record.
     *
     * The descriptor is a string that describes a record for the user. For
     * person records, this may be the firstname and the lastname, for
     * companies it may be the company name plus the city etc.
     * The descriptor is used when displaying records in a dropdown for
     * example, or in the title of editpages, delete confirmations etc.
     *
     * The descriptor method calls a method named descriptor_def() on the node
     * to retrieve a template for the descriptor (string with attributenames
     * between blockquotes, for example "[lastname], [firstname]".
     *
     * If the node has no descriptor_def() method, the first attribute of the
     * node is used as descriptor.
     *
     * Derived classes may override this method to implement custom descriptor
     * logic.
     *
     * @param array $rec The record for which the descriptor is returned.
     * @return String The descriptor for the record.
     */
    function descriptor($rec="")
    {
      // Descriptor template is set?
      if ($this->m_descTemplate != NULL)
      {
        atkimport("atk.utils.atkstringparser");
        $parser = new atkStringParser($this->m_descTemplate);
          return $parser->parse($rec);
      }
      // See if node has a custom descriptor definition.
      else if (method_exists($this,"descriptor_def"))
      {
        atkimport("atk.utils.atkstringparser");
        $parser = new atkStringParser($this->descriptor_def());
          return $parser->parse($rec);
      }
      else
      {
        // default descriptor.. (default is first attribute of a node)
        $keys = array_keys($this->m_attribList);
        return $rec[$keys[0]];
      }
    }

    /**
     * Validates a record.
     *
     * Validates unique fields, required fields, dataformat etc.
     *
     * @internal This method instantiates the node's validator object, and
     *           delegates validation to that object.
     *
     * @param array $record The record to validate
     * @param String mode The mode for which validation is performed ('add' or 'update')
     * @param array ignoreList The list of attributes that should not be
     *                         validated
     */
    function validate(&$record, $mode, $ignoreList=array())
    {
      $validateObj = &atknew($this->m_validate_class);

      $validateObj->setNode($this);
      $validateObj->setRecord($record);
      $validateObj->setIgnoreList($ignoreList);
      $validateObj->setMode($mode);

      return $validateObj->validate();
    }

    /**
     * Add a unique field set.
     *
     * When you add a set of attributes using this method, any combination of
     * values for the attributes should be unique. For example, if you pass
     * array("name", "parent_id"), name does not have to be unique, parent_id
     * does not have to be unique, but the combination should be unique.
     *
     * @param array $fieldArr The list of names of attributes that should be
     *                        unique in combination.
     */
    function addUniqueFieldset($fieldArr)
    {
      sort($fieldArr);
      if(!in_array($fieldArr, $this->m_uniqueFieldSets))
        $this->m_uniqueFieldSets[] = $fieldArr;
    }

    /**
     * Update a record in the database.
     *
     * The record should already exist in the database, or this method will
     * fail.
     * @param array $record The record to update in the database.
     * @param bool wether to execute the pre/post update triggers
     * @return boolean True if succesful, false if not.
     */
    function updateDb(&$record,$exectrigger=true)
    {
      $db = &$this->getDb();
      $query = &$db->createQuery();

      $query->addTable($this->m_table);

      // The record that must be updated is indicated by 'atkorgkey'
      // (not by atkselector, since the primary key might have
      // changed, so we use the atkorgkey, which is the value before
      // any update happened.)
      if ($record['atkprimkey']!="")
      {
        $pk = $record['atkprimkey'];

        // If we need to track changes, we first load the original values..
        if ($this->hasFlag(NF_TRACK_CHANGES))
        {
          //We need to add the NO_FILTER flag in case the new values would filter the record.
          //Need to restore the NO_FILTER bit back to its original value.
          $flags = $this->m_flags;
          $this->addFlag(NF_NO_FILTER);
          $recordset = $this->selectDb($pk,"","","","","edit"); // we need the same record we got when editing.
          $this->m_flags = $flags;
          $record["atkorgrec"] = $recordset[0];
        }
        $query->addCondition($pk);

        $storelist = array("pre"=>array(), "post"=>array(), "query"=>array());

        foreach (array_keys($this->m_attribList) as $attribname)
        {
          $p_attrib = &$this->m_attribList[$attribname];
          if ($p_attrib->needsUpdate($record))
          {
            $storemode = $p_attrib->storageType("update");
            if (hasFlag($storemode, PRESTORE)) $storelist["pre"][]=$attribname;
            if (hasFlag($storemode, POSTSTORE)) $storelist["post"][]=$attribname;
            if (hasFlag($storemode, ADDTOQUERY)) $storelist["query"][]=$attribname;
          }
        }

        if (!$this->_storeAttributes($storelist["pre"], $record, "update")) return false;

        for ($i = 0, $_i = count($storelist["query"]); $i < $_i; $i++)
        {
          $p_attrib = &$this->m_attribList[$storelist["query"][$i]];
          $p_attrib->addToQuery($query,$this->m_table,"",$record,1,"update"); // start at level 1
        }

        if($exectrigger) $this->executeTrigger("preUpdate",$record);

        if (count($query->m_fields) && !$query->executeUpdate()) return false;

        if($this->hasFlag(NF_ML)&&$record["atkmlsplit"]=="")
        {
          $record["atkmlsplit"]=1;

          $mltool = &atkinstance("atk.utils.atkmlsplitter");
          $mltool->updateMlRecords($this, $record,"update");
        }

        if (!$this->_storeAttributes($storelist["post"], $record, "update")) return false;

        // Now we call a postUpdate function, that can be used to do some processing after the record
        // has been saved.
        if($exectrigger)
          return $this->executeTrigger("postUpdate",$record);
        else
          return true;
      }
      else
      {
        atkdebug("NOT UPDATING! NO SELECTOR SET!");
      }
      return true;
    }

    /**
     * Call the store() method on a list of attributes.
     * @access private
     * @param array $storelist The list of attributes for which the
     *                         store() method should be called.
     * @param array $record The master record being stored.
     * @param String $mode The storage mode ("add", "copy" or "update")
     * @return boolean True if succesful, false if not.
     */
    function _storeAttributes($storelist, &$record, $mode)
    {
      // store special storage attributes.
      for ($i = 0, $_i = count($storelist); $i < $_i; $i++)
      {
        $p_attrib = &$this->m_attribList[$storelist[$i]];
        if (!$p_attrib->store($this->getDb(), $record, $mode))
        {
          // something went wrong.
          atkdebug("Store aborted. Attribute '".$storelist[$i]."' reported an error.");
          return false;
        }
      }
      return true;
    }

    /**
    * Checks if the attribute is needed in a select query
    *
    * @param string attribname name of the attribute
    * @param array list of exclueded attribs
    * @param array list of inclueded attribs
    * @return bool true in case the attribute needs to be in the select
    */
    function isAllowedQueryList($attribname,$excludeList="",$includeList="")
    {
      return (
              ((is_array($includeList) && in_array($attribname,$includeList))
              || (is_array($excludeList) && !in_array($attribname,$excludeList)))
              || (!is_array($excludeList) && !is_array($includeList))
              || ($this->m_attribList[$attribname]->hasFlag(AF_FORCE_LOAD))
              );
    }

    /**
     * Count the number of records in the database that a call to selectDb
     * would return.
     *
     * @param String $selector Sql expression used as a where-condition, to
     *                         count only records that match the expression.
     * @param array $excludeList List of attributes to be excluded from the
     *                           query. By default, the attributes that should
     *                           be loaded are determined by the $mode
     *                           parameter. Using $excludeList, you can
     *                           explicitly exclude attributes.
     * @param array $includeList List of attributes to include in the query,
     *                           regardless of the $mode parameter.
     * @param String $mode The action for which the selectDb is called.
     *                     This param is used to determine which attributes
     *                     to include in the query and which not to include.
     * @return int Number of records
     */
    function countDb($selector="", $excludeList="", $includeList="", $mode="")
    {
      $searching = false;
      $loadlist = array();
      $mltool = &atkinstance("atk.utils.atkmlsplitter");
      $db = &$this->getDb();
      $query = &$db->createQuery();

      $query->addTable($this->m_table);
      $query->addCondition($selector);
      if (isset($this->m_postvars['atksearchmethod']))
      {
        $query->m_searchmethod = $this->m_postvars['atksearchmethod'];
      }
      
      // First we check if we are about to search and set a few things
      // if we are
      if (!atkReadOptimizer())
      {
        /* there may be search criteria, which we also filter */
        $searchArray = isset($this->m_postvars['atksearch']) ? $this->m_postvars['atksearch'] : null;
        $smartSearchCriteria = isset($this->m_postvars['atksmartsearch']) ? $this->m_postvars['atksmartsearch'] : null;
        if (is_array($searchArray) && count($searchArray)>0 || is_array($smartSearchCriteria) && count($smartSearchCriteria) > 0)
        {
          $searching = true;
          //The searchmode of an index should be used only once, therefore it uses atksinglesearchmode instead of atksearchmode.
          $searchmode = isset($this->m_postvars["atksinglesearchmode"]) ? $this->m_postvars["atksinglesearchmode"] : (isset($this->m_postvars["atksearchmode"]) ? $this->m_postvars["atksearchmode"] : "");

          if ($searchmode=="") $searchmode = atkconfig("search_defaultmode");
        }
      }      

      if(array_key_exists("atkfilter", $this->m_postvars))
       $query->addCondition($this->validateFilter($this->m_postvars["atkfilter"]));

      if (!$this->hasFlag(NF_NO_FILTER))
      {
        /* hard filters may be set */
        foreach ($this->m_filters as $key => $value)
        {
          $query->addCondition($key."='".$value."'");
        }

        /* fuzzy filters may be set */
        for ($i = 0, $_i = count($this->m_fuzzyFilters); $i < $_i; $i++)
        {
          $query->addCondition($this->m_fuzzyFilters[$i]);
        }
      }

      foreach (array_keys($this->m_attribList) as $attribname)
      {
        $p_attrib = &$this->m_attribList[$attribname];
        if (is_object($p_attrib) && $this->isAllowedQueryList($attribname,$excludeList,$includeList))
        {
          $loadmode = $p_attrib->loadType($mode);
          if (hasFlag($loadmode, ADDTOQUERY)) $loadlist["query"][]=$attribname;
        }
      }

      for ($i = 0, $_i = count($loadlist["query"]); $i < $_i; $i++)
      {
        $p_attrib = &$this->m_attribList[$loadlist["query"][$i]];
        $p_attrib->addToQuery($query,$this->m_table,"","",1,$mode); // start at level 1
      }

      // there may be search criteria, which we also filter
      if (is_array($searchArray) && count($searchArray)>0)
      {
        while (list($key,$value) = each($searchArray))
        {
          if( (($value!="") && ($mode=="admin")) || (($value!="") && in_array($key,$loadlist["query"])) )
          {
            $p_attrib = &$this->getAttribute($key);

            if (is_object($p_attrib))
            {
              if (is_array($value) && isset($value[$key]))
              {
                $value = $value[$key];
              }

              if(is_array($searchmode))
              {
                $search = $searchmode[$key];
                if ($search=="") $search = atkconfig("search_defaultmode");
                $p_attrib->searchCondition($query, $this->m_table, $value, $search);
              }
              else
              {
                $p_attrib->searchCondition($query, $this->m_table, $value, $searchmode);
              }
            }
            else
            {
              $condition = "lower(".$this->m_table.".".$key.") LIKE lower('%".escapeSQL($value,true)."%')";
              $query->addSearchCondition($condition);
            }
          }
        }
      }
      
      // there may be smart search criteria which we need to apply
      if (is_array($smartSearchCriteria) && count($smartSearchCriteria) > 0)
      {
        $this->applySmartSearchCriteria($query, $smartSearchCriteria);
      }     

      $mltool->addMlCondition($query,$this,$mode,$excludeList,$includeList);

      $querystring = $query->buildCount();
      $db = &$this->getDb();
      $result = $db->getrows($querystring);

      // group by fix:
      // if there exists a group by (added by atk/filters) the 'else' situation occurs
      if(count($result)==1)
        return $result[0]["count"];
      else
        return count($result);
    }

    /**
     * Copy a record in the database.
     *
     * Primarykeys are automatically regenerated for the copied record. Any
     * detail records (onetomanyrelation) are copied too. Refered records
     * manytoonerelation) are not copied.
     *
     * @param array $record The record to copy.
     * @return boolean True if succesful, false if not.
     */
    function copyDb(&$record, $mode="copy")
    {
      // remove primarykey (copied record will get a new primary key)
      unset($record["atkprimkey"]);

      // remove trigger has been executed references
      foreach (array_keys($record) as $key)
      {
        if (preg_match('/^__executed.*$/', $key))
          unset($record[$key]);
      }      
      
      $this->preCopy($record);
      return $this->addDb($record, true, $mode);
    }

    /**
     * Retrieve records from the database.
     *
     * Note that if 'atksearch' is set in the request vars, the search
     * expressions are automatically added as extra where-clauses.
     *
     * @todo Handling atksearch in this method is dirty, this should be
     *       controlable with parameters.
     *
     * @param String $selector Sql expression used as a where-condition, to
     *                         retrieve only records that match the
     *                         expression.
     * @param String $order The order in which to retrieve the records.
     * @param array $limit Array containing an "offset" and a "limit" to
     *                     retrieve only part of the resultset. (Pass NULL or
     *                     an empty string to retrieve all records.)
     * @param array $excludeList List of attributes to be excluded from the
     *                           query. By default, the attributes that should
     *                           be loaded are determined by the $mode
     *                           parameter. Using $excludeList, you can
     *                           explicitly exclude attributes.
     * @param array $includeList List of attributes to include in the query,
     *                           regardless of the $mode parameter.
     * @param String $mode The action for which the selectDb is called.
     *                     This param is used to determine which attributes
     *                     to include in the query and which not to include.
     * @return array Array containing the retrieved records
     */
    function selectDb($selector="", $order="", $limit="", $excludeList="",$includeList="", $mode="")
    {
      $searching=false;
      $selectlist = array();
      $loadlist = array();

      if($order=="" && $this->m_default_order!="") $order=$this->m_default_order;

      $db = &$this->getDb();
      $query = &$db->createQuery();

      $mltool = &atkinstance("atk.utils.atkmlsplitter");

      $query->addTable($this->m_table);
      $query->addCondition($selector);

      $query->m_searchmethod = isset($this->m_postvars['atksearchmethod']) ? $this->m_postvars['atksearchmethod'] : null;

      // First we check if we are about to search and set a few things
      // if we are
      if (!atkReadOptimizer())
      {
        /* there may be search criteria, which we also filter */
        $searchArray = isset($this->m_postvars['atksearch']) ? $this->m_postvars['atksearch'] : null;
        $smartSearchCriteria = isset($this->m_postvars['atksmartsearch']) ? $this->m_postvars['atksmartsearch'] : null;
        if (is_array($searchArray) && count($searchArray)>0 || is_array($smartSearchCriteria) && count($smartSearchCriteria) > 0)
        {
          $searching = true;
          //The searchmode of an index should be used only once, therefore it uses atksinglesearchmode instead of atksearchmode.
          $searchmode = isset($this->m_postvars["atksinglesearchmode"]) ? $this->m_postvars["atksinglesearchmode"] : (isset($this->m_postvars["atksearchmode"]) ? $this->m_postvars["atksearchmode"] : "");

          if ($searchmode=="") $searchmode = atkconfig("search_defaultmode");
        }
      }

      if (!$this->hasFlag(NF_NO_FILTER))
      {
        /* hard filters may be set */
        foreach($this->m_filters as $key => $value)
        {
          $query->addCondition($key."='".$value."'");
        }

        /* fuzzy filters may be set */
        for ($i = 0, $_i = count($this->m_fuzzyFilters); $i < $_i; $i++)
        {
          $query->addCondition($this->m_fuzzyFilters[$i]);
        }
      }

      $preloadlist = array();
      $loadlist = array();
      $postloadlist = array();
      $cpylist = array();
      $querylist = array();
      foreach (array_keys($this->m_attribList) as $attribname)
      {
        $p_attrib = &$this->m_attribList[$attribname];
        if (is_object($p_attrib) && $this->isAllowedQueryList($attribname,$excludeList,$includeList))
        {
          $loadmode = $p_attrib->loadType($mode, $searching);
          if (hasFlag($loadmode, POSTLOAD))
          {
            $postloadlist[]=$attribname;
          }
          if (hasFlag($loadmode, PRELOAD))
          {
            $preloadlist[]=$attribname;
          }
          if (hasFlag($loadmode, ADDTOQUERY))
          {
            $querylist[]=$attribname;
          }
        }

        // ML hack
        if(is_subclass_of($p_attrib,"atkrelation") && $p_attrib->createDestination() && $p_attrib->m_destInstance->hasFlag(NF_ML))
        {
          $cpylist[]=$attribname;
        }
      }


      $recordset = array();
      // First load PRELOAD attribs
      for ($j = 0, $_j = count($preloadlist); $j < $_j; $j++)
      {
        $p_attrib = &$this->m_attribList[$preloadlist[$j]];

        $preloadedrecs = $p_attrib->load($this->getDb(), $recordset[$i], $mode);

        // todo, dirty hack, if preload returns x records, we treat them as 1 record now.
        $recordset[0][$preloadlist[$j]] = $preloadedrecs;
      }

      // Add attributes to query
      for ($i=0, $_i=count($querylist); $i<$_i; $i++)
      {
        $p_attrib = &$this->m_attribList[$querylist[$i]];
        $p_attrib->addToQuery($query,$this->m_table,"",(isset($recordset[0])?$recordset[0]:array()),1,$mode); // start at level 1
      }

      if (!atkReadOptimizer())
      {
        // there may be search criteria, which we also filter
        if (is_array($searchArray) && count($searchArray)>0)
        {
          while (list($key,$value) = each($searchArray))
          {
            if( (($value!="") && ($mode=="admin")) || (($value!="") && in_array($key,$querylist)) )
            {
              $p_attrib = &$this->getAttribute($key);

              if (is_object($p_attrib))
              {
                if (is_array($value) && isset($value[$key]))
                {
                  $value = $value[$key];
                }

                if(is_array($searchmode))
                {
                  $search = $searchmode[$key];
                  if ($search=="") $search = atkconfig("search_defaultmode");
                  $p_attrib->searchCondition($query, $this->m_table, $value, $search);
                }
                else
                {
                  $p_attrib->searchCondition($query, $this->m_table, $value, $searchmode);
                }
              }
              else
              {
                $condition = "lower(".$this->m_table.".".$key.") LIKE lower('%".escapeSQL($value,true)."%')";
                $query->addSearchCondition($condition);
              }
            }
          }
        }
        
        // there may be smart search criteria which we need to apply
        if (is_array($smartSearchCriteria) && count($smartSearchCriteria) > 0)
        {
          $this->applySmartSearchCriteria($query, $smartSearchCriteria);
        }
      }

      $mltool->addMlCondition($query,$this,$mode,$excludeList,$includeList);

      if ($order!="") $query->addOrderBy($order);

      if (is_array($limit))
      {
        if (count($limit) == 2)
        {
          $query->setLimit($limit["offset"], $limit["limit"]);
        }
      }
      else if (!empty($limit))
      {
        $query->setLimit(0, $limit);
      }

      $querystring = $query->buildSelect();

      $db = &$this->getDb();
      $dbrecords = $db->getrows($querystring);

      if($mode=="copy" && $this->hasFlag(NF_ML)) // do we have mlrecords?
      {
        // When copying there could be more than one multilanguage record in the dbrecords array.
        // So we split this before sending it to the multilanguage splitter.
        // This only happens in case of an atkonetomanyrelation.
        $result = array();
        $ids = array();
        $indexes =array();
        for ($i=0;$i<count($dbrecords);$i++)
        {
          if (!in_array($dbrecords[$i][$this->primaryKeyField()], $ids))
          {
            $j = count($result);
            $result[$j][] = $dbrecords[$i];
            $ids[] = $dbrecords[$i][$this->primaryKeyField()];
            $indexes[] = $j;
          }
          else
          {
            $index = array_search($dbrecords[$i][$this->primaryKeyField()], $ids);
            $result[$indexes[$index]][] = $dbrecords[$i];
          }
        }
        // Combine the multilanguage records and put them in the dbrecords array.
        $dbrecords = array();
        for ($i=0;$i<count($result);$i++)
        {
          $mltool->combineMlRecordSet($this, $result[$i],$query);
          $dbrecords[$i] = $result[$i][0];
        }
      }
      if($mode=="edit" && $this->hasFlag(NF_ML)) // do we have mlrecords?
      {
        $mltool->combineMlRecordSet($this, $dbrecords,$query);
      }

      $recordset = array();
      for ($i = 0, $_i = count($dbrecords); $i < $_i; $i++)
      {
        $therecord = array();
        $query->deAlias($dbrecords[$i]); // dereference aliases..
        atkDataDecode($dbrecords[$i]);
        $recordcopy = $dbrecords[$i];
        foreach (array_keys($this->m_attribList) as $attribname)
        {
          $p_attrib = &$this->m_attribList[$attribname];
          if(in_array($attribname, $cpylist))
          {
            $therecord[$attribname] = $recordcopy[$attribname];
          }
          else if ($p_attrib)
          {
            $therecord[$attribname] = $p_attrib->db2value($dbrecords[$i]);
          }
          else
          {
            atkerror("Attribute with name: $attribname and node: ".$this->m_type." is empty in attributelist or does not support db2value method");
          }
        }

        // We now have a semi-complete record. We can now calculate it's primarykey, assuming
        // that attributes with load() functions can never be part of the primary key.
        // They can't be, because load() might *depend* on a primary key.
        // so the primary key must always be known before we start loading
        // attributes with load() functions.
        $therecord["atkprimkey"] = $this->primaryKey($therecord);

        $recordset[] = $therecord;
      }

      // also load special storage attributes. (POST)
      for ($i = 0, $_i = count($recordset); $i < $_i; $i++)
      {
        for ($j = 0, $_j = count($postloadlist); $j < $_j; $j++)
        {
          $p_attrib = &$this->m_attribList[$postloadlist[$j]];

          $recordset[$i][$postloadlist[$j]] = $p_attrib->load($this->getDb(), $recordset[$i], $mode);
        }
      }
      return $recordset;
    }
    
    /**
     * Apply the given smart search criteria to the
     * given query object.
     * 
     * @param atkQuery $query    The query object.
     * @param Array    $criteria The smart search criteria.
     */
    function applySmartSearchCriteria(&$query, $criteria)
    {
      foreach ($criteria as $id => $criterium)
      {
        $path = $criterium['attrs'];
        $value = $criterium['value'];
        $mode = $criterium['mode'];
        
        $attrName = array_shift($path);
        $attr = &$this->getAttribute($attrName);
        
        if (is_object($attr))
        {
          $attr->smartSearchCondition($id, 0, $path, $query, $this->m_table, $value, $mode);
        }
      }
    }

    /**
     * Add this node to an existing query.
     *
     * Framework method, it should not be necessary to call this method
     * directly.
     * This method is used when adding the entire node to an existing
     * query, as part of a join.
     * @todo The allfields parameter is too inflexible.
     * @param atkQuery $query The query statement
     * @param String $alias The aliasprefix to use for fields from this node
     * @param int $level The recursion level.
     * @param boolean $allfields If set to true, all fields from the node are
     *                           added to the query. If set to false, only
     *                           the primary key and fields from the desriptor
     *                           are added.
     */
    function addToQuery(&$query, $alias="", $level=0, $allfields=false)
    {
      $usefieldalias = false;

      if ($alias=="")
      {
        $alias = $this->m_table;
      }
      else
      {
        $usefieldalias = true;
      }

      // If allfields is set, we load the entire record.. otherwise, we only
      // load the important fields (descriptor and primary key fields)
      // this is mainly used by onetoonerelation.
      if ($allfields)
      {
        $usedFields = array_keys($this->m_attribList);
      }
      else
      {
        $usedFields = atk_array_merge($this->descriptorFields(),$this->m_primaryKey);
        foreach (array_keys($this->m_attribList) as $name)
          if (is_object($this->m_attribList[$name]) && $this->m_attribList[$name]->hasFlag(AF_FORCE_LOAD)) $usedFields[] = $name;
        $usedFields = array_unique($usedFields);
      }

      foreach ($usedFields as $usedfield)
      {
        list($attribname) = explode(".", $usedfield);
        $p_attrib = &$this->m_attribList[$attribname];
        if (is_object($p_attrib))
        {
          $loadmode = $p_attrib->loadType("");

          if ($loadmode && hasFlag($loadmode, ADDTOQUERY))
          {
            if ($usefieldalias) $fieldaliasprefix = $alias."_AE_";

            $p_attrib->addToQuery($query,$alias, $fieldaliasprefix,"",$level+1, "select");
          }
        }
        else atkdebug("$attribname is not an object?! Check your descriptor_def for non-existant fields");
      }
    }

    /**
     * Save a new record to the database.
     *
     * The record is passed by reference, because any autoincrement field gets
     * its value when stored to the database. The record is updated, so after
     * the call to addDb you can use access the primary key fields.
     *
     * @param array $record The record to save.
     * @param boolean $exectrigger Indicates whether the postAdd trigger
     *                             should be fired.
     * @param array $excludelist List of attributenames that should be ignored
     *                           and not stored in the database.
     * @return boolean True if succesful, false if not.
     */
    function addDb(&$record, $exectrigger=true, $mode="add", $excludelist=array())
    {
      if ($exectrigger)
       if(!$this->executeTrigger("preAdd",$record,$mode)) return atkerror("preAdd() failed!");

      $db = &$this->getDb();
      $query = &$db->createQuery();

      $storelist = array("pre"=>array(), "post"=>array(), "query"=>array());

      $query->addTable($this->m_table);

      foreach (array_keys($this->m_attribList) as $attribname)
      {
        $p_attrib = &$this->m_attribList[$attribname];
        if (!atk_in_array($attribname, $excludelist) && ($mode != "add" || $p_attrib->needsInsert($record)))
        {
          $storemode = $p_attrib->storageType($mode);
          if (hasFlag($storemode, PRESTORE)) $storelist["pre"][]=$attribname;
          if (hasFlag($storemode, POSTSTORE)) $storelist["post"][]=$attribname;
          if (hasFlag($storemode, ADDTOQUERY)) $storelist["query"][]=$attribname;
        }
      }

      if (!$this->_storeAttributes($storelist["pre"], $record, $mode)) return false;

      for ($i = 0, $_i = count($storelist["query"]); $i < $_i; $i++)
      {
        $p_attrib = &$this->m_attribList[$storelist["query"][$i]];
        if ($p_attrib->hasFlag(AF_AUTO_INCREMENT))
        {
          $record[$p_attrib->fieldName()]=$db->nextid($this->m_seq);
        }

        $p_attrib->addToQuery($query,$this->m_table,"",$record,1,"add"); // start at level 1
      }

      if (!$query->executeInsert())
      {
        atkdebug("executeInsert failed..");
        return false;
      }

      // new primary key
      $record["atkprimkey"] = $this->primaryKey($record);

      if($this->hasFlag(NF_ML)&&$record["atkmlsplit"]=="")
      {
        $record["atkmlsplit"]=1;
        $mltool = &atkinstance("atk.utils.atkmlsplitter");
        $mltool->updateMlRecords($this, $record,"add");
      }


      if (!$this->_storeAttributes($storelist["post"], $record, $mode))
      {
        atkdebug("_storeAttributes failed..");
        return false;
      }

      // Now we call a postAdd function, that can be used to do some processing after the record
      // has been saved.
      if ($exectrigger && !$this->executeTrigger("postAdd",$record,$mode)) return false;

      return true;
    }

    /**
     * Executes a trigger on a add,update or delete action
     *
     * To prevent triggers from executing twice, the method stores an
     * indication in the record when a trigger is executed.
     * ('__executed<triggername>')
     *
     * @param string $trigger function, such as 'postUpdate'
     * @param array $record record on which action is performed
     * @param string $mode mode like add or update
     * @return bool true on case of success or when the trigger isn't returning anything (assumes success)
     */
    function executeTrigger($trigger,&$record,$mode=null)
    {
      if (!isset($record["__executed".$trigger]))
      {
        $return = $this->$trigger($record, $mode);

        $record["__executed".$trigger]=true;

        if(!isset($return))
        {
          atkdebug("Undefined return:".$this->atkNodeType().".$trigger doesn't return anything, it should return a boolean!", DEBUG_WARNING);
          $return = true;
        }
        if(!$return) atkdebug("$trigger failed!");
        return $return;
      }
      return true;
    }

    /**
     * Delete record(s) from the database.
     *
     * After deletion, the postDel() trigger in the node method is called, and
     * on any attribute that has the AF_CASCADE_DELETE flag set, the delete()
     * method is invoked.
     *
     * @todo There's a discrepancy between updateDb, addDb and deleteDb:
     *       There should be a deleteDb which accepts a record, instead
     *       of a selector.
     * @param String $selector SQL expression used as where-clause that
     *                         indicates which records to delete.
     * @param bool $exectrigger wether to execute the pre/post triggers
     * @returns boolean True if successful, false if not.
     */
    function deleteDb($selector,$exectrigger=true)
    {
      $recordset = $this->selectDb($selector,"","","","","delete");

      if($exectrigger)
      {
        for ($i = 0, $_i = count($recordset); $i < $_i; $i++)
  	    {
  	      $return = $this->executeTrigger("preDelete",$recordset[$i]);
  	      if (!$return) return false;
  	    }
      }

      if (count($this->m_cascadingAttribs)>0)
      {
        for ($i = 0, $_i = count($recordset); $i < $_i; $i++)
        {
          for ($j = 0, $_j = count($this->m_cascadingAttribs); $j < $_j; $j++)
          {
            $p_attrib = &$this->m_attribList[$this->m_cascadingAttribs[$j]];
            if (is_array($recordset[$i][$this->m_cascadingAttribs[$j]]) && $recordset[$i][$this->m_cascadingAttribs[$j]] !== NULL && !$p_attrib->isEmpty($recordset[$i]))
            {
              if (!$p_attrib->delete($recordset[$i]))
              {
                // error
                return false;
              }
            }
          }
        }
      }

      $db = &$this->getDb();
      if ($db->query("DELETE FROM ".$this->m_table." WHERE ".$selector))
      {
        if($exectrigger)
        {
          for ($i = 0, $_i = count($recordset); $i < $_i; $i++)
          {
            $return = ($this->executeTrigger("postDel",$recordset[$i]) && $this->executeTrigger("postDelete",$recordset[$i]));
            if (!$return) return false;
          }
        }
        return true;
      }
      else
      {
        return false;
      }
    }

    /**
     * Function that is called by the framework, right after a new record has
     * been saved to the database.
     *
     * This function does essentially nothing, but it can be overriden in
     * derived classes if you want to do something special after you saved a
     * record.
     *
     * @param array $record The record that has just been saved.
     * @param String $mode The 'mode' indicates whether the added record was a
     *                     completely new record ("add") or a copy ("copy").
     * @return boolean True if succesful, false if not.
     */
    function postAdd($record, $mode="add")
    {
      // Do nothing
      return true;
    }

    /**
     * Function that is called by the framework, just before a new record will
     * be saved to the database.
     *
     * This function does essentially nothing, but it can be overriden in
     * derived classes if you want to modify the record just before it will
     * be saved.
     *
     * @param array $record The record that will be saved to the database.
     */
    function preAdd(&$record)
    {
      // Do nothing
      return true;
    }

    /**
     * Function that is called by the framework, right after an existing
     * record has been updated in the database.
     *
     * This function does essentially nothing, but it can be overriden in
     * derived classes if you want to do something special after the record is
     * updated.
     *
     * If the NF_TRACK_CHANGES flag is present for the node, both the new
     * and the original record are passed to this method. The original
     * record is stored in the new record, in $record["atkorgrec"].
     *
     * @param array $record The record that has just been updated in the
     *                      database.
     * @return boolean True if succesful, false if not.
     */
    function postUpdate($record)
    {
      // Do nothing
      return true;
    }

    /**
     * Function that is called by the framework, just before an existing
     * record will be saved to the database.
     *
     * This function does essentially nothing, but it can be overriden in
     * derived classes if you want to modify the record just before it will
     * be saved.
     *
     * @param array $record The record that will be updated in the database.
     * @return bool Wether or not we succeeded in what we wanted to do.
     */
    function preUpdate(&$record)
    {
      // Do nothing
      return true;
    }

    /**
     * Function that is called by the framework, right before a record will be
     * deleted. Should this method return false the deleting will halt.
     *
     * This function does essentially nothing, but it can be overriden in
     * derived classes if you want to do something special after a record is
     * deleted.
     *
     * If this function returns false the delete action will not continue.
     *
     * @param array $record The record that will be deleted.
     */
    function preDelete($record)
    {
      return true;
    }

    /**
     * Deprecated function that is called by the framework,
     * right after a record has been deleted.
     * Please use postDelete() instead.
     * @param array $record The record that has just been deleted.
     * @return bool Wether or not we succeeded in what we wanted to do.
     */
    function postDel($record)
    {
      // Do nothing
      return true;
    }

    /**
     * Function that is called by the framework, right after a record has been
     * deleted.
     *
     * This function does essentially nothing, but it can be overriden in
     * derived classes if you want to do something special after a record is
     * deleted.
     *
     * @param array $record The record that has just been deleted.
     * @return bool Wether or not we succeeded in what we wanted to do.
     */
    function postDelete($record)
    {
      // Do nothing
      return true;
    }

    /**
     * Function that is called by the framework, right before a copied record
     * is stored to the database.
     *
     * This function does nothing, but it can be overriden in derived classes
     * if you want to do some processing on a record before it is
     * being copied.
     * Typical usage would be: Suppose you have a field named 'title' in a
     * record. In the preCopy method, you could change the title field of the
     * record to 'Copy of ..', so the user can distinguish between the
     * original and the copy.
     *
     * @param &$record A reference to the copied record. You can change the
     *                 contents of the record, since it is passed by
     *                 reference.
     */
    function preCopy(&$record)
    {
    }

    /**
     * Function that is called for each record in a recordlist, to determine
     * what actions may be performed on the record.
     *
     * This function does nothing, but it can be overriden in derived classes,
     * to make custom actions for certain records.
     * The array with actions (edit, delete, etc.) is passed to the function
     * and can be modified.
     * To create a new action, just do $actions["new_action"]=$url;
     * in the derived function.
     * To disable existing actions, for example the edit action, for a record,
     * use: unset($actions["edit"]);
     *
     * @param array $record The record for which the actions need to be
     *                      determined.
     * @param &$actions Reference to an array with the already defined
     *                  actions. This is an associative array with the action
     *                  identifier as key, and an url as value. Actions can be
     *                  removed from it, or added to the array.
     * @param &$mraactions List of multirecordactions that are supported for
     *                     the passed record.
     */
    function recordActions($record, &$actions, &$mraactions)
    {
      // Do nothing.
    }

    /**
     * Retrieve the security key of an action.
     *
     * Returns the privilege required to perform a certain action.
     * Usually, the privilege and the action are equal, but in m_securityMap,
     * aliasses may be defined.
     * @param String $action The action for which you want to determine the
     *                       privilege.
     * @return String The security privilege required to perform the action.
     */
    function securityKey($action)
    {
      if (!isset($this->m_securityMap[$action])) return $action;
      return $this->m_securityMap[$action];
    }

    /**
     * Get the full atknodetype of this node (module.nodetype notation).
     * @return String The atknodetype of the node.
     */
    function atknodetype()
    {
      return (empty($this->m_module) ? "" : $this->m_module.".").$this->m_type;
    }

    /**
     * This function determines if the user has the privilege to perform a certain
     * action on the node.
     *
     * @param String $action The action to be checked.
     * @param array $record The record on which the action is to be performed.
     *                      The standard implementation ignores this
     *                      parameter, but derived classes may override this
     *                      method to implement their own record based
     *                      security policy. Keep in mind that a record is not
     *                      passed in every occasion. The method is called
     *                      several times without a record, to just see if
     *                      the user has the privilege for the action
     *                      regardless of the record being processed.
     *
     * @return boolean True if the action may be performed, false if not.
     */
    function allowed($action, $record="")
    {
      $secMgr = &atkGetSecurityManager();
      if (empty($this->m_securityAlias))
      {
        $alias = $this->atknodetype();
      }
      else
      {
        $alias = $this->m_securityAlias;
      }

      // Resolve action
      $action = $this->securityKey($action);

      // If action contains a dot, it's a complete nodename.action alias.
      // Else, it's only an action alias, and we use the default node.
      // (TODO: make it possible to use modulename.nodename.action aliases)
      if (strpos($action, ".")!==false)
      {
        list($node, $action) = explode(".", $action);
        $alias = $this->m_module.".".$node;
      }

      return ($this->hasFlag(NF_NO_SECURITY)
              ||in_array($action, $this->m_unsecuredActions)
              || $secMgr->allowed($alias,$action)
              || (isset($this->m_securityImplied[$action]) && $secMgr->allowed($alias, $this->m_securityImplied[$action])));
    }

    /**
     * Set the security alias of a node.
     *
     * By default a node has it's own set of privileges. With this method,
     * the privileges of another node can be used. This is useful when you
     * have a master/detail relationship, and people may manipulate details
     * when they have privileges on the master node.
     * Note: When setting an alias for the node, the node no longer has to
     * have a registerNode call in the getNodes method in module.inc.
     *
     * @param String $alias The node (module.nodename) to set as a security
     *                      alias for this node.
     */
    function setSecurityAlias($alias)
    {
      $this->m_securityAlias = $alias;
    }

    /**
     * Disable privilege checking for an action.
     *
     * This method disables privilege checks for the specified action, for the
     * duration of the current http request.
     * @param String $action The name of the action for which security is
     *                       disabled.
     */
    function addAllowedAction($action)
    {
      if (is_array($action))
      {
        $this->m_unsecuredActions = atk_array_merge($this->m_unsecuredActions,$action);
      }
      else
      {
        $this->m_unsecuredActions[] = $action;
      }
    }

    /**
     * Display a statusbar with a stacktrace and a help button.
     * @deprecated Use the {statusbar} tag in templates instead.
     * @param boolean $locked is the currently displayed item locked?
     */
    function statusbar($locked=FALSE)
    {
      atkdebug("Obsolete use of statusbar()");
      if (!$this->m_statusbarDone)
      {
        global $g_sessionManager;
        $ui = &$this->getUi();

        $params = array();

        $this->m_statusbarDone = true;
        if (atkconfig("stacktrace"))
        {
          $params["stacktrace"] = $g_sessionManager->stackTrace();
        }

        $help = $this->getHelp();
        $params = array_merge($params, $help);

        $params["lockstatus"] = $this->getLockStatusIcon($locked);

        return $ui->render("statusbar.tpl", $params);
      }
      return "";
    }

    /**
     * Retrieve help link for the current node.
     * @return String Complete html link, linking to the help popup.
     */
    function getHelp()
    {
      $res = array();
      $res["helpurl"] = $this->helpUrl();
      if ($res["helpurl"]!="")
      {
        $page = &$this->getPage();
        $page->register_script(atkconfig("atkroot")."atk/javascript/newwindow.js");
        $res["helplabel"] = atktext("help");
        $res["helplink"] = '<a href="'.$res["helpurl"].'">'.$res["helplabel"].'</a>';
      }
      return $res;
    }

    /**
     * Get img tag for lock icon.
     * @param boolean $lockstatus True if the record is locked, false if not.
     * @return String HTML image tag with the correct lock icon.
     */
    function getLockStatusIcon($lockstatus)
    {
      if ($lockstatus)
      {
        return '<img src="'.atkconfig("atkroot").'atk/images/lock.gif" name="_lock_">';
      }
      return "";
    }

    /**
     * Get the help url for this node.
     *
     * Retrieves the url of the help popup, if there is help available for
     * this node.
     * @return String The help url, or an empty string if help is not
     *                available.
     */
    function helpUrl()
    {
      $language = atkconfig("language");
      $file   = "help/".$language."/help.".$this->m_type.".inc";
      if (!file_exists($file))
      {
        // no help available..
        return "";
      }
      else
      {
       $name = atktext("help");
       $node = $this->m_type;
       return atkPopup('atk/popups/help.inc','node='.$node,$name,650,650,'yes','no');
      }
    }

    /**
     * Invoke the handler for an action.
     *
     * If there is a known registered external handler method for the
     * specified action, this method will call it. If there is no custom
     * external handler, the atkActionHandler object is determined and the
     * actionis invoked on the actionhandler.
     * @param String $action the node action
     */
    function callHandler($action)
    {
      $handler = &getNodeHandler($this->m_type, $action);

      // handler function
      if ($handler != NULL && function_exists($handler))
      {
        atkdebug("Calling external handler function for '".$action."'");
        $handler($this, $action);
      }

      // no (valid) handler
      else
      {
        $this->m_handler = &$this->getHandler($action);
        $this->m_handler->handle($this, $action, $this->m_postvars);
      }
    }

    /**
     * Get the atkActionHandler object for a certain action.
     *
     * The default implementation returns a default handler for the action,
     * but derived classes may override this to return a custom handler.
     * @param String $action The action for which the handler is retrieved.
     * @return atkActionHandler The action handler.
     */
    function &getHandler($action)
    {
      atkimport("atk.handlers.atkactionhandler");
      $handler = &atkActionHandler::getDefaultHandler($action);
      $handler->setNode($this);
      return $handler;
    }

    /**
     * Sets the search action.
     *
     * The search action is the action that will be performed
     * if only a single record is found after doing a certain search query.
     *
     * You can specify more then 1 action. If the user isn't allowed to
     * execute the 1st action, the 2nd action will be used, etc. If you
     * want to pass multiple actions, just pass multiple params (function
     * has a variable number of arguments).
     * @todo Using func_get_args is non-standard. It's cleaner to accept an
     *       array.
     * @param String $action The name of the action.
     */
    function setSearchAction()
    {
      $this->m_search_action = func_get_args();
    }

    /**
     * This function resorts the attribIndexList.
     *
     * This is necessary if you add attributes *after* init() is already
     * called, and you set an order for those attributes.
     */
    function attribSort()
    {
      usort($this->m_attribIndexList,array("atknode","attrib_cmp"));
    }

    /**
     * Search all records for the occurance of a certain expression.
     *
     * This function searches in all fields that are not AF_HIDE_SEARCH, for
     * a certain expression (substring match). The search performed is an
     * 'or' search. If any of the fields contains the expression, the record
     * is added to the resultset.
     * @param String $epxression The keyword to search for.
     * @return array Set of records matching the keyword.
     */
    function searchDb($expression, $searchmethod="OR")
    {
      // Set default searchmethod to OR (put it in m_postvars, because selectDb
      // will use m_postvars to built it's search conditions).
      $this->m_postvars['atksearchmethod'] = $searchmethod;

      // To perform the search, we fill atksearch, so selectDb automatically
      // searches. Because an atksearch variable may have already been set,
      // we save it to restore it after the query.
      $orgsearch = $this->m_postvars['atksearch'];

      // Built whereclause.
      foreach (array_keys($this->m_attribList) as $attribname)
      {
        $p_attrib = &$this->m_attribList[$attribname];
        // Only search in fields that aren't explicitly hidden from search
        if (!$p_attrib->hasFlag(AF_HIDE_SEARCH))
        {
           $this->m_postvars['atksearch'][$attribname]=$expression;
        }
      }

      // We load records in admin mode, se we are certain that all fields are added.
      $recs = $this->selectDb("", $this->m_defaultOrder, "", $this->m_listExcludes, "", "admin");

      // Restore original atksearch
      $this->m_postvars['atksearch'] = $orgsearch;

      return $recs;
    }

    /**
     * Determine the url for the feedbackpage.
     *
     * Output is dependent on the feedback configuration. If feedback is not
     * enabled for the action, this method returns an empty string, so the
     * result of this method can be passed directly to the redirect() method
     * after completing the action.
     *
     * The $record parameter is ignored by the default implementation, but
     * derived classes may override this method to perform record-specific
     * feedback.
     * @param String $action The action that was performed
     * @param int $status The status of the action.
     * @param array $record The record on which the action was performed.
     * @param String $message An optional message to pass to the feedbackpage,
     *                        for example to explain the reason why an action
     *                        failed.
     * @return String The feedback url.
     */
    function feedbackUrl($action, $status, $record="", $message="")
    {
      return ((isset($this->m_feedback[$action])&&hasFlag($this->m_feedback[$action], $status))
                      ||$status==ACTION_FAILED?session_url(dispatch_url($this->atkNodeType(),"feedback", array("atkfbaction"=>$action, "atkactionstatus"=>$status,"atkfbmessage"=>$message)),SESSION_REPLACE):"");
    }

    /**
     * Validates if a filter is valid for this node.
     *
     * A filter is considered valid if it doesn't contain any fields that are
     * not part of the node.
     *
     * Why isn't this used more often???
     *
     * @param String $filter The filter expression to validate
     * @returns String Returns $filter if the filter is valid or a empty
     *                 string if not.
     */
    function validateFilter($filter)
    {
      // Get the field that it applies to
      // $relation holds the tablename and $field holds the field
      list($relation, $field) = explode('.', $filter);

      // if the filter includes a tablename
      if ($field != "")
      {
        // check if the filter table is the same as the nodes table
        if (strtolower(trim($relation)) !== strtolower($this->m_table) && !in_array($relation, array_keys($this->m_attribList))) return "";

        // go on to check the field
        $relation = $field;
      }

      // All standard SQL operators
      $sqloperators = array('=','<>','>','<','>=','<=','BETWEEN','LIKE','IN', 'IS');

      // Check the filter for every operator
      foreach ($sqloperators as $sqloperator)
      {
        // split the fieldname from the fieldvalue
        list($relation_new, $dummy) = explode($sqloperator, $relation);

        // check if the fieldname is in the attriblist
        if (in_array(trim($relation_new), array_keys($this->m_attribList))) return $filter;
      }
      return "";
    }

    /**
     * Add a stylesheet to the page.
     *
     * The theme engine is used to determine the path, and load the correct
     * stylesheet.
     * @param String $style The filename of the stylesheet (without path).
     */
    function addStyle($style)
    {
      $theme = &atkinstance("atk.ui.atktheme");
      $page = &$this->getPage();
      $page->register_style($theme->stylePath($style));
    }

    /**
     * Sets numbering of the attributes to begin with the number that was passed to it,
     * or defaults to 1.
     * @param mixed $number the number that the first attribute begins with
     */
    function setNumbering($number = 1)
    {
      $this->m_numbering = $number;
    }

    /**
     * Gets the numbering of the attributes
     * @return mixed the number whith which the numbering starts
     */
    function getNumbering()
    {
      return $this->m_numbering;
    }

    /**
     * Set the security of one or more actions action the same as other actions.
     * If $mapped is empty $action has to be an array. The key would be used as action and would be mapped to the value.
     * If $mapped is not empty $action kan be a string containing one action of an array with one or more action. In both
     * cases al actions would be mapped to $mappped
     * @param Mixed $action The action that has to be mapped
     * @param String $mapped The action on witch $action has to be mapped
     */
    function addSecurityMap($action, $mapped="")
    {
      if($mapped != "")
        if(!is_array($action))
        {
          $this->m_securityMap[$action] = $mapped;
          $this->changeMapping($action,$mapped);
        }
        else
        {
          foreach ($action as $value)
          {
            $this->m_securityMap[$value] = $mapped;
            $this->changeMapping($value,$mapped);
          }
        }
      else
        if(is_array($action))
          foreach ($action as $key=>$value)
          {
            $this->m_securityMap[$key] = $value;
            $this->changeMapping($key,$value);
          }
    }

    /**
     * change the securitymap that already exist. Where actions are mapped on $oldmapped change it by $newmapped
     * @param string $oldmapped the old value
     * @param string $newmapped the new value with replace the old one
     */
    function changeMapping($oldmapped, $newmapped)
    {
      foreach ($this->m_securityMap as $key=>$value)
      {
        if($value == $oldmapped)
          $this->m_securityMap[$key] = $newmapped;
      }
    }

    /**
     * Add an atkActionListener to the node.
     *
     * @param atkActionListener $listener
     */
    function addListener(&$listener)
    {
      $listener->setNode($this);
      $this->m_listeners[] = $listener;
    }

    /**
     * Notify all listeners of the occurance of a certain action.
     *
     * @param String $action The action that occurred
     * @param array $record The record on which the action was performed
     */
    function notify($action, $record)
    {
      for($i=0, $_i=count($this->m_listeners); $i<$_i; $i++)
      {
        $this->m_listeners[$i]->notify($action, $record);
      }
    }

    /**
     * Get the column configuration object
     *
     * @return atkColumnConfig
     */
    function &getColumnConfig()
    {
      atkimport("atk.recordlist.atkcolumnconfig");
      $columnConfig = &atkColumnConfig::getConfig($this);
      return $columnConfig;
    }

    /**
     * Translate using this node's module and type.
     */
    function text($string, $module=NULL, $lng="", $firstfallback="", $nodefaulttext=false)
    {
      if ($module==NULL) $module = $this->m_module;
      return atktext($string, $module, $this->m_type, $lng, $firstfallback, $nodefaulttext);
    }

  }
?>

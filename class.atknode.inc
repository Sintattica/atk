<?php

  // Define some flags for nodes. Use the constructor of the atkNode
  // class to set the flags. (concatenate multiple flags with '|')

  define("NF_NO_ADD"        ,  1); // No new records may be added.
  define("NF_NO_EDIT"       ,  2); // Records may not be edited
  define("NF_NO_DELETE"     ,  4); // Records may not be deleted
  define("NF_EDITAFTERADD"  ,  8); // Immediately after you add a new record,
                                   // you get the editpage for that record.
  define("NF_NO_SEARCH"     , 16); // Records may not be searched.
  define("NF_NO_FILTER"     , 32); // Ignore addFilter filters..
  define("NF_ADD_LINK"      , 64); // Doesn't show an add form on admin pages,
                                   // but a link to the form.
  define("NF_NO_VIEW"       ,128); // Records may not be viewed.

  define("NF_COPY",                256); // Records / trees may  be copied
  
  define("NF_AUTOSELECT", 4096); // If this flag is set and only one record is
                                 // present on a selectpage, atk automatically
                                 // selects it and moves on to the target.

  define("NF_TRACK_CHANGES", 8192); // If set, atk stores the old values of
                                    // a record as ["atkold"] in the $rec that
                                    // gets passed to the postUpdate

  define("NF_NO_SECURITY", 16384);  // Quick way to disable accessright checking
                                    // for an entire node. (Everybody may access
                                    // this node

  define("NF_NO_EXTENDED_SEARCH", 32768); // Extended search feature is turned off.

  define("NF_MULTI_RECORD_ACTIONS",  65536); // Multi-selection of records is turned on.
  define("NF_MRA", NF_MULTI_RECORD_ACTIONS); // Shortcut for the multi-record actions flag.
  define("NF_MRPA",                 131072); // Multi-priority-selection of records is turned on.

  define("NF_LOCK",                 262144); // lock records
  define("NF_ML",                   524288); // multilanguage support
  define("NF_MULTILANGUAGE",         NF_ML);

  define("NF_READONLY", NF_NO_ADD|NF_NO_DELETE|NF_NO_EDIT); // aggregate flag to quickly create readonly nodes.
    
  define("NF_SPECIFIC_1",                    33554432); // specific node flag 1
  define("NF_SPECIFIC_2",                    67108864); // specific node flag 2
  define("NF_SPECIFIC_3",                   134217728); // specific node flag 3
  define("NF_SPECIFIC_4",                   268435456); // specific node flag 4
  define("NF_SPECIFIC_5",                   536870912); // specific node flag 5  

  /*
   * action status flags
   * Note that these have binary numbers, even though an action could never have
   * two statusses at the same time.
   * This is done however, so the flags can be used as a mask in the setFeedback
   * function.
   */
  define("ACTION_CANCELLED", 1);
  define("ACTION_FAILED", 2);
  define("ACTION_SUCCESS", 4);

  /**
   * The atkNode class represents a piece of information that
   * is part of an application. This class provides standard
   * functionality for adding, editing and deleting nodes.
   * This class must be seen as an abstract base class: For
   * every piece of information in an application, a class
   * must be derived from this class with specific
   * implementations for that type of node.
   *
   * @author Ivo Jansch (ivo@achievo.org)
   *
   * @version $Revision$
   *
   * $Id$
   *
   */  

  // Global theme variable, must be declared before the includes
  $g_theme = array();

  // Global node list
  $g_nodes = array();
  // Global module list
  $g_modules = array();
  // Global menu
  $g_menu = array();
  // Global moduleflags per module
  $g_moduleflags = array();

  // modifiers
  $g_modifiers = array();
  $g_overloaders = array();

  // Another workaround. We use PHP_SELF a lot. Withour register_globals=On, we
  // need to make it global ourselves. Todo: replace EVERY reference to
  // PHP_SELF with _SERVER["PHP_SELF"] eventually.
  $PHP_SELF = $_SERVER["PHP_SELF"];

  require_once($config_atkroot."atk/atkconfigtools.inc");
  require_once($config_atkroot."atk/defaultconfig.inc.php");

  // Auto-register modules
  if (file_exists("$config_module_path/config.modules.inc"))
    include_once("$config_module_path/config.modules.inc");

  include_once($config_atkroot."config.inc.php");
  require_once($config_atkroot."atk/atktools.inc");

  // Once atktools is loaded, we can register our own error handler.
//  set_error_handler("atkErrorHandler");
  //error_reporting(E_ALL);

  require_once($config_atkroot."atk/ui/class.atkoutput.inc");
  require_once($config_atkroot."atk/class.layout.inc");  
  require_once($config_atkroot."atk/db/class.atk".$config_database."db.inc");  
  require_once($config_atkroot."atk/attributes/class.atkattribute.inc");  
  require_once($config_atkroot."atk/session/class.atksessionmanager.inc");
  require_once($config_atkroot."atk/security/class.atksecuritymanager.inc");
  require_once($config_atkroot."atk/modules/atkmoduletools.inc");
  require_once($config_atkroot."atk/modules/class.atkmodule.inc");  
  require_once($config_atkroot."atk/lock/class.atklock.inc");

  // Default language file
  require_once($config_atkroot."atk/languages/".$config_languagefile);

  // Application specific language file
  if (file_exists($config_atkroot."languages/".$config_languagefile))
  {
    include_once($config_atkroot."languages/".$config_languagefile);
  }

  // Create database object.
  include_once($config_atkroot."atk/include/db.inc");

   $g_stickyurl = array();

  /************* MODULE HANDLING *************/
  foreach ($g_modules as $modname => $modpath)
  {
    // Module specific language file
    $filename = "$modpath/languages/$config_languagefile";
    if (file_exists($filename))
    {
      include_once($filename);
    }

    $filename = "$modpath/module.inc";

    // The module itself
    if (file_exists($filename))
    {
      // the include file may specify modifiers.
      $modifiers = array();
      $overloaders = array();
      include_once($filename);
      for ($i=0, $_i = count($modifiers); $i < $_i; $i++)
      {
        $g_modifiers[$modifiers[$i]][] = $modname;
      }
      if (count($overloaders)>0)
      {
        $g_overloaders = array_merge($g_overloaders,$overloaders);
      }
    }
    else
    {
      atkdebug("Couldn't find module.inc for module '$modname' in '$modpath'");
    }

  }
  /*******************************************/

  // The atk node class
  class atkNode
  {

    /*** Member variables ***/
    
        
    /**
    * reference to the class which is used to validate atknodes
    * the validator is overridable by changing this variabele
    *
    * @access private
    * @var string validate_class
    */
    var $m_validate_class = "atk.atknodevalidator";

    /**
    * unique field sets of a certain node
    * doesn't contain the unique fields which have been set by flag AF_UNIQUE
    * use uniqueFieldSet() to add sets of unique fields
    *
    * @access private
    * @var array uniqueFieldSets
    */
    var $m_uniqueFieldSets = array();


    /**
     * Nodes must be initialised using the init() function before they can be used.
     * This member indicated whether the node has been initialised.
     */
    var $m_initialised = false;

    /**
     * Check to prevent double execution of setAttribSizes on pages with more than one form
     */
    var $m_attribsizesset = false;

    /**
     * The list of attributes of a node. These should be of the class atkAtribute
     * or one of its derivatives.
     */
    var $m_attribList = array();
    var $m_attribIndexList = array();
    
    var $m_page = NULL;

    /**
     * List of available tabs. Keyvalue array in form array('action'=>'tablist')
     */
    var $m_tabList = array();
    
    /**
     * The type of node. (The constructor of a derived class passes its type
     * to the atkNode class.
     */
    var $m_type;

    /**
     * The module of the node. (If it's part of a module)
     */
    var $m_module;

    /**
     * The table to use for data storage.
     */
    var $m_table;
    var $m_seq;
    
    /**
     * Language field
     */
    var $m_lngfield;

    /**
     * The primary key of this node
     */

    var $m_primaryKey = array();

    /**
     * The postvars (or getvars) that are passed to a page will be passed
     * to the class using the dispatch function. We store them in a member
     * variable for easy access.
     */
    var $m_postvars = array();

    /**
     * The action that we are currently performing.
     */
    var $m_action;
    
    /**
     * The active action handler.
     */
    var $m_handler = NULL;

    /**
     * This array is used to store error in the input data. The array is an
     * associative array with fieldname as key and an errormessage as value.
     */
    //var $m_errors = array();

    var $m_default_order = "";

    /**
    * var used for tracking relation within this node
    */
    var $m_relations = array();
    
    /**
     * Node flags
     */
    var $m_flags;

    /*
     * If set, we create an alphabetical index on top of the recordlist in adminpages,
     * based on the tablefield indicated by m_index.
     */
    var $m_index = "";

    /**
     * Default tab being displayed in add/edit mode.
     */
    var $m_default_tab = "default";

    /**
     * Record filters
     */
    var $m_filters = array();
    var $m_fuzzyFilters = array();

    /**
     * For speed, we keep a list of fields we don't have to load in recordlists.
     */
    var $m_listExcludes = array();

    /**
     * For speed, we keep a list of fields we don't have to load when in view mode
     */
    var $m_viewExcludes = array();

    /**
     * For speed, we keep a list of fields that have the cascade delete flag set..
     */
    var $m_cascadingAttribs = array();

    /**
     * Actions are mapped to security units. For example, both actions "save" and "add"
     * require access "add". If an item is not in this list, it's treated 'as-is'.
     */
    var $m_securityMap = array("save"=>"add",
                               "update"=>"edit",
                               "copy"=>"add",
                               "editcopy"=>"add",
                               "search"=>"admin");

    /**
     * The right to execute certain actions can be implied by the fact that you
     * have some other right. For example, if you have the right to access a
     * feature (admin right), you may also view that record, and don't need
     * explicit rights to view it. So the 'view' right is said to be 'implied'
     *  by the 'admin' right.
     */
    var $m_securityImplied = array("view"=>"admin");

    /**
     * If a class is named 'project', then by default, if the system needs to know whether
     * a user may edit a record, the securitymanager searches for 'edit' access on 'project'.
     * However, if an alias is set here, the securitymanger searches for 'edit' on that alias.
     */
    var $m_securityAlias = "";

    /*
     * Nodes can specify actions that require no access level
     * Note: for the moment, the "select" action is always allowed.
     * TODO: This may not be correct. We have to find a way to bind the
     * select action to the action that follows after the select.
     */
    var $m_unsecuredActions = array("select", "feedback");

    /*
     *
     * Boolean that is set to true when the stacktrace is displayed, so it
     * is displayed only once.
     */
    var $m_statusbarDone = false;

    /* search action(s), action that will be performed if only one record is found */
    var $m_search_action;

    /* priority actions and min/max range for multi-record-priority actions */
    var $m_priority_actions = array();
    var $m_priority_min = 1;
    var $m_priority_max = 0;

    /* the lock instance */
    var $m_lock = NULL;

    /* List of actions that should give feedback */
    var $m_feedback = array();
        
    
    /* default language used by Multilanguage Nodes */
    var $m_defaultlanguage = "";
    
    /* cached ui object */
    var $m_atkUI = NULL; 

    /*** Public functions ***/

    /**
     * Constructor. This initialises stuff..
     * <br>
     * <b>Example:</b>
     *        $this->atkNode('test',AN_NO_EDIT);
     * @param $type Type of node
     * @param $flags The flags for the node
     */
    function atkNode($type, $flags=0)
    {
      $this->m_type = $type;
      $this->m_flags = $flags;            
    }

    /**
     * Add an atkAttribute to the node ($attribute should be an object of type
     * atkAttribute or one of its derivatives)
     * @param $attribute the attribute you want to add
     * @param $tab on which tab should the field be displayed?
     *        (use array for multiple tabs or "*" for all tabs!)
     * @param $order override the attribute order
     */
    function add($attribute,$tab=NULL,$order=0)
    {
      static $attrib_order = 100;
      static $attrib_index = 0;
      $attribute->m_owner = $this->m_type;

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
      if(is_object($this->m_attribList[$attribute->fieldName()]))
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
            if (!$attribute->hasFlag(AF_HIDE_ADD)) $this->m_tabList["add"] = atk_array_merge($this->m_tabList["add"], $tab);
            if (!$attribute->hasFlag(AF_HIDE_EDIT)) $this->m_tabList["edit"] = atk_array_merge($this->m_tabList["edit"], $tab);
            if (!$attribute->hasFlag(AF_HIDE_ADD|AF_HIDE_EDIT) && !$attribute->hasFlag(AF_HIDE_VIEW)) $this->m_tabList["view"] = atk_array_merge($this->m_tabList["view"], $tab);
          }
        }

        $attribute->m_order = $order;
        $this->m_attribIndexList[$attrib_index]=array("name"=>$attribute->fieldName(),"tabs"=>$tab,"order"=>$attribute->m_order);
        $attribute->m_index = $attrib_index;        
        $attribute->m_tabs = $tab;
        $attrib_index++;
      }
      $this->m_attribList[$attribute->fieldName()]=&$attribute;
      
      if(is_subclass_of($attribute,"atkrelation"))
      {
        $this->m_relations[strtolower(get_class($attribute))][$attribute->fieldName()]=&$attribute;
      }
    }

    /**
     * Completely remove an attribute.
     */
    function remove($attribname)
    {
      if (is_object($this->m_attribList[$attribname]))
      {
        $listindex = $this->m_attribList[$attribname]->m_index;

        unset($this->m_attribList[$attribname]);
        for ($i=0, $_i=count($this->m_listExcludes); $i<$_i; $i++)
        {
          if ($this->m_listExcludes[$i] == $attribname) unset($this->m_listExcludes[$i]);
        }
        for ($i=0, $_i=count($this->m_viewExcludes); $i<$_i; $i++)
        {
          if ($this->m_viewExcludes[$i] == $attribname) unset($this->m_viewExcludes[$i]);
        }
        for ($i=0, $_i=count($this->m_cascadingAttribs); $i<$_i; $i++)
        {
          if ($this->m_cascadingAttribs[$i] == $attribname) unset($this->m_cascadingAttribs[$i]);
        }

        unset($this->m_attribIndexList[$listindex]);
      }
    }
    
    /**
     * Checks if the the flag is set
     * @param $flag check if flag is set
     */
    function hasFlag($flag)
    {
      return (($this->m_flags & $flag) == $flag);
    }

    /**
     * Returns the primary key
     * @return Primary Key
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
     * WATCH OUT, THIS FUNCTION ONLY RETURNS THE FIRST PRIMARY KEY ATTRIB (so watch out
     * when using this with classes that have multiple)
     * @return Primary key field
     */
    function primaryKeyField()
    {
      return $this->m_primaryKey[0];
    }


    /**
     * Returns the primary key
     * @return Primary key
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
      atkdebug("Primary key tpl: ".$primKey);
      return $primKey;
    }


   /**
    * Set default order for the class
    * @param $tablename Table name
    * @fields $fields The fields for the order
    */
    function setOrder($fields)
    {
      $this->m_default_order = $fields;
    }


    /**
     * Set the table that the node should use. This should be called in the
     * constructor of the node-derived classes but AFTER the constructor of
     * the atkNode class itself is called.
     * @param $tablename The Tablename
     * @param $seq sequence
     */
    function setTable($tablename,$seq="")
    {
      $this->m_table      = $tablename;
      if ($seq=="") $seq = $tablename;
      $this->m_seq        = $seq;
    }

    /**
     * Create an alphabetical index in admin and selectpages, based on the
     * specified attribute. Ofcourse, the indexed field must be a string type.
     * @param attribname The name of the attribute on which to create an index.
     */
    function setIndex($attribname)
    {
      $this->m_index = $attribname;
    }

    /**
     * Set default tab being displayed in view/add/edit mode.
     * After calling this method, all attributes which are added after the method call
     * without specification of tab will be placed on the default tab. This means
     * you should use this method probably before you add any attributes to your node!
     * @param $tab the name of the default tab
     */
    function setDefaultTab($tab="default")
    {
      $this->m_default_tab = $tab;
    }
    
    /**
     * Get a list of tabs for a certain action.
     */
    function getTabs($action)
    {
      $list = &$this->m_tabList[$this->m_action];
      if (!is_array($list)) 
      {
        // fallback to view tabs.
        $list = &$this->m_tabList["view"];
      }
      
      // Attributes can also add tabs to the tablist.
      foreach(array_keys($this->m_attribList) as $attribname)
      {
        $p_attrib = &$this->m_attribList[$attribname];
        if(is_object($p_attrib))
        {
          $list = atk_array_merge($list, $p_attrib->getTabs($action));
        } else atkdebug("atknode::getTabs() Warning: $attribname is not een object!?");
      }
      return $list;
    }

    /**
     * Returns the currently active tab.
     */
    function getActiveTab()
    {
      global $ATK_VARS;
      $tablist = $this->getTabs($action);          
      
      // Note: we may not read atktab from $this->m_postvars, because $this->m_postvars is not filled if this is
      // a nested node (in a relation for example).
      if (!empty($ATK_VARS["atktab"]) && in_array($ATK_VARS["atktab"], $tablist)) $tab = $ATK_VARS["atktab"];
      elseif (!empty($this->m_default_tab) && in_array($this->m_default_tab, $tablist)) $tab = $this->m_default_tab;
      else $tab = $tablist[0];      
      return $tab;
    }

    /**
     * Add a filter
     * @param $filter The fieldname you want to filter OR a where clause expression
     * @param $value Value of the fieldname specified by filter (don't use this
     *               parameter if you use $filter as an expression).
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
          $result[] = '<input type="submit" class="btn_saveandclose" value="'.text('saveandclose').'">';
        }
        $result[] = '<input type="submit" class="btn_save" name="atknoclose" value="'.text('save').'">';
        $result[] = '<input type="submit" class="btn_cancel" name="atkcancel" value="'.text('cancel').'">';
      }

      return $result;
    }

    /**
     * Creates a *locked* page
     */
    function lockPage()
    {
      $output = $this->statusbar();
      $output.= '<img src="'.atkconfig("atkroot").'atk/images/lock.gif"><br><br>'.text("lock_locked").'<br>';
      $output.= '<br><form method="get">'.session_form(SESSION_BACK).
                        '<input type="submit" class="btn_cancel" value="&lt;&lt; '.text('back').'"></form>';
      
      $ui = &$this->getUi();
      if (is_object($ui))
      {                
        $total = $ui->renderBox(array("title"=>$this->actionTitle($this->m_action),
                                      "content"=>$output));
      }
      return $total;
      
    }
    
    function &getUi()
    {          
      if (!is_object($this->m_atkUI))
      {      
        $this->m_atkUI = &atknew("atk.ui.atkui");
        if (is_object($this->m_atkUI))
        {
          $this->m_atkUI->setNode($this);
        }
      }
      return $this->m_atkUI;
    }
    
    function actionTitle($action, $record="")
    {      
      $ui = &$this->getUi();
      if (is_object($ui))
      {
        $res = $ui->title($this->m_module, $this->m_type, $action);
      }
      if ($record!="")
      {
        $descr = $this->descriptor($record);
        if ($descr!="") $res.= "&nbsp; [".$descr."]";
      }
      return $res;
    }     
    
    function tabulate($action, $content)
    {
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
    
    function getDefaultActionParams($locked=false)
    {
      $params = $this->getHelp();
      $params["lockstatus"] = $this->getLockStatusIcon($locked);
      $params["formend"] = '</form>';
      return $params;
    }        
    
    /**
     * Function outputs an array with edit fields. For each field the array
     * contains the name, edit HTML code etc. ( name, html, obligatory, error, label }
     *
     * @param $mode         the edit mode ("add" or "edit")
     * @param $forcelist    a key-value array used to preset certain fields to a certain value
     * @param $suppresslist an array of fields that you want to hide
     * @param $fieldprefix  if set, each form element is prefixed with the specified
     *                      prefix (used in embedded form fields)
     *
     * @return array with edit fields (per field ( name, html, obligatory, error, label })
     */
    function editArray($mode="add", $record=NULL, $forceList="", $suppressList="", $fieldprefix="")
    {
      global $g_sessionData;

      /* read metadata */
      $this->setAttribSizes();

      /* default values */
      $defaults = $record;

      /* formdata from session */
      if ($this->m_postvars["atkformdata"]!="")
      {
        atkdebug("Restoring formdata from session");
        $this->modifyRecord($defaults, $this->m_postvars["atkformdata"]);
        $this->m_postvars["atkformdata"]="";
        $g_sessionData["default"]["stack"][atkStackID()][atkLevel()]["atkformdata"]="";
      }

      /* edit mode */
      if ($mode == "edit")
      {
        $result["hide"][] = '<input type="hidden" name="'.$fieldprefix.'atkaction" value="update">';

        /* nodes can define edit_values */
        if (method_exists($this, "edit_values"))
        {
          $overrides = $this->edit_values($defaults);
          foreach ($overrides as $varname=>$value)
          {
            $defaults[$varname]=$value;
          }
        }
      }

      /* add mode */
      else
      {
        $result["hide"][] = '<input type="hidden" name="'.$fieldprefix.'atkaction" value="save">';

        /* nodes can define initial values, if they don't already have values. */
        if (method_exists($this,"initial_values"))
        {
          $overrides = $this->initial_values();
          foreach ($overrides as $varname=>$value)
          {
            if ($defaults[$varname]=="") $defaults[$varname]=$value;
          }
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
//echo $attribname."<br>";
        if ($p_attrib!=NULL)
        {
          /* fields that have not yet been initialised may be overriden in the url */
          if ($defaults[$p_attrib->fieldName()]=="" && $this->m_postvars[$p_attrib->fieldName()]!="")
          {
            $defaults[$p_attrib->fieldName()] = $this->m_postvars[$p_attrib->fieldName()];
          }
  
          /* sometimes a field is hidden although not specified by the field itself */
          $tabs = $this->m_attribIndexList[$r]["tabs"];
  
          if ((is_array($suppressList) && count($suppressList) > 0 && in_array($attribname,$suppressList)) ||
              !$p_attrib->showOnTab($tab))
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
     * The system calls this method to determine which template to use when 
     * rendering a certain screen. The default implementation always returns 
     * the same template. You can override this method in your node however,
     * to determine on the fly which template to use. 
     * The action, the current record (if any) and the tab are passed as
     * parameter. By using these params, you can have custom templates per
     * action, and/or per tab, and even per record.
     */
    function getTemplate($action, $record="", $tab="")
    {
      switch ($action)
      {
        case "add": // add and edit both use the same form.
        case "edit": return "editform.tpl";
        case "view": return "viewform.tpl";
        case "search": return "searchform.tpl";
      }
    }

    /**
     * Function outputs a form with all values hidden. This is probably only
     * usefull for the atkOneToOneRelation's hide method.
     *
     * @param $mode         the edit mode ("add" or "edit")
     * @param $forcelist    a key-value array used to preset certain fields to a certain value
     * @param $fieldprefix  if set, each form element is prefixed with the specified
     *
     * @return hide form HTML string
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
     * Builds a tab navigation panel
     * @return tab navigation panel HTML code
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
        foreach ($list as $t)
        {
          $newtab["title"] = text("tab_$t",$this->m_type);
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
     * return an array with the default actions for a certain mode.
     *
     * @param $mode The mode for which you want a list of actions.
     *              Currently available modes for this method:
     *               - "admin" (for actions in adminscreens)
     *               - "relation" (for the list of actions when
     *                 displaying a recordlist in a onetomany-relation)
     *               - "view" (for actions when viewing only)
     *              Note: the default implementation of defaultActions
     *              makes no difference between "relation" and "admin" and
     *              will return the same actions for both, but you might want
     *              to override this behaviour in derived classes.
     * @param $params An array of extra parameters to add to all the action
     *                urls. You can use this to pass things like an atkfilter
     *                for example. The array should be key/value based.
     *
     * @example defaultActions("admin") will return a list of actions
     *          that can be performed on records of this node in an
     *          admin screen.
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
     * @param $min the minimum priority
     * @param $max the maximum priority (0 for auto => min + record count)
     */
    function setPriorityRange($min=1, $max=0)
    {
      $this->m_priority_min = (int)$min;
      if ($max < $this->m_priority_min) $max = 0;
      else $this->m_priority_max = $max;
    }

    /**
     * Sets the possible multi-record-priority actions.
     * @param $actions list of actions
     */
    function setPriorityActions($actions)
    {
      if (!is_array($actions)) $this->m_priority_actions = array();
      else $this->m_priority_actions = $actions;
    }        
  
    /**
     * Function outputs a page in which the user is asked if he really wants
     * to perform a certain action.
     * @param $atkselector Selected record you want to delete
     */
    function confirmAction($atkselector, $action, $locked=FALSE, $checkoverride=TRUE)
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
      $buttons[] = '<input name="confirm" class="btn" type="submit" value="'.text('yes').'">';
      $buttons[] = '<input name="cancel" type="submit" class="btn_cancel" value="'.text('no').'">';
      
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
        if (method_exists($this,"descriptor_def"))
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
      
//      $output.=$ui->renderAction("delete", array("content"=>$this->confirmDeleteText($atkselector),
      $output.=$ui->renderAction("delete", array("content"=>$this->confirmActionText($atkselector),
                                                "formstart"=>$formstart,
                                                "formend"=>'</form>',
                                                "buttons"=>$buttons));
      return $ui->renderBox(array("title"=>$this->actionTitle('delete', $record),
                                  "content"=>$output));                                          
    }

    /**
     * Confirmation message.
     */
    function confirmActionText($atkselector="", $action="delete", $checkoverride=TRUE)
    {
      $method = 'confirm'.$action.'text';
      if ($checkoverride && method_exists($this, $method)) return $this->$method($atkselector);
      else return text("confirm_$action".(is_array($atkselector) && count($atkselector) > 1 ? '_multi' : ''), $this->m_type, $this->m_module);
    }

    // Small compare function for sorting attribs on order field
    function attrib_cmp($a,$b)
    {
      if ($a["order"] == $b["order"]) return 0;
      return ($a["order"] < $b["order"]) ? -1 : 1;
    }

    /**
     * This function initialises certain elements of the node. This must be called right
     * after the constructor. The function has a check to prevent it from being executed
     * twice
     */
    function init()
    {
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

      // See if there are modules active that modify all nodes with the same name
      // for example, modules modifying all classes in all modules with the
      // name 'bill'.
      for ($i = 0, $_i = count($g_modifiers[$this->atknodetype()]); $i < $_i; $i++)
      {
        $modname = $g_modifiers[$this->atknodetype()][$i];
        $mod = getModule($modname);
        $modifiername = $this->m_type."_modifier";

        if (method_exists($mod,$modifiername))
        {
          atkdebug("Applying modifier from module $modname to ".$this->m_type);
          $mod->$modifiername($this);
        }
        else
        {
          atkdebug("Modifier function for $modifiername not found!");
        }
      }
      // See if there are modules active that modify this module.node..
      // for example, modules modifying 'finance.bill'
      for ($i = 0, $_i = count($g_modifiers[$this->atknodetype()]); $i < $_i; $i++)
      {
        $modname = $g_modifiers[$this->atknodetype()][$i];

        $mod = getModule($modname);
        $modifiername = str_replace(".","_",$this->atknodetype())."_modifier";

        if (method_exists($mod,$modifiername))
        {
          atkdebug("Applying modifier from module $modname to ".$this->atknodetype());
          $mod->$modifiername($this);
        }
      }

      $this->attribSort();

      if ($this->hasFlag(NF_LOCK))
      {        
        $this->m_lock = &atkLock::getInstance();
      }
      
      $this->m_defaultlanguage = strtoupper(atkconfig("defaultlanguage"));

      $this->m_initialised = true;
    }

    /**
     * This function reads meta information from the database and initialises it's attributes
     * with the value of it.
     */
    function setAttribSizes()
    {
      if ($this->m_attribsizesset) return true;
      global $g_db;

      $metainfo = $g_db->tableMeta($this->m_table);

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
     * The dispatcher. This functions looks at the atkaction from the postvars
     * and determines what should be done.
     * @param $postvars Posted vars
     * @param $fullpage If true, the dispatch function renders a complete
     *                  html page, including <html><head><body> etc.
     *                  This is the default. If you set this to false,
     *                  just plain output is generated. Warning:
     *                  If you set this to false, YOU are responsible for
     *                  handling $page->m_scripts, $page->m_stylesheets
     *                  and $page->m_submitscripts. (when $fullpage is true,
     *                  these are automatically includedn in the head of the page.
     *
     */
    function dispatch($postvars, $fullpage=true)
    {
      global $g_db;
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
      }
      else
      {
        $page->addContent($this->genericPage(text('access_denied'), "<br><br>".text("error_node_action_access_denied", $this->m_type)."<br><br><br>"));      
      }
      
      $ui = &$this->getUi();
      $screen = $page->render(text('app_shorttitle')." - ".$ui->title($this->m_module, $this->m_type,$this->m_postvars['atkaction']), $fullpage);            
      
      $output = &atkOutput::getInstance();
      $output->output($screen);

      // This is the end of all things for this page..
      // so we clean up some resources..
      $g_db->disconnect();
      atkdebug("disconnected from the database");
    }

    /*** The node actions ***/        
       
    function genericPage($title, $content)
    {
       $ui = &$this->getUi();
       $this->addStyle("style.css");
       return $this->renderActionPage($title, $ui->renderBox(array("title"=>$title,
                                                                   "content"=>$content)));
    }                    
   
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
     * @param $action The action for which feedback is enabled. You can either
     *                pass one action or an array of actions.
     * @param $statusmask The status(ses) for which feedback is enabled.
     *                    If for example this is set to ACTION_FAILED,
     *                    feedback is enabled only when the specified action
     *                    failed. It is possible to specify more than one
     *                    status by concatenating them with '|'.
     *
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
            
    function &getPage()
    {
      atkimport("atk.ui.atkpage");
      $page = &atkPage::getInstance();
     
      // Backwardscompatibility hack. We must inform the (deprecated) layout class
      // of the page.
      global $g_layout;
      $g_layout->m_atkpage = &$page;
      
      return $page;
    }

    /**
     * Make browser of the user go to another page. This should be called before any call
     * to layout::outputFlush();
     */
    function redirect($location="", $record=array())
    {
      global $g_returnurl,$g_db;

      atkdebug("atknode::redirect()");

      if ($g_returnurl!="") $location = $g_returnurl;

      if ($location=="")
      {
        $location = session_url($_SERVER["PHP_SELF"],SESSION_BACK,1);
      }
      
      if (count($record))
      {
        if ($this->m_postvars["atkpkret"]!="")
        {
        //foreach ($params as $key=>$value)
        //{
          $location.="&".$this->m_postvars["atkpkret"]."=".rawurlencode($this->primaryKey($record));
        //}
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
     * Parse the $postvars and fill the record with its data.
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
     * Update a record with a set of postvars/sessionvars
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
     * Search for descriptors of the fields
     * @return array with fieldnames
     */
    function descriptorFields()
    {
      $fields = array();

      // See if node has a custom descriptor definition.
      if (method_exists($this,"descriptor_def"))
      {
        $descriptordef = $this->descriptor_def();

        // parse fields from descriptordef
        $fields = stringfields($descriptordef);

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
     * Search for descriptor in custom descriptor definition, else first attribute of a node
     * @return descriptor
     */
    function descriptor($rec="")
    {
      // See if node has a custom descriptor definition.
      if (method_exists($this,"descriptor_def"))
      {
        $descriptor = $this->descriptor_def();
        return stringparse($descriptor,$rec);
      }
      else
      {
        // default descriptor.. (default is first attribute of a node)
        $keys = array_keys($this->m_attribList);
        return $rec[$keys[0]];
      }
    }

    /**
     * Validates obligatory fields (but not the auto_increment ones, because they don't have a value yet)
     * @param KeyVal record
     * @param string mode (add | update)
     * @param KeyVal ignoreLisdt
     */
    function validate(&$record, $mode, $ignoreList=array())
    {
      $validateObj = &atknew($this->m_validate_class);

      $validateObj->setNode($this);
      $validateObj->setRecord($record);
      $validateObj->setIgnoreList($ignoreList);
      $validateObj->setMode($mode);

      return $validateObj->validate();
    } // end function validate

    /**
    * add a unique field set that will be checked within add and update actions
    *
    * @param array fieldArr
    * @return void
    */
    function addUniqueFieldset($fieldArr)
    {	
      sort($fieldArr);
      if(!in_array($fieldArr, $this->m_uniqueFieldSets))
        $this->m_uniqueFieldSets[] = $fieldArr;
    }

    /**
     * Update a record in the database
     * Note: for new records use addDb()).
     */
    function updateDb(&$record)
    {
      global $g_db;      
      
      $query = &$g_db->createQuery();

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
          $recordset = $this->selectDb($pk,"","","","","edit"); // we need the same record we got when editing.
          $record["atkorgrec"] = $recordset[0];
        }
        $query->addCondition($pk);

        $storelist = array();

        foreach (array_keys($this->m_attribList) as $attribname)
        {
          $p_attrib = &$this->m_attribList[$attribname];
          if ($p_attrib->needsUpdate($record))
          {
            $storemode = $p_attrib->storageType($mode);            
            if (hasFlag($storemode, PRESTORE)) $storelist["pre"][]=$attribname;
            if (hasFlag($storemode, POSTSTORE)) $storelist["post"][]=$attribname;
            if (hasFlag($storemode, ADDTOQUERY)) $storelist["query"][]=$attribname;
          }
        }
        
        if (!$this->storeAttributes($storelist["pre"], $record, "update")) return false;
        
        for ($i = 0, $_i = count($storelist["query"]); $i < $_i; $i++)
        {
          $p_attrib = &$this->m_attribList[$storelist["query"][$i]]; 
          $p_attrib->addToQuery($query,$this->m_table,"",$record,1,"update"); // start at level 1
        }

        if (count($query->m_fields) && !$query->executeUpdate()) return false;                      
        
        if($this->hasFlag(NF_ML)&&$record["atkmlsplit"]=="")
        {
          $record["atkmlsplit"]=1;
          $mltool = &atknew("atk.utils.atkmlsplitter");
          $mltool->updateMlRecords($this, $record,"update");
        }

        if (!$this->storeAttributes($storelist["post"], $record, "update")) return false;        

        // Now we call a postUpdate function, that can be used to do some processing after the record
        // has been saved.
        $return =  $this->postUpdate($record);
        if(!isSet($return))
        {
          atkdebug("<font color=red>Undefined return:".$this->atkNodeType().".postUpdate doesn't return anything, it should return a boolean!</font>");
          $return = true;
        }        
        return $return;
      }
      else
      {
        atkdebug("NOT UPDATING! NO SELECTOR SET!");
      }
      return true;
    }
    
    function storeAttributes($storelist, &$record, $mode)
    {       
      global $g_db;
         
      // store special storage attributes.
      for ($i = 0, $_i = count($storelist); $i < $_i; $i++)
      {        
        $p_attrib = &$this->m_attribList[$storelist[$i]];
        if (!$p_attrib->store($g_db, $record, $mode))
        {
          // something went wrong.
          return false;
        }
      }
      return true;
    }

    /**
     * Count the record(s) from a certain select query.
     * The 'selector' parameter can be anything that's valid in a 'where' statement.
     * @param $selector The 'where' clause that indicates which records to select.
     * @param $execludeList List of attributes to be excluded from the query
     * @param $includeList List of attributes that have to be included into the query
     */
    function countDb($selector="", $excludeList="", $includeList="", $mode="")
    {
      global $g_db;
      $query = &$g_db->createQuery();

      $query->addTable($this->m_table);
      $query->addCondition($selector);
      $query->m_searchmethod = $this->m_postvars['atksearchmethod'];

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

      /* there may be search criteria, which we also filter */
      $searchArray = $this->m_postvars['atksearch'];
      if (is_array($searchArray) && count($searchArray)>0)
      {
        $searchmode = $this->m_postvars["atksearchmode"];
        if ($searchmode=="") $searchmode = atkconfig("search_defaultmode");
        while (list($key,$value) = each($searchArray))
        {
          if ($value!="")
          {
            $p_attrib = &$this->m_attribList[$key];
            if (is_object($p_attrib))
            {
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

      foreach (array_keys($this->m_attribList) as $attribname)
      {
        $p_attrib = &$this->m_attribList[$attribname];
        if (
             (
                ((is_array($includeList) && in_array($attribname,$includeList))
                || (is_array($excludeList) && !in_array($attribname,$excludeList)))
                || (!is_array($excludeList) && !is_array($includeList))
                || ($p_attrib->hasFlag(AF_FORCE_LOAD))
              )
           )
        {
          $loadmode = $p_attrib->loadType($mode);          
          if (hasFlag($loadmode, ADDTOQUERY)) $loadlist["query"][]=$attribname;
          
        }
      }            

      for ($i = 0, $_i = count($loadlist["query"]); $i < $_i; $i++)
      {
        $p_attrib = &$this->m_attribList[$loadlist["query"][$i]];
        if(!$this->hasFlag(NF_ML))
        {
          $p_attrib->addToQuery($query,$this->m_table,"","",1,$mode); // start at level 1
        }
        else if($this->m_primaryKey[0] == $p_attrib->fieldName())
        {
          $query->addCondition("UPPER(".$this->m_lngfield.")='".$this->m_defaultlanguage."'");
        }
      }            
   

      $querystring = $query->buildCount($this->hasFlag(N_ML));
      //$g_db->query($querystring);
      $result = $g_db->getrows($querystring);          
      
      // group by fix:
      // if there exists a group by (added by atk/filters) the 'else' situation occurs
      if(count($result)==1)
        return $result[0]["count"];
      else
        return count($result);
    }

   /** Copies a record
        *
        *@param $selector The 'where' clause that indicates which records to select.
        */
    function copyDb(&$record)
    {
      $this->preCopy($record);
      if(!$this->addDb($record, true, "copy"))
      {
        return false;
      }
      else
      {
        return true;
      }
    }

    /**
     * Select record(s) from the database that have certain criteria.
     * The 'selector' parameter can be anything that's valid in a 'where'
     * statement.
     * @param $selector The 'where' clause that indicates which records to select.
     * @param $order Order field
     * @param $limit Limit (Not supported for Oracle databases yet)
     */
    function selectDb($selector="", $order="", $limit="", $excludeList="",$includeList="", $mode="")
    {
      global $g_db;

      $selectlist = array();
      $loadlist = array();

      if($order=="" && $this->m_default_order!="") $order=$this->m_default_order;

      $query = &$g_db->createQuery();

      $query->addTable($this->m_table);
      $query->addCondition($selector);
      if($this->hasFlag(NF_ML) && $mode!="edit" && $mode!="copy")
      {
        $query->addCondition("UPPER(".$this->m_lngfield.")='".$this->m_defaultlanguage."'");
      }
      
      $query->m_searchmethod = $this->m_postvars['atksearchmethod'];

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

      if (!atkReadOptimizer())
      {
        /* there may be search criteria, which we also filter */
        $searchArray = $this->m_postvars['atksearch'];        
        if (is_array($searchArray) && count($searchArray)>0)      
        {
          $searchmode = $this->m_postvars["atksearchmode"];
          if ($searchmode=="") $searchmode = atkconfig("search_defaultmode");
          while (list($key,$value) = each($searchArray))
          {
            if ($value!="")
            {
              $p_attrib = &$this->m_attribList[$key];
              if (is_object($p_attrib))
              {
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
      }
      $loadlist = array();
      foreach (array_keys($this->m_attribList) as $attribname)
      {
        $p_attrib = &$this->m_attribList[$attribname];
        if (
             (
                ((is_array($includeList) && in_array($attribname,$includeList))
                || (is_array($excludeList) && !in_array($attribname,$excludeList)))
                || (!is_array($excludeList) && !is_array($includeList))
                || ($p_attrib->hasFlag(AF_FORCE_LOAD))
              )
           )
        {
          $loadmode = $p_attrib->loadType($mode);        
          if (hasFlag($loadmode, POSTLOAD))
          {
            $loadlist[]=$attribname;
          }
          if (hasFlag($loadmode, ADDTOQUERY))
          {
            $p_attrib->addToQuery($query,$this->m_table,"","",1,$mode); // start at level 1
          }
        }
      }

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

      $dbrecords = $g_db->getrows($querystring);
      
      if(($mode=="edit" || $mode=="copy") && $this->hasFlag(NF_ML)) // do we have mlrecords?
      {
        $mltool = &atknew("atk.utils.atkmlsplitter");
        $mltool->combineMlRecordSet($this, $dbrecords,$query);
      }

//      $this->setOrgKeyValue($dbrecords[0]);

      $recordset = array();
      for ($i = 0, $_i = count($dbrecords); $i < $_i; $i++)
      {
        $therecord = array();
        $recordcopy = $dbrecords[$i];
        $query->deAlias($dbrecords[$i]); // dereference aliases..
        atkDataDecode($dbrecords[$i]);
        foreach (array_keys($this->m_attribList) as $attribname)
        {
          $p_attrib = &$this->m_attribList[$attribname];
          if(is_subclass_of($p_attrib,"atkrelation") && $p_attrib->createDestination() && $p_attrib->m_destInstance->hasFlag(NF_ML))
          {
            $therecord[$attribname] = $recordcopy[$attribname];
          } else  $therecord[$attribname] = $p_attrib->db2value($dbrecords[$i]);
        }
        
        // We now have a semi-complete record. We can now calculate it's primarykey, assuming
        // that attributes with load() functions can never be part of the primary key.
        // They can't be, because load() might *depend* on a primary key.
        // so the primary key must always be known before we start loading
        // attributes with load() functions.
        $therecord["atkprimkey"] = $this->primaryKey($therecord);

        $recordset[] = $therecord;

      }
      
      // also load special storage attributes.

      for ($i = 0, $_i = count($recordset); $i < $_i; $i++)
      {
        for ($j = 0, $_j = count($loadlist); $j < $_j; $j++)
        {
          $p_attrib = &$this->m_attribList[$loadlist[$j]];

          $recordset[$i][$loadlist[$j]] = $p_attrib->load($g_db, $recordset[$i], $mode);
        }
      }
      return $recordset;
    }
  
    /**
    * Add this node to a query. (mostly used when you have to join two nodes in a relation.
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
          if ($this->m_attribList[$name]->hasFlag(AF_FORCE_LOAD)) $usedFields[] = $name;
        $usedFields = array_unique($usedFields);
      }

      foreach ($usedFields as $usedfield)
      {
        list($attribname) = explode(".", $usedfield);
        $p_attrib = &$this->m_attribList[$attribname];
        $loadmode = $p_attrib->loadType($mode);
        if (hasFlag($loadmode, ADDTOQUERY))
        {        
          if ($usefieldalias) $fieldaliasprefix = $alias."_AE_";
          if (!is_object($p_attrib))
          {
            atkdebug($usedfield." is not an object?! Check your descriptor_def for non-existant fields");
          }
          else
          {
            $p_attrib->addToQuery($query,$alias, $fieldaliasprefix,"",$level+1, "select");
          }
        }
      }

    }

    /**
     * Save the current record to the database.
     * Note: the passed record is changed: it may contain new values for auto_increment fields.
     * @param $record The record to save
     * @param $exectrigger Indicates whether postAdd trigger should be executed or not
     * (defaults to yes)
     */
    function addDb(&$record, $exectrigger=true, $mode="add", $excludelist=array())
    {
      global $g_db;

      $query = &$g_db->createQuery();

      $storelist = array();      

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
      
      if (!$this->storeAttributes($storelist["pre"], $record, $mode)) return false;

      for ($i = 0, $_i = count($storelist["query"]); $i < $_i; $i++)
      {
        $p_attrib = &$this->m_attribList[$storelist["query"][$i]];
        if ($p_attrib->hasFlag(AF_AUTO_INCREMENT))
        {
          $record[$p_attrib->fieldName()]=$g_db->nextid($this->m_seq);
        }

        $p_attrib->addToQuery($query,$this->m_table,"",$record,1,"add"); // start at level 1
      }            

      if (!$query->executeInsert()) return false;
      
      if (!$this->storeAttributes($storelist["post"], $record, $mode)) return false;
      

      // new primary key
      $record["atkprimkey"] = $this->primaryKey($record);
        
      if($this->hasFlag(NF_ML)&&$record["atkmlsplit"]=="")
      {
        $record["atkmlsplit"]=1;
        $mltool = &atknew("atk.utils.atkmlsplitter");
        $mltool->updateMlRecords($this, $record,"add");
      }

      // Now we call a postAdd function, that can be used to do some processing after the record
      // has been saved.
      if ($exectrigger) 
      {
        $return = $this->postAdd($record, $mode);
        if(!isSet($return))
        {
          atkdebug("<font color=red>Undefined return:".$this->atkNodeType().".postAdd doesn't return anything, it should return a boolean!</font>");
          $return = true;
        }

        return $return;

      }
      
      atkdebug($g_db->m_error);
      return false;
    }

    /**
     * delete record from the database.
     * todo: instead of delete, set the deleted flag.
     * @param $selector Selector
     * @returns true if successful, false if error
     */
    function deleteDb($selector)
    {
      global $g_db;


      $recordset = $this->selectDb($selector,"","","","","delete");

      if (count($this->m_cascadingAttribs)>0)
      {
        for ($i = 0, $_i = count($recordset); $i < $_i; $i++)
        {
          for ($j = 0, $_j = count($this->m_cascadingAttribs); $j < $_j; $j++)
          {
            $p_attrib = &$this->m_attribList[$this->m_cascadingAttribs[$j]];
            if (!$p_attrib->delete($recordset[$i]))
            {
              // error
              return false;
            }
          }
        }
      }

      if ($g_db->query("DELETE FROM ".$this->m_table." WHERE ".$selector))
      {
        for ($i = 0, $_i = count($recordset); $i < $_i; $i++)
        {
          $return = $this->postDel($recordset[$i]);
          if(!isSet($return))
          {
            atkdebug("<font color=red>Undefined return:".$this->atkNodeType().".postDel doesn't return anything, it should return a boolean!</font>");
            $return = true;
          }

          if (!$return)
          {
            // error.
            return false;
          };
        }
        return true;
      }
      else
      {
        return false;
      }
    }

    /**
     * Function that is called right after a new record has been saved to the
     * database. This function does essentially nothing, but it can be
     * overriden in your derived classes if you want to do something special
     * after you saved a record.
     *
     * @param $record The record that has just been saved.
     * @param $mode   The 'mode' indicates whether this is a completely new
     *                record ("add") or a copy ("copy").
     */
    function postAdd($record, $mode="add")
    {
      // Do nothing
      return true;
    }

    /**
     * Function that is called before a new record has been saved to the
     * database. This function does essentially nothing, but it can be
     * overriden in your derived classes if you want to do something special
     * after you saved a record.
     *
     * @param $record The record that has just been saved.
     */
    function preAdd(&$record)
    {
      // Do nothing
    }

    /**
     * Function that is called right after an existing record has been saved to
     * the database. This function does essentially nothing, but it can be
     * overriden in your derived classes if you want to do something special
     * after you saved a record.
     *
     * @param $record The record that has just been saved.
     */
    function postUpdate($record)
    {
      // Do nothing
      return true;
    }

    /**
     * Function that is called before an existing record has been saved to
     * the database. This function does essentially nothing, but it can be
     * overriden in your derived classes if you want to do something special
     * after you saved a record.
     *
     * @param $record The record that has just been saved.
     */
    function preUpdate(&$rec)
    {
      // Do nothing
    }

    /**
     * Function that is called right after an existing record has been deleted.
     * This function does essentially nothing, but it can be
     * overriden in your derived classes if you want to do something special
     * after you deleted a record.
     *
     * @param $record The record that has just been deleted.
     */
    function postDel($record)
    {
      // Do nothing
      return true;
    }

    /**
     * Function that is called right before a copied record is stored to
     * the database.
     * This function does nothing, but it can be overriden in your derived
     * classes if you want to do some processing on a record before it is
     * being copied. For example, suppose you have a field 'title' in a
     * record. You could change the contents to 'Copy of ..', so the user can
     * distinguish between the original and the copy.
     *
     * @param &$record A reference to the copied record. You can change the
     *                 contents of the record. It is not necessary to return
     *                 the record.
     */
    function preCopy(&$record)
    {
      // remove primarykey
      unset($record["atkprimkey"]);
    }

    /**
     * Function that is called for each record in a recordlist, to determine
     * what actions may be performed on the record.
     * This function does nothing, but it can be overriden in your derived
     * class.
     * The array with actions (edit, delete, etc.) is passed to the function
     * and can be modified.
     * To create a new action, just do $actions["new_action"]="somefile.php";
     * in the derived function.
     * To disable the edit action for a record, use unset($actions["edit"]);
     *
     * @param $record The record for which actions need to be determined
     * @param &$actions Reference to an array with the already defined actions.
     *                  This is an associative array with the action identifier
     *                  as key, and an url as value.
     *
     */
    function recordActions($record, &$actions, &$mraactions)
    {
      // Do nothing.
    }
    

    /**
     * Lookup the security 'key' for an action
     */
    function securityKey($action)
    {
      if ($this->m_securityMap[$action]=="") return $action;
      return $this->m_securityMap[$action];
    }

    /**
     * Return full atknodetype of this node (module.nodetype or, if no module is set, just nodetype)
     */
    function atknodetype()
    {
      return (empty($this->m_module) ? "" : $this->m_module.".").$this->m_type;
    }

    /**
     * This function determines if the user has the right to perform a certain
     * action on the node.
     *
     * @param  $action - The action to be checked
     *         $record (optional) - The record on which the action is to be
     *                              performed. The standard implementation
     *                              ignores this parameter, but in derived
     *                              nodes you can override this function and
     *                              implement your own record based security
     *                              policy. Keep in mind though that a
     *                              record is not passed in every situation.
     *
     * @return true if the action may be performed.
     *         false if the action is not allowed
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
              || ($this->m_securityImplied[$action]!="" && $secMgr->allowed($alias, $this->m_securityImplied[$action])));
    }

    function setSecurityAlias($alias)
    {
      $this->m_securityAlias = $alias;
    }

    /**
     * Specify that an action requires no accesslevel.
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
     * @param $locked is the currently displayed item locked or not?
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
    
    function getHelp()
    {
      $res = array();
      $res["helpurl"] = $this->helpUrl();
      if ($res["helpurl"]!="")
      {
        $page = &$this->getPage();
        $page->register_script(atkconfig("atkroot")."atk/javascript/newwindow.js");          
        $res["helplabel"] = text("help");
        $res["helplink"] = '<a href="'.$res["helpurl"].'">'.$res["helplabel"].'</a>';
      }
      return $res;
    }
    
    function getLockStatusIcon($lockstatus)
    {
      if ($lockstatus)
      {
        return '<img src="'.atkconfig("atkroot").'atk/images/lock.gif" name="_lock_">';
      }
      return "";
    }

    /**
     * If there is help available for this node, return the url
     */
    function helpUrl()
    {
      $language = strtok(atkconfig("languagefile"),".");
      $file   = "help/".$language."/help.".$this->m_type.".inc";
      if (!file_exists($file))
      {
        // no help available..
        return "";
      }
      else
      {
       $name = text("help");
       $node = $this->m_type;
       return atkPopup('atk/popups/help.inc','node='.$node,$name,650,650,'yes','no');
      }
    }

    /**
     * If there is a known registered handler method for the specified action
     * we call this handler method instead of the normal node action method.
     * @param $action the node action
     */
    function callHandler($action)
    {
      $handler = getNodeHandler($this->m_type, $action);

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
     * Get a handler object for a certain action.
     */ 
    function &getHandler($action)
    {
      atkimport("atk.handlers.atkactionhandler");
      $handler = &atkActionHandler::getDefaultHandler($action);
      $handler->setNode($this);
      return $handler;
    }

    /**
     * Sets the search action. The search action is the action that will be performed
     * on the single record that is found after doing a certain search query.
     *
     * NOTE: you can specify more then 1 action. If the user isn't allowed to execute
     *       the 1st action, the 2nd action will be used, etc.
     *
     * @param ... the action name(s)
     */
    function setSearchAction()
    {
      $this->m_search_action = func_get_args();
    }

    /**
     * This function resorts the attribIndexList. This is necessary if you add
     * attributes *after* init() is already called, and you set an order for those
     * attributes.
     */
    function attribSort()
    {
      usort($this->m_attribIndexList,array("atknode","attrib_cmp"));
    }

    /**
     * Search all records for the occurance of a certain expression.
     *
     * This function searches in all fields that are not AF_HIDE_SEARCH for
     * a certain expression.
     */
    function searchDb($expression)
    {
      // Set default searchmethod to OR (put it in m_postvars, because selectDb
      // will use m_postvars to built it's search conditions).
      $this->m_postvars['atksearchmethod'] = "OR";
      
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

    function feedbackUrl($action, $status, $record="", $message="")
    {
      return (hasFlag($this->m_feedback[$action], $status)||$status==ACTION_FAILED?session_url(dispatch_url($this->atkNodeType(),"feedback", array("atkfbaction"=>$action, "atkactionstatus"=>$status,"atkfbmessage"=>$message)),SESSION_REPLACE):"");
    }

    /**
     * Validates the filter if field of filter is part of the attributelist
     * @param $filter the filter to validate
     * @returns $filter if filter is part of the attributelist or a empty string
     */
    function validateFilter($filter)
    {
      list($relation, $field) = explode('.', $filter);
      if ($field != "")
      {
        if (in_array($relation, array_keys($this->m_attribList))) return $filter;
      }
      else
      {
        list($relation_new, $dummy) = explode('=', $relation);
        if (in_array($relation_new, array_keys($this->m_attribList))) return $filter;
      }
      return "";
    }    
    
    function addStyle($style)
    {
      atkimport("atk.ui.atktheme");
      $theme = &atkTheme::getInstance();
      $page = &$this->getPage();
      $page->register_style($theme->stylePath($style));
    }
    
  }
?>
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
  define("NF_TREE_NO_ROOT_DELETE", 512); // No root elements can be deleted
  define("NF_TREE_NO_ROOT_COPY",  1024); // No root elements can be copied
  define("NF_TREE_NO_ROOT_ADD",   2048); // No root elements can be added

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

  define("NF_READONLY", NF_NO_ADD|NF_NO_DELETE|NF_NO_EDIT); // aggregate flag to quickly create readonly nodes.
    
  define("NF_SPECIFIC_1",                    33554432); // specific node flag 1
  define("NF_SPECIFIC_2",                    67108864); // specific node flag 2
  define("NF_SPECIFIC_3",                   134217728); // specific node flag 3
  define("NF_SPECIFIC_4",                   268435456); // specific node flag 4
  define("NF_SPECIFIC_5",                   536870912); // specific node flag 5

  /* recordlist flags */
  define("RL_NO_SORT",                         1); // recordlist is not sortable
  define("RL_NO_SEARCH",                       2); // recordlist is not searchable
  define("RL_EMBED",   RL_NO_SEARCH|RL_NO_SORT|4); // recordlist is embedded
  define("RL_MRA",                             8); // multi-record-actions enabled
  define("RL_MRPA",                           16); // multi-record-priority-actions enabled
  define("RL_LOCK",                           32); // records can be locked

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

  // The atk version number. (REMEMBER: update this whenever a new atk release is tagged stable!)
  $g_atkversion = "4.2";

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
  require_once($config_atkroot."atk/utils/adodb-time.inc.php");
  require_once($config_atkroot."atk/db/class.atk".$config_database."db.inc");
  require_once($config_atkroot."atk/db/class.atkquery.inc");
  require_once($config_atkroot."atk/attributes/class.atkattribute.inc");
  require_once($config_atkroot."atk/relations/class.atkrelation.inc");
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

   // session vars are valid until they are set to something else. if you go a session level higher,
   // the next level will still contain these vars (unless overriden in the url)
   $g_sessionVars = array("atknodetype","atkfilter","atkselector","atkaction");

   // pagevars are valid on a page. if you go a session level higher, the pagevars are no longer
   // visible until you return.

   $g_pageVars = array("atksearch","atksearchmode","atkorderby","atkstartat","atktarget","atkformdata","atktree","atksuppress","atktab");

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
    * @var vartype varname
    */
    var $m_validate_class = "atk.atknodevalidator";

    /**
    * unique field sets of a certain node
    * doesn't contain the unique fields which have been set by flag AF_UNIQUE
    * use uniqueFieldSet() to add sets of unique fields
    *
    * @access private
    * @var vartype varname
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
     * This array is used to store error in the input data. The array is an
     * associative array with fieldname as key and an errormessage as value.
     */
    //var $m_errors = array();

    var $m_default_order = "";

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
    
    /* Cached ui object */
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
        $attrib_index++;
      }
      $this->m_attribList[$attribute->fieldName()]=&$attribute;

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
     * Add an atkAttribute to the node ($attribute should be an object of type
     * atkAttribute or one of its derivatives)
     * This function is an alias for add(), and is the same in every way.
     * (It exists for backwardcompatibility reasons)
     * @param $attribute the attribute you want to add
     */
    function addAttribute($attribute)
    {
      $this->add($attribute);
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
     * Returns the currently active tab.
     */
    function getActiveTab()
    {
      $list = &$this->m_tabList[$this->m_action];
      if (!is_array($list)) 
      {
        // fallback to view tabs.
        $list = &$this->m_tabList["view"];
      }
      
      if (!empty($this->m_postvars["atktab"]) && in_array($this->m_postvars["atktab"], $list)) $tab = $this->m_postvars["atktab"];
      elseif (!empty($this->m_default_tab) && in_array($this->m_default_tab, $list)) $tab = $this->m_default_tab;
      else $tab = $list[0];
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
      if (!is_object($this->atkUI))
      {      
        $this->atkUI = &atknew("atk.ui.atkui");
        if (is_object($this->atkUI))
        {
          $this->atkUI->setNode($this);
        }
      }
      return $this->atkUI;
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

    /**
     * Creates an edit page
     */
    function editPage($record, $locked=FALSE)
    {        
      $page = &$this->getPage();
      $page->register_script(atkconfig("atkroot")."atk/javascript/formfocus.js");      
      $page->register_script(atkconfig("atkroot")."atk/javascript/formnavigation.js");
      $page->register_loadscript("init_nav();");
      $this->addStyle("style.css");
      
      $params = $this->getDefaultActionParams($locked);

      $params["formstart"] = '<form name="entryform" enctype="multipart/form-data" action="'.$_SERVER["PHP_SELF"].'?'.SID.'"'.
                                   ' method="post" onsubmit="return globalSubmit(this)">'.
                                   session_form();
      
      $ui = &$this->getUi();
      
      if (is_object($ui))
      {
      
        $forceList = decodeKeyValueSet($this->m_postvars['atkfilter']);
        $form =$this->editForm("edit",$record,$forceList,$this->m_postvars['atksuppress']);

        $params["content"] = $this->tabulate("edit", $form);
        
        $params["buttons"] = $this->getFormButtons("edit", $record);
        
        $output = $ui->renderAction("edit", $params);
              
        return $ui->renderBox(array("title"=>$ui->title($this->m_module, $this->m_type,'edit'),
                                      "content"=>$output));        
      }
      else
      {
        atkerror("Failed to create ui object");
      }      
    }        
    
    function tabulate($action, $content)
    {
      if (count($this->m_tabList[$action]) > 1) 
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
     * Creates an view (=readonly) page
     */
    function viewPage($record)
    {      
      $ui = &$this->getUi();
      $this->addStyle("style.css");
      
      if (is_object($ui))
      {
        $params = $this->getDefaultActionParams();        
        
        $fields = array();          
  
        // get active tab
        $tab = $this->getActiveTab();
  
        // For all attributes we use the display() function to display the
        // attributes current value. This may be overridden by supplying
        // an <attributename>_display function in the derived classes.
        foreach (array_keys($this->m_attribIndexList) as $i)
        {
          $tplfield = array();
          $p_attrib = &$this->m_attribList[$this->m_attribIndexList[$i]["name"]];
  
          if (!$p_attrib->hasFlag(AF_HIDE_VIEW) && ($this->m_attribIndexList[$i]["tabs"] == "*" || in_array($tab, $this->m_attribIndexList[$i]["tabs"])))
          {          
            // An <attributename>_display function may be provided in a derived
            // class to display an attribute. If it exists we will use that method
            // else we will just use the attribute's display method.
            $funcname = $p_attrib->m_name."_display";
            if (method_exists($this, $funcname)) $editsrc = $this->$funcname($record, "view");
            else $editsrc=$p_attrib->display($record, "view");
  
            $tplfield["full"] = $editsrc;
            $tplfield["widget"] = $editsrc; // in view mode, widget and full are equal
             
            // The Label of the attribute (can be suppressed with AF_NOLABEL or AF_BLANKLABEL)
            // For each attribute, a txt_<attributename> must be provided in the language files.
            if ($p_attrib->hasFlag(AF_NOLABEL)==false)
            {
              if ($p_attrib->hasFlag(AF_BLANKLABEL))
              {
                $tplfield["label"] = "";
              }
              else
              {
  
                $tplfield["label"] = $p_attrib->label($record);
              }
            }
            else
            {
              // Make the rest fill up the entire line
              $tplfield["label"] = "";
              $tplfield["line"] = $tplfield["full"];
            }
            
            $fields[] = $tplfield;
            
          }          
  
        }  
        
        $innerform = $ui->render($this->getTemplate("view", $record, $tab), array("fields"=>$fields));
        
        $params["content"] = $this->tabulate("view", $innerform);        
  
        $params["formstart"] = '<form method="get">'.session_form(SESSION_NESTED);
        $params["buttons"][] = '<input type="submit" class="btn_cancel" name="atkback" value="&lt;&lt; '.text('back').'">';
                          
        // if appropriate, display an edit button.
        if (!$this->hasFlag(NF_NO_EDIT)&&$this->allowed("edit", $record))
        {        
          $params["buttons"][] = '<input type="hidden" name="atkaction" value="edit">'.
                     '<input type="hidden" name="atknodetype" value="'.$this->atkNodeType().'">'.
                     '<input type="submit" class="btn" value="'.text('edit').'">';
        }      
      
        $params["formend"] = '</form>';                
        
        $output = $ui->renderAction("view", $params);
        
        $total = $ui->renderBox(array("title"=>$ui->title($this->m_module, $this->m_type,'view'),
                                      "content"=>$output));
        return $total;
      }
      else
      {
        atkerror("ui object error");
      }
    }

    /**
     * Creates an add page
     */
    function addPage($record=NULL)
    {            
      $page = &$this->getPage();
            
      $page->register_script(atkconfig("atkroot")."atk/javascript/formfocus.js");
      $page->register_script(atkconfig("atkroot")."atk/javascript/formnavigation.js");
      $page->register_loadscript("init_nav();");
      $this->addStyle("style.css");
      
      $params = $this->getDefaultActionParams();

      $formstart ='<form name="entryform" enctype="multipart/form-data" action="'.$_SERVER["PHP_SELF"].'?'.SID.'"'.
                     ' method="post" onsubmit="return globalSubmit(this)">';

      /* if we are on an admin page we would like to stay on the admin page after adding a new record */
      if ($this->m_action == "admin") $formstart.=session_form(SESSION_NESTED);
      else $formstart.=session_form();
      
      $params["formstart"] = $formstart;
      
      $ui = &$this->getUi();
      if (is_object($ui))
      {        
        $forceList = decodeKeyValueSet($this->m_postvars['atkfilter']);
        $form = $this->editForm("add",$record,$forceList);                      
        
        $params["content"] = $this->tabulate("add", $form);                
                    
        $params["buttons"][] = '<input type="submit" class="btn_save" value="'.text('save').'&nbsp;">';
        if ($this->m_action=="add")
        {
          // if action is admin, we don't show the cancelbutton..
          $params["buttons"][] ='<input type="submit" class="btn_cancel" name="atkcancel" value="'.text('cancel').'">';
        }
        $params["formend"] = '</form><SCRIPT LANGUAGE="JavaScript">placeFocus()</SCRIPT>';
        
        $output = $ui->renderAction("add", $params);
              
        $total = $ui->renderBox(array("title"=>$this->actionTitle('add'),
                                      "content"=>$output));
        
        return $total;               
      }      
      else
      {
        atkerror("ui object failure");
      }
    }

    function searchPage($record=NULL)
    {         
      $this->addStyle("style.css");      
      
      $page = &$this->getPage();
      $page->register_script(atkconfig("atkroot")."atk/javascript/formfocus.js");
      $page->register_script($config_atkroot."atk/javascript/formnavigation.js");
      $page->register_loadscript("init_nav();");
      $ui = &$this->getUi();
      if (is_object($ui))
      {        
        $params = array();
        $params["formstart"] = '<form name="entryform" action="'.$_SERVER["PHP_SELF"].'?'.SID.'" method="post">';

        // usually, extended search is called from a page, and we go back to that page
        // when the user presses search.
        if (atkLevel()>0)
        {
          $params["formstart"].=session_form(SESSION_BACK);
        }
        // but sometimes we directly call the searchpage. if that happens,
        // we reload the adminpage with the searched items.
        else
        {
          $params["formstart"].=session_form(SESSION_DEFAULT);
          $params["formstart"].='<input type="hidden" name="'.$fieldprefix.'atkaction" value="admin">';
        }
        $params["formstart"].='<input type="hidden" name="atknodetype" value="'.$this->atknodetype().'">';        
        $params["formstart"].='<input type="hidden" name="atkstartat" value="0">'; // start at first page after new search


        $params["content"] = $this->searchForm($record);
        $params["buttons"][] = '<input type="submit" class="btn_search" value="'.text('search').'">';
        $params["formend"] = '</form><SCRIPT LANGUAGE="JavaScript">placeFocus()</SCRIPT>';      
      
        $output = $ui->renderAction("search", $params);
              
        $total = $ui->renderBox(array("title"=>$this->actionTitle('search'),
                                      "content"=>$output));
        
        return $total;               
      }      
      else
      {
        atkerror("ui object failure");
      }
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

      $pk = $this->primaryKey($record);

      /* edit mode */
      if ($mode == "edit")
      {
        $result["hide"][] = '<input type="hidden" name="'.$fieldprefix.'atkaction" value="update">';

        /* nodes can define edit_values */
        if (method_exists($this, "edit_values"))
        {
          $overrides = $this->edit_values($defaults);
          while (list($varname,$value) = each($overrides))
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
          while (list($varname,$value) = each($overrides))
          {
            if ($defaults[$varname]=="") $defaults[$varname]=$value;
          }
        }
      }

      /* check for forced values */
      if (is_array($forceList))
      {
        while(list($forcedvarname,$forcedvalue)=each($forceList))
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
              if (!$p_attrib->hasFlag(AF_NO_FILTER)) $p_attrib->m_flags |= AF_READONLY|AF_HIDE_ADD;
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

        /* fields that have not yet been initialised may be overriden in the url */
        if ($defaults[$p_attrib->fieldName()]=="" && $this->m_postvars[$p_attrib->fieldName()]!="")
        {
          $defaults[$p_attrib->fieldName()] = $this->m_postvars[$p_attrib->fieldName()];
        }

        /* sometimes a field is hidden although not specified by the field itself */
        $tabs = $this->m_attribIndexList[$r]["tabs"];

        if ((is_array($suppressList) && count($suppressList) > 0 && in_array($attribname,$suppressList)) ||
            ($tabs != "*" && !atk_in_array($tab, $tabs)))
        {
          $p_attrib->m_flags |= ($mode == "add" ? AF_HIDE_ADD : AF_HIDE_EDIT);
        }

        /* we let the attribute add itself to the edit array */
        $p_attrib->addToEditArray($mode, $result, $defaults, $record['atkerror'], $fieldprefix);
      }

      /* check for errors */
      $result["error"] = $record['atkerror'];

      /* return the result array */
      return $result;
    }

    /**
     * Function outputs a form in which a record can be edited.
     *
     * @param $mode         the edit mode ("add" or "edit")
     * @param $forcelist    a key-value array used to preset certain fields to a certain value
     * @param $suppresslist an array of fields that you want to hide
     * @param $fieldprefix  if set, each form element is prefixed with the specified
     *                      prefix (used in embedded form fields)
     *
     * @return edit form HTML string
     */
    function editForm($mode="add", $record = NULL, $forceList="", $suppressList="", $fieldprefix="")
    {      
      /* get data, transform into form, return */            
      $data = $this->editArray($mode, $record, $forceList, $suppressList, $fieldprefix);                 
      // Format some things for use in tpl.
      /* check for errors and display them */
      $tab = $this->getActiveTab();
      $error_title = "";
      
      // Handle errors
      $errors = array();
      if (count($data['error']) > 0)
      {      
        $error_title = '<b>'.text('error_formdataerror').'</b>';
        $errormsg = "";

        foreach ($data["error"] as $error)
        {
          if ($error['err'] == "error_primarykey_exists")
          {
            $pk_err_attrib[] = $error['attrib_name'];
          }
          else
          {
            $type = (empty($error["node"]) ? $this->m_type : $error["node"]);
            
            if(is_array($error['attrib_name']))
              $attrib_name = $error['attrib_name'][0];
            else
              $attrib_name = $error['attrib_name'];

            $index = $this->m_attribList[$attrib_name]->m_index;
            $tabs  = $this->m_attribIndexList[$index]["tabs"];
            if (count($this->m_tabList[$this->m_action]) > 1
                 && $tabs != "*" && !atk_in_array($tab, $tabs))
              $error_tab = ' ('.text("error_tab").' '.href($_SERVER["PHP_SELF"]."?atktab=".$tabs[0],text("tab_".$tabs[0], $node->m_type),SESSION_DEFAULT,true).')';
            else $error_tab = "";
            
            if(!is_array($error['attrib_name']))
            {	
              $label = text($error['attrib_name'], $type);
            }
            else
            {
              $label = array();
              foreach($error['attrib_name'] as $attrib)
                $label[] = text($attrib, $type);

                $label= implode(", ", $label);
            }
                
            $errors[] = array("msg"=>$error['msg'].$error_tab, "label"=>$label);
          }
        }
        if (count($pk_err_attrib)>0) // Make primary key error message
        {                  
          for($i=0;$i<count($pk_err_attrib); $i++)
          {
            $pk_err_msg .= text($pk_err_attrib[$i]);
            if (($i+1) < count($pk_err_attrib)) $pk_err_msg .= ", ";
          }
          $errors[] = array("label"=>text("error_primarykey_exists"),
                            "msg"=>$pk_err_msg);
        }        
        
      }
      
      // Handle fields
      // load images
      atkimport("atk.ui.atktheme");
      $theme = &atkTheme::getInstance();
      $tipimg = $theme->imgPath("help.gif");
      $reqimg = '<img align="top" onMouseOver="javascript:window.status=\''.
                   addslashes(text("field_obligatory")).'\';" src="'.$theme->imgPath("required_field.gif").'" border="0" alt="'.
                   text("field_obligatory").'">';            

      /* display the edit fields */
      $fields = array();
      for ($i = 0, $_i= count($data["fields"]); $i<$_i; $i++)
      {
        $field = &$data["fields"][$i];                
        $tplfield = array();

        /* check for seperator */
        if (!is_array($field) && $field == "-" && $i > 0 && $data["fields"][$i-1] != "-")
        {
          $tplfield["line"] = "<hr>"; 
        }
        /* double seperator, ignore */
        elseif (!is_array($field) && $field == "-")
        {
        }        
        /* only HTML */
        elseif (!is_array($field))
        {
          $tplfield["line"] = $field;
        }
        /* edit field */
        else
        {                  
          /* does the field have a label? */
          if ($field["label"] !== NULL)
          {
            if ($field["label"] == "") 
            {
              $tplfield["label"] = "";
            }
            else
            {
              $tplfield["label"] =  $field["label"];
              if ($field["error"]) // TODO KEES
              {
                $tplfield["error"] = $field["error"];
              }
            }            
          }
          
          /* html source */
          $tplfield["widget"] = $field["html"];
          $editsrc = $field["html"];

          /* obligatory indicator */
          if ($field["obligatory"]) 
          {
            $editsrc.= $reqimg."&nbsp;";
            $tplfield["obligatory"] = $reqimg;
          }

          /* tooltip */
          $ttip = "txt_".$this->m_type."_".$field["name"]."_tooltip";
          if ($GLOBALS[$ttip] != "")
          {
            $tooltip = '<img align="top" src="'.$tipimg.'" border="0" alt="'.$GLOBALS[$ttip].'" onClick="javascript:alert(\''.addslashes($GLOBALS[$ttip]).'\')" onMouseOver="javascript:window.status=\''.addslashes($GLOBALS[$ttip]).'\';">';
            $tplfield["tooltip"] = $tooltip;
            $editsrc.=$tooltip."&nbsp;";
          }

          $tplfield["full"] = $editsrc;        
        }
        $fields[] = $tplfield; // make field available in numeric array
        $params[$field["name"]] = $tplfield; // make field available in associative array
      }
      
      $ui = &$this->getUi();
      $page = &$this->getPage();
      $page->register_script(atkconfig("atkroot")."atk/javascript/formsubmit.js");
      foreach ($data["hide"] as $hidden)
      {
        $result.= $hidden;
      }
      
      $params["fields"] = $fields; // add all fields as an numeric array.
      $params["errortitle"] = $error_title;
      $params["errors"] = $errors; // Add the list of errors.
      
      $result.= $ui->render($this->getTemplate($mode, $record, $tab), $params);
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
     * Function outputs a form that the user can use to search records.
     */
    function searchForm($record = NULL)
    {
      global $g_db;
      
      $ui = &$this->getUi();
      
      if (is_object($ui))
      {
        $this->setAttribSizes();
  
        $defaults = $record;
  
        $dbSearchModes = $g_db->getSearchModes();
        $params = array();
        $params['searchmode_title'] = text("search_mode");
        $params['searchmode_and'] = '<input type="radio" name="atksearchmethod" value="AND" checked>'.text("search_and");
        $params['searchmode_or'] = '<input type="radio" name="atksearchmethod" value="OR">'.text("search_or");      
        $params["fields"] = array();                
  
        foreach (array_keys($this->m_attribList) as $attribname)
        {
          $p_attrib = &$this->m_attribList[$attribname];
          $attribSearchModes = $p_attrib->getSearchModes();        
  
          if (!$p_attrib->hasFlag(AF_HIDE_SEARCH))
          {
            $field = array();
            // We must perform an array_values over array_intersect, to maintain a
            // correct numeric array index (array_intersect mangles those)
            $searchModes = array_values(array_intersect($dbSearchModes,$attribSearchModes));
  
            if (count($searchModes)>0) // Don't display this attribute if it doesn't support searching.
            {
            
              $funcname = $p_attrib->m_name."_search";
  
              if (method_exists($this,$funcname))
              {
                $field["widget"] = $this->$funcname($defaults, $fieldprefix);
              }
              else
              {
                $field["widget"] = $p_attrib->search($defaults, true); // second param indicates extended search.
              }
              
              $field["full"] = $field["widget"]; // lateron, we might add more to full
              
              if(count($searchModes)==1)
              {
                $field["searchmode"] = '<input type="hidden" name="atksearchmode['.$p_attrib->m_name.']" value="'.$searchModes[0].'">'.text("search_".$searchModes[0]);
              }
              else
              {
                $sel = '<select name="atksearchmode['.$p_attrib->m_name.']">';
                while (list($key, $searchMode) = each($searchModes))
                {
                  // if supported, we select the 'like' searchmode by default
                  $selected="";
                  if ($this->m_postvars["atksearchmode"][$p_attrib->m_name]==$searchMode||
                      ((!is_array($this->m_postvars["atksearchmode"]) || $this->m_postvars["atksearchmode"][$p_attrib->m_name]=="")&&
                       $searchMode=="substring")) $selected="selected";

                  $sel.= '<option value="'.$searchMode.'" '.$selected.'>'.text("search_".$searchMode);
                }
                $sel.= '</select>';
                $field["searchmode"] = $sel;
              }                        
            
              // The Label of the attribute (can be suppressed with AF_NOLABEL or AF_BLANKLABEL)
              // For each attribute, a txt_<attributename> must be provided in the language files.
              if ($p_attrib->hasFlag(AF_NOLABEL)==false)
              {
                if ($p_attrib->hasFlag(AF_BLANKLABEL))
                {
                  $field['label'] = "";
                }
                else
                {
                  $field['label'] = $p_attrib->label($record);
                }
              }
              else
              {
                $field['line'] = $field['widget'];
              }
              $params["fields"][] = $field;
              
            }            
          }                
        }
                
        return $ui->render($this->getTemplate("search", $record), $params);

      }    
      else
      {
        atkerror("ui object error");
      }
      
    }

    /**
     * Creates a navigation bar, for browsing through the record pages
     * (if a limit is set, and there are more records)
     * @return a HTML string for navigating through records
     */
    function buildNavigation($mode="")
    {
      $limit = (int)$this->m_postvars['atklimit'];

      $filter = $this->validateFilter($this->m_postvars["atkfilter"]);

      $count = (int)$this->countDb($filter, $this->m_listExcludes, "", $mode);

      // maximum number of bookmarks to pages.
      $max_bm = 10;

      if (!($limit > 0 && $count > $limit && ceil($count / $limit) > 1)) return "";

      $pages = ceil($count / $limit);
      $curr  = ($this->m_postvars['atkstartat'] / $limit) + 1;
      $begpg = $curr - floor(($max_bm-1) / 2);
      $endpg = $curr + ceil(($max_bm-1) / 2);

      if ($begpg < 1)
      {
        $begpg = 1;
        $endpg = min($pages, $max_bm);
      }

      if ($endpg > $pages)
      {
        $endpg = $pages;
        $begpg = max(1,$pages - $max_bm + 1);
      }

      if ($curr > 1)
      {
        $newstart = $this->m_postvars['atkstartat'] - $limit;
        $nav = href($_SERVER["PHP_SELF"]."?atknodetype=".$this->atkNodeType()."&atkaction=".$this->m_action."&atkstartat=".$newstart,text('previous'))."&nbsp;|&nbsp;";
      }

      for ($i = $begpg; $i <= $endpg; $i++)
      {
        $nav .= ($i == $curr) ? "<b>$i</b>" : href($_SERVER["PHP_SELF"]."?atknodetype=".$this->atkNodeType()."&atkaction=".$this->m_action."&atkstartat=".max(0, ($i-1) * $limit),"$i");
        if ($i != $endpg) $nav .= "&nbsp;|&nbsp;";
      }

      if ($curr < $pages)
      {
        $newstart = $this->m_postvars['atkstartat'] + $limit;;
        $nav .= "&nbsp;|&nbsp;".href($_SERVER["PHP_SELF"].'?atkstartat='.$newstart,text('next'));
      }

      return $nav;
    }

    /**
     * Creates an alphabetical index, for quick lookup of records.
     * @param selected The letter that is currently selected. (Can be
     *                 a string; this function only takes the first char
     *                 of the parameter.
     */
    function buildIndex($selected="")
    {

      // TODO: show only those letters that are present in the database.
      // e.g. don't show the letter Q if there aren't any records starting
      // with a Q.
      // for now, we use all letters:
      $available_letters = array('A','B','C','D','E','F','G','H','I','J','K','L','M',
                                 'N','O','P','Q','R','S','T','U','V','W','X','Y','Z');
      $count = count($available_letters);
      for ($i=0;$i<$count;$i++)
      {
        $char = $available_letters[$i];
        if (strtoupper($selected[0])==$char) $label="<b>$char</b>";
        else $label=$char;

        // We use wildcard search for the index. Should be supported by any database.

        $nav.=href($_SERVER["PHP_SELF"]."?atknodetype=".$this->atkNodeType()."&atkaction=".$this->m_action."&atksearch[".$this->m_index."]=".$char."*&atksearchmode[".$this->m_index."]=wildcard",$label);

        // uncomment the following line if 26 letters seems to become too wide.
        //if ($count>13 && ($i+1)==floor($count/2)) $nav.='<br>'; else
        if ($i!=$count-1) $nav.=" ";
      }

      return $nav;
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
      $list = &$this->m_tabList[$action];
      
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
     * Admin page displays records and the actions that can be performed on
     * them (edit, delete)
     */
    function adminPage($actions="")
    {     
      $this->addStyle("style.css");
      
      $ui = &$this->getUi();
      if (is_object($ui))
      {        
        $params = array();

        // When there's a lot of data, records will be spread across multiple
        // pages.
        if ($this->m_postvars['atklimit']=="") $this->m_postvars['atklimit']=atkconfig("recordsperpage");
        if ($this->m_postvars['atkstartat']=="") $this->m_postvars['atkstartat']=0;

        $recordset = $this->selectDb($this->m_postvars['atkfilter'],
                                     $this->m_postvars['atkorderby'],
                                     array("offset" => $this->m_postvars['atkstartat'], "limit" => $this->m_postvars['atklimit']),
                                     $this->m_listExcludes,
                                     "",
                                     "admin");

        // check for search-action (if search returns only one record!)
        if (is_array($this->m_postvars['atksearch']) && count($this->m_postvars['atksearch']) > 0 &&
            is_array($this->m_search_action) && count($recordset) == 1)
        {
          for ($i = 0, $_i = count($this->m_search_action); $i < $_i; $i++)
            if ($this->allowed($this->m_search_action[$i], $recordset[$i]))
            {
              $this->redirect(session_url($_SERVER["PHP_SELF"].'?atknodetype='.$this->atkNodeType().
                              '&atkaction='.$this->m_search_action[$i].
                              '&atkselector='.rawurlencode($this->primaryKey($recordset[0])),
                               SESSION_REPLACE));
              break;
            }
        }                

        $params["header"] = $this->adminHeader();      

        if ($this->hasFlag(NF_ADD_LINK) && !$this->hasFlag(NF_NO_ADD) && $this->allowed("add"))
        {
          $addurl = $_SERVER["PHP_SELF"].'?atknodetype='.$this->atknodetype().'&atkaction=add';
          if ($GLOBALS["txt_link_".getNodeType($this->m_type)."_add"]!="")
          {
            // specific text
            $label = text("link_".getNodeType($this->m_type)."_add");
          }
          else
          {
            // generic text
            $label = text(getNodeType($this->m_type))." ".text("add");
          }
          $params["header"].="<br><br>".href($addurl,$label,SESSION_NESTED);
        }

        // create index and navigation bar
        if ($this->m_index != "") 
        {
          $params["index"] = $this->buildIndex($recordset[0][$this->m_index]);
        }
        $params["navbar"] = $this->buildNavigation("admin");
        
        if(!is_array($actions))
        {
          $actions = $this->defaultActions("admin");
        }

        $flags = ($this->hasFlag(NF_MRA) ? RL_MRA : 0) | ($this->hasFlag(NF_MRPA) ? RL_MRPA : 0) | ($this->hasFlag(NF_LOCK) ? RL_LOCK : 0);
        $params["list"] = $this->normalRecordList($recordset, $actions, $flags);
 
        $params["footer"] = $this->adminFooter();                            
        
        $output = $ui->renderList("admin", $params);   
      
        return $ui->renderBox(array("title"=>$this->actionTitle('admin'),
                                    "content"=>$output));
      }                 
      else
      {
        atkerror("Failed to create ui object");
      }      
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
     * Function outputs an array with all information necessary to output a recordlist.
     *
     * @param $recordset    list of records that need to be displayed
     * @param $flags        recordlist flags
     * @param $prefix       prefix for each column name (used for subcalls)
     * @param $actions      list of default actions for each record
     * @param $suppress     an array of fields that you want to hide
     *
     * The result array contains the following information:
     *  "heading"  => for each visible column an array containing: "title" {, "url"}
     *  "search"   => for each visible column HTML input field(s) for searching
     *  "rows"     => list of rows, per row: "data", "actions", "mra", "record"
     *  "totalraw" => for each totalisable column the sum value field(s) (raw)
     *  "total"    => for each totalisable column the sum value (display)
     *  "mra"      => list of all multi-record actions
     *
     * @return see above
     */
    function listArray(&$recordset, $flags=0, $prefix="", $actions=array(), $suppress=array())
    {
      if (!is_array($suppress)) $suppress = array();
      $result = array("heading" => array(), "search" => array(), "rows" => array(),
                      "totalraw" => array(), "total" => array(), "mra" => array());

      /* get the heading and search columns */
      if (!hasFlag($flags, RL_NO_SEARCH)) $this->setAttribSizes();
      foreach (array_keys($this->m_attribIndexList) as $r)
      {
        $name = $this->m_attribIndexList[$r]["name"];
        if (!in_array($name, $suppress))
        {
          $attribute = &$this->m_attribList[$name];
          $attribute->addToListArrayHeader($this->m_action, $result, $prefix, $flags,
                                           $this->m_postvars["atksearch"], $this->m_postvars["atkorderby"]);
        }
      }

      /* actions array can contain multi-record-actions */
      if (count($actions) == 2 && count(array_diff(array_keys($actions), array("actions", "mra"))) == 0)
      {
        $mra = $actions["mra"];
        $actions = $actions["actions"];
      }
      else
        $mra = $this->hasFlag(NF_NO_DELETE) ? array() : array("delete");

      /* get the rows */
      for ($i = 0, $_i = count($recordset); $i < $_i; $i++)
      {
        $result["rows"][$i] = array("columns" => array(), "actions" => $actions, "mra" => $mra, "record" => &$recordset[$i], "data" => array());
        $result["rows"][$i]["selector"] = $this->primaryKey($recordset[$i]);
        $row = &$result["rows"][$i];

        /* locked */
        if (hasFlag($flags, RL_LOCK))
        {
          $result["rows"][$i]["lock"] = $this->m_lock->isLocked($result["rows"][$i]["selector"], $this->m_table);
          if (is_array($result["rows"][$i]["lock"]))
          {
            unset($row["actions"]["edit"]);
            unset($row["actions"]["delete"]);
            $row["mra"] = array();
          }
        }

        /* actions / mra */
        $this->recordActions($row["record"], $row["actions"], $row["mra"]);
        $result["mra"] = array_merge($result["mra"], $row["mra"]);
        foreach($row["actions"] as $name => $url)
        {
          if (!empty($url) && $this->allowed($name, $row["record"]))
          {
            /* dirty hack */
            $atkencoded = strpos($url, "_15B") > 0;

            $url = str_replace("%5B","[",     $url);
            $url = str_replace("%5D","]",     $url);
            $url = str_replace("_1"."5B","[", $url);
            $url = str_replace("_1"."5D","]", $url);

            if ($atkencoded) $url = str_replace('[pk]', atkurlencode(rawurlencode($row["selector"]), false), $url);
            else $url = str_replace('[pk]', rawurlencode($row["selector"]), $url);

            $url = stringparse($url, $row["record"], true);
            $row["actions"][$name] = $url;
          }
          else
          {
            unset($row["actions"][$name]);
          }
        }

        /* columns */
        foreach (array_keys($this->m_attribIndexList) as $r)
        {
          $name = $this->m_attribIndexList[$r]["name"];
          if (!in_array($name, $suppress))
          {
            $attribute = &$this->m_attribList[$name];
            $attribute->addToListArrayRow($this->m_action, $result, $i, $prefix, $flags);
          }
        }
      }

      if (hasFlag($flags, RL_MRA))
        $result["mra"] = array_values(array_unique($result["mra"]));

      return $result;
    }

    /**
     * Creates recordlist (this method only exists for backwards compatability reasons)
     * @param $recordset the list of records
     * @param $actions the default actions array
     * @param $sortable is the recordlist sortable?
     * @param $suppresslist fields we don't display
     * @param $sessionsave links are embedded in a form, save form data?
     */
    function recordList($recordset, $actions, $sortable=true, $suppressList="", $sessionsave=false)
    {
      $flags = (!$sortable ? RL_NO_SORT : 0) | ($sessionsave ? RL_EMBED : 0);
      $flags |= (($this->m_action == "admin" || $sessionsave) && $this->hasFlag(NF_MRA)) ? RL_MRA : 0;
      $flags |= (($this->m_action == "admin" || $sessionsave) && $this->hasFlag(NF_MRPA)) ? RL_MRPA : 0;
      $flags |= ($this->hasFlag(NF_NO_SEARCH) || $this->m_postvars["atkaction"] == "search") ? RL_NO_SEARCH : 0;
      $flags |= $this->hasFlag(NF_LOCK) ? RL_LOCK : 0;
      $form  = ($sessionsave ? "entryform" : "");
      return $this->normalRecordList($recordset, $actions, $flags, $suppressList, $form);
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
     * Creates a normal recordlist based on the given parameters.
     * @param $recordset the list of records
     * @param $actions the default actions array
     * @param $flags recordlist flags (see the top of this file)
     * @param $suppressList fields we don't display
     * @param $formName if embedded the form name in which we are embedded
     */
    function normalRecordList($recordset, $actions, $flags=0, $suppressList="", $formName="")
    {      
      $this->addStyle("recordlist.css");
      
      atkimport("atk.ui.atktheme");
      $theme = &atkTheme::getInstance();
     

      /* retrieve list array */
      $list = $this->listArray($recordset, $flags, "", $actions, $suppressList);

      /* Check if some flags are still valid or not... */
      if (hasFlag($flags, RL_MRA) && (count($list["mra"]) == 0 || count($list["rows"]) == 0)) $flags ^= RL_MRA;
      if (!hasFlag($flags, RL_NO_SEARCH) && count($list["search"]) == 0) $flags |= RL_NO_SEARCH;
      if (hasFlag($flags, RL_MRPA) && (count($this->m_priority_actions) == 0 || count($list["rows"]) == 0)) $flags ^= RL_MRPA;
      elseif (hasFlag($flags, RL_MRPA))
      {
        $flags = ($flags | RL_MRA | RL_MRPA ) ^ RL_MRA;
        if ($this->m_priority_max == 0)
          $this->m_priority_max = $this->m_priority_min + count($list["rows"]) - 1;
      }      

      $orientation  = atkconfig("recordlist_orientation");
      if (trim(atkconfig("recordlist_vorientation")) != "")
        $vorientation = atkconfig("recordlist_vorientation");
      else $vorientation = 'top';
      
      $ui = &$this->getUi();
      $page = &$this->getPage();
      if (is_object($ui) && is_object($page))
      {                

        /**************/
        /* HEADER ROW */
        /**************/        
        $headercols = array();
        
        if (hasFlag($flags, RL_MRA) || hasFlag($flags, RL_MRPA)) 
        {
          $headercols[] = array("content"=>""); // Empty leader on top of mra action list.
        }
        if (hasFlag($flags, RL_LOCK)) 
        {
          $headercols[] = array("content"=>'<img src="'.atkconfig("atkroot").'atk/images/lock_head.gif">');
        }
        if ($orientation == "left" || $orientation == "both") 
        {
          $headercols[] = array("content"=>"");
        }    
  
        foreach ($list["heading"] as $field => $head)
        {
          if (empty($head["url"])) 
          {
            $headercols[] = array("content"=>$head["title"]);
          }
          else 
          {
            $headercols[] = array("content"=>href($head["url"], $head["title"]));
          }
        }
  
        if ($orientation == "right" || $orientation == "both") 
        {
          $headercols[] = array("content"=>"");
        }
  
        /**************/
        /* SEARCH ROW */
        /**************/
    
        $searchcols = array();
        
        if (!hasFlag($flags, RL_NO_SEARCH))
        {
          $searchmode = $this->m_postvars["atksearchmode"];
          if (empty($searchmode)) $searchmode = atkconfig("search_defaultmode");
  
          /* Get the first one, because they are all the same for now
           * TODO: every attrib needs to get his own searchmode
           */
          if (is_array($searchmode)) $searchmode = $searchmode[0];
  
          $button = '<input type="submit" class="btn_search" value="'.text("search").'">';
          if (!$this->hasFlag(NF_NO_EXTENDED_SEARCH))
          {
            $button .= '<br>'.href($_SERVER["PHP_SELF"]."?atknodetype=".$this->atknodetype()."&atkaction=search&atksearchmode=".
                                               $searchmode,"(".text("search_extended").")",SESSION_NESTED);
          }
  
          $searchstart = '<a name="searchform"></a><form action="'.$_SERVER["PHP_SELF"].'?'.SID.'" method="get">'.
                       session_form().
                         '<input type="hidden" name="atknodetype" value="'.$this->atknodetype().'">'.
                         '<input type="hidden" name="atkaction" value="'.$this->m_action.'">'.
                         '<input type="hidden" name="atksearchmode" value="'.$searchmode.'">';
  
          if (hasFlag($flags, RL_MRA) || hasFlag($flags, RL_MRPA))
          {
            $searchcols[] = array("content"=>"");
          }
          if (hasFlag($flags, RL_LOCK)) 
          {
            $searchcols[] = array("content"=>"");
          }
          if ($orientation == "left" || $orientation == "both") 
          {
            $searchcols[] = array("content"=>$button);            
          }
  
          foreach (array_keys($list["heading"]) as $key)
          {
            if (isset($list["search"][$key])) 
            {              
              $searchcols[] = array("content"=>$list["search"][$key]);
            }
            else 
            {
              $searchcols[] = array("content"=>"");
            }
          }
          if ($orientation == "right" || $orientation == "both") 
          {
            $searchcols[] = array("content"=>$button);
          }
  
          $searchend = '</form>';
        }
  
        /*******************************************/
        /* MULTI-RECORD-(PRIORITY-)ACTIONS FORM DATA */
        /*******************************************/
        if (hasFlag($flags, RL_MRA) || hasFlag($flags, RL_MRPA))
        {  
          $page->register_script(atkconfig("atkroot")."atk/javascript/formselect.js");          
  
          $listName = "atkrl_".getUniqueId("normalRecordList");
  
          if (!hasFlag($flags, RL_EMBED))
          {
            if (empty($formName)) $formName = $listName;
            $liststart = '<form name="'.$formName.'" method="post">'.
                       session_form(SESSION_NESTED).
                       '<input type="hidden" name="atkescape" value="">'.
                       '<input type="hidden" name="atknodetype" value="'.$this->atkNodeType().'">'.
                       '<input type="hidden" name="atkaction" value="'.$this->m_action.'">';
          }
  
          if (hasFlag($flags, RL_MRA))
          {
            $liststart.= '<script language="javascript">var '.$listName.' = new Object();</script>';
          }
        }
  
        /********/
        /* ROWS */
        /********/
        
        $records = array();    
        
        for ($i = 0, $_i = count($list["rows"]); $i < $_i; $i++)
        {
          $record = array();
          
          /* Special rowColor method makes it possible to change the row color based on the record data.
           * the method can return a simple value (which will be used for the normal row color), or can be
           * an array, in which case the first element will be the normal row color, and the second the mouseover
           * row color, example: function rowColor(&$record, $num) { return array('red', 'blue'); }
           */
          $method = "rowColor";          
          $bgh = $theme->getAttribute("highlight");
          if (method_exists($this, $method))
          {
            $bgn = $this->$method($recordset[$i], $i);
            if (is_array($bgn)) list($bgn, $bgh) = $bgn;
          }
                    
  
          /* alternate colors of rows */          
          $record["background"] = $bgn;
          $record["highlight"] = $bgh;
          $record["rownum"] = $i;
  
          /* multi-record-priority-actions -> priority selection */
          if (hasFlag($flags, RL_MRPA))
          {
            $select = '<select name="'.$listName.'_atkselector[]">'.
                        '<option value="'.rawurlencode($list["rows"][$i]["selector"]).'"></option>';
            for ($j = $this->m_priority_min; $j <= $this->m_priority_max; $j++)
              $select .= '<option value="'.$j.'">'.$j.'</option>';
            $select .= '</select>';
            $record["cols"][] = array("content" => $select);
          }
  
          /* multi-record-actions -> checkbox */
          elseif (hasFlag($flags, RL_MRA))
          {
            if (count($list["rows"][$i]["mra"]) > 0)
              $record["cols"][] = array(
                           "content"=>'<input type="checkbox" name="'.$listName.'_atkselector[]" value="'.rawurlencode($list["rows"][$i]["selector"]).'" onclick="if (this.disabled) this.checked = false">'.
                                      '<script language="javascript">'.$listName.'["'.rawurlencode($list["rows"][$i]["selector"]).'"] = new Array("'.implode($list["rows"][$i]["mra"], '","').'");</script>');
  
            else $record["cols"][] = array("content"=>"");
          }
  
          /* locked? */
          if (hasFlag($flags, RL_LOCK))
          {
            if (is_array($list["rows"][$i]["lock"]))
            {
              $alt = $list["rows"][$i]["lock"]["user_id"]." / ".$list["rows"][$i]["lock"]["user_ip"];
              $record["cols"][] = array("content"=>'<img src="'.atkconfig("atkroot").'atk/images/lock.gif" alt="'.$alt.'" border="0">');
            }
            else $record["cols"][] = array("content"=>"");
          }
  
          $str_actions = "";
          foreach ($list["rows"][$i]["actions"] as $name => $url)
            $str_actions .= href($url, text($name, $this->m_type), SESSION_NESTED, hasFlag($flags, RL_EMBED)).'&nbsp;';
  
          /* actions (left) */
          if ($orientation == "left" || $orientation == "both")
          {
            if (!empty($str_actions)) 
            {
              $record["cols"][] = array("content"=>$str_actions);
            }
            else  $record["cols"][] = array("content"=>"");
            
          }
  
          /* columns */
          foreach ($list["rows"][$i]["data"] as $html)
            $record["cols"][] = array("content"=>$html);
  
          /* actions (right) */
          if ($orientation=="right"||$orientation=="both")
          {
            if (!empty($str_actions)) $record["cols"][] = array("content"=>$str_actions);
            else $record["cols"][] = array("content"=>"");
          }
  
          $records[] = $record;

        }
  
        /*************/
        /* TOTAL ROW */
        /*************/
        $totalcols = array();
        
        if (count($list["total"]) > 0)
        {          
          if (hasFlag($flags, RL_MRA) || hasFlag($flags, RL_MRPA)) $totalcols[] = array("content"=>"");
          if (hasFlag($flags, RL_LOCK)) $totalcols[] = array("content"=>"");
          if ($orientation == "left" || $orientation == "both") $totalcols[] = array("content"=>"");
  
          foreach (array_keys($list["heading"]) as $key)
          {
            if (empty($list["total"][$key])) $totalcols[] = array("content"=>"");
            else $totalcols[] = array("content"=>$list["total"][$key]);
          }
  
          if ($orientation == "right" || $orientation == "both") $totalcols[] = array("content"=>"");          
        }
          
        /*************************************************/
        /* MULTI-RECORD-PRIORITY-ACTION FORM (CONTINUED) */
        /*************************************************/
        if (hasFlag($flags, RL_MRPA))
        {
          $target = session_url($_SERVER["PHP_SELF"].'?atknodetype='.$this->atkNodeType(), SESSION_NESTED);
    
          /* multiple actions -> dropdown */
          if (count($this->m_priority_actions) > 1)
          {
            $mra = '<select name="'.$listName.'_atkaction">'.
                         '<option value="">'.text("with_selected").':</option>';
  
            foreach ($this->m_priority_actions as $name)
              $mra .= '<option value="'.$name.'">'.text($name).'</option>';
  
            $mra .= '</select>&nbsp;'.
                       '<input type="button" class="btn" value="'.text("submit").'" onclick="atkSubmitMRPA(\''.$listName.'\', this.form, \''.$target.'\')">';
          }
  
          /* one action -> only the submit button */
          else
          {
            $mra = '<input type="hidden" name="'.$listName.'_atkaction" value="'.$this->m_priority_actions[0].'">'.
                       '<input type="button" class="btn" value="'.text($this->m_priority_actions[0]).'" onclick="atkSubmitMRPA(\''.$listName.'\', this.form, \''.$target.'\')">';
          }        
  
          if (!hasFlag($flags, RL_EMBED)) $listend = '</form>';
        }
  
        /****************************************/
        /* MULTI-RECORD-ACTION FORM (CONTINUED) */
        /****************************************/
        elseif (hasFlag($flags, RL_MRA))
        {
          $target = session_url($_SERVER["PHP_SELF"].'?atknodetype='.$this->atkNodeType(), SESSION_NESTED);
  
          $mra = (count($list["rows"]) > 1 ?
                      '<a href="javascript:updateSelection(\''.$listName.'\', document.forms[\''.$formName.'\'], \'all\')">'.text("select_all").'</a> / '.
                      '<a href="javascript:updateSelection(\''.$listName.'\', document.forms[\''.$formName.'\'], \'none\')">'.text("deselect_all").'</a> / '.
                      '<a href="javascript:updateSelection(\''.$listName.'\', document.forms[\''.$formName.'\'], \'invert\')">'.text("select_invert").'</a>'
                     :
                      '<img src="'.atkconfig("atkroot").'atk/images/dummy.gif" border="0" height="1" width="1">');
                    
          /* multiple actions -> dropdown */
          if (count($list["mra"]) > 1)
          {
            $mra .= '<select name="'.$listName.'_atkaction" onchange="javascript:updateSelectable(\''.$listName.'\', this.form)">'.
                       '<option value="">'.text("with_selected").':</option>';
  
            foreach ($list["mra"] as $name)
              $mra .= '<option value="'.$name.'">'.text($name).'</option>';
  
            $mra .= '</select>&nbsp;'.
                       '<input type="button" class="btn" value="'.text("submit").'" onclick="atkSubmitMRA(\''.$listName.'\', this.form, \''.$target.'\')">';
          }
  
          /* one action -> only the submit button */
          else
          {
            $mra .= '&nbsp; <input type="hidden" name="'.$listName.'_atkaction" value="'.$list["mra"][0].'">'.
                       '<input type="button" class="btn" value="'.text($list["mra"][0]).'" onclick="atkSubmitMRA(\''.$listName.'\', this.form, \''.$target.'\')">';
          }
  
          if (!hasFlag($flags, RL_EMBED)) $listend = '</form>';
        }
        $res = $ui->render("recordlist.tpl", array("vorientation"=>$vorientation,
                                                   "rows"=>$records,
                                                   "header"=>$headercols,
                                                   "search"=>$searchcols,
                                                   "total"=>$totalcols,
                                                   "searchstart"=>$searchstart,
                                                   "searchend"=>$searchend,
                                                   "liststart"=>$liststart,
                                                   "listend"=>$listend,
                                                   "mra"=>$mra));
        return $res;
      }
    }

    /**
     * Creates printableRecordlist
     * @param $suppresslist
     * obsolete by specialRecordList
     */
    function printableRecordList($recordset, $suppressList="")
    {
      $output='<table border="0" cellspacing="0" cellpadding="4">';

      $output.="<tr>";

      // stuff for the totals row..
      $totalisable = false;
      $totals = array();

      // display a headerrow with titles.
      // Since we are looping the attriblist anyway, we also check if there
      // are totalisable collumns.
      foreach (array_keys($this->m_attribList) as $attribname)
      {
        $p_attrib = &$this->m_attribList[$attribname];
        $musthide=(is_array($suppressList)&&count($suppressList)>0&&in_array($attribname,$suppressList));
        if (
            ($p_attrib->hasFlag(AF_HIDE_LIST)==false)
            &&
            (
              ($p_attrib->hasFlag(AF_HIDE_SELECT)==false)
              ||($this->m_action!="select")
            )
            &&$musthide==false
           )
        {
          $output.='<td><b>'.text($p_attrib->fieldName(),$this->m_type).'</b></td>';

          // the totalisable check..
          if ($p_attrib->hasFlag(AF_TOTAL))
          {
            $totalisable = true;
          }

        }
      }

      $output.="</tr>";

      for ($i = 0, $_i = count($recordset); $i < $_i; $i++)
      {
        $output.='<tr>';
        foreach (array_keys($this->m_attribList) as $attribname)
        {
          $p_attrib = &$this->m_attribList[$attribname];
          $musthide=(is_array($suppressList)&&count($suppressList)>0&&in_array($attribname,$suppressList));
          if (
              ($p_attrib->hasFlag(AF_HIDE_LIST)==false)
              &&
              (
                ($p_attrib->hasFlag(AF_HIDE_SELECT)==false)
                ||($this->m_action!="select")
              )
              &&$musthide==false
             )
          {
            // An <attributename>_display function may be provided in a derived
            // class to display an attribute.
            $funcname = $p_attrib->m_name."_display";

            if (method_exists($this,$funcname))
            {
              $value=$this->$funcname($recordset[$i], "list");
            }
            else
            {
              // otherwise, the display function of the particular attribute
              // is called.
              $value=$p_attrib->display($recordset[$i], "list");
            }
            $output.='<td>'.($value==""?"&nbsp;":$value).'</td>';

            // Calculate totals..
            if ($p_attrib->hasFlag(AF_TOTAL))
            {
              $totals[$attribname] = $p_attrib->sum($totals[$attribname], $recordset[$i]);
            }
          }
        }

        $output.='</tr>';
      }

      // totalrow..
      if ($totalisable)
      {
        $totalRow = '<tr>';

        // Third loop.. this time for the totals row.
        foreach (array_keys($this->m_attribList) as $attribname)
        {
          $p_attrib = &$this->m_attribList[$attribname];
         $musthide=(is_array($suppressList)&&count($suppressList)>0&&in_array($attribname,$suppressList));
          if (
              ($p_attrib->hasFlag(AF_HIDE_LIST)==false)
              &&
              (
                ($p_attrib->hasFlag(AF_HIDE_SELECT)==false)
                ||($this->m_action!="select")
              )
              &&$musthide==false
             )
          {
            if ($p_attrib->hasFlag(AF_TOTAL))
            {
              $totalRow.='<td><b>'.$p_attrib->display($totals[$attribname], "list").'</b></td>';
            }
            else
            {
              $totalRow.='<td>&nbsp;</td>';
            }
          }
        }

        $totalRow.="</tr>";

        $output.=$totalRow;
      }

      $output.='</table>';

      return $output;

    }

  /**
     * Creates a special Recordlist that can be used for exporting to files of to make it printable
     * @param $recordset $suppresslist

     * @param $sol                -- start row/record/line
     * @param $eof                -- end field
     * @param $sof                -- start field
     * @param $eol                -- end record/line
     * @param $type               -- 0=in simple table ; 1= export
     * @param outputparamKeyVal   -- Key-Value parameters for output. Currently existening: 
     *                               filename   -- the name of the file (without extension .cvs)
   *@param $compression         -- Compression technique (bzip / gzip)
  */
   function specialRecordList($recordset, $sol, $sof, $eof, $eol, $type="0", $compression="",$suppressList="", $outputparamKeyVal = Array())
    {
      // example      html         csv

      // $sol     = "<tr>"         or  ""
      // $eof     = "</td>"        or  ";"
      // $sof     = "<td>"         or  ""
      // $eol     = "</tr>"        or  "\r\n"
      //$empty  om lege tabelvelden op te vullen;

      // stuff for the totals row..
      $totalisable = false;
      $totals = array();
      if ($type=="0")
       {
        $output = '<table border="1" cellspacing="0" cellpadding="4">';
        $empty = "&nbsp;";
       }
      if ($type=="1")
       {
        $output="";
        $empty="";
       }

       $output .= $sol;

      // display a headerrow with titles.
      // Since we are looping the attriblist anyway, we also check if there
      // are totalisable collumns.
      foreach (array_keys($this->m_attribList) as $attribname)
      {
        $p_attrib = &$this->m_attribList[$attribname];
        $musthide=(is_array($suppressList)&&count($suppressList)>0&&in_array($attribname,$suppressList));
        if (
            ($p_attrib->hasFlag(AF_HIDE_LIST)==false)
            &&
            (
              ($p_attrib->hasFlag(AF_HIDE_SELECT)==false)
              ||($this->m_action!="select")
            )
            &&$musthide==false
           )
        {
          $output.=$sof.text($p_attrib->fieldName(),$this->m_type).$eof;

          // the totalisable check..
          if ($p_attrib->hasFlag(AF_TOTAL))
          {
            $totalisable = true;
          }

        }
      }

      $output.=$eol;

      for ($i = 0, $_i = count($recordset); $i < $_i; $i++)
      {
        $output.=$sol;
        foreach (array_keys($this->m_attribList) as $attribname)
        {
          $p_attrib = &$this->m_attribList[$attribname];
          $musthide=(is_array($suppressList)&&count($suppressList)>0&&in_array($attribname,$suppressList));
          if (
              ($p_attrib->hasFlag(AF_HIDE_LIST)==false)
              &&
              (
                ($p_attrib->hasFlag(AF_HIDE_SELECT)==false)
                ||($this->m_action!="select")
              )
              &&$musthide==false
             )
          {
            // An <attributename>_display function may be provided in a derived
            // class to display an attribute.
            $funcname = $p_attrib->m_name."_display";

            if (method_exists($this,$funcname))
            {
              $value=$this->$funcname($recordset[$i], "list");
            }
            else
            {
              // otherwise, the display function of the particular attribute
              // is called.
              $value=$p_attrib->display($recordset[$i], "list");
            }
            $output.=$sof.($value==""?$empty:$value).$eof;

            // Calculate totals..
            if ($p_attrib->hasFlag(AF_TOTAL))
            {
              $totals[$attribname] = $p_attrib->sum($totals[$attribname], $recordset[$i]);
            }
          }
        }

        $output.=$eol;
      }

      // totalrow..
      if ($totalisable)
      {
        $totalRow = $sol;

        // Third loop.. this time for the totals row.
        foreach (array_keys($this->m_attribList) as $attribname)
        {
          $p_attrib = &$this->m_attribList[$attribname];
        $musthide=(is_array($suppressList)&&count($suppressList)>0&&in_array($attribname,$suppressList));
          if (
              ($p_attrib->hasFlag(AF_HIDE_LIST)==false)
              &&
              (
                ($p_attrib->hasFlag(AF_HIDE_SELECT)==false)
                ||($this->m_action!="select")
              )
              &&$musthide==false
             )
          {
            if ($p_attrib->hasFlag(AF_TOTAL))
            {
              $value = $p_attrib->display($totals[$attribname], "list");
              $totalRow.=$sof.($value==""?$empty:$value).$eof;

            }
            else
            {
              $totalRow.= $sof.$empty.$eof;
            }
          }
        }

        $totalRow.=$eol;

        $output.=$totalRow;
      }

      // To the Screen
      if ($type=="0")
      {
        $output .= "</table>";
        return $output;
      }
      // To a File
      if ($type=="1") 
      {
        if (!array_key_exists("filename", $outputparamKeyVal))
          $outputparamKeyVal["filename"] = "achievo";

        $this->export2File($output, $outputparamKeyVal["filename"],"csv","csv",$compression);
      }
    }

    /**
    * export2file   exporteert data naar een file
    *@param  $data  -- the content
    *@param $fileName -- bestandsnaam
    *@param  $ext   -- extension of the file
    *@param  $type  -- the type (cvs / excel)
    *@param $compression -- Compression technique (bzip / gzip)
    * used by specialRecordist();
    * BROWSER BUG:
    * IE has problems with the use of attachment; needs atachment (someone at MS can't spell) or none.
    * however ns under version 6 accepts this also.
    * NS 6+ has problems with the absense of attachment; and the misspelling of attachment;
    * at present ie 5 on mac gives wrong filename and NS 6+ gives wrong filename.
    */

   function export2File($data, $fileName, $type, $ext, $compression="")
   {
    if($compression=="bzip")
    {
      $mime_type='application/x-bzip';
      $ext = "bz2";
    }
    elseif($compression=="gzip")
    {
      $mime_type='application/x-gzip';
      $ext = "gz";
    }
    elseif($type=="csv"||$type="excel")
    {
      $mime_type='text/x-csv';
      $ext = "csv";
    }
    else
    {
      $mime_type='application/octetstream';
    }

    header('Content-Type: '. $mime_type);
    header('Content-Disposition:  filename="'.$fileName.'.'.$ext.'"');
    header('Pragma: no-cache');
    header('Expires: 0');

    // 1. as a bzipped file
    if($compression=="bzip")
    {
      if (@function_exists('bzcompress'))
      {
        echo bzcompress($data);
      }
    }
    // 2. as a gzipped file
    else if ($compression == 'gzip')
    {
      if (@function_exists('gzencode'))
      {
        // without the optional parameter level because it bug
        echo gzencode($data);
      }
    }
    // 3. on screen
    else
    {
      echo $data;
    }

    exit;
   }


    /**
     * Select page displays records and gives the user the ability to select a record
     */
    function selectPage()
    {
      
      // When there's a lot of data, records will be spread across multiple pages.
      if ($this->m_postvars['atklimit']=="") $this->m_postvars['atklimit']=atkconfig("recordsperpage");
      if ($this->m_postvars['atkstartat']=="") $this->m_postvars['atkstartat']=0;

      $filter = $this->validateFilter($this->m_postvars["atkfilter"]);          

      $recordset = $this->selectDb($filter,
                                   $this->m_postvars['atkorderby'],
                                   array("offset" => $this->m_postvars['atkstartat'], "limit" => $this->m_postvars['atklimit']),
                                   $this->m_listExcludes,
                                   "",
                                   "select");

      if (count($recordset)==1 && $this->hasFlag(NF_AUTOSELECT))
      {
        // There's only one record and the autoselect flag is set, so we
        // automatically go to the target.
        $target = stringparse(rawurldecode(atkurldecode($this->m_postvars['atktarget'])),$recordset[0], true);
        $this->redirect(session_url($target, SESSION_NESTED));
        return "";
      }
      else
      {        
        $ui = &$this->getUi();
        
        $params["header"] = text("title_select", $this->m_type);

        if ($this->m_index != "") 
        {
          $params["index"] = $this->buildIndex($recordset[0][$this->m_index]);
        }

        // create navigation bar
        $params["navbar"] = $this->buildNavigation("select");        

        $actions["select"]=atkurldecode($this->m_postvars['atktarget']);

        $params["list"] = $this->normalRecordList($recordset, $actions);
        
        $output = $ui->renderList("select", $params);
        
        return $ui->renderBox(array("title"=>$this->actionTitle('select'),
                                    "content"=>$output));         
      }

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
      $formstart.='<input type="hidden" name="atkaction" value="delete">';
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
        $this->m_lock = &atkLock::getInstance();

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
      global $g_db, $g_sessionManager;
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

    function action_view()
    {
      atkdebug("Action view");
      $recordset = $this->selectDb($this->m_postvars['atkselector'],"","",$this->m_viewExcludes,"","view");
      
      $page = &$this->getPage();
      
      $page->addContent($this->renderActionPage("admin", $this->viewPage($recordset[0])));
    }

    function action_add()
    {
      atkdebug("Action add");
      $page = &$this->getPage();
      $page->addContent($this->renderActionPage("add", $this->addPage()));
    }

    function action_update()
    {
      global $g_db;

      if ($this->m_postvars['atkcancel']=="")
      {
        $record = $this->updateRecord();

        // just before we validate the record we call the preUpdate() to check if the record needs to be modified
        $this->preUpdate($record);

        $this->validate($record, "update");

        $error = count($record['atkerror']) > 0;
        foreach (array_keys($record) as $key)
          $error = $error || (is_array($record[$key]) && count($record[$key]['atkerror']) > 0);

        if ($error)
        {
          $this->m_action="edit";
          $page = &$this->getPage();
          $page->addContent($this->editPage($record));
        }
        else
        {
          if(!$this->updateDb($record))
          {
            $g_db->rollback();
            if($g_db->getErrorType()=="user")
            {
              triggerError($record, 'Error', $g_db->getErrorMsg(), '', '');
              $this->m_action="edit";
              $this->editPage($record);
            }
            else
            {
              $location = $this->feedbackUrl("update",ACTION_FAILED,$g_db->getErrorMsg());
              $this->redirect($location);
            }
          }
          else
          {
            $g_db->commit();
            if ($this->m_postvars['atknoclose']=="")
            {
              // 'save and close' was clicked
              $location = $this->feedbackUrl("update", ACTION_SUCCESS);
              $this->redirect($location);
            }
            else
            {
              // 'save' was clicked
              $this->m_action="edit";
              //update succesful, pk value might be changed so update m_orgkey
              $record["atkprimkey"] = $this->primaryKey($record);

              $locked = FALSE;
              if ($this->hasFlag(NF_LOCK))
              {
                $page = &$this->getPage();
                $page->addContent($this->lockPage());
                return;
              }

              //$this->setOrgKeyValue($record);
              $this->editPage($record, $locked);
              $page = &$this->getPage();
              $page->addContent($this->editPage($record, $locked));
            }

          }
        }
      }
      else
      {
        // Cancel was pressed
        $location = $this->feedbackUrl("update", ACTION_CANCELLED);
        $this->redirect($location);
      }
    }

    function action_save()
    {
      global $g_db;
      if ($this->m_postvars['atkcancel']=="")
      {
        $record = $this->updateRecord();

        // just before we validate the record we call the preAdd() to check if the record needs to be modified
        $this->preAdd($record);

        $this->validate($record, "add");


        if(!is_array($record['atkerror']))
          $record['atkerror'] = array();

        $error = count($record['atkerror']) > 0;

        foreach (array_keys($record) as $key)
        {
          $error = $error || (is_array($record[$key]) && array_key_exists('atkerror', $record[$key]) && count($record[$key]['atkerror']) > 0);
        }

        if ($error)
        {
          $this->m_action="add";
          $page = &$this->getPage();
          $page->addContent($this->addPage($record));
        }
        else
        {
          if(!$this->addDb($record, true, "add"))
          {
            $g_db->rollback();
            if($g_db->getErrorType()=="user")
            {
              triggerError($record, 'Error', $g_db->getErrorMsg(), '', '');
              $this->m_action="add";
              $this->addPage($record);
            }
            else
            {
              $location = $this->feedbackUrl("save",ACTION_FAILED,$g_db->getErrorMsg());
              $this->redirect($location);
            }
          }
          else
          {
            $g_db->commit();
            $location="";
            if ($this->hasFlag(NF_EDITAFTERADD))
            {
              $location = session_url($_SERVER["PHP_SELF"].'?atknodetype='.$this->atknodetype().'&atkaction=edit&atkselector='.rawurlencode($this->primaryKey($record)), SESSION_REPLACE);
            }
            else
            {
              $location = $this->feedbackUrl("save", ACTION_SUCCESS);
            }
            $this->redirect($location);
          }
        }
      }
      else
      {
        // Cancel was pressed
        $location = $this->feedbackUrl("save", ACTION_CANCELLED);
        $this->redirect($location);
      }
    }
    
    function genericPage($title, $content)
    {
       $ui = &$this->getUi();
       $this->addStyle("style.css");
       return $this->renderActionPage($title, $ui->renderBox(array("title"=>$title,
                                                                   "content"=>$content)));
    }

    function action_delete()
    {    
      if (!empty($this->m_postvars['confirm']))
      {
        global $g_db;
        // Confirmation page was displayed and 'yes' was clicked
        if($this->deleteDb($this->m_postvars['atkselector']))
        {
          $g_db->commit();
          $location = $this->feedbackUrl("delete", ACTION_SUCCESS);
          $this->redirect($location);
        }
        else
        {
          $g_db->rollback();
          $location = $this->feedbackUrl("delete",ACTION_FAILED,$g_db->getErrorMsg());
          $this->redirect($location);
        }
      }
      elseif (empty($this->m_postvars['cancel']))
      {
        $locked = FALSE;
        if ($this->hasFlag(NF_LOCK))
        {
          $locked = TRUE;
          if (is_array($this->m_postvars['atkselector']))
          {
            foreach ($this->m_postvars['atkselector'] as $selector)
             if (!$this->m_lock->lock($selector, $this->m_table)) $locked = FALSE;
          }
          elseif (!$this->m_lock->lock($this->m_postvars['atkselector'], $this->m_table)) $locked = FALSE;

          if (!$locked)
          {          
            $page = &$this->getPage();
            $page->addContent($this->lockPage());
            return;
          }
        }

        // Confirmation page was not displayed
        $page = &$this->getPage();        
        $page->addContent($this->confirmAction($this->m_postvars['atkselector'], "delete", $locked, TRUE));        
      }
      else
      {
        // Confirmation page was displayed and 'no' was clicked
        $location = $this->feedbackUrl("delete", ACTION_CANCELLED);
        $this->redirect($location);
      }      
    }

    function action_copy()
    {
      global $g_db;

      atkdebug("atknode::action_copy()");
      $recordset = $this->selectDb($this->m_postvars['atkselector'],"","","","","copy");
      if(count($recordset)>0)
      {
        if(!$this->copyDb($recordset[0]))
        {
          atkdebug("atknode::action_copy() -> Error");
          $g_db->rollback();
          $location = $this->feedbackUrl("save",ACTION_FAILED,$g_db->getErrorMsg());
          atkdebug("atknode::action_copy() -> Redirect");
          $this->redirect($location);
        }
        else
        {
          $g_db->commit();
        }
      }
      else
      {
        atkdebug("Geen records gevonden met selector: $selector");
      }

      $this->redirect();
    }

    function action_editcopy()
    {
      atkdebug("atknode::action_editcopy()");
      $recordset = $this->selectDb($this->m_postvars['atkselector'],"","","","","copy");

      if(count($recordset) > 0)
      {

        if(!$this->copyDb($recordset[0]))
        {
          $g_db->rollback();
          $location = $this->feedbackUrl("editcopy",ACTION_FAILED,$g_db->getErrorMsg());
          $this->redirect($location);
        }
        else
        {
          $g_db->commit();
          $location = session_url(dispatch_url($this->atknodetype(), "edit", array("atkselector" => $this->primaryKey($recordset[0]))), SESSION_REPLACE);
          $this->redirect($location);
        }
      }

      else
      {
        atkdebug("Geen records gevonden met selector: $selector");
        $this->redirect();
      }
    }

    function action_xml()
    {
      if ($this->m_postvars['atkselector']!="")
      {
        $recordset = $this->selectDb($this->m_postvars['atkselector'],"","","","","xml");
        $this->xml($recordset[0]);
      }
      else
      {
        $recordset = $this->selectDb("","","","","","xml");
        for ($i = 0, $_i = count($recordset); $i < $_i; $i++)
        {
          $this->xml($recordset[$i]);
        }
      }
    }

    function action_admin()
    {
      $page = &$this->getPage();
      
      $res = array();
      if ($this->hasFlag(NF_NO_ADD)==false&&$this->allowed("add"))
      {
        if (!$this->hasFlag(NF_ADD_LINK)) // otherwise, in adminPage, an add link will be added.
        {
          $res[]=$this->addPage();
        }
      }
      $res[]= $this->adminPage();          
                        
      $page->addContent($this->renderActionPage("admin", $res));
            
    }

    function action_search()
    {
      $page = &$this->getPage();
      $page->addcontent($this->renderActionPage("search", $this->searchPage($this->m_postvars["atksearch"])));
    }

    function action_edit()
    {
      $page = &$this->getPage();    
      
      $recordset = $this->selectDb($this->m_postvars['atkselector'],"","","","","edit");
      
      if ($this->hasFlag(NF_LOCK))
      {
        if ($this->m_lock->lock($this->primaryKey($recordset[0]), $this->m_table)) 
        {
          $res = $this->editPage($recordset[0], true);
        }
        else
        {
          $res = $this->lockPage();
        }
      }  
      else
      {
        $res = $this->editPage($recordset[0], false);
      }
            
      $page->addContent($this->renderActionPage("edit", $res));
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

    function action_select()
    {      
      $output = $this->selectPage();
      if ($output!="")
      {
        $page =  &$this->getPage();
        $page->addContent($this->renderActionPage("select", $output));
      }
    }

    function action_feedback()
    {
      $page = &$this->getPage();
      $output = $this->feedbackPage($this->m_postvars["atkfbaction"], $this->m_postvars["atkactionstatus"]);
      $page->addContent($this->renderActionPage("feedback", $output));
    }

    function feedbackPage($action, $actionstatus, $message="")
    {
      $ui = &$this->getUi();            
      
      $this->addStyle("style.css");

      $params["content"] = text('feedback_'.$action.'_'.atkActionStatus($actionstatus), $this->m_type).
                           ' '.$message;

      if (atkLevel()>0)
      {      
        $params["formstart"] = '<form method="get">'.session_form(SESSION_BACK);
        $params["buttons"][] = '<input type="submit" class="btn_cancel" value="&lt;&lt; '.text('back').'">';
        $params["formend"] = '</form>';
      }
      
      $output = $ui->renderAction($action, $params);
              
      return $ui->renderBox(array("title"=>$this->actionTitle($action),
                                  "content"=>$output)); 
    
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
    function redirect($location="")
    {
      global $g_returnurl,$g_db;

      atkdebug("atknode::redirect()");

      if ($g_returnurl!="") $location = $g_returnurl;

      if ($location=="")
      {
        $location = session_url($_SERVER["PHP_SELF"],SESSION_BACK,1);
      }
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
     * Parse xml tags
     */
    function xml($record)
    {
      $xml = "<".$this->m_type." ";

      foreach (array_keys($this->m_attribList) as $attribname)
      {
        $p_attrib = &$this->m_attribList[$attribname];
        if ($record[$p_attrib->fieldName()]!="")
        {
          $xml.=$attribname.'="'.$record[$p_attrib->fieldName()].'" ';
        }
      }
      $xml.='/>';

      if ($this->m_postvars['tohtml']==1)
      {
        return htmlspecialchars($xml).'<br>';
      }
      else
      {
        return $xml;
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
      $validateObj = &atkNew($this->m_validate_class);

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
      ("UPDATEDB CALLL!!!");
      global $g_db;

      $name = "atk".atkconfig("database")."query";
      $query = new $name();

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
            if ($p_attrib->hasStore($mode))
            {
              $storelist[]=$attribname;
            }
            else
            {
              $p_attrib->addToQuery($query,$this->m_table,"",$record,1,"update"); // start at level 1
            }
          }
        }

        if (count($query->m_fields) > 0)
        {
          if (!$query->executeUpdate())
          {          
            return false;
          }
        }

        // also store special storage attributes.
        for ($i = 0, $_i = count($storelist); $i < $_i; $i++)
        {
          $p_attrib = &$this->m_attribList[$storelist[$i]];
          if (!$p_attrib->store($g_db, $record,"update"))
          {
            // something went wrong.
            return false;
          }
        }

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
      $name = "atk".atkconfig("database")."query";
      $query = new $name();

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
          if (method_exists($p_attrib,"load"))
          {
            $loadlist[]=$attribname;
          }
          else
          {
            $p_attrib->addToQuery($query,$this->m_table,"","",1,$mode); // start at level 1
          }
        }
      }


      $querystring = $query->buildCount();
      //$g_db->query($querystring);
      $result = $g_db->getrows($querystring);
      return $result[0]["count"];
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

      $name = "atk".atkconfig("database")."query";
      $query = new $name();

      $query->addTable($this->m_table);
      $query->addCondition($selector);
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
          if (method_exists($p_attrib,"load"))
          {
            $loadlist[]=$attribname;
          }
          else
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

//      $this->setOrgKeyValue($dbrecords[0]);

      $recordset = array();
      for ($i = 0, $_i = count($dbrecords); $i < $_i; $i++)
      {
        $therecord = array();
        $query->deAlias($dbrecords[$i]); // dereference aliases..
        atkDataDecode($dbrecords[$i]);
        foreach (array_keys($this->m_attribList) as $attribname)
        {
          $p_attrib = &$this->m_attribList[$attribname];
          $therecord[$attribname] = $p_attrib->db2value($dbrecords[$i]);
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
        if (method_exists($p_attrib,"load"))
        {
          //$loadlist[]=$attribname;
          // for now.. do nothing..
        }
        else
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
    function addDb(&$record, $exectrigger=true, $mode="add")
    {
      global $g_db;

      atkdebug("atkNode::addDb() start ".$this->m_type." mode: ".$mode);

      $name = "atk".atkconfig("database")."query";
      $query = new $name();

      $storelist = array();
      $querylist = array();

      $query->addTable($this->m_table);

      foreach (array_keys($this->m_attribList) as $attribname)
      {
        $p_attrib = &$this->m_attribList[$attribname];
        if ($mode != "add" || $p_attrib->needsInsert($record))
        {
          if ($p_attrib->hasStore($mode))
          {
            $storelist[]=$attribname;
          }
          else
          {
            $querylist[]=$attribname;
          }
        }
      }

      for ($i = 0, $_i = count($querylist); $i < $_i; $i++)
      {
        $p_attrib = &$this->m_attribList[$querylist[$i]];
        if ($p_attrib->hasFlag(AF_AUTO_INCREMENT))
        {
          $record[$p_attrib->fieldName()]=$g_db->nextid($this->m_seq);
        }

        $p_attrib->addToQuery($query,$this->m_table,"",$record,1,"add"); // start at level 1
      }

      if ($query->executeInsert())
      {

        // also store special storage attributes.
        for ($i = 0, $_i = count($storelist); $i < $_i; $i++)
        {
          $p_attrib = &$this->m_attribList[$storelist[$i]];

          if (!$p_attrib->store($g_db, $record, $mode))
          {
            atkdebug("store of ".$storelist[$i]."returned false or nothing!");
            // error
            return false;
          }
        }

        // new primary key
        $record["atkprimkey"] = $this->primaryKey($record);

        // Now we call a postAdd function, that can be used to do some processing after the record
        // has been saved.
        if ($exectrigger) {
          $return = $this->postAdd($record, $mode);
          if(!isSet($return))
          {
            atkdebug("<font color=red>Undefined return:".$this->atkNodeType().".postAdd doesn't return anything, it should return a boolean!</font>");
            $return = true;
          }

          return $return;

        }
        return true;
      }
      
      atkdebug($g_db->m_error);
      atkdebug("atkNode::addDb() stop ".$this->m_type." mode:".$mode);

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
     * Function that is called when creating an adminPage. Developers can override
     * this function in their classes and return a string.
     */
    function adminHeader()
    {
      return "";
    }

   /**
     * Function that is called when creating an adminPage. Developers can override
     * this function in their classes and return a string.
     */
    function adminFooter()
    {
      return "";
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
     * Puts all sticky vars in hidden form elements.
     * DEPRECATED.
     */
    function stickyForm()
    {
    }

    /**
     * Dirty function to see if the current edit form is following an add screen
     * that had NF_EDITAFTERADD set...
     * DEPRECATED
     */
    function editingAfterAdd()
    {
      atkdebug("WARNING: use of deprecated function 'editingAfterAdd()'!");
      return FALSE;
    }

    /**
     * Display a statusbar with a stacktrace and a help button.
     * @param $locked is the currently displayed item locked or not?
     */
    function statusbar($locked=FALSE)
    {      
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
        atkdebug("Calling (external) handler function for '".$action."'");
        $handler($this, $action);
      }

      // no (valid) handler
      else
      {
        $function = "action_$action";
        if (method_exists($this, $function)) $this->$function();
        else atkdebug("dispatcher error: no action defined for '".$action."'");
      }
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
      $recs = $this->selectDb($where, $this->m_defaultOrder, "", $this->m_listExcludes, "", "admin");
      
      // Restore original atksearch
      $this->m_postvars['atksearch'] = $orgsearch;
      
      return $recs;
    }

    function feedbackUrl($action, $status,$message="")
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
    /**
    * TODO: Remove this wrapper!
    *
    * @return void
    */
    function debug($m)
    {	
      kdb($this->atkNodeType().":".$m);
    }
  }
?>
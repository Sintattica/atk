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

  // modifiers
  $g_modifiers = array();
  $g_overloaders = array();

  require_once($config_atkroot."atk/atkconfigtools.inc");
  require_once($config_atkroot."atk/defaultconfig.inc.php");

  // Auto-register modules
  if (file_exists("$config_module_path/config.modules.inc"))
    include_once("$config_module_path/config.modules.inc");

  include_once($config_atkroot."config.inc.php");
  require_once($config_atkroot."atk/atktools.inc");

  // Once atktools is loaded, we can register our own error handler.
  set_error_handler("atkErrorHandler");
  error_reporting(E_ALL);

  require_once($config_atkroot."atk/class.layout.inc");
  require_once($config_atkroot."atk/db/class.atk".$config_database."db.inc");
  require_once($config_atkroot."atk/db/class.atkquery.inc");
  require_once($config_atkroot."atk/attributes/class.atkattribute.inc");
  require_once($config_atkroot."atk/relations/class.atkrelation.inc");
  require_once($config_atkroot."atk/session/class.atksessionmanager.inc");
  require_once($config_atkroot."atk/security/class.atksecuritymanager.inc");
  require_once($config_atkroot."atk/atkmenutools.inc");
  require_once($config_atkroot."atk/modules/atkmoduletools.inc");
  require_once($config_atkroot."atk/modules/class.atkmodule.inc");

  // Default language file
  require_once($config_atkroot."atk/languages/".$config_languagefile);

  // Application specific language file
  if (file_exists($config_atkroot."languages/".$config_languagefile))
  {
    include_once($config_atkroot."languages/".$config_languagefile);
  }
  // Application specific language file
  if (file_exists("languages/".$config_languagefile))
  {
    include_once("languages/".$config_languagefile);
  }

  // Create database object.
  include_once($config_atkroot."atk/include/db.inc");

  /**
   * Sticky vars are variables that are passed in each url, or each form post.
   * There are a few default sticky vars, but nodes can add their own sticky vars
   * if they want to.
   * Sticky vars use the global value of a var, so if you change the value in your
   * code, the changed value is passed along.
   *
   * Sticky vars.. sticky vars.. what are they feeding you....
   * Sticky vars.. sticky vars.. it's not your fault!
   */
   //$g_stickyVars = array("atknodetype","atktarget","atkfilter","atkaction");

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
      for ($i=0;$i<count($modifiers);$i++)
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

  // At some places we need a random number generator, so we seed the generator here.
  srand ((double) microtime() * 1000000);

  // The atk node class
  class atkNode
  {

    /*** Member variables ***/

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

    /**
     * List of available tabs.
     */
    var $m_tabListAdd = array();
    var $m_tabListEdit = array();
    var $m_tabListView = array();

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
     * For speed, we keep a list of fields which are multilangual.
     * We also want to know if there is a multilanguage select box.
     */
    var $m_listMlAdd = array();
    var $m_listMlEdit = array();
    var $m_hasMlSelectAdd = 0;
    var $m_hasMlSelectEdit = 0;

    /**
     * Actions are mapped to security units. For example, both actions "save" and "add"
     * require access "add". If an item is not in this list, it's treated 'as-is'.
     */
    var $m_securityMap = array("save"=>"add",
                               "update"=>"edit",
                               "copy"=>"add",
                               "search"=>"admin");

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
    var $m_unsecuredActions = array("select");

    /*
     *
     * Boolean that is set to true when the stacktrace is displayed, so it
     * is displayed only once.
     */
    var $m_statusbarDone = false;



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
      global $g_securityManager;
      static $attrib_order = 100;
      static $attrib_index = 0;
      $attribute->m_owner = $this->m_type;

      // *** API HACK ***
      // Because tabs will probably be used more than the order override option
      // the API for this method now favours the $tab argument. For backwards
      // compatibility we still support the old API ($attribute,$order=0).
      if (!($tab === NULL) && is_int($tab))
      {
        $order = $tab;
        $tab = $this->m_default_tab;
      }

      // If no tab is specified or tabs are disabled, we use the current default tab
      // (specified with the setDefaultTab method, or "default" otherwise)
      elseif ($tab === NULL || strlen($tab) == 0 || !atkconfig("tabs")) $tab = $this->m_default_tab;

      // NOTE: THIS SHOULD WORK. BUT, since add() is called from inside the $this
      // constructor, m_ownerInstance ends up being a copy of $this, rather than
      // a reference. Don't ask me why, it has something to do with the way PHP
      // handles the constructor.
      // To work around this, we reassign the this pointer to the attributes as
      // soon as possible AFTER the constructor. (the dispatcher function)
      $attribute->m_ownerInstance = &$this;
      if (!$g_securityManager->attribAllowed($this->m_type, $attribute->m_name, "edit"))
      {
        $attribute->m_flags |= AF_READONLY;

        if (!$g_securityManager->attribAllowed($this->m_type, $attribute->m_name, "view"))
        {
          $attribute->m_flags |= AF_HIDE;
        }
      }

      if ($attribute->hasFlag(AF_PRIMARY))
      {
        if (!in_array($attribute->fieldName(),$this->m_primaryKey))
        {
          $this->m_primaryKey[] = $attribute->fieldName();
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

      // Speed optimization, we remember which attributes are multilangual at add mode
      if ($attribute->hasFlag(AF_MULTILANGUAGE)&&!$attribute->hasFlag(AF_HIDE_ADD))
      {
        if (!in_array($attribute->fieldName(),$this->m_listMlAdd))
        {
          $this->m_listMlAdd[]=$attribute->fieldName();
        }
      }

      // Speed optimization, we remember which attributes are multilangual at add mode
      if ($attribute->hasFlag(AF_MULTILANGUAGE)&&!$attribute->hasFlag(AF_HIDE_EDIT))
      {
        if (!in_array($attribute->fieldName(),$this->m_listMlEdit))
        {
          $this->m_listMlEdit[]=$attribute->fieldName();
        }
      }

      // check for multilanguage attribute at add/edit mode
      if ($attribute->fieldName() == 'multilanguage_select' && !$attribute->hasFlag(AF_HIDE_ADD)) $this->m_hasMlSelectAdd = 1;
      if ($attribute->fieldName() == 'multilanguage_select' && !$attribute->hasFlag(AF_HIDE_EDIT)) $this->m_hasMlSelectEdit = 1;


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

        // add new tab(s) to the tab list ("*" isn't a tab!)
        if (!is_array($tab) && $tab != "*") $tab = array($tab);
        if ($tab != "*")
        {
          if (!$attribute->hasFlag(AF_HIDE_ADD)) $this->m_tabListAdd = atk_array_merge($this->m_tabListAdd, $tab);
          if (!$attribute->hasFlag(AF_HIDE_EDIT)) $this->m_tabListEdit = atk_array_merge($this->m_tabListEdit, $tab);
          if (!$attribute->hasFlag(AF_HIDE_ADD|AF_HIDE_EDIT)) $this->m_tabListView = atk_array_merge($this->m_tabListView, $tab);
        }

        $attribute->m_order = $order;
        $this->m_attribIndexList[$attrib_index]=array("name"=>$attribute->fieldName(),"tabs"=>$tab,"order"=>$attribute->m_order);
        $attribute->m_index = $attrib_index;
        $attrib_index++;
      }
      $this->m_attribList[$attribute->fieldName()]=&$attribute;
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
      $this->add(&$attribute);
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
      if ($this->m_action == "add") $list = &$this->m_tabListAdd;
      elseif ($this->m_action == "edit") $list = &$this->m_tabListEdit;
      else $list = &$this->m_tabListView;
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
     * Creates an edit page
     */
    function editPage($record)
    {
      global $g_layout, $PHP_SELF,$config_atkroot;
      $g_layout->register_script($config_atkroot."atk/javascript/formfocus.js");
      $g_layout->ui_top($g_layout->title($this->m_module, $this->m_type,"edit"));
      $g_layout->output($this->statusbar());
      $g_layout->output('<form name="entryform" enctype="multipart/form-data" action="'.$PHP_SELF.'?'.SID.'"'.
                                   ' method="post" onsubmit="return globalSubmit(this)">');
      $g_layout->output(session_form());

      if (count($this->m_tabListEdit) > 1) $g_layout->tabs_top($this->buildTabs());
      $forceList = decodeKeyValueSet($this->m_postvars['atkfilter']);
      $g_layout->output($this->editForm("edit",$record,$forceList,$this->m_postvars['atksuppress']));
      if (count($this->m_tabListEdit) > 1) $g_layout->tabs_bottom();

      //$g_layout->ui_bottom($g_layout->title($this->m_module, $this->m_type,"edit"));

      $g_layout->output('<br>');
      if (atkLevel()>0) // if atklevel is 0 or less, we are at the bottom of the session stack,
                       // which means that 'saveandclose' doesn't close anyway, so we leave out
                       // the 'saveandclose' button.
      {
        $g_layout->output('&nbsp;<input type="submit" value="'.text('saveandclose').'">&nbsp;');
      }
      $g_layout->output('&nbsp;<input type="submit" name="atknoclose" value="'.text('save').'">&nbsp;');
      $g_layout->output('&nbsp;<input type="submit" name="atkcancel" value="'.text('cancel').'">&nbsp;');
      $g_layout->output('</form>');
      $g_layout->ui_bottom();
    }

    /**
     * Creates an view (=readonly) page
     */
    function viewPage($record)
    {
      global $g_layout, $PHP_SELF;

      $g_layout->ui_top($g_layout->title($this->m_module, $this->m_type,'view'));
      $g_layout->output($this->statusbar());

      $page.=$g_layout->ret_table_simple();

      // get active tab
      $tab = $this->getActiveTab();

      // For all attributes we use the display() function to display the
      // attributes current value. This may be overridden by supplying
      // an <attributename>_display function in the derived classes.
      for($i=0;$i<count($this->m_attribIndexList);$i++)
      {
        $p_attrib = &$this->m_attribList[$this->m_attribIndexList[$i]["name"]];

        if (!$p_attrib->hasFlag(AF_HIDE_VIEW) && ($this->m_attribIndexList[$i]["tabs"] == "*" || in_array($tab, $this->m_attribIndexList[$i]["tabs"])))
        {
          // fields that have not yet been initialised may be overriden in the url..
          $page.='<tr>';

          // Keep track of the number of td's we have to fill with the edit thingee..
          // This depends on AF_NOLABEL for example.
          $tdcount = 1;

          // The Label of the attribute (can be suppressed with AF_NOLABEL or AF_BLANKLABEL)
          // For each attribute, a txt_<attributename> must be provided in the language files.
          if ($p_attrib->hasFlag(AF_NOLABEL)==false)
          {
            if ($p_attrib->hasFlag(AF_BLANKLABEL))
            {
              $page.=$g_layout->ret_td('&nbsp;');
            }
            else
            {

              $page.=$g_layout->ret_td(text($p_attrib->fieldName(),$this->m_type).': ','valign="top"');
            }
          }
          else
          {
            $tdcount++; // If there's no label, the other td's have to be filled up.
          }

          // An <attributename>_display function may be provided in a derived
          // class to display an attribute. If it exists we will use that method
          // else we will just use the attribute's display method.
          $funcname = $p_attrib->m_name."_display";
          if (method_exists($this, $funcname)) $editsrc = $this->$funcname($record, "view");
          else $editsrc=$p_attrib->display($record, "view");

          $page .= $g_layout->ret_td($editsrc, 'colspan="'.$tdcount.'" valign="top"');
          $page .= "</tr>";
        }

      }
      $page.='</table>';

      if (count($this->m_tabListView) > 1) $g_layout->tabs_top($this->buildTabs());
      $g_layout->output($page);
      if (count($this->m_tabListView) > 1) $g_layout->tabs_bottom();

      $g_layout->output('<br><form method="get">'.session_form(SESSION_BACK).
                        '<input type="submit" value="'.text('back').'"></form>');

      $g_layout->ui_bottom();
    }

    /**
     * Creates an add page
     */
    function addPage($record=NULL)
    {
      global $g_layout, $PHP_SELF, $config_atkroot;
      $g_layout->register_script($config_atkroot."atk/javascript/formfocus.js");
      $g_layout->ui_top($g_layout->title($this->m_module, $this->m_type, 'add'));
      $g_layout->output($this->statusbar());

      $g_layout->output('<form name="entryform" enctype="multipart/form-data" action="'.$PHP_SELF.'?'.SID.'"'.
                                   ' method="post" onsubmit="return globalSubmit(this)">');

      /* if we are on an admin page we would like to stay on the admin page after adding a new record */
      if ($this->m_action == "admin") $g_layout->output(session_form(SESSION_NESTED));
      else $g_layout->output(session_form());


      if (count($this->m_tabListAdd) > 1) $g_layout->tabs_top($this->buildTabs());
      $forceList = decodeKeyValueSet($this->m_postvars['atkfilter']);
      $g_layout->output($this->editForm("add",$record,$forceList));
      if (count($this->m_tabListAdd) > 1) $g_layout->tabs_bottom();

      $g_layout->output('<br>&nbsp;<input type="submit" value="'.text('save').'&nbsp;">');
      if ($this->m_action=="add")
      {
        // if action is admin, we don't show the cancelbutton..
        $g_layout->output('&nbsp;<input type="submit" name="atkcancel" value="'.text('cancel').'">&nbsp;');
      }
      $g_layout->output('</form>');
      $g_layout->ui_bottom();
      $g_layout->output('<SCRIPT LANGUAGE="JavaScript">placeFocus()</SCRIPT>');
    }

    function searchPage($record=NULL)
    {
      global $g_layout, $PHP_SELF, $config_atkroot;
      $g_layout->register_script($config_atkroot."atk/javascript/formfocus.js");
      $g_layout->ui_top($g_layout->title($this->m_module, $this->m_type,'search'));
      $g_layout->output($this->statusbar());

      $g_layout->output('<form name="entryform" action="'.$PHP_SELF.'?'.SID.'" method="post">');

      // usually, extended search is called from a page, and we go back to that page
      // when the user presses search.
      if (atkLevel()>0)
      {
        $g_layout->output(session_form(SESSION_BACK));
      }
      // but sometimes we directly call the searchpage. if that happens,
      // we reload the adminpage with the searched items.
      else
      {
        $g_layout->output(session_form(SESSION_DEFAULT));
        $g_layout->output('<input type="hidden" name="'.$fieldprefix.'atkaction" value="admin">');
      }
      $form.='<input type="hidden" name="atknodetype" value="'.$this->atknodetype().'">';

      $g_layout->output($this->searchForm($record));
      $g_layout->output('<br>&nbsp;<input type="submit" value="'.text('search').'">&nbsp;');
      $g_layout->output('</form>');
      $g_layout->ui_bottom();
      $g_layout->output('<SCRIPT LANGUAGE="JavaScript">placeFocus()</SCRIPT>');
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
      global $config_atkroot, $g_sessionData;

      /* read metadata */
      $this->setAttribSizes();

      /* default values */
      $defaults = $record;

      /* formdata from session */
      if ($this->m_postvars["atkformdata"]!="")
      {
        atkdebug("Restoring formdata from session");
        $this->modifyRecord(&$defaults, $this->m_postvars["atkformdata"]);
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
          if ($forcedvarname!="")
          {
            if (strpos($forcedvarname,'.')>0)
            {
              list($table,$field) = split('\.',$forcedvarname);
              $defaults[$table][$field] = $forcedvalue;
              $attribname = $table;
            }
            else
            {
              $defaults[$forcedvarname]=$forcedvalue;
              $attribname = $forcedvarname;
            }
            $p_attrib = &$this->m_attribList[$attribname];
            $p_attrib->m_flags |= AF_READONLY|AF_HIDE_ADD;
          }
        }
      }

      /* extra submission data */
      $result["hide"][] = '<input type="hidden" name="'.$fieldprefix.'atkescape">';
      $result["hide"][] = '<input type="hidden" name="'.$fieldprefix.'atknodetype" value="'.$this->atknodetype().'">';
      $result["hide"][] = '<input type="hidden" name="'.$fieldprefix.'atkprimkey" value="'.$record["atkprimkey"].'">';

      /* For all attributes we use the edit() method to get HTML code for editting the
       * attribute's data. If the attribute is hidden we use the hide() method method
       * to get HTML code for hideing the attribute's data. You can override the attribute's
       * edit() method by supplying an <attributename>_edit function in the derived classes.
       */
      $tab = $this->getActiveTab();
      for ($r = 0; $r < count($this->m_attribIndexList); $r++)
      {
        $field = array("name" => $this->m_attribIndexList[$r]["name"]);
        $p_attrib = &$this->m_attribList[$field["name"]];

        /* fields that have not yet been initialised may be overriden in the url */
        if ($defaults[$p_attrib->fieldName()]=="" && $this->m_postvars[$p_attrib->fieldName()]!="")
        {
          $defaults[$p_attrib->fieldName()] = $this->m_postvars[$p_attrib->fieldName()];
        }

        /* sometimes a field is hidden although not specified by the field itself */
        $tabs = $this->m_attribIndexList[$r]["tabs"];
        if ((is_array($suppressList) && count($suppressList) > 0 && in_array($attribname,$suppressList)) ||
            ($tabs != "*" && !in_array($tab, $tabs)))
        {
          $p_attrib->m_flags |= ($mode == "add" ? AF_HIDE_ADD : AF_HIDE_EDIT);
        }

        /* we let the attribute add itself to the edit array */
        $p_attrib->addToEditArray($mode, &$result, &$defaults, &$record['atkerror'], $fieldprefix);
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
      global $g_layout;

      /* get data, transform into form, return */
      $data = $this->editArray($mode, $record, $forceList, $suppressList, $fieldprefix);
      $form = $g_layout->ret_editForm(&$data, &$this);
      return $form;
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
      global $g_layout, $config_atkroot,$g_sessionData;

      /* suppress all */
      $suppressList = array();
      for ($r = 0; $r < count($this->m_attribIndexList); $r++)
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
      global $g_layout, $g_db;

      $this->setAttribSizes();

      $defaults = $record;

      $form.=$g_layout->ret_table_simple();

      $dbSearchModes = $g_db->getSearchModes();
      $form.='<tr>';
      $form.=$g_layout->ret_td(text("search_mode"),'colspan="1"');
      $sel.='<input type="radio" name="atksearchmethod" value="AND" checked>'.text("search_and").'&nbsp;&nbsp;&nbsp;';
      $sel.='<input type="radio" name="atksearchmethod" value="OR">'.text("search_or");
      $form.=$g_layout->ret_td($sel);
      $form.='</tr>';
      $form.='<tr>'.$g_layout->ret_td("<hr>",'colspan="3"').'</tr>';

      foreach (array_keys($this->m_attribList) as $attribname)
      {
        $p_attrib = &$this->m_attribList[$attribname];
        $attribSearchModes = $p_attrib->getsearchmodes();
        $searchModes = array_intersect($dbSearchModes,$attribSearchModes);

        if (!$p_attrib->hasFlag(AF_HIDE_SEARCH))
        {
          $form.='<tr>';

          // Keep track of the number of td's we have to fill with the edit thingee..
          // This depends on AF_NOLABEL for example.
          $tdcount = 1;

          // The Label of the attribute (can be suppressed with AF_NOLABEL or AF_BLANKLABEL)
          // For each attribute, a txt_<attributename> must be provided in the language files.
          if ($p_attrib->hasFlag(AF_NOLABEL)==false)
          {
            if ($p_attrib->hasFlag(AF_BLANKLABEL))
            {
              $form.=$g_layout->ret_td('&nbsp;');
            }
            else
            {
              $form.=$g_layout->ret_td(text($p_attrib->fieldName(),$this->m_type).': ','valign="top"');
            }
          }
          else
          {
            $tdcount++; // If there's no label, the other td's have to be filled up.
          }


          $funcname = $p_attrib->m_name."_search";

          if (method_exists($this,$funcname))
          {
            $editsrc = $this->$funcname($defaults, $fieldprefix);
          }
          else
          {
            $editsrc = $p_attrib->search($defaults, true); // second param indicates extended search.
          }


          $form.=$g_layout->ret_td($editsrc,'colspan="'.$tdcount.'" valign="top"');
          if(count($searchModes)==1)
          {
            $sel = '<input type="hidden" name="atksearchmode['.$p_attrib->m_name.']" value="'.$searchModes[0].'">'.text("search_".$searchModes[0]);
          }
          else
          {
            $sel = '<select name="atksearchmode['.$p_attrib->m_name.']">';
            for ($i=0;$i<count($searchModes);$i++)
            {
              // if supported, we select the 'like' searchmode by default
              $selected="";
              if ($this->m_postvars["atksearchmode"][$p_attrib->m_name]==$searchModes[$i]||($this->m_postvars["atksearchmode"][$p_attrib->m_name]==""&&$searchModes[$i]=="substring")) $selected="selected";
              $sel.= '<option value="'.$searchModes[$i].'" '.$selected.'>'.text("search_".$searchModes[$i]);
            }
            $sel.= '</select>';
          }
          $form.=$g_layout->ret_td($sel);
          $form.='</tr>';
        }

      }

      $form.='</table>';

      return $form;
    }

    /**
     * Creates a navigation bar, for browsing through the record pages
     * (if a limit is set, and there are more records)
     * @return a HTML string for navigating through records
     */
    function buildNavigation($mode="")
    {
      global $PHP_SELF;
      $limit = (int)$this->m_postvars['atklimit'];
      $count = (int)$this->countDb($this->m_postvars['atkfilter'], $this->m_listExcludes, "", $mode);

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
        $nav = href($PHP_SELF.'?atkstartat='.$newstart,text('previous'))."&nbsp;|&nbsp;";
      }

      for ($i = $begpg; $i <= $endpg; $i++)
      {
        $nav .= ($i == $curr) ? "<b>$i</b>" : href($PHP_SELF.'?atkstartat='.max(0, ($i-1) * $limit),"$i");
        if ($i != $endpg) $nav .= "&nbsp;|&nbsp;";
      }

      if ($curr < $pages)
      {
        $newstart = $this->m_postvars['atkstartat'] + $limit;;
        $nav .= "&nbsp;|&nbsp;".href($PHP_SELF.'?atkstartat='.$newstart,text('next'));
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
      global $PHP_SELF;

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
        $nav.=href($PHP_SELF."?atksearch[".$this->m_index."]=".$char."*&atksearchmode[".$this->m_index."]=wildcard",$label);

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
    function buildTabs()
    {
      global $PHP_SELF, $g_layout;

      $result = "";

      // which tab is currently selected
      $tab = $this->getActiveTab();

      // build navigator
      if ($this->m_action == "add") $list = &$this->m_tabListAdd;
      elseif ($this->m_action == "edit") $list = &$this->m_tabListEdit;
      else $list = &$this->m_tabListView;
      foreach ($list as $t)
      {
        $title = text("tab_$t",$this->m_type);
        $url = $PHP_SELF."?atknodetype=".$this->atkNodeType()."&atkaction=".$this->m_action."&atktab=".$t;
        if ($this->m_action == "view") $link = session_url($url, SESSION_DEFAULT);
        else $link = "javascript:atkSubmit('".atkurlencode(session_url($url, SESSION_DEFAULT))."')";
        $result .= $g_layout->ret_tab($title, $link, ($t == $tab));
      }

      return $result;
    }

    /**
     * Admin page displays records and the actions that can be performed on
     * them (edit, delete)
     */
    function adminPage($actions="")
    {
      global $g_securityManager, $g_layout, $PHP_SELF, $atksearch,$config_atkroot;

      $g_layout->ui_top($g_layout->title($this->m_module, $this->m_type,'admin'));
      $g_layout->output($this->statusbar());

      $adminHeader = $this->adminHeader();
      if ($adminHeader!="")
      {
        $g_layout->output($adminHeader."<br><br>");
      }

      // When there's a lot of data, records will be spread across multiple
      // pages.
      if ($this->m_postvars['atklimit']=="") $this->m_postvars['atklimit']=atkconfig("recordsperpage");

      if ($this->m_postvars['atkstartat']=="" || isset($atksearch)) $this->m_postvars['atkstartat']=0;

      $recordset = $this->selectDb($this->m_postvars['atkfilter'],
                                   $this->m_postvars['atkorderby'],
                                   array("offset" => $this->m_postvars['atkstartat'], "limit" => $this->m_postvars['atklimit']),
                                   $this->m_listExcludes,
                                   "",
                                   "admin");

      if ($this->hasFlag(NF_ADD_LINK) && !$this->hasFlag(NF_NO_ADD) && $this->allowed("add"))
      {
        $addurl = $PHP_SELF.'?atknodetype='.$this->atknodetype().'&atkaction=add';
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
        $g_layout->output(href($addurl,$label,SESSION_NESTED).'<br><br>');
      }

      // create navigation bar
      if ($this->m_index != "") $g_layout->output($this->buildIndex($recordset[0][$this->m_index]).'<br><br>');
      $nav = $this->buildNavigation("admin");
      if (!empty($nav)) $g_layout->output("$nav<br>");
      $g_layout->output('<br>');

      if(!is_array($actions))
      {
        $actions = array();

        if (!$this->hasFlag(NF_NO_EDIT)&&$this->allowed("edit"))
        {
          $actions["edit"] = $PHP_SELF.'?atknodetype='.$this->atknodetype().'&atkaction=edit&atkselector=[pk]';
        }
        else
        {
          // if you may not edit, maybe you are allowed to view..
          if (!$this->hasFlag(NF_NO_VIEW)&&$this->allowed("view"))
          {
            $actions["view"] = $PHP_SELF.'?atknodetype='.$this->atknodetype().'&atkaction=view&atkselector=[pk]';
          }
        }
        if (!$this->hasFlag(NF_NO_DELETE)&&$this->allowed("delete"))
        {
          $actions["delete"] = $PHP_SELF.'?atknodetype='.$this->atknodetype().'&atkaction=delete&atkselector=[pk]';
        }
        if($this->hasFlag(NF_COPY)&&$this->allowed("copy"))
        {
          $actions["copy"] = $PHP_SELF.'?atknodetype='.$this->atknodetype().'&atkaction=copy&atkselector=[pk]';
        }
      }

      $g_layout->output($this->recordList($recordset, $actions));

      if (!empty($nav)) $g_layout->output('<br>'.$nav);
      $g_layout->output('<br><br>');

      $g_layout->ui_bottom();
    }


    /**
     * Creates recordlist
     * @param $actions is an array of actions..
     * @param $sortable
     * @param $suppresslist
     */
    function recordList($recordset, $actions, $sortable=true,$suppressList="", $sessionsave=false)
    {
      global $g_layout, $PHP_SELF,$g_theme;
      $output = $g_layout->data_top();

      $output.="<tr>";

      // stuff for the totals row..
      $totalisable = false;
      $totals = array();

      $orientation  = atkconfig("recordlist_orientation");

      // Vertical orientation for td contents
      if (trim(atkconfig("recordlist_vorientation")) != "")
      {
       $vorientation = "valign='".atkconfig("recordlist_vorientation")."'";
      }
      else
      {
       $vorientation = "valign='top'";
      }

      // display a headerrow with titles. each attributetitle is clickable to
      // sort the list by that attribute.
      // Since we are looping the attriblist anyway, we also check if there
      // are totalisable collumns.
      for($i=0;$i<count($this->m_attribIndexList);$i++)
      {
        $attribname = $this->m_attribIndexList[$i]["name"];
        $p_attrib = &$this->m_attribList[$attribname];
        //$p_attrib = &$this->m_attribList[$attribname];
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
          $neworder=$this->m_table.".".$p_attrib->fieldName();
				 if ($neworder==$this->m_postvars["atkorderby"])
				 {
					 // we were already sorting, so now we sort the other way
					   $neworder.=" DESC";
					 }

          if ($sortable && !$p_attrib->hasFlag(AF_NOSORT))
          {
            $tmp = href($PHP_SELF.'?atkorderby='.rawurlencode($neworder),text($p_attrib->fieldName(),$this->m_type));
          }
          else $tmp = text($p_attrib->fieldName(),$this->m_type);

          $headerRow.=$g_layout->ret_td_datatitle($tmp);

          // the totalisable check..
          if ($p_attrib->hasFlag(AF_TOTAL))
          {
            $totalisable = true;
          }

        }
      }

      // Searchrow.. (do not display if NF_NO_SEARCH flag has been set,
      // or if we are in extended search mode (otherwise we have 2 search forms)
      if (!$this->hasFlag(NF_NO_SEARCH)&&$this->m_postvars["atkaction"]!="search")
      {
        $rand = rand(1,1000);

        // The above row was used to move the browserwindow to the bottom of the screen,
        // so a user doesn't have to scroll down after each search. But there's some stupid
        // thing in internet explorer that it doesn't post the form anymore after you've
        // used it once.
        $searchRow = '<tr><a name="searchform"><form action="'.$PHP_SELF.'?'.SID.'" method="get">';
        $searchRow.= '<input type="hidden" name="atknodetype" value="'.$this->atknodetype().'">';
        $searchRow.= session_form();

        $searchmode = $this->m_postvars["atksearchmode"];
        if ($searchmode=="") $searchmode = atkconfig("search_defaultmode");
        if(is_array($searchmode))
        {
          // Get the first one, because they are all the same for now
          // TODO: every attrib needs to get his own searchmode
          $searchRow.= '<input type="hidden" name="atksearchmode" value="'.$searchmode[0].'">';
        }
        else
        {
          $searchRow.= '<input type="hidden" name="atksearchmode" value="'.$searchmode.'">';
        }
        $searchable = false;

        $searchBtn = '<input type="submit" value="'.text("search").'">';
        if (!$this->hasFlag(NF_NO_EXTENDED_SEARCH))
        {
          $searchBtn.='<br>'.href($PHP_SELF."?atknodetype=".$this->atknodetype()."&atkaction=search&atksearchmode=".$searchmode,"(".text("search_extended").")",SESSION_NESTED);
        }
        $actioncol = $g_layout->ret_td_datatitle($searchBtn);

        $actioncol.= '<input type="hidden" name="atkaction" value="'.$this->m_action.'">';

        if ($orientation=="left"||$orientation=="both")
        {
          $searchRow.=$actioncol;
        }

        // Second loop.. this time for the search fields.
        for($i=0;$i<count($this->m_attribIndexList);$i++)
        {
          $attribname = $this->m_attribIndexList[$i]["name"];
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
            if ($p_attrib->hasFlag(AF_SEARCHABLE))
            {
              $searchable = true;
              $searchRow.=$g_layout->ret_td_datatitle($p_attrib->search($this->m_postvars['atksearch']));
            }
            else
            {
              $searchRow.=$g_layout->ret_td_datatitle();
            }
          }
        }

        if ($orientation=="right"||$orientation=="both")
        {
          $searchRow.=$actioncol;
        }
        //$searchRow.=$this->stickyForm();
        $searchRow.="</form></tr>";

      }

      // one empty title which comes on top of the 'edit/delete/...' column
      if (($orientation=="left"||$orientation=="both") && (count($actions)>0||$searchable))
      {
        $output.=$g_layout->ret_td_datatitle();
      }

      $output.=$headerRow;

      // one empty title which comes on top of the 'edit/delete/...' column
      if (($orientation=="right"||$orientation=="both") && (count($actions)>0||$searchable))
      {
        $output.=$g_layout->ret_td_datatitle();
      }

      $output.="</tr>";

      if ($searchable)
      {
        $this->setAttribSizes();
        $output.=$searchRow;
      }


      for ($i=0;$i<count($recordset);$i++)
      {
        // Look for custom record actions.
        $recordactions = $actions;
        $this->recordActions($recordset[$i], $recordactions);

        // Actions passed to the function.

        $actionCol="";
        if (count($recordactions)>0)
        {
          $stractions = "";
          $pk = $this->primaryKey($recordset[$i]);

          foreach($recordactions as $action_name => $action_url)
          {
            if ($action_url!="")
            {
              // dirty hack:
              $atkencoded = false;
              if (strpos($action_url,"_1")>0)
              {
                // normal value..
                $atkencoded = true;
              }
              $action=str_replace("%5B","[",$action_url);
              $action=str_replace("%5D","]",$action);

              $action=str_replace("_1"."5B","[",$action);
              $action=str_replace("_1"."5D","]",$action);

              if ($atkencoded)
              {
                $action=str_replace('[pk]',atkurlencode(rawurlencode($pk), false),$action);
              }
              else
              {
                $action=str_replace('[pk]',rawurlencode($pk),$action);
              }
              $action=stringparse($action,$recordset[$i],true);

              $stractions.=href($action,text($action_name),SESSION_NESTED, $sessionsave).'&nbsp;';
            }
          }
          $actionCol=$g_layout->ret_td($stractions, $vorientation);
        }
        else
        {
          if ($searchable)
          {
            $actionCol.=$g_layout->ret_td("&nbsp;");
          }
        }

        // special rowColor method makes it possible to change the row color based on the record data.
        // the method can return a simple value (which will be used for the normal row color), or can be
        // an array, in which case the first element will be the normal row color, and the second the mouseover
        // row color, example: function rowColor(&$record, $num) { return array('red', 'blue'); }
        $method = "rowColor";
        
        $bgh = $g_theme["RowMouseOver"];
        
        if (method_exists($this, $method))
        {
          $bgn = $this->$method(&$recordset[$i], $i);
          if (is_array($bgn))
          {
            $bgh = $bgn[1];
            $bgn = $bgn[0];
          }
        }        

        // alternate colors of rows
        $output.='<tr class="row'.(($i%2==0)?1:2).'" '.(empty($bgn) ? '' : 'style="background-color:'.$bgn.'" ').
                 '    onmouseover="if (typeof(this.style) != \'undefined\') this.style.backgroundColor = \''.$bgh.'\'"'.
                 '    onmouseout="if (typeof(this.style) != \'undefined\') this.style.backgroundColor = \''.(empty($bgn)?'':$bgn).'\'">';

        if ($orientation=="left"||$orientation=="both")
        {
          $output.=$actionCol;
        }

        for($r=0;$r<(count($this->m_attribIndexList));$r++)
        {
          $attribname = $this->m_attribIndexList[$r]["name"];
          $p_attrib = &$this->m_attribList[$attribname];
          //echo $r."/".(count($this->m_attribIndexList)-1)." - ".$this->m_attribIndexList[$r]["name"]."<br>";
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
              $output.=$g_layout->ret_td($this->$funcname($recordset[$i], "list"), $vorientation);
            }
            else
            {
              // otherwise, the display function of the particular attribute
              // is called.
              $output.=$g_layout->ret_td($p_attrib->display($recordset[$i], "list"), $vorientation);
            }

            // Calculate totals..
            if ($p_attrib->hasFlag(AF_TOTAL))
            {
              $totals[$attribname] = $p_attrib->sum($totals[$attribname], $recordset[$i]);
            }
          }
        }

        if ($orientation=="right"||$orientation=="both")
        {
          $output.=$actionCol;
        }

        $output.='</tr>';
      }

      // totalrow..
      if ($totalisable)
      {
        $totalRow = '<tr>';

        // one empty title which comes below the 'edit/delete/...' column
        if (($orientation=="left"||$orientation=="both")&&(count($actions)>0||$searchable))
        {
          $totalRow.=$g_layout->ret_td_datatitle();
        }

        // Third loop.. this time for the totals row.
        for($i=0;$i<count($this->m_attribIndexList);$i++)
        {
          $attribname = $this->m_attribIndexList[$i]["name"];
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
              $totalRow.=$g_layout->ret_td_datatitle($p_attrib->display($totals[$attribname], "list"));
            }
            else
            {
              $totalRow.=$g_layout->ret_td_datatitle();
            }
          }
        }

        // one empty title which comes below the 'edit/delete/...' column
        if (($orientation=="right"||$orientation=="both")&&(count($actions)>0||$searchable))
        {
          $totalRow.=$g_layout->ret_td_datatitle();
        }

        $totalRow.="</tr>";

        $output.=$totalRow;
      }

      $output.=$g_layout->data_bottom();

      return $output;

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

      for ($i=0;$i<count($recordset);$i++)
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

     * @param $sol      -- start row/record/line
     * @param $eof      -- end field
     * @param $sof      -- start field
     * @param $eol      -- end record/line
     * @param $type     -- 0=in simple table ; 1= export
	 *@param $compression -- Compression technique (bzip / gzip)
  */
   function specialRecordList($recordset, $sol, $sof, $eof, $eol, $type="0", $compression="",$suppressList="")
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

      for ($i=0;$i<count($recordset);$i++)
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

        $this->export2File($output,"achievo","csv","csv",$compression);
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
      global $g_layout, $atksearch;

      // When there's a lot of data, records will be spread across multiple pages.
      if ($this->m_postvars['atklimit']=="") $this->m_postvars['atklimit']=atkconfig("recordsperpage");
      if ($this->m_postvars['atkstartat']=="" || isset($atksearch)) $this->m_postvars['atkstartat']=0;

      $recordset = $this->selectDb($this->m_postvars['atkfilter'],
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
        $this->redirect($target);
      }
      else
      {
        $g_layout->ui_top($g_layout->title($this->m_module, $this->m_type,'select'));
        $g_layout->output($this->statusbar());
        $g_layout->output(text($this->m_type.'_select').'<br>');
        $g_layout->output('<br>');

        if ($this->m_index != "") $g_layout->output($this->buildIndex($recordset[0][$this->m_index]).'<br><br>');

        // create navigation bar
        $nav = $this->buildNavigation("select");
        if (!empty($nav)) $g_layout->output("$nav<br>");
        $g_layout->output('<br>');

        $actions["select"]=atkurldecode($this->m_postvars['atktarget']);

        $g_layout->output($this->recordList($recordset, $actions));

        if (!empty($nav)) $g_layout->output('<br>'.$nav.'<br>');
        $g_layout->output('<br>');

        $g_layout->ui_bottom();
      }

    }

    /**
     * Function outputs a page in which the user is asked if he really wants.
     * to delete the record.
     * @param $atkselector Selected record you want to delete
     */
    function confirmDelete($atkselector)
    {
      global $g_layout, $PHP_SELF;

      $g_layout->ui_top($g_layout->title($this->m_module, $this->m_type,'delete'));
      $g_layout->output($this->statusbar());

      $g_layout->output('<form action="'.$PHP_SELF.'?"'.SID.' method="post">');
      $g_layout->output(session_form());
      $g_layout->output('<input type="hidden" name="atkaction" value="delete">');
      $g_layout->output('<input type="hidden" name="atknodetype" value="'.$this->atknodetype().'">');
      $g_layout->output('<input type="hidden" name="atkselector" value="'.$atkselector.'">');

      $g_layout->table_simple();

      $g_layout->output('<tr>');
      $g_layout->td(text('confirm_delete'));
      $g_layout->output('</tr><tr>');
      $g_layout->td('&nbsp;<input name="confirm" type="submit" value="'.text('yes').'"><input name="cancel" type="submit" value="'.text('no').'">','colspan="2"');
      $g_layout->output('</tr></table></form>');

      $g_layout->ui_bottom();
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
      for ($i=0;$i<count($g_modifiers[$this->m_type]);$i++)
      {
        $modname = $g_modifiers[$this->atknodetype()][$i];
        $mod = getModule($modname);
        $modifiername = $this->m_type."_modifier";

        if (method_exists($mod,$modifiername))
        {
          atkdebug("Applying modifier from module $modname to ".$this->m_type);
          $mod->$modifiername(&$this);
        }
      }
      // See if there are modules active that modify this module.node..
      // for example, modules modifying 'finance.bill'
      for ($i=0;$i<count($g_modifiers[$this->atknodetype()]);$i++)
      {
        $modname = $g_modifiers[$this->atknodetype()][$i];
        $mod = getModule($modname);
        $modifiername = str_replace(".","_",$this->atknodetype())."_modifier";

        if (method_exists($mod,$modifiername))
        {
          atkdebug("Applying modifier from module $modname to ".$this->atknodetype());
          $mod->$modifiername(&$this);
        }
      }

      //atk_var_dump($this->m_attribIndexList);
      usort($this->m_attribIndexList,array("atknode","attrib_cmp"));
      //atk_var_dump($this->m_attribIndexList);
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
     */
    function dispatch($postvars)
    {
      global $g_securityManager, $g_db, $g_sessionManager, $g_layout;
      $this->m_postvars = $postvars;

      $this->m_action = $postvars['atkaction'];

      /** DIRTY HACK
          if action is something else than edit, we clear the atkformdata sessionstuff **/
      if ($this->m_action!="edit"&&$this->m_action!="add"&&$this->m_action!="admin")
      {
        atkdebug("Clearing stored formdata");
        $this->m_postvars["atkformdata"]="";
      }

      // check for fake submit (session saving href thingee)
      if ($this->m_postvars["atkescape"]!="")
      {
        $this->redirect(atkurldecode($this->m_postvars["atkescape"]));
      }
      else
      {
        if ($this->allowed($this->m_action))
        {
          $g_securityManager->logAction($this->m_type, $this->m_action);
          $this->callHandler($this->m_action);
        }
        else
        {
          $g_layout->output("Sorry, you don't have permission to perform this action");
        }

        $g_layout->page(text('app_shorttitle')." - ".$g_layout->title($this->m_module, $this->m_type,$this->m_postvars['atkaction']));

        // This is the end of all things for this page..
        // so we clean up some resources..
      }
      $g_db->disconnect();
      atkdebug("disconnected from the database");
    }

    /*** The node actions ***/

    function action_view()
    {
      atkdebug("Action view");
      $recordset = $this->selectDb($this->m_postvars['atkselector'],"","",$this->m_viewExcludes,"","view");
      $this->viewPage($recordset[0]);
    }

    function action_add()
    {
      atkdebug("Action add");
      $this->addPage();
    }

    function action_update()
    {
      if ($this->m_postvars['atkcancel']=="")
      {
        $record = $this->updateRecord();
        $this->validate($record, "update");
        if (count($record['atkerror'])>0)
        {
          $this->m_action="edit";
          $this->editPage($record);
        }
        else
        {
          $this->updateDb($record);
          if ($this->m_postvars['atknoclose']=="")
          {
            // 'save and close' was clicked
            $this->redirect();
          }
          else
          {
            // 'save' was clicked
            $this->m_action="edit";
            //update succesful, pk value might be changed so update m_orgkey
            $record["atkprimkey"] = $this->primaryKey($record);
            //$this->setOrgKeyValue($record);
            $this->editPage($record);
          }
        }
      }
      else
      {
        // Cancel was pressed
        $this->redirect();
      }
    }

    function action_save()
    {
      global $PHP_SELF;
      if ($this->m_postvars['atkcancel']=="")
      {
        $record = $this->updateRecord();
        $this->validate($record, "add");
        if (count($record['atkerror'])>0)
        {
          $this->m_action="add";
          $this->addPage($record);
        }
        else
        {
          $this->addDb($record);

          $location="";
          if ($this->hasFlag(NF_EDITAFTERADD))
          {
            $location = session_url($PHP_SELF.'?atknodetype='.$this->atknodetype().'&atkaction=edit&atkselector='.rawurlencode($this->primaryKey($record)), SESSION_REPLACE);
          }
          $this->redirect($location);
        }
      }
      else
      {
        // Cancel was pressed
        $this->redirect();
      }
    }

    function action_delete()
    {
      if ($this->m_postvars['confirm']==text('yes'))
      {
        // Confirmation page was displayed and 'yes' was clicked
        $recordset = $this->deleteDb($this->m_postvars['atkselector']);
        $this->redirect();
      }
      else if ($this->m_postvars['cancel']!=text('no'))
      {
        // Confirmation page was not displayed
        $this->confirmDelete($this->m_postvars['atkselector']);
      }
      else
      {
        // Confirmation page was displayed and 'no' was clicked
        $this->redirect();
      }
    }

    function action_copy()
    {
      $recordset = $this->selectDb($this->m_postvars['atkselector'],"","","","","copy");
      if(count($recordset)>0)
      {
        $this->copyDb($recordset[0]);
      }
      else
      {
        atkdebug(" Geen records gevonden met Selector: $selector");
      }

      $this->redirect();
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
        for ($i=0;$i<count($recordset);$i++)
        {
          $this->xml($recordset[$i]);
        }
      }
    }

    function action_admin()
    {
      if ($this->hasFlag(NF_NO_ADD)==false&&$this->allowed("add"))
      {
        if (!$this->hasFlag(NF_ADD_LINK)) // otherwise, in adminPage, an add link will be added.
        {
          $this->addPage();
        }
      }
      $this->adminPage();
    }

    function action_search()
    {
      $this->searchPage($this->m_postvars["atksearch"]);
    }

    function action_edit()
    {
      $recordset = $this->selectDb($this->m_postvars['atkselector'],"","","","","edit");

      $this->editPage($recordset[0]);
    }

    function action_select()
    {
      $this->selectPage();
    }

    /**
     * Make browser of the user go to another page. This should be called before any call
     * to layout::outputFlush();
     */
    function redirect($location="")
    {
      global $g_returnurl, $PHP_SELF;

      atkdebug("atknode::redirect()");

      if ($g_returnurl!="") $location = $g_returnurl;

      if ($location=="")
      {
        $location = session_url($PHP_SELF,SESSION_BACK,1);
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
      global $g_layout;

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
        $g_layout->output(htmlspecialchars($xml).'<br>');
      }
      else
      {
        $g_layout->rawoutput($xml);
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

//        preg_match_all('/\[\w+\]/', $descriptordef, $fields);
  //      var_dump($fields);

        // parse fields from descriptordef
        $fields = stringfields($descriptordef);
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
     */
    function validate(&$record, $mode)
    {
      global $g_db;
      atkdebug("validate()");

      foreach (array_keys($this->m_attribList) as $attribname)
      {
        $p_attrib = &$this->m_attribList[$attribname];
        //Validate if ne
        if ($p_attrib->hasFlag(AF_PRIMARY) && !$p_attrib->hasFlag(AF_AUTO_INCREMENT))
        {
        //  $atkorgkey = $this->orgKey();
          $atkorgkey = $record["atkprimkey"];
          if(($atkorgkey == '' // no orgkey, so adding this record
              || $atkorgkey != $this->primaryKey($record)) // key has changed, so check is necessary
             &&  count($g_db->getrows("SELECT ".$p_attrib->fieldName()." FROM ".$this->m_table." WHERE ".$this->primaryKey($record)))>0
            )
          {
            triggerError(&$record, $attribname, 'error_primarykey_exists');
          }
        }

        // if no root elements may be added to the tree, then every record needs to have a parent!
        if ($p_attrib->hasFlag(AF_PARENT) && $this->hasFlag(NF_TREE_NO_ROOT_ADD) && $this->m_action == "save")
          $p_attrib->m_flags |= AF_OBLIGATORY;

        // validate obligatory fields (but not the auto_increment ones, because they don't have a value yet)
        if ($p_attrib->hasFlag(AF_OBLIGATORY) && !$p_attrib->hasFlag(AF_AUTO_INCREMENT) && $p_attrib->isEmpty($record))
        {
          triggerError(&$record, $attribname, 'error_obligatoryfield');
        }
        // if flag is primary
        else if ($p_attrib->hasFlag(AF_UNIQUE) && !$p_attrib->hasFlag(AF_PRIMARY) &&
                 count($g_db->getrows("SELECT ".$p_attrib->fieldName()." FROM ".$this->m_table." WHERE $attribname='".escapeSQL($this->m_postvars[$attribname])."' AND NOT (".$this->primaryKey($record).")"))>0
                )
        {
          triggerError(&$record, $attribname, 'error_uniquefield');
        }

        else
        {
          //don't validate empty fields
          if (!( $p_attrib->isEmpty($record)))
          {
            $funcname = $p_attrib->m_name."_validate";

            if (method_exists($this,$funcname))
            {
              $this->$funcname(&$record, $mode);
            }
            else
            {
              $p_attrib->validate(&$record, $mode);
            }
          }
        }
      }
    }

    /**
     * Update a record in the database
     * Note: for new records use addDb()).
     */
    function updateDb(&$record)
    {
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
            if (method_exists($p_attrib,"store"))
            {
              $storelist[]=$attribname;
            }
            else
            {
              $p_attrib->addToQuery($query,$this->m_table,"",$record,1,"update"); // start at level 1
            }
	        }
        }

        $querystring = $query->buildUpdate();

        $g_db->query($querystring);

        // also store special storage attributes.
        for ($i=0;$i<count($storelist);$i++)
        {
          $p_attrib = &$this->m_attribList[$storelist[$i]];
          $p_attrib->store($g_db, $record,"update");
        }

        // Now we call a postUpdate function, that can be used to do some processing after the record
        // has been saved.
        $this->postUpdate($record);
      }
      else
      {
        atkdebug("NOT UPDATING! NO SELECTOR SET!");
      }

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
        for ($i=0;$i<count($this->m_fuzzyFilters);$i++)
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
    function copyDb($record)
    {
      $this->preCopy($record);
      $this->addDb($record);
      return "";
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
        for ($i=0;$i<count($this->m_fuzzyFilters);$i++)
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

      if ($order!="") $query->addOrderBy($order);
      $querystring = $query->buildSelect();

      if (is_array($limit) && count($limit) == 2) $dbrecords = $g_db->getrows($querystring, $limit["offset"], $limit["limit"]);
      else $dbrecords = $g_db->getrows($querystring);

//      $this->setOrgKeyValue($dbrecords[0]);

      $recordset = array();
      for ($i=0;$i<count($dbrecords);$i++)
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

      for ($i=0;$i<count($recordset);$i++)
      {
        for ($j=0;$j<count($loadlist);$j++)
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
      }

      for ($i=0;$i<count($usedFields);$i++)
      {
        $p_attrib = &$this->m_attribList[$usedFields[$i]];
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
            atkdebug($usedFields[$i]." is not an object?! Check your descriptor_def for non-existant fields");
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
     */
    function addDb(&$record)
    {
		 //var_dump($record);
      global $g_db;

      $name = "atk".atkconfig("database")."query";
      $query = new $name();

      $storelist = array();
      $querylist = array();

      $query->addTable($this->m_table);

      foreach (array_keys($this->m_attribList) as $attribname)
      {
        $p_attrib = &$this->m_attribList[$attribname];
        if (method_exists($p_attrib,"store"))
        {
          $storelist[]=$attribname;
        }
        else
        {
          $querylist[]=$attribname;
        }
      }


      for($i=0;$i<count($querylist);$i++)
      {
        $p_attrib = &$this->m_attribList[$querylist[$i]];
        if ($p_attrib->hasFlag(AF_AUTO_INCREMENT))
        {
          $record[$p_attrib->fieldName()]=$g_db->nextid($this->m_seq);
        }

        $p_attrib->addToQuery($query,$this->m_table,"",$record,1,"add"); // start at level 1
      }

      $querystring = $query->buildInsert();
      $g_db->query($querystring);

      // also store special storage attributes.
      for ($i=0;$i<count($storelist);$i++)
      {
        $p_attrib = &$this->m_attribList[$storelist[$i]];

        $p_attrib->store($g_db, $record,"add");

      }


      // Now we call a postAdd function, that can be used to do some processing after the record
      // has been saved.
      $this->postAdd($record);

      atkdebug($g_db->m_error);
    }

    /**
     * delete record from the database.
     * todo: instead of delete, set the deleted flag.
     * @param $selector Selector
     * @returns the records that were deleted
     */
    function deleteDb($selector)
    {
      global $g_db;

      $recordset = $this->selectDb($selector,"","","","","delete");
      if (count($this->m_cascadingAttribs)>0)
      {
        for ($i=0;$i<count($recordset);$i++)
        {
          for ($j=0;$j<count($this->m_cascadingAttribs);$j++)
          {
            $p_attrib = &$this->m_attribList[$this->m_cascadingAttribs[$j]];
            $p_attrib->delete($recordset[$i]);
          }
        }
      }

      $query = "DELETE FROM ".$this->m_table." WHERE ".$selector;
      $g_db->query($query);
      // todo: instead of delete, set the deleted flag.

      for ($i=0;$i<count($recordset);$i++)
      {
        $this->postDel($recordset[$i]);
      }

      return $recordset;
    }

    /**
     * Function that is called right after a new record has been saved to the
     * database. This function does essentially nothing, but it can be
     * overriden in your derived classes if you want to do something special
     * after you saved a record.
     *
     * @param $record The record that has just been saved.
     */
    function postAdd($record)
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
      // Do nothing
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
    function recordActions($record, &$actions)
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

    function allowed($action)
    {
      global $g_securityManager;
      if (empty($this->m_securityAlias))
      {
        $alias = $this->atknodetype();
      }
      else
      {
        $alias = $this->m_securityAlias;
      }

      return ($this->hasFlag(NF_NO_SECURITY)
              ||in_array($this->securityKey($action), $this->m_unsecuredActions)
              || $g_securityManager->allowed($alias,$this->securityKey($action)));
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
     */
    function statusbar()
    {
      global $g_sessionManager, $g_layout, $config_atkroot;
      if (!$this->m_statusbarDone)
      {
        $this->m_statusbarDone = true;
        if (atkconfig("stacktrace"))
        {
          $statusbar.= $g_layout->ret_td('<i>'.$g_sessionManager->stackTrace().'</i>','align="left"');
        }

        $helpurl = $this->helpUrl();
        if ($helpurl!="")
        {
          $g_layout->register_script($config_atkroot."atk/javascript/newwindow.js");
          $statusbar.= $g_layout->ret_td('<i><a href="'.$helpurl.'">'.text("help").'</a></i>','align="right"');
        }

        if ($statusbar!="") return $g_layout->ret_table_simple(0,true).'<tr>'.$statusbar.'</tr></table><br>';
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
       return atkPopup('help.php','node='.$node,$name,650,650,'yes','no');
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
  }
?>

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
  define("NF_COPY"          ,256); // Records / trees may  be copied
  define("NF_TREE_NO_ROOT_DELETE",512); // No root elements can be deleted
  define("NF_TREE_NO_ROOT_COPY",1024); // No root elements can be copied
    
  define("NF_AUTOSELECT",2048); // If this flag is set and only one record is 
                                // present on a selectpage, atk automatically 
                                // selects it and moves on to the target.
  define("NF_TRACK_CHANGES",4096); // If set, atk stores the old values of 
                                  // a record as ["atkold"] in the $rec that
                                  // gets passed to the postUpdate
  define("NF_NO_SECURITY",8192);  // Quick way to disable accessright checking
                                  // for an entire node. (Everybody may access
                                  // this node
  define("NF_NO_EXTENDED_SEARCH",16384); // Extended search feature is turned 
                                         // off.

  /**
   * The atkNode class represents a piece of information that
   * is part of an application. This class provides standard
   * functionality for adding, editing and deleting nodes.
   * This class must be seen as an abstract base class: For
   * every piece of information in an application, a class
   * must be derived from this class with specific
   * implementations for that type of node.
   *
   * <b>Todo's</b>   : - Incorporate some metadata about a node as in Stefan
   *  	                 Niederhauser's node-code. (like creation date etc.)
   *                   - Authorization: who is allowed to do what on which node
   *                   - Install function. And a script that you can use to
   *                     install new nodetypes in your achievo. The install
   *                     function would typically create a table and register
   *                     itself somewhere in the menu.
   *                   - Lots of other small things need to be done. Search for
   *                     'todo' in the code :)
   *
   * @author Ivo Jansch (ivo@achievo.com)
   * @version $Revision$
   *
   * $Id$
   * $Log$
   * Revision 4.84  2001/07/19 11:35:04  ivo
   * Formsession save now works for action==admin
   *
   * Revision 4.83  2001/07/18 16:08:18  ivo
   * $this->addAttribute() now has a shorter alias: $this->add()
   *
   * Revision 4.82  2001/07/18 12:10:10  ivo
   * New flags: AF_READONLY_ADD and AF_READONLY_EDIT (AF_READONLY automatically
   * selects both flags)
   *
   * Revision 4.81  2001/07/17 09:51:31  ivo
   * Replaced all dispatch.php occurrences with $PHP_SELF.
   *
   * Revision 4.80  2001/07/15 18:10:54  ivo
   * alphabetical index now also available in selectPage.
   *
   * Revision 4.79  2001/07/15 17:03:59  ivo
   * New feature: Alphabetical index on top of recordlists.
   * (use e.g. $this->setIndex("name"); in your class constructor to activate)
   * Fixed some bugs in extended search.
   *
   * Revision 4.78  2001/07/15 16:37:19  ivo
   * New atk.inc includefile in skel.
   * New feature: extended search.
   * Fixed a bug in session management; style.php and other files that get
   * loaded between two dispatches could corrupt a session.
   *
   * Revision 4.77  2001/07/11 21:14:30  ivo
   * Added a global atk version number.
   *
   * Revision 4.74  2001/07/04 10:50:27  ivo
   * Fix to help feature (help.php moved to skel)
   *
   * Revision 4.73  2001/07/04 07:17:10  ivo
   * searchform now uses method="get"
   *
   * Revision 4.72  2001/07/03 16:07:13  ivo
   * the *_display override of readonly attribs was not called.
   *
   * Revision 4.71  2001/07/03 15:49:14  maurice
   * The validate function of an attribute is only called when a field is not empty.
   *
   * Revision 4.70  2001/07/01 10:09:52  ivo
   * Replaced the clickheretoadd_prefix/postfix string by link_<nodetype>_add.
   * If you upgrade, please verify your language files!
   *
   * Revision 4.69  2001/06/29 13:57:20  ivo
   * Bugfix in selectPage [pk] parser.
   *
   * Revision 4.68  2001/06/29 13:12:10  ivo
   * New feature: integrated contextsensitive help (contributed by Rene)
   *
   * Revision 4.67  2001/06/29 11:26:59  ivo
   * Patches to oracle support.
   *
   * Revision 4.66  2001/06/29 09:08:19  ivo
   * "Field is required" msg now only appears if there actually are required
   * fields.
   *
   * Revision 4.65  2001/06/27 15:25:42  peter
   * A lot of fixes to the oci8 support, including better outer joins, and
   * limitting queries! Fields now have an asterix behind them if they are
   * required. Some other small enhancements.
   *
   * Revision 4.64  2001/06/27 09:35:32  peter
   * Fixed bug with searching and browsing through records which sometimes
   * prevented ATK from showing search results.
   *
   * Revision 4.63  2001/06/26 07:36:52  martin
   * number of posts to dispatch.php changed to phpself.
   *
   * Revision 4.62  2001/06/23 22:52:35  martin
   * changed hardcoded dispatch.php to $PHP_SELF in navigationbar
   *
   * Revision 4.61  2001/06/22 15:34:44  ivo
   * Dirty hack to atkurlencode [pk]'s once they have been replaced.
   * TODO: clean up!
   *   
   * (older logs have been removed...)
   */
   
  // The atk version number. (REMEMBER: update this whenever a new atk release is tagged stable!)
  $g_atkversion = "4.2";

  // Global theme variable, must be declared before the includes
  $g_theme = array();
  // Global array to store meta-information about tables, so we don't have to read them from the db for each class instance.
  $g_tableMeta = array();
  
  // Global node list
  $g_nodes = array();
  // Global module list
  $g_modules = array();
  // Global menu
  $g_menu = array();    
  
  require_once($config_atkroot."atk/atkconfigtools.inc");
  require_once($config_atkroot."atk/defaultconfig.inc.php");

  include_once($config_atkroot."config.inc.php");
  require_once($config_atkroot."atk/atktools.inc");
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

  // Global database..
  $atkdb = "atk".$config_database."db";
  $g_db = new $atkdb();

  $g_db->m_database = $config_databasename;
  $g_db->m_user     = $config_databaseuser;
  $g_db->m_password = $config_databasepassword;
  $g_db->m_host     = $config_databasehost;
  $g_db->m_debug    = 0;
  
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

   $g_pageVars = array("atksearch","atksearchmode","atkorderby","atkstartat","atktarget","atkformdata","atktree");

  /************* MODULE HANDLING *************/

  // Auto-register modules
  if (file_exists("$config_module_path/config.modules.inc"))
    include_once("$config_module_path/config.modules.inc");
  
  // Module specific language file
  for ($i = 0; $i < sizeof($g_modules); $i++)
  {
    $filename = "$config_module_path/".$g_modules[$i]."/languages/$config_languagefile";
    if (file_exists($filename))
    {
      include_once($filename);
    }
  }
  
  // Module attributes (do not move, needs database!)
  for ($i = 0; $i < sizeof($g_modules); $i++)
  {
    $module = new $g_modules[$i]();
    $attr_arr = $module->getAttributes();
    for ($j = 0; $j < sizeof($attr_arr); $j++)
    {
      if (file_exists("$config_module_path/".$g_modules[$i]."/".$attr_arr[$j]))
      include_once("$config_module_path/".$g_modules[$i]."/".$attr_arr[$j]);
    }
  }
  
  /*******************************************/  

  // At some places we need a random number generator, so we seed the generator here.
  srand ((double) microtime() * 1000000);

  // Since atk pages are always dynamic, we have to prevent that some browsers cache
  // the pages
  header ("Expires: Mon, 26 Jul 1997 05:00:00 GMT");    // Date in the past
  header ("Last-Modified: " . gmdate("D, d M Y H:i:s") . " GMT"); // always modified
  header ("Cache-Control: no-cache, must-revalidate");  // HTTP/1.1
  header ("Pragma: no-cache");                          // HTTP/1.0
  
  // Set the content type.
  header ("Content-Type: text/html; charset=".text('charset')); // The character set (defined
                                                                // in the language files
                                                                

  // The atk node class
  class atkNode
  {

    /*** Member variables ***/

    /**
     * This array will hold the data of records that are read from the database.
     * NOTE: USE OF THIS VARIABLE IS OBSOLETE
     */
    var $m_records = array();

    /**
     * Pointer to the current record.
     * NOTE: USE OF THIS VARIABLE IS OBSOLETE
     */
    var $m_currentRec = 0;

    /**
     * The list of attributes of a node. These should be of the class atkAtribute
     * or one of its derivatives.
     */
    var $m_attribList = array();

    /**
     * The list of relations with other nodes. These should be of the class
     * atkRelation or one of its derivatives.
     */
    var $m_relationList = array();

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
     * Array containing the metadata of the table (with fieldname, type and length)
     */
    var $m_tableMeta = array();    

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
    var $m_errors = array();

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
     * parent Attribute flag (treeview)
     */
     var $m_parent;

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
      global $g_layout;

      atkdebug('atkNode::atkNode('.$type.')');
      $this->m_type = $type;
      $this->m_flags = $flags;      
    }
    
    /**
     * Add an atkAttribute to the node ($attribute should be an object of type
     * atkAttribute or one of its derivatives)
     * @param $attribute the attribute you want to add
     */
    function add($attribute)
    {
      global $g_securityManager;

      $attribute->m_owner = $this->m_type;
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
        $this->m_primaryKey[] = $attribute->fieldName();
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

      // check for title fieldname (treeview)
      if($attribute->hasFlag(AF_TITLE))
      {
        $this->m_title = $attribute->fieldName();
      }

      if ($attribute->hasFlag(AF_HIDE_LIST)&&!$attribute->hasFlag(AF_PRIMARY))
      {
        $this->m_listExcludes[]=$attribute->fieldName();
      }

      // Speed optimization, we remember which attributes are multilangual at add mode
      if ($attribute->hasFlag(AF_MULTILANGUAGE)&&!$attribute->hasFlag(AF_HIDE_ADD))
      {
        $this->m_listMlAdd[]=$attribute->fieldName();
      }

      // Speed optimization, we remember which attributes are multilangual at add mode
      if ($attribute->hasFlag(AF_MULTILANGUAGE)&&!$attribute->hasFlag(AF_HIDE_EDIT))
      {
        $this->m_listMlEdit[]=$attribute->fieldName();
      }

      // check for multilanguage attribute at add/edit mode
      if ($attribute->fieldName() == 'multilanguage_select' && !$attribute->hasFlag(AF_HIDE_ADD)) $this->m_hasMlSelectAdd = 1;
      if ($attribute->fieldName() == 'multilanguage_select' && !$attribute->hasFlag(AF_HIDE_EDIT)) $this->m_hasMlSelectEdit = 1;
      
      $attribute->init();            
      
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
      atkdebug('atkNode::setOrder('.$fields.')');

      $this->m_default_order = $fields;
    }


    /**
     * Set the table that the node should use. This should be called in the
     * constructor of the node-derived classes but AFTER the constructor of
     * the atkNode class itself is called.
     * @param $tablename The Tablename
     * @param $seq sequence
     */
    function setTable($tablename,$seq="node")
    {
      global $g_tableMeta, $g_db, $g_layout;

      $this->m_table      = $tablename;
      $this->m_seq        = $seq;

      if (!is_array($g_tableMeta[$tablename]))
      {
        // Get metainformation about the table
        $tmparr = $g_db->metadata($tablename);

        // Store the metadata in a more convenient format.
        for ($i=0;$i<count($tmparr);$i++)
        {
          $this->m_tableMeta[$tmparr[$i]['name']]['type'] = $tmparr[$i]['type'];
          $this->m_tableMeta[$tmparr[$i]['name']]['len'] = $tmparr[$i]['len'];
          $this->m_tableMeta[$tmparr[$i]['name']]['flags'] = $tmparr[$i]['flags'];
          $fieldname = $tmparr[$i]['name'];          
        }

        $g_tableMeta[$tablename] = $this->m_tableMeta;
      }

      $this->m_tableMeta = $g_tableMeta[$tablename];
      foreach (array_keys($this->m_attribList) as $attribname)
      {
        $p_attrib = &$this->m_attribList[$attribname];
        
        if (is_object($p_attrib))
        {
          $p_attrib->fetchMeta($this->m_tableMeta);
          $p_attrib->m_size = min($this->m_tableMeta[$attribname]['len'], $g_layout->maxInputSize());
          $p_attrib->m_searchsize = min($this->m_tableMeta[$attribname]['len'], $g_layout->searchSize());      
          $p_attrib->m_maxsize = $this->m_tableMeta[$attribname]['len'];                     
        }
      }
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
      global $g_layout, $atklevel, $PHP_SELF;
      $g_layout->register_script("atk/javascript/formfocus.js");      
      $g_layout->ui_top(text('title_'.$this->m_type.'_edit'));      
      $g_layout->output($this->statusbar());
      $g_layout->output('<form name="entryform" enctype="multipart/form-data" action="'.$PHP_SELF.'?'.SID.'"'.
                                   ' method="post"'.(sizeof($this->m_listMlEdit) > 0 ? ' onsubmit="submitSave(this)"' : '').'>');
      $g_layout->output(session_form());
      $forceList = decodeKeyValueSet($this->m_postvars['atkfilter']);
      $g_layout->output($this->editForm("edit",$record,$forceList));
      
      $g_layout->output('<br>');
      if ($atklevel>0) // if atklevel is 0 or less, we are at the bottom of the session stack, 
                       // which means that 'saveandclose' doesn't close anyway, so we leave out
                       // the 'saveandclose' button.
      {
        $g_layout->output('<input type="submit" value="'.text('saveandclose').'">');
      }
      $g_layout->output('<input type="submit" name="atknoclose" value="'.text('save').'">');
      $g_layout->output('<input type="submit" name="atkcancel" value="'.text('cancel').'">');
      $g_layout->output('</form>');
      $g_layout->ui_bottom();
      $g_layout->output('<SCRIPT LANGUAGE="JavaScript">placeFocus()</SCRIPT>');
    }
    
    /**
     * Creates an view (=readonly) page
     */
    function viewPage($record)
    {
      global $g_layout, $PHP_SELF;  
    
      $g_layout->ui_top(text('title_'.$this->m_type.'_view'));
      $g_layout->output($this->statusbar());

      $page.=$g_layout->ret_table_simple();

      // For all attributes we use the edit() function to display an
      // appropriate way to edit the data. This may be overridden by supplying
      // an <attributename>_edit function in the derived classes.
      foreach (array_keys($this->m_attribList) as $attribname)
      {
        $p_attrib = &$this->m_attribList[$attribname];
        
        if (!$p_attrib->hasFlag(AF_HIDE))
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
              
              $page.=$g_layout->ret_td(text($p_attrib->fieldName()).': ','valign="top"');
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
          if (method_exists($this, $funcname)) $editsrc = $this->$funcname($record);
          else $editsrc=$p_attrib->display($record);
            
          $page .= $g_layout->ret_td($editsrc, 'colspan="'.$tdcount.'" valign="top"');
          $page .= "</tr>";
        }
                
      }
      $page.='</table>';    
            
      $page.='<br><br>'.href($PHP_SELF,text('back'),SESSION_BACK);
      $g_layout->output($page);
      
      $g_layout->ui_bottom();
    }    

    /**
     * Creates an add page
     */
    function addPage($record=NULL)
    {
      global $g_layout, $PHP_SELF;
      $g_layout->register_script("atk/javascript/formfocus.js");
      $g_layout->ui_top(text('title_'.$this->m_type.'_add'));
      $g_layout->output($this->statusbar());
      
      $g_layout->output('<form name="entryform" enctype="multipart/form-data" action="'.$PHP_SELF.'?'.SID.'"'.
                                   ' method="post"'.(sizeof($this->m_listMlAdd) > 0 ? ' onsubmit="submitSave(this)"' : '').'>');
      $g_layout->output(session_form());                                   
      $forceList = decodeKeyValueSet($this->m_postvars['atkfilter']);
      $g_layout->output($this->editForm("add",$record,$forceList));
      $g_layout->output('<br><input type="submit" value="'.text('save').'">');
      if ($this->m_action=="add")
      {
        // if action is admin, we don't show the cancelbutton..
        $g_layout->output('<input type="submit" name="atkcancel" value="'.text('cancel').'">');
      }
      $g_layout->output('</form>');
      $g_layout->ui_bottom();
      $g_layout->output('<SCRIPT LANGUAGE="JavaScript">placeFocus()</SCRIPT>');
    }
    
    function searchPage($record=NULL)
    {
      global $g_layout, $PHP_SELF;
      $g_layout->register_script("atk/javascript/formfocus.js");
      $g_layout->ui_top(text('title_'.$this->m_type.'_search'));
      $g_layout->output($this->statusbar());
      
      $g_layout->output('<form name="entryform" action="'.$PHP_SELF.'?'.SID.'" method="post">');
      $g_layout->output(session_form(SESSION_BACK));
      
      $g_layout->output($this->searchForm($record));
      $g_layout->output('<br><input type="submit" value="'.text('search').'">');
      $g_layout->output('</form>');
      $g_layout->ui_bottom();
      $g_layout->output('<SCRIPT LANGUAGE="JavaScript">placeFocus()</SCRIPT>');
    }

    /**
     * Function outputs a form in which a record can be edited.
     * or, if there is no record, defaults from the postvars will be 
     * read.
     * @param $mode Mode of the form ("edit" or "add")
     * @param $forcelist A key-value array used to preset certain fields to 
     *                   a certain value
     * @param $suppresslist An array of fields that you want to hide.
     * @param $fieldprefix If set, each form element is prefixed with the 
     *                     specified prefix (used in embedded forms)
     */
    function editForm($mode="add",$record = NULL, $forceList="",$suppressList="", $fieldprefix="")
    {
      global $g_layout, $txt_field_obligatory;      
      
      // if there are displayable obligatory fields, we display a message near the bottom.
      $has_oblies = false;
      
      $g_layout->register_script("atk/javascript/formsubmit.js");
      
      if (($mode == 'add' && sizeof($this->m_listMlAdd) > 0 && !$this->m_hasMlSelectAdd) ||
         ($mode == 'edit' && sizeof($this->m_listMlEdit) > 0 && !$this->m_hasMlSelectEdit))
      {
        $selector = new atkMlSelectorAttribute();
        array_unshift($this->m_attribList, $selector);
      }

//      $form.=$this->stickyForm();

      $defaults = $record;
      
      // Form may have been saved in a session..
      if ($this->m_postvars["atkformdata"]!="")
      {
        atkdebug("Session formdata present");
        $this->modifyRecord(&$defaults, $this->m_postvars["atkformdata"]);    
      }

      $pk = $this->primaryKey($record);
      
      // hidden value that is used by the atkSubmit javascript function as a means
      // of posting extra data when submitting.. 
      $form.='<input type="hidden" name="'.$fieldprefix.'atkescape">';

      if ($mode=="edit")
      {
        $form.='<input type="hidden" name="'.$fieldprefix.'atkaction" value="update">';
        $form.='<input type="hidden" name="'.$fieldprefix.'atkselector" value="'.rawurlencode($pk).'">';        

        // Nodes can define edit_values
        if (method_exists($this,"edit_values"))
        {
          $overrides = $this->edit_values($defaults);
          while (list($varname,$value) = each($overrides))
          {
            $defaults[$varname]=$value;
          }
        }
      }
      else
      {
        $form.='<input type="hidden" name="'.$fieldprefix.'atkaction" value="save">';
//        $defaults = $this->m_postvars;

        // Nodes can define initial values, if they don't already have values.
        if (method_exists($this,"initial_values"))
        {
          $overrides = $this->initial_values();
          while (list($varname,$value) = each($overrides))
          {          
            if ($defaults[$varname]=="") $defaults[$varname]=$value;
          }
        }
      }
      
      $form.='<input type="hidden" name="'.$fieldprefix.'atkorgkey" value="'.$pk.'">';
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

      $form.=$g_layout->ret_table_simple();

      if (count($this->m_errors)>0)
      {
        $form.='<tr>';
        $errormsg = '<b>'.text('error_formdataerror').'</b>';
        
        foreach ($this->m_errors as $attribname => $msg)
        {
          $errormsg.='<br>'.text($attribname).': '.$msg;
        }

        $form.=$g_layout->ret_td($errormsg,'colspan="2"');
        $form.='</tr>';
      } 
            
      // For all attributes we use the edit() function to display an
      // appropriate way to edit the data. This may be overridden by supplying
      // an <attributename>_edit function in the derived classes.
      foreach (array_keys($this->m_attribList) as $attribname)
      {      
        $p_attrib = &$this->m_attribList[$attribname];
        // fields that have not yet been initialised may be overriden in the url..
        
        if ($defaults[$p_attrib->fieldName()]=="" && $this->m_postvars[$p_attrib->fieldName()]!="")
        {
          $defaults[$p_attrib->fieldName()] = $this->m_postvars[$p_attrib->fieldName()];
        }
        if (is_array($suppressList)&&count($suppressList)>0&&in_array($attribname,$suppressList))
        {
          $form.=$p_attrib->hide($defaults, $fieldprefix);
        }
        else
        {
          if (($mode=="edit"&&$p_attrib->hasFlag(AF_HIDE_EDIT))
              ||
              ($mode=="add"&&$p_attrib->hasFlag(AF_HIDE_ADD))
             )
          {
            if ($mode=="edit" || ($mode=="add" && !$p_attrib->isEmpty($defaults))) // when adding, there's nothing to hide..
            {
              $form.=$p_attrib->hide($defaults, $fieldprefix);
            }
          }
          else
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
              else if ($this->m_errors[$attribname]!="")
              {
                $form.=$g_layout->ret_td('<div class="error">'.text($p_attrib->fieldName()).': </div>', 'valign="top"');
              }
              else
              {
                $form.=$g_layout->ret_td(text($p_attrib->fieldName()).': ','valign="top"');
              }
            }
            else
            {
              $tdcount++; // If there's no label, the other td's have to be filled up.
            }            
            	                                
            if (($mode=="edit"&&$p_attrib->hasFlag(AF_READONLY_EDIT))
              ||
              ($mode=="add"&&$p_attrib->hasFlag(AF_READONLY_ADD))
             )
            {
              // readonly, display value..              
              $editsrc=$p_attrib->hide($defaults, $fieldprefix);
              $funcname = $p_attrib->m_name."_display";
              if (method_exists($this, $funcname)) $editsrc.= $this->$funcname($record);
              else $editsrc.=$p_attrib->display($defaults);                              
            }
            else
            {            
              $funcname = $p_attrib->m_name."_edit";
              
              if (method_exists($this,$funcname))
              {            
                $editsrc = $this->$funcname($defaults, $fieldprefix);
              }
              else
              {                             
                $editsrc = $p_attrib->edit($defaults, $fieldprefix);
              }
            }
            
            if ($p_attrib->hasFlag(AF_OBLIGATORY))
            {
              $editsrc .= "&nbsp; *";
              $has_oblies = true;
            }
	    
            $form.=$g_layout->ret_td($editsrc,'colspan="'.$tdcount.'" valign="top"');	    
            $form.='</tr>';
          }
        }        
      }
      
      $form.='</table>';

      if ($has_oblies && !empty($txt_field_obligatory))
      {
        $form.= '<span style="font-size:8pt">'.text("field_obligatory").'</span><br>';
      }
      
      return $form;
    }
    
    /**
     * Function outputs a form that the user can use to search records.
     */
    function searchForm($record = NULL)
    {
      global $g_layout, $g_db;
      
      $defaults = $record;           

      $form.=$g_layout->ret_table_simple();      
   
      $searchModes = $g_db->getSearchModes();
            
      if (count($searchModes)>1)
      {      
        $form.='<tr>';
        $form.=$g_layout->ret_td(text("search_mode"));                
        $sel = '<select name="atksearchmode">';
        for ($i=0;$i<count($searchModes);$i++)
        {
          // if supported, we select the 'like' searchmode by default
          $selected="";
          if ($this->m_postvars["atksearchmode"]==$searchModes[$i]||($this->m_postvars["atksearchmode"]==""&&$searchModes[$i]=="substring")) $selected="selected";
          
          $sel.= '<option value="'.$searchModes[$i].'" '.$selected.'>'.text("search_".$searchModes[$i]);
        }
        $sel.= '</select>';
        $form.=$g_layout->ret_td($sel);
        $form.='</tr>';
      }
      elseif (count($searchModes)==1)
      {
        $form.='<input type="hidden" name="atksearchmode" value="'.$searchModes[0].'">';
      }
      
      $form.='<tr>'.$g_layout->ret_td("<hr>",'colspan="2"').'</tr>';
                      
      foreach (array_keys($this->m_attribList) as $attribname)
      {      
        $p_attrib = &$this->m_attribList[$attribname];
        
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
              $form.=$g_layout->ret_td(text($p_attrib->fieldName()).': ','valign="top"');
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
    function buildNavigation()
    {
      global $PHP_SELF;
      $limit = (int)$this->m_postvars['atklimit'];
      $count = (int)$this->countDb($this->m_postvars['atkfilter'], $this->m_listExcludes);
      
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
        $nav.=href($PHP_SELF."?atksearch[".$this->m_index."]=".$char."*&atksearchmode=wildcard",$label);
        
        // uncomment the following line if 26 letters seems to become too wide.
        //if ($count>13 && ($i+1)==floor($count/2)) $nav.='<br>'; else 
        if ($i!=$count-1) $nav.=" ";
      }      
      
      return $nav;
    }


    /**
     * Admin page displays records and the actions that can be performed on
     * them (edit, delete)
     */
    function adminPage()
    {
      global $g_securityManager, $g_layout, $PHP_SELF, $atksearch;

      $g_layout->ui_top(text('title_'.$this->m_type.'_admin'));
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

      $recordset = $this->selectDb($this->m_postvars['atkfilter'],$this->m_postvars['atkorderby'],array("offset" => $this->m_postvars['atkstartat'], "limit" => $this->m_postvars['atklimit']),$this->m_listExcludes);

      if ($this->hasFlag(NF_ADD_LINK) && !$this->hasFlag(NF_NO_ADD) && $this->allowed("add"))
      {
        $addurl = $PHP_SELF.'?atkaction=add';
        $g_layout->output(href($addurl,text("link_".getNodeType($this->m_type)."_add"),SESSION_NESTED).'<br><br>');
      }

      // create navigation bar      
      if ($this->m_index != "") $g_layout->output($this->buildIndex($recordset[0][$this->m_index]).'<br><br>');
      $nav = $this->buildNavigation();
      if (!empty($nav)) $g_layout->output("$nav<br>");
      $g_layout->output('<br>');
      $actions = array();

      if (!$this->hasFlag(NF_NO_EDIT)&&$this->allowed("edit"))
      {
        $actions[]=href($PHP_SELF.'?atkaction=edit&atkselector=[pk]',text('edit'),SESSION_NESTED);
      }
      else
      {
        // if you may not edit, maybe you are allowed to view..
        if (!$this->hasFlag(NF_NO_VIEW)&&$this->allowed("view"))
        {
          $actions[]=href($PHP_SELF.'?atkaction=view&atkselector=[pk]',text('view'),SESSION_NESTED);
        }
      }
      if (!$this->hasFlag(NF_NO_DELETE)&&$this->allowed("delete"))
      {
        $actions[]=href($PHP_SELF.'?atkaction=delete&atkselector=[pk]',text('delete'),SESSION_NESTED);
      }
      if($this->hasFlag(NF_COPY)&&$this->allowed("copy"))
      {
        $actions[]=href($PHP_SELF.'?atkaction=copy&atkselector=[pk]',text('copy'),SESSION_NESTED);
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
    function recordList($recordset, $actions,$sortable=true,$suppressList="")
    {
      global $g_layout, $PHP_SELF;
      $output = $g_layout->data_top();

      $output.="<tr>";      
   
      // stuff for the totals row..
      $totalisable = false;
      $totals = array();      
      
      $orientation = atkconfig("recordlist_orientation");

      // display a headerrow with titles. each attributetitle is clickable to
      // sort the list by that attribute.
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
          $neworder=$this->m_table.".".$p_attrib->fieldName();

          if ($sortable && !$p_attrib->hasFlag(AF_NOSORT))
          {
            $tmp = href($PHP_SELF.'?atkorderby='.$neworder,text($p_attrib->fieldName()));
          }
          else $tmp = text($p_attrib->fieldName());

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
        $searchRow.= session_form();
        
        $searchmode = $this->m_postvars["atksearchmode"];
        if ($searchmode=="") $searchmode = atkconfig("search_defaultmode");
        
        $searchRow.= '<input type="hidden" name="atksearchmode" value="'.$searchmode.'">';
        $searchable = false;
        
        $searchBtn = '<input type="submit" value="'.text("search").'">';        
        if (!$this->hasFlag(NF_NO_EXTENDED_SEARCH))
        {
          $searchBtn.='<br>'.href($PHP_SELF."?atkaction=search&atksearchmode=".$searchmode,"(".text("search_extended").")",SESSION_NESTED);
          //$searchBtn.='<form action="'.$PHP_SELF.'?'.SID.'" method="get">'.
            //          session_form(SESSION_NESTED).
              //        '<input type="hidden" name="atkaction" value="search">'.
                //      '<input type="submit" value="'.text("ext").'"></form>';
        }
        $actioncol = $g_layout->ret_td_datatitle($searchBtn);       

        $actioncol.= '<input type="hidden" name="atkaction" value="'.$this->m_action.'">'; 
        
        if ($orientation=="left"||$orientation=="both")
        {
          $searchRow.=$actioncol;
        }

        // Second loop.. this time for the search fields.
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

      if ($searchable) $output.=$searchRow;           
      
      
      for ($i=0;$i<count($recordset);$i++)
      {  
        // the functionality list:        
        // TODO: pick some cool little icons instead of text        
        if (count($actions)>0)
        {       
          $stractions = "";
          $pk = $this->primaryKey($recordset[$i]);
	  
          for ($j=0;$j<count($actions);$j++)
          {
            //$action=str_replace('[pk]',rawurlencode($pk),$actions[$j]);
            // experimental: [] tags could be encoded but we still parse them..           
            
            // dirty hack:
            $atkencoded = false;
            if (strpos($actions[$i],"_#")>0)
            {
              // normal value.. 
              $atkencoded = true;
            }            
            $action=str_replace("%5B","[",$actions[$j]);
            $action=str_replace("%5D","]",$action);
            $action=str_replace("_#5B","[",$action);
            $action=str_replace("_#5D","]",$action);
            
            if ($atkencoded)
            {
              $action=str_replace('[pk]',atkurlencode(rawurlencode($pk)),$action);            
            }
            else
            {
              $action=str_replace('[pk]',rawurlencode($pk),$action);
            }
            $action=stringparse($action,$recordset[$i],true);
            $stractions.=$action.'&nbsp;';
          }
          $actionCol=$g_layout->ret_td($stractions);
        }
        else
        {
          if ($searchable)
          {
            $actionCol.=$g_layout->ret_td("&nbsp;");
          }
        }
        
        // We alternate the color for even and non-even rows.        
        $output.='<tr class="row'.(($i%2==0)?1:2).'">';
        
        if ($orientation=="left"||$orientation=="both")
        {
          $output.=$actionCol;
        }
        
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
              $output.=$g_layout->ret_td($this->$funcname($recordset[$i]));
            }
            else
            {
              // otherwise, the display function of the particular attribute
              // is called.
              $output.=$g_layout->ret_td($p_attrib->display($recordset[$i]),'');
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
              $totalRow.=$g_layout->ret_td_datatitle($p_attrib->display($totals[$attribname]));
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
          $output.='<td><b>'.text($p_attrib->fieldName()).'</b></td>';
          
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
              $value=$this->$funcname($recordset[$i]);
            }
            else
            {
              // otherwise, the display function of the particular attribute
              // is called.              
              $value=$p_attrib->display($recordset[$i]);
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
              $totalRow.='<td><b>'.$p_attrib->display($totals[$attribname]).'</b></td>';
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
     * Select page displays records and gives the user the ability to select a record
     */
    function selectPage()
    {
      global $g_layout, $atksearch;

      // When there's a lot of data, records will be spread across multiple pages.
      if ($this->m_postvars['atklimit']=="") $this->m_postvars['atklimit']=atkconfig("recordsperpage");
      if ($this->m_postvars['atkstartat']=="" || isset($atksearch)) $this->m_postvars['atkstartat']=0;

      $recordset = $this->selectDb($this->m_postvars['atkfilter'],$this->m_postvars['atkorderby'],array("offset" => $this->m_postvars['atkstartat'], "limit" => $this->m_postvars['atklimit']),$this->m_listExcludes);
      
      if (count($recordset)==1 && $this->hasFlag(NF_AUTOSELECT))
      {
        // There's only one record and the autoselect flag is set, so we 
        // automatically go to the target.
        $target = stringparse(rawurldecode($this->m_postvars['atktarget']),$recordset[0], true);	
        $this->redirect($target);
      }
      else
      {
        $g_layout->ui_top(text('title_'.$this->m_type.'_select'));
        $g_layout->output($this->statusbar());      
        $g_layout->output(text($this->m_type.'_select').'<br>');
        $g_layout->output('<br>');
        
        if ($this->m_index != "") $g_layout->output($this->buildIndex($recordset[0][$this->m_index]).'<br><br>');
        
        // create navigation bar
        $nav = $this->buildNavigation();
        if (!empty($nav)) $g_layout->output("$nav<br>");
        $g_layout->output('<br>');
	
        $actions=array(href($this->m_postvars['atktarget'],text('select'),SESSION_NESTED));

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
      
      $g_layout->ui_top(text('title_delete'));
      $g_layout->output($this->statusbar());

      $g_layout->output('<form action="'.$PHP_SELF.'?"'.SID.' method="post">');
      $g_layout->output(session_form());
      $g_layout->output('<input type="hidden" name="atkaction" value="delete">');
      $g_layout->output('<input type="hidden" name="atkselector" value="'.$atkselector.'">');
      

      $g_layout->table_simple();

      $g_layout->output('<tr>');

      $g_layout->td($errormsg,'colspan="2"');
      $g_layout->output('</tr>');
      $g_layout->td(text('confirm_delete'));
      $g_layout->output('<tr>');
      $g_layout->td('<input name="confirm" type="submit" value="'.text('yes').'"><input name="cancel" type="submit" value="'.text('no').'">','colspan="2"');
      $g_layout->output('</tr></table></form>');

      $g_layout->ui_bottom();
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
    //echo $this->m_postvars["atkescape"];
        $this->redirect(atkurldecode($this->m_postvars["atkescape"]));
      }
      else
      {            
        if ($this->allowed($this->m_action))
        {
          $g_securityManager->logAction($this->m_type, $this->m_action);
 
          $funcname="action_".$this->m_action;
          if (method_exists($this,$funcname))
          {
            $this->$funcname();
          } 
          else
          {
            atkdebug("dispatcher error: no action defined for '".$this->m_postvars['atkaction']."'");
          }
        }
        else
        {
          $g_layout->output("Sorry, you don't have permission to perform this action");
        }      
      
        $g_layout->page(text('app_shorttitle')." - ".text('title_'.$this->m_type.'_'.$this->m_postvars['atkaction']));
      
        // This is the end of all things for this page..
        // so we clean up some resources..
      }
      $g_db->disconnect();
      atkdebug("disconnected from the database");
    }
    
    /*** The node actions ***/
    
    function action_view()
    {
      $recordset = $this->selectDb($this->m_postvars['atkselector']);
      $this->viewPage($recordset[0]);
    }
      
    function action_add()
    {
      $this->addPage();
    }
     
    function action_update()
    {
      if ($this->m_postvars['atkcancel']=="")
      {
        $record = $this->updateRecord();
        $this->validate($record);
        if (count($this->m_errors)>0)
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
        $this->validate($record);
        if (count($this->m_errors)>0)
        {
          $this->addPage($record);
        }
        else
        {
          $this->addDb($record);
          
          $location="";
          if ($this->hasFlag(NF_EDITAFTERADD))              
          {          
            $location = session_url($PHP_SELF.'?atkaction=edit&atkselector='.rawurlencode($this->primaryKey($record)),SESSION_NESTED);
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
      $this->copyDb($this->m_postvars['atkselector']);
      $this->redirect();
    }
    
    function action_xml()
    {
      if ($this->m_postvars['atkselector']!="")
      {
        $recordset = $this->selectDb($this->m_postvars['atkselector']);
        $this->xml($recordset[0]);
      }
      else
      {
        $recordset = $this->selectDb();
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
      $recordset = $this->selectDb($this->m_postvars['atkselector']);
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
      $back = 0;
      
      if ($g_returnurl!="") $location = $g_returnurl;
      
      if ($location=="")
      {      
        // The page we redirect to is depending on the action we are doing.
        switch ($this->m_action)
        {          
          case "update":
            if ($this->editingAfterAdd()) // If we're doing an edit, and we came from add
                                          // we go 2 levels back (skip the add).
            {                      
              $back = 2;              
            } 
            else
            {
              $back = 1;
            }
            break;
          default:
            $back = 1;
            break;
        }       
        
        if ($back>0)
        {
          $location = session_url($PHP_SELF,SESSION_BACK,$back);
        }        
        else    
        {
          if ($location=="")
          {
            $location = session_url($PHP_SELF,SESSION_DEFAULT);
          }
        }

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
    function validate($record)
    {
      global $g_db;
      atkdebug("validate()");
      foreach (array_keys($this->m_attribList) as $attribname)
      {
        $p_attrib = &$this->m_attribList[$attribname];
        // validate obligatory fields (but not the auto_increment ones, because they don't have a value yet)
        if ($p_attrib->hasFlag(AF_OBLIGATORY) && !$p_attrib->hasFlag(AF_AUTO_INCREMENT) && $p_attrib->isEmpty($record))
        {
          $this->m_errors[$attribname] = text('error_obligatoryfield');
        }
        else if ($p_attrib->hasFlag(AF_UNIQUE) &&
                 count($g_db->getrows("SELECT ".$p_attrib->fieldName()." FROM ".$this->m_table." WHERE $attribname='".escapeSQL($this->m_postvars[$attribname])."' AND NOT (".$this->primaryKey($record).")"))>0
                )
        {
          $this->m_errors[$attribname] = text('error_uniquefield');
        }
        else
        {
          //don't validate empty fields
          if (!( $p_attrib->isEmpty($record)))
          {
            $funcname = $p_attrib->m_name."_validate";

            if (method_exists($this,$funcname))
            {
              $error = $this->$funcname($record[$attribname]);
              if ($error!="") $this->m_errors[$attribname] = $error;
            }
            else
            {
              $error = $p_attrib->validate($record[$attribname]);
              if ($error!="") $this->m_errors[$attribname] = $error;
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
      // any updated happended.)
      if ($this->m_postvars['atkorgkey']!="")
      {
        // If we need to track changes, we first load the original values..
        if ($this->hasFlag(NF_TRACK_CHANGES))
        {
          $recordset = $this->selectDb($this->m_postvars['atkorgkey']);
          $record["atkorgrec"] = $recordset[0];
        }      
      
        $query->addCondition($this->m_postvars['atkorgkey']);

        $storelist = array();

        foreach (array_keys($this->m_attribList) as $attribname)
        {
          $p_attrib = &$this->m_attribList[$attribname];        
          if ($p_attrib->hasFlag(AF_READONLY_EDIT)==false && $p_attrib->hasFlag(AF_HIDE_EDIT)==false)
 	        {
            if (method_exists($p_attrib,"store"))
            {
              $storelist[]=$attribname;
            }
            else
            {
              $p_attrib->addToQuery($query,$this->m_table,"",$record,1,"edit"); // start at level 1
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
     * Create a table to store the node.
     * TODO (function will be used for autoinstallation of modules)
     */
    function createDb()
    {
      // TODO (function will be used for autoinstallation of modules)
    }

    /**
     * Count the record(s) from a certain select query.
     * The 'selector' parameter can be anything that's valid in a 'where' statement.
     * @param $selector The 'where' clause that indicates which records to select.
     * @param $execludeList List of attributes to be excluded from the query
     * @param $includeList List of attributes that have to be included into the query
     */
    function countDb($selector="", $excludeList="", $includeList="")
    {
      global $g_db;

      $name = "atk".atkconfig("database")."query";
      $query = new $name();

      $query->addTable($this->m_table);
      $query->addCondition($selector);

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
              $p_attrib->searchCondition($query, $this->m_table, $value, $searchmode);
            }
            else 
            {
              $condition = "lower(".$this->m_table.".".$key.") LIKE lower('%".escapeSQL($value,true)."%')";
              $query->addCondition($condition);              
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
            $p_attrib->addToQuery($query,$this->m_table,"","",1,"select"); // start at level 1
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
    function copyDb($selector)
    {
      
      $recordset = $this->selectDb($selector);
      
      if(count($recordset)>0)
      {      
        $this->addDb($recordset[0]);
      }
      else
      {
        atkdebug(" Geen records gevonden met Selector: $selector - $parent");
      }
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
    function selectDb($selector="", $order="", $limit="", $excludeList="",$includeList="")
    {
      global $g_db;
      
      $this->m_records = array();
      $selectlist = array();
      $loadlist = array();

      if($order=="" && $this->m_default_order!="") $order=$this->m_default_order;

      $name = "atk".atkconfig("database")."query";
      $query = new $name();

      $query->addTable($this->m_table);
      $query->addCondition($selector);

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
              $p_attrib->searchCondition($query, $this->m_table, $value, $searchmode);
            }
            else 
            {
              $condition = "lower(".$this->m_table.".".$key.") LIKE lower('%".escapeSQL($value,true)."%')";
              $query->addCondition($condition);              
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
            $p_attrib->addToQuery($query,$this->m_table,"","",1,"select"); // start at level 1
          }
        }        
      }

      $querystring = $query->buildSelect();
      if ($order!="") $querystring.=" ORDER BY ".$order;
      
      if (is_array($limit) && count($limit) == 2) $dbrecords = $g_db->getrows($querystring, $limit["offset"], $limit["limit"]);
      else $dbrecords = $g_db->getrows($querystring);
      
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
                
        $recordset[] = $therecord;
      }      

      // also load special storage attributes.
      for ($i=0;$i<count($loadlist);$i++)
      {
        for ($j=0;$j<count($recordset);$j++)
        {
          $p_attrib = &$this->m_attribList[$loadlist[$i]];
          $recordset[$j][$loadlist[$i]] = $p_attrib->load($g_db, $recordset[$j]);
        }
      }
      
      // The $this->m_records is here for compatibility reasons.
      // USE OF M_RECORDS IS OBSOLETE!!!!!!!!!
      // new usage of selectDb: $recordset = $this->selectDb(...);
      // use the returned recordset instead of $this->m_records;
      $this->m_records = $recordset;
      $this->m_currentRec = 0;
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
                 

      // also store special storage attributes.      
      for ($i=0;$i<count($storelist);$i++)
      {            
        $p_attrib = &$this->m_attribList[$storelist[$i]];
      
        $p_attrib->store($g_db, $record,"add");
       
      }

      $querystring = $query->buildInsert();
      $g_db->query($querystring);

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
      
      $recordset = $this->selectDb($selector);
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
    
    function allowed($action)
    {
      global $g_securityManager;

      return ($this->hasFlag(NF_NO_SECURITY)
              ||in_array($this->securityKey($action), $this->m_unsecuredActions)
              || $g_securityManager->allowed((empty($this->m_module) ? "" : $this->m_module.".").$this->m_type,$this->securityKey($action)));
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
     * Register a new sticky var
     * @param $varname can be a string, or an array of strings.
     * @param $global global or local stickyvar
     */
    /*function addStickyVar($varname,$global=true)
    {
      if (is_array($varname))
      {
        $this->m_stickyVars = atk_array_merge($this->m_stickyVars,$varname);
      }
      else
      {
        $this->m_stickyVars[] = $varname;
      }   
    }*/
  
   /**
    * This function gets the value of a sticky var. There are several scopes
    * in which the value could have been defined. They are searched in the 
    * order $this->m_postvars, $HTTP_GET_VARS, $HTTP_POST_VARS, $GLOBALS.
    */
/*    function getStickyValue($key)
    {
      global $HTTP_GET_VARS, $HTTP_POST_VARS;
      if ($this->m_postvars[$key]!="") return $this->m_postvars[$key];
      if ($HTTP_GET_VARS[$key]!="") return $HTTP_GET_VARS[$key];
      if ($HTTP_POST_VARS[$key]!="") return $HTTP_POST_VARS[$key];
      if ($GLOBALS[$key]!="") return $GLOBALS[$key];
      return "";
    }      */
    
    /**
     * Puts all sticky vars in hidden form elements.
     */     
    function stickyForm()
    {    /*  
      for ($i=0;$i<count($this->m_stickyVars);$i++)
      {    
        $value = $this->m_postvars[$this->m_stickyVars[$i]];
        if ($value!="")
        {        
          $form.="\n".'<input type="hidden" name="'.$this->m_stickyVars[$i].'" value="'.$value.'">';                  
        }
      }
      return $form;*/
    }
    
    /**
     * Dirty function to see if the current edit form is following an add screen
     * that had NF_EDITAFTERADD set..
     */ 
    function editingAfterAdd()
    {
      global $g_sessionData, $atklevel;
      $res = ($this->m_action == "update"
              && $g_sessionData["stack"][$atklevel-1]["atkaction"] == "add"
              && $g_sessionData["stack"][$atklevel-1]["atknodetype"] == $this->m_type);  
      return $res;
    }
    
    /**
     * Display a statusbar with a stacktrace and a help button.
     */
    function statusbar()
    {
      global $g_sessionManager, $g_layout;
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
          $g_layout->register_script("atk/javascript/newwindow.js");      
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
        return "javascript:NewWindow('help.php?node=".$this->m_type."','".text("help")."',600,400,'yes')";
      }
    }

  }  

?>

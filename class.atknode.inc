<?php

// Multiple inclusion protector
if (!defined("CLASS_ATKNODE_INC"))
{
  define("CLASS_ATKNODE_INC",1);

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
  
  define("NF_AUTOSELECT",2048); // No root elements can be copied 

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
   *  	                Niederhauser's node-code. (like creation date etc.)
   *                   - Authorization: who is allowed to do what on which node
   *                   - Install function. And a script that you can use to
   *                     install new nodetypes in your achievo. The install
   *                     function would typically create a table and register
   *                     itself somewhere in the menu.
   *                   - Lots of other small things need to be done. Search for
   *                     'todo' in the code :)
   *
   * @author Ivo Jansch (ivo@achievo.com)
   * @version 0.21
   *
   * <b>Changes</b>:
   *
   * 0.21    - The editForm() didn't posted readonly attribs. Added hidden values for
   *           those attribs. (martin)
   *         - Added new function postDel(). You can use this in your classfiles to
   *           to do something when you delete a record. (martin)
   * 0.20    - Attributes can now have an edit_values($record)  function.
   *           the value of the database will be overwritten. (Sandy)
   *         - Added a BR tot add_page and the edit_page (Sandy)
   *         - Move layout class into class.atknode.inc for the template support (Sandy)
   * 0.19    - Attributes are now informed of the table meta information of
   *           the field (Peter Verhage <peter@ibuildings.net>)
   *         - Previous/next links now also appear above recordlists (Ivo)
   * 0.18    - Small optimisations (Ivo)
   * 0.17    - Support for new flags: AF_NO_QUOTES and AF_FORCE_LOAD (Ivo)
   *         - Fixed a bug in search feature en previous/next links (Ivo)
   * 0.16    - Fixed a bug when using the search feature in Internet Explorer (Ivo)
   *         - Support for attribute AF_HIDE_SELECT flag (Ivo)
   * 0.15    - New flags: NF_EDITAFTERADD, NF_NO_SEARCH and NF_NO_FILTER (Ivo)
   *         - Fixed a bug in search feature (Ivo)
   *         - Created default language files for atk (atk now runs even if you don't
   *           have a languages dir in your app) (Ivo)
   *         - Speed optimisations (less queries, no unneeded joins) (Ivo)
   *         - Added 'save and close' and 'cancel' buttons to editpage. (Ivo)
   *         - Fix for primary keys containing spaces (Ivo)
   *         - Default configuration file (Ivo)
   * 0.14    - Atk now calls isEmpty function on attributes to see if they're empty
   *           (instead of checking for "" in the postvars) (Ivo)
   * 0.13    - Atk now disconnects from the database on end of page. (Ivo)
   * 0.12    - Fixed a bug in primary key function when primary key was a relation (Ivo)
   * 0.11    - Fixed a bug in the search boxes (when spaces were used) (Ivo)
   * 0.10    - Default sort (Sandy)
   *         - RecordList now has search boxes to search in records (Ivo)
   * 0.9     - Documented everything and made docs phpdoc compliant (Sandy)
   *         - Added postAdd() and postUpdate() triggers (Ivo)
   * 0.8     - Oracle support (Sandy)
   *         - Solved bug when changing a records primary key (Ivo)
   *         - Sticky atkfilter (filtering records through url) (Ivo)
   *         - Solved a bug in the next/previous links (Ivo)
   * 0.7     - Better API for retrieving currentrec and in the list (Ivo)
   * 0.6     - Major speedup in admin page (1 query for the entire list instead of
   *           1 for each relation in each record) (Ivo Jansch <ivo@achievo.com>
   * 0.5     - AF_READONLY flag for attributes (Sandy Pleyte <sandy@ibuildings.nl>)
   *
   * $Id$
   * $Log$
   * Revision 4.10  2001/05/01 09:15:51  ivo
   * Initial session based atk version.
   *
   * Revision 4.9  2001/04/25 09:39:36  ivo
   * typo fix
   *
   * Revision 4.7  2001/04/25 08:46:08  ivo
   * Big optimisations: using references for attribute manipulation.
   *
   * Revision 4.6  2001/04/25 07:29:40  sandy
   * fixed a bug in the data javascript for multilanguage
   * added the stickyvars to the onetomany relations urls
   *
   * Revision 4.5  2001/04/24 13:51:50  ivo
   * Fixed some small bugs, and updated the language files, improved the menu.
   *
   * Revision 4.4  2001/04/24 12:40:50  ivo
   * skel cleanup, typo fix in moduletools.
   *
   * Revision 4.3  2001/04/24 11:50:13  ivo
   * Bugfixes to modulesupport: changed all 'require' statements to 'include'
   * statements, since 'require' is always executed at script compile time,
   * not at runtime.
   *
   * Revision 4.2  2001/04/24 09:06:09  sandy
   * first level of the tree always visible, and fixed the NF_TREE flags.
   *
   * Revision 4.1  2001/04/23 13:21:22  peter
   * Introduction of module support. An ATK application can now have zero
   * or more modules which can, but don't have to, contain ATK nodes.
   *
   * Revision 4.0  2001/04/23 10:19:38  ivo
   * Revision number update
   *
   * Revision 1.1.1.1  2001/04/23 10:17:05  ivo
   * Added atk4 to repository.
   *
   * Revision 1.52  2001/04/19 07:36:32  ivo
   * New flag NF_AUTOSELECT for automatic redirect in select pages when only
   * one record is present.
   * Updated Norwegian language file.
   *
   * Revision 1.51  2001/04/18 13:06:32  sandy
   * form focus added, new nodeflags, and some tree bugfixes
   *
   * Revision 1.49  2001/04/11 14:42:44  sandy
   * fixed edit and add link for treeview
   *
   * Revision 1.48  2001/04/11 14:36:25  ivo
   * Optimisation in treeview.
   *
   * Revision 1.47  2001/04/10 20:22:30  martin
   * Fixed two (small) bugs in treePage(). Code cleanup for treeview features. Small speed optimisations also for atktree features
   *
   * Revision 1.46  2001/04/10 13:28:49  ivo
   * Small fix: <br>'s when navigation is invisible.
   *
   * Revision 1.45  2001/04/10 11:51:46  martin
   * ATK treeview added for parent child relations. Just use NF_TREE as flag when you call the atknode constructor.
   *
   * Revision 1.44  2001/04/06 15:09:35  ivo
   * Fixed bugs in matrixrelation.
   *
   * Revision 1.43  2001/04/05 14:50:51  peter
   * fixed a bug with the load methods of special attributes
   *
   * Revision 1.42  2001/04/05 14:17:58  ivo
   * Navigation fix.
   *
   * Revision 1.41  2001/04/05 14:01:58  ivo
   * Small fix to navigation on select pages.
   *
   * Revision 1.40  2001/04/05 13:58:31  ivo
   * Small navigation fix.
   * Layout changes: head can now be rendered at the end.
   *
   * Revision 1.39  2001/04/05 13:22:52  peter
   * optimized navigation through records
   *
   * Revision 1.38  2001/04/05 11:02:49  ivo
   * Readded some of the removed code from previous revision, because
   * the code was necessary when using where clauses on joined tables.
   *
   * Revision 1.37  2001/04/05 10:46:42  ivo
   * Optimized the record count feature. (Removed buildCount and a lot of
   * duplicate code)
   *
   * Revision 1.36  2001/04/05 10:19:06  peter
   * new way of navigation through the record list
   *
   * Revision 1.35  2001/04/04 09:01:48  ivo
   * AF_READONLY now also affects add forms.
   * Implemented hide() method for manytoonerelation.
   *
   * Revision 1.34  2001/04/03 09:06:05  ivo
   * Fixed a bug in printableRecordList.
   *
   * Revision 1.33  2001/04/02 14:53:18  ivo
   * New feature: printableRecordList.
   *
   * Revision 1.32  2001/04/02 08:22:58  ivo
   * Improved groupbased attribute security.
   *
   * Revision 1.31  2001/03/30 10:06:14  ivo
   * Attribute 'read' action is now called 'view'.
   *
   * Revision 1.30  2001/03/29 11:22:28  ivo
   * New feature: logging
   *
   * Revision 1.29  2001/03/28 14:07:50  ivo
   * Added support for actions that require no access privileges.
   *
   * Revision 1.28  2001/03/27 09:55:06  ivo
   * Fixed bug: When sorting a recordlist, the stickyvars would get lost.
   *
   * Revision 1.27  2001/03/27 07:05:55  ivo
   * data_top and data_bottom are now themeable.
   *
   * Revision 1.26  2001/03/23 15:28:55  ivo
   * Fixed minor bugs in skel files.
   * Updated Themes-HOWTO to reflext new theme structure.
   * Fixed a bug in the atk delete action.
   * Fixed a bug in the theme support.
   *
   * Revision 1.25  2001/03/21 15:02:06  ivo
   * New feature: sticky vars (variables that 'stick' to all forms and
   * urls once you've set them.
   * Stripped the m_prefix function, since embedded forms have become
   * obsolete.
   *
   * Revision 1.24  2001/03/21 11:00:19  peter
   * new database management classes
   *
   * Revision 1.23  2001/03/21 10:18:38  ivo
   * Fixed bug: viewPage ignored AF_HIDE flag.
   *
   * Revision 1.22  2001/03/20 15:34:11  ivo
   * Added 'view' action (a readonly edit).
   *
   * Revision 1.21  2001/03/16 09:25:23  ivo
   * Commented some code.
   *
   * Revision 1.20  2001/03/16 07:51:59  ivo
   * Improved group-based security.
   *
   * Revision 1.19  2001/03/14 18:04:18  peter
   * removed ORDER BY clause in previous commit, added it again
   *
   * Revision 1.18  2001/03/14 16:16:30  sandy
   * database name and userid field is configurable
   *
   * Revision 1.17  2001/03/14 16:05:30  ivo
   * Fixed a bug which made select pages slow.
   *
   * Revision 1.16  2001/03/14 15:21:54  ivo
   * New feature: AF_TOTAL flag for attributes. This results in a total row
   * at the bottom of the record list.
   *
   * Revision 1.15  2001/03/14 13:16:54  peter
   * extended query method of the database "drivers" with limit support and 
   * added PostgreSQL support (which ain't perfect yet, but can't be fixed until 
   * PostgreSQL gets support for LEFT JOIN's)
   *
   * Revision 1.14  2001/03/14 10:22:27  ivo
   * Attributes now have a hide() function to do their own input=hidden implementation.
   *
   * Revision 1.13  2001/03/02 15:06:44  ivo
   * New node flag: NF_ADD_LINK (instead of an addform on an adminpage, you get
   * a 'click here to add' link)
   *
   * Revision 1.12  2001/02/28 16:00:51  sandy
   * - fixed the search box length
   *
   * Revision 1.11  2001/02/28 15:34:32  sandy
   * - fixed a bug in the text attributes (length)
   *
   * Revision 1.10  2001/02/28 08:32:13  sandy
   * - Bugfix in the class.atknode for theme support
   * - Enabled the haltfunctions again
   *
   * Revision 1.9  2001/02/23 11:17:30  sandy
   * - Updated the layout class with new template engine
   * - New Dummy attribute
   * - New skel directory
   * - and some small new config vars
   *
   * Revision 1.8  2001/02/22 22:51:32  peter
   * added multilanguage support, changed addToQuery API for i
   * attributes, fixed other things, clean-up of code
   *
   * Revision 1.7  2001/02/22 12:20:35  ivo
   * Fixed bug: when NF_EDITAFTERADD was set, atkfilter would disappear.
   *
   * Revision 1.6  2001/02/22 11:38:44  ivo
   * Added some sourcecode comments
   *
   * Revision 1.5  2001/02/20 13:17:02  martin
   * cvs commit test
   *
   * Revision 1.4  2001/02/16 16:00:27  peter
   * changed filename defaultconfig.inc.php to defaultconfig.inc.php3 
   * because of possible issues with PHP3
   *
   * Revision 1.3  2001/02/15 16:20:24  ivo
   * Major new feature: security.
   *
   * Revision 1.2  2001/02/04 14:54:36  martin
   * Added new function postDel().
   * Added hidden values in editForm() for readonly attribs.
   *
   * Revision 1.1.1.1  2001/01/10 13:57:56  sandy
   * Achievo Tool Kit
   *
   */

  // Global theme variable, must be declared before the includes
  $g_theme = array();
  // Global array to store meta-information about tables, so we don't have to read them from the db for each class instance.
  $g_tableMeta = array();
  // Global debug msg
  $g_debug_msg = "";
  // Global node list
  $g_nodes = array();
  // Global module list
  $g_modules = array();
  // Global menu
  $g_menu = array();

  require "atk/atkconfigtools.inc";
  require "atk/defaultconfig.inc.php";

  include "./config.inc.php";
  require "atk/atktools.inc";
  require "atk/class.layout.inc";
  require ("atk/db/class.atk".$config_database."db.inc");
  require "atk/db/class.atkquery.inc";  
  require "atk/attributes/class.atkattribute.inc";
  require "atk/relations/class.atkrelation.inc";
  require "atk/security/class.atksecuritymanager.inc";
  require "atk/session/class.atksessionmanager.inc";
  require "atk/atkmenutools.inc";  
  require "atk/modules/atkmoduletools.inc";
  require "atk/modules/class.atkmodule.inc";

  // Default language file
  require "atk/languages/".$config_languagefile;

  // Application specific language file
  if (file_exists("languages/".$config_languagefile))
  {
    include "languages/".$config_languagefile;
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
   
   $g_sessionVars = array("atknodetype","atktarget","atkfilter","atkaction","atkselector");

  /************* MODULE HANDLING *************/

  // Auto-register modules
  if (file_exists("$config_module_path/config.modules.inc"))
    include "$config_module_path/config.modules.inc";
  
  // Module specific language file
  for ($i = 0; $i < sizeof($g_modules); $i++)
  {
    $filename = "$config_module_path/".$g_modules[$i]."/languages/$config_languagefile";
    if (file_exists($filename))
    {
      include ($filename);
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
      include ("$config_module_path/".$g_modules[$i]."/".$attr_arr[$j]);
    }
  }
  
  /*******************************************/

  // Login
  if (!$g_securityManager->authenticate())
  {
    atkdebug("login failed");
    exit;
  }
 
  // At some places we need a random number generator, so we seed the generator here.
  srand ((double) microtime() * 1000000);

  // Since atk pages are always dynamic, we have to prevent that some browsers cache
  // the pages
  header ("Expires: Mon, 26 Jul 1997 05:00:00 GMT");    // Date in the past
  header ("Last-Modified: " . gmdate("D, d M Y H:i:s") . " GMT"); // always modified
  header ("Cache-Control: no-cache, must-revalidate");  // HTTP/1.1
  header ("Pragma: no-cache");                          // HTTP/1.0

  // The atk node class
  class atkNode
  {

    /*** Member variables ***/

    /**
     * This array will hold the data of records that are read from the database.
     */
    var $m_records = Array();

    /**
     * Pointer to the current record.
     */
    var $m_currentRec = 0;

    /**
     * The list of attributes of a node. These should be of the class atkAtribute
     * or one of its derivatives.
     */
    var $m_attribList = Array();

    /**
     * The list of relations with other nodes. These should be of the class
     * atkRelation or one of its derivatives.
     */
    var $m_relationList = Array();

    /**
     * The type of node. (The constructor of a derived class passes its type
     * to the atkNode class.
     */
    var $m_type;

    /**
     * The table to use for data storage.
     */
    var $m_table;
    var $m_seq;

    /**
     * The primary key of this node
     */
    var $m_primaryKey = Array();

    /**
     * Array containing the metadata of the table (with fieldname, type and length)
     */
    var $m_tableMeta = Array();

    /**
     * Boolean that indicates whether a record is currently loaded from the database.
     */
    var $m_recordLoaded = 0;

    /**
     * The postvars (or getvars) that are passed to a page will be passed
     * to the class using the dispatch function. We store them in a member
     * variable for easy access.
     */
    var $m_postvars = Array();

    /**
     * The action that we are currently performing.
     */
    var $m_action;

    /**
     * This array is used to store error in the input data. The array is an
     * associative array with fieldname as key and an errormessage as value.
     */
    var $m_errors = Array();

    var $m_default_order = "";

    /**
     * Node flags
     */
    var $m_flags;

    /**
     * parent Attribute flag (treeview)
     */
     var $m_parent;

    /**
     * Record filters
     */
    var $m_filters = Array();
    var $m_fuzzyFilters = Array();

    /**
     * For speed, we keep a list of fields we don't have to load in recordlists.
     */
    var $m_listExcludes = Array();

    /**
     * For speed, we keep a list of fields which are multilangual.
     * We also want to know if there is a multilanguage select box.
     */
    var $m_listMlAdd = Array();
    var $m_listMlEdit = Array();
    var $m_hasMlSelectAdd = 0;
    var $m_hasMlSelectEdit = 0;
    
    /**
     * Actions are mapped to security units. For example, both actions "save" and "add"
     * require access "add". If an item is not in this list, it's treated 'as-is'.
     */
    var $m_securityMap = array("save"=>"add",
                               "update"=>"edit",
                               "copy"=>"add");
                               
    /*
     * Nodes can specify actions that require no access level 
     * Note: for the moment, the "select" action is always allowed.
     * TODO: This may not be correct. We have to find a way to bind the 
     * select action to the action that follows after the select.
     */
    var $m_unsecuredActions = array("select");                                   

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
      $this->m_recordLoaded = 0;
    }

    /**
     * Add an atkAttribute to the node ($attribute should be an object of type
     * atkAttribute or one of its derivatives)
     * @param $attribute the attribute you want to add
     */
    function addAttribute(&$attribute)
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
      

      $this->m_attribList[$attribute->fieldName()]=&$attribute;

      if ($attribute->hasFlag(AF_PRIMARY))
      {
        $this->m_primaryKey[] = $attribute->fieldName();
      }

      // check for parent fieldname (treeview)
      if($attribute->hasFlag(AF_PARENT))
      {
        $this->m_parent = $attribute->fieldName();
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
    function primaryKey($rec="")
    {
      if (!is_array($rec)) $rec = $this->m_records[$this->m_currentRec];
      $primKey="";
      $nrOfElements = count($this->m_primaryKey);
      for ($i=0;$i<$nrOfElements;$i++)
      {
//        $primKey.=$this->m_table.".".$this->m_primaryKey[$i]."='".$this->m_records[$this->m_currentRec][$this->m_primaryKey[$i]]."'";
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
     * Sets the module name a certain node is part off.
     * @param $module the module name
     */
//    function setModule($module)
//    {
//      $this->m_module = $module;
//    }

    /**
     * Get the relative path for this node
     * (this does *not* include the trailing slash!)
     */
//    function getPath()
//    {
//      global $config_module_path;
      
//      /* path for module */
//      if (!empty($this->m_module))
//      {
//        $path = "$config_module_path/".$this->m_module;
//        return $path;
//      }
      
      /* default path */
//      return "./";
//    }

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
    function editPage()
    {
      global $g_layout;
      $g_layout->register_script("atk/javascript/formfocus.js");
      $g_layout->output('<br>');
      $g_layout->ui_top(text('title_'.$this->m_type.'_edit'));
      $g_layout->output('<form name="entryform" enctype="multipart/form-data" action="dispatch.php?'.SID.'"'.
                                   ' method="post"'.(sizeof($this->m_listMlEdit) > 0 ? ' onsubmit="submitSave(this)"' : '').'>');
      $g_layout->output(session_form());
      $forceList = decodeKeyValueSet($this->m_postvars['atkfilter']);
      $g_layout->output($this->editForm("edit",$forceList));
      $g_layout->output('<br><input type="submit" value="'.text('saveandclose').'">');
      $g_layout->output('<input type="submit" name="atknoclose" value="'.text('save').'">');
      $g_layout->output('<input type="submit" name="atkcancel" value="'.text('cancel').'">');
      $g_layout->output('</form>');
      $g_layout->ui_bottom();
      $g_layout->output('<SCRIPT LANGUAGE="JavaScript">placeFocus()</SCRIPT>');
    }
    
    /**
     * Creates an view (=readonly) page
     */
    function viewPage()
    {
      global $g_layout;  
    
      $g_layout->output('<br>');
      $g_layout->ui_top(text('title_'.$this->m_type.'_view'));

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
        	         
          $editsrc=$p_attrib->display($this->m_records[$this->m_currentRec]);
                        
          $page.=$g_layout->ret_td($editsrc,'colspan="'.$tdcount.'" valign="top"');	    
          $page.='</tr>';
        }
                
      }
      $page.='</table>';    
            
      $page.='<br><br>'.href("dispatch.php",text('back'),SESSION_BACK);
      $g_layout->output($page);
      
      $g_layout->ui_bottom();
    }    

    /**
     * Creates an add page
     */
    function addPage()
    {
      global $g_layout;
      $g_layout->register_script("atk/javascript/formfocus.js");
      $g_layout->output('<br>');
      $g_layout->ui_top(text('title_'.$this->m_type.'_add'));
      $g_layout->output('<form name="entryform" enctype="multipart/form-data" action="dispatch.php?'.SID.'"'.
                                   ' method="post"'.(sizeof($this->m_listMlAdd) > 0 ? ' onsubmit="submitSave(this)"' : '').'>');
      $g_layout->output(session_form());                                   
      $forceList = decodeKeyValueSet($this->m_postvars['atkfilter']);
      $g_layout->output($this->editForm("add",$forceList));
      $g_layout->output('<br><input type="submit" value="'.text('save').'">');
      $g_layout->output('</form>');
      $g_layout->ui_bottom();
      $g_layout->output('<SCRIPT LANGUAGE="JavaScript">placeFocus()</SCRIPT>');
    }

    /**
     * Function outputs a form in which the current record can be edited.
     * or, if there is no current record, defaults from the postvars will be read.
     * @param $mode Mode of the form
     * @param $forcelist ?
     * @param $suppresslist ?
     */
    function editForm($mode="add",$forceList="",$suppressList="")
    {
      global $g_layout;
      
      
      if (($mode == 'add' && sizeof($this->m_listMlAdd) > 0 && !$this->m_hasMlSelectAdd) ||
         ($mode == 'edit' && sizeof($this->m_listMlEdit) > 0 && !$this->m_hasMlSelectEdit))
      {
        $selector = new atkMlSelectorAttribute();
        array_unshift($this->m_attribList, $selector);
      }

//      $form.=$this->stickyForm();

      $defaults = $this->m_records[$this->m_currentRec];

      $pk = $this->primaryKey();

      if ($this->m_action=="edit")
      {
        $form.='<input type="hidden" name="atkaction" value="update">';
        $form.='<input type="hidden" name="atkselector" value="'.rawurlencode($pk).'">';
        //$defaults = $this->m_records[$this->m_currentRec];

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
        $form.='<input type="hidden" name="atkaction" value="save">';
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
      
      $form.='<input type="hidden" name="atkorgkey" value="'.$pk.'">';

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

        reset($this->m_errors);
        while(list($attribname,$msg)=each($this->m_errors))
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
          $form.=$p_attrib->hide($defaults);
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
              $form.=$p_attrib->hide($defaults);
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
            	                    
            if ($p_attrib->hasFlag(AF_READONLY))
	          {
              // readonly, display value..
	            $editsrc=$p_attrib->hide($defaults);
              $editsrc.=$p_attrib->display($defaults);
            }
	          else
            {
              $funcname = $p_attrib->m_name."_edit";
              
              if (method_exists($this,$funcname))
              {
                $editsrc = $this->$funcname($defaults);
              }
              else
              {                		
                $editsrc = $p_attrib->edit($defaults);
              }
	          }
            
            $form.=$g_layout->ret_td($editsrc,'colspan="'.$tdcount.'" valign="top"');	    
            $form.='</tr>';
          }
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
      $limit    = (int)$this->m_postvars['atklimit'];
      $count    = (int)$this->countDb($this->m_postvars['atkfilter'], $this->m_listExcludes);
      
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
        $nav = href('dispatch.php?atkstartat='.$newstart,text('previous'))."&nbsp;|&nbsp;";
      }

      for ($i = $begpg; $i <= $endpg; $i++)
      {      
        $nav .= ($i == $curr) ? "<b>$i</b>" : href('dispatch.php?atkstartat='.max(0, ($i-1) * $limit),"$i");
        if ($i != $endpg) $nav .= "&nbsp;|&nbsp;";
      }

      if ($curr < $pages)
      {
        $newstart = $this->m_postvars['atkstartat'] + $limit;;        
        $nav .= "&nbsp;|&nbsp;".href('dispatch.php?atkstartat='.$newstart,text('next'));
      }

      return $nav;
    }


    /**
     * Admin page displays records and the actions that can be performed on
     * them (edit, delete)
     */
    function adminPage()
    {
      global $g_securityManager, $g_layout;

      $g_layout->ui_top(text('title_'.$this->m_type.'_admin'));

      $g_layout->output('<br>');
      
      $adminHeader = $this->adminHeader();
      if ($adminHeader!="")
      {
        $g_layout->output($adminHeader."<br><br>");
      }

      // When there's a lot of data, records will be spread across multiple
      // pages.
      if ($this->m_postvars['atklimit']=="") $this->m_postvars['atklimit']=atkconfig("recordsperpage");

      if ($this->m_postvars['atkstartat']=="") $this->m_postvars['atkstartat']=0;

      $this->selectDb($this->m_postvars['atkfilter'],$this->m_postvars['atkorderby'],array("offset" => $this->m_postvars['atkstartat'], "limit" => $this->m_postvars['atklimit']),$this->m_listExcludes);

      if ($this->hasFlag(NF_ADD_LINK) && !$this->hasFlag(NF_NO_ADD) && $this->allowed("add"))
      {
        $addurl = 'dispatch.php?atkaction=add';
        $g_layout->output(href($addurl,text("clickheretoadd_prefix").text($this->m_type).text("clickheretoadd_postfix"),SESSION_NESTED).'<br><br>');
      }

      // create navigation bar
      $nav = $this->buildNavigation();
      if (!empty($nav)) $g_layout->output("$nav<br><br>");

      $actions = Array();

      if (!$this->hasFlag(NF_NO_EDIT)&&$this->allowed("edit"))
      {
        $actions[]=href('dispatch.php?atkaction=edit&atkselector=[pk]',text('edit'),SESSION_NESTED);
      }
      else
      {
        // if you may not edit, maybe you are allowed to view..
        if (!$this->hasFlag(NF_NO_VIEW)&&$this->allowed("view"))
        {
          $actions[]=href('dispatch.php?atkaction=view&atkselector=[pk]',text('view'),SESSION_NESTED);
        }
      }
      if (!$this->hasFlag(NF_NO_DELETE)&&$this->allowed("delete"))
      {
        $actions[]=href('dispatch.php?atkaction=delete&atkselector=[pk]',text('delete'),SESSION_NESTED);
      }
      if($this->hasFlag(NF_COPY)&&$this->allowed("copy"))
      {
        $actions[]=href('dispatch.php?atkaction=copy&atkselector=[pk]',text('copy'),SESSION_NESTED);
      }

      $g_layout->output($this->recordList($actions));

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
    function recordList($actions,$sortable=true,$suppressList="")
    {
      global $g_layout;
      $output = $g_layout->data_top();

      $output.="<tr>";
   
      // stuff for the totals row..
      $totalisable = false;
      $totals = Array();      

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
            $tmp = href('dispatch.php?atkorderby='.$neworder,text($p_attrib->fieldName()));
          }
          else $tmp = text($p_attrib->fieldName());

          $output.=$g_layout->ret_td_datatitle($tmp);
          
          // the totalisable check..
          if ($p_attrib->hasFlag(AF_TOTAL))
          {
            $totalisable = true;
          }

        }
      }      

      // Searchrow.. 
      if (!$this->hasFlag(NF_NO_SEARCH))
      {
        $rand = rand(1,1000);
        
        // The above row was used to move the browserwindow to the bottom of the screen,
        // so a user doesn't have to scroll down after each search. But there's some stupid
        // thing in internet explorer that it doesn't post the form anymore after you've
        // used it once.
        $searchRow = '<tr><a name="searchform"><form action="dispatch.php?'.SID.'" method="post">';
        $searchRow.= session_form();
        $searchable = false;

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

        $searchRow.=$g_layout->ret_td_datatitle('<input type="submit" value="'.text("search").'">');
        $searchRow.='<input type="hidden" name="atkaction" value="'.$this->m_action.'">';
        //$searchRow.=$this->stickyForm();
        $searchRow.="</form></tr>";

      }

      // one empty title which comes on top of the 'edit/delete/...' column
      if (count($actions)>0||$searchable)
      {
        $output.=$g_layout->ret_td_datatitle();
      }

      $output.="</tr>";

      if ($searchable) $output.=$searchRow;

      for ($this->m_currentRec=0;$this->m_currentRec<count($this->m_records);$this->m_currentRec++)
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
              $output.=$g_layout->ret_td($this->$funcname($this->m_records[$this->m_currentRec]));
            }
            else
            {
              // otherwise, the display function of the particular attribute
              // is called.
              $output.=$g_layout->ret_td($p_attrib->display($this->m_records[$this->m_currentRec]));
            }
            
            // Calculate totals..
            if ($p_attrib->hasFlag(AF_TOTAL))
            {
              $totals[$attribname] = $p_attrib->sum($totals[$attribname], $this->m_records[$this->m_currentRec]);
            }
          }
        }
        // the functionality list:        
        // TODO: pick some cool little icons instead of text        
        if (count($actions)>0)
        {
          reset($actions);
          $stractions = "";
          $pk = $this->primaryKey();
          for ($j=0;$j<count($actions);$j++)
          {
            $action=str_replace('[pk]',rawurlencode($pk),$actions[$j]);
            $action=stringparse($action,$this->m_records[$this->m_currentRec],true);
            $stractions.=$action.'&nbsp;';
          }
          $output.=$g_layout->ret_td($stractions);
        }
        else
        {
          if ($searchable)
          {
            $output.=$g_layout->ret_td("&nbsp;");
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
              $totalRow.=$g_layout->ret_td_datatitle($p_attrib->display($totals[$attribname]));
            }
            else
            {
              $totalRow.=$g_layout->ret_td_datatitle();
            }
          }
        }     
      
        // one empty title which comes below the 'edit/delete/...' column
        if (count($actions)>0||$searchable)
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
    function printableRecordList($suppressList="")
    {
      $output='<table border="1" cellspacing="0" cellpadding="4">';

      $output.="<tr>";
   
      // stuff for the totals row..
      $totalisable = false;
      $totals = Array();      

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

      for ($this->m_currentRec=0;$this->m_currentRec<count($this->m_records);$this->m_currentRec++)
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
              $value=$this->$funcname($this->m_records[$this->m_currentRec]);
            }
            else
            {
              // otherwise, the display function of the particular attribute
              // is called.              
              $value=$p_attrib->display($this->m_records[$this->m_currentRec]);
            }
            $output.='<td>'.($value==""?"&nbsp;":$value).'</td>';
            
            // Calculate totals..
            if ($p_attrib->hasFlag(AF_TOTAL))
            {
              $totals[$attribname] = $p_attrib->sum($totals[$attribname], $this->m_records[$this->m_currentRec]);
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
      global $g_layout;

      // When there's a lot of data, records will be spread across multiple
      // pages.
      if ($this->m_postvars['atklimit']=="") $this->m_postvars['atklimit']=atkconfig("recordsperpage");

      if ($this->m_postvars['atkstartat']=="") $this->m_postvars['atkstartat']=0;

      $this->selectDb($this->m_postvars['atkfilter'],$this->m_postvars['atkorderby'],array("offset" => $this->m_postvars['atkstartat'], "limit" => $this->m_postvars['atklimit']),$this->m_listExcludes);
      
      if (count($this->m_records)==1 && $this->hasFlag(NF_AUTOSELECT))
      {
        // There's only one record and the autoselect flag is set, so we 
        // automatically go to the target..
        $target=stringparse($this->m_postvars['atktarget'],$this->m_records[0],true);
        $this->redirect($target);
      }
      else
      {
        $g_layout->ui_top(text('title_'.$this->m_type.'_select'));
        $g_layout->output('<br>');
      
        $g_layout->output('<br>'.text($this->m_type.'_select').'<br>');
        $g_layout->output('<br>');
        
        // create navigation bar
        $nav = $this->buildNavigation();
        if (!empty($nav)) $g_layout->output("$nav<br><br>");

        $actions=Array(href($this->m_postvars['atktarget'],text('select')));

        $g_layout->output($this->recordList($actions));

        if (!empty($nav)) $g_layout->output('<br>'.$nav.'<br><br>');

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
      global $g_layout;
      
      $g_layout->ui_top(text('title_delete'));

      $g_layout->output('<form action="dispatch.php?"'.SID.' method="post">');
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
          switch ($this->m_postvars['atkaction'])
          {
            case "edit":
              $this->selectDb($this->m_postvars['atkselector']);
              $this->editPage();
              break;
            case "view":
              $this->selectDb($this->m_postvars['atkselector']);
              $this->viewPage();
              break;              
            case "add":
              $this->addPage();
              break;
            case "update":
              if ($this->m_postvars['atkcancel']=="")
              {
                $this->updateRecord();
                $this->validate();
                if (count($this->m_errors)>0)
                {
                  $this->m_action="edit";
                  $this->editPage();
                }
                else
                {
                  $this->updateDb();
                  if ($this->m_postvars['atknoclose']=="")
                  {
                    // 'save and close' was clicked
                    $this->redirect();
                  }
                  else
                  {
                    // 'save' was clicked
                    $this->m_action="edit";
                    $this->editPage();
                  }
                }
              }
              else
              {
                // Cancel was pressed
                $this->redirect();
              }
              break;
            case "save":
              $this->updateRecord();
              $this->validate();
              if (count($this->m_errors)>0)
              {
                $this->addPage();
              }
              else
              {
                $this->addDb();
                $this->redirect();
              }
              break;
            case "delete":
              if ($this->m_postvars['confirm']==text('yes'))
              {
                // Confirmation page was displayed and 'yes' was clicked
                $this->deleteDb($this->m_postvars['atkselector']);
                $this->postDel($this->currentRec());
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
              break;
            case "copy":
              $this->copyDb($this->m_postvars['atkselector']);
              $this->redirect();
            case "admin":
              if ($this->hasFlag(NF_NO_ADD)==false&&$this->allowed("add"))
              {
                if (!$this->hasFlag(NF_ADD_LINK)) // otherwise, in adminPage, an add link will be added.
                {
                  $this->addPage();
                }
              }
              $this->adminPage();
              
              break;
            case "select":
              $this->selectPage();
              break;
            case "xml":
              if ($this->m_postvars['atkselector']!="")
              {
                 $this->selectDb($this->m_postvars['atkselector']);
                $this->xml();
              }
              else
              {
                $this->selectDb();
                for ($this->m_currentRec=0;$this->m_currentRec<count($this->m_records);$this->m_currentRec++)
                {
                  $this->xml();
                }
              }
              break;
            default:
              atkdebug("dispatcher error: no action defined for '".$this->m_postvars['atkaction']."'");
              break;
            // TODO: also, admin should not always have an 'addform' and 'adminPage'. This could be
            // customised.
          }
        }
      }
      else
      {
        $g_layout->output("Sorry, you don't have permission to perform this action");
      }
      $g_layout->page(text('app_shorttitle')." - ".text('title_'.$this->m_type.'_'.$this->m_postvars['atkaction']));
      
      // This is the end of all things for this page..
      // so we clean up some resources..
      $g_db->disconnect();
      atkdebug("disconnected from the database");
    }

    /**
     * Make browser of the user go to another page. This should be called before any call
     * to layout::outputFlush();
     */
    function redirect($location="")
    {
      atkdebug("atknode::redirect()");
      $back = false;
      
      if ($location=="")
      {      
        // The page we redirect to is depending on the action we are doing.
        switch ($this->m_action)
        {
          case "save":
            if ($this->hasFlag(NF_EDITAFTERADD))              
            {
              // todo: FIXME
              $location = session_url('dispatch.php?atkaction=edit&atkselector='.rawurlencode($this->primaryKey()),SESSION_NESTED);
            } 
            else
            {
              $back = true;
            }
            break;
          default:
            $back = true;
            break;
        }       
        
        if ($back)
        {
          $location = session_url("dispatch.php",SESSION_BACK);
        }     
        else
        {
          $location = session_url("dispatch.php",SESSION_DEFAULT);
        }

      }
      
  
      if (atkconfig("debug")>=2)
      {
        atkdebug('nondebug version would have redirected to <a href="'.$location.'">'.$location.'</a>');
      }
      else
      {
        header('Location: '.$location);
      }
    }

    /**
     * Parse xml tags
     */
    function xml()
    {
      global $g_layout;  
    
      $xml = "<".$this->m_type." ";

      foreach (array_keys($this->m_attribList) as $attribname)
      {
        $p_attrib = &$this->m_attribList[$attribname];
        if ($this->m_records[$this->m_currentRec][$p_attrib->fieldName()]!="")
        {
          $xml.=$attribname.'="'.$this->m_records[$this->m_currentRec][$p_attrib->fieldName()].'" ';
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
    function updateRecord()
    {
      foreach (array_keys($this->m_attribList) as $attribname)
      {
        $p_attrib = &$this->m_attribList[$attribname];
        $this->m_records[$this->m_currentRec][$p_attrib->fieldName()]=$p_attrib->fetchValue($this->m_postvars);
      }
    }

    /**
     * Search for descriptors of the fields
     * @return array with fieldnames
     */
    function descriptorFields()
    {
      $fields = Array();

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
     * Check if there's a record which has $fieldname = $value
     * todo: check multiple fields at the same time
     * @return Boolean true or false
     */
    function contains($fieldname,$value)
    {
      for ($i=0;$i<count($this->m_records);$i++)
      {
        if ($this->m_records[$i][$fieldname]==$value) return true;
        // todo: attributes may have their own way of comparing values..
      }
      return false;
    }

    /**
     * Search for descriptor in custom descriptor definition, else first attribute of a node
     * @return descriptor
     */
    function descriptor($rec="")
    {
      if (!is_array($rec)) $rec = $this->m_records[$this->m_currentRec];
    
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
    function validate()
    {
      global $g_db;
      atkdebug("validate()");
      foreach (array_keys($this->m_attribList) as $attribname)
      {
        $p_attrib = &$this->m_attribList[$attribname];
        // validate obligatory fields (but not the auto_increment ones, because they don't have a value yet)
        if ($p_attrib->hasFlag(AF_OBLIGATORY) && !$p_attrib->hasFlag(AF_AUTO_INCREMENT) && $p_attrib->isEmpty($this->m_postvars))
        {
          $this->m_errors[$attribname] = text('error_obligatoryfield');
        }
        else if ($p_attrib->hasFlag(AF_UNIQUE) &&
                 count($g_db->getrows("SELECT ".$p_attrib->fieldName()." FROM ".$this->m_table." WHERE $attribname='".$this->m_postvars[$attribname]."' AND NOT (".$this->primaryKey().")"))>0
                )
        {
          $this->m_errors[$attribname] = text('error_uniquefield');
        }
        else
        {
          $funcname = $p_attrib->m_name."_validate";

          if (method_exists($this,$funcname))
          {
            $error = $this->$funcname($this->m_postvars[$attribname]);
            if ($error!="") $this->m_errors[$attribname] = $error;
          }
          else
          {
            $error = $p_attrib->validate($this->m_postvars[$attribname]);
            if ($error!="") $this->m_errors[$attribname] = $error;
          }
        }
      }
    }

    /**
     * Write the current record to the database (if it doesn't exist already,
     * use addDb()).
     */
    function updateDb()
    {
      global $g_db;

      $name = "atk".atkconfig("database")."query";
      $query = new $name();

      $query->addTable($this->m_table);
      //$query->addCondition($this->primaryKey());
      $query->addCondition($this->m_postvars['atkorgkey']);

      $storelist = Array();

      foreach (array_keys($this->m_attribList) as $attribname)
      {
        $p_attrib = &$this->m_attribList[$attribname];        
        if ($p_attrib->hasFlag(AF_READONLY)==false && $p_attrib->hasFlag(AF_HIDE_EDIT)==false)
 	      {
          if (method_exists($p_attrib,"store"))
          {
            $storelist[]=$attribname;
          }
          else
          {
            $p_attrib->addToQuery($query,$this->m_table,"",$this->m_records[$this->m_currentRec],1,"edit"); // start at level 1
          }
	      }
      }

      $querystring = $query->buildUpdate();

      //$query.= " WHERE ;
      atkdebug("updatedb - querystring: ".$querystring);
      $g_db->query($querystring);

      // also store special storage attributes.
      for ($i=0;$i<count($storelist);$i++)
      {
        $p_attrib = &$this->m_attribList[$storelist[$i]];
        $p_attrib->store($g_db, $this->m_records[$this->m_currentRec],"update");
      }

      // Now we call a postUpdate function, that can be used to do some processing after the record
      // has been saved.
      $this->postUpdate($this->currentRec());

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
        reset($this->m_filters);
        while(list($key,$value)=each($this->m_filters))
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
        while (list($key,$value) = each($searchArray))
        {
          if ($value!="")
          {
            $query->addCondition($this->m_table.".".$key." LIKE '%".$value."%'");
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
      atkdebug("countDb() - query: ".$querystring);
      $g_db->query($querystring);
      $result = $g_db->getrows($querystring);
      return $result[0][0];
    }

   /** Copies a record
        *
        *@param $selector The 'where' clause that indicates which records to select.
        */
    function copyDb($selector)
    {
      
      $this->selectDb($selector);
      $tmprecs = $this->m_records;
      if(count($tmprecs)>0)
      {      
        $this->addDb();
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
      
      $this->m_records = Array();
      $selectlist = Array();
      $loadlist = Array();

      if($order=="" && $this->m_default_order!="") $order=$this->m_default_order;

      $name = "atk".atkconfig("database")."query";
      $query = new $name();

      $query->addTable($this->m_table);
      $query->addCondition($selector);

      if (!$this->hasFlag(NF_NO_FILTER))
      {
        /* hard filters may be set */
        reset($this->m_filters);
        while(list($key,$value)=each($this->m_filters))
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
        while (list($key,$value) = each($searchArray))
        {
          if ($value!="")
          {
            $query->addCondition($this->m_table.".".$key." LIKE '%".$value."%'");
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
      atkdebug("selectDb() - query: ".$querystring);

      $this->recordLoaded = 1;

      //$this->m_records = $this->m_db->getrows($querystring);

      if (is_array($limit) && count($limit) == 2) $g_db->query($querystring, $limit["offset"], $limit["limit"]);
      else $g_db->query($querystring);

      while ($g_db->next_record())
      {        
        $therecord = Array();
        $dbrecord = $g_db->m_record;        
        $query->deAlias($dbrecord); // dereference aliases..
        atkDataDecode($dbrecord);
        reset($this->m_attribList);
        foreach (array_keys($this->m_attribList) as $attribname)
        {
          $p_attrib = &$this->m_attribList[$attribname];        
          $therecord[$attribname] = $p_attrib->db2value($dbrecord);
        }
        $this->m_records[] = $therecord;
      }

      // also load special storage attributes.
      for ($i=0;$i<count($loadlist);$i++)
      {
        for ($j=0;$j<count($this->m_records);$j++)
        {
          $p_attrib = &$this->m_attribList[$loadlist[$i]];
          $this->m_records[$j][$loadlist[$i]] = $p_attrib->load($g_db, $this->m_records[$j]);
        }
      }
    }


   /**
    * Add this node to a query. (mostly used when you have to join two nodes in a relation.
    */
    function addToQuery(&$query, $alias="", $level=0)
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

      $usedFields = atk_array_merge($this->descriptorFields(),$this->m_primaryKey);

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
          if ($usefieldalias) $fieldaliasprefix = $alias."_AMDAE_";
          if (!is_object($p_attrib))
          {
            atkdebug($p_attrib->m_name." is not an object?! Check your descriptor_def for non-existant fields");
          }
          else
          {
            $p_attrib->addToQuery($query,$alias, $fieldaliasprefix,"",$level+1, "select");
          }
        }
      }

    }

    /**
     * Return the current record as an array.
     */
    function currentRec()
    {
      return $this->m_records[$this->m_currentRec];
    }

    /**
     * Advance the pointer to the next record.
     * If you want to loop all records, first call resetRec();
     * example:
     * $this->resetRec();
     * while ($this->nextRec())
     * {
     *    var_dump($this->currentRec());
     * }
     */
    function nextRec()
    {
      if ($this->m_currentRec >= (count($this->m_records)-1))
      {
        return false;
      }
      $this->m_currentRec++;
      return true;
    }

    /**
     * Set the currentRecord to -1, which means that the next call to
     * nextRec will set the pointer to the first record.
     */
    function resetRec()
    {
      $this->m_currentRec = -1;
    }

    /**
     * Save the current record to the database.
     */
    function addDb()
    {
      global $g_db;
      
      $name = "atk".atkconfig("database")."query";
      $query = new $name();

      $storelist = Array();
      $querylist = Array();

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
          $this->m_records[$this->m_currentRec][$p_attrib->fieldName()]=$g_db->nextid($this->m_seq);     
        }

        $p_attrib->addToQuery($query,$this->m_table,"",$this->m_records[$this->m_currentRec],1,"add"); // start at level 1
      }
                 

      // also store special storage attributes.
      reset($this->m_attribList);
      for ($i=0;$i<count($storelist);$i++)
      {            
        $p_attrib = &$this->m_attribList[$storelist[$i]];
      
        $p_attrib->store($g_db, $this->m_records[$this->m_currentRec],"add");
       
      }

      $querystring = $query->buildInsert();
      atkdebug("adddb - querystring: ".$querystring);
      $g_db->query($querystring);

      // Now we call a postAdd function, that can be used to do some processing after the record
      // has been saved.
      $this->postAdd($this->m_records[$this->m_currentRec]);

      atkdebug($g_db->m_error);
    }

    /**
     * delete record from the database.
     * todo: instead of delete, set the deleted flag.
     * @param $selector Selector
     */
    function deleteDb($selector)
    {
      global $g_db;

      $this->selectDb($selector);

      for ($i=0;$i<count($this->m_records);$i++)
      {
        foreach (array_keys($this->m_attribList) as $attribname)
        {
          $p_attrib = &$this->m_attribList[$attribname];
          if ($p_attrib->hasFlag(AF_CASCADE_DELETE))
          {
            $p_attrib->delete($this->m_records[$i]);
          }
        }

      }

      $query = "DELETE FROM ".$this->m_table." WHERE ".$selector;
      atkdebug("deleteDb - query: ".$query);
      $g_db->query($query);
      // todo: instead of delete, set the deleted flag.
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
      
      return (in_array($this->securityKey($action), $this->m_unsecuredActions)
              || $g_securityManager->allowed($this->m_type,$this->securityKey($action)));
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
    function addStickyVar($varname,$global=true)
    {
      if (is_array($varname))
      {
        $this->m_stickyVars = atk_array_merge($this->m_stickyVars,$varname);
      }
      else
      {
        $this->m_stickyVars[] = $varname;
      }   
    }
  
   /**
    * This function gets the value of a sticky var. There are several scopes
    * in which the value could have been defined. They are searched in the 
    * order $this->m_postvars, $HTTP_GET_VARS, $HTTP_POST_VARS, $GLOBALS.
    */
    function getStickyValue($key)
    {
      global $HTTP_GET_VARS, $HTTP_POST_VARS;
      if ($this->m_postvars[$key]!="") return $this->m_postvars[$key];
      if ($HTTP_GET_VARS[$key]!="") return $HTTP_GET_VARS[$key];
      if ($HTTP_POST_VARS[$key]!="") return $HTTP_POST_VARS[$key];
      if ($GLOBALS[$key]!="") return $GLOBALS[$key];
      return "";
    }      
    
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

  }
  
  require "atk/class.atktreenode.inc";


} // endif multiple inclusion protector

?>
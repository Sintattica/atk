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
   * Revision 4.60  2001/06/22 12:09:23  ivo
   * Fixed a bug in [pk] replacement in encoded actions..
   *
   * Revision 4.59  2001/06/22 11:44:42  ivo
   * Workaround for IE4 'bug' that causes javascript to decode urlencoded
   * values that it shouldn't encode: atkurlencode and atkurldecode.
   *
   * Revision 4.58  2001/06/19 14:00:01  ivo
   * searchform posts to phpself instead of dispatch.php
   *
   * Revision 4.57  2001/06/19 09:04:32  peter
   * viewPage(...) allows you now to override a attribute's display method
   *
   * Revision 4.56  2001/06/18 10:05:51  ivo
   * New attribute: atkProfileAttribute, for editing security profiles.
   * New node flag: NF_NO_SECURITY
   *
   * Revision 4.55  2001/06/14 12:30:21  ivo
   * Moved postDel to inside deleteDb, instead of in action_delete.
   *
   * Revision 4.54  2001/06/13 10:40:11  martin
   * Dirty evil hack :)
   *
   * Revision 4.53  2001/06/12 15:05:16  ivo
   * New feature: NF_TRACK_CHANGES nodeflag passes the original record
   * to postUpdate.
   *
   * Revision 4.52  2001/06/12 13:19:55  ivo
   * Added a <br> to the bottom of the treeview.
   * Removed save and close button when editpage is bottom of the stack.
   *
   * Revision 4.51  2001/06/12 12:26:54  ivo
   * Fix to treenode: if no icondir is set, it takes the default.
   * Fix to atknode: display a '<br'> if no navigation is active.
   *
   * Revision 4.50  2001/06/08 14:33:30  ivo
   * Optimisations to atktreenode.
   * Smaller imp of row color alternationi feature.
   *
   * Revision 4.49  2001/06/08 14:19:59  paul
   * datatable made themable
   *
   * Revision 4.48  2001/06/07 12:36:26  ivo
   * Atknode now reads records atomic.
   * Removed some debug echo's from the db classes.
   *
   * Revision 4.47  2001/06/07 09:26:16  ivo
   * removed a whitespace (how do these things keep getting in again :()
   *
   * Revision 4.46  2001/06/07 09:24:49  ivo
   * Updated language files.
   * Support for non-western-european charsets. (backport from 0.6.4)
   *
   * Revision 4.45  2001/06/01 12:52:49  peter
   * Fixed a bug in menu.php (atksecure() was called to late).
   * Updated english language file month names (long names to short).
   * Fixed bug in atknode selectPage() -> atktarget was not decoded.
   *
   * Revision 4.44  2001/05/30 07:05:43  ivo
   * Removed is_null funtion calls (because they require PHP4.0.4)
   * Moved session management initialisation code to a function.
   *
   * Revision 4.43  2001/05/23 09:46:28  ivo
   * SelectPage now also parses encoded action templates.
   *
   * Revision 4.42  2001/05/22 15:22:39  peter
   * Different small enhancements fixes...
   *
   * Revision 4.41  2001/05/21 15:18:26  ivo
   * Made securitymanager cookie and session based. Authentication now only
   * executes once per session.
   *
   * Revision 4.40  2001/05/18 09:14:27  ivo
   * atkaction is now a sessionvar instead of pagevar.
   *
   * Revision 4.39  2001/05/18 07:49:42  ivo
   * Fixes to sessionmanagement.
   *
   * Revision 4.38  2001/05/17 15:19:18  peter
   * Security manager now knows how to handle module nodes.
   *
   * Revision 4.37  2001/05/17 12:28:46  ivo
   * Committed the wrong pageVars in previous commit.
   *
   * Revision 4.36  2001/05/17 12:24:32  ivo
   * I'm stupid. Ofcourse atkselector is a sessionvar. just not a pagevar per
   * se.
   *
   * Revision 4.35  2001/05/17 12:20:36  ivo
   * removed atkselector from sessionvars.. it's not a session var!
   *
   * Revision 4.34  2001/05/17 12:09:11  ivo
   * Improvements to session mangement: search values and page numbers are
   * now remembered if you come back to the admin page.
   *
   * Revision 4.33  2001/05/17 10:27:12  ivo
   * Fixes to session management, new AF_LARGE feature for manytoonerelation.
   * Improved handling of GET_VARS and POST_VARS (requires a new dispatcher)
   *
   * Revision 4.32  2001/05/17 08:06:33  ivo
   * Fixes to atkfileattribute and onetoonerelation.
   *
   * Revision 4.31  2001/05/16 06:52:24  ivo
   * removed php short-tags.
   *
   * Revision 4.30  2001/05/14 10:20:19  ivo
   * Improved error reporting when database queries fail.
   * New feature: atk can now send a detailed error report by email.
   *
   * Revision 4.29  2001/05/11 15:24:54  peter
   * Some updates for the module support and some other things...
   *
   * Revision 4.28  2001/05/11 14:59:56  ivo
   * Fixed serious bug in onetoone relation that could overwrite records in a
   * table..
   *
   * Revision 4.27  2001/05/11 12:25:52  ivo
   * Fixed bugs in atkOneToOneRelation and atkManyToManyRelation (bug only
   * appeared when the two were used in combination)
   *
   * Revision 4.26  2001/05/10 14:18:49  ivo
   * Fixed an includd_once typo.
   *
   * Revision 4.25  2001/05/10 09:48:07  ivo
   * Added a cancel buttons to add forms.
   *
   * Revision 4.24  2001/05/10 09:42:28  sandy
   * bugfix in menu.php (config menu delimiter) and for the clear posting
   *
   * Revision 4.23  2001/05/10 09:16:47  ivo
   * Fix to formsession.
   *
   * Revision 4.22  2001/05/10 08:31:00  ivo
   * Major upgrade. Changes:
   * * Deprecated the m_records/m_currentRec feature of atknode. Nodes are now
   *   singletons by default, and nodefunctions pass around recordsets.
   * + Session management for forms. If you now leave a page through a click on
   *   a link, the session remembers everything from your form and restores it
   *   when you return.
   * + New relation: oneToOneRelation
   * + Reimplemented the embedded editForm feature (forms inside forms)
   *
   * Revision 4.21  2001/05/07 15:13:49  ivo
   * Put config_atkroot in all files.
   *
   * Revision 4.20  2001/05/07 14:47:32  ivo
   * fix in atkroot.
   *
   * Revision 4.19  2001/05/07 14:46:15  ivo
   * Atk now has a configurable rootdirectory (specified in dispatcher)
   *
   * Revision 4.18  2001/05/04 11:37:42  sandy
   * small bug fixes (split function, refresh, treeview,etc)
   *
   * Revision 4.17  2001/05/03 07:47:35  ivo
   * Stacktrace is now only displayed once per page..
   *
   * Revision 4.16  2001/05/02 14:55:46  ivo
   * New cool feature: stacktrace in the topright screen (navigate through
   * pages you visited before)
   *
   * Revision 4.15  2001/05/02 13:09:48  ivo
   * fix to editafteradd session handling.
   *
   * Revision 4.14  2001/05/02 13:06:23  sandy
   * fixed the AF_CASCADE_DELETE flag
   *
   * Revision 4.13  2001/05/02 12:44:21  ivo
   * multilevel session_back feature.
   * new relational features: maxRecords for oneToManyRelation, filter for
   * onetomanyrelation.
   *
   * Revision 4.12  2001/05/01 12:08:38  ivo
   * Small bug in EDITAFTERADD flag.
   *
   * Revision 4.11  2001/05/01 09:49:49  ivo
   * Replaced all require() and include() calls by require_once() and
   * include_once() calls. The if(!DEFINED)... inclusion protection in files
   * is now obsolete.
   *
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
  require_once($config_atkroot."atk/security/class.atksecuritymanager.inc");
  require_once($config_atkroot."atk/session/class.atksessionmanager.inc");
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

   $g_pageVars = array("atksearch","atkorderby","atkstartat","atktarget","atkformdata","atktree");

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
                               "copy"=>"add");
                               
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
    var $m_stackTraceDone = false; 

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
    function addAttribute($attribute)
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
      $g_layout->output($this->stackTrace());
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
      global $g_layout;  
    
      $g_layout->ui_top(text('title_'.$this->m_type.'_view'));
      $g_layout->output($this->stackTrace());

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
            
      $page.='<br><br>'.href("dispatch.php",text('back'),SESSION_BACK);
      $g_layout->output($page);
      
      $g_layout->ui_bottom();
    }    

    /**
     * Creates an add page
     */
    function addPage($record=NULL)
    {
      global $g_layout;
      $g_layout->register_script("atk/javascript/formfocus.js");
      $g_layout->ui_top(text('title_'.$this->m_type.'_add'));
      $g_layout->output($this->stackTrace());
      
      $g_layout->output('<form name="entryform" enctype="multipart/form-data" action="dispatch.php?'.SID.'"'.
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
      global $g_layout;      
      
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
            	                    
            if ($p_attrib->hasFlag(AF_READONLY))
	          {
              // readonly, display value..              
	            $editsrc=$p_attrib->hide($defaults, $fieldprefix);
              $editsrc.=$p_attrib->display($defaults);
                              
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
      global $PHP_SELF;
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
     * Admin page displays records and the actions that can be performed on
     * them (edit, delete)
     */
    function adminPage()
    {
      global $g_securityManager, $g_layout, $PHP_SELF;

      $g_layout->ui_top(text('title_'.$this->m_type.'_admin'));
      $g_layout->output($this->stackTrace());

      $adminHeader = $this->adminHeader();
      if ($adminHeader!="")
      {
        $g_layout->output($adminHeader."<br><br>");
      }

      // When there's a lot of data, records will be spread across multiple
      // pages.
      if ($this->m_postvars['atklimit']=="") $this->m_postvars['atklimit']=atkconfig("recordsperpage");

      if ($this->m_postvars['atkstartat']=="") $this->m_postvars['atkstartat']=0;

      $recordset = $this->selectDb($this->m_postvars['atkfilter'],$this->m_postvars['atkorderby'],array("offset" => $this->m_postvars['atkstartat'], "limit" => $this->m_postvars['atklimit']),$this->m_listExcludes);

      if ($this->hasFlag(NF_ADD_LINK) && !$this->hasFlag(NF_NO_ADD) && $this->allowed("add"))
      {
        $addurl = $PHP_SELF.'?atkaction=add';
        $g_layout->output(href($addurl,text("clickheretoadd_prefix").text($this->m_type).text("clickheretoadd_postfix"),SESSION_NESTED).'<br><br>');
      }

      // create navigation bar
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
        $searchRow = '<tr><a name="searchform"><form action="'.$PHP_SELF.'?'.SID.'" method="post">';
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
      
      
      for ($i=0;$i<count($recordset);$i++)
      {  
        // We alternate the color for even and non-even rows.        
        $output.='<tr class="row'.(($i%2==0)?1:2).'">';
        
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
            $action=str_replace("_#5B","[",$actions[$j]);
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
      global $g_layout;

      // When there's a lot of data, records will be spread across multiple
      // pages.
      if ($this->m_postvars['atklimit']=="") $this->m_postvars['atklimit']=atkconfig("recordsperpage");

      if ($this->m_postvars['atkstartat']=="") $this->m_postvars['atkstartat']=0;

      $recordset = $this->selectDb($this->m_postvars['atkfilter'],$this->m_postvars['atkorderby'],array("offset" => $this->m_postvars['atkstartat'], "limit" => $this->m_postvars['atklimit']),$this->m_listExcludes);
      
      if (count($recordset)==1 && $this->hasFlag(NF_AUTOSELECT))
      {
        // There's only one record and the autoselect flag is set, so we 
        // automatically go to the target..
        $target=stringparse(rawurldecode($this->m_postvars['atktarget']),$recordset[0],true);
        $this->redirect($target);
      }
      else
      {
        $g_layout->ui_top(text('title_'.$this->m_type.'_select'));
        $g_layout->output($this->stackTrace());      
      
        $g_layout->output(text($this->m_type.'_select').'<br>');
        $g_layout->output('<br>');
        
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
      global $g_layout;
      
      $g_layout->ui_top(text('title_delete'));
      $g_layout->output($this->stackTrace());

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
      
      /** DIRTY HACK 
          if action is something else than edit, we clear the atkformdata sessionstuff **/    
      if ($this->m_action!="edit")
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
      global $g_returnurl;
    
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
          $location = session_url("dispatch.php",SESSION_BACK,$back);
        }        
        else    
        {
          if ($location=="")
          {
            $location = session_url("dispatch.php",SESSION_DEFAULT);
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
                 count($g_db->getrows("SELECT ".$p_attrib->fieldName()." FROM ".$this->m_table." WHERE $attribname='".$this->m_postvars[$attribname]."' AND NOT (".$this->primaryKey($record).")"))>0
                )
        {
          $this->m_errors[$attribname] = text('error_uniquefield');
        }
        else
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
          if ($p_attrib->hasFlag(AF_READONLY)==false && $p_attrib->hasFlag(AF_HIDE_EDIT)==false)
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

        //$query.= " WHERE ;
        atkdebug("updatedb - querystring: ".$querystring);
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
        while (list($key,$value) = each($searchArray))
        {
          if ($value!="")
          {
            $p_attrib = &$this->m_attribList[$key];
            if (is_object($p_attrib)) $condition = $p_attrib->searchCondition($this->m_table, $value);
            else $condition = $this->m_table.".".$key." LIKE '%".$value."%'";
            $query->addCondition($condition);              
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
        while (list($key,$value) = each($searchArray))
        {
          if ($value!="")
          {
            $p_attrib = &$this->m_attribList[$key];
            if (is_object($p_attrib)) $condition = $p_attrib->searchCondition($this->m_table, $value);
            else $condition = $this->m_table.".".$key." LIKE '%".$value."%'";
            $query->addCondition($condition);              
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
      atkdebug("adddb - querystring: ".$querystring);
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
      atkdebug("deleteDb - query: ".$query);
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
    
    function stackTrace()
    {
      global $g_sessionManager;
      if (atkconfig("stacktrace")&&!$this->m_stackTraceDone)
      {
        $this->m_stackTraceDone = true;
        return '<div align="right">'.$g_sessionManager->stackTrace().'</div><br>';   
      }
      return "";
    }      

  }
  
  require_once($config_atkroot."atk/class.atktreenode.inc");

?>
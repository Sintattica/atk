<?php
  /**
   * This file is part of the Achievo ATK distribution.
   * Detailed copyright and licensing information can be found
   * in the doc/COPYRIGHT and doc/LICENSE files which should be
   * included in the distribution.
   *
   * The file contains the default values for most configuration
   * settings.
   *
   * @package atk
   *
   * @copyright (c)2000-2004 Ibuildings.nl BV
   * @copyright (c)2000-2004 Ivo Jansch
   * @license http://www.achievo.org/atk/licensing ATK Open Source License
   *
   * @todo Make sure EVERY config variable is located here and properly
   * documented.
   *
   * @version $Revision$
   * $Id$
   */

  /********************* FILE LOCATIONS & PATHS ******************************/

  /**
   * The application root
   * @var String The application root
   * @todo update this bit of documentation as it doesn't really say much
   */
  $config_application_root = "/";

  if ($config_atkroot == "")
  {
    /**
     * The root of the ATK application, where the atk/ directory resides
     * @var String The root
     */
     $config_atkroot = "./";
  }

  /**
   * Module path (without trailing slash!) where the modules directory resides,
   * defaults to the atk root, so not IN the atk/ directory, but more likely in
   * the application root
   * @var String
   */
  $config_module_path = $config_atkroot."modules";

  $config_corporate_node_base = "";
  $config_corporate_module_base = "";

  /**
   * The location of a directory that is writable to ATK and that ATK can
   * store it's temporary files in.
   * @var String
   */
  $config_atktempdir = $config_atkroot."atktmp/";

  /**
   * The location of the module specific configuration files.
   * @var String
   */
  $config_configdir = $config_atkroot."configs/";

  /**
   * Use the given meta policy as the default meta policy
   * @var String
   */
  $config_meta_policy = "atk.meta.atkmetapolicy";

  /**
   * Use the given meta handler as the default meta handler
   * @var String
   */
  $config_meta_handler = "atk.meta.atkmetahandler";

  /************************** DATABASE SETTINGS ******************************/

  /**
   * The IP or hostname of the database host
   * @var String
   */
  $config_db["default"]["host"] = "localhost";

  /**
   * The name of the database to use
   * @var String
   */
  $config_db["default"]["db"] = "";

  /**
   * The name of the database user with which to connect to the host
   * @var String
   */
  $config_db["default"]["user"] = "";

  /**
   * The password for the database user, used to connect to the databasehost
   * @var String
   */
  $config_db["default"]["password"] = "";

  /**
   * Wether or not to use a persistent connection.
   *
   * Note that this is usefull if you don't have a lot of applications doing
   * this as the application won't constantly have to connect to the database.
   * However, the database server won't be able to handle a lot of persistent
   * connections.
   * @var bool
   */
  $config_databasepersistent = true;

  /**
   * The databasetype.
   *
   * Currently supported are:
   *   mysql:   MySQL (3.X ?)
   *   mysql41: MySQL > 4.1.3
   *   oci8:    Oracle 8i
   *   oci9:    Oracle 9i and 10g
   *   pgsql:   PostgreSQL
   *   mssql:   Microsoft SQL Server
   * @var String
   */
  $config_db["default"]["driver"]="mysql";

  /**
   * Backwardscompatibility setting. Set this to MYSQL_BOTH if your
   * software relies on numerical indexes (WHICH IS A BAD IDEA!!)
   * @var int
   */
  $config_mysqlfetchmode = MYSQL_ASSOC;

  /********************************** SECURITY *******************************/

  /**
   * The password to use for administrator login.
   * An administrator password that is empty will *DISABLE* administrator login!
   * @var mixed
   */
  $config_administratorpassword = "";

  /**
   * The password to use for guest login.
   * A guest password that is empty will *DISABLE* guest login!
   * @var String
   */
  $config_guestpassword = "";

  /**
   *
   * @var String
   */
  $config_authentication = "none";

  /**
   *
   * @var bool
   */
  $config_authentication_md5 = true;

  /**
   *
   * @var bool
   */
  $config_authentication_cookie = false;

  /**
   * The default cookie expiry time (in minutes) (7 days)
   * @var int
   */
  $config_authentication_cookie_expire = 10080;

  /**
   *
   * @var bool
   */
  $config_authentication_session = true;

  /**
   *
   * @var String
   */
  $config_securityscheme = "none";

  /**
   *
   * @var bool
   */
  $config_restrictive = true;

  /**
   *
   * @var bool
   */
  $config_security_attributes = false;

  /**
   * By default, there is no 'grantall' privilege. Apps can set this if necessary.
   * Syntax: "module.nodename.privilege"
   */
  $config_auth_grantall_privilege = "";

  /**
   * Zero is no logging
   * @var int
   */
  $config_logging = 0;

  /**
   *
   * @var String
   */
  $config_logfile = "/tmp/atk-security.log";

  /************************** AUTHENTICATION *********************************/

  /**
   *
   * @var String
   */
  $config_auth_usertable   = "user";

  /**
   * Defaults to usertable
   * @var String
   */
  $config_auth_leveltable  = "";

  /**
   *
   * @var String
   */
  $config_auth_accesstable = "access";

  /**
   *
   * @var String
   */
  $config_auth_userfield   = "userid";

  /**
   * Primary key of usertable
   * @var String
   */
  $config_auth_userpk = "userid";

  /**
   *
   * @var String
   */
  $config_auth_passwordfield = "password";

  /**
   *
   * @var String
   */
  $config_auth_accountdisablefield = "";

  /**
   *
   * @var String
   */
  $config_auth_levelfield = "entity";

  /**
   * Name of table containing the groups.
   * (only necessary to support hierarchical groups!).
   * @var String
   */
  $config_auth_grouptable = "";

  /**
   * Name of primary key attribute in group table.
   * (only necessary to support hierarchical groups!)
   * @var String
   */
  $config_auth_groupfield = "";

  /**
   * Name of parent attribute in group table.
   * (only necessary to support hierarchical groups!)
   * @var String
   */
  $config_auth_groupparentfield = "";

  /**
   * Default pop3 port
   * @var String
   */
  $config_auth_mail_port = "110";

  /**
   * No vmail.
   * @var bool
   */
  $config_auth_mail_virtual = false;

  /**
   * Use bugzilla-style crypted password storage
   * @var bool
   */
  $config_auth_usecryptedpassword = false;

  /**
   * 0 = no maximum.
   * @var int
   */
  $config_max_loginattempts = 5;

  /**
   *
   * @var bool
   */
  $config_auth_dropdown = false;

  /**
   *
   * @var String
   */
  $config_auth_userdescriptor = "[".$config_auth_userfield."]";

  /**
   * This parameter can be used to specify a where clause which will be used
   * to validate users login credentials
   * @var String
   */
  $config_auth_accountenableexpression = "";


  /***************************** LDAP settings *******************************/
  /**
   * To use LDAP you should fill this config_variables with the right values
   */

  /**
   *
   * @var String
   */
  $config_authentication_ldap_host    = "";

  /**
   *
   * @var String
   */
  $config_authentication_ldap_context = "";

  /**
   *
   * @var String
   */
  $config_authentication_ldap_field   = "";

  /***************** DEBUGGING AND ERROR HANDLING ****************************/

  /**
   *
   * @var int
   */
  $config_debug = 0;

  /**
   *
   * @var String
   */
  $config_debuglog = "";

  /**
   *
   * @var Array
   */
  $config_smart_debug = array();

  /**
   *
   * @var bool
   */
  $config_display_errors = true;

  /**
   *
   * @var String
   */
  $config_halt_on_error = "critical";

  /**
   * Automatic error reporting is turned off by default.
   * @var String
   */
  $config_mailreport = "";

  /************************************ LAYOUT *******************************/

  /**
   * The default theme to use
   * @var String
   */
  $config_defaulttheme = "default";

  /**
   * Fullscreen mode (IE only)
   * @var String
   */
  $config_fullscreen = false;

  /**
   * Whatever tabs are enabled or not
   * @var bool
   */
  $config_tabs = true;

  /**
   * The number of records to display on a single page
   * @var int
   */
  $config_recordsperpage=25;

  /**
   * Display a 'stack' of the user activities in the top right corner.
   * @var bool
   */
  $config_stacktrace = true;

  /**
   * The maximum length of an HTML input field generated by atkAttribute or
   * descendants
   * @var int
   */
  $config_max_input_size = 70;

  /********************************** LANGUAGE *******************************/

  /**
   * The language to use for the application
   * @var String
   */
  $config_language="en";

  /**
   * The default language for the application
   * @var String
   */
  $config_defaultlanguage = "en";

  /**
   * The name of the directory atkLanguage can find it's languages in.
   * @var String
   */
  $config_language_basedir = "languages/";

  /**
   * True: one language switch attributes automatically switches all others on
   * screen.
   * False: each language switch attributes operates only on it's own node
   * @var bool
   */
  $config_multilanguage_linked = true;

  /**
   * Module/node checking for strings in atkLanguage (if you don't know, don't
   * change)
   * comment out to disable checking for module of node
   * 1 to check just for node
   * 2 to check for module and node
   * @var String
   */
  $config_atklangcheckmodule = 2;

  /********************* TEMPLATE ENGINE CONFIGURATION ***********************/

  /**
   * By default all templates are described by their relative
   * path, relative to the applications' root dir.
   * @var String
   */
  $config_tplroot = $config_atkroot;

  /**
   *
   * @var bool
   */
  $config_tplcaching = false;

  /**
   * default one hour
   * @var int
   */
  $config_tplcachelifetime = 3600;
  /**
   *
   * @var String
   */
  $config_tplcompiledir = $config_atktempdir."compiled/tpl/";

  /**
   *
   * @var String
   */
  $config_tplcachedir = $config_atktempdir."tplcache/";

  /**
   * Check templates to see if they changed
   * @var String
   */
  $config_tplcompilecheck = "true";

  /**
  * Use subdirectories for compiled and cached templates
  */
  $config_tplusesubdirs = false;

  /****************** MISCELLANEOUS CONFIGURATION OPTIONS ********************/

  /**
   * The application identifier (used for sessions)
   * @var String
   * @todo update this bit of documentation as it doesn't really say much
   */
  $config_identifier = "default";

  /**
   * Lock type, only supported type at this time is "db".
   * If empty locking is disabled.
   * @var String
   * @todo update this bit of documentation as it doesn't really say much
   */
  $config_lock_type = "";

  /**
   * The default encryption method for atkEncryption
   * @var String
   * @todo update this bit of documentation as it doesn't really say much
   */
  $config_encryption_defaultmethod = "base64";

  /**
   * The default searchmode
   * @var String
   * @todo update this bit of documentation as it doesn't really say much
   */
  $config_search_defaultmode = "substring";

  /**
   * Wether or not to enable Internet Explorer extensions
   * @var bool
   * @todo update this bit of documentation as it doesn't really say much
   */
  $config_enable_ie_extensions = false;

  /**
   * Files that are allowed to be included by the include wrapper script
   * NOTE: this has nothing to do with useattrib and userelation etc.!
   * @var Array
   */
  $config_allowed_includes = array("atk/lock/lock.php", "atk/lock/lock.js.php", "atk/javascript/class.atkdateattribute.js.inc",
                                               "atk/popups/help.inc", "atk/popups/colorpicker.inc");

  /**
   * Forces the themecompiler to recompile the theme all the time
   * This can be handy when working on themes.
   * @var bool
   */
  $config_force_theme_recompile = false;

  /**
   * Wether or not to use the keyboardhandler for attributes and the recordlist
   * Defaults to 1 (true), comment or set to null to remove keyboard handler
   * @var int
   */
  $config_use_keyboard_handler = 1;


  /**
   * Session cache expire (minutes)
   * @var int
   */
  $config_session_cache_expire = 180;

  /**
   * Session cache limiter
   *
   * Possible values:
   * - nocache
   * - public  (permits caching by proxies and clients
   * - private (permits caching by clients
   * - private_no_expire (permits caching by clients but not sending expire
   *   headers >PHP4.2.0)
   * @var String
   */
  $config_session_cache_limiter = "nocache";

  /**
   * Default sequence prefix.
   * @var String
   */
  $config_database_sequenceprefix = "seq_";

  /**
   * Make the recordlist use a javascript
   * confirm box for deleting instead of a seperate page
   * @var bool
   */
  $config_javascript_confirmation = false;

  /**
   * This should be turned on when an application makes use
   * of OpenSSL encryption (atk.security.encryption.atkopensslencryption)
   * It makes sure that the user password is available in the session
   * for the private key.
   * @var bool
   */
  $config_enable_ssl_encryption = false;

?>
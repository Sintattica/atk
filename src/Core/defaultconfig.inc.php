<?php

/* * ******************* FILE LOCATIONS & PATHS ***************************** */

/**
 * The application root, used to set the cookiepath when using PHP sessions.
 *
 * If you're using urlrewrites within your httpd or htaccess configuration this should be '/'
 * be careful with this setting because it could create a security vulnerability.
 *
 * @var String The application root
 */
$config_application_root = "/";

$config_application_config = "";

if ($config_atkroot == "" || (ini_get('register_globals'))) { // may not be passed in request (register_globals danger)
    /**
     * The root of the ATK application, where the atk/ directory resides
     * @var String The root
     */
    $config_atkroot = "./";
}

$config_assets_url = $config_theme_url = '/bundles/atk/';


if (!isset($config_application_dir) || empty($config_application_dir) || (ini_get('register_globals'))) {
    /**
     * Root directory of your application code (modules/themes/configuration files/etc)
     * relative to the script calling ATK.
     * Defaults to the atkroot.
     *
     * @var String Directory where the application code can be found
     */
    $config_application_dir = $config_atkroot;
}

/**
 * Module path (without trailing slash!) where the modules directory resides,
 * defaults to the atk root, so not IN the atk/ directory, but more likely in
 * the application root
 * @var String
 */
$config_module_path = $config_application_dir . "modules";

/**
 * The location of a directory that is writable to ATK and that ATK can
 * store it's temporary files in.
 * @var String
 */
$config_atktempdir = $config_application_dir . "atktmp/";

/**
 * The location of the module specific configuration files.
 * @var String
 */
$config_configdir = $config_application_dir . "configs/";

/**
 * Use the built-in ATK error handler (highly recommended!)
 * @var Bool
 */
$config_use_atkerrorhandler = true;

/**
 * Cache table meta data and compiled meta node code.
 *
 * On development environments this option should be set to false, but
 * on production environments you should really enable it. If you enable
 * this option and your table structure changes you should manually clear
 * the cache in the atktmp directory!
 *
 * @var bool
 */
$config_meta_caching = true;

/**
 * Use the given class for creating datagrids.
 */
$config_datagrid_class = "\\Sintattica\\Atk\\DataGrid\\DataGrid";


/**
 * The dispatcher, all request (should) lead to this setting.
 */
$config_dispatcher = 'index.php';

/* * ************************ DATABASE SETTINGS ***************************** */

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
 * @var boolean
 */
$config_databasepersistent = true;

/**
 * The databasetype.
 *
 * Currently supported are:
 *   MySql:   MySQL (3.X ?)
 *   MySqli:  MySQL > 4.1.3
 *   PgSql:   PostgreSQL
 *   MsSql:   Microsoft SQL Server
 * @var String
 */
$config_db["default"]["driver"] = "MySqli";

/**
 * Test database mapping. Maps normal databases to their test database.
 * Most of the applications only use one database in that case the default
 * should be sufficient. But in case you use multiple database and also
 * want to run tests on all these database you can override this mapping
 * or add your own mappings.
 *
 * @var array
 */
$config_test_db_mapping = array('default' => 'test');

/**
 * Backwardscompatibility setting. Set this to MYSQL_BOTH if your
 * software relies on numerical indexes (WHICH IS A BAD IDEA!!)
 * @var int
 */
$config_mysqlfetchmode = defined("MYSQL_ASSOC") ? MYSQL_ASSOC : 0;

/**
 * Backwardscompatibility setting. Set this to PGSQL_BOTH if your
 * software relies on numerical indexes (WHICH IS A BAD IDEA!!)
 * @var int
 */
$config_pgsqlfetchmode = defined("PGSQL_ASSOC") ? PGSQL_ASSOC : 0;

/**
 * Database Cluster nodes
 * $config_db_cluster["default"] = array("master","slave","slave2");
 */
$config_db_cluster = array();

/* * ******************************** SECURITY ****************************** */

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
 * The method to use for user/password validation.
 *
 * Currently supported are:
 * - "none": No authentication
 * - "db"  : the credentials are stored in the database.
 * - "pop3": the passwords are validated against a pop3 server.
 * - "config": the credentials are stored in the configurationfile.
 * - "imap": the passwords are validated against an IMAP server.
 * - "ldap": the passwords are validated against an LDAP server.
 * - "server": Authentication is done through the webserver.
 *
 * @var String
 */
$config_authentication = "none";

/**
 * Wether your authentication method supports MD5 passwords
 * @var boolean
 */
$config_authentication_md5 = true;

/**
 * Use a cookie to store authentication information.
 * @var boolean
 */
$config_authentication_cookie = false;

/**
 * The default cookie expiry time (in minutes) (7 days)
 * @var int
 */
$config_authentication_cookie_expire = 10080;

/**
 * The default state cookie expiry time (in minutes) (7 days)
 * @var int
 */
$config_state_cookie_expire = 10080;

/**
 * Use the session to store authentication information.
 * @var boolean
 */
$config_authentication_session = true;

/**
 * The scheme to use for security.
 *
 * Currently supported are:
 * - "none": No security scheme is used.
 * - "group": Use group-based security.
 * - "level": Use level-based security.
 *
 * @var String
 */
$config_securityscheme = "none";

/**
 *
 * @var boolean
 */
$config_restrictive = true;

/**
 *
 * @var boolean
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

/**
 * Password Restrictions if required
 *
 * 0 => ignore restriction
 * >0 => implement restriction
 * Special characters are    !@#$%^&*()-+_=[]{}\|;:'\",.<>/?
 */
$config_password_minsize = 0;
$config_password_minupperchars = 0;
$config_password_minlowerchars = 0;
$config_password_minalphabeticchars = 0;
$config_password_minnumbers = 0;
$config_password_minspecialchars = 0;

/* * ************************ AUTHENTICATION ******************************** */

/**
 *
 * @var String
 */
$config_auth_database = "default";

/**
 *
 * @var String
 */
$config_auth_usertable = "user";

/**
 * Defaults to usertable
 * @var String
 */
$config_auth_leveltable = "";

/**
 *
 * @var String
 */
$config_auth_accesstable = "access";

/**
 * If left empty auth_levelfield is used.
 *
 * @var String
 */
$config_auth_accessfield = "";

/**
 *
 * @var String
 */
$config_auth_userfield = "userid";

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
$config_auth_languagefield = "lng";

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
 * @var boolean
 */
$config_auth_mail_virtual = false;

/**
 * Use bugzilla-style crypted password storage
 * @var boolean
 */
$config_auth_usecryptedpassword = false;

/**
 * When changerealm is true, the authentication realm is changed on every
 * login.
 *
 * Advantage: the user is able to logout using the logout link.
 * Disadvantage: browser's 'remember password' feature won't work.
 *
 * This setting only affects the http login box, so it is only relevant if
 * $config_auth_loginform is set to false.
 *
 * The default is true for backwardscompatibility reasons. For new
 * applications, it defaults to false since the skel setting is set to false
 * by default.
 * @var boolean
 */
$config_auth_changerealm = true;

/**
 * 0 = no maximum.
 * @var int
 */
$config_max_loginattempts = 5;


/**
 *
 * @var boolean
 */
$config_auth_dropdown = false;

/**
 *
 * @var String
 */
$config_auth_userdescriptor = "[" . $config_auth_userfield . "]";

/**
 * This parameter can be used to specify a where clause which will be used
 * to validate users login credentials
 * @var String
 */
$config_auth_accountenableexpression = "";


/* * *************************** LDAP settings ****************************** */
/**
 * To use LDAP you should fill this config_variables with the right values
 */
/**
 *
 * @var String
 */
$config_authentication_ldap_host = "";

/**
 *
 * @var String
 */
$config_authentication_ldap_context = "";

/**
 *
 * @var String
 */
$config_authentication_ldap_field = "";

/* * *************** DEBUGGING AND ERROR HANDLING *************************** */

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
 * @var array
 */
$config_smart_debug = array();

/**
 *
 * @var boolean
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

/**
 * Output missing translation "errors".
 * @var String
 */
$config_debug_translations = false;

/* * ********************************** LAYOUT ****************************** */

/**
 *
 * @var String
 */
$config_doctype = '<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN"
        "http://www.w3.org/TR/html4/loose.dtd">';
//$config_doctype = '<!DOCTYPE html>';


/**
 *
 * @var String
 */
$config_defaulttheme = "stillblue";


/**
 * Whatever tabs are enabled or not
 * @var boolean
 */
$config_tabs = true;

/**
 * Whatever DHTML tabs should be stateful or not
 * (E.g. the current tab is saved for the current node/selector combination)
 * @var boolean
 */
$config_dhtml_tabs_stateful = true;

/**
 * The default number of records to display on a single page
 * @var int
 */
$config_recordsperpage = 25;

/**
 * The number of records per page options to display on drop down list
 * @var array
 */
$config_recordsperpage_options = array(10, 25, 50, 100);

/**
 * Add a 'show all' option to the records per page selector.
 * @var boolean
 */
$config_enable_showall = true;

/**
 * The (max) number of page navigation links to show
 * @var int
 */
$config_pagelinks = 10;

/**
 * Show go to previous page and go to next page links in recordlist
 * @var bool
 */
$config_pagelinks_previous_next = true;

/**
 * Show go to first page and go to last page links in recordlist
 * @var bool
 */
$config_pagelinks_first_last = false;

/**
 * Display a 'stack' of the user activities in the top right corner.
 * @var boolean
 */
$config_stacktrace = true;

/**
 * The maximum length of an HTML input field generated by Attribute or descendants
 * @var int
 */
$config_max_input_size = 70;

/**
 * The maximum length of an HTML input search field generated by Attribute or descendants
 * @var int
 */
$config_max_searchinput_size = 20;

/**
 * Set to true, clicking on a record redirects to its view or edit page
 * @var boolean
 */
$config_recordlist_onclick = false;

/**
 * The position of MRA (multi record actions): "top" or "bottom"
 * @var string
 */
$config_mra_position = 'bottom';


/* * ********************************* OUTPUT ******************************* */

/**
 * Set to true, to output pages gzip compressed to the browser if the
 * browser supports it.
 *
 * Note: This should only be used for situations where either the webserver (Apache)
 * doesn't support it or you can't get to the webserver configuration,
 * as webservers are generally much better at this than ATK is.
 */
$config_output_gzip = false;

/* * ******************************** LANGUAGE ****************************** */

/**
 *
 * @var String
 */
$config_language = "en";

/**
 *
 * @var String
 */
$config_defaultlanguage = "en";

/**
 *
 * @var String
 */
$config_language_basedir = "languages/";

/**
 * Use browser language to detect application language.
 * By default set to false to remain backwards compatible.
 *
 * @var boolean
 */
$config_use_browser_language = false;


/**
 * Module/node checking for strings in atkLanguage (if you don't know, don't
 * change)
 * comment out to disable checking for module of node
 * 1 to check just for node
 * 2 to check for module and node
 * @var String
 */
$config_atklangcheckmodule = 2;

/**
 * Where ATK should look for it's supported languages
 *
 * In your own application you should probably make this the module
 * with the most language translations.
 * Leaving this empty will turn off functionality where we check
 * for the user language in the browser or in the user session and will
 * make sure the application is always presented in the default language.
 * This config var also accepts 2 'special' modules:
 * - atk (making it use the languages of ATK)
 * - langoverrides (making it use the language overrides directory)
 *
 * @var String
 */
//$config_supported_languages_module = $config_atkroot.'atk/languages/';
$config_supported_languages_module = '';

/* * ******************* TEMPLATE ENGINE CONFIGURATION ********************** */

/**
 * By default all templates are described by their relative
 * path, relative to the applications' root dir.
 * @var String
 */
$config_tplroot = $config_application_dir;

/**
 *
 * @var boolean
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
$config_tplcompiledir = $config_atktempdir . "compiled/tpl/";

/**
 *
 * @var String
 */
$config_tplcachedir = $config_atktempdir . "tplcache/";

/**
 * Check templates to see if they changed
 * @var String
 */
$config_tplcompilecheck = "true";

/**
 * Use subdirectories for compiled and cached templates
 */
$config_tplusesubdirs = false;

/* * **************** MISCELLANEOUS CONFIGURATION OPTIONS ******************* */

/**
 * The session name. If this configuration option is not set the
 * $config_identifier option is used instead.
 *
 * @var string
 */
$config_session_name = "";

/**
 * The maximum inactivity period for a stack in the session manager before
 * it expires.
 *
 * Set to a value <= 0 to disable.
 *
 * @var int
 */
$config_session_max_stack_inactivity_period = 3600; // 1 hour

/**
 * The application identifier.
 *
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
 * @var boolean
 * @todo update this bit of documentation as it doesn't really say much
 */
$config_enable_ie_extensions = false;

/**
 * Files that are allowed to be included by the include wrapper script
 * NOTE: this has nothing to do with useattrib and userelation etc.!
 * @var array
 */
$config_allowed_includes = array(
    "atk/lock/lock.php",
    "atk/lock/lock.js.php",
    "atk/popups/help.php",
    "atk/popups/colorpicker.php"
);

/**
 * Forces the themecompiler to recompile the theme all the time
 * This can be handy when working on themes.
 * @var boolean
 */
$config_force_theme_recompile = false;

/**
 * Wether or not to use the keyboardhandler for attributes and the recordlist
 * When set to true, arrow keys can be used to navigate through fields and
 * records, as well as shortcuts 'e' for edit, 'd' for delete, and left/right
 * cursor for paging. Note however, that using cursor keys to navigate
 * through fields is not standard web application behaviour.
 * @var int
 */
$config_use_keyboard_handler = false;

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
 * Initialize sessions by default.
 *
 * When atksessionmanager is included, if this configuration value is true (by default),
 * ATK will configure and start a PHP session for you.
 * When you do not want this (in CLI environnements?) you can disable this in your script.
 *
 * DO NOT ENABLE IN THIS CONFIG or you won't be able to set it in your script.
 * Appears here for documentation purposes only.
 *
 * @var bool
 */
//$config_session_init = true;

/**
 * Default sequence prefix.
 * @var String
 */
$config_database_sequenceprefix = "seq_";

/**
 * Make the recordlist use a javascript
 * confirm box for deleting instead of a seperate page
 * @var boolean
 */
$config_recordlist_javascript_delete = false;

/**
 * This should be turned on when an application makes use
 * of OpenSSL encryption (atk.security.encryption.atkopensslencryption)
 * It makes sure that the user password is available in the session
 * for the private key.
 * @var boolean
 */
$config_enable_ssl_encryption = false;


/**
 * Default extended search action. This action can always be overriden
 * in the node by using $node->setExtendedSearchAction. At this time
 * (by default) the following values are supported: 'search' or 'smartsearch'
 *
 * @var string
 */
$config_extended_search_action = 'search';

/**
 * Lists that are obligatory, by default have no 'Select none' option.
 * This leads to the user just selecting the first item since that is the default.
 * If this is a problem set this config variable to true; this will add a 'Select none'
 * option to obligatory lists so the user is forced to make a selection.
 * Can be disabled per individual atkListAttribute with AF_LIST_NO_OBLIGATORY_NULL_ITEM.
 *
 * @var boolean
 */
$config_list_obligatory_null_item = false;

/**
 * Should all many-to-one relations have the Attribute::AF_RELATION_AUTOCOMPLETE flag set?
 *
 * @var boolean
 */
$config_manytoone_autocomplete_default = false;

/**
 * Should all many-to-one relations that have the Attribute::AF_LARGE flag set also
 * have the Attribute::AF_RELATION_AUTOCOMPLETE flag set?
 *
 * @var boolean
 */
$config_manytoone_autocomplete_large = true;

/**
 * Should manytoone relations having the Attribute::AF_RELATION_AUTOCOMPLETE flag also
 * use auto completion in search forms?
 *
 * @var boolean
 */
$config_manytoone_search_autocomplete = true;

/**
 * Controls how many characters a user must enter before an auto-completion
 * search is being performed.
 *
 * @var int
 */
$config_manytoone_autocomplete_minchars = 2;

/**
 * The length of the HTML input field generated in auto-completion mode.
 *
 * @var int
 */
$config_manytoone_autocomplete_size = 50;

/**
 * The search mode of the autocomplete fields. Can be 'startswith', 'exact' or 'contains'.
 *
 * @access private
 * @var String
 */
$config_manytoone_autocomplete_searchmode = "contains";

/**
 * Value determines wether the search of the autocompletion is case-sensitive.
 *
 * @var boolean
 */
$config_manytoone_autocomplete_search_case_sensitive = false;

/**
 * Warn the user if he/she has changed something in a form
 * and leaves the page without pressing save or cancel.
 *
 * @var bool
 */
$config_lose_changes_warning = false;

/**
 * Optionally set the export file parameters
 */
$config_export_delimiter = ";";
$config_export_enclosure = "&quot;";
$config_export_titlerow_checked = true;

/**
 * Directories that contains modules (needed for testcases)
 */
//  $config_module_dirs = array("/modules");

// --------- FCK Image Upload Option ---------

$config_fck_filemanager_enabled = false;
$config_fck_upload_path = '../atktmp/';

/**
 * Zend framework path (relative to ATK root).
 *
 * Example: "../library/Zend/";
 */
$config_zend_framework_path = null;


/**
 * Use the ATK autoloader for ATK classes.
 *
 * @var bool
 */
$config_autoload_classes = true;

/**
 * Re-index the cached class list when a class is missing?
 *
 * When this option is turned off you should manually delete the
 * cached classes file (atktmp/classes.inc) when updating ATK to
 * a newer version. It's recommended to not change the value of this
 * option unless you are an ATK developer.
 *
 * @var bool
 */
$config_autoload_reindex_on_missing_class = false;

/**
 * Normally atkerror silently ignores an error and sends an e-mail and/or
 * adds the error to the debug output. Using this switch ATK will throw
 * an exception when atkerror is called.
 *
 * @var boolean
 */
$config_throw_exception_on_error = false;

/**
 * Inverts check logic of attributes rights: default all allowed, the
 * "attribaccess" table will store attributes modes not allowed
 */
$config_reverse_attributeaccess_logic = false;

/**
 * atkCKAttribute configuration options override
 */
$config_ck_options = array();


/**
 * recordlist orientation (left or right)
 */
$config_recordlist_orientation = "left";

/**
 * logo
 */
$config_login_logo = $config_assets_url . 'images/login_logo.jpg';

/**
 * icons
 */
$config_icon_canceled = 'fa fa-times';
$config_icon_copy = 'fa fa-files-o';
$config_icon_default = 'fa fa-file-o';
$config_icon_delete = 'fa fa-trash';
$config_icon_document = 'fa fa-file-o';
$config_icon_done = 'fa fa-check';
$config_icon_edit = 'fa fa-pencil';
$config_icon_editcopy = 'fa fa-files-o';
$config_icon_export = 'fa fa-file-excel-o';
$config_icon_preview = 'fa fa-file-o';
$config_icon_select = 'fa fa-hand-pointer-o';
$config_icon_view = 'fa fa-search';
$config_icon_email = $config_icon_e_mail = 'fa fa-envelope-o';
$config_icon_print = 'fa fa-print';
$config_icon_plussquare = 'fa fa-plus-square-o';
$config_icon_minussquare = 'fa fa-minus-square-o';
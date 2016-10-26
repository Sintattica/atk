<?php

use Sintattica\Atk\Core\Config;

$_configAppRoot = __DIR__.'/../../../../../../';
$_configTempDir = $_configAppRoot.'var/atktmp/';
$_configAssetsUrl = '/bundles/atk/';
$_configDirName = 'config';

$_public_dir = $_configAppRoot.'web';
if($_public_dir_real = realpath($_public_dir)){
    $_public_dir = $_public_dir_real.'/';
}else{
    $_public_dir = $_public_dir.'/';
}

return [

    /********************* FILE LOCATIONS & PATHS ******************************/

    'configdirname' => $_configDirName,

    'application_dir' => $_configAppRoot,

    'application_config_dir' => $_configAppRoot.$_configDirName.'/',

    'application_config' => $_configAppRoot.$_configDirName.'/atk.php',

    'assets_url' => $_configAssetsUrl,

    'template_dir' => __DIR__.'/../templates/',

    'public_dir' => $_public_dir,

    /*
     * The location of a directory that is writable to ATK and that ATK can
     * store it's temporary files in.
     * @var String
     */
    'atktempdir' => $_configTempDir,

    /*
     * Use the built-in ATK error handler (highly recommended!)
     * @var Bool
     */
    'use_atkerrorhandler' => true,

    /*
     * Cache table meta data and compiled meta node code.
     *
     * On development environments this option should be set to false, but
     * on production environments you should really enable it. If you enable
     * this option and your table structure changes you should manually clear
     * the cache in the atktmp directory!
     *
     *
     * @var int
     */
    'meta_caching' => Config::env('META_CACHING', 1),

    /*
     * Use the given class for creating datagrids.
     */
    'datagrid_class' => '\\Sintattica\\Atk\\DataGrid\\DataGrid',

    'datagrid_display_top_paginator' => true,
    'datagrid_display_bottom_paginator' => true,

    /*
     * The dispatcher, all request (should) lead to this setting.
     */
    'dispatcher' => 'index.php',

    /************************** DATABASE SETTINGS ******************************/

    'db' => [
        'default' => [
            'host' => Config::env('DB_HOST', 'localhost'),
            'db' => Config::env('DB_NAME', 'atk'),
            'user' => Config::env('DB_USER', 'root'),
            'password' => Config::env('DB_PASSWORD', ''),
            'charset' => Config::env('DB_CHARSET', 'utf8'),
            'driver' => Config::env('DB_DRIVER', 'MySqli'),
        ],
    ],

    /*
     * Wether or not to use a persistent connection.
     *
     * Note that this is usefull if you don't have a lot of applications doing
     * this as the application won't constantly have to connect to the database.
     * However, the database server won't be able to handle a lot of persistent
     * connections .
     * @var boolean
     */
    'databasepersistent' => true,

    /********************************** SECURITY *******************************/

    /*
     * The password to use for administrator login.
     * An administrator password that is empty will *DISABLE* administrator login!
     * @var mixed
     */
    'administratorpassword' => Config::env('ADMIN_PASSWORD', ''),

    /*
     * The password to use for guest login.
     * A guest password that is empty will *DISABLE* guest login!
     * @var String
     */
    'guestpassword' => Config::env('GUEST_PASSWORD', ''),

    /*
     * The method to use for user/password validation.
     *
     * Currently supported are:
     * - "none": No authentication
     * - "db"  : the credentials are stored in the database.
     * - "config": the credentials are stored in the configurationfile.
     * - "imap": the passwords are validated against an IMAP server.
     * - "ldap": the passwords are validated against an LDAP server.
     * - "server": Authentication is done through the webserver.
     *
     * @var String
     */
    'authentication' => 'none',

    /*
     * The default state cookie expiry time (in minutes) (7 days)
     * @var int
     */
    'state_cookie_expire' => 10080,

    /*
     * Use the session to store authentication information.
     * @var boolean
     */
    'authentication_session' => true,

    /*
     * The scheme to use for security.
     *
     * Currently supported are:
     * - "none": No security scheme is used.
     * - "group": Use group-based security.
     * - "level": Use level-based security.
     *
     * @var String
     */
    'securityscheme' => 'none',

    /*
     *
     * @var boolean
     */
    'restrictive' => true,

    /*
     *
     * @var boolean
     */
    'security_attributes' => false,

    /*
     * By default, there is no 'grantall' privilege. Apps can set this if necessary.
     * Syntax: "module.nodename.privilege"
     */
    'auth_grantall_privilege' => '',

    /*
     * Zero is no logging
     * @var int
     */
    'logging' => 0,

    /*
     *
     * @var String
     */
    'logfile' => $_configAppRoot.'atk-security.log',

    /*
     * Password Restrictions if required
     *
     * 0 => ignore restriction
     * >0 => implement restriction
     * Special characters are    !@#$%^&*()-+_=[]{}\|,:'\",.<>/?
     */
    'password_minsize' => 0,
    'password_minupperchars' => 0,
    'password_minlowerchars' => 0,
    'password_minalphabeticchars' => 0,
    'password_minnumbers' => 0,
    'password_minspecialchars' => 0,


    /************************** AUTHENTICATION *********************************/

    /*
     *
     * @var String
     */
    'auth_database' => 'default',

    /*
     *
     * @var String
     */
    'auth_usertable' => 'user',

    /*
     * Defaults to usertable
     * @var String
     */
    'auth_leveltable' => '',

    /*
     *
     * @var String
     */
    'auth_accesstable' => 'access',

    /*
     * If left empty auth_levelfield is used.
     *
     * @var String
     */
    'auth_accessfield' => '',

    /*
     *
     * @var String
     */
    'auth_userfield' => 'userid',

    /*
     * Primary key of usertable
     * @var String
     */
    'auth_userpk' => 'userid',

    /*
     *
     * @var String
     */
    'auth_passwordfield' => 'password',

    /*
     *
     * @var String
     */
    'auth_languagefield' => 'lng',

    /*
     *
     * @var String
     */
    'auth_accountdisablefield' => '',

    /*
     *
     * @var String
     */
    'auth_levelfield' => 'entity',

    /*
     * Name of table containing the groups.
     * (only necessary to support hierarchical groups!).
     * @var String
     */
    'auth_grouptable' => '',

    /*
     * Name of primary key attribute in group table.
     * (only necessary to support hierarchical groups!)
     * @var String
     */
    'auth_groupfield' => '',

    /*
     * Name of parent attribute in group table.
     * (only necessary to support hierarchical groups!)
     * @var String
     */
    'auth_groupparentfield' => '',

    /*
     * No vmail.
     * @var boolean
     */
    'auth_mail_virtual' => false,

    /*
     * Use bugzilla-style crypted password storage
     * @var boolean
     */
    'auth_usecryptedpassword' => false,

    /*
     * Setting this to true will make ATK use a loginform instead of a browserpopup.
     */
    'auth_loginform' => true,

    /*
     * When changerealm is true, the authentication realm is changed on every
     * login.
     *
     * Advantage: the user is able to logout using the logout link.
     * Disadvantage: browser's 'remember password' feature won't work.
     *
     * This setting only affects the http login box, so it is only relevant if
     * 'auth_loginform is set to false.
     *
     * The default is true for backwardscompatibility reasons. For new
     * applications, it defaults to false since the skel setting is set to false
     * by default.
     * @var boolean
     */
    'auth_changerealm' => false,

    /*
     *
     * @var String
     */
    'auth_userdescriptor' => '[userid]',

    /*
     * This parameter can be used to specify a where clause which will be used
     * to validate users login credentials
     * @var String
     */
    'auth_accountenableexpression' => '',


    /************************** REMEMBER ME *********************************/

    /*
     * Enable or disable Remember me
     * For security reasons, remember me is not available for administrator and guest users
     */
    'auth_enable_rememberme' => false,

    /*
     * Set Remember me expire interval in DateTime format
     */
    'auth_rememberme_expireinterval' => '+14 days',

    /*
     * The Remember me cookie name
     */
    'auth_rememberme_cookiename' => 'rememberme',

    /*
     * The table where to store remember me tokens
     */
    'auth_rememberme_dbtable' => 'auth_tokens',

    /***************************** LDAP settings *******************************/
    /*
     * To use LDAP you should fill this config_variables with the right values
     */
    /*
     *
     * @var String
     */
    'authentication_ldap_host' => '',

    /*
     *
     * @var String
     */
    'authentication_ldap_context' => '',

    /*
     *
     * @var String
     */
    'authentication_ldap_field' => '',

    /***************** DEBUGGING AND ERROR HANDLING ****************************/

    /*
     *
     * @var int
     */
    'debug' => Config::env('DEBUG_LEVEL', 0),

    /*
     *
     * @var String
     */
    'debuglog' => '',

    /*
     *
     * @var boolean
     */
    'display_errors' => true,

    /*
     *
     * @var String
     */
    'halt_on_error' => 'critical',

    /*
     * Automatic error reporting is turned off by default.
     * @var String
     */
    'mailreport' => '',

    /*
     * Output missing translation "errors".
     * @var String
     */
    'debug_translations' => false,

    /************************** INDEX *********************************/

    'indexPage' => Sintattica\Atk\Ui\IndexPage::class,

    /************************** MENU *********************************/

    'menu' => Sintattica\Atk\Core\Menu::class,

    /*
     * Show a link in the menu to logout
     */
    'menu_show_logout_link' => true,

    /*
     * Show the logged-in user on the right side of the menu
     */
    'menu_show_user' => true,

    /************************************ LAYOUT ****************************** */

    /*
     * Whatever tabs are enabled or not
     * @var boolean
     */
    'tabs' => true,

    /*
     * Whatever DHTML tabs should be stateful or not
     * (E.g. the current tab is saved for the current node/selector combination)
     * @var boolean
     */
    'dhtml_tabs_stateful' => true,

    /*
     * The default number of records to display on a single page
     * @var int
     */
    'recordsperpage' => 25,

    /*
     * The number of records per page options to display on drop down list
     * @var array
     */
    'recordsperpage_options' => [10, 25, 50, 100],

    /*
     * Add a 'show all' option to the records per page selector.
     * @var boolean
     */
    'enable_showall' => true,

    /*
     * The (max) number of page navigation links to show
     * @var int
     */
    'pagelinks' => 10,

    /*
     * Show go to previous page and go to next page links in recordlist
     * @var bool
     */
    'pagelinks_previous_next' => true,

    /*
     * Show go to first page and go to last page links in recordlist
     * @var bool
     */
    'pagelinks_first_last' => false,

    /*
     * Display a 'stack' of the user activities in the top right corner.
     * @var boolean
     */
    'stacktrace' => true,

    /*
     * The maximum length of an HTML input field generated by Attribute or descendants
     * @var int
     */
    'max_input_size' => 70,

    /*
     * The maximum length of an HTML input search field generated by Attribute or descendants
     * @var int
     */
    'max_searchinput_size' => 20,

    /*
     * Set to true, clicking on a record redirects to its view or edit page
     * @var boolean
     */
    'recordlist_onclick' => false,

    /*
     * The position of MRA (multi record actions): "top" or "bottom"
     * @var string
     */
    'mra_position' => 'bottom',

    /*********************************** OUTPUT ********************************/

    /*
     * Set to true, to output pages gzip compressed to the browser if the
     * browser supports it.
     *
     * Note: This should only be used for situations where either the webserver (Apache)
     * doesn't support it or you can't get to the webserver configuration,
     * as webservers are generally much better at this than ATK is.
     */
    'output_gzip' => false,

    /********************************** LANGUAGE *******************************/

    /*
     *
     * @var String
     */
    'language' => 'en',

    'supported_languages' => ['en'],

    /*
     *
     * @var String
     */
    'language_basedir' => 'languages/',

    /*
     * Use browser language to detect application language.
     * By default set to false to remain backwards compatible.
     *
     * @var boolean
     */
    'use_browser_language' => false,

    /********************* TEMPLATE ENGINE CONFIGURATION ***********************/

    'tplcompiledir' => $_configTempDir.'tpl/',
    'tplcompilecheck' => Config::env('TPL_COMPILE_CHECK', 1),
    'tplforcecompile' => Config::env('TPL_FORCE_COMPILE', 0),

    /****************** MISCELLANEOUS CONFIGURATION OPTIONS ********************/

    /*
     * @var array List of enabled modules
     * eg: [App\Modules\App\Module::class, App\Modules\Auth\Module::class,]
     *
     */
    'modules' => [],

    /*
     * The cookie application root, used to set the cookiepath when using PHP sessions.
     *
     * If you're using urlrewrites within your httpd or htaccess configuration this should be '/'
     * be careful with this setting because it could create a security vulnerability.
     *
     * @var String The application root
     */
    'cookie_path' => '/',

    /*
     * The session name. If this configuration option is not set the
     * 'identifier option is used instead.
     *
     * @var string
     */
    'session_name' => '',

    /*
     * The maximum inactivity period for a stack in the session manager before
     * it expires.
     *
     * Set to a value <= 0 to disable.
     *
     * @var int
     */
    'session_max_stack_inactivity_period' => 3600, // 1 hour

    /*
     * Enable the session autorefresh ajax call
     * @var bool
     */
    'session_autorefresh' => false,

    /*
     * Refresh every n milliseconds
     * @var int
     */
    'session_autorefresh_time' => 300000, // 5 minutes

    /*
     * Key used to detect the autorefresh calls from ajax
     * @var string
     */
    'session_autorefresh_key' => '_sessionautorefresh',
    
    /*
     * The application identifier.
     *
     * @var String
     */
    'identifier' => 'default',

    /*
     * The default encryption method for Encryption
     * @var String
     */
    'encryption_defaultmethod' => 'base64',

    /*
     * The default searchmode
     * @var String
     */
    'search_defaultmode' => 'substring',

    /*
     * Session cache expire (minutes)
     * @var int
     */
    'session_cache_expire' => 180,

    /*
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
    'session_cache_limiter' => 'nocache',

    /*
     * Default sequence prefix.
     * @var String
     */
    'database_sequenceprefix' => '',

    /*
     * Default sequence suffix.
     * @var String
     */
    'database_sequencesuffix' => '_seq',

    /*
     * Make the recordlist use a javascript
     * confirm box for deleting instead of a seperate page
     * @var boolean
     */
    'recordlist_javascript_delete' => false,

    /*
     * This should be turned on when an application makes use
     * of OpenSSL encryption (atk.security.encryption.atkopensslencryption)
     * It makes sure that the user password is available in the session
     * for the private key.
     * @var boolean
     */
    'enable_ssl_encryption' => false,

    /*
     * Default extended search action. This action can always be overriden
     * in the node by using $node->setExtendedSearchAction. At this time
     * (by default) the following values are supported: 'search' or 'smartsearch'
     *
     * @var string
     */
    'extended_search_action' => 'search',

    /*
     * Lists that are obligatory, by default have no 'Select none' option.
     * This leads to the user just selecting the first item since that is the default.
     * If this is a problem set this config variable to true, this will add a 'Select none'
     * option to obligatory lists so the user is forced to make a selection.
     * Can be disabled per individual atkListAttribute with AF_LIST_NO_OBLIGATORY_NULL_ITEM.
     *
     * @var boolean
     */
    'list_obligatory_null_item' => false,

    /*
     * Should all many-to-one relations have the Attribute::AF_RELATION_AUTOCOMPLETE flag set?
     *
     * @var boolean
     */
    'manytoone_autocomplete_default' => false,

    /*
     * Should all many-to-one relations that have the Attribute::AF_LARGE flag set also
     * have the Attribute::AF_RELATION_AUTOCOMPLETE flag set?
     *
     * @var boolean
     */
    'manytoone_autocomplete_large' => true,

    /*
     * Should manytoone relations having the Attribute::AF_RELATION_AUTOCOMPLETE flag also
     * use auto completion in search forms?
     *
     * @var boolean
     */
    'manytoone_search_autocomplete' => true,

    /*
     * Controls how many characters a user must enter before an auto-completion
     * search is being performed.
     *
     * @var int
     */
    'manytoone_autocomplete_minchars' => 2,

    /*
     * The length of the HTML input field generated in auto-completion mode.
     *
     * @var int
     */
    'manytoone_autocomplete_size' => 50,

    /*
     * The search mode of the autocomplete fields. Can be 'startswith', 'exact' or 'contains'.
     *
     * @access private
     * @var String
     */
    'manytoone_autocomplete_searchmode' => 'contains',

    /*
     * Value determines wether the search of the autocompletion is case-sensitive.
     *
     * @var boolean
     */
    'manytoone_autocomplete_search_case_sensitive' => false,

    /*
     * Value determines the minimal number of records for showing the automcomplete. If there are less records the normal dropdown is shown
     *
     * @var int
     */
    'manytoone_autocomplete_minrecords' => -1,

    'manytoone_autocomplete_pagination_limit' => 50,

    /*
     * OneToMany add link position (top or bottom)
     */
    'onetomany_addlink_position' => 'top',

    /*
     * Warn the user if he/she has changed something in a form
     * and leaves the page without pressing save or cancel.
     *
     * @var bool
     */
    'lose_changes_warning' => false,

    /*
     * Optionally set the export file parameters
     */
    'export_delimiter' => ',',
    'export_enclosure' => '&quot;',
    'export_titlerow_checked' => true,

    /*
     * Normally atkerror silently ignores an error and sends an e-mail and/or
     * adds the error to the debug output. Using this switch ATK will throw
     * an exception when atkerror is called.
     *
     * @var boolean
     */
    'throw_exception_on_error' => false,

    /*
     * Inverts check logic of attributes rights: default all allowed, the
     * "attribaccess" table will store attributes modes not allowed
     */
    'reverse_attributeaccess_logic' => false,

    /*
     * atkCKAttribute configuration options override
     */
    'ck_options' => [],

    /*
     * recordlist orientation (left or right)
     */
    'recordlist_orientation' => 'left',

    /*
     * logo
     */
    'login_logo' => $_configAssetsUrl.'images/login_logo.jpg',
    'brand_logo' =>  '',

    /*
     * icons
     */
    'recordlist_icons' => true,

    'icon_canceled' => 'fa fa-times',
    'icon_copy' => 'fa fa-files-o',
    'icon_default' => 'fa fa-file-o',
    'icon_delete' => 'fa fa-trash-o text-danger',
    'icon_document' => 'fa fa-file-o',
    'icon_done' => 'fa fa-check',
    'icon_edit' => 'fa fa-pencil',
    'icon_editcopy' => 'fa fa-files-o',
    'icon_export' => 'fa fa-file-excel-o',
    'icon_preview' => 'fa fa-file-o',
    'icon_select' => 'fa fa-hand-pointer-o',
    'icon_view' => 'fa fa-search',
    'icon_email' => 'fa fa-envelope-o',
    'icon_e_mail' => 'fa fa-envelope-o',
    'icon_print' => 'fa fa-print',
    'icon_plussquare' => 'fa fa-plus-square-o',
    'icon_minussquare' => 'fa fa-minus-square-o',

    /****************** CACHING ********************/

    // Cache method
    'cache_method' => 'var',
    // Cache namespace, change this when you are hosting your application on a shared hosting.
    'cache_namespace' => 'default',
];

<?php
  // the application identifier (used for sessions)
  $config_identifier   = "default";

  // The application root
  $config_application_root = "/";
  if ($config_atkroot == "") $config_atkroot = "./";

  // set several default configuration options
  $config_databasehost = "localhost";
  $config_databasename = "";
  $config_databaseuser = "";
  $config_databasepassword = "";
  $config_databasepersistent = true;

  // mysql, oci8 or pgsql
  $config_database="mysql";

  $config_language="en";
  $config_recordsperpage=25;

  // lock type
  $config_lock_type = "dummy";

  // security
  $config_authentication = "none";
  $config_authentication_md5 = true;
  $config_authentication_cookie = false;
  $config_authentication_cookie_expire = 10080; // the default cookie expiry time (in minutes) (7 days)
  $config_authentication_session = true;
  $config_securityscheme = "none";
  $config_restrictive = true;
  $config_security_attributes = false;

  $config_auth_usertable   = "user";
  $config_auth_leveltable  = ""; // defaults to usertable
  $config_auth_accesstable = "access";
  $config_auth_userfield   = "userid";
  $config_auth_userpk = "userid";//primary key of usertable
  $config_auth_passwordfield = "password";
  $config_auth_accountdisablefield = "";
  $config_auth_levelfield = "entity";
  $config_auth_mail_port = "110"; // default pop3 port
  $config_auth_mail_virtual = false; // no vmail.
  $config_max_loginattempts = 5; // 0 = no maximum.
  $config_auth_dropdown = false;
  $config_auth_userdescriptor = "[".$config_auth_userfield."]";
  // this parameter can be used to specify a where clause which will be used to validate users login credentials
  $config_auth_accountenableexpression = "";

  // LDAP settings
  // to use LDAP you should fill this config_variabels with the right values
  $config_authentication_ldap_host    = "";
  $config_authentication_ldap_context = "";
  $config_authentication_ldap_field   = "";

  $config_logging = 0; // no logging;
  $config_logfile = "/tmp/atk-security.log";

  $config_debug = 0;
  $config_display_errors = true;
  $config_halt_on_error = "critical";

  // Layout config
  $config_menu_delimiter = "<br>";
  $config_menu_pos = "left";
  $config_menu_layout = "default";
  $config_top_frame = 0; // 0 = no   - 1 = yes
  $config_defaulttheme = "default";
  $config_defaultlanguage = "nl";
  $config_multilanguage_linked = true; // True: one language switch attributes automatically switches all others on screen.
                                       // False: each language switch attributes operates only on it's own node

  // Display a 'stack' of the user activities in the top right corner.
  $config_stacktrace = true;

  // An administrator password that is empty will *DISABLE* administrator login!
  $config_administratorpassword = "";

  // A guest password that is empty will *DISABLE* guest login!
  $config_guestpassword = "";

  // Module path (without trailing slash!)
  $config_module_path = $config_atkroot."modules";

  // Automatic error reporting is turned off by default.
  $config_mailreport = "";

  $config_search_defaultmode = "substring";

  // Whether the action links in a recordlist appear left or right
  $config_recordlist_orientation  = "left";
  $config_recordlist_vorientation = "middle";
	
  // Use icons for action links or not  
	$config_recordlist_icons = "true";
	
  $config_enable_ie_extensions = false;

  // Whatever tabs are enabled or not
  $config_tabs = true;  

  // Backwardscompatibility setting. Set this to MYSQL_BOTH if your
  // software relies on numerical indexes (WHICH IS A BAD IDEA!!)
  $config_mysqlfetchmode = MYSQL_ASSOC;
  
  $config_atktempdir = $config_atkroot."atktmp/";
  
  
  // Template engine configuration
  $config_tplroot = $config_atkroot; // By default all templates are described by their relative
                                     // path, relative to the applications' root dir.
  $config_tplcaching = false;
  $config_tplcachelifetime = 3600; // default one hour
  $config_tplcompiledir = $config_atktempdir."compiled/tpl/";
  $config_tplcachedir = $config_atktempdir."tplcache/";
  

  // files that are allowed to be included by the include wrapper script
  // NOTE: this has nothing to do with useattrib and userelation etc.!
  $config_allowed_includes = array("atk/lock/lock.php", "atk/lock/lock.js.php", "atk/javascript/class.atkdateattribute.js.inc",
                                               "atk/popups/help.inc", "atk/popups/colorpicker.inc");

  // fullscreen mode (IE only)
  $config_fullscreen = false;
?>

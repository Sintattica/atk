<?php
  // the application identifier (used for sessions)
  $config_identifier   = "default";

  // set several default configuration options
  $config_databasehost = "localhost";
  $config_databasename = "";
  $config_databaseuser = "";
  $config_databasepassword = "";

  // mysql, oci8 or pgsql
  $config_database="mysql";

  $config_languagefile="english.lng";
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
  $config_auth_passwordfield = "password";
  $config_auth_accountdisablefield = "locked";
  $config_auth_levelfield = "entity";
  $config_auth_mail_port = "110"; // default pop3 port
  $config_auth_mail_virtual = false; // no vmail.
  $config_max_loginattempts = 5;

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
  $config_recordlist_orientation  = "right";
  $config_recordlist_vorientation = "middle";

  $config_enable_ie_extensions = false;

  // Whatever tabs are enabled or not
  $config_tabs = true;

  // include optimizer
  $config_atk_optimized_includes = true;

  // files that are allowed to be included by the include wrapper script
  // NOTE: this has nothing to do with useattrib and userelation etc.!
  $config_allowed_includes = array("atk/lock/lock.php", "atk/lock/lock.js.php");
?>

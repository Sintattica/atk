<?php

  // set several default configuration options
  $config_databasehost = "localhost";
  $config_databasename = "";
  $config_databaseuser = "";
  $config_databasepassword = "";

  // mysql, oci8 or pgsql
  $config_database="mysql";

  $config_languagefile="english.lng";
  $config_recordsperpage=25; 

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
  $config_auth_leveltable  = "user";
  $config_auth_accesstable = "access";
  $config_auth_userfield   = "userid";
  $config_auth_passwordfield = "password";  
  $config_auth_levelfield = "entity";
  $config_auth_mail_port = "110"; // default pop3 port
  $config_auth_mail_virtual = false; // no vmail.
  
  $config_logging = 0; // no logging;
  $config_logfile = "/tmp/atk-security.log";

  $config_debug = 0;
  $config_halt_on_error = "critical";
  
  // Layout config
  $config_menu_delimiter = "<br>";
  $config_menu_pos = "left";
  $config_top_frame = 0; // 0 = no   - 1 = yes
  $config_defaulttheme = "default";
 
  // Display a 'stack' of the user activities in the top right corner.
  $config_stacktrace = true;

  // An administrator password that is empty will *DISABLE* administrator login!
  $config_administratorpassword = "";

  // Module path (without trailing slash!)
  $config_module_path = "./modules";
  
  // Automatic error reporting is turned off by default.
  $config_mailreport = "";

  $config_search_defaultmode = "substring";
  
  // Whether the action links in a recordlist appear left or right 
  $config_recordlist_orientation = "right";

?>

<?

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
  $config_securityscheme = "none";
  $config_restrictive = true;
  $config_security_attributes = false;

  $config_auth_usertable   = "user";
  $config_auth_accesstable = "access";
  $config_auth_userfield   = "userid";
  $config_auth_passwordfield = "password";  
  
  $config_logging = 0; // no logging;
  $config_logfile = "/tmp/atk-security.log";

  $config_debug = 0;
  $config_halt_on_error = "critical";
  
  // Layout config
  $config_menu_delimiter = "<br>";
  $config_menu_pos = "left";
  $config_top_frame = 0; // 0 = no   - 1 = yes
  $config_defaulttheme = "default";

  // An administrator password that is empty will *DISABLE* administrator login!
  $config_administratorpassword = "";

?>

<?php
  // ATK Configuration file. 
  
  //----------------- DATABASE CONFIGURATION --------------------

  // Specify the type of database. 
  // Currently supported are: "mysql", "oci8" and "pgsql".
  $config_database="mysql";

  // The database configuration. Specify the hostname of the database server,
  // the database to use and the user/password.
  $config_databasehost = "localhost";
  $config_databasename = "";
  $config_databaseuser = "";
  $config_databasepassword = "";
  
  // In admin pages, atk shows you a number of records with previous and
  // next buttons. You can specify the number of records to show on a page.
  $config_recordsperpage=25; 
  
  //----------------- LAYOUT CONFIGURATION --------------------

  // The theme defines the layout of your application. You can see which 
  // themes there are in the directory atk/themes.
  $config_defaulttheme = "default";
  
  // The language of the application. You can use any language for which
  // a language file is present in the atk/languages directory.
  $config_languagefile="english.lng";
  
  // Menu configuration. You can have a menu at "top", "left", "right" or 
  // "bottom". If you use a horizontal menu (top or bottom, you may want
  // to change the menu_delimiter to " " (menu_delimiter is what atk
  // puts between menu items).
  $config_menu_pos = "left"; 
  $config_menu_delimiter = "<br>";
  
  // If you have a menu at either left or right, you can add an
  // extra topframe by setting the following option to 1.
  // If you set it to 1, you must provide a "top.php" file in
  // your application directory.
  $config_top_frame = 0;
  
  //----------------- SECURITY CONFIGURATION --------------------

  // The type of authentication (user/password verification) to use.
  // Currently supported are: 
  // "none"   - no authentication
  // "config" - users/passwords are set in this configfile
  // "db"     - users/passwords are stored in a table in the database.
  // "mail" - users / passwords are stored in the POP3 or IMAP server
  $config_authentication = "none";
      
  // NOTE, the following options are only useful if authentication is not 
  // set to "none".    
  
  // This parameter specifies whether the passwords are stored as an md5 
  // string in the database / configfile / whatever. 
  // If set to false, the passwords are assumed to be plain text.   
  // Note: Not all authentication methods support md5! e.g, if you use
  //       pop3 authentication, set this to false.
  // Note2: If set to false, and authentication_cookie is set to true,
  //        the password in the cookie will be stored plaintext!!!
  $config_authentication_md5 = true;
  
  // If you specify an administrator password here, you are always able 
  // to login using user 'administrator' and the specified password,
  // regardless of the type of authentication used!
  // if you set it to nothing (""), administrator login is disabled,
  // and only valid users are allowed to login (depending on the type of
  // authentication used).
  $config_administratorpassword = "";
  
  // The security scheme is used to determine who is allowed to do what.
  // Currently supported:
  // "none"   - anyone who is logged in may do anything.
  // "level"  - users have a certain level, and certain features of the 
  //            application require a minimum level.
  // "group"  - users belong to a group, and certain features may only
  //            be executed by a specific group. 
  $config_securityscheme = "none";
  
  // If config_restrictive is set to true, access is denied for all features
  // for which no access requirements are set. If set to false, access is 
  // always granted if no access requirements are set.
  $config_restrictive = true;

  // If set to true, a cookie with username/password is written, so 
  // users will stay logged in, even if they close their browser. 
  $config_authentication_cookie = false;
  
  // If you use "db" as authentication type, you can set the table and fields
  // from which atk should read the username and password information.
  $config_auth_usertable   = "user";
  $config_auth_userfield   = "userid";
  $config_auth_passwordfield = "password";
  
  // if you use "mail" as authentication, you have to fill in these
  //parameters.
  // mail login type
  // "" = normal mail type
  // "vmailmgr" = for virtual mail, it will add "@<domain>"
  $config_authentication_mail_login_type = "vmailmgr";
  // Mail suffix, only is mail login type = "vmailmgr"
  $config_authentication_mail_suffix = "ibuildings.nl";
  // Mail server name
  $config_authentication_mail_server = "localhost";
  // Port of the mail server
  $config_authentication_mail_port = 143;

  //if you use "ldap" as authentication, these parameters are nessesary
  $config_authentication_ldap_host = "";
  $config_authentication_ldap_context = "";
  
  // Atk can write security events to a logfile. 
  // There are several values you can choose for $config_logging.
  // 0 - No logging
  // 2 - Log actions ("User x performed action x on module y")
  $config_logging = 0;
  $config_logfile = "/tmp/atk-security.log";

  // If you have config_authentication set to "config", you may now specify
  // a set of usernames and passwords (in plain text). 
  // Example:
  // user("harry","password");
  
  // If securityscheme is "level" or "group", you may also specify the 
  // level or group as third parameter:
  // user("harry","password",3);
  // user("harry","password","admins");

  // The debug level.
  // 0 - No debug information
  // 1 - Print some debug information at the bottom of each screen
  // 2 - Print debug information, and pause before redirects
  $config_debug = 0;
  
  // The automatic error reporter
  // Error reports are sent to the given email address.
  // If you set this to "", error reporting will be turned off.
  // WARNING: DO NOT LEAVE THIS TO THE DEFAULT ADDRESS OR PREPARE TO BE
  // SEVERELY FLAMED!  
  $config_mailreport = "ivo@ibuildings.net";

?>

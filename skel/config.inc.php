<?php
  // ATK Configuration file. 
  
  // Unique application identifier
  // Set this when using multiple applications within one website
  $config_identifier = "atkapp";

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
  
  //----------------- DEBUGGER CONFIGURATION ----------------------
  
  // The automatic error reporter
  // Error reports are sent to the given email address.
  // If you set this to "", error reporting will be turned off.
  // WARNING: DO NOT LEAVE THIS TO THE DEFAULT ADDRESS OR PREPARE TO BE
  // SEVERELY FLAMED!  
  // $config_mailreport = "ivo@ibuildings.net";
  
    // The debug level.
  // 0 - No debug information
  // 1 - Print some debug information at the bottom of each screen
  // 2 - Print debug information, and pause before redirects
  $config_debug = 1;
  
  //----------------- LAYOUT CONFIGURATION --------------------

  // The theme defines the layout of your application. You can see which 
  // themes there are in the directory atk/themes.
  $config_defaulttheme = "outlook";
  
  // The language of the application. You can use any language for which
  // a language file is present in the atk/languages directory.
  $config_language="en";
  
  // Menu configuration. You can have a menu at "top", "left", "right" or 
  // "bottom". If you use a horizontal menu (top or bottom, you may want
  // to change the menu_delimiter to " " (menu_delimiter is what atk
  // puts between menu items).
  $config_menu_pos = "left"; 
  $config_menu_delimiter = "<br>";
  $config_menu_layout = "plain";
  
  // If you have a menu at either left or right, you can add an
  // extra topframe by setting the following option to 1.
  // If you set it to 1, you must provide a "top.php" file in
  // your application directory.
  $config_top_frame = 1;
  
  // This configures whether the action links (edit/delete) in a recordlist 
  // appear to the left or right of the records. If you are crazy, you 
  // might try the option "both".
  $config_recordlist_orientation = "right";

  // If the users are using IE, then the application can be run in fullscreen
  // mode. Set the next variable to true to enable this:
  $config_fullscreen = false;
  
  //----------------- SECURITY CONFIGURATION --------------------

  // The type of authentication (user/password verification) to use.
  // Currently supported are: 
  // "none"   - no authentication
  // "config" - users / passwords are set in this configfile
  // "db"     - users / passwords are stored in a table in the database.
  // "imap"   - users / passwords are stored in the IMAP server
  // "pop3"   - users / passwords are stored in the POP3 server
  // if you need to use multiple authentication types list them delimited by comma
  $config_authentication = "none";

  // The type of authorization (what is a user allowed to do?)
  // Normally this will be the same as the authentication, but in
  // special cases like POP3 authentication you might want to
  // authorize via a table in the database.
  // $config_authorization = "none";
      
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
  $config_administratorpassword = "demo";
  
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
   
  // if you work with groups/levels  you need these parameters 
  $config_auth_leveltable  = "users";
  $config_auth_levelfield  = "entity";
  
  // use this if you want a loginform instead off an htaccess boxske
  // currently this is only supported when using the outlook theme
  $config_auth_loginform = true;
  $config_max_loginattempts = 5;

  // if you use "pop3" or "imap" as authentication, you have to fill in 
  // these parameters:
  
  // Set this to true if you have virtual mail domains.
  // Atk will append '@' and the config_auth_mail_suffix
  // to the login name.
  // $config_auth_mail_virtual = false;
  
  // Mail suffix, if mail_virtual is set to true.
  //$config_auth_mail_suffix = "ibuildings.nl";
  
  // Mail server name
  // $config_auth_mail_server = "localhost";
  
  // Port of the mail server (default is 110 (pop3) but you can set it 
  // to 143 (imap) or something else.
  // $config_auth_mail_port = 143;

  // if you use "ldap" as authentication, these parameters are nessesary
  // $config_auth_ldap_host = "";
  // $config_auth_ldap_context = "";
  
  // The application root
  // if you're using urlrewrites within your httpd or htaccess configuration i think this should be '/'
  // be careful with this setting because it could create a major securityhole.
  // It is used to set the cookiepath when using PHP sessions.
  $config_application_root = str_replace($_SERVER["DOCUMENT_ROOT"],"",dirname(__FILE__)."/");

  // Atk can write security events to a logfile. 
  // There are several values you can choose for $config_logging.
  // 0 - No logging
  // 1 - Log logins
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

?>

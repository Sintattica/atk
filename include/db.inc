<?
  // NOTE:
  // $config_atkroot should be specified in the mainprogram
  // which uses this include file
  //
  // $config_atkroot = "../../";
  // include $config_atkroot."atk/include/db.inc";
  // ------------------------------------------------------

  // include our config (contains DB type)
  include_once($config_atkroot."config.inc.php");

  // include ATK tools (contains atkconfig function)
  include_once($config_atkroot."atk/atktools.inc");
  
  /**
   * Get an instance of an atk databaseconnection class.
   *
   * This function is called once automatically in atk/include/db.inc
   * to instantiate the global $g_db object.
   * It is only useful to call this function again if you need
   * a connection to another database.
   *
   * All parameters are optional. If a parameter is not specified,
   * the values from the configfile are used. (For example, if you
   * call getDb with no parameter at all, you get a default database
   * connection, like $g_db).
   *
   * @param $database The name of the database to use
   * @param $user The username for connecting to the database
   * @param $password The password for connecting to the database
   * @param $host The servername on which the database is running.
   * @param $type The databasetype ("mysql", "pgsql", "oci8", etc)
   *
   * @return An instance of a database class.
   */ 
  function getDb($database="", $user="", $password="", $host="", $type="")
  {
    global $config_databasename, $config_databaseuser, $config_databasepassword, $config_databasehost, $config_database, $config_atkroot;
    if ($database=="") $database = $config_databasename;
    if ($user=="") $user = $config_databaseuser;
    if ($password=="") $password = $config_databasepassword;
    if ($host=="") $host = $config_databasehost;
    if ($type=="") $type = $config_database;
    
    // include DB stuff
    require_once($config_atkroot."atk/db/class.atk".$type."db.inc");
    require_once($config_atkroot."atk/db/class.atkquery.inc");  
    
    // init DB
    $classname = "atk".$type."db";
    $db             = new $classname();
    $db->m_database = $database;
    $db->m_user     = $user;
    $db->m_password = $password;
    $db->m_host     = $host;
    $db->m_debug    = 0;
    return $db;
  }

  // Create the default database object:  
  $g_db = getDb();

?>

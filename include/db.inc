<?php
  /**
   * This file is part of the Achievo ATK distribution.
   * Detailed copyright and licensing information can be found
   * in the doc/COPYRIGHT and doc/LICENSE files which should be 
   * included in the distribution.
   *
   * The file contains functions for retrieving database
   * connections.
   * Note: $config_atkroot should be specified in the file which 
   * includes this file. Not doing so could lead to vulnerabilities
   * because of dynamic includes.
   *
   * <b>Example</b>
   * <code>
   *   $config_atkroot = "../../";
   *   include $config_atkroot."atk/include/db.inc";
   * </code>
   *
   * @package atk
   * @subpackage db
   *
   * @author Wim Kosten <wim@ibuildings.nl>
   *
   * @copyright (c)2000-2004 Ibuildings.nl BV   
   * @license http://www.achievo.org/atk/licensing ATK Open Source License
   *
   * @version $Revision$
   * $Id$
   */
     

  /**
   * include our config (contains DB type)
   */
  include_once($config_atkroot."config.inc.php");

  /**
   * include ATK tools (contains atkconfig function)
   */
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
   * call getDb with no parameter at all, you get a second default
   * database connection, like $g_db).
   *
   * @param String $database The name of the database to use
   * @param String $user The username for connecting to the database
   * @param String $password The password for connecting to the database
   * @param String $host The servername on which the database is running.
   * @param String $type The database driver to use ("mysql", "oci8", etc)
   *
   * @return atkDb An instance of a database connection.
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

  /**
   * The global default database object.
   * @global atkDb $g_db
   */
  $g_db = getDb();

?>
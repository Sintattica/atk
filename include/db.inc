<?
  // NOTE:
  // $config_atkroot should be specified in the mainprogram
  // which uses this include file
  //
  // $config_atkroot = "../../";
  // include $config_atkroot."atk/include/db.inc";
  // ------------------------------------------------------

  // include our config (contains DB type)
  include $config_atkroot."config.inc.php";

  // include ATK tools (contains atkconfig function)
  include $config_atkroot."atk/atktools.inc";

  // include DB stuff
  require_once($config_atkroot."atk/db/class.atk".$config_database."db.inc");
  require_once($config_atkroot."atk/db/class.atkquery.inc");

  // init DB
  $atkdb            = "atk".$config_database."db";
  $g_db             = new $atkdb();
  $g_db->m_database = $config_databasename;
  $g_db->m_user     = $config_databaseuser;
  $g_db->m_password = $config_databasepassword;
  $g_db->m_host     = $config_databasehost;
  $g_db->m_debug    = 0;
?>

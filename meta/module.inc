<?php
/**
 * The meta module.
 *
 * To enable this module just add the following line
 * in the config.modules.inc file:
 *
 *   module("meta", atkconfig("atkroot")."/atk/meta");
 *
 * If you are not logged in as administrator you might have to
 * do so to get access to the module and/or give yourself
 * the proper access rights.
 * 
 * @author Peter C. Verhage <peter@achievo.org>
 * @version $Revision$
 */
class mod_meta extends atkModule
{
  /**
   * Return dummy node.
   */
  function getNodes()
  {
    registerNode("meta.meta", array("admin", "add", "edit", "delete"));
  }
  
  /**
   * Return menu items.
   */
  function getMenuItems()
  {    
    global $g_db;
    $tables = $g_db->table_names();

    menuitem("meta", "", "main", array("meta.meta", "admin"));
    foreach ($tables as $table)
      menuitem($table["table_name"], dispatch_url("meta.".$table["table_name"], "admin"), "meta", array("meta.meta", "admin"));
  }

  /**
   * Construct a new meta node.
   * @param $node the node type
   * @return new node object
   */
  function &newNode($node)
  {
    $table = getNodeType($node);
    $node = new atkMetaNode(array("type" => $table, "table" => $table, "handler" => new atkMetaHandler(array(&$this, "initMetaPolicy"))));
    $node->m_module = "meta";
    $node->setSecurityAlias("meta.meta");
    return $node;
  }
  
  /**
   * Initialize meta policy.
   */
  function initMetaPolicy(&$policy)
  {
    $attrs = &$policy->getAttributes();
    
    foreach(array_keys($attrs) as $name)
    {
      if (hasFlag($attrs[$name]["flags"], AF_AUTOKEY))
      {
        $attrs[$name]["flags"] ^= AF_HIDE;
        $attrs[$name]["flags"] |= AF_READONLY|AF_HIDE_ADD;
      }
    }
  }
}
?>
<?php
/**
 * The meta module.
 *
 * Plug & play. :)
 * 
 * @author Peter C. Verhage <peter@ibuildings.nl>
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
    $node = new atkMetaNode(array("type" => $table, "table" => $table, "handler" => new atkMetaHandler(array($this, "initMetaPolicy"))));
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
      if (($attrs[$name]["flags"] & AF_AUTOKEY) == AF_AUTOKEY)
      {
        $attrs[$name]["flags"] ^= AF_HIDE;
        $attrs[$name]["flags"] |= AF_READONLY;
      }
    }
    
    $policy->removeFlags(array_keys($attrs), AF_HIDE);
  }
}
?>
<?php
atkimport("atk.meta.atkmetapolicy");
atkimport("atk.meta.atkmetahandler");

/**
 * The ATK Meta Node class.
 *
 * Makes it possible to create nodes in 1 line of code
 * using metadata from the database.
 *
 * @author Peter C. Verhage <peter@achievo.org>
 * @version $Revision$
 */
class atkMetaNode extends atkNode
{
  var $m_metaPolicy = NULL;
  var $m_metaHandler = NULL;

  /**
   * Constructor.
   *
   * This constructor accepts a variety of parameters in different order.
   * To make this possible (and for supporting passing parameters by reference)
   * the constructor accepts an array which may contain the following fields:
   *
   * - type      node type
   * - table     table name
   * - sequence  sequence name
   * - db        database instance
   * - policy    meta policy, the meta policy to use ((sub-)class atkMetaPolicy instance)
   * - handler   meta handler, the meta handler to use ((sub-)class atkMetaPolicy instance)   
   * - flags     node flags
   */
  function atkMetaNode($args=array())
  {
    $this->atkNode(array_key_exists("type", $args) ? $args["type"] : get_class($this), array_key_exists("flags", $args) ? $args["flags"] : 0);

    global $g_db;
    if (array_key_exists("db", $args))
      $this->m_db = &$args["db"];
    else $this->m_db = &$g_db;
    
    $policy = NULL;
    if (array_key_exists("policy", $args))
      $policy = &$args["policy"];
    else $policy = &$this->_defaultMetaPolicy();
    $this->setMetaPolicy($policy);

    $handler = NULL;
    if (array_key_exists("handler", $args))
      $handler = &$args["handler"];
    else $handler = &$this->_defaultMetaHandler();
    $this->setMetaHandler($handler);
    
    $this->m_table = array_key_exists("table", $args) ? $args["table"] : $policy->getTable();
    $this->m_seq = array_key_exists("sequence", $args) ? $args["sequence"] : $policy->getSequence();

    $this->applyMetaPolicy($this->m_metaPolicy);
  }
  
  /**
   * Returns the default meta policy.
   */
  function &_defaultMetaPolicy()
  {
    return atkMetaPolicy::defaultInstance($this);  
  }

  /**
   * Returns the default meta handler.
   */
  function &_defaultMetaHandler()
  {
    return atkMetaHandler::defaultInstance($this);  
  }
  
  /**
   * Returns the table.
   */
  function getTable()
  {
    return $this->m_table;
  }

  /**
   * Returns the meta policy.
   */
  function &getMetaPolicy()
  {
    return $this->m_metaPolicy;
  }
  
  /**
   * Sets the meta policy.
   */
  function setMetaPolicy(&$metaPolicy)
  {
    $this->m_metaPolicy = &$metaPolicy;
    $this->m_metaPolicy->setNode($this);
  }
  
  /**
   * Sets the meta handler.
   */
  function setMetaHandler(&$metaHandler)
  {
    $this->m_metaHandler = &$metaHandler;
  }  
  
  /**
   * Initialize the meta policy data.
   */
  function applyMetaPolicy(&$metaPolicy)
  {
    $metaPolicy->init();
    $this->m_metaHandler->handle($metaPolicy, $this);
    $metaPolicy->apply();
  }

  /**
   * Make it possible to set some of the
   * meta policy options.
   */
  function meta(&$metaPolicy)
  {
  }
}
?>
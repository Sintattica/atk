<?php
/**
 * This file is part of the Achievo ATK distribution.
 * Detailed copyright and licensing information can be found
 * in the doc/COPYRIGHT and doc/LICENSE files which should be
 * included in the distribution.
 *
 * @package atk
 *
 * @copyright (c) 2004-2005 Peter C. Verhage   
 * @license http://www.achievo.org/atk/licensing ATK Open Source License
 *
 * @version $Revision$
 * $Id$
 */

/**
 * imports..
 */
atkimport("atk.atknode");
atkimport("atk.meta.grammar.atkmetagrammar");
atkimport("atk.meta.atkmetapolicy");
atkimport("atk.meta.atkmetahandler");

/**
 * The ATK Meta Node class.
 *
 * Makes it possible to create nodes in 1 line of code
 * using metadata from the database.
 *
 * @author Peter C. Verhage <peter@achievo.org> 
 *
 * @package atk
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
   * - type        node type
   * - table       table name
   * - sequence    sequence name
   * - db/database database name or instance
   * - policy      meta policy, the meta policy to use ((sub-)class atkMetaPolicy instance)
   * - handler     meta handler, the meta handler to use ((sub-)class atkMetaPolicy instance)   
   * - grammar     meta grammar, the meta grammar to use ((sub-)class atkMetaGrammar instance)
   * - flags       node flags
   * - descriptor  descriptor template for this node
   * - order       (default) order to sort fields
   * - filter      filter
   * 
   * All of these variables can also be specified by creating a class variable with
   * the same name. If you do so for flags, and have to use multiple flags, use
   * an array of flags.
   */
  function atkMetaNode($args=array())
  {
    $type = get_class($this);
    if (isset($this->type))
      $type = $this->type;
    else if (array_key_exists("type", $args))
      $type = $args["type"]; 
    
    $flags = 0;
    if (isset($this->flags))
    {
      $flags = $this->flags;
      if (is_array($flags))
      {
        $flagList = $flags;
        $flags = 0;
        foreach ($flagList as $flag)
          $flags |= $flag;        
      }
    }
    else if (array_key_exists("flags", $args))
      $flags = $args["flags"]; 

    $this->atkNode($type, $flags);
    
    if (isset($this->db) && is_string($this->db))
      $this->m_db = $this->db;
    else if (isset($this->db) && is_object($this->db))
      $this->m_db = &$this->db;
    else if (array_key_exists("db", $args) && is_string($args["db"]))
      $this->m_db = $args["db"];
    else if (array_key_exists("db", $args) && is_object($args["db"]))
      $this->m_db = &$args["db"];
    else if (isset($this->database) && is_string($this->database))
      $this->m_db = $this->database;
    else if (isset($this->database) && is_object($this->database))
      $this->m_db = &$this->database;
    else if (array_key_exists("database", $args) && is_string($args["database"]))
      $this->m_db = $args["database"];
    else if (array_key_exists("database", $args) && is_object($args["database"]))
      $this->m_db = &$args["database"];     
    
    if (isset($this->meta_policy))
      $policy = &$this->meta_policy;
    else if (array_key_exists("policy", $args))
      $policy = &$args["policy"];
    else $policy = &$this->_defaultMetaPolicy();
    $this->setMetaPolicy($policy);

    if (isset($this->meta_handler))
      $handler = &$this->meta_handler;
    else if (array_key_exists("handler", $args))
      $handler = &$args["handler"];
    else $handler = &$this->_defaultMetaHandler();
    $this->setMetaHandler($handler);
    
    if (isset($this->meta_grammar) && is_object($this->meta_grammar))
      $grammar = &$this->meta_grammar;
    else if (isset($this->meta_grammar))    
      $grammar = &atkMetaGrammar::getInstance($this->meta_grammar);
    else if (array_key_exists("grammar", $args) && is_object($args["grammar"]))
      $grammar = &$args["grammar"];
    else if (array_key_exists("grammar", $args))
      $grammar = &atkMetaGrammar::getInstance($args["grammar"]);      
    else $grammar = &atkMetaGrammar::getInstance();      
    $this->m_metaPolicy->setGrammar($grammar);    

    if (isset($this->table)) 
      $this->m_table = $this->table;
    else if (array_key_exists("table", $args)) 
      $this->m_table = $args["table"];
    else $this->m_table = $policy->getTable();
    
    if (isset($this->sequence)) 
      $this->m_seq = $this->sequence;
    else if (array_key_exists("sequence", $args)) 
      $this->m_seq = $args["sequence"];
    else $this->m_seq = $policy->getSequence();
    
    if (isset($this->descriptor))
      $this->setDescriptorTemplate($this->descriptor);
    else if (array_key_exists("descriptor", $args))
      $this->setDescriptorTemplate($args["descriptor"]);
      
    if (isset($this->order))
      $this->setOrder($this->order);
    else if (array_key_exists("order", $args))
      $this->setOrder($args["order"]); 
    
    if (isset($this->filter))
      $this->addFilter($this->filter);
    else if (array_key_exists("filter", $args))
      $this->addFilter($args["filter"]);       

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
   * Returns the default meta grammar.
   */
  function &_defaultMetaGrammar()
  {
    return atkMetaGrammar::defaultInstance($this);  
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
   * Sets the meta grammar.
   */
  function setMetaGrammar(&$metaGrammar)
  {
    $this->m_metaGrammar = &$metaGrammar;
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
  
  /**
   * Returns the default values for the node's attributes.
   */
  function initial_values()
  {
    return $this->m_metaPolicy->getDefaultValues();
  }
}
?>

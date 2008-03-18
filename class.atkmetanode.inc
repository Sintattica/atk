<?php
/**
 * This file is part of the Achievo ATK distribution.
 * Detailed copyright and licensing information can be found
 * in the doc/COPYRIGHT and doc/LICENSE files which should be
 * included in the distribution.
 *
 * @package atk
 *
 * @copyright (c) 2004-2007 Peter C. Verhage
 * @license http://www.achievo.org/atk/licensing ATK Open Source License
 *
 * @version $Revision$
 * $Id$
 */

atkimport("atk.atknode");

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
  /**
   * Meta options.
   *
   * @var array
   */
  private $m_metaOptions;
  
  /**
   * Constructor.
   *
   * This constructor accepts a variety of parameters in different order.
   * To make this possible (and for supporting passing parameters by reference)
   * the constructor accepts an array which may contain the following fields:
   *
   * - type           node type
   * - table          table name
   * - sequence       sequence name
   * - db/database    database name or instance
   * - policy         meta policy, the meta policy to use ((sub-)class atkMetaPolicy instance)
   * - grammar        meta grammar, the meta grammar to use ((sub-)class atkMetaGrammar instance)
   * - compiler       meta compiler, the meta compiler to use ((sub-)class atkMetaCompiler instance)
   * - handler        meta handler, handler which needs to be called instead of the default meta method
   * - flags          node flags
   * - descriptor     descriptor template for this node
   * - order          (default) order to sort fields
   * - index          create indexed navigation on a attribute/fieldname
   * - filter         filter
   * - securityAlias  security alias for this node
   * - securityMap    security map for this node (will be added to the existing security map!)

   *
   * All of these variables can also be specified by creating a class variable with
   * the same name. If you do so for flags, and have to use multiple flags, use
   * an array of flags.
   * 
   * @param $options meta node options
   * 
   * @return atkMetaNode
   */
  public function __construct($options=array())
  {
    $this->m_metaOptions = $options;
    
    $type = $this->getMetaOption('type', strtolower(get_class($this)));
  
    parent::__construct($type);

    if (!$this->_constructFromCache())
      $this->_constructFromPolicy();
    
    $this->postMeta();
  }
  
  /**
   * Old-style constructor for backwards-compatibility.
   *
   * @return atkMetaNode
   */
  protected function atkMetaNode()
  {
    $args = func_get_args();
    call_user_func_array(array($this, '__construct'), $args);
  }
  
  /**
   * Returns the value for the meta option with the given name.
   * 
   * @param string $name     meta option name
   * @param mixed  $default fallback value
   * 
   * @return mixed meta option value
   */
  public function getMetaOption($name, $default=null)
  { 
    if (isset($this->$name))
    {
      return $this->$name;
    }
    else if (isset($this->m_metaOptions[$name]))
    {
      return $this->m_metaOptions[$name];
    }
    else 
    {
      return $default;
    }
  }  
  
  /**
   * Is the node structure cachable?
   * 
   * @return boolean cachable?
   */
  public function isCacheable()
  {
    if (!atkconfig('meta_caching', true)) return false;
    if (strtolower(get_class($this)) == 'atkmetanode') return false;
    if (!method_exists($this, 'meta')) return true;    
    $method = new ReflectionMethod(get_class($this), 'meta');
    return $method->isStatic();
  }  
  
  /**
   * Constructs the meta node from the cache if this node is cachable and the node
   * structure has been cached.
   * 
   * @return boolean is the node constructed from the cache?
   */
  private function _constructFromCache()
  {
    if (!$this->isCacheable()) return false;
    
    atkimport('atk.utils.atktmpfile');
    $file = new atkTmpFile("meta/".$this->getModule()."/".$this->getType().".php");
    if ($file->exists())
    {
      $node = $this;
      include($file->getPath());
      return true;
    }
    
    return false;
  }
  
  /**
   * Create policy.
   * 
   * @return atkMetaPolicy policy
   */
  protected function _createPolicy()
  {
    atkimport('atk.meta.atkmetapolicy');
    $policy = $this->getMetaOption('policy');
    return atkMetaPolicy::create($this, $policy);
  }
  
  /**
   * Constructs the meta node using the meta policy.
   */
  protected function _constructFromPolicy()
  {
    $policy = $this->_createPolicy();
    $policy->apply();
  }
  
  /**
   * Post meta. 
   * 
   * This method is called just after the node is constructed from the cache 
   * or using the meta policy and allows you to do some node initialization
   * which cannot be done by the meta policy.
   */
  public function postMeta()
  {
    // do nothing
  }
}
<?php
  /**
   * This file is part of the Achievo ATK distribution.
   * Detailed copyright and licensing information can be found
   * in the doc/COPYRIGHT and doc/LICENSE files which should be
   * included in the distribution.
   *
   * @package atk
   * @subpackage db
   *
   * @copyright (c)2008 Ibuildings.nl BV
   * @license http://www.achievo.org/atk/licensing ATK Open Source License
   *
   * @version $Revision$
   * $Id$
   */

  /**
   * ATK driver for clustered databases. This class proxies queries
   * to correct read/write slaves.
   * 
   * @author Boy Baukema <boy@ibuildings.nl>
   * @package atk
   * @subpackage db
   */
  class atkClusterDb
  {
    protected $m_name;

   /**
    * Array of read-only slaves
    * 
    * @var array
    */
    protected $m_readonly_nodes_config =array();
    
   /**
    * Array of write-only slaves
    * 
    * @var array
    */    
    protected $m_writeonly_nodes_config=array();
    protected $m_nodes_config = array();

    /**
     * Cluster node (database) that we are currently proxying for
     *
     * @var atkDb
     */
    protected $m_current_clusternode;

    ////////////////////////////// ATKDB METHODS //////////////////////////////

   /**
	* Initialize the atkClusterDb
	* 
	* @param string $connectionname The name of the database connection
	* @param string $mode			Mode can be r, w or rw
	* 
	* @access public 
	* @return void
	*/
    public function init($connectionname, $mode='rw')
    {
      $this->m_name   = $connectionname;
      $this->setConfig();
      $this->setCurrentClusterNode($mode);
      return $this;
    }

   /**
    * Connects to a cluster node and sets the node as "current node" 
    * 
    * @param string $mode Mode can be r, w or rw
    * 
    * @access public
    * @return bool Whether the connect succeded or not
    */
    public function connect($mode='rw')
    {
      $this->setCurrentClusterNode($mode);
      return $this->m_current_clusternode->connect($mode);
    }

   /**
	* Returns nodes that have a specific mode set
	* 
	* @param string $mode Mode can be r, w or rw
	* 
	* @access public
	* @return array
	*/
    public function hasMode($mode)
    {
      static $s_modes=array();
      if (isset($s_modes[$mode])) return $s_modes[$mode];

      $configtypes = array($this->m_readonly_nodes_config,$this->m_writeonly_nodes_config,$this->m_nodes_config);
      foreach ($configtypes as $configtype)
      {
        foreach ($configtype as $node)
        {
          if (isset($node['mode']) && strstr($node['mode'],$mode))
          {
            $s_modes[$mode] = true;
          }
        }
      }
      if (!isset($s_modes[$mode])) $s_modes[$mode] = false;
      return $s_modes[$mode];
    }

    /**
     * Query method, first detects the query mode (read/write)
     * and connects to the proper database before executing the query on it.
     *
     * @return bool Wether or not the query was executed successfully
     */
    public function query()
    {
      $args = func_get_args();

      $this->connect(atkDb::getQueryMode($args[0]));

      return call_user_method_array('query',$this->m_current_clusternode, $args);
    }

   /**
	* Creates a new a new query object based on the current nodes type
	* 
	* @access public
	* @return object
	*/
    public function createQuery()
    {
      $query = &atknew("atk.db.atk{$this->m_current_clusternode->m_type}query");
      $query->m_db = $this;
      return $query;
    }

   /**
	* Creates a new new atkDDL based on current cluster nodes type
	* 
	* @access public 
	* @return atkDDL
	*/
    public function createDDL()
    {
      atkimport("atk.db.atkddl");
      $ddl = &atkDDL::create($this->m_current_clusternode->m_type);
      $ddl->m_db = $this;
      return $ddl;
    }

   /**
    * Gets the next available id 
    *
    * @access public
    * @return int
    */
    public function nextid()
    {
      $args = func_get_args();
      $this->connect('w');
      return call_user_method_array('nextid', $this->m_current_clusternode, $args);
    }

    ////////////////////////////// CLUSTER METHODS //////////////////////////////

   /**
    * Sets config and mode for all configured nodes
    *
    * @access protected 
    * @return void
    */
    protected function setConfig()
    {
      $dbconfig = atkconfig('db');
      $config = $dbconfig[$this->m_name];
      foreach ($config['nodes'] as $mode=>$nodes)
      {
        if (is_array($nodes))
        {
          foreach ($nodes as $node) $this->setNodeConfig($node, $dbconfig[$node], $mode);
        }
        else $this->setNodeConfig($nodes, $dbconfig[$nodes], $mode);
      }
    }

   /**
    * Sets the config and mode for a named node
    *
    * @param string $nodename
    * @param array  $nodeconfig
    * @param string $mode
    * 
    * @access protected
    * @return void
    */
    protected function setNodeConfig($nodename, $nodeconfig, $mode)
    {
      if      ($mode==='r') { $this->m_readonly_nodes_config[$nodename]  = $nodeconfig; }
      else if ($mode==='w') { $this->m_writeonly_nodes_config[$nodename] = $nodeconfig; }
      else                  { $this->m_nodes_config[$nodename]           = $nodeconfig; }
    }

   /**
    * Sets a random cluster node as the current node based on the mode provided
    *
    * @param string $mode
    * 
    * @access protected
    * @return void
    */
    protected function setCurrentClusterNode($mode)
    {
      if (!$this->m_current_clusternode || !$this->m_current_clusternode->hasMode($mode))
      {
        if      ($mode==='r' && !empty($this->m_readonly_nodes_config))  { $this->setRandomNodeFromNodeConfigs($this->m_readonly_nodes_config,$mode); }
        else if ($mode==='w' && !empty($this->m_writeonly_nodes_config)) { $this->setRandomNodeFromNodeConfigs($this->m_writeonly_nodes_config,$mode); }
        else                                                             { $this->setRandomNodeFromNodeConfigs($this->m_nodes_config         ,$mode); }
      }
    }

   /**
    * Selects a random node from the node configuration based
    * on the mode.
    *
    * @param array  $nodeconfigs
    * @param string $mode
    * 
    * @access protected
    * @return void
    */
    protected function setRandomNodeFromNodeConfigs($nodeconfigs,$mode)
    {
      $nodenames = array_keys($nodeconfigs);
      $number = mt_rand(0,count($nodeconfigs)-1);
      $nodename = $nodenames[$number];

      $this->m_current_clusternode = atkGetDb($nodename,false,$mode);
    }

    ////////////////////////////// OVERLOADING METHODS //////////////////////////////

   /**
    * Allows setting key/value pairs for the current node
    *
    * @param string $name
    * @param mixed  $value
    * 
    * @access public
    * @return void
    */
    public function __set($name, $value)
    {
      $this->m_current_clusternode->$name = $value;
    }

   /**
    * Gets a value from current nodes properties based on key
    *
    * @param string $name
    * @return void
    */
    public function __get($name)
    {
      return $this->m_current_clusternode->$name;
    }

   /**
    * Checks if current node has the property set
    *
    * @param string $name
    * @return bool
    */
    public function __isset($name)
    {
      return isset($this->m_current_clusternode->$name);
    }

    public function __unset($name)
    {
      unset($this->m_current_clusternode->$name);
    }

    public function __call($name, $arguments)
    {
      return call_user_method_array($name,$this->m_current_clusternode,$arguments);
    }

    public static function __callStatic($name, $arguments)
    {
      return call_user_func_array(array(__NAMESPACE__ .'::'.get_class($this->m_current_clusternode), $name), $arguments);
    }
  }

?>
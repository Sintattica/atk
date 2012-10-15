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
  class atkClusterDb extends atkDb
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

    /**
     * Magic unset function
     *
     * @param string $name
     */
    public function __unset($name)
    {
      unset($this->m_current_clusternode->$name);
    }

    /**
     * Magic call function
     *
     * @param string $name
     * @param array $arguments
     * @return mixed
     */
    public function __call($name, $arguments)
    {
      return call_user_method_array($name,$this->m_current_clusternode,$arguments);
    }

    /**
     * Magic callstatic function
     *
     * @param string $name
     * @param array $arguments
     * @return mixed
     */
    public static function __callStatic($name, $arguments)
    {
      return call_user_func_array(array(__NAMESPACE__ .'::'.get_class($this->m_current_clusternode), $name), $arguments);
    }
    
    /**
     * Because we extend atkDb __call won't be called for the atkDb public methods as they
     * are already implemented. We can't not-extend atkDb because this would break typehinting
     * (people are using atkDb as typehints everywhere). The most decent way to fix this issue
     * would be to make atkDb into an interface and then have atkClusterDb and atkDb implement it
     * (and use 'atkDbInterface' for typehinting instead of atkDb), but this would break backward
     * compatibility (maybe other people use atkDb in their code as well for typehinting) and since 
     * atkClusterDb doesn't seem to be used very often anyway for now we just solved this issue 
     * using some vogon poetry. Continue reading at your own risk.
     */
    
    public function setSequenceValue() { $args = func_get_args(); return $this->__call(__FUNCTION__, $args); }
    public function useMapping() { $args = func_get_args(); return $this->__call(__FUNCTION__, $args); }
    public function getMapping() { $args = func_get_args(); return $this->__call(__FUNCTION__, $args); }
    public function clearMapping() { $args = func_get_args(); return $this->__call(__FUNCTION__, $args); }
    public function getTranslatedDatabaseName() { $args = func_get_args(); return $this->__call(__FUNCTION__, $args); }
    public function _getOrUseMapping() { $args = func_get_args(); return $this->__call(__FUNCTION__, $args); }
    public function getType() { $args = func_get_args(); return $this->__call(__FUNCTION__, $args); }
    public function link_id() { $args = func_get_args(); return $this->__call(__FUNCTION__, $args); }
    public function hasError() { $args = func_get_args(); return $this->__call(__FUNCTION__, $args); }
    public function getErrorType() { $args = func_get_args(); return $this->__call(__FUNCTION__, $args); }
    public function getAtkDbErrno() { $args = func_get_args(); return $this->__call(__FUNCTION__, $args); }
    public function getDbErrno() { $args = func_get_args(); return $this->__call(__FUNCTION__, $args); }
    public function getDbError() { $args = func_get_args(); return $this->__call(__FUNCTION__, $args); }
    public function setUserError() { $args = func_get_args(); return $this->__call(__FUNCTION__, $args); }
    public function getQueryMode() { $args = func_get_args(); return $this->__call(__FUNCTION__, $args); }
    public function errorLookup() { $args = func_get_args(); return $this->__call(__FUNCTION__, $args); }
    public function getErrorMsg() { $args = func_get_args(); return $this->__call(__FUNCTION__, $args); }
    public function halt() { $args = func_get_args(); return $this->__call(__FUNCTION__, $args); }
    public function query_id() { $args = func_get_args(); return $this->__call(__FUNCTION__, $args); }
    public function doConnect($host, $user, $password, $database, $port, $charset)  { $args = func_get_args(); return $this->__call(__FUNCTION__, $args); }
    public function _translateError() { $args = func_get_args(); return $this->__call(__FUNCTION__, $args); }
    public function disconnect() { $args = func_get_args(); return $this->__call(__FUNCTION__, $args); }
    public function commit() { $args = func_get_args(); return $this->__call(__FUNCTION__, $args); }
    public function savepoint() { $args = func_get_args(); return $this->__call(__FUNCTION__, $args); }
    public function rollback() { $args = func_get_args(); return $this->__call(__FUNCTION__, $args); }
    public function next_record() { $args = func_get_args(); return $this->__call(__FUNCTION__, $args); }
    public function lock() { $args = func_get_args(); return $this->__call(__FUNCTION__, $args); }
    public function unlock() { $args = func_get_args(); return $this->__call(__FUNCTION__, $args); }
    public function affected_rows() { $args = func_get_args(); return $this->__call(__FUNCTION__, $args); }
    public function metadata() { $args = func_get_args(); return $this->__call(__FUNCTION__, $args); }
    public function table_names() { $args = func_get_args(); return $this->__call(__FUNCTION__, $args); }
    public function tableExists() { $args = func_get_args(); return $this->__call(__FUNCTION__, $args); }
    public function getrows() { $args = func_get_args(); return $this->__call(__FUNCTION__, $args); }
    public function getValue() { $args = func_get_args(); return $this->__call(__FUNCTION__, $args); }
    public function getValues() { $args = func_get_args(); return $this->__call(__FUNCTION__, $args); }
    public function getSearchModes() { $args = func_get_args(); return $this->__call(__FUNCTION__, $args); }
    public function tableMeta() { $args = func_get_args(); return $this->__call(__FUNCTION__, $args); }
    public function func_now() { $args = func_get_args(); return $this->__call(__FUNCTION__, $args); }
    public function func_substring() { $args = func_get_args(); return $this->__call(__FUNCTION__, $args); }
    public function func_datetochar() { $args = func_get_args(); return $this->__call(__FUNCTION__, $args); }
    public function func_concat() { $args = func_get_args(); return $this->__call(__FUNCTION__, $args); }
    public function func_concat_ws() { $args = func_get_args(); return $this->__call(__FUNCTION__, $args); }
    public function vendorDateFormat() { $args = func_get_args(); return $this->__call(__FUNCTION__, $args); }
    public function func_datetimetochar() { $args = func_get_args(); return $this->__call(__FUNCTION__, $args); }
    public function maxIdentifierLength() { $args = func_get_args(); return $this->__call(__FUNCTION__, $args); }
    public function escapeSQL() { $args = func_get_args(); return $this->__call(__FUNCTION__, $args); }
    public function toggleForeignKeys() { $args = func_get_args(); return $this->__call(__FUNCTION__, $args); }
    public function deleteAll() { $args = func_get_args(); return $this->__call(__FUNCTION__, $args); }
    public function dropAll() { $args = func_get_args(); return $this->__call(__FUNCTION__, $args); }
    public function cloneAll() { $args = func_get_args(); return $this->__call(__FUNCTION__, $args); }
    public function &getInstance() { $args = func_get_args(); return $this->__call(__FUNCTION__, $args); }
    public function &setInstance() { $args = func_get_args(); return $this->__call(__FUNCTION__, $args); }
    public function setHaltOnError() { $args = func_get_args(); return $this->__call(__FUNCTION__, $args); }
    public function getDbStatus() { $args = func_get_args(); return $this->__call(__FUNCTION__, $args); }
    public function quoteIdentifier() { $args = func_get_args(); return $this->__call(__FUNCTION__, $args); }
  }
?>
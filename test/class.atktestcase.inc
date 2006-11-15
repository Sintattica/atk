<?php
  /**
   * This file is part of the Achievo ATK distribution.
   * Detailed copyright and licensing information can be found
   * in the doc/COPYRIGHT and doc/LICENSE files which should be
   * included in the distribution.
   *
   * @package atk
   * @subpackage test
   *
   * @copyright (c)2005 Ibuildings.nl
   * @license http://www.achievo.org/atk/licensing ATK Open Source License
   *
   * @version $Revision$
   * $Id$
   */

  /**
   * atkTestCase is a specialization of SimpleTest's UnitTestCase. It
   * contains utility methods that can be used by testcases, such as
   * the ability to swap the default database driver with a mock
   * version
   *
   * @author Ivo Jansch <ivo@achievo.org>
   * @package atk
   * @subpackage test
   *
   */   
  class atkTestCase extends UnitTestCase
  {
    var $m_restoreDb = array();
    var $m_restoreNode = array();
    var $m_restoreSecMgr = NULL;
    
    var $m_useTestDatabase = NULL;
    var $m_fixtures = array();
    var $m_fixtureData = array();
    
    /**
     * Constructor.
     *
     * @return atkTestCase
     */
    function atkTestCase()
    {
    }
    
    /**
     * Use test database?
     * 
     * @return boolean use test database?
     */
    function useTestDatabase()
    {
      if ($this->m_useTestDatabase === NULL)
      {
        $config = atkconfig('db');
        $this->setUseTestDatabase(isset($config['test']));
      }
      
      return $this->m_useTestDatabase;
    }
    
    /**
     * Use test database?
     *
     * @param boolean $value use test database?
     */
    function setUseTestDatabase($value)
    {
      atkdebug("Use test database? => ".print_r($value, true));
      $this->m_useTestDatabase = $value;
    }

    /**
     * Called before each test method to setup some
     * data etc. By default this means the fixture data
     * will be loaded into the test database. Make sure
     * you call the parent if you override this method!
     */
    function setUp()
    {
      if ($this->useTestDatabase())
      {
        $this->_loadData();
      }
    }
    
    /**
     * Called after each test method. By default this
     * means the fixture data and any other data added
     * to the database will be removed from the test
     * database. Make sure you call the parent if you
     * override this method!
     */
    function tearDown()
    {
      if ($this->useTestDatabase())
      {
        $this->_cleanData();
      }
    }    
    
    /**
     * Returns the fixture for the given table name with 
     * the given name. If the fixture doesn't exist
     * NULL is returned.
     * 
     * @param string $table table name
     * @param string $name  fixture name
     * 
     * @return array fixture data
     */
    function fixture($table, $name)
    {
      if (isset($this->m_fixtureData[$table][$name]))
      {
        return $this->m_fixtureData[$table][$name];
      }
      else 
      {
        return NULL;
      }
    }

    /**
     * Override __call to intercept method calls for fixture data.
     * This makes it possible to access fixtures if there is a method
     * with the same name as the table the fixture is defined for. Only
     * works properly in PHP5.
     * 
     * @param string $method method name (table name)
     * @param array  $args   method arguments
     * 
     * @return array fixture data
     */
    function __call($method, $args)
    {
      if (isset($this->m_fixtureData[$method]))
      {
        return $this->fixture($method, $args[0]);
      }
      else 
      {
        atkerror("Invalid method name $method for ".get_class($this)."!");
      }
    }
    
    /**
     * Returns the test database.
     *
     * @return atkDb test database
     */
    function &_getTestDB()
    {
      return atkGetDb('test');
    }
    
    /**
     * Clean-up data in the test database so we have a nice clean
     * database for the next test method.
     */
    function _cleanData()
    {
      atkdebug("Clean test database...");
      
    	$db = &$this->_getTestDB();      
    	$config = atkconfig("db");
      
      if (!isset($config["test"]["transactional_fixtures"]) || 
          !$config["test"]["transactional_fixtures"])
      {
        $db->deleteAll();
      }
      else
      {
        $db->rollback();
      }
    }
    
    /**
     * Load fixture data into the test database.
     */
    function _loadData()
    {
      atkdebug("Load fixtures into test database...");
      
      if (count($this->m_fixtures) == 0) return;

      $this->m_fixtureData = array();
      foreach($this->m_fixtures as $fullname)
      {
        $this->_loadFixtureData($fullname);
      }
    }
    
    /**
     * Load data for the given fixture into the test database.
     * 
     * @param string $fullname full fixture name
     */
    function _loadFixtureData($fullname)
    {
      $path = $this->_getFixturePath($fullname);
      if ($path == NULL)
      {
        atkdebug("WARNING: no fixture data file found for fixture '{$fullname}'!");
        return;
      }
      
      $type = $this->_getFixtureType($path);
      $method = "_get{$type}FixtureData";
      if (!method_exists($this, $method))
      {
        atkdebug("WARNING: don't know how to load fixture data of type '{$type}' (fixture '{$fullname}')!");
      }
      
      $table = $this->_getFixtureTable($fullname);
      $db = &$this->_getTestDB();
      if (!$db->tableExists($table))
      {
        atkdebug("WARNING: table '{$table}' not found in test database (fixture '$fullname')!");
      }
      
      $data = $this->$method($fullname, $path);
      $this->_saveFixtureData($table, $data);
    }
    
    /**
     * Get the fixture path for the given fixture (full-)name.
     * 
     * @param string $fullname full fixture name
     * @return stringn fixture path
     */
    function _getFixturePath($fullname)
    {
      $parts = explode('.', $fullname);
      
      // beneath application root
      if (count($parts) == 1)
      {
        $fullname = 'testcases.fixtures.'.$fullname;
      }
      
      // beneath module directory
      else if (
        count($parts) == 2 &&
        !is_dir(atkconfig("atkroot").$parts[0].'/'.$parts[1]) && 
        moduleExists($parts[0]))
      {
        $fullname = 'module.'.$parts[0].'.testcases.fixtures.'.$parts[1];
      }
      
      // full path (without testcases/fixtures directory!)
      else
      {
        $fullname = implode('.', array_slice(0, count($parts) - 1)).'.testcases.fixtures.'.$parts[count($parts) - 1];
      }
      
      $path = getClassPath($fullname, false);
      $files = glob("{$path}.*");   
      
      return count($files) == 0 ? NULL : $files[0];
    }
    
    /**
     * Returns the fixture type for the given fixture path.
     * 
     * @param string $path fixture path
     * @return string fixture type
     */
    function _getFixtureType($path)
    {
      return substr($path, strrpos($path, '.') + 1);
    }
    
    /**
     * Returns the fixture table for the given full fixture name.
     * 
     * @param string $fullname full fixture name
     * @return string table name
     */
    function _getFixtureTable($fullname)
    {
      return end(explode('.', $fullname));
    }
    
    /**
     * Returns the YAML fixture data from the given path.
     *
     * @param string $fullname full fixture name
     * @param string $path     fixture path
     * 
     * @return array data
     */
    function _getYmlFixtureData($fullname, $path)
    {
      $contents = file_get_contents($path);
      $contents = $this->_parsePHP($contents);
    	include_once(atkconfig("atkroot")."atk/ext/spyc/spyc.php");
      $data = Spyc::YAMLLoad($contents);
      return $data;
    }
    
    /**
     * Returns the PHP fixture data from the given path.
     *
     * @param string $fullname full fixture name
     * @param string $path     fixture path
     * 
     * @return array data
     */
    function _getPHPFixtureData($fullname, $path)
    {
      $data = array();
      include_once($path);
      return $data;
    }
    
    /**
     * Parses the given string using PHP. Parsed results will be returned. 
     * PHP code must be surrounded by PHP open and close tags. Script code
     * has full access to all loaded ATK files.
     */
    function _parsePHP($contents)
    {
    	ob_start();
      eval(str_replace(array("?>\r\n", "?>\n"), array("?> \r\n", "?> \n"), "?>".$contents));
      $contents = ob_get_contents();
      ob_end_clean();
      return $contents;
    }
    
    /**
     * Save fixture data to the database in the given table.
     * 
     * @param string $table table name
     * @param array  $data  fixture data 
     */
    function _saveFixtureData($table, $data)
    {
      $db = &$this->_getTestDB();
            
      // save data
      foreach ($data as $item => $record)
      {
        // make fixture data available
        $this->m_fixtureData[$table][$item] = $record;
        
        // insert data into database
        $query = &$db->createQuery();
        $query->addTable($table);
        
        foreach($record as $field => $value)
        {
          $query->addField($field, $value);
        }
        
        $query->executeInsert();
      }
      
      // set sequence value(s)
      $metadata = $db->metadata($table);
      foreach($metadata as $field)
      {
        if (hasFlag($field['flags'], MF_AUTO_INCREMENT))
        {
          $sequence = isset($field['sequence']) ? $field['sequence'] : $table;
          $query = "SELECT MAX(" .$field['name'] .") AS value FROM $table";
          list($result) = $db->getRows($query);
          $db->setSequenceValue($sequence, $result['value']);
        }
      }
    }
    
    /**
     * Add the given fixtures (each argument can be the fullname of a fixture).
     */
    function addFixture()
    {
      $fixtures = func_get_args();
      $this->m_fixtures = array_merge($this->m_fixtures, $fixtures);
    }
    
    /**
     * Adds the given fixtures (each argument can be the fullname of a fixture).
     */
    function addFixtures()
    {
      $fixtures = func_get_args();
      $this->m_fixtures = array_merge($this->m_fixtures, $fixtures);
    }
    
    function setMockDb($conn=NULL)
    {
      global $config_db;
      
      if ($conn === NULL)
        $conn = atkDb::getDefault();

      $config_db["mock"]["driver"] = "mock";

      $mockdb = &atkGetDb("mock");
      $this->m_restoreDb[$conn] = &atkDb::setInstance($conn, $mockdb);
    }

    function restoreDb($conn=NULL)
    {
      if ($conn === NULL)
        $conn = atkDb::getDefault();      
      //$copy = $this->m_restoreDb[$conn];
      atkDb::setInstance($conn, $this->m_restoreDb[$conn]);
      //$this->m_restoreDb[$conn]="";
    }

    function &setMockNode($nodename, &$mocknode)
    {
      $this->m_restoreNode[$nodename] = &atkSetNode($nodename, $mocknode);
    }

    function restoreNode($nodename)
    {
      atkSetNode($nodename, $this->m_restoreNode[$nodename]);
    }

    function &setMockSecurityManager(&$mockmanager)
    {
      $this->m_restoreSecMgr = &atkSetSecurityManager($mockmanager);
    }

    function restoreSecurityManager()
    {
      atkSetSecurityManager($this->m_restoreSecMgr);
    }
  }

?>

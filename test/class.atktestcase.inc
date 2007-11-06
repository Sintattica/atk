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
    function atkTestCase($label = false)
    {
      $this->UnitTestCase($label);
    }

    function getLabel()
    {
      $label = parent::getLabel();
      return empty($label) ? get_class($this) : $label;
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
        $this->setUseTestDatabase(atkTestSuite::useTestDatabase());
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
      // don't let test-cases influence each other, so make sure
      // the node repository etc. is empty when we start a new test-case
      global $g_nodeRepository, $g_moduleRepository, $g_nodeHandlers, $g_nodeListeners, $g_nodeControllers;
      $g_nodeRepository = array();
      $g_moduleRepository = array();
      $g_nodeHandlers = array();
      $g_nodeListeners = array();
      $g_nodeControllers = array();

      if ($this->useTestDatabase())
      {
        // switch to the test databases
        $mapping = atkconfig("test_db_mapping");
        atkDb::useMapping($mapping);
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
        // stop using the test database(s)
        $this->_cleanData();
        atkDb::clearMapping();
      }
    }

    /**
     * Returns the fixture for the given table name with the given name.
     * If the fixture doesn't exist NULL is returned. If no database is specified
     * all databases will be searched. If no name is specified all fixtures for
     * the given table will be returned.
     *
     * @param string $table table name
     * @param string $name  fixture name
     *
     * @return array fixture data
     */
    function fixture($table, $name=NULL, $database=NULL)
    {
      if ($database != NULL && $name == NULL && isset($this->m_fixtureData[$database][$table]))
      {
        return $this->m_fixtureData[$database][$table];
      }
      else if ($database != NULL && $name != NULL && isset($this->m_fixtureData[$database][$table][$name]))
      {
        return $this->m_fixtureData[$database][$table][$name];
      }
      else if ($database == NULL)
      {
        foreach (array_keys($this->m_fixtureData) as $database)
        {
          $data = $this->fixture($table, $name, $database);
          if ($data != NULL) return $data;
        }
      }

      return NULL;
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
      foreach (array_keys($this->m_fixtureData) as $database)
      {
        if (isset($this->m_fixtureData[$database][$method]))
          return $this->fixture($method, isset($args[0]) ? $args[0] : NULL, isset($args[1]) ? $args[1] : NULL);
      }

      atkerror("Invalid method name $method for ".get_class($this)." (fixture not loaded?)!");
      return NULL;
    }

    /**
     * Clean-up data in the test database so we have a nice clean
     * database for the next test method.
     */
    function _cleanData()
    {
      atkdebug("Clean test database(s)...");

      $config = atkconfig("db");
      $mapping = atkconfig("test_db_mapping");

      foreach ($mapping as $normal => $test)
      {
      	$db = &$this->_getTestDb($normal);
        if (!isset($config[$test]["transactional_fixtures"]) ||
            !$config[$test]["transactional_fixtures"])
        {
          $db->toggleForeignKeys(false);

          $db->deleteAll();
          $db->commit();

          $db->toggleForeignKeys(true);
        }
        else
        {
          $db->rollback();
        }
      }
    }

    /**
     * Load fixture data into the test database.
     */
    function _loadData()
    {
      atkdebug("Load fixtures into test database(s)...");

      if (count($this->m_fixtures) == 0) return;

      $config = atkconfig("db");      
      $mapping = atkconfig("test_db_mapping");      
      
      atkimport('atk.fixture.atkfixturemanager');
      $manager = &atkFixtureManager::getInstance();

      $this->m_fixtureData = array();
      foreach($this->m_fixtures as $database => $fixtures)
      {
        $db = &$this->_getTestDb($database);

        if (!isset($config[$mapping[$database]]["transactional_fixtures"]) ||
            !$config[$mapping[$database]]["transactional_fixtures"])
        {
          $db->toggleForeignKeys(false);
        }

        foreach ($fixtures as $fullname)
        {
          $result = $manager->load($fullname, $db, $this->_getPath());
          if ($result === false)
          {
            atkerror("Error loading fixture '$fullname'!");
          }
          else
          {
            $table = $result['table'];
            $data = $result['data'];

            if (!isset($this->m_fixtureData[$database][$table]))
              $this->m_fixtureData[$database][$table] = array();

            $this->m_fixtureData[$database][$table] =
              array_merge($this->m_fixtureData[$database][$table], $data);
          }
        }

        if (!isset($config[$mapping[$database]]["transactional_fixtures"]) ||
            !$config[$mapping[$database]]["transactional_fixtures"])
        {
          $db->toggleForeignKeys(true);
          $db->commit();
        }
      }
    }

    /**
     * Get path for this test-case.
     *
     * @return returns the path for this test-case
     */
    function _getPath()
    {
      static $s_path = array();
      $class = get_class($this);

      if (!isset($s_path[$class]))
      {
        $includes = get_included_files();
        $name = "class.{$class}.inc";
        $nameLength = strlen("class.{$class}.inc");
        foreach ($includes as $file)
        {
          if (strcasecmp($name, substr($file, -$nameLength)) == 0)
          {
            $s_path[$class] = substr($file, 0, -$nameLength);
            break;
          }
        }
      }

      if (!isset($s_path[$class]))
        atkerror('Could not determine path for test-case: '.$class);
      else return $s_path[$class];
    }

    /**
     * Get test database.
     */
    function &_getTestDb($database)
    {
      return atkGetDb($database);
    }

    /**
     * Adds the given fixture to the given database.
     *
     * @param string $name     fixture name
     * @param string $database database name
     */
    function addFixture($name, $database="default")
    {
      $this->addFixtures(array($name), $database);
    }

    /**
     * Adds the given fixtures to the given database.
     *
     * @param array $names     fixture names
     * @param string $database database name
     */
    function addFixtures($names, $database="default")
    {
      if (!isset($this->m_fixtures[$database]))
        $this->m_fixtures[$database] = array();
      $this->m_fixtures[$database] = array_unique(array_merge($this->m_fixtures[$database], $names));
    }

    function setMockDb($conn=NULL)
    {
      global $config_db;

      if ($conn === NULL)
        $conn = "default";

      $config_db["mock"]["driver"] = "mock";

      $mockdb = &atkGetDb("mock");
      $this->m_restoreDb[$conn] = &atkDb::setInstance($conn, $mockdb);
    }

    function restoreDb($conn=NULL)
    {
      if ($conn === NULL)
        $conn = "default";
      atkDb::setInstance($conn, $this->m_restoreDb[$conn]);
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
    
    /**
     * Asserts the given attribute(s) value did not cause the given validation error.
     *
     * $attribName can contain the name of a single attributename, or an array with
     * attributenames.
     * 
     * @param array  $record
     * @param mixed $attribName
     * @param string $error
     */
    function _hasValidationError($record, $attribName, $error)
    {
      if (!isset($record['atkerror'])) return false;
      
      $errors = $record['atkerror'];

      $found = false;
      
      foreach ($errors as $entry)
      {
        //does the error match?
        if($entry['err'] == $error)
        {
          //If both $attribName and $entry["attrib_name"] are arrays, we could have a match.
          if(is_array($attribName) && is_array($entry["attrib_name"]))
          {
            //if the number of elements is not the same, we do not have a match.
            if(count($attribName)==count($entry["attrib_name"]))
            {
              //check if the attributes in the arrays are the same
              $allIn = true;
              foreach($attribName as $att)
              {
                if(!in_array($att,$entry["attrib_name"]))
                {
                  $allIn=false;
                  break;
                }
              }
            
              //if the arrays are the same, we have found a match
              if($allIn)
              {
                $found = true;
                break;
              }
            }
          }
          //If neither is an array, we could have a match.
          elseif(!is_array($attribName) && !is_array($error["attrib_name"]))
          {
            //If the names are the same, we have a match.
            if($entry['attrib_name'] == $attribName)
            {
              $found = true;
              break;
            }
          }          
        }
      }     
      
      return $found;      
    }    
    
    /**
     * Asserts the given attribute(s) value did not cause the given validation error.
     *
     * $attribName can contain the name of a single attributename, or an array with
     * attributenames.
     * 
     * @param array  $record
     * @param mixed $attribName
     * @param string $error
     */    
    function assertNoValidationError($record, $attribName, $error)
    {   
      $found = $this->_hasValidationError($record, $attribName, $error);
      if(is_array($attribName))
        $this->assertTrue(!$found, 'Validation error '.$error.' not found for attributes '.implode(", ",$attribName));
      else
        $this->assertTrue(!$found, 'Validation error '.$error.' not found for attribute '.$attribName);          
    }
    
    /**
     * Asserts the given attribute(s) value caused the given validation error.
     * 
     * $attribName can contain the name of a single attributename, or an array with
     * attributenames.
     *
     * @param array  $record
     * @param mixed $attribName
     * @param string $error
     */
    function assertValidationError($record, $attribName, $error)
    {
      $found = $this->_hasValidationError($record, $attribName, $error);
      if(is_array($attribName))
        $this->assertTrue($found, 'Validation error '.$error.' for attributes '.implode(", ",$attribName));        
      else 
        $this->assertTrue($found, 'Validation error '.$error.' for attribute '.$attribName);        
    }
    
    /**
     * Asserts if a certain attribute in the given node has the given flag.
     * You should pass the flag name to this method!
     *
     * @param atkNodeType $node
     * @param string $attribName
     * @param string $flagName
     */
    function assertAttributeHasFlag($node, $attribName, $flagName)
    {
      $flag = eval("return ".$flagName.";");
      $hasFlag = $node->getAttribute($attribName)->hasFlag($flag);
      $this->assertTrue($hasFlag, "Attribute ".$node->atkNodeType()."::$attribName has flag $flagName");
    }

    /**
     * Asserts if a certain attribute in the given node doesn't have the given flag.
     * You should pass the flag name to this method!
     *
     * @param atkNodeType $node
     * @param string $attribName
     * @param string $flagName
     */
    function assertAttributeNotHasFlag($node, $attribName, $flagName)
    {
      $flag = eval("return ".$flagName.";");
      $hasFlag = $node->getAttribute($attribName)->hasFlag($flag);
      $this->assertFalse($hasFlag, "Attribute ".$node->atkNodeType()."::$attribName not has flag $flagName");
    }    
  }
?>
<?php

  /**
   * This file is part of the Achievo ATK distribution.
   * Detailed copyright and licensing information can be found
   * in the doc/COPYRIGHT and doc/LICENSE files which should be 
   * included in the distribution.
   *
   * @package atk
   * @subpackage db
   * @access private
   *
   * @copyright (c)2005 Ibuildings.nl BV
   * @license http://www.achievo.org/atk/licensing ATK Open Source License
   *
   * @version $Revision$
   * $Id$
   */

  /**
   * Include the class we are about to test.
   */
  atkimport("atk.db.atkdb");
  atkimport("atk.db.atkmysqldb");  

  /**
   * Testcases for db abstraction
   *
   * @author Ivo Jansch <ivo@achievo.org>
   * @package atk
   * @subpackage db
   * @access private
   */
  class test_atkdb extends UnitTestCase
  {        
    /**
     * Setup the testcase.
     */
    function setup()
    {          
    }
    
    /**
     * Test method. 
     */
    function testVendorDateFormat()
    {  
      $db = new atkDb();
      // Check generic db date format
      $this->assertEqual($db->vendorDateFormat("Y-m-d H:i:s"), "YYYY-MM-DD HH24:MI:SS", "generic vendor dateformat check");
      // Check mysql db date format
      
      $mysqldb = new atkMysqlDb();
      $this->assertEqual($mysqldb->vendorDateFormat("Y-m-d H:i:s"), "%Y-%m-%d %H:%i:%s", "mysql dateformat check");
    }    
    
    function testQueryMode()
    {
      $db = null;
      $sql = '';
      $mode = '';
      $master = '';
      $slave = '';
      $cluster = null;
      $servers = null;
      
      $cluster = atkconfig('db_cluster');
      $servers = atkconfig('db');
      
      if (is_array($cluster) && count($cluster) > 0)
      {
        foreach ($cluster['default'] as $server)
        {
          if ($servers[$server]['mode'] == 'rw')
            $master = $server;
          else 
            $slave = $server;
        }
      }
      else 
      {
        $master = 'default';
        $slave = 'default';
      }
      
      $db = atkGetDb();
      
      $sql = "SELECT * FROM table WHERE conditie";
      $mode = $db->getQueryMode($sql);
      $this->assertEqual($mode, 'r', 'Slave db check - select at one line');
      $db->connect($mode); //switch db
      $this->assertEqual($db->m_connection, $slave, 'Slave db check - Right server selected?');
      $this->assertNotNull($db->m_link_id, 'Slave db check - Connected?');
      
      
      $sql = "SELECT
                *
              FROM
                table
              WHERE
                conditie";
      $mode = $db->getQueryMode($sql);
      $this->assertEqual($mode, 'r', 'Slave db check - select at multiple lines');
      $db->connect($mode); //switch db
      $this->assertEqual($db->m_connection, $slave, 'Slave db check - Right server selected?');
      $this->assertNotNull($db->m_link_id, 'Slave db check - Connected?');
      
      
      $sql = 'SHOW VARIABLES WHERE value > 0';
      $mode = $db->getQueryMode($sql);
      $this->assertEqual($mode, 'r', 'Slave db check - show at one line');
      $db->connect($mode); //switch db
      $this->assertEqual($db->m_connection, $slave, 'Slave db check - Right server selected?');
      $this->assertNotNull($db->m_link_id, 'Slave db check - Connected?');
      
      
      $sql = 'SHOW
                VARIABLES 
              WHERE 
                value > 0';
      $mode = $db->getQueryMode($sql);
      $this->assertEqual($mode, 'r', 'Slave db check - show at multiple lines');
      $db->connect($mode); //switch db
      $this->assertEqual($db->m_connection, $slave, 'Slave db check - Right server selected?');
      $this->assertNotNull($db->m_link_id, 'Slave db check - Connected?');
      
      
      $sql = "INSERT INTO table (kolom1, kolom2) VALUES (value1, value2)";
      $mode = $db->getQueryMode($sql);
      $this->assertEqual($mode, 'w', 'Master db check - insert into at one line');
      $db->connect($mode); //switch db
      $this->assertEqual($db->m_connection, $master, 'Master db check - Right server selected?');
      $this->assertNotNull($db->m_link_id, 'Master db check - Connected?');
      
      
      $sql = "INSERT INTO 
                table (kolom1, kolom2) 
              VALUES 
                (value1, value2)";
      $mode = $db->getQueryMode($sql);
      $this->assertEqual($mode, 'w', 'Master db check - insert into at multiple lines');
      $db->connect($mode); //switch db
      $this->assertEqual($db->m_connection, $master, 'Master db check - Right server selected?');
      $this->assertNotNull($db->m_link_id, 'Master db check - Connected?');
      
      
      $sql = "UPDATE table SET age='22' WHERE age='21'";
      $mode = $db->getQueryMode($sql);
      $this->assertEqual($mode, 'w', 'Master db check - update at one line');
      $db->connect($mode); //switch db
      $this->assertEqual($db->m_connection, $master, 'Master db check - Right server selected?');
      $this->assertNotNull($db->m_link_id, 'Master db check - Connected?');
      
      
      $sql = "UPDATE 
                table 
              SET 
                age='22' 
              WHERE 
                age='21'";
      $mode = $db->getQueryMode($sql);
      $this->assertEqual($mode, 'w', 'Master db check - update at multiple lines');
      $db->connect($mode); //switch db
      $this->assertEqual($db->m_connection, $master, 'Master db check - Right server selected?');
      $this->assertNotNull($db->m_link_id, 'Master db check - Connected?');
      
      
      $sql = "DELETE table WHERE age='21'";
      $mode = $db->getQueryMode($sql);
      $this->assertEqual($mode, 'w', 'Master db check - delete at one line');
      $db->connect($mode); //switch db
      $this->assertEqual($db->m_connection, $master, 'Master db check - Right server selected?');
      $this->assertNotNull($db->m_link_id, 'Master db check - Connected?');
      
      
      $sql = "DELETE 
                table 
              WHERE 
                age='21'";
      $mode = $db->getQueryMode($sql);
      $this->assertEqual($mode, 'w', 'Master db check - delete at multiple lines');
      $db->connect($mode); //switch db
      $this->assertEqual($db->m_connection, $master, 'Master db check - Right server selected?');
      $this->assertNotNull($db->m_link_id, 'Master db check - Connected?');
      
      
      $sql = "SELECT INTO table (kolom1, kolom2) VALUES (value1, value2)";
      $mode = $db->getQueryMode($sql);
      $this->assertEqual($mode, 'w', 'Master db check - select into at one line');
      $db->connect($mode); //switch db
      $this->assertEqual($db->m_connection, $master, 'Master db check - Right server selected?');
      $this->assertNotNull($db->m_link_id, 'Master db check - Connected?');
      
      
      $sql = "SELECT INTO 
                table (kolom1, kolom2) 
              VALUES 
                (value1, value2)";
      $mode = $db->getQueryMode($sql);
      $this->assertEqual($mode, 'w', 'Master db check - select into at multiple lines');
      $db->connect($mode); //switch db
      $this->assertEqual($db->m_connection, $master, 'Master db check - Right server selected?');
      $this->assertNotNull($db->m_link_id, 'Master db check - Connected?');
      
            
      $sql = "SELECT 
              INTO 
                table (kolom1, kolom2) 
              VALUES 
                (value1, value2)";
      $mode = $db->getQueryMode($sql);
      $this->assertEqual($mode, 'w', 'Master db check - select into at multiple lines');
      $db->connect($mode); //switch db
      $this->assertEqual($db->m_connection, $master, 'Master db check - Right server selected?');
      $this->assertNotNull($db->m_link_id, 'Master db check - Connected?');
    }
  }

?>

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
    var $m_org_config_db;
    var $m_org_config_db_cluster;

    var $m_master  = 'test_atkdb_master';
    var $m_slave   = 'test_atkdb_slave';
    var $m_cluster = 'test_atkdb_cluster';

    /**
     * Setup the testcase.
     */
    function setup()
    {
      // Whatever we do have a cluster or not we just try to search for a database
      // which we can use for a cluster. We create a master and a slave which connect
      // both to the same database, but we aren't really performing any queries, so this
      // should not be a problem.
      global $config_db, $config_db_cluster;

      $this->m_org_config_db = $config_db;
      $this->m_org_config_db_cluster = $config_db_cluster;

      $clone = NULL;
      if (isset($config_db_cluster['test']) && (atkGetDb($config_db_cluster['test'][0])->getDbStatus() == DB_SUCCESS))
        $clone = $config_db_cluster['test'][0];
      else if (isset($config_db['test']) && (atkGetDb('test')->getDbStatus() == DB_SUCCESS))
        $clone = 'test';
      else if (isset($config_db_cluster['default']) && (atkGetDb($config_db_cluster['default'][0])->getDbStatus() == DB_SUCCESS))
        $clone = $config_db_cluster['default'][0];
      else if (isset($config_db['default']) && (atkGetDb('default')->getDbStatus() == DB_SUCCESS))
        $clone = 'default';
      else
      {
        // this will probably never happen
        echo "Can't test cluster properties of atkDb. No valid database configuration found.";
        die;
      }

      $config_db[$this->m_master] = array_merge($config_db[$clone], array("mode" => "rw"));
      $config_db[$this->m_slave] = array_merge($config_db[$clone], array("mode" => "r"));
      $config_db_cluster[$this->m_cluster] = array($this->m_master, $this->m_slave);
    }

    function teardown()
    {
      global $config_db, $config_db_cluster;
      $config_db = $this->m_org_config_db;
      $config_db_cluster = $this->m_org_config_db_cluster;
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

    function assertConnectionAndQueryMode(&$db, $query, $connection, $mode, $message)
    {
      $queryMode = $db->getQueryMode($query);
      $this->assertEqual($queryMode, $mode, $message.' (mode)');
      $db->connect($queryMode);
      $this->assertEqual($db->m_connection, $connection, $message.' (connection)');
      $this->assertNotNull($db->m_link_id, $message.' (connected?)');
    }

    function testQueryMode()
    {
      $db = &atkGetDb($this->m_cluster);

      // select
      $query = "SELECT * FROM table WHERE conditie";
      $this->assertConnectionAndQueryMode($db, $query, $this->m_slave, 'r', 'Slave DB check, select at one line');

      $query = "  SELECT * FROM table WHERE conditie  ";
      $this->assertConnectionAndQueryMode($db, $query, $this->m_slave, 'r', 'Slave DB check, select at one line with spaces');

      $query = "SELECT
                  *
                FROM
                  table
                WHERE
                 conditie";
      $this->assertConnectionAndQueryMode($db, $query, $this->m_slave, 'r', 'Slave DB check, select on multiple lines');

      $query = "
        SELECT
          *
        FROM
          table
        WHERE
          conditie
      ";
      $this->assertConnectionAndQueryMode($db, $query, $this->m_slave, 'r', 'Slave DB check, select on multiple lines with spaces');

      $oldLinkId = $db->m_link_id;

      // insert
      $query = "INSERT INTO table (kolom1, kolom2) VALUES (value1, value2)";
      $this->assertConnectionAndQueryMode($db, $query, $this->m_master, 'w', 'Master DB check, insert at one line');

      $newLinkId = $db->m_link_id;

      $this->assertTrue(($oldLinkId !== $newLinkId), "Check if we are really connected to a different database \$oldLinkId !== \$newLinkId");

      $query = "     INSERT INTO table (kolom1, kolom2) VALUES (value1, value2)     ";
      $this->assertConnectionAndQueryMode($db, $query, $this->m_master, 'w', 'Master DB check, insert at one line with spaces');

      $query = "INSERT
                  INTO
                  table (kolom1, kolom2)
                VALUES
                 (value1, value2)";
      $this->assertConnectionAndQueryMode($db, $query, $this->m_master, 'w', 'Master DB check, insert at multiple lines');


      $query = "
               INSERT
                  INTO
                  table (kolom1, kolom2)
                VALUES
                 (value1, value2)
            ";
      $this->assertConnectionAndQueryMode($db, $query, $this->m_master, 'w', 'Master DB check, insert at multiple lines with spaces');

      // update
      $query = "UPDATE table SET age='22' WHERE age='21'";
      $this->assertConnectionAndQueryMode($db, $query, $this->m_master, 'w', 'Master DB check, update at one line');

      $query = "   UPDATE table SET age='22' WHERE age='21'   ";
      $this->assertConnectionAndQueryMode($db, $query, $this->m_master, 'w', 'Master DB check, update at one line with spaces');

      $sql = "UPDATE
                table
              SET
                age='22'
              WHERE
                age='21'";
      $this->assertConnectionAndQueryMode($db, $query, $this->m_master, 'w', 'Master DB check, update at multiple lines');

      $sql = "
             UPDATE
                table
              SET
                age='22'
              WHERE
                age='21'
            ";
      $this->assertConnectionAndQueryMode($db, $query, $this->m_master, 'w', 'Master DB check, update at multiple lines with spaces');

      // show
      $query = "SHOW VARIABLES WHERE value > 0";
      $this->assertConnectionAndQueryMode($db, $query, $this->m_master, 'r', 'Master DB check (although mode r), show at one line');

      $query = "   SHOW VARIABLES WHERE value > 0  ";
      $this->assertConnectionAndQueryMode($db, $query, $this->m_master, 'r', 'Master DB check (although mode r), show at one line with spaces');

      $query = "SHOW
                  VARIABLES
                WHERE
                  value > 0";
      $this->assertConnectionAndQueryMode($db, $query, $this->m_master, 'r', 'Master DB check (although mode r), show at multiple lines');

      $query = "
                SHOW
                  VARIABLES
                WHERE
                  value > 0
               ";
      $this->assertConnectionAndQueryMode($db, $query, $this->m_master, 'r', 'Master DB check (although mode r), show at multiple lines with spaces');

      // delete
      $query = "DELETE table WHERE age='21'";
      $this->assertConnectionAndQueryMode($db, $query, $this->m_master, 'w', 'Master DB check, delete at one line');

      $query = "    DELETE table WHERE age='21'  ";
      $this->assertConnectionAndQueryMode($db, $query, $this->m_master, 'w', 'Master DB check, delete at one line with spaces');

      $query = "DELETE
                  table
                WHERE
                  age='21'";
      $this->assertConnectionAndQueryMode($db, $query, $this->m_master, 'w', 'Master DB check, delete at multiple lines');

      $query = "
               DELETE
                  table
                WHERE
                  age='21'
               ";
      $this->assertConnectionAndQueryMode($db, $query, $this->m_master, 'w', 'Master DB check, delete at multiple lines with spaces');

      // select into
      $query = "SELECT INTO table (kolom1, kolom2) VALUES (value1, value2)";
      $this->assertConnectionAndQueryMode($db, $query, $this->m_master, 'w', 'Master DB check, select into at one line');

      $query = "  SELECT INTO table (kolom1, kolom2) VALUES (value1, value2)   ";
      $this->assertConnectionAndQueryMode($db, $query, $this->m_master, 'w', 'Master DB check, select into at one line with spaces');

      $query = "SELECT
               INTO table
                (kolom1, kolom2)
                VALUES
                  (value1, value2)";
      $this->assertConnectionAndQueryMode($db, $query, $this->m_master, 'w', 'Master DB check, select into at multiple lines');

      $query = "
          SELECT
               INTO table
                (kolom1, kolom2)
                VALUES
                  (value1, value2)
           ";
      $this->assertConnectionAndQueryMode($db, $query, $this->m_master, 'w', 'Master DB check, select into at multiple lines with spaces');

      // alter table
      $query = "ALTER TABLE x ADD COLUMN y INTEGER NOT NULL";
      $this->assertConnectionAndQueryMode($db, $query, $this->m_master, 'w', 'Master DB check, alter table one line');

      $query = "  ALTER TABLE x ADD COLUMN y INTEGER NOT NULL   ";
      $this->assertConnectionAndQueryMode($db, $query, $this->m_master, 'w', 'Master DB check, alter table at one line with spaces');

      $query = "ALTER
                 TABLE x
                ADD COLUMN
                 y INTEGER NOT NULL";
      $this->assertConnectionAndQueryMode($db, $query, $this->m_master, 'w', 'Master DB check, alter table at multiple lines');

      $query = "
        ALTER TABLE x
        ADD COLUMN y INTEGER
        NOT NULL
       ";
      $this->assertConnectionAndQueryMode($db, $query, $this->m_master, 'w', 'Master DB check, alter table at multiple lines with spaces');

      // create table
      $query = "CREATE TABLE x ( x INTEGER PRIMARY KEY )";
      $this->assertConnectionAndQueryMode($db, $query, $this->m_master, 'w', 'Master DB check, create table one line');

      $query = "   CREATE TABLE x ( x INTEGER PRIMARY KEY )  ";
      $this->assertConnectionAndQueryMode($db, $query, $this->m_master, 'w', 'Master DB check, create table at one line with spaces');

      $query = "CREATE
                  TABLE x
                (
                  x INTEGER PRIMARY KEY
                )";
      $this->assertConnectionAndQueryMode($db, $query, $this->m_master, 'w', 'Master DB check, create table at multiple lines');

      $query = "
        CREATE TABLE x
        (
          x INTEGER PRIMARY KEY
        )
      ";
      $this->assertConnectionAndQueryMode($db, $query, $this->m_master, 'w', 'Master DB check, create table at multiple lines with spaces');
    }
  }
?>
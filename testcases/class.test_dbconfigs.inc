<?php
  /**
   * This file is part of the Achievo ATK distribution.
   * Detailed copyright and licensing information can be found
   * in the doc/COPYRIGHT and doc/LICENSE files which should be
   * included in the distribution.
   *
   * @package atk
   * @access private
   *
   * @copyright (c)2005 Ibuildings
   * @license http://www.achievo.org/atk/licensing ATK Open Source License
   */

  /**
   * Testcases for several configs.
   *
   * @author René van den Ouden <rene@ibuildings.nl>
   * @package atk
   * @access private
   */
   
   class Test_DbConfigs extends atkTestCase
   {
    /**
     * Test database settings
     * TODO: works only for mysql dbs now.  
     *       add support for other db drivers(oracle!).
     */
    function test_Privileges()
    {
      $dbs = atkconfig("db");
      $dbnamess = array_keys($dbs);
      foreach($dbnamess as $dbname)
      {
        $db = &atkGetDb($dbname);
        $dbdriver = $dbs["$dbname"]["driver"];
        $grants = "";
        
        /**
         * @todo return Oracle grants from current user
         */
//        if(ereg("oci",$dbdriver)) $grants = $this->getOracleGrants($db, $dbname);
        if(ereg("oci",$dbdriver)) $grants = NULL;
        elseif(ereg("mysql",$dbdriver)) $grants = $this->getMysqlGrants($db, $dbname);
        elseif(ereg("pgsql", $dbdriver)) $grants = NULL;
        else return;
        
        if($grants == NULL) return;
        
        $this->assertWantedPattern("/SELECT,|SELECT ON/", $grants, "SELECT is granted on $dbname");
        if($db->hasMode($db, "w") || $db->hasMode($db, "rw"))
        {
          $this->assertWantedPattern("/UPDATE,|UPDATE ON/", $grants, "UPDATE is granted on $dbname");
          $this->assertWantedPattern("/DELETE,|DELETE ON/", $grants, "DELETE is granted on $dbname");;
          $this->assertWantedPattern("/INSERT,|DROP ON/", $grants, "INSERT is granted on $dbname");
        }
      }
    }
    
    /**
     * Return grants for current user on current mysql db
     */
    function getMysqlGrants($db, $dbname)
    {
      if($this->assertTrue(extension_loaded("mysql"), "Mysql extension loaded"))
      {
        if(!$this->assertTrue(@mysql_connect($db->m_host, $db->m_user, $db->m_password, true),
           "Connect Mysql database")) return NULL; 
        else
        {
          $rows = $db->getrows("SHOW GRANTS FOR ".$db->m_user."@".$db->m_host);
          $specificprivilegefound = false;
          if(count($rows)>1)//check for specific grants
          {
            foreach($rows as $privileges)
            {
              $privileges = stripslashes(array_shift($privileges));
              if(ereg("`".$db->m_database."`", $privileges))//if there are specific privileges for current database
              {
                $grants = $privileges;
                $specificprivilegefound = true;
                break;
              }
            }
          }
          if($specificprivilegefound == false)
          {
            $grants = stripslashes(array_shift($rows["0"]));
          }
          if(ereg("ALL PRIVILEGES", $grants))//because grants isn't a list of all privileges now
          {
            $grants = "SELECT," .
                      "UPDATE," .
                      "DELETE," .
                      "INSERT,";
          }
          return $grants;
        } 
      }
      else return NULL;
    }
    
    /**
     * Return grants for current user on current oracle db
     * @todo get grants for current user
     */
    function getOracleGrants($db, $dbname)
    {
      if($this->assertTrue(extension_loaded("oracle"), "Oracle extension loaded"))
      {
        if(!$this->assertTrue($db->connect(), "Oracle database connected")) return NULL;  
        else
        {
          $rows = $db->getrows("SELECT * FROM session_privs");
          foreach($rows as $privileges)
          {
            $privileges = stripslashes(array_shift($privileges));
            $grants .= $privileges;
          }
          return $grants;
        }
      }
      else return NULL;
    }
  }
?>

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
    	$extensions = get_loaded_extensions(); //loaded php extensions on the server
      $dbs = atkconfig("db");
      $dbnamess = array_keys($dbs);
      foreach($dbnamess as $dbname)
      {
        $db = &atkGetDb($dbname);
        
        $dbdriver = $dbs["$dbname"]["driver"];
        $grants = "";
        
        if(ereg("oci",$dbdriver)) $grants = $this->getOracleGrants($db, $dbname);
        elseif(ereg("mysql",$dbdriver)) $grants = $this->getMysqlGrants($db, $dbname);
        elseif(ereg("pgsql", $dbdriver)) $grants = NULL;
        else 
        {
          echo "*  unknown database driver for database: $dbname<br>";
          return;
        }
        
        if($grants == NULL) continue;
        
        if(!$db->hasMode($db, "w") && $db->hasMode($db, "r")) //readonly
        {
          $this->assertWantedPattern("/SELECT/", $grants, "SELECT is granted on $dbname");
        }
        else
        {
          $this->assertWantedPattern("/SELECT,|SELECT ON/", $grants, "SELECT is granted on $dbname");
          $this->assertWantedPattern("/UPDATE,|UPDATE ON/", $grants, "UPDATE is granted on $dbname");
          $this->assertWantedPattern("/DELETE,|DELETE ON/", $grants, "DELETE is granted on $dbname");;
          $this->assertWantedPattern("/CREATE,|CREATE ON/", $grants, "CREATE is granted on $dbname");
          $this->assertWantedPattern("/DROP,|DROP ON/", $grants, "DROP is granted on $dbname");
          $this->assertWantedPattern("/INSERT,|DROP ON/", $grants, "INSERT is granted on $dbname");
          $this->assertWantedPattern("/ALTER,|ALTER ON/", $grants, "ALTER is granted on $dbname");
        }
      }
    }
    
    /**
     * Return grants for current user on current mysql db
     */
    function getMysqlGrants($db, $dbname)
    {
      if(extension_loaded("mysql"))
      {
        if(!(@mysql_connect($db->m_host, $db->m_user, $db->m_password, true)))
        {
          echo "*  can't connect with configured database: $dbname, check config.inc.php<br>";
          return NULL; 
        } 
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
                      "CREATE," .
                      "DROP," .
                      "INSERT," .
                      "ALTER,";
          }
          return $grants;
        } 
      }
      else
      {
        echo "*  no mysql extension loaded on the server, so can't test database: $dbname<br>";
        return NULL;
      }
    }
    
    /**
     * Return grants for current user on current oracle db
     */
    function getOracleGrants($db, $dbname)
    {
      if(extension_loaded("oracle"))
      {
        if(!(@ocilogon($db->m_user, $db->m_password, $db->m_host)))
        {
          echo "*  can't connect with configured database: $dbname, check config.inc.php<br>";
          return NULL;  
        } 
        else
        {
          $rows = $db->getrows("SELECT * FROM session_privs");
          foreach($rows as $privileges)
          {
            //TODO: get privileges..
            $privileges = stripslashes(array_shift($privileges));
            $grants .= $privileges;
          }
          return $grants;
        }
      }
      else
      {
      	echo "*  no oracle extension loaded on the server, so can't test database: $dbname<br>";
        return NULL;
      }
    }
  }
?>

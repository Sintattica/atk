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
   
   class Test_Config extends atkTestCase
   {
   	function test_Config()
    {
    	
    }
    
    function setUp()
    {
    	parent::setUp();
    }
    
    /**
     * Test database settings
     */
    function test_Privileges()
    {
    	$db = &atkGetDb();
      $rows = $db->getrows("SHOW GRANTS FOR CURRENT_USER");
      $grants = "";
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
      if(!$db->hasMode($db, "w") && $db->hasMode($db, "r")) //readonly
      {
        $this->assertWantedPattern("/SELECT/", $grants, "SELECT is granted");
      }
      else
      {
      	$this->assertWantedPattern("/SELECT,|SELECT ON/", $grants, "SELECT is granted");
        $this->assertWantedPattern("/UPDATE,|UPDATE ON/", $grants, "UPDATE is granted");
        $this->assertWantedPattern("/DELETE,|DELETE ON/", $grants, "DELETE is granted");
        $this->assertWantedPattern("/CREATE,|CREATE ON/", $grants, "CREATE is granted");
        $this->assertWantedPattern("/DROP,|DROP ON/", $grants, "DROP is granted");
        $this->assertWantedPattern("/INSERT,|DROP ON/", $grants, "INSERT is granted");
        $this->assertWantedPattern("/ALTER,|ALTER ON/", $grants, "ALTER is granted");
      }
    }
    
    function tearDown()
    {
    	parent::tearDown();
    }
   }
?>

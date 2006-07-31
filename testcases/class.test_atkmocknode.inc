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
   *
   * @version $Revision$
   * $Id$
   */

  /**
   * Testcases for class atkMockNode.
   *
   * @author Ivo Jansch <ivo@achievo.org>
   * @package atk
   * @access private
   */
  class test_atkMockNode extends atkTestCase
  {
    function test_mocknode()
    {      
      atkimport("atk.atkmocknode");
      $mock = &new atkMockNode("test");
      
      // First test if original result is retrieved when no
      // result is set.
      $result = $mock->getTabs("edit");    
      $this->assertTrue((is_array($result)&&count($result)==0), "default getTabs should empty array");
      
      // Now test mocking the call
      $mock->setResult("getTabs", array("apple", "banana"));
      
      $result = $mock->getTabs("edit");            
      
      $this->assertTrue((is_array($result) && count($result)==2 && $result[1]=="banana"), "mocked getTabs should return apple and banana");
    }

    function test_mocknodegetTabs()
    {      
      atkimport("atk.atkmocknode");
      atkimport("atk.attributes.atkattribute");   
      
      $mock = &new atkMockNode("test");

      $mock->add(new atkAttribute("id", AF_AUTOKEY));
      $mock->add(new atkAttribute("default"));
      $mock->add(new atkAttribute("new",AF_HIDE_ADD),"new");

      $mock->setResult("checkTabRights", array("default"));

      $p_attib = &$mock->getAttribute("new");

      $flags = $p_attib->getFlags();
      $tabs = $mock->getTabs("add");
      $this->assertEqual($p_attib->getFlags(),$flags,"attributes flags has been change evently");
    }
    
  }

?>
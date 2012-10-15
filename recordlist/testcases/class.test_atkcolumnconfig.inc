<?php

  /**
   * This file is part of the Achievo ATK distribution.
   * Detailed copyright and licensing information can be found
   * in the doc/COPYRIGHT and doc/LICENSE files which should be
   * included in the distribution.
   *
   * @package atk
   * @subpackage recordlist
   * @access private
   *
   * @copyright (c)2006 Ibuildings
   * @license http://www.achievo.org/atk/licensing ATK Open Source License
   *
   * $Id$
   * @author Yury Golovnya  <ygolovnya@ccenter.utel.com.ua>
   * @version $Revision: 4362 $
   */


  /**
   * @internal includes
   */
  atkimport("atk.atknode");
  atkimport("atk.recordlist.atkcolumnconfig");

  /**
   * Utility node for the atlcolumnconfig testcases
   *
   * @author Yury Golovnya <ygolovnya@kyiv.utel.com.ua>
   * @package atk
   * @subpackage utils
   * @access private
   */
  class testNode extends atkNode
  {
    function testNode()
    {
      $this->atkNode("testNode");
      $this->add(new atkNumberAttribute("id", AF_AUTOKEY));
      $this->add(new atkAttribute("name", AF_AUTOKEY));

      $this->setTable("__test__");
    }
  }


  /**
   * Testcases for class atkColumnConfig
   *
   * @author Yury Golovnya <ygolovnya@kyiv.utel.com.ua>
   * @package atk
   * @subpackage utils
   * @access private
   */
  class test_atkcolumnconfig extends UnitTestCase
  {
    var $m_node = null;
    var $m_columnConfig = null;
    /**
     * Setup the testcase.
     */
    function setup()
    {
			$this->m_node = &new testNode();
			$this->m_columnConfig = &new atkColumnConfig();
    }

    /**
     * Test method.
     */
    function test_setNode()
    {
      $this->m_columnConfig->setNode($this->m_node);
      $this->assertReference($this->m_node, $this->m_columnConfig->m_node);
    }
    
    protected function assertOrder($order)
    {
      $columnConfig = $this->m_node->getColumnConfig(null, true);      
      $this->assertEqual($order, $columnConfig->getOrderbyStatement());
    }

    function test_getOrderByStatement()
    {
      $this->m_node->setOrder('name');
      $this->assertOrder("LOWER(__test__.name) ASC");
      
    	$this->m_node->setOrder('name DESC');
      $this->assertOrder("LOWER(__test__.name) DESC");
      
      $this->m_node->setOrder('__test__.name asc');
      $this->assertOrder("LOWER(__test__.name) ASC");
      
      $this->m_node->setOrder('__test__.name desc');
      $this->assertOrder("LOWER(__test__.name) DESC");
      
      $this->m_node->setOrder('id ASC');
      $this->assertOrder("__test__.id ASC");
      
      $this->m_node->setOrder('id DESC');
      $this->assertOrder("__test__.id DESC");
      
      $this->m_node->setOrder('id, name');
      $this->assertOrder("__test__.id ASC, LOWER(__test__.name) ASC");
      
      $this->m_node->setOrder('id DESC, name  DESC');
      $this->assertOrder("__test__.id DESC, LOWER(__test__.name) DESC");
    }
  }
?>
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
   * @version $Revision$
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
      $this->add(new atkAttribute("id", AF_AUTOKEY));

      $this->setOrder("id DESC");
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

    function test_getOrderByStatement()
    {
      $columnConfig = &$this->m_node->getColumnConfig();
      $this->assertEqual("__test__.id DESC",$columnConfig->getOrderbyStatement());
    }
  }
?>

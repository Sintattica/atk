<?php

  /**
   * This file is part of the Achievo ATK distribution.
   * Detailed copyright and licensing information can be found
   * in the doc/COPYRIGHT and doc/LICENSE files which should be
   * included in the distribution.
   *
   * @package atk
   * @subpackage utils
   * @access private
   *
   * @copyright (c)2005 Ibuildings
   * @license http://www.achievo.org/atk/licensing ATK Open Source License
   *
   * @version $Revision: 4204 $
   * $Id$
   */


  /**
   * @internal includes
   */
  atkimport("atk.atknode");
  atkimport("atk.utils.atkactionlistener");

  /**
   * Utility node for the listener testcases
   *
   * @author Yury Golovnya <ygolovnya@kyiv.utel.com.ua>
   * @package atk
   * @subpackage utils
   * @access private
   */
  class listenerTest extends atkNode
  {
    function listenerTest()
    {
      $this->atkNode("listenerTest");
      $this->add(new atkAttribute("id", AF_AUTOKEY));
	    $this->addListener(atknew("atk.utils.atkeventlog"));

      $this->setTable("listenerTest");
    }
  }


  /**
   * Testcases for class atkActionListener
   *
   * @author Yury Golovnya <ygolovnya@kyiv.utel.com.ua>
   * @package atk
   * @subpackage utils
   * @access private
   */
  class test_atkactionlistener extends UnitTestCase
  {
    var $m_node = null;
    /**
     * Setup the testcase.
     */
    function setup()
    {
			$this->m_node = &new listenerTest();
    }

    /**
     * Test method.
     */
    function test_setNode()
    {
      $this->assertReference($this->m_node, $this->m_node->m_actionListeners[0]->m_node);
    }
  }
?>

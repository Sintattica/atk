<?php

  /**
   * This file is part of the Achievo ATK distribution.
   * Detailed copyright and licensing information can be found
   * in the doc/COPYRIGHT and doc/LICENSE files which should be
   * included in the distribution.
   *
   * @package atk
   * @subpackage attributes
   * @access private
   *
   * @copyright (c)2006 Ibuildings
   * @license http://www.achievo.org/atk/licensing ATK Open Source License
   *
   * $Id$
   * @author Yury Golovnya  <ygolovnya@achievo.org>
   * @version $Revision$
   */


  /**
   * @internal includes
   */
  atkimport("atk.attributes.atklistattribute");

  /**
   * Testcases for class atkListAttribute
   *
   * @author Yury Golovnya <ygolovnya@achievo.org>
   * @package atk
   * @subpackage utils
   * @access private
   */
  class test_atklistattribute extends UnitTestCase
  {
    var $m_attribute = null;
    /**
     * Setup the testcase.
     */
    function setup()
    {
    }

    /**
     * Test method.
     */
    function test_selectingValue()
    {
      $this->m_attribute = &new atkListAttribute("list",array(0,1,2),"",AF_OBLIGATORY);
      $record = array("list"=>0);
      $this->assertTrue(strpos($this->m_attribute->edit($record),'value="0" selected>'),"testing number 0 as list index when editing");
    }

  }
?>
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
   * @copyright (c)2006 Ivo Jansch
   * @license http://www.achievo.org/atk/licensing ATK Open Source License
   *
   * @version $Revision$
   * $Id$
   */

  /**
   * Utility node class for testing the atknode triggers
   * @access private
   */
  atkimport("atk.atknode");
  class triggerNode extends atkNode
  {
    var $m_counter = 0;

    function preAdd($record, $mode)  { $this->m_counter++; }
    function postAdd($record, $mode) { $this->m_counter--; }
    function preUpdate($record)      { $this->m_counter++; }
    function postUpdate($record)     { $this->m_counter--; }
    function preDelete($record)      { $this->m_counter++; }
    function postDelete($record)     { $this->m_counter--; }

  }

  /**
   * Testcases for class atkNode.
   *
   * @author Ivo Jansch <ivo@achievo.org>
   * @package atk
   * @access private
   */
  class test_triggers extends atkTestCase
  {
    var $m_node = NULL;

    function test_duplicatetriggers()
    {
      $node = &new triggerNode();
      $record = array("id"=>"dummy");

      // note that since we use a single counter, if one testcases fails, all may fail,
      // so in the case of error, first fix the first error and rerun the test, the rest
      // might be fixed as well.

      $node->executeTrigger("preAdd", $record);
      $this->assertEqual($node->m_counter, 1, "preAdd should be executed once");
      $node->executeTrigger("preAdd", $record);
      $this->assertEqual($node->m_counter, 1, "preAdd should not be executed twice");

      $node->executeTrigger("postAdd", $record);
      $this->assertEqual($node->m_counter, 0, "postAdd should be executed once");
      $node->executeTrigger("postAdd", $record);
      $this->assertEqual($node->m_counter, 0, "postAdd should not be executed twice");

      $node->executeTrigger("preUpdate", $record);
      $this->assertEqual($node->m_counter, 1, "preUpdate should be executed once");
      $node->executeTrigger("preUpdate", $record);
      $this->assertEqual($node->m_counter, 1, "preUpdate should not be executed twice");

      $node->executeTrigger("postUpdate", $record);
      $this->assertEqual($node->m_counter, 0, "postUpdate should be executed once");
      $node->executeTrigger("postUpdate", $record);
      $this->assertEqual($node->m_counter, 0, "postUpdate should not be executed twice");

      $node->executeTrigger("preDelete", $record);
      $this->assertEqual($node->m_counter, 1, "preDelete should be executed once");
      $node->executeTrigger("preDelete", $record);
      $this->assertEqual($node->m_counter, 1, "preDelete should not be executed twice");

      $node->executeTrigger("postDelete", $record);
      $this->assertEqual($node->m_counter, 0, "postDelete should be executed once");
      $node->executeTrigger("postDelete", $record);
      $this->assertEqual($node->m_counter, 0, "postDelete should not be executed twice");

    }

  }

?>
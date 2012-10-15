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
   * @copyright (c)2005 Ivo Jansch
   * @license http://www.achievo.org/atk/licensing ATK Open Source License
   *
   * @version $Revision: 5798 $
   * $Id$
   */

  /**
   * Include the class we are about to test.
   */
  atkimport("atk.utils.atkdirectorytraverser");
  
   /**
   * Utilityclass
   *
   * @author Ivo Jansch <ivo@achievo.org>
   * @package atk
   * @subpackage utils
   * @access private
   */
  class simplevisitor
  {
    var $m_files = array();
    var $m_dirs = array();
    
    function visitDir($pathname)
    {
      $this->m_dirs[] = $pathname;
    }
    
    function visitFile($pathname)
    {
      $this->m_files[] = $pathname;
    }
  }

   /**
   * Testcases for class atkdirectorytraverser.
   *
   * @author Ivo Jansch <ivo@achievo.org>
   * @package atk
   * @subpackage utils
   * @access private
   */
  class test_atkDirectoryTraverser extends UnitTestCase
  {
    var $m_visitor = null;
    var $m_traverser = null;
    
    function setup()
    {
      $this->m_visitor = &new simplevisitor();
      $this->m_traverser = &new atkDirectoryTraverser();
      $this->m_traverser->addCallbackObject($this->m_visitor);
    }
    
    function testTraverser()
    {
      $this->m_traverser->traverse(atkconfig("atkroot")."atk");
      
      // test if regular file was traversed
      $this->assertTrue(in_array(atkconfig("atkroot")."atk/class.atknode.inc", $this->m_visitor->m_files));
      
      // test if regular file in sub, sub dir was traverserd
      $this->assertTrue(in_array(atkconfig("atkroot")."atk/db/languages/mysql_en.lng", $this->m_visitor->m_files));
      
      // test if dirs are traversed
      $this->assertTrue(in_array(atkconfig("atkroot")."atk/security", $this->m_visitor->m_dirs));
      
    }
    
    
  }

?>
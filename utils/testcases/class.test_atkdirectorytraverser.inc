<?php

  atkimport("atk.utils.atkdirectorytraverser");
  
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
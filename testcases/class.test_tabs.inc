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
  
  atkimport("atk.atknode");
  
  /**
   * Tests the tabs      
   *
   * @access private
   * @author dennis <dennis@ibuildings.nl>
   */
  class test_tabs extends atkTestCase
  {
    function test_getTabsFromSections()
    {
      $sections = array();
      $sections[] = "tab1.section1";
      $sections[] = "tab2.section2";
      $sections[] = "tab1.section2";
      $sections[] = "tab3.section1";
      
      $node = new atkNode("testnode");
      $tabs = $node->getTabsFromSections($sections);      
      $this->assertEqual(3,count($tabs));
      $this->assertTrue(in_array("tab1",$tabs));
      $this->assertTrue(in_array("tab2",$tabs));
      $this->assertTrue(in_array("tab3",$tabs));      
      //todo test      
      
    }
    
    function test_getTabFromSection()
    {
      $node = new atkNode("testnode");
      
      $section = "tab1.section1";
      $tab = $node->getTabFromSection($section);
      $this->assertNotNull($tab);
      $this->assertEqual("tab1",$tab);
            
      $section = ".section1";
      $tab = $node->getTabFromSection($section);
      $this->assertNotNull($tab);
      $this->assertEqual("default",$tab);
      
      $section = ".";
      $tab = $node->getTabFromSection($section);
      $this->assertNotNull($tab);
      $this->assertEqual("default",$tab);
      
      $section = "";
      $tab = $node->getTabFromSection($section);
      $this->assertNotNull($tab);
      $this->assertEqual("default",$tab);           
    }
    
    function test_addAndRemoveFromNode()
    {
      $node = new atkNode("testnode");
      $attr1 = &$node->add(new atkAttribute('test1'),array('tab1', 'tab2'));
      $attr2 = &$node->add(new atkAttribute('test2'),'tab2');
      $attr3 = &$node->add(new atkAttribute('test3',AF_HIDE_EDIT),'tab3');
      
      //test tabs after calling the add function of atkNode.
      $this->assertEqual(3,count($node->m_tabList['add']));
      $this->assertEqual(2,count($node->m_tabList['edit']));
     
      //test call to addToEditArray()      
      $attr1 = $node->getAttribute('test1');
      
      $data = array();    $defaults = array();
      $errors = array();  $fieldprefix = "";
      $attr1->addToEditArray('add',$data,$defaults,$errors,$fieldprefix);      
      
      //The data field should not be null
      $this->assertNotNull($data);
            
      //the defaults and errors array should be empty.
      $this->assertEqual(0,count($defaults));
      $this->assertEqual(0,count($errors));      
      $fields = $data["fields"][0];
                
      $this->assertTrue(in_array('tab1',$node->getTabs('add')));
      $this->assertTrue(in_array('tab2',$node->getTabs('add')));
      $this->assertTrue(in_array('tab3',$node->getTabs('add')));
      $this->assertFalse(in_array('tab3',$node->getTabs('edit')));
      
      $this->assertTrue(in_array('tab1',$fields["attribute"]->m_tabs));
      $this->assertTrue(in_array('tab2',$fields["attribute"]->m_tabs));
      $this->assertFalse(in_array('tab3',$fields["attribute"]->m_tabs));
      
      $this->assertTrue($attr1->showOnTab('tab2'));
      $this->assertTrue($attr2->showOnTab('tab2'));
      $this->assertFalse($attr3->showOnTab('tab2'));
      
      $this->assertTrue(in_array('tab1',$fields["tabs"]));
      $this->assertTrue(in_array('tab2',$fields["tabs"]));
      $this->assertFalse(in_array('tab3',$fields["tabs"]));
      
      /**
      * Test removing an attribute from a node and checking if the tabs 
      * are removed accordingly.     
      */      
      $node->remove('test3');
      $this->assertTrue(in_array('tab1',$node->getTabs('add')));
      $this->assertTrue(in_array('tab2',$node->getTabs('add')));
      $this->assertFalse(in_array('tab3',$node->getTabs('add')));      
    }
    
    
    
    /**
     * Test if, when adding a section to the tabs doesn't break the
     * orginal tab functionality.
     */
    function test_addTabAndSectionToNode()
    {
      $node = new atkNode("testnode");
      $attr1 = &$node->add(new atkAttribute('test1'),array('tab1.section1', 'tab2.section1'));
      $attr2 = &$node->add(new atkAttribute('test2'),'tab2.section2');
      $attr3 = &$node->add(new atkAttribute('test3',AF_HIDE_EDIT),'tab3.section2');
      $attr4 = &$node->add(new atkAttribute('test4'));
      
      //test tabs after calling the add function of atkNode.
      $this->assertEqual(4,count($node->m_tabList['add']));
      $this->assertEqual(3,count($node->m_tabList['edit']));
     
      //test call to addToEditArray()
      $data = array();    $defaults = array();
      $errors = array();  $fieldprefix = "";
      $attr1->addToEditArray('add',$data,$defaults,$errors,$fieldprefix);      
      
      //The data field should not be null
      $this->assertNotNull($data);
            
      //the defaults and errors array should be empty.
      $this->assertEqual(0,count($defaults));
      $this->assertEqual(0,count($errors));      
      $fields = $data["fields"][0];
      
      $this->assertTrue(in_array('tab1',$node->getTabs('add')));
      $this->assertTrue(in_array('tab2',$node->getTabs('add')));
      $this->assertTrue(in_array('tab3',$node->getTabs('add')));
      $this->assertFalse(in_array('tab3',$node->getTabs('edit')));
      
      $this->assertTrue(in_array('tab1',$fields["attribute"]->getTabs('add')));
      $this->assertTrue(in_array('tab2',$fields["attribute"]->getTabs('add')));
      $this->assertFalse(in_array('tab3',$fields["attribute"]->getTabs('add')));
      
      $this->assertTrue($attr1->showOnTab('tab1'));
      $this->assertTrue($attr1->showOnTab('tab2'));
      $this->assertTrue($attr2->showOnTab('tab2'));
      $this->assertFalse($attr3->showOnTab('tab2'));
      $this->assertFalse($attr4->showOnTab('tab1'));
      $this->assertTrue($attr4->showOnTab('default'));

      $this->assertTrue(in_array('tab1',$fields["tabs"]));
      $this->assertTrue(in_array('tab2',$fields["tabs"]));
      $this->assertFalse(in_array('tab3',$fields["tabs"]));
    }
    
    function test_section()
    {
      $node = new atkNode("testnode");
      $attr1 = &$node->add(new atkAttribute('test1'),array('tab1.section1', 'tab2.section1'));
      $attr2 = &$node->add(new atkAttribute('test2'),'tab2.section2');
      $attr3 = &$node->add(new atkAttribute('test3',AF_HIDE_EDIT),'tab3.section2');
      $attr4 = &$node->add(new atkAttribute('test4'));
      
      //test if sections array is correctly filled.      
      $this->assertNotNull($attr1->getSections());
      $this->assertNotEqual(0,count($attr1->getSections()));
      $this->assertTrue(in_array("tab1.section1",$attr1->getSections()));
      $this->assertTrue(in_array("tab2.section2",$attr2->getSections()));      
      $this->assertTrue(in_array("tab3.section2",$attr3->getSections()));
      $this->assertFalse(in_array("tab1.section1",$attr3->getSections()));
      $this->assertTrue(in_array("default", $attr4->getSections()));
    }
      
  }

?>
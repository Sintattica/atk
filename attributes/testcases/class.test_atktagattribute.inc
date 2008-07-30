<?php

  /**
   * This file is part of the Achievo ATK distribution.
   * Detailed copyright and licensing information can be found
   * in the doc/COPYRIGHT and doc/LICENSE files which should be
   * included in the distribution.
   *
   * @package atk
   * @subpackage testcases
   *
   * @copyright (c)2007-2008 Ibuildings.nl BV
   * @license http://www.achievo.org/atk/licensing ATK Open Source License
   *
   * @version $Revision$
   */

    /**
     * @internal Include the class we are about to test.
     */
    atkimport("atk.attributes.atktagattribute");
  
    /**
     * Dummy class for the testcases
     * @package atk
     * @subpackage testcases
     */
    class dummyTagAttribute extends atkTagAttribute 
    {
      var $m_dummytags = null;
      
      /**
       * We override this method so we can test the attribute without 
       * database access or linking to another atkNode.
       *
       * @return Array
       */
      function createDestinationInstance()
      {        
        return true;
      }
  
      /**
       * We override this method so we can test the attribute without 
       * database access or linking to another atkNode.
       *
       * @return Array
       */
      function createLink()
      {       
        return true;
      }
      
      /**
       * We override this method so we can test the attribute without 
       * database access or linking to another atkNode.
       *
       * @return Array
       */
      function _getDefaultTags()
      {
        $this->m_dummytags = array();
        for($i=0,$_i=4;$i<$_i;$i++)
        {
          $this->m_dummytags[$i]['id'] = $i+1;
          $this->m_dummytags[$i]['name'] = 'test'.($i+1);
        }
        return $this->m_dummytags;
      }

    }
    
    /**
     * Testcases for class atktagattribute.
     *
     * @package atk
     * @subpackage testcases
     * @internal
     */
    class test_atktagattribute extends atkAttributeTestCase
    {
      var $m_attribute = null;
      
      function test_isValidKeyWord()
      {
        $this->m_attribute = &new dummyTagAttribute("tag", 'dummydest', "id",'tag_pagetag', TA_ADD);
        
        $this->assertFalse($this->m_attribute->isValidKeyWord("invalidname<br>test"));
        $this->assertFalse($this->m_attribute->isValidKeyWord("invalidname2<br>\r\ntest"));
        $this->assertFalse($this->m_attribute->isValidKeyWord("invalidname3<br>\ntest"));
        $this->assertFalse($this->m_attribute->isValidKeyWord("invalidname4\r\ntest"));
        $this->assertFalse($this->m_attribute->isValidKeyWord("<br/>"));
        
        $this->assertTrue($this->m_attribute->isValidKeyWord('validname1'));
        $this->assertTrue($this->m_attribute->isValidKeyWord('a valid name'));
        $this->assertTrue($this->m_attribute->isValidKeyWord('bleh  '));
        $this->assertTrue($this->m_attribute->isValidKeyWord('This is a test!'));        
      }
    }

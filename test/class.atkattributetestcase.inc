<?php

  /**
   * This file is part of the Achievo ATK distribution.
   * Detailed copyright and licensing information can be found
   * in the doc/COPYRIGHT and doc/LICENSE files which should be 
   * included in the distribution.
   *
   * @package atk
   * @subpackage test
   *
   * @copyright (c)2005 Ivo Jansch
   * @license http://www.achievo.org/atk/licensing ATK Open Source License
   *
   * @version $Revision$
   * $Id$
   */      
  
  /**
   * Baseclass for attribute unittests. This class is an extension to the 
   * default UnitTestCase. It provides an apiTest method that performs a 
   * basic test if the attribute responds to standard api calls correctly
   *
   * @author Ivo Jansch <ivo@achievo.org>
   * @package atk
   * @subpackage subpackage
   * @access private
   */
  class atkAttributeTestCase extends UnitTestCase
  {        
    var $m_attribute = null;
        
    /**
     * Perform api test. 
     *
     * Checks an attribute for general characteristics, like
     * valid return types for often overriden methods, the ability to
     * check, add, remove flags, compatibility of value2db with db2value, 
     * and canonical reactions to methods like isEmpty when called with empty
     * records.
     *
     * @param atkAttribute $attribute Instance of the attribute to check.
     */
    function apiTest(&$attribute)
    {
      $this->m_attribute = $attribute;
      $this->_testFlagGetSet();
      $this->_testDbConversion();
      $this->_testIsEmpty();
      $this->_testReturnValues();
    }
      
    /**
     * Test addFlag, removeFlag and hasFlag.
     * @access private
     */
    function _testFlagGetSet()
    {
      $this->m_attribute->m_flags = 0;
      $this->assertFalse($this->m_attribute->hasFlag(AF_HIDE_LIST), "hasflagfalse");      
      $this->m_attribute->addFlag(AF_HIDE_LIST);
      $this->assertTrue($this->m_attribute->hasFlag(AF_HIDE_LIST), "hasflagtrue");      
      $this->m_attribute->removeFlag(AF_HIDE_LIST);
      $this->assertFalse($this->m_attribute->hasFlag(AF_HIDE_LIST), "hasflagfalse");      
    }
    
    /**
     * Check if value2db and db2value are eachothers full opposite.
     * @access private
     */
    function _testDbConversion()
    {
      // check roundtrip
      $dbvalue = "300";
      $internalvalue = $this->m_attribute->db2value(array($this->m_attribute->m_name=>$dbvalue));      
      $roundtrip = $this->m_attribute->value2db(array($this->m_attribute->m_name=>$internalvalue));
      $this->assertEqual($dbvalue, $roundtrip, $this->_getMsg("db/value roundtrip check"));                        
    }
    
    /**
     * Check if isEmpty returns true for empty records.
     * @access private
     */
    function _testIsEmpty()
    {
      // check if empty record is recognized as such.
      // we can't do the reverse check, since each attribute defines for itself what 'not empty'
      // means.
      $this->assertTrue($this->m_attribute->isEmpty(array()), "isempty");
      $this->assertTrue($this->m_attribute->isEmpty(array($this->m_attribute->m_name=>"")), "isempty");
    }
    
    /**
     * Check return types for methods where the system is dependent on a proper use of
     * the returned value.
     * @access private
     */
    function _testReturnValues()
    {
      $this->assertBoolean($this->m_attribute->hasStore("admin"), "hasstore");      
      $this->assertBoolean($this->m_attribute->deleteAllowed(), "deleteAllowed");      
      $this->assertBoolean($this->m_attribute->needsInsert(array()), "needsinsert");      
      $this->assertBoolean($this->m_attribute->needsUpdate(array()), "needsupdate");      
      $this->assertNumeric($this->m_attribute->dbFieldSize(), "dbfieldsize");
      $this->assertInteger($this->m_attribute->loadType("edit"), "loadtype");
      $this->assertInteger($this->m_attribute->storageType("edit"), "storagetype");
    }
    
    /**
     * Assert if a value is a boolean (true or false).
     * @param mixed $value Value to check
     * @param String $msg Message to display for test
     */     
    function assertBoolean($value, $msg="")
    {
      $this->assertTrue((is_bool($value)), "is_bool ".$msg);
    }
    
    /**
     * Assert if a value is numeric (checks only the actual value, not its
     * type).
     * @param mixed $value Value to check
     * @param String $msg Message to display for test
     */     
    function assertNumeric($value, $msg="")
    {
      $this->assertTrue((is_numeric($value)), "is_numeric ".$msg);
    }
    
    /**
     * Assert if a value is an integer.
     * @param mixed $value Value to check
     * @param String $msg Message to display for test
     */     
    function assertInteger($value, $msg="")
    {
      $this->assertTrue((is_integer($value)), "is_numeric ".$msg);
    }
    
    /**
     * Get canonical test message (attribute classname + custom message + %s.
     * @access private
     * @param String $msg Message to display for test
     */     
    function _getMsg($msg="")
    {
      return get_class($this->m_attribute)." $msg %s";
    }      
    
  }

?>
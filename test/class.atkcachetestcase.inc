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
 * @copyright (c)2008 Sandy Pleyte
 * @author Sandy Pleyte <sandy@achievo.org>
 * 
 * @license http://www.achievo.org/atk/licensing ATK Open Source License
 *
 * @version $Revision$
 * $Id$
 */     
class atkCacheTestCase extends UnitTestCase
{
  
  protected $m_cache;
  protected $m_lifetime = 5;
    
  protected $m_type='';
    
  public function setup()
  {
    atkimport("atk.cache.atkcache");
  
    // Set lifetime for testcases
    $GLOBALS['config_cache'][$this->m_type]['lifetime'] = $this->m_lifetime;
    
    
    $this->m_cache = atkCache::getinstance($this->m_type);

    // We need to make sure that the cache is empty
    // It's not correct, but I don't know an other 
    // way at te moment
    $this->m_cache->deleteAll();
  }
  
  /**
   * Check if we have the correct object
   */
  public function test__construct()
  {
     $classname = "atkCache_".$this->m_type;
     $this->assertTrue($this->m_cache instanceof $classname,"Do we have the correct object (".$this->m_type.") ?");
  }
  
  /**
   * Check the isActive function
   */
  public function test_isActive()
  {
    // should be active by default
    $this->assertTrue($this->m_cache->isActive(),"Is cache active ?");
    
    // turn it off
    $this->m_cache->setActive(false);
    $this->assertFalse($this->m_cache->isActive(),"Is cache deactivated ?");
    
    // turn it back on
    $this->m_cache->setActive(true);
    $this->assertTrue($this->m_cache->isActive(),"Is cache active ?");
  }

  /**
   * Check the getLifeTime function
   */
  public function test_getLifetime()
  {
    $id = 'test_getlifetime';
    $data = 'Lifetime data';
    
    // configured from setup
    $this->assertEqual($this->m_cache->getLifetime(), $this->m_lifetime);
    
    // store something
    $this->assertTrue($this->m_cache->set($id, $data),'Save data');
    $this->assertEqual($this->m_cache->get($id), $data);
    
    // wait until just before the lifetime,
    // we should still get data
    sleep($this->m_cache->getLifetime() - 2);
    $this->assertEqual($this->m_cache->get($id), $data);
    
    // wait until just after the lifetime,
    // we should get nothing
    sleep(3);
    $this->assertFalse($this->m_cache->get($id));
    
    // with custom lifetime on the set function
    $this->assertTrue($this->m_cache->set($id, $data, 8),'Save data');
    $this->assertEqual($this->m_cache->get($id), $data);
    
    //make sure that the default time isn't used
    sleep(5); 
    $this->assertEqual($this->m_cache->get($id), $data);
    
    sleep(3);
    $this->assertFalse($this->m_cache->get($id));
 
  }

  /**
   * Test if adding data to cache works
   */
  public function test_add()
  {
    $id = 'test_add';
    $data = 'Add data';
    $id2 = 'test_add2';
    $data2 = 'Add data2';
    
    // add for the first time
    $this->m_cache[$id2]=$data2;
    //$this->assertTrue($this->m_cache[$id]=$data),"Add data");
    $this->assertEqual($this->m_cache[$id2], $data2);

    $this->assertTrue($this->m_cache->add($id, $data),"Add data");
    $this->assertEqual($this->m_cache->get($id), $data);
    
    // add for the second time with a different value, should fail
    $this->assertFalse($this->m_cache->add($id, 'atk'),"Add data for second time with different value");
    
    // make sure it really didn't overwrite the data
    $this->assertEqual($this->m_cache->get($id), $data);
  }


  /**
   * Test if we can get data from the cache
   */
  public function test_get()
  {
    $id = 'test_get';
    $data = 'Get data';
    
    // data has not been stored yet
    $this->assertFalse($this->m_cache->get($id));
    
    // store it
    $this->assertTrue($this->m_cache->set($id, $data),"Save data");
    
    // and we should be able to get now
    $this->assertEqual($this->m_cache->get($id), $data);
    
    // deactivate then try to get
    $this->m_cache->setActive(false);
    $this->assertFalse($this->m_cache->isActive());
    $this->assertNull($this->m_cache->get($id));
    
    // re-activate then try to get
    $this->m_cache->setActive(true);
    $this->assertTrue($this->m_cache->isActive(),'Is cache still active ?');
    $this->assertEqual($this->m_cache->get($id), $data);
  }
  
  /**
   * Test if we can save array's to the cache
   */
  public function test_set_Array()
  {
    $id = 'save_Array';
    $data = array(
        'name'   => 'atkCache',
        'parent' => 'atk',
        'year'   => '2008',
    );
    $this->assertTrue($this->m_cache->set($id, $data),"Save data");
    $this->assertEqual($this->m_cache->get($id), $data);
  }
  
  /**
   * Test if we can save objects's to the cache
   */
  public function test_set_Object()
  {
    $id = 'save_object';
    $data = atknew("atk.atklanguage");
    $this->assertTrue($this->m_cache->set($id, $data),"Save data");
    $this->assertEqual($this->m_cache->get($id), $data);
  }

  /**
   * Test if we can save strings to the cache
   */
  public function test_set_String()
  {
    $id = 'test_save_string';
    $data = 'Save string data';
    $this->assertTrue($this->m_cache->set($id, $data),'Save data');
    $this->assertEqual($this->m_cache->get($id), $data);
  }
  
  /**
   * Test if we can remove data from the cache
   */
  public function test_delete()
  {
    $id = 'test_delete';
    $data = 'Delete data';
    
    // data has not been stored yet
    $this->assertFalse($this->m_cache->get($id));
    
    // store it
    $this->assertTrue($this->m_cache->set($id, $data),"Save data");
    
    // and we should be able to get now
    $this->assertEqual($this->m_cache->get($id), $data);
    
    // delete it, should not be able to get again
    $this->m_cache->delete($id);
    $this->assertFalse($this->m_cache->get($id));
  }

  /**
   * Test if we can delete the complete cache
   */
  public function test_deleteAll()
  {
    $list = array(1, 2, 'three','four');
    $data = 'Delete all data';
    
    foreach ($list as $id) {
        // data has not been stored yet
        $this->assertFalse($this->m_cache->get($id));
        // so store some data
        $this->assertTrue($this->m_cache->set($id, $data),"Save data");
        // and we should be able to get now
        $this->assertEqual($this->m_cache->get($id), $data);
    }
    
    // delete everything
    $this->m_cache->deleteAll();
    
    // should not be able to get again
    foreach ($list as $id) {
        $this->assertFalse($this->m_cache->get($id));
    }
  }

  /**
   * Test if we got the correct type
   */
  public function test_getType()
  {
    $this->assertTrue($this->m_cache->getType()==$this->m_type,"Is the cache of the correct type (".$this->m_cache->getType()." == ".$this->m_type.")?");
  }
  
}



?>
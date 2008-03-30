<?php
/**
 * This file is part of the Achievo ATK distribution.
 * Detailed copyright and licensing information can be found
 * in the doc/COPYRIGHT and doc/LICENSE files which should be
 * included in the distribution.
 *
 * Test class for memcache
 * 
 * @package atk
 * @subpackage cache
 *
 * @copyright (c)2008 Sandy Pleyte
 * @author Sandy Pleyte <sandy@achievo.org>
 * @license http://www.achievo.org/atk/licensing ATK Open Source License
 *
 * @version $Revision$
 * $Id$
 */
class test_atkcache_memcache extends atkCacheTestCase 
{
   protected $m_cache;
    
   protected $m_type= "memcache";
    
  public function skip()
  {
    $this->skipIf(!extension_loaded('memcache'),'The memcache extension is not loaded');
  }
  
}
?>
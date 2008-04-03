<?php
/**
 * This file is part of the Achievo ATK distribution.
 * Detailed copyright and licensing information can be found
 * in the doc/COPYRIGHT and doc/LICENSE files which should be
 * included in the distribution.
 *
 * Test class for xcache
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
atkimport("atk.test.atkcachetestcase");

class test_atkcache_xcache extends atkCacheTestCase
{
   protected $m_cache;

   protected $m_type= "xcache";

  public function skip()
  {
    $this->skipIf(!(extension_loaded('xcache') && ini_get('xcache.cacher')),'The xCache extension is not loaded');
  }


  /**
   * Currently turned off because I can't get the xCache
   * working with logging in
   */
  public function test_deleteAll() {}

}
?>
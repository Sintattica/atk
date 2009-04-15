<?php
/**
 * This file is part of the Achievo ATK distribution.
 * Detailed copyright and licensing information can be found
 * in the doc/COPYRIGHT and doc/LICENSE files which should be
 * included in the distribution.
 *
 * Test class for apc
 *
 * @package atk
 * @subpackage cache
 *
 * @copyright (c)2008 Sandy Pleyte
 * @author Sandy Pleyte <sandy@achievo.org>
 * @license http://www.achievo.org/atk/licensing ATK Open Source License
 *
 * @version $Revision: 5898 $
 * $Id$
 */
atkimport("atk.test.atkcachetestcase");

class test_atkcache_apc extends atkCacheTestCase
{
   protected $m_cache;

   protected $m_type= "apc";

   public function skip()
   {
     $this->skipIf(!(extension_loaded('apc') && ini_get('apc.enabled')), "The APC extension is not loaded or disabled");
   }

}
?>
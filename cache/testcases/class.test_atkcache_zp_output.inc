<?php
/**
 * This file is part of the Achievo ATK distribution.
 * Detailed copyright and licensing information can be found
 * in the doc/COPYRIGHT and doc/LICENSE files which should be
 * included in the distribution.
 *
 * Test class for Zend Platform (output)
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
class test_atkcache_zp_output extends atkCacheTestCase 
{
   protected $m_cache;
    
   protected $m_type= "zp_output";
  
   public function skip()
   {
     $this->skipIf(!function_exists('accelerator_license_info'),'The Zend Platform extension is not loaded');
     
     if(function_exists('accelerator_license_info')) 
     {
       $licenseInfo = accelerator_license_info();
       $this->skipIf(!function_exists('accelerator_get_configuration'),'The Zend Platform extension is not loaded correctly: '.$licenseInfo['failure_reason']); 
     }
     
     if(function_exists('accelerator_get_configuration'))
     {
       $acceleratorConfig = accelerator_get_configuration();
       $this->skipIf(@!$acceleratorConfig['output_cache_licensed'],'The Zend Platform extension does not have the proper license to use content caching features.');
       $this->skipIf(@!$acceleratorConfig['output_cache_enabled'],'The Zend Platform content caching feature must be enabled for using this backend, set the \'zend_accelerator.output_cache_enabled\' directive to On !');
       $this->skipIf(!is_writable($acceleratorConfig['output_cache_dir']),'The cache copies directory \''. ini_get('zend_accelerator.output_cache_dir') .'\' must be writable !');
       $this->skipIf(!function_exists('output_cache_put'),'The Zend Platform doesn\'t support the ouput cache anymore (version 3.0 and lower only)');
     }
   }
  
  /**
   * Zend platform doesn't support the deleteAll function
   * so we remove the test_add key before starting the
   * add test.
   */
  public function test_add()
  {
    $this->m_cache->delete('test_add');
    parent::test_add();
  }

  /**
   * Zend platform (output) doesn't support the delete all function
   * so we skip this test
   */
  public function test_deleteAll() {}
   
}
?>
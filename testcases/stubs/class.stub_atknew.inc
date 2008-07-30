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
   * @copyright (c)2008 Ibuildings.nl BV
   * @license http://www.achievo.org/atk/licensing ATK Open Source License
   *
   * @version $Revision$
   */

/**
 * Stub class used for testing atknew in the test class test_atktools.
 * @internal
 * @package atk
 * @subpackage testcases
 */
class Stub_atkNew
{
  private $m_args;
  
  /**
   * Constructor.
   */
  public function __construct()
  {
    $this->m_args = func_get_args();
  }
  
  /**
   * Returns the constructor arguments.
   */
  public function getArgs()
  {
    return $this->m_args;
  }
}

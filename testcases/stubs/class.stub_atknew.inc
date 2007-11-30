<?php
/**
 * Stub class used for testing atknew in the test class test_atktools.
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
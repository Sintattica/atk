<?php
/**
 * Small wrapper for PHP views to make template variables accessible using
 * $this->varname syntax in preparation of using real Zend_View templates.
 */
class atkPHPView
{
  private $m_path;
  private $m_vars;
  
  /**
   * Constructor.
   * 
   * @param string $path template path
   * @param array  $vars template variables (name/value)
   */
  public function __construct($path, $vars)
  {
    $this->m_path = $path;
    $this->m_vars = $vars;
  }
  
  /**
   * Render.
   */
  public function __toString()
  {
    extract($this->m_vars);
    ob_start();
    include $this->m_path;
    return ob_get_clean();
  }
  
  /**
   * Checks whatever the given template variable is set / exists.
   * 
   * @param string $name template variable name
   * 
   * @return boolean is variable set?
   */
  public function __isset($name)
  {
    return isset($this->m_vars[$name]);
  }
  
  /**
   * Returns the template variable with the given name.
   * 
   * @param string $name template variable name
   */
  public function __get($name)
  {
    return $this->m_vars[$name];
  }
}
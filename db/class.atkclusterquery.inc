<?php
  /**
   * This file is part of the Achievo ATK distribution.
   * Detailed copyright and licensing information can be found
   * in the doc/COPYRIGHT and doc/LICENSE files which should be
   * included in the distribution.
   *
   * @package atk
   * @subpackage db
   *
   * @copyright (c)2008 Ibuildings.nl BV
   * @license http://www.achievo.org/atk/licensing ATK Open Source License
   *
   * @version $Revision$
   * $Id$
   */

  /**
   * A clustered query driver
   *
   * Proxies a query, creates a query bound to the current database node
   * and performs the query on current database node.
   *
   * @author Boy Baukema <boy@ibuildings.nl>
   * @package atk
   * @subpackage db
   */
  class atkClusterQuery
  {
    private $m_query;

   /**
    * Constructs a new atkClusterQuery object
    * 
    * @return void
    */
    public function __construct()
    {
      $this->m_query = atkGetDb()->createQuery();
    }

    ////////////////////////////// OVERLOADING METHODS //////////////////////////////

    /**
     * Magic set method
     *
     * @param string $name
     * @param mixed $value
     */
    public function __set($name, $value)
    {
      $this->m_query->$name = $value;
    }

    /**
     * Magic get function
     *
     * @param string $name
     * @return mized
     */
    public function __get($name)
    {
      return $this->m_query->$name;
    }

    /**
     * Magic isset function
     *
     * @param string $name
     * @return bool
     */
    public function __isset($name)
    {
      return isset($this->m_query->$name);
    }

    /**
     * Magic unset method
     *
     * @param string $name
     */
    public function __unset($name)
    {
      unset($this->m_query->$name);
    }

    /**
     * Magic call function
     *
     * @param string $name
     * @param array $arguments
     * @return mixed
     */
    public function __call($name, $arguments)
    {
      return call_user_method_array($name,$this->m_query,$arguments);
    }

    /**
     * Magic callstatic function
     *
     * @param string $name
     * @param array $arguments
     * @return mixed
     */
    public static function __callStatic($name, $arguments)
    {
      return call_user_func_array(array(__NAMESPACE__ .'::'.get_class($this->m_query), $name), $arguments);
    }
  }

?>
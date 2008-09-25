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
   * Proxy DDL for cluster configurations.
   *
   * Proxies everything on the DDL for the current node.
   * 
   * @author Boy Baukema <boy@ibuildings.nl>
   * @package atk
   * @subpackage db
   */
  class atkClusterDDL
  {
    private $m_ddl;

   /**
    * Constructs the atkClusterDDL object
    *
    * @access public
    * @return void
    */
    public function __construct()
    {
      $this->m_ddl = atkGetDb()->createDDL();
    }

    ////////////////////////////// OVERLOADING METHODS //////////////////////////////

    public function __set($name, $value)
    {
      $this->m_ddl->$name = $value;
    }

    public function __get($name)
    {
      return $this->m_ddl->$name;
    }

    public function __isset($name)
    {
      return isset($this->m_ddl->$name);
    }

    public function __unset($name)
    {
      unset($this->m_ddl->$name);
    }

    public function __call($name, $arguments)
    {
      return call_user_method_array($name,$this->m_ddl,$arguments);
    }

    public static function __callStatic($name, $arguments)
    {
      return call_user_func_array(array(__NAMESPACE__ .'::'.get_class($this->m_ddl), $name), $arguments);
    }
  }

?>
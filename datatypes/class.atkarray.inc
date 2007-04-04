<?php

/**
   * This file is part of the Achievo ATK distribution.
   * Detailed copyright and licensing information can be found
   * in the doc/COPYRIGHT and doc/LICENSE files which should be
   * included in the distribution.
   *
   * @package atk
   * @subpackage datatypes
   *
   * @copyright (c)2000-2004 Ibuildings.nl BV
   * @license http://www.achievo.org/atk/licensing ATK Open Source License
   *
   * @version $Revision$
   * $Id$
   */
  atkimport('atk.datatypes.atkdatatype');

  /**
   * The 'array' datatype.
   * Useful for performing various small operations on arrays fluently.
   *
   * @author Patrick van der Velden <patrick@achievo.org>
   * @package atk
   * @subpackage datatypes
   */
  class atkArray extends atkDataType
  {
    /**
     * @var array The internal value of the current array object
     */
    protected $m_array = array();

    /***************** BASICS *****************/

    /**
     * The 'array' datatype for easy manipulation of arrays.
     *
     * @param array $array
     */
    public function __construct($array)
    {
      $this->m_array = $array;
    }

    /***************** OPERATIONS *****************/

    /**
     * (Multidimensional) atkArray replace function
     *
     * @param mixed $search
     * @param mixed $replace
     * @param bool $recursive
     */
    function replace($search, $replace, $recursive=true)
    {
      $this->_replace($this->m_array, $search, $replace, $recursive);
    }

    /**
     * Private (Multidimensional) array replace function
     *
     * @param array $array
     * @param mixed $search
     * @param mixed $replace
     * @param bool $recursive
     */
    private function _replace(&$array, $search, $replace, $recursive=true)
    {
      foreach($array as $key => &$value)
      {
        if (is_array($value) && $recursive)
        {
          $this->_replace($value, $search, $replace);
        }
        elseif ($value == $search)
        {
          $array[$key] = $replace;
        }
      }
    }

    /***************** GETTERS *****************/

    /**
     * Get the current array.
     *
     * @return array The current array
     */
    public function getArray()
    {
      return $this->m_array;
    }

  }

?>
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

  /**
   * Abstract baseclass for ATK datatypes
   *
   * Implements some custom functionality and defines some methods that
   * derived classes should override.
   * 
   * Datatypes are meant as lightweight, easy and fluent to use PHP5 objects
   * for converting one format into another and doing simple operations
   * on various datatypes.
   *
   * @deprecated Scheduled for removal.
   * @author Boy Baukema <boy@achievo.org>
   * @package atk
   * @subpackage datatypes
   */
  abstract class atkDataType
  {    
    /**
     * Factory method, creates a new atkDatatype object.
     * Override this where possible!
     * 
     * @param array $arguments
     * @return atkDataType
     */
    public function create($arguments=array())
    {
      $class=null;
      eval("\$class = new ".get_class($this).
           "(".implode(',',$arguments).")");
      return $class;
    }
    
    /**
     * Dump the internal value of the datatype to debugging
     *
     * @param string $label The label for the debug dump
     * @return atkDataType The current datatype
     */
    public function dump($label="")
    {
      atk_var_dump($this->getData(),$label);
      return $this;
    }
    
    /**
     * Returns the raw data from the datatype. 
     * For internal use only (dumping, etc.)!
     *
     * @return atkDataType
     */
    protected function getData()
    {
      return $this;
    }
  }

?>

<?php

  /**
   * This file is part of the Achievo ATK distribution.
   * Detailed copyright and licensing information can be found
   * in the doc/COPYRIGHT and doc/LICENSE files which should be
   * included in the distribution.
   *
   * @package atk
   * @subpackage attributes
   *
   * @copyright (c)2000-2004 Ibuildings.nl BV
   * @license http://www.achievo.org/atk/licensing ATK Open Source License
   *
   */

  /**
   * The atkNumberAttribute can be used for money values.
   *
   * @author Mark Baaijens <mark@ibuildings.nl>
   * @package atk
   * @subpackage attributes
   *
   */
useattrib("atknumberattribute");

class atkCurrencyAttribute extends atkNumberAttribute
{
  var $m_valutasymbol;

  /**
    * Constructor
    * @param $name  Name of the attribute
    * @param $valutasymbol The symbol which is printed in front of the value.
    * @param $flags Flags for this attribute
    * @param $size  The size(s) for this attribute (default 10)
    *
    */
  function atkCurrencyAttribute ($name,$valutasymbol,$flags,$size)
  {
    $this->atkAttribute($name,$flags|AF_NO_QUOTES, $size); // base class constructor
    $this->m_decimals = 2;
    $this->m_valutasymbol = $valutasymbol;
  }
  
  /**
   * overrides the edit function to put the currencysymbol in from of the input field
   */
  function edit($record="", $fieldprefix="")
  {
    return $this->m_valutasymbol." ".parent::edit($record,$fieldprefix);
  }
  
  /**
   * overrides the edit function to put the currencysymbol in from of the input field
   */
  function display($record, $mode="")
  {
    return $this->m_valutasymbol.parent::display($record,$mode);
  }
}


?>

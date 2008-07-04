<?php
useattrib("atknumberattribute");

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
   * The atkCurrencyAttribute can be used for money values.
   *
   * @author Mark Baaijens <mark@ibuildings.nl>
   * @package atk
   * @subpackage attributes
   *
   */
class atkCurrencyAttribute extends atkNumberAttribute
{
  var $m_currencysymbol;

  /**
    * Constructor
    * @param string $name  Name of the attribute
    * @param int    $flags Flags for this attribute
    * @param int    $size  The size(s) for this attribute (default 10)
    * @param string $currencysymbol The symbol which is printed in front of the value.
    * @param int    $decimals The number of decimals (default 2)
    * @param string $decimalseparator The separator which is printed for the decimals.
    * @param string $thousandsseparator The separator which is printed for the thousands.
    *
    */
  function atkCurrencyAttribute($name,$flags=0,$size=10,$currencysymbol="",$decimals=2,$decimalseparator="",$thousandsseparator="")
  {
    $this->atkNumberAttribute($name,$flags,$size,$decimals); // base class constructor

    if($currencysymbol=="") $currencysymbol=atktext("currencysymbol","atk","","","",true);
    
    $this->m_currencysymbol = $currencysymbol;
    $this->m_decimalseparator = ($decimalseparator!=""?$decimalseparator:".");
    $this->m_thousandsseparator = ($thousandsseparator!=""?$thousandsseparator:",");
    
    $this->setUseThousandsSeparator(true);
  }

  /**
   * overrides the edit function to put the currencysymbol in front of the input field
   */
  function edit($record="", $fieldprefix="")
  {
    return $this->m_currencysymbol." ".parent::edit($record,$fieldprefix);
  }

  /**
   * overrides the display function to put the currencysymbol in front of the input field
   */
  function display($record, $mode="")
  {
    $result = empty($this->m_currencysymbol) ? "" : $this->m_currencysymbol." ";
    $result .= parent::display($record,$fieldprefix);
    return $result;
  }
}


?>

<?php
/**
 * This file is part of the Achievo ATK distribution.
 * Detailed copyright and licensing information can be found
 * in the doc/COPYRIGHT and doc/LICENSE files which should be
 * included in the distribution.
 *
 * @package atk
 * @subpackage utils
 *
 * @copyright (c) 2000-2007 Ibuildings.nl BV
 * 
 * @license http://www.achievo.org/atk/licensing ATK Open Source License
 */

atkimport('atk.datagrid.atkdgcomponent');

/**
 * The data grid limit box. Can be used to render a 
 * limit box for an ATK data grid.
 *
 * @author Peter C. Verhage <peter@achievo.org>
 * @package atk
 * @subpackage datagrid
 */
class atkDGLimit extends atkDGComponent
{
  /**
   * Returns a list of possible limit values.
   *
   * @return array list of possible limit values
   */
  protected function getValues()
  {
    $defaultLimit = $this->getGrid()->getDefaultLimit();
    $limit = $this->getGrid()->getLimit();    
    $values = array(5, 10, 15, 20, 25, 30, 40, 50, 100, $defaultLimit, $limit);
    $values = array_diff($values, array(-1));
    $values = array_unique($values);
    sort($values);    
    return $values;
  }
  
  /**
   * Returns the possible options.
   *
   * @param array $values possible limits
   * 
   * @return array possible options
   */
  protected function getOptions($values)
  {
    $options = array();
    
    $limit = $this->getGrid()->getLimit();
        
    foreach ($values as $value)
    {
      $current = $value == $limit;
      $options[] = array('title' => $value, 'value' => $value, 'current' => $current);
    }   
    
    $options[] = array('title' => $this->text('all'), 'value' => -1, 'current' => $limit == -1);
    
    return $options;
  }    
  
  /**
   * Renders the limit box for the given data grid.
   *
   * @return string rendered HTML
   */
  public function render()
  {
    $values = $this->getValues();
    
    if ($this->getGrid()->getCount() <= $values[0])
    {
      return '';
    }
    
    $options = $this->getOptions($values);
    $call = $this->getGrid()->getUpdateCall(array('atkstartat' => 0));
    $result = $this->getUi()->render('dglimit.tpl', array('name' => 'atklimit', 'options' => $options, 'call' => $call));
    return $result;    
  }
}
?>
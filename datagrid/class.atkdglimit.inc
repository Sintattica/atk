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
 * @copyright (c)2000-2006 Ibuildings.nl BV
 * @copyright (c)2000-2006 Ivo Jansch
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
   * Returns the possible options.
   *
   * @param atkDataGrid $grid
   * @return array possible options
   */
  function getOptions(&$grid, $defaultLimit, $limit)
  {
    $options = array();
    
    $defaultLimit = $grid->getDefaultLimit();
    $limit = $grid->getLimit();    
    
    $values = array(5, 10, 15, 20, 25, 30, 40, 50, 100, $defaultLimit, $limit);
    $values = array_unique($values);
    sort($values);
    
    foreach ($values as $value)
    {
      $current = $value == $limit;
      $options[] = array('title' => $value, 'value' => $value, 'current' => $current);
    }   
    
    return $options;
  }    
  
  /**
   * Renders the limit box for the given data grid.
   *
   * @param atkDataGrid $grid the data grid
   * @return string rendered HTML
   */
  function render(&$grid)
  {
    $options = $this->getOptions($grid);
    $script = $grid->createLink(array('atkstartat' => 0), array('atklimit' => '$F(this)'));
    
    $ui = &$grid->getUi();
    $result = $ui->render('dglimit.tpl', array('options' => $options, 'script' => $script));
    
    return $result;    
  }
}
?>
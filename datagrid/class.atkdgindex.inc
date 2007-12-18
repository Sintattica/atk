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
 * The data grid index. Can be used to render an alphanumeric index
 * for an ATK data grid.
 *
 * @author Peter C. Verhage <peter@achievo.org>
 * @package atk
 * @subpackage datagrid
 */
class atkDGIndex extends atkDGComponent
{
  /**
   * Returns the available indices.
   * 
   * @return array available indices
   * 
   * @todo Implement retrieval of available indices ourselves either
   *       part of this class or atkDataGrid. But first selectDb and
   *       countDb of atkNode should be refactored so we can create
   *       a small clean implementation.
   */
  protected function getAvailableIndices()
  {
    atkimport('atk.utils.atknavigator');
    return atkNavigator::getAvailableIndices($this->getNode());  
  }
  
  /**
   * Returns an array with index links.
   */
  protected function getLinks()
  {
    $grid = $this->getGrid();
    $links = array();    
    
    $chars = $this->getAvailableIndices();
    $current = $grid->getIndex();  
    
    // indices
    foreach ($chars as $char)
    {
      $title = $char;
      $call = $grid->getUpdateCall(array('atkstartat' => 0, 'atkindex' => "{$char}*"));
      $links[] = array('type' => 'index', 'title' => $title, 'call' => $call, 'current' => "{$char}*" == $current);
    }

    // view all
    if (!empty($current))
    {
      $title = $grid->text('view_all');               
      $call = $grid->getUpdateCall(array('atkindex' => ''));
      $links[] = array('type' => 'all', 'call' => $call, 'title' => $title);      
    }    
    
    return $links;
  }
  
  /**
   * Renders the index for the given data grid.
   *
   * @return string rendered HTML
   */
  public function render()
  {
    $links = $this->getLinks();
    $result = $this->getUi()->render('dgindex.tpl', array('links' => $links));
    return $result;
  }
}
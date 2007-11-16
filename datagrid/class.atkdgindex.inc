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
 * @copyright (c) 2000-2007 Peter C. Verhage
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
   * @todo use database to get an overview of real available indices
   */
  protected function getAvailableIndices()
  {
    return array('A','B','C','D','E','F','G','H','I','J','K','L','M',
                 'N','O','P','Q','R','S','T','U','V','W','X','Y','Z');    
  }
  
  /**
   * Returns an array with index links.
   */
  protected function getLinks()
  {
    $grid = $this->getGrid();
    $links = array();    
    
    $chars = $this->getAvailableIndices();
    $index = $grid->getIndex();  
    $search = $grid->getSearch();
    $current = isset($search[$index]) ? $search[$index]{0} : NULL;
    
    // indices
    foreach ($chars as $char)
    {
      $title = $char;
      $call = $grid->getUpdateCall(array('atkstartat' => 0, 'atksearch' => array($index => "{$char}*"), 'atksinglesearchmode' => array($index => 'wildcard')));
      $links[] = array('type' => 'index', 'title' => $title, 'call' => $call, 'current' => $char == $current);
    }

    // view all
    if (count($search) > 0)
    {
      $title = $grid->text('view_all');               
      $call = $grid->getUpdateCall(array('atksearch' => ''));
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
    $result = $this->getGrid()->getUi()->render('dgindex.tpl', array('links' => $links));
    return $result;
  }
}
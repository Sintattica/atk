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
   * @param atkDataGrid $grid 
   * @return array available indices
   * 
   * @todo use database to get an overview of real available indices
   */
  function getAvailableIndices(&$grid)
  {
    return array('A','B','C','D','E','F','G','H','I','J','K','L','M',
                 'N','O','P','Q','R','S','T','U','V','W','X','Y','Z');    
  }
  
  /**
   * Returns an array with index links.
   *
   * @param atkDataGrid $grid 
   */
  function getLinks(&$grid)
  {
    $links = array();    
    
    $chars = $this->getAvailableIndices($grid);
    $index = $grid->getIndex();  
    $search = $grid->getSearch();
    $current = isset($search[$index]) ? $search[$index]{0} : NULL;
    
    // indices
    foreach ($chars as $char)
    {
      $title = $char;
      $url = $grid->createLink(array('atkstartat' => 0, 'atksearch' => array($index => "{$char}*"), 'atksinglesearchmode' => array($index => 'wildcard')));
      $links[] = array('type' => 'index', 'title' => $title, 'url' => $url, 'current' => $char == $current);
    }

    // view all
    if (count($search) > 0)
    {
      $title = $grid->text('view_all');               
      $url = $grid->createLink(array('atksearch' => ''));
      $links[] = array('type' => 'all', 'url' => $url, 'title' => $title);      
    }    
    
    return $links;
  }
  
  /**
   * Renders the index for the given data grid.
   *
   * @param atkDataGrid $grid the data grid
   * @return string rendered HTML
   */
  function render(&$grid)
  {
    $links = $this->getLinks($grid);
    
    $ui = &$grid->getUi();
    $result = $ui->render('dgindex.tpl', array('links' => $links));
    
    return $result;
  }
}
?>
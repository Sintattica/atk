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

atkimport('atk.datagrid.atkdatagrid');

/**
 * The data grid component base class. All data grid component extend this
 * class and implement the render method.
 *
 * @author Peter C. Verhage <peter@achievo.org>
 * @package atk
 * @subpackage datagrid
 */
abstract class atkDGComponent
{
  /**
   * The atkDataGrid.
   *
   * @var atkDataGrid
   */
  private $m_grid;
  
  /**
   * Constructor.
   *
   * @param atkDataGrid $grid
   */
  public function __construct($grid)
  {
    $this->m_grid = $grid;
  }
  
  /**
   * Returns the data grid.
   *
   * @return atkDataGrid data grid
   */
  protected function getGrid()
  {
    return $this->m_grid;
  }
  
  /**
   * Returns the data grid node.
   * 
   * @return atkNode node
   */
  protected function getNode()
  {
    return $this->getGrid()->getNode();
  }
  
  /**
   * Returns the page object.
   * 
   * @return atkPage page
   */
  protected function getPage()
  {
    return $this->getNode()->getPage();
  }
  
  /**
   * Returns the Ui object.
   * 
   * @return atkUi ui
   */
  protected function getUi()
  {
    return $this->getNode()->getUi();
  }
  
  /**
   * Translates the given string.
   */
  protected function text($string, $fallback='', $useDefault=false)
  {
    return $this->getGrid()->text($string, $fallback, $useDefault);
  }
  
  /**
   * Renders this component.
   */
  public abstract function render();
}
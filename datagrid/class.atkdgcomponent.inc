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
   * Returns the UI object.
   * 
   * @return atkUi ui
   */
  protected function getUi()
  {
    return $this->getNode()->getUi();
  }
  
  /**
   * Return the theme object.
   * 
   * @return atkTheme theme
   */
  protected function getTheme()
  {
    return atkinstance("atk.ui.atktheme");
  } 
  
  /**
   * Translate the given string using the grid node. 
   * 
   * The value of $fallback will be returned if no translation can be found.
   * If you want NULL to be returned when no translation can be found then
   * leave the fallback empty and set $useDefault to false.  
   *
   * @param string $string      string to translate
   * @param string $fallback    fallback in-case no translation can be found
   * @param boolean $useDefault use default ATK translation if no translation can be found?
   */
  protected function text($string, $fallback='', $useDefault=false)
  {
    return $this->getGrid()->text($string, $fallback, $useDefault);
  }
  
  /**
   * Renders the component.
   * 
   * @return string component HTML
   */
  public abstract function render();
}
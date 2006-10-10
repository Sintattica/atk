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
class atkDGComponent
{
  /**
   * Renders the data grid component.
   *
   * @param atkDataGrid $grid the data grid
   * @return string rendered HTML
   */
  function render(&$grid)
  {
    return "";
  }
}
?>
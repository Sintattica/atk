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
 * The data grid summary. Can be used to render a 
 * summary for an ATK data grid.
 *
 * @author Peter C. Verhage <peter@achievo.org>
 * @package atk
 * @subpackage datagrid
 */
class atkDGSummary extends atkDGComponent
{
  /**
   * Renders the summary for the given data grid.
   *
   * @param atkDataGrid $grid the data grid
   * @return string rendered HTML
   */
  function render(&$grid)
  {
    $limit = $grid->getLimit();
    $count = $grid->getCount();
	  $start = $grid->getOffset();
	  $end = min($start + $limit - 1, $count);

	  $string = $grid->text('datagrid_summary');
	  $params = array('start' => $start, 'end' => $end, 'count' => $count, 'limit' => $limit);

	  atkimport("atk.utils.atkstringparser");	  
	  $parser = new atkStringParser($string);
	  $result = $parser->parse($params);
	  
	  return $result;
  }
}
?>
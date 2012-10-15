<?php
/**
 * This file is part of the Achievo ATK distribution.
 * Detailed copyright and licensing information can be found
 * in the doc/COPYRIGHT and doc/LICENSE files which should be
 * included in the distribution.
 *
 * @package atk
 * @subpackage ui
 *
 * @copyright (c)2006 Ivo Jansch
 * @license http://www.achievo.org/atk/licensing ATK Open Source License
 *
 * @version $Revision: 4362 $
 * $Id$
 */

/**
 * Implements the {atknavigatorinfo} block plugin for use in templates.
 *
 * The navigator info plug-in can be used to easily generate a information about
 * pagination through page results (e.g. "item 10 - 20 of 40").
 *
 * Params: 
 * - name:      navigatorinfo name (name of the special variable inside the block)
 * - limit:     limit
 * - offset:    current offset
 * - count:     total number of items
 *
 * The special navigatorinfo variable which is available inside the block contains
 * information about the current pagination and contains the following properties:
 * 
 * - page_count: total number of pages with results
 * - page:       current page number
 * - start:      index of first item on this page
 * - end:        index of last item on this page
 * - count:      total number of items
 *
 * Example:
 * {atknavigatorinfo name='info' limit=10 offset=0 count=500}
 *   (Result {$info.start} - {$info.end} of {$info.count})
 * {/atknavigatorinfo}
 *
 * @author Peter C. Verhage <peter@ibuildings.nl>
 */
function smarty_block_atknavigatorinfo($params, $content, &$smarty, &$repeat)
{
  $name = isset($params['name']) ? $params['name'] : 'navigatorinfo';
  $offset = $params['offset'];
  $limit = $params['limit'];
  $count = $params['count'];
  
  $pageCount = ceil($count / $limit);
  $currentPage = ($offset / $limit) + 1;
  
  if ($repeat)
  {
    $data = array(
      'page_count' => $pageCount, 
      'page' => $currentPage, 
      'start' => $offset + 1, 
      'end' => min($offset + $limit, $count),
      'count' => $count
    );
    
    $smarty->assign($name, $data);
  }
  else 
  {
    $smarty->clear_assign($name);
    return $content;
  }
}
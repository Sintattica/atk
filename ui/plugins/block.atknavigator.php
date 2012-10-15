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
 * Implements the {atknavigator} block plugin for use in templates.
 *
 * The navigator plug-in can be used to easily generate a page navigation
 * bar for search results etc. The block's body is called for each page
 * and gives access to a special variable (default $navigation) which gives
 * access to the current's page properties.
 *
 * Params: 
 * - name:      navigator name (name of the special variable inside the block)
 * - limit:     limit
 * - offset:    current offset
 * - count:     total number of items
 * - max_pages: max. number of page links
 *
 * The special navigator variable which is available inside the block contains
 * information for the current page in the iteration and contains the following
 * properties:
 * 
 * - type:      preview / page / next
 * - page:      page number
 * - offset:    offset for this link
 * - isFirst:   first page?
 * - isLast:    last page?
 * - isCurrent: current page?
 *
 * Example:
 * {atknavigator name='nav' limit=10 offset=0 count=500 max_pages=10}
 *   <a href="?offset={$nav.offset}">
 *     {if $nav.type == 'previous'}&lt;&lt;{/if}
 *     {if $nav.type == 'page'}{$nav.page}{/if} 
 *     {if $nav.type == 'next'}&gt;&gt;{/if}
 *   </a>
 *   {if !$nav.isLast}|{/if}
 * {/atknavigator}
 *
 * @author Peter C. Verhage <peter@ibuildings.nl>
 */
function smarty_block_atknavigator($params, $content, &$smarty, &$repeat)
{
  static $loopPages = array();
  static $loopToggles = array();
  
  $name = isset($params['name']) ? $params['name'] : 'navigator';
  
  if (!isset($loopPages[$name]))
  {
    $offset = $params['offset'];
    $limit = $params['limit'];
    $count = $params['count'];
    $maxPages = isset($params['max_pages']) ? $params['max_pages'] : 10;
    if (empty($maxP))
  
    if (!($limit > 0 && $count > $limit && ceil($count / $limit) > 1))
    {
      $repeat = false;
      return;  
    }
  
    // calculate number of pages, first, current and last page
    $pageCount = ceil($count / $limit);
    $currentPage = ($offset / $limit) + 1;
    $firstPage = $currentPage - floor(($maxPages - 1) / 2);
    $lastPage = $currentPage + ceil(($maxPages - 1) / 2);

    if ($firstPage < 1)
    {
      $firstPage = 1;
      $lastPage = min($pageCount, $maxPages);
    }
  
    if ($lastPage > $pageCount)
    {
      $lastPage = $pageCount;
      $firstPage = max(1, $pageCount - $maxPages + 1);
    }  
    
    $pages = array();
    
    if ($currentPage > $firstPage)
    {
      $pages[] = array(
        'type' => 'previous',
        'page' => $currentPage - 1, 
        'offset' => max(0, ($currentPage - 2) * $limit),    
        'isFirst' => true,             
        'isLast' => false,
        'isCurrent' => false
      );
    }
    
    for ($i = $firstPage; $i <= $lastPage; $i++)
    {
      $pages[] = array(
        'type' => 'page',
        'page' => $i, 
        'offset' => max(0, ($i - 1) * $limit),    
        'isFirst' => count($pages) == 0,     
        'isLast' => $currentPage == $lastPage,
        'isCurrent' => $i == $currentPage
      );
    }
    
    if ($currentPage < $lastPage)
    {
      $pages[] = array(
        'type' => 'next',
        'page' => $currentPage + 1,         
        'offset' => max(0, $currentPage * $limit),    
        'isFirst' => false,             
        'isLast' => true,
        'isCurrent' => false
      );
    }
    
    $loopPages[$name] = $pages;
    $loopToggles[$name] = true;
    $repeat = true;
    return;
  }
  
  else if ($loopToggles[$name])
  {
    $page = array_shift($loopPages[$name]);
    $smarty->assign($name, $page);
    $loopToggles[$name] = false;    
    $repeat = true;
  }
  
  else 
  {
    $loopToggles[$name] = true;    
    
    $repeat = count($loopPages[$name]) > 0;
    if (!$repeat)
    {
      unset($loopPages[$name]);
      unset($loopToggles[$name]);  
    }
    
    return $content;
  }
}
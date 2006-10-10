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
 * The data grid paginator. Can be used to render pagination 
 * links for an ATK data grid.
 *
 * @author Peter C. Verhage <peter@achievo.org>
 * @package atk
 * @subpackage datagrid
 */
class atkDGPaginator extends atkDGComponent
{
  /**
   * Maximum number of pagination links (excluding
   * the previous and next links).
   *
   * @var int
   */
  var $m_maxLinks;

  
  /**
   * Constructor.
   *
   * @return atkDGPaginator
   */
  function atkDGPaginator()
  {
    $this->m_maxLinks = 10;
  }
  
  /**
   * Returns an array with pagination links.
   *
   * @param atkDataGrid $grid
   */
  function getLinks(&$grid)
  {
    $links = array();

    $count = $grid->getCount();        
    $limit = $grid->getLimit();
    $offset = $grid->getOffset();
    
    // no navigation links needed
    if ($limit == 0 || $count <= $limit) 
    {
      return $links;
    }
    
    // calculate pages, first and last page
    $pages = ceil($count / $limit);
    $current = floor($offset / $limit) + 1;
    $first = $current - (floor($this->m_maxLinks - 1) / 2);
    $last = $current + (ceil($this->m_maxLinks - 1) / 2);
    
    // fix invalid first page
    if ($first < 1)
    {
      $first = 1;
      $last = min($pages, $this->m_maxLinks);
    }
    
    // fix invalid last page
    if ($last > $pages)
    {
      $first = max(1, $pages - $this->m_maxLinks + 1);      
      $last = $pages;
    }
    
    // previous link
    if ($current > 1)
    {
      $title = $grid->text('previous');           
      $url = $grid->createLink(array('atkstartat' => $offset - $limit));
      $links[] = array('type' => 'previous', 'url' => $url, 'title' => $title);
    }

    // normal pagination links
    for ($i = $first; $i <= $last; $i++)
    {
      if ($i == $current)
      {
        $title = $i;
        $links[] = array('type' => 'page', 'title' => $i, 'current' => true);
      }
      else 
      {  
        $title = $i;              
        $url = $grid->createLink(array('atkstartat' => max(0, ($i - 1) * $limit)));
        $links[] = array('type' => 'page', 'url' => $url, 'title' => $title, 'current' => false);      
      }
    }

    // next link
    if ($current < $pages)
    {
      $title = $grid->text('next');         
      $url = $grid->createLink(array('atkstartat' => $offset + $limit));
      $links[] = array('type' => 'next', 'url' => $url, 'title' => $title);      
    }    
    
    return $links;
  }
  
  /**
   * Renders the paginator for the given data grid.
   *
   * @param atkDataGrid $grid the data grid
   * @return string rendered HTML
   */
  function render(&$grid)
  {
    $links = $this->getLinks($grid);
    
    $ui = &$grid->getUi();
    $result = $ui->render('dgpaginator.tpl', array('links' => $links));
    
    return $result;
  }
}
?>
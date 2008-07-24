<?php
/**
 * This file is part of the Achievo ATK distribution.
 * Detailed copyright and licensing information can be found
 * in the doc/COPYRIGHT and doc/LICENSE files which should be
 * included in the distribution.
 *
 * @package atk
 *
 * @copyright (c) 2007 Ibuildings.nl BV
 * @license http://www.achievo.org/atk/licensing ATK Open Source License
 *
 * @version $Revision$
 * $Id$
 */

/**
 * The ATK data node can be used to create nodes that don't retrieve their
 * data from the database.
 *
 * The data can either be provided using the setData method or the getData
 * method (and possibly other methods) can be overriden to provide the
 * data dynamically.
 *
 * @author Peter C. Verhage <peter@ibuildings.nl>
 * @package atk
 */
class atkDataNode extends atkNode
{
  /**
   * Data.
   *
   * @var array
   */
  private $m_data = array();

  /**
   * Constructor.
   *
   * @param string $type  node type (by default the class name)
   * @param int    $flags node flags
   *
   * @return atkDataNode
   */
  public function __construct($type='', $flags=0)
  {
    parent::__construct($type, $flags|NF_NO_ADD|NF_NO_EDIT);
    $this->setTable($this->m_type);
  }

  /**
   * Sets the data that this node should use.
   *
   * @param array $data data list
   */
  public function setData($data)
  {
    $this->m_data = $data;
  }

  /**
   * Returns the internal data.
   *
   * @param array criteria (can be ignored in which case filterData will filter the data)
   *
   * @return array data list
   */
  protected function getData($criteria=null)
  {
    return $this->m_data;
  }

  /**
   * Select records using the given criteria.
   *
   * @param string $selector selector string
   * @param string $order    order string
   * @param array  $limit    limit array
   *
   * @return array selected records
   */
  public function selectDb($selector=null, $order=null, $limit=null)
  {
    atkdebug(get_class($this).'::selectDb('.$selector.')');

    if ($order == null)
    {
      $order = $this->getOrder();
    }

    $params = array(
      'selector' => $selector,
      'order' => $order,
      'offset' => isset($limit['offset']) ? $limit['offset'] : 0,
      'limit' => isset($limit['limit']) ? $limit['limit'] : -1
    );

    $result = $this->findData($params);
    atkdebug('Result '.get_class($this).'::selectDb('.$selector.') => '.count($result).' row(s)');
    return $result;
  }

  /**
   * Returns how many records will be returned for the given selector.
   *
   * @param string $selector selector string
   *
   * @return int record count
   */
  public function countDb($selector=null)
  {
    $params = array(
      'selector' => $selector
    );

    return $this->countData($params);
  }
  
  /**
   * Count "rows".
   *
   * Supported parameters are: selector, limit, offset and order.
   * 
   * @param array $params parameters
   * 
   * @return int number of "records"
   */
  protected function countData($params=array())
  {
    return count($this->findData($params));    
  }

  /**
   * Find data using the given parameters.
   * Supported parameters are: selector, limit, offset and order.
   *
   * @param array $params parameters
   *
   * @return array found data
   */
  protected function findData($params=array())
  {
    $selector = @$params['selector'] ? $params['selector'] : '';
    $limit = @$params['limit'] ? $params['limit'] : -1;
    $offset = @$params['offset'] ? $params['offset'] : 0;
    $order = @$params['order'] ? $params['order'] : null;

    $selector = $this->getSelector($selector);
    $criteria = $this->getCriteria($selector);

    $data = $this->getData($criteria);
    $data = $this->filterColumns($data);
    $data = $this->filterData($data, $criteria);
    $data = $this->sortData($data, $order);
    $data = $this->limitData($data, $limit, $offset);
    return $data;
  }

  /**
   * Filter invalid columns.
   *
   * @param array $data data
   *
   * @return data
   */
  protected function filterColumns($data)
  {
    $result = array();

    foreach ($data as $row)
    {
      foreach (array_keys($row) as $column)
      {
        if ($this->getAttribute($column) == null)
        {
          unset($row[$column]);
        }
      }

      $result[] = $row;
    }

    return $result;
  }

  /**
   * Returns the full selector including added filters.
   *
   * @param string $selector selector
   *
   * @return string full selector string
   */
  protected function getSelector($selector)
  {
    $result = $selector;

    foreach ($this->m_fuzzyFilters as $filter)
    {
      if (!empty($result))
      {
        $result .= ' AND ';
      }

      $result .= $filter;
    }

    return $result;
  }

  /**
   * Translate the given selector to a criteria array
   * which key/values can be used to filter data.
   *
   * @param string $selector selector string
   *
   * @return array criteria
   */
  protected function getCriteria($selector)
  {
    $criteria = $this->m_filters;
    
    if (empty($selector))
    {
      return $criteria;
    }

    $keyValueSet = decodeKeyValueSet($selector);

    foreach ($keyValueSet as $column => $value)
    {
      $column = trim($column);
      $value = trim($value);

      if (strpos($column, '.') !== false)
      {
        list($table, $column) = explode('.', $column);
        if ($table != $this->getTable()) continue;
      }

      $value = stripslashes(stripQuotes($value));

      $criteria[$column] = $value;
    }

    return $criteria;
  }

  /**
   * Filter data using the given selector.
   *
   * @param array  $data     data list
   * @param string $criteria criteria list
   *
   * @return array filtered data
   */
  protected function filterData($data, $criteria)
  {
    $result = array();

    foreach ($data as $item)
    {
      $valid = true;
      foreach ($criteria as $key => $value)
      {
        $valid = $valid && $item[$key] == $value;
        if (!$valid) break;
      }

      if ($valid)
      {
        $result[] = $item;
      }
    }

    return $result;
  }

  /**
   * Parse the order to something we can use. If the order
   * is invalid false is returned.
   *
   * @param string $order order string
   * @return array|boolean array 1st element column, 2nd element ascending? or false
   */
  protected function translateOrder($order)
  {
    if (empty($order)) return false;

    list($column, $direction) = explode(' ', $order);

    if (strpos($column, '.') !== false)
    {
      list($table,$column) = explode('.', $column);
      if ($table != $this->getTable())
      {
        return false;
      }
    }

    $column = trim($column);
    $direction = trim($direction);

    $asc = $direction != 'desc';

    if ($this->getAttribute($column) != null)
    {
      return array($column, $asc);
    }

    return false;
  }

  /**
   * Sort data by the given order string.
   *
   * @param array  $data  data list
   * @param string $order order string
   *
   * @return array data list
   */
  protected function sortData($data, $order)
  {
    list($column, $asc) = $this->translateOrder($order);

    if ($column != false)
    {
      $attr = $this->getAttribute($column);

      if ($attr instanceof atkNumberAttribute)
      {
        usort($data, create_function('$a, $b', 'return $a["'.$column.'"] == $b["'.$column.'"] ? 0 : ($a["'.$column.'"] < $b["'.$column.'"] ? -1 : 1);'));
      }
      else
      {
        usort($data, create_function('$a, $b', 'return strcasecmp($a["'.$column.'"], $b["'.$column.'"]);'));
      }

      if (!$asc)
      {
        $data = array_reverse($data);
      }
    }

    return $data;
  }

  /**
   * Limit data using the given limit and offset.
   *
   * @param array $data   data list
   * @param int   $limit  limit
   * @param int   $offset offset
   *
   * @return array limitted data
   */
  protected function limitData($data, $limit=-1, $offset=0)
  {
    if ($limit >= 0)
    {
      $data = array_slice($data, $offset, $limit);
    }
    else
    {
      $data = array_slice($data, $offset);
    }

    return $data;
  }

  /**
   * Add is not supported.
   *
   * @return boolean false
   */
  public function addDb()
  {
    return false;
  }

  /**
   * Update is not supported.
   *
   * @return boolean false
   */
  public function updateDb()
  {
    return false;
  }

  /**
   * Delete is not supported.
   *
   * @return boolean false
   */
  public function deleteDb()
  {
    return false;
  }
  
  /**
   * Don't fetch meta data.
   */
  public function setAttribSizes()
  {
  }
}
<?php

/**
 * Fluent interface helper class for retrieving records from a node. PHP5 only.
 *
 * @author Peter C. Verhage <peter@ibuildings.nl>
 * @version $Revision$
 */
class atkSelector implements ArrayAccess, Countable, SeekableIterator
{
  /**
   * This selector's node.
   *
   * @var atkNode
   */
  protected $m_node;
  
  protected $m_selector = null;
  protected $m_distinct = false;
  protected $m_mode = '';
  protected $m_order = '';
  protected $m_limit = -1;
  protected $m_offset = 0;
  protected $m_excludes = '';
  protected $m_includes = '';
  
  protected $m_rows = null;
  protected $m_rowCount = null;
  
  /**
   * Constructor.
   *
   * @param atkNode $node     this selector's node
   * @param string  $selector selector string
   */
  public function __construct($node, $selector=null)
  {
    $this->m_node = $node;
    $this->m_selector = $selector;
  }
  
  /**
   * Sets the selector.
   * 
   * @param string $selector where clause
   * @return atkSelector
   */
  public function where($selector)
  {
    $this->m_selector = $selector;
    return $this;
  }
  
  /**
   * Distinct selection?
   * 
   * @param boolean $distinct distinct selection?
   * @return atkSelector
   */
  public function distinct($distinct)
  {
    $this->m_distinct = $distinct;
    return $this;
  }
  
  /**
   * Set the select mode.
   * 
   * @param string $mode select mode
   * @return atkSelector
   */
  public function mode($mode)
  {
    $this->m_mode = $mode;
    return $this;
  }
  
  /**
   * Order by the given order by string.
   *
   * @param string $order order by string
   * @return atkSelector
   */
  public function orderBy($order)
  {
    $this->m_order = $order;
    return $this;    
  }
  
  /**
   * Limit the results bij the given limit (and from the optional offset).
   *
   * @param int $limit  limit
   * @param int $offset offset
   * @return atkSelector
   */
  public function limit($limit, $offset=0)
  {
    $this->m_limit = $limit;
    $this->m_offset = $offset;
    return $this;    
  }
  
  /**
   * Include only the following list of attributes.
   *
   * @param array $includes list of includes
   * @return atkSelector
   */
  public function includes($includes)
  {
    $this->m_includes = $includes;
    return $this;    
  }
  
  
  /**
   * Exclude the following list of attributes.
   *
   * @param array $excludes list of excludes
   * @return atkSelector
   */  
  public function excludes($excludes)
  {
    $this->m_excludes = $excludes;
    return $this;
  }
  
  /**
   * Returns the first found row.
   *
   * @return array first row
   */
  public function firstRow()
  {
    list($row) = $this->limit(1, $this->m_offset)->all();
    return $row;
  }
  
  /**
   * Return all rows.
   * 
   * @return array all rows
   */
  public function allRows()
  {
    if ($this->m_rows === null)
    {
      $this->m_rows = $this->m_node->selectDb($this->m_selector, $this->m_order, array('limit' => $this->m_limit, 'offset' => $this->m_offset), $this->m_excludes, $this->m_includes, $this->m_mode, $this->m_distinct);
    }
    
    return $this->m_rows;
  }
  
  /**
   * Return row count.
   *
   * @return int row count
   */
  public function rowCount()
  {
    if ($this->m_rowCount === null)
    {
      $this->m_rowCount = (int)$this->m_node->countDb($this->m_selector, $this->m_excludes, $this->m_includes, $this->m_mode, $this->m_distinct);
    }
    
    return $this->m_rowCount;
  }
  
  /**
   * Does the given offset exist?
   *
   * @param string|int $key key
   * @return boolean offset exists?
   */
  public function offsetExists($key)
  {
    $this->allRows();
    return isset($this->m_rows[$key]);
  }
  
  /**
   * Returns the given offset.
   *
   * @param string|int $key key
   * @return mixed
   */
  public function offsetGet($key)
  {
    $this->allRows();    
    return $this->m_rows[$key];
  }
  
  /**
   * Sets the value for the given offset.
   *
   * @param string|int $key
   * @param mixed $value
   */
  public function offsetSet($key, $value)
  {
    $this->allRows();    
    return $this->m_rows[$key] = $value;
  }
  
  /**
   * Unset the given element.
   *
   * @param string|int $key
   */
  public function offsetUnset($key)
  {
    $this->allRows();    
    unset($this->m_rows[$key]);
  }
  
  /**
   * Returns the current row.
   *
   * @return array|boolean current row (false if no current row)
   */
  public function current()
  {
    $this->allRows();    
    return current($this->m_rows);
  }
  
  /**
   * Returns the current key.
   *
   * @return array|boolean current key (false if no current key)
   */
  public function key()
  {
    $this->allRows();    
    return key($this->m_rows);    
  }
  
  /**
   * Moves the internal pointer to the next row and
   * returns the new current row (false if no current row).
   *
   * @return array|boolean next row
   */
  public function next()
  {
    $this->allRows();    
    return next($this->m_rows);    
  }
  
  /**
   * Reset to the first row and return.
   *
   * @return array|boolean first row (false if no current row)
   */
  public function rewind()
  {
    $this->allRows();    
    return reset($this->m_rows);    
  }
  
  /**
   * Is the pointer set to a valid row?
   *
   * @return boolean valid row?
   */
  public function valid()
  {
    $this->allRows();    
    return current($this->m_rows) !== false;
  }
  
  /**
   * Seek to the given index.
   *
   * @param int $index seek index
   */
  public function seek($index) 
  {
    $this->rewind();
    $position = 0;
    
    while ($position < $index && $this->valid()) {
      $this->next();
      $position++;
    }
    
    if (!$this->valid()) 
    {
      throw new OutOfBoundsException('Invalid seek position');
    }
  }  
  
  /**
   * Returns the row count (used when calling count on an atkSelector object,
   * don't use this if you want to efficiently retrieve the row count using
   * a count() select statement, use rowCount instead!
   *
   * @return int row count
   */
  public function count()
  {
    $this->allRows();
    return count($this->m_rows);
  }
}
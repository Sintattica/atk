<?php

/**
 * Fluent interface helper class for retrieving records from a node. PHP5 only.
 *
 * @author Peter C. Verhage <peter@ibuildings.nl>
 * @version $Revision$
 */
class atkSelector
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
  public function first()
  {
    list($row) = $this->limit(1, $this->m_offset)->all();
    return $row;
  }
  
  /**
   * Return all rows.
   * 
   * @return array all rows
   */
  public function all()
  {
    return $this->m_node->selectDb($this->m_selector, $this->m_order, array('limit' => $this->m_limit, 'offset' => $this->m_offset), $this->m_excludes, $this->m_includes, $this->m_mode, $this->m_distinct);
  }
  
  /**
   * Return row count.
   *
   * @return int row count
   */
  public function count()
  {
    return (int)$this->m_node->countDb($this->m_selector, $this->m_excludes, $this->m_includes, $this->m_mode, $this->m_distinct);
  }
}
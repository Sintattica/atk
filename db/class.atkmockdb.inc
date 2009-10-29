<?php
  /**
   * This file is part of the Achievo ATK distribution.
   * Detailed copyright and licensing information can be found
   * in the doc/COPYRIGHT and doc/LICENSE files which should be
   * included in the distribution.
   *
   * @package atk
   * @subpackage db
   *
   * @copyright (c)2006 Ibuildings.nl BV
   * @license http://www.achievo.org/atk/licensing ATK Open Source License
   *
   * @version $Revision: 6311 $
   * $Id$
   */

  /**
   * A mock database driver.
   *
   * This is a dummy database driver that can be used in testcases. The
   * results that should be returned upon a call to getrows() can be
   * influenced.
   *
   * @author Boy Baukema <boy@ibuildings.nl>
   * @package atk
   * @subpackage db
   */
  class atkMockDb extends atkDb
  {
    var $m_type = "mock";
    var $m_results = array();
    var $m_regex_results = array();
    var $m_query_history = array();
    var $m_next_ids = array();
    
    private $m_currentResult = null;
    

    /**
     * Connect to the database.
     * @return int Connection status
     * @abstract
     */
    function connect()
    {
      return true;
    }

    /**
     * Parse and execute a query.
     *
     * If the query is a select query, the rows can be retrieved using the
     * next_record() method.
     *
     * @param string $query The SQL query to execute
     * @param int $offset Retrieve the results starting at the specified
     *                    record number. Pass -1 or 0 to start at the first
     *                    record.
     * @param int $limit Indicates how many rows to retrieve. Pass -1 to
     *                   retrieve all rows.
     * @abstract
     */
    function query($query, $offset=-1, $limit=-1)
    {
      $this->m_query_history[] = $query;
      
      $this->m_row = 0;
      $this->m_currentResult = null;
      $result = null;
      
      if (isset($this->m_results[$offset][$limit][$query])) 
      {
        $result = $this->m_results[$offset][$limit][$query];
      }
      else if (!empty($this->m_regex_results))
      {
        foreach ($this->m_regex_results[$offset][$limit] as $regex => $res)
        {         
          if (preg_match($regex, $query)) 
          {
            $result = $res;
            break;
          }
        }
      }
      
      if (is_array($result))
      {
        $this->m_currentResult = $result;
      }
      else if (isset($result))
      {
        return $result;
      }
      else
      {
        return true;
      }
    }


    /**
     * Retrieve the next record in the resultset.
     * 
     * @return mixed An array containing the record, or false if there are no more
     *               records to retrieve.
     */
    public function next_record()
    {
      if ($this->m_currentResult !== null && count($this->m_currentResult) > 0)
      {
        $record = array_shift($this->m_currentResult);
        $this->m_record = $record;
        $this->m_row++;
        return $record;
      }
      else
      {
        return false;
      }
    }
    
    /**
     * Set the result for the query (to mock executing a query)
     *
     * @param mixed $result
     * @param string $query
     * @param int $offset
     * @param int $limit
     */
    function setResult($result, $query, $offset=-1, $limit=-1)
    {
      $this->m_results[$offset][$limit][$query] = $result;
    }

    /**
     * Set the regex result for the query
     *
     * @param mixed $result
     * @param string $regex
     * @param int $offset
     * @param int $limit
     */
    function setRegexResult($result, $regex, $offset=-1, $limit=-1)
    {
      $this->m_regex_results[$offset][$limit][$regex] = $result;
    }
    
    /**
     * Retrieve the query history.
     *
     * @return Array
     */
    function getQueryHistory()
    {
      return $this->m_query_history;
    }
    
    /**
     * Clear the query history.
     */
    function clearQueryHistory()
    {
      $this->m_query_history = array();
    }
    
    /**
     * Set the next id
     *
     * @param string $sequence
     * @param int $nextid
     */
    function setNextId($sequence, $nextid)
    {
      $this->m_next_ids[$sequence] = $nextid;
    }
    
    /**
     * returns nextid
     * When the sequence isn't set the value 1 is returned.
     *
     * @param string $sequence
     * @return int The nextid
     */
    function nextid($sequence)
    {
      if(array_key_exists($sequence, $this->m_next_ids))
      {
        return $this->m_next_ids[$sequence];
      }
      else
      {
        return 1;
      }
    }
    
    /**
     * Clear the nextids array
     *
     */
    function clearNextId()
    {
      $this->m_next_ids = array();
    }
  }

?>

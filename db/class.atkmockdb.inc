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
   * @version $Revision$
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
     * @param String $query The SQL query to execute
     * @param int $offset Retrieve the results starting at the specified
     *                    record number. Pass -1 or 0 to start at the first
     *                    record.
     * @param int $limit Indicates how many rows to retrieve. Pass -1 to
     *                   retrieve all rows.
     * @abstract
     */
    function query($query, $offset=-1, $limit=-1)
    {
      atk_var_dump($query,'inatkmockdb teh query');
      $this->m_query_history[] = $query;
      
      if (isset($this->m_results[$offset][$limit][$query])) return $this->m_results[$offset][$limit][$query];
      if (!empty($this->m_regex_results))
      {
        foreach ($this->m_regex_results[$offset][$limit] as $regex => $result)
        {         
          if (preg_match($regex, $query)) return $result;        
        }
      }
      return true;
    }


    /**
     * Get all rows that are the result
     * of a certain specified query
     *
     * Note: This is not an efficient way to retrieve
     * records, as this will load all records into one
     * array into memory. If you retrieve a lot of records,
     * you might hit the memory_limit and your script will die.
     *
     * @param $query the query
     * @return array with rows
     */
    function getrows($query, $offset=-1, $limit=-1)
    {
      $res = $this->query($query, $offset, $limit);
      if (is_array($res))
        return $res;
      return array();
    }

    function setResult($result, $query, $offset=-1, $limit=-1)
    {
      $this->m_results[$offset][$limit][$query] = $result;
    }

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
    
    function setNextId($sequence, $nextid)
    {
      $this->m_next_ids[$sequence] = $nextid;
    }
    
    /**
     * returns nextid
     * When the sequence isn't set the value 1 is returned.
     *
     * @param integer
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
    
    function clearNextId()
    {
      $this->m_next_ids = array();
    }
  }

?>
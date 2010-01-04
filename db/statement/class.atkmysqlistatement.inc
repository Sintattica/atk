<?php
/**
 * This file is part of the Achievo ATK distribution.
 * Detailed copyright and licensing information can be found
 * in the doc/COPYRIGHT and doc/LICENSE files which should be
 * included in the distribution.
 *
 * @package atk
 * @subpackage db.statement
 *
 * @copyright (c) 2009 Peter C. Verhage
 * @license http://www.achievo.org/atk/licensing ATK Open Source License
 *
 * @version $Revision$
 * $Id$
 */
atkimport('atk.db.statement.atkstatement');

/**
 * MySQLi statement implementation.
 * 
 * @author Peter C. Verhage <peter@achievo.org>
 *
 * @package atk
 * @subpackage db.statement
 */
class atkMySQLiStatement extends atkStatement
{
  /**
   * MySQLi statement.
   * 
   * @var MySQLi_Statement
   */
  private $m_stmt;
  
  /**
   * Column names.
   * 
   * @var array
   */
  private $m_columnNames = null;
  
  /**
   * Row value bindings.
   * 
   * @var array
   */
  private $m_values = null;
  
  /**
   * Prepares the statement for execution.
   */  
  protected function _prepare()
  {
    if ($this->getDb()->connect() != DB_SUCCESS)
    {
      throw new atkStatementException("Cannot connect to database.", atkStatementException::NO_DATABASE_CONNECTION);  
    }
    
    $conn = $this->getDb()->link_id();
    atkdebug("Prepare query: ".$this->_getParsedQuery());
    $this->m_stmt = $conn->prepare($this->_getParsedQuery());
    if (!$this->m_stmt || $conn->errno)
    {
      throw new atkStatementException("Cannot prepare statement (ERROR: {$conn->errno} - {$conn->error}).", atkStatementException::PREPARE_STATEMENT_ERROR);
    }
  }  
  
  /**
   * Moves the cursor back to the beginning of the result set.
   * 
   * NOTE:
   * Depending on the database driver, using this method might result in the
   * query to be executed again.
   */
  public function rewind()
  {
    if ($this->_getLatestParams() === null)
    {
      throw new atkStatementException("Statement has not been executed yet.", atkStatementException::STATEMENT_NOT_EXECUTED);
    }
    
    $this->m_stmt->data_seek(0);
  }   
 
  /**
   * Bind statement parameters.
   * 
   * @param array $params parameters
   */
  private function _bindParams($params)
  {
    if (count($params) == 0)
    {
      return;
    }
    
    $i = 0;
    $args = array();
    $args[] = str_repeat('s', count($this->_getBindPositions()));
    foreach ($this->_getBindPositions() as $param)
    { 
      atkdebug("Bind param {$i}: ".($params[$param] === null ? 'NULL' : $params[$param]));
      $args[] = &$params[$param];
      $i++;
    }  
    
    call_user_func_array(array($this->m_stmt, 'bind_param'), $args);
  }

  /**
   * Store the column names from this statement's metadata.
   */
  private function _storeColumnNames()
  {
    if ($this->m_columnNames !== null)
    {
      // already stored on previous execution
      return;
    }
    
    $metadata = $this->m_stmt->result_metadata();
    if ($this->m_stmt->errno)
    {
      throw new atkStatementException("Cannot retrieve metadata (ERROR: {$this->m_stmt->errno} - {$this->m_stmt->error}).", atkStatementException::OTHER_ERROR);
    }
    
    if (!$metadata)
    {
      // no result set (INSERT, UPDATE, DELETE, ... queries)
      return;
    }
    
    $this->m_columnNames = array();
    foreach ($metadata->fetch_fields() as $column) 
    {
      $this->m_columnNames[] = $column->name;
    }
  }
  
  /**
   * Bind result columns to values array so we can read the result when
   * fetching rows.
   */
  private function _bindResult()
  {
    $this->_storeColumnNames();
    if ($this->m_columnNames === null)
    {
      // no result set (INSERT, UPDATE, DELETE, ... queries)
      return;
    }

    $this->m_values = array();
    $refs = array();

    for ($i = 0; $i < count($this->m_columnNames); $i++)
    {
      $this->m_values[$i] = null;
      $refs[$i] = &$this->m_values[$i];
    }

    $this->m_stmt->store_result();
    call_user_func_array(array($this->m_stmt, 'bind_result'), $refs);
  }
  
  /**
   * Executes the statement using the given bind parameters.
   * 
   * @param array $params bind parameters
   */
  protected function _execute($params)
  {
    if (atkconfig("debug") >= 0)
    {
      atkimport("atk.utils.atkdebugger");
      atkDebugger::addQuery($this->_getParsedQuery(), false);
    }
    
    $this->_bindParams($params);
    
    if (!$this->m_stmt->execute())
    {
      throw new atkStatementException("Cannot execute statement.", atkStatementException::STATEMENT_ERROR);
    }
    
    $this->m_insertId = $this->getDb()->link_id()->insert_id;
    
    $this->_bindResult();
    
    if ($this->m_columnNames === null)
    {
      $this->getDb()->debugWarnings();
    }
  }
  
  /**
   * Fetches the next row from the result set.
   * 
   * @return array next row from the result set (false if no other rows exist)
   */
  protected function _fetch()
  {
    if (!$this->m_stmt->fetch())
    {
      return false;
    }

    $values = array();
    foreach ($this->m_values as $value) 
    {
      $values[] = $value;
    }    
    
    $row = array_combine($this->m_columnNames, $values);
    return $row;
  }
  
  /**
   * Resets the statement so that it can be re-used again.
   */
  protected function _reset()
  {
    @$this->m_stmt->free_result();
    @$this->m_stmt->reset();
  }

  /**
   * Frees up all resources for this statement. The statement cannot be
   * re-used anymore.
   */
  public function _close()
  {
    @$this->m_stmt->close();
  }
  
  /**
   * Returns the number of affected rows in case of an INSERT, UPDATE 
   * or DELETE query. Called immediatly after atkStatement::_execute().
   */
  protected function _getAffectedRowCount()
  {  
    return $this->m_stmt->affected_rows;
  }
  
  /**
   * Returns the auto-generated id used in the last query.
   * 
   * @return int auto-generated id
   */
  public function getInsertId()
  {
    return $this->m_insertId;
  }
}
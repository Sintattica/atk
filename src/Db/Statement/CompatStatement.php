<?php

namespace Sintattica\Atk\Db\Statement;

use Sintattica\Atk\Core\Tools;
use Sintattica\Atk\Db\Db;

/**
 * Base statement class used for database drivers which don't have their own
 * implementation.
 *
 * Bind parameters are replaced by their value before executing the query
 * using the database driver. This means you can still use bind parameters but
 * you don't have the performance benefits or real prepared statement.
 *
 * @author Peter C. Verhage <peter@achievo.org>
 */
class CompatStatement extends Statement
{
    /**
     * Query resource.
     *
     * @var mixed
     */
    private $m_resource;

    /**
     * Prepares the statement for execution.
     */
    protected function _prepare()
    {
        if ($this->getDb()->connect() != Db::DB_SUCCESS) {
            throw new StatementException('Cannot connect to database.', StatementException::NO_DATABASE_CONNECTION);
        }

        Tools::atkdebug('Prepare query: '.$this->_getParsedQuery());
    }

    /**
     * Replace the bind parameters in the parsed query with their escaped values.
     *
     * @param array $params parameters
     *
     * @return string query
     */
    protected function _bindParams($params)
    {
        $query = $this->_getParsedQuery();
        Tools::atkdebug('Binding parameters for query: '.$this->_getParsedQuery());

        foreach (array_values($this->_getBindPositions()) as $i => $param) {
            Tools::atkdebug("Bind param {$i}: ".($params[$param] === null ? 'NULL' : $params[$param]));
        }

        foreach (array_reverse($this->_getBindPositions(), true) as $position => $param) {
            $query = substr($query, 0, $position).($params[$param] === null ? 'NULL' : "'".$this->getDb()->escapeSQL($params[$param])."'").substr($query,
                    $position + 1);
        }

        return $query;
    }

    /**
     * Executes the statement using the given bind parameters.
     *
     * @param array $params bind parameters
     *
     * @throws StatementException
     */
    protected function _execute($params)
    {
        // replace the bind parameters with their values
        $query = $this->_bindParams($params);

        // store the current query resource
        $oldId = $this->getDb()->getQueryId();
        $oldHaltOnError = $this->getDb()->getHaltOnError();

        // execute the query using the driver
        $this->getDb()->setHaltOnError(false);
        $result = $this->getDb()->query($query);

        // retrieve and reset the query resource so we can use it later on and
        // the database driver won't free/close it unless we want it
        $this->m_resource = $this->getDb()->getQueryId();
        $this->getDb()->resetQueryId();

        // restore the old query resource
        $this->getDb()->setQueryId($oldId);
        $this->getDb()->setHaltOnError($oldHaltOnError);

        if (!$result) {
            $this->m_resource = null;
            throw new StatementException('Cannot execute statement: '.$query, StatementException::STATEMENT_ERROR);
        }
    }

    /**
     * Fetches the next row from the result set.
     *
     * @return array|false next row from the result set (false if no other rows exist)
     */
    protected function _fetch()
    {
        // store the current query resource    
        $oldId = $this->getDb()->getQueryId();

        // set our own query resource in the database driver so we can use
        // the Db::next_record() method and retrieve the new record
        $this->getDb()->setQueryId($this->m_resource);
        $this->getDb()->next_record();
        $row = $this->getDb()->m_record;

        // restore the old query resource
        $this->getDb()->setQueryId($oldId);

        return $row;
    }

    /**
     * Resets the statement so that it can be re-used again.
     */
    protected function _reset()
    {
        $this->m_resource = null;
    }

    /**
     * Frees up all resources for this statement. The statement cannot be
     * re-used anymore.
     */
    public function _close()
    {
        // There is no proper way to do this which is compatible with all drivers
        // because there is no Db::free() method. We could retrieve all rows,
        // which automatically closes the current resource, but that's not very 
        // efficient. We can also execute a dummy query but the same query should
        // work for all drivers... So instead we simply leak a resource which is
        // cleaned at the end of the execution of this PHP script. 
        $this->m_resource = null;
    }

    /**
     * Returns the number of affected rows in case of an INSERT, UPDATE
     * or DELETE query. Called immediately after Statement::_execute().
     */
    protected function _getAffectedRowCount()
    {
        // store the current query resource    
        $oldId = $this->getDb()->getQueryId();

        // set our own query resource in the database driver so we can use
        // the Db::affected_rows() method and retrieve the affected row count
        $this->getDb()->setQueryId($this->m_resource);
        $result = $this->getDb()->affected_rows();

        // restore the old query resource
        $this->getDb()->setQueryId($oldId);

        return $result;
    }
}

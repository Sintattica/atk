<?php

namespace Sintattica\Atk\Db\Statement;

use mysqli_stmt;
use Sintattica\Atk\Core\Tools;
use Sintattica\Atk\Db\MySqliDb;
use Sintattica\Atk\Utils\Debugger;
use Sintattica\Atk\Core\Config;
use Sintattica\Atk\Db\Db;

/**
 * MySQLi statement implementation.
 *
 * @author Peter C. Verhage <peter@achievo.org>
 */
class MySqliStatement extends Statement
{
    /**
     * MySQLi statement.
     */
    private mysqli_stmt $m_stmt;
    private ?array $m_columnNames = null;
    private ?array $m_values = null;
    private $m_insertId;

    /**
     * Prepares the statement for execution.
     * @throws StatementException
     */
    protected function _prepare(): void
    {
        if ($this->getDb()->connect() !== Db::DB_SUCCESS) {
            throw new StatementException('Cannot connect to database.', StatementException::NO_DATABASE_CONNECTION);
        }

        $query = $this->_getParsedQuery();
        $conn = $this->getDb()->link_id();
        Tools::atkdebug("Prepare query: $query");
        $stmt = mysqli_prepare($conn, $query);

        if (!$stmt && $conn->errno === 2006) {
            // retry
            Tools::atkdebug('DB has gone away, try to reconnect...');
            $this->getDb()->disconnect();
            if ($this->getDb()->connect() !== Db::DB_SUCCESS) {
                throw new StatementException('Cannot connect to database.', StatementException::NO_DATABASE_CONNECTION);
            }
            $conn = $this->getDb()->link_id();
            Tools::atkdebug("Prepare query after reconnection: $query");
            $stmt = mysqli_prepare($conn, $query);
        }

        if ($stmt === false) {
            throw new StatementException("Cannot prepare the statement (Query: $query).", StatementException::PREPARE_STATEMENT_ERROR);
        }

        if ($conn->errno) {
            throw new StatementException("Cannot prepare statement (ERROR: $conn->errno - $conn->error).", StatementException::PREPARE_STATEMENT_ERROR);
        }

        $this->m_stmt = $stmt;
    }

    /**
     * Moves the cursor back to the beginning of the result set.
     *
     * NOTE:
     * Depending on the database driver, using this method might result in the
     * query to be executed again.
     * @throws StatementException
     */
    public function rewind()
    {
        if ($this->_getLatestParams() === null) {
            throw new StatementException('Statement has not been executed yet.', StatementException::STATEMENT_NOT_EXECUTED);
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
        if (Tools::count($params) == 0) {
            return;
        }

        $i = 0;
        $args = [];
        $args[] = str_repeat('s', Tools::count($this->_getBindPositions()));
        foreach ($this->_getBindPositions() as $param) {
            Tools::atkdebug("Bind param {$i}: " . ($params[$param] === null ? 'NULL' : $params[$param]));
            $args[] = &$params[$param];
            ++$i;
        }

        call_user_func_array(array($this->m_stmt, 'bind_param'), $args);
    }

    /**
     * Store the column names from this statement's metadata.
     * @throws StatementException
     */
    private function _storeColumnNames()
    {
        if ($this->m_columnNames !== null) {
            // already stored on previous execution
            return;
        }

        $metadata = $this->m_stmt->result_metadata();
        if ($this->m_stmt->errno) {
            throw new StatementException("Cannot retrieve metadata (ERROR: {$this->m_stmt->errno} - {$this->m_stmt->error}).", StatementException::OTHER_ERROR);
        }

        if (!$metadata) {
            // no result set (INSERT, UPDATE, DELETE, ... queries)
            return;
        }

        $this->m_columnNames = [];
        foreach ($metadata->fetch_fields() as $column) {
            $this->m_columnNames[] = $column->name;
        }
    }

    /**
     * Bind result columns to values array so we can read the result when
     * fetching rows.
     * @throws StatementException
     */
    private function _bindResult()
    {
        $this->_storeColumnNames();
        if ($this->m_columnNames === null) {
            // no result set (INSERT, UPDATE, DELETE, ... queries)
            return;
        }

        $this->m_values = [];
        $refs = [];

        for ($i = 0; $i < Tools::count($this->m_columnNames); ++$i) {
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
     *
     * @throws StatementException
     */
    protected function _execute($params)
    {
        if (Config::getGlobal('debug') >= 0) {
            Debugger::addQuery($this->_getParsedQuery(), false);
        }

        $this->_bindParams($params);

        if (!$this->m_stmt->execute()) {
            throw new StatementException("Cannot execute statement: {$this->m_stmt->error}", StatementException::STATEMENT_ERROR);
        }

        $this->m_insertId = $this->getDb()->link_id()->insert_id;

        $this->_bindResult();

        if ($this->m_columnNames === null) {
            /** @var MySqliDb $db */
            $db = $this->getDb();
            $db->debugWarnings();
        }
    }

    /**
     * Fetches the next row from the result set.
     *
     * @return array|false next row from the result set (false if no other rows exist)
     */
    protected function _fetch()
    {
        if (!$this->m_stmt->fetch()) {
            return false;
        }

        $values = [];
        foreach ($this->m_values as $value) {
            $values[] = $value;
        }

        return array_combine($this->m_columnNames, $values);
    }

    /**
     * Resets the statement so that it can be re-used again.
     */
    protected function _reset(): void
    {
        @$this->m_stmt->free_result();
        @$this->m_stmt->reset();
    }

    /**
     * Frees up all resources for this statement. The statement cannot be
     * re-used anymore.
     */
    public function _close(): void
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

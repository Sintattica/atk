<?php

namespace Sintattica\Atk\Db\Statement;

use IteratorAggregate;
use Sintattica\Atk\Db\Db;
use Sintattica\Atk\Core\Tools;
use Traversable;

/**
 * A statement can be used to execute a query.
 *
 * The query can be re-used, e.g. executed multiple times, and may contain bind
 * parameters. Both named and anonymous bind parameters are supported, but
 * can't be mixed together. Named bind parameters are in the form of ":name",
 * anonymous bind parameters are simply represented by a "?".
 *
 * When fetching rows for a given query you can either use an iterator
 * (efficient one-by-one retrieval of rows) or one of the convenience methods
 * (e.g. getFirstRow, getAllRows, ...).
 *
 * To create an instance please use the Db::prepare($query) method.
 *
 * Example:
 * $stmt = Db::getInstance()->prepare("SELECT COUNT(*) FROM people WHERE birthday > :birthday");
 * $stmt->execute(array('birthday' => '1985-09-20'));
 * foreach ($stmt as $person)
 * {
 *   echo "{$person['firstname']} {$person['lastname'}\n";
 * }
 * $stmt->close();
 *
 * @author Peter C. Verhage <peter@achievo.org>
 */
abstract class Statement implements IteratorAggregate
{
    /**
     * (Original) SQL query.
     *
     * @var string query
     */
    private $m_query;

    /**
     * Parsed SQL query.
     *
     * @var string
     */
    private $m_parsedQuery;

    /**
     * Positions of bind parameters.
     *
     * @var array
     */
    private $m_bindPositions;

    /**
     * Current row offset position.
     *
     * @var int
     */
    private $m_position = false;

    /**
     * Latest parameters supplied to the execute() method.
     *
     * @var array
     */
    private $m_latestParams = [];

    /**
     * @var int
     */
    private $m_affectedRowCount = 0;


    public $errno;

    public $error;

    public $m_db;

    /**
     * Constructs a new statement for the given query.
     *
     * @param Db $db database instance
     * @param string $query SQL query
     */
    public function __construct($db, $query)
    {
        $this->m_db = $db;
        $this->m_query = $query;
        $this->_parse();
        $this->_prepare();
    }

    /**
     * Destructor.
     */
    public function __destruct()
    {
        $this->close();
    }

    /**
     * Parses the query. Named bind parameters are replaced by anonymous bind
     * parameters and the positions of the different named/anonymous bind
     * parameters are made available for later use.
     */
    protected function _parse()
    {
        $parser = new StatementParser($this->getQuery());
        $this->m_parsedQuery = $parser->getParsedQuery();
        $this->m_bindPositions = $parser->getBindPositions();
    }

    /**
     * Returns the database instance.
     *
     * @return Db database instance
     */
    public function getDb()
    {
        return $this->m_db;
    }

    /**
     * Returns the query on which this statement is based.
     *
     * @return string query
     */
    public function getQuery()
    {
        return $this->m_query;
    }

    /**
     * Returns the parsed query for this statement (e.g. named bind parameters
     * are replaced by anonymous bind parameters).
     *
     * @return string
     */
    protected function _getParsedQuery()
    {
        return $this->m_parsedQuery;
    }

    /**
     * Returns the positions for the bind parameters in the query.
     *
     * The key of the array contains the character position, the value
     * contains the bind parameter name or offset.
     *
     * @return array bind positions
     */
    protected function _getBindPositions()
    {
        return $this->m_bindPositions;
    }

    /**
     * Get latest execution parameters.
     *
     * @return array execution parameters
     */
    protected function _getLatestParams()
    {
        return $this->m_latestParams;
    }

    /**
     * Prepares the statement for execution.
     */
    abstract protected function _prepare(): void;

    /**
     * Executes the statement using the given bind parameters.
     *
     * @param array $params bind parameters
     */
    abstract protected function _execute(array $params): void;

    /**
     * Fetches the next row from the result set.
     *
     * @return array|false next row from the result set (false if no other rows exist)
     */
    abstract protected function _fetch(): false|array;

    /**
     * Resets the statement so that it can be re-used again.
     */
    abstract protected function _reset(): void;

    /**
     * Frees up all resources for this statement. The statement cannot be
     * re-used anymore.
     */
    abstract protected function _close(): void;

    /**
     * Returns the number of affected rows in case of an INSERT, UPDATE
     * or DELETE query. Called immediately after Statement::_execute().
     */
    abstract protected function _getAffectedRowCount();

    /**
     * Resets this statement so that it can be re-used again.
     */
    public function reset(): void
    {
        $this->m_position = false;
        $this->m_latestParams = null;
        $this->_reset();
    }

    /**
     * Close this statement.
     *
     * Frees all resources after which this statement cannot be used anymore.
     * If you want to re-use the statement, use the Statement::reset() method.
     */
    public function close(): void
    {
        $this->m_position = false;
        $this->m_latestParams = null;
        $this->_reset();
        $this->_close();
    }

    /**
     * Moves the cursor back to the beginning of the result set.
     *
     * NOTE:
     * Depending on the database driver, using this method might result in the
     * query to be executed again.
     * @throws StatementException
     */
    public function rewind(): void
    {
        if ($this->_getLatestParams() === null) {
            throw new StatementException('Statement has not been executed yet.', StatementException::STATEMENT_NOT_EXECUTED);
        }

        if ($this->m_position !== false) {
            $this->m_position = false;
            $this->execute($this->_getLatestParams());
        }
    }

    /**
     * Validates if all bind parameters are supplied.
     *
     * @param array $params bind parameters
     *
     * @throws StatementException on Missing bind parameter
     */
    protected function _validateParams($params): void
    {
        foreach ($this->_getBindPositions() as $position => $param) {
            if (!array_key_exists($param, $params)) {
                throw new StatementException('Missing bind parameter '.(!is_numeric($param) ? ':' : '').$param.'.', StatementException::MISSING_BIND_PARAMETER);
            }
        }
    }

    /**
     * Executes the statement.
     *
     * @param array $params bind parameters
     * @throws StatementException
     */
    public function execute(array $params = []): void
    {
        $this->reset();
        $this->_validateParams($params);
        $this->_execute($params);
        $this->m_latestParams = $params;
        $this->m_affectedRowCount = $this->_getAffectedRowCount();
    }

    /**
     * Fetches the next row from the result set.
     *
     * @return array|false next row or false if there are no more rows
     *@throws StatementException
     *
     */
    public function fetch(): array|false
    {
        if ($this->_getLatestParams() === null) {
            throw new StatementException('Statement has not been executed yet.', StatementException::STATEMENT_NOT_EXECUTED);
        }

        $result = $this->_fetch();

        if ($result) {
            $this->m_position = $this->m_position !== false ? $this->m_position + 1 : 0;
        }

        return $result;
    }

    /**
     * Returns an iterator for iterating over the result rows for this statement.
     *
     * NOTE:
     * Depending on the database driver, using this method multiple times might
     * result in the query to be executed multiple times.
     *
     * @return StatementIterator iterator
     * @throws StatementException
     */
    public function getIterator(): Traversable
    {
        $this->rewind();

        return new StatementIterator($this);
    }

    /**
     * Returns the first row.
     *
     * NOTE:
     * Depending on the database driver, using this method multiple times might
     * result in the query to be executed multiple times.
     *
     * @return array|null row
     * @throws StatementException
     */
    public function getFirstRow(): ?array
    {
        $this->rewind();

        if ($row = $this->fetch()) {
            return $row;
        }

        return null;
    }

    /**
     * Get all rows for the given query.
     *
     * NOTE:
     * This is not an efficient way to retrieve records, as this will load all
     * records into an array in memory. If you retrieve a lot of records, you
     * are better of using Statement::getIterator which only retrieves one
     * row at a time.
     *
     * Depending on the database driver, using this method multiple times might
     * result in the query to be executed multiple times.
     *
     * @return array rows
     */
    public function getAllRows()
    {
        return $this->getAllRowsAssoc(null);
    }

    /**
     * Get rows in an associative array with the given column used as key for the rows.
     *
     * NOTE:
     * This is not an efficient way to retrieve records, as this will load all
     * records into an array in memory. If you retrieve a lot of records, you
     * are better of using Statement::getIterator which only retrieves one
     * row at a time.
     *
     * Depending on the database driver, using this method multiple times might
     * result in the query to be executed multiple times.
     *
     * @param int|string $keyColumn column index / name (default first column) to be used as key
     *
     * @return array rows
     */
    public function getAllRowsAssoc($keyColumn = 0)
    {
        $this->rewind();

        $result = [];

        for ($i = 0; $row = $this->fetch(); ++$i) {
            if ($keyColumn === null) {
                $key = $i;
            } else {
                if (is_numeric($keyColumn)) {
                    $key = Tools::atkArrayNvl(array_values($row), $keyColumn);
                } else {
                    $key = $row[$keyColumn];
                }
            }

            $result[$key] = $row;
        }

        return $result;
    }

    /**
     * Get the value of the first (or the given) column of the first row in the result.
     *
     * NOTE:
     * Depending on the database driver, using this method multiple times might
     * result in the query to be executed multiple times.
     *
     * @param int|string $valueColumn column index / name (default first column) to be used as value
     * @param mixed $fallback fallback value if no result
     *
     * @return mixed first value
     */
    public function getFirstValue($valueColumn = 0, $fallback = null)
    {
        $row = $this->getFirstRow();

        if ($row == null) {
            return $fallback;
        } else {
            if (is_numeric($valueColumn)) {
                return Tools::atkArrayNvl(array_values($row), $valueColumn);
            } else {
                return $row[$valueColumn];
            }
        }
    }

    /**
     * Get an array with all the values in the specified column.
     *
     * NOTE:
     * This is not an efficient way to retrieve records, as this will load all
     * records into an array in memory. If you retrieve a lot of records, you
     * are better of using Statement::getIterator which only retrieves one
     * row at a time.
     *
     * Depending on the database driver, using this method multiple times might
     * result in the query to be executed multiple times.
     *
     * @param int|string $valueColumn column index / name (default first column) to be used as value
     *
     * @return array with values
     */
    public function getAllValues($valueColumn = 0)
    {
        return $this->getAllValuesAssoc(null, $valueColumn);
    }

    /**
     * Get rows in an associative array with the given key column used as
     * key and the given value column used as value.
     *
     * NOTE:
     * This is not an efficient way to retrieve records, as this will load all
     * records into an array in memory. If you retrieve a lot of records, you
     * are better of using Statement::getIterator which only retrieves one
     * row at a time.
     *
     * Depending on the database driver, using this method multiple times might
     * result in the query to be executed multiple times.
     *
     * @param int|string $keyColumn column index / name (default first column) to be used as key
     * @param int|string $valueColumn column index / name (default first column) to be used as value
     *
     * @return array rows
     */
    public function getAllValuesAssoc($keyColumn = 0, $valueColumn = 1)
    {
        $rows = $this->getAllRowsAssoc($keyColumn);
        foreach ($rows as $key => &$value) {
            if (is_numeric($valueColumn)) {
                $value = Tools::atkArrayNvl(array_values($value), $valueColumn);
            } else {
                $value = $value[$valueColumn];
            }
        }

        return $rows;
    }

    /**
     * Returns the number of affected rows in case of an INSERT, UPDATE
     * or DELETE query.
     */
    public function getAffectedRowCount()
    {
        if ($this->_getLatestParams() === null) {
            throw new StatementException('Statement has not been executed yet.', StatementException::STATEMENT_NOT_EXECUTED);
        }

        return $this->m_affectedRowCount;
    }
}

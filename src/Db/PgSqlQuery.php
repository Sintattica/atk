<?php namespace Sintattica\Atk\Db;

/**
 * SQL query builder for PostgreSQL.
 *
 * @author Peter C. Verhage <peter@ibuildings.nl>
 * @package atk
 * @subpackage db
 *
 */
class PgSqlQuery extends Query
{

    /**
     * Makes a join SQL query for PostgreSQL
     * @param string $table the table name
     * @param string $alias alias for the table
     * @param string $condition join condition
     * @param bool $outer Wether to use an outer (left) join or an inner join
     * @return Query The query object (for fluent usage)
     */
    public function &addJoin($table, $alias, $condition, $outer)
    {
        if ($outer) {
            $join = "LEFT JOIN ";
        } else {
            $join = "JOIN ";
        }
        $this->m_joins[] = " ".$join.$table." ".$alias." ON ".$condition." ";
    }

    /**
     * Add limiting clauses to the query.
     * Default implementation: no limit supported. Derived classes should implement this.
     *
     * @param string $query The query to add the limiter to
     */
    public function _addLimiter(&$query)
    {
        if ($this->m_offset >= 0 && $this->m_limit > 0) {
            $query .= " LIMIT ".$this->m_limit." OFFSET ".$this->m_offset;
        }
    }

    /**
     * Builds the SQL Select COUNT(*) query. This is different from select,
     * because we do joins, like in a select, but we don't really select the
     * fields.
     *
     * @param bool $distinct distinct rows?
     *
     * @return String a SQL Select COUNT(*) Query
     */
    public function buildCount($distinct = false)
    {
        $query = "SELECT COUNT(*) AS count FROM (".$this->buildSelect($distinct).") x";

        return $query;
    }
}

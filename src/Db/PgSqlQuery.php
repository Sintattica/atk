<?php

namespace Sintattica\Atk\Db;
use Sintattica\Atk\Core\Tools;

/**
 * SQL query builder for PostgreSQL.
 *
 * @author Peter C. Verhage <peter@ibuildings.nl>
 */
class PgSqlQuery extends Query
{
    /**
     * Makes a join SQL query for PostgreSQL.
     *
     * @param string $table the table name
     * @param string $alias alias for the table
     * @param string $condition join condition
     * @param bool $outer Wether to use an outer (left) join or an inner join
     *
     * @return Query The query object (for fluent usage)
     */
    public function addJoin($table, $alias, $condition, $outer = false)
    {
        if ($outer) {
            $join = 'LEFT JOIN ';
        } else {
            $join = 'JOIN ';
        }
        $this->m_joins[] = ' '.$join.$table.' '.$alias.' ON '.$condition.' ';
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
            $query .= ' LIMIT '.$this->m_limit.' OFFSET '.$this->m_offset;
        }
    }

    /**
     * Builds the SQL Select COUNT(*) query. This is different from select,
     * because we do joins, like in a select, but we don't really select the
     * fields.
     *
     * @param bool $distinct distinct rows?
     *
     * @return string a SQL Select COUNT(*) Query
     */
    public function buildCount($distinct = false)
    {
        $query = 'SELECT COUNT(*) AS count FROM ('.$this->buildSelect($distinct).') x';

        return $query;
    }

    /**
     * Generate a searchcondition that checks whether $field contains $value .
     *
     * @param string $field The field
     * @param string $value The value
     *
     * @return string The substring condition
     */
    public function substringCondition($field, $value)
    {
        if ($this->getDb()->getForceCaseInsensitive()) {
            if ($value[0] == '!') {
                return $field." NOT ILIKE '%".substr($value, 1, Tools::atk_strlen($value))."%'";
            }
            return $field." ILIKE '%".$value."%'";
        } else {
            if ($value[0] == '!') {
                return $field." NOT LIKE '%".substr($value, 1, Tools::atk_strlen($value))."%'";
            }
            return $field." LIKE '%".$value."%'";
        }
    }

    /**
     * Generate a searchcondition that accepts '*' as wildcard character.
     *
     * @param string $field
     * @param string $value
     */
    public function wildcardCondition($field, $value)
    {
        if ($this->getDb()->getForceCaseInsensitive()) {
            if ($value[0] == '!') {
                return $field." NOT ILIKE '".str_replace('*', '%', substr($value, 1, Tools::atk_strlen($value)))."'";
            }
            return $field." ILIKE '".str_replace('*', '%', $value)."'";
        } else {
            if ($value[0] == '!') {
                return $field." NOT LIKE '".str_replace('*', '%', substr($value, 1, Tools::atk_strlen($value)))."'";
            }
            return $field." LIKE '".str_replace('*', '%', $value)."'";
        }
    }

    public function exactBoolCondition($field, $value)
    {
        $value = $value ? 'true' : 'false';

        return "$field = $value";
    }
}

<?php

namespace Sintattica\Atk\Db;

use Sintattica\Atk\Core\Tools;

/**
 * SQL query builder
 *
 * @author Ivo Jansch <ivo@achievo.org>
 * @author Samuel BF
 */
class Query
{
    /**
     * Array with Fieldnames (quoted)
     *
     * @var array[] string
     */
    public $m_fields;

    /*
     * Array with expressions.
     */
    public $m_expressions;

    /**
     * Table name for current query (unquoted)
     *
     * @var string
     */
    public $m_table;

    /*
     * Array with conditions (quoted)
     *
     * @var QueryPart[]
     */
    public $m_conditions;
    public $m_searchconditions;

    /*
     * Var with AND or OR method
     *
     * @var string
     */
    public $m_searchmethod;

    /*
     * Array with field aliases (unquoted, but no need to quote them)
     */
    private $m_fieldaliases;

    /*
     * Array with aliases from joins (quoted)
     */
    public $m_joinaliases;

    /*
     * Array with Joins
     */
    public $m_joins;

    /*
     * Array with group by statements (quoted)
     */
    public $m_groupbys;

    /*
     * Array with order by statements (quoted)
     */
    public $m_orderbys;

    /*
     * Do we need to perform a DISTINCT query?
     */
    public $m_distinct = false;

    /*
     * Do we need to fetch only a specific set of records?
     */
    public $m_offset = 0;
    public $m_limit = 0;

    /*
     * Array with generated aliasses (quoted)
     * Oracle has a problem when aliases are too long
     */
    public $m_generatedAlias;

    /*
     * The database that this query does it's thing on
     */
    public $m_db;

    /**
     * Reference to the field where the new sequence
     * value should be stored.
     *
     * @var int
     */
    protected $m_seqValue;

    /**
     * Sequence name.
     *
     * @var string
     */
    protected $m_seqName;

    /**
     * Should we return a sequence value by setting
     * $this->m_seqValue?
     *
     * @var bool
     */
    protected $m_returnSeqValue = false;

    /**
     * Initialize all variables.
     */
    public function __construct()
    {
        $this->m_fields = [];
        $this->m_expressions = [];
        $this->m_table = '';
        $this->m_conditions = [];
        $this->m_searchconditions = [];
        $this->m_values = [];
        $this->m_fieldaliases = [];
        $this->m_joinaliases = [];
        $this->m_joins = [];
        $this->m_orderbys = [];
        $this->m_groupbys = [];
        $this->m_searchmethod = '';

        // start at 'a'.
        $this->m_generatedAlias = 'a';

        $this->m_aliasLookup = [];
    }

    /****************************** Getter / setters *****************************************/
    /**
     * Sets the database instance.
     *
     * @var Db database instance
     */
    public function setDb($db)
    {
        $this->m_db = $db;
    }

    /**
     * Returns the database instance.
     *
     * @return Db database instance
     */
    private function getDb()
    {
        if (!isset($this->m_db)) {
            $this->m_db = Db::getInstance();
        }

        return $this->m_db;
    }

    /**
     * Add's a field to the query.
     *
     * @param string $name Field name
     * @param string $value Field value
     * @param string $table Table name
     * @param string $fieldaliasprefix Field alias prefix
     *
     * @return Query The query object itself (for fluent usage)
     */
    public function addField($name, $value = '', $table = '', $fieldaliasprefix = '')
    {
        if ($table != '') {
            $fieldname = Db::quoteIdentifier($table).'.'.Db::quoteIdentifier($name);
        } else {
            $fieldname = Db::quoteIdentifier($name);
        }
        if (!in_array($fieldname, $this->m_fields)) {
            $this->m_fields[] = $fieldname;
        }

        $this->m_values[$fieldname] = $value;

        if ($fieldaliasprefix != '') {
            $this->m_aliasLookup['al_'.$this->m_generatedAlias] = $fieldaliasprefix.$name;
            $this->m_fieldaliases[$fieldname] = 'al_'.$this->m_generatedAlias;

            ++$this->m_generatedAlias;
        }

        return $this;
    }

    /**
     * Add a * to the field list (select all fields)
     *
     * @param string $table of the fields. If not set, will select fields from table and all joins.
     *
     * @return Query the query object itself
     */
    public function addAllFields($table = null)
    {
        if ($table) {
            $this->m_fields[] = Db::quoteIdentifier($table).'.*';
        } else {
            $this->m_fields[] = '*';
        }
        return $this;
    }

    /**
     * Add's a sequence field to the query.
     *
     * @param string $fieldName field name
     * @param int $value field to store the new sequence value in, note certain drivers
     *                          might populate this field only after the insert query has been
     *                          executed
     * @param string $seqName sequence name to store the value if the DB does not autoincrement
     *                         field by itself.
     *
     * @return Query
     */
    public function addSequenceField($fieldName, &$value, $seqName = null)
    {
        $meta = $this->getDb()->tableMeta($this->m_table);
        if (!Tools::hasFlag($meta[$fieldName]['flags'], Db::MF_AUTO_INCREMENT)) {
            $value = $this->getDb()->nextid($seqName);
            $this->addField($fieldName, $value, null, null, false, true);
            return $this;
        }

        $this->m_seqValue = &$value;
        $this->m_seqValue = -1;
        $this->m_seqName = $this->m_table.'_'.$fieldName.'_seq';
        $this->m_returnSeqValue = true;

        return $this;
    }

    /**
     * Add multiple fields at once.
     *
     * @param array $fields array with field value pairs
     * @param string $table Table name
     * @param string $fieldaliasprefix Field alias prefix
     * @param bool $quote If this parameter is true, stuff is inserted into the db
     *                                 using quotes, e.g. SET name = 'piet'. If it is false, it's
     *                                 done without quotes, e.d. SET number = 4.
     *
     * @return Query The query object itself (for fluent usage)
     */
    public function addFields(array $fields, $table = '', $fieldaliasprefix = '', $quote = true)
    {
        foreach ($fields as $name => $value) {
            $this->addField($name, $value, $table, $fieldaliasprefix, $quote);
        }

        return $this;
    }

    /**
     * Add's an expression to the select query.
     *
     * @param string $fieldName expression field name
     * @param string $expression expression value
     * @param string $fieldAliasPrefix field alias prefix
     *
     * @return Query The query object itself (for fluent usage)
     */
    public function addExpression($fieldName, $expression, $fieldAliasPrefix = '')
    {
        $this->m_expressions[] = ['name' => $fieldAliasPrefix.$fieldName, 'expression' => $expression];

        if (!empty($fieldAliasPrefix)) {
            $this->m_aliasLookup['al_'.$this->m_generatedAlias] = $fieldAliasPrefix.$fieldName;
            $this->m_fieldaliases[$fieldAliasPrefix.$fieldName] = 'al_'.$this->m_generatedAlias;
            ++$this->m_generatedAlias;
        }

        return $this;
    }

    /**
     * Clear field list.
     */
    public function clearFields()
    {
        $this->m_fields = [];
    }

    /**
     * Clear expression list.
     */
    public function clearExpressions()
    {
        $this->m_expressions = [];
    }

    /**
     * Set the table on which the query will be executed.
     *
     * @param string $name Table name
     *
     * @return Query The query object itself (for fluent usage)
     */
    public function setTable($name)
    {
        $this->m_table = $name;
        return $this;
    }

    /**
     * Add a table to current query.
     *
     * @deprecated : use setTable
     *
     * @param string $name Table name
     * @param string $alias Alias of table
     *
     * @return Query The query object itself (for fluent usage)
     */
    public function addTable($name, $alias = '')
    {
        Tools::atkwarning('Query->addTable deprecated. Use setTable().');
        return $this->setTable($name);
    }

    /**
     * Add join to Join Array.
     *
     * @param string $table Table name
     * @param string $alias Alias of table
     * @param string $condition Condition for the Join
     * @param bool $outer Wether to use an outer (left) join or an inner join
     *
     * @return Query The query object itself (for fluent usage)
     */
    public function addJoin($table, $alias, $condition, $outer = false)
    {
        $join = ' '.($outer ? 'LEFT JOIN ' : 'JOIN ').Db::quoteIdentifier($table).' '.Db::quoteIdentifier($alias).' ON ('.$condition.') ';
        if (!in_array($join, $this->m_joins)) {
            $this->m_joins[] = $join;
        }

        return $this;
    }

    /**
     * Add a group-by statement.
     *
     * @param string $element Group by expression
     *
     * @return Query The query object itself (for fluent usage)
     */
    public function addGroupBy($element)
    {
        $this->m_groupbys[] = $element;

        return $this;
    }

    /**
     * Add order-by statement.
     *
     * @param string $element Order by expression
     *
     * @return Query The query object itself (for fluent usage)
     */
    public function addOrderBy($element)
    {
        $this->m_orderbys[] = $element;

        return $this;
    }

    /**
     * Sets this queries search method.
     *
     * @param string $searchMethod search method
     *
     * @return Query
     */
    public function setSearchMethod($searchMethod)
    {
        $this->m_searchmethod = $searchMethod;

        return $this;
    }

    /**
     * Add a query condition (conditions are where-expressions that are AND-ed).
     *
     * @param QueryPart|string $query sql condition
     * @param array $parameters for the condition (only if $query is not already au QueryPart)
     *
     * @return Query The query object itself (for fluent usage)
     */
    public function addCondition($query, $parameters = [])
    {
        if (!$query instanceof QueryPart) {
            if ($query == '') {
                return;
            }
            $query = new QueryPart($query, $parameters);
        }

        $this->m_conditions[] = $query;
        return $this;
    }

    /**
     * Add search condition to the query. Basically similar to addCondition, but
     * searchconditions make use of the searchmode setting to determine whether the
     * different searchconditions should be and'ed or or'ed.
     *
     * @param QueryPart|string $query condition
     * @param array $parameters for the condition (only if $query is not already a QueryPart)
     *
     * @return Query The query object itself (for fluent usage)
     */
    public function addSearchCondition($query, $parameters = [])
    {
        if (!$query instanceof QueryPart) {
            if ($query == '') {
                return;
            }
            $query = new QueryPart($query, $parameters);
        }

        $this->m_searchconditions[] = $query;
        return $this;
    }

    /**
     * Set the 'distinct' mode for the query.
     * If set to true, a 'SELECT DISTINCT' will be performed. If set to false,
     * a regular 'SELECT' will be performed.
     *
     * @param bool $distinct Set to true to perform a distinct select,
     *                       false for a regular select.
     *
     * @return Query The query object itself (for fluent usage)
     */
    public function setDistinct($distinct)
    {
        $this->m_distinct = $distinct;

        return $this;
    }

    /**
     * Set a limit to the number of results.
     *
     * @param int $offset Retrieve records starting with record ...
     * @param int $limit Retrieve only this many records.
     *
     * @return Query The query object itself (for fluent usage)
     */
    public function setLimit($offset, $limit)
    {
        $this->m_offset = $offset;
        $this->m_limit = $limit;

        return $this;
    }

    /******************************** Helper functions ****************************************/
    /**
     * Search Alias in alias array.
     *
     * @param array $record Array with fields
     */
    public function deAlias(&$record)
    {
        foreach ($record as $name => $value) {
            if (isset($this->m_aliasLookup[$name])) {
                $record[$this->m_aliasLookup[$name]] = $value;
                unset($record[$name]);
            }
        }
    }

    /**
     * build the WHERE part of the query with conditions and searchconditions
     *
     * @return QueryPart to append to current query
     */
    private function whereClause()
    {
        $query = new QueryPart('');
        if (!empty($this->m_conditions)) {
            $query->appendSql(' WHERE (');
            $query->append(QueryPart::implode(') AND (', $this->m_conditions));
            $query->appendSql(')');
        }

        if (!empty($this->m_searchconditions)) {
            $query->appendSql(empty($this->m_conditions) ? 'WHERE (':'AND (');
            $searchOperator = ($this->m_searchmethod == '' || $this->m_searchmethod == 'AND') ? 'AND':'OR';
            $query->append(QueryPart::implode($searchOperator, $this->m_searchconditions));
            $query->appendSql(')');
        }

        return $query;
    }

    /**
     * Add limiting clauses to the query.
     *
     * @return QueryPart limiter clause (which can be appended)
     */
    private function limiterClause()
    {
        if ($this->m_offset >= 0 && $this->m_limit > 0) {
            return new QueryPart(
                'LIMIT :limit OFFSET :offset',
                [':limit' => [(int) $this->m_limit, \PDO::PARAM_INT], ':offset' => [(int) $this->m_offset, \PDO::PARAM_INT]]
                );
        }
    }

    /**
     * Add the ORDER BY clause.
     *
     * @return QueryPart order clause (which can be appended)
     */
    private function orderByClause()
    {
        if (empty($this->m_orderbys)) {
            return new QueryPart('');
        }
        return new QueryPart('ORDER BY ' . implode(', ', $this->m_orderbys));
        
    }

    /******************************** Builder/executer functions *****************************/

    /**
     * Builds the SQL Select query.
     *
     * @param bool $distinct distinct records?
     *
     * @return QueryPart a SQL Select Query
     */
    public function buildSelect($distinct = false)
    {
        if (empty($this->m_fields) && empty($this->m_expressions)) {
            return false;
        }
        $query = new QueryPart('SELECT'.($distinct || $this->m_distinct ? ' DISTINCT' : ''));
        
        // Fields and expressions
        $fieldAndAliasFn = function ($field) {
            if (isset($this->m_fieldaliases[$field])) {
                return "{$field} AS {$this->m_fieldaliases[$field]}";
            } else {
                return $field;
            }
        };
        $fieldsAndAlias = array_map($fieldAndAliasFn, $this->m_fields);
        
        $exprAndAliasFn = function($expression) {
            return "({$expression['expression']}) AS ".
                ($this->m_fieldaliases[$expression['name']] ?? Db::quoteIdentifier($expression['name']));
        };
        $exprsAndAlias = array_map($exprAndAliasFn, $this->m_expressions);
        
        $query->appendSql(implode(', ', array_merge($fieldsAndAlias, $exprsAndAlias)));

        $query->appendSql('FROM '.Db::quoteIdentifier($this->m_table));
        $query->appendSql(implode(' ', $this->m_joins));

        $query->append($this->whereClause());

        if (!empty($this->m_groupbys)) {
           $query->appendSql(' GROUP BY '.implode(', ', $this->m_groupbys));
        }

        if (!empty($this->m_orderbys)) {
            $query->append($this->orderByClause());
        }

        if ($this->m_limit > 0) {
            $query->append($this->limiterClause());
        }

        return $query;
    }

    /**
     * Builds the SQL Select COUNT(*) query. This is different from select,
     * because we do joins, like in a select, but we don't really select the
     * fields.
     *
     * @return string a SQL Select COUNT(*) Query
     */
    public function buildCount($distinct = false)
    {
        $query = new QueryPart('');
        if (($distinct || $this->m_distinct) && !empty($this->m_fields)) {
            $query->appendSql('SELECT COUNT(DISTINCT ');
            $query->appendSql(implode(', ', $this->m_fields));
            $query->appendSql(') as count FROM');
        } else {
            $query->appendSql('SELECT COUNT(*) AS count FROM');
        }
        
        $query->appendSql(Db::quoteIdentifier($this->m_table));
        $query->appendSql(implode(' ', $this->m_joins));

        $query->append($this->whereClause());

        if (!empty($this->m_groupbys)) {
           $query->appendSql(' GROUP BY '.implode(', ', $this->m_groupbys));
        }

        return $query;
    }

    /**
     * Wrapper function to execute a select query.
     *
     * @param bool $distinct Set to true to perform a distinct select,
     *                       false for a regular select.
     *
     * @return array The set of records returned by the database.
     */
    public function executeSelect($distinct = false)
    {
        $query = $this->buildSelect($distinct);

        return $this->getDb()->getRows($query);
    }

    /**
     * Builds the SQL Update query.
     *
     * @return string a SQL Update Query
     */
    public function buildUpdate()
    {
        $query = new QueryPart('UPDATE '.Db::quoteIdentifier($this->m_table).' SET');

        $updateFieldFn = function($field) {
            $placeholder = QueryPart::placeholder($field);
            $value = $this->m_values[$field];
            return new QueryPart("{$field}={$placeholder}", [$placeholder => [$value]]);
        };
        $query->append(QueryPart::implode(', ', array_map($updateFieldFn, $this->m_fields)));

        $query->append($this->whereClause());

        return $query;
    }

    /**
     * Wrapper function to execute an update query.
     */
    public function executeUpdate()
    {
        $query = $this->buildUpdate();

        return $this->getDb()->queryP($query);
    }

    /**
     * Builds the SQL Insert query.
     *
     * @return string a SQL Insert Query
     */
    public function buildInsert()
    {
        $query = new QueryPart('INSERT INTO '.Db::quoteIdentifier($this->m_table).'');

        $query->appendSql('('.implode(', ', $this->m_fields).') VALUES (');
        foreach ($this->m_fields as $field) {
            $placeholder = QueryPart::placeholder($field);
            $parameters[$placeholder] = [$this->m_values[$field]];
        }
        $query->append(new QueryPart(implode(', ', array_keys($parameters)), $parameters));
        $query->appendSql(')');

        return $query;
    }

    /**
     * Wrapper function to execute an insert query.
     */
    public function executeInsert()
    {
        $query = $this->buildInsert();

        $result = $this->getDb()->queryP($query);
        if ($result && $this->m_returnSeqValue) {
            $this->m_seqValue = $this->getDb()->lastInsertId($this->m_seqName);
            Tools::atkdebug("Value for sequence {$this->m_seqName}: {$this->m_seqValue}");
        }

        return $result;
    }

    /**
     * Builds the SQL Delete query.
     *
     * @return string a SQL Delete Query
     */
    public function buildDelete()
    {
        $query = new QueryPart('DELETE FROM '.Db::quoteIdentifier($this->m_table));
        $query->append($this->whereClause());
        return $query;
    }

    /**
     * Wrapper function to execute a delete query.
     */
    public function executeDelete()
    {
        $query = $this->buildDelete();

        return $this->getDb()->queryP($query);
    }

    /**************************** Search functions **********************************************
     * General form :
     * public function myCondition(string $field, mixed $value)
     *
     * @param string $field name that should be quoted BEFORE calling this function
     * @param mixed $value (0 to 2 values depending the search function)
     *
     * @return QueryPart
     ********************************************************************************************/

    /**
     * Generate a searchcondition that checks if the field is null.
     *
     * @param string $field
     * @param bool $emptyStringIsNull
     *
     * @return QueryPart
     */
    public function nullCondition($field, $emptyStringIsNull = false)
    {
        $result = "$field IS NULL";
        if ($emptyStringIsNull) {
            $result = "($result OR $field = '')";
        }

        return new QueryPart($result);
    }

    /**
     * Generate a searchcondition that checks if the field is not null.
     *
     * @param string $field
     * @param bool $emptyStringIsNull
     *
     * @return QueryPart
     */
    public function notNullCondition($field, $emptyStringIsNull = false)
    {
        $result = "$field IS NOT NULL";
        if ($emptyStringIsNull) {
            $result = "($result AND $field != '')";
        }

        return new QueryPart($result);
    }

    /**
     * Generate a searchcondition that check boolean value
     *
     * @param string $field full qualified table column
     * @param mixed $value integer/float/double etc.
     *
     * @return QueryPart
     */
    public function exactBoolCondition($field, $value)
    {
        $value = $value ? 'true' : 'false';

        return new QueryPart("$field = $value");
    }

    /**
     * Conditions of type "field [OPERATOR] value" (OPERATOR like '<', '>', 'LIKE'...)
     *
     * this is the meta for (greater|lower)than(equal)?Condition functions
     *
     * @param string $field The database field
     * @param string $value The value
     * @param string $positiveOp ('>', '<',...) when value does not start with '!'
     * @param string $negativeOp ('>', '<',...) when value starts with '!'
     *
     * @return QueryPart
     */
    private function simpleCondition($field, $value, $positiveOp, $negativeOp)
    {
        $placeholder = QueryPart::placeholder($field);
        $operator = ($value[0] == '!') ? $negativeOp:$positiveOp;
        if ($value[0] == '!') {
            $value = substr($value, 1);
        }
        $parameter = [$placeholder => [$value]];
        
        if ($this->getDb()->getForceCaseInsensitive()) {
            return new QueryPart("LOWER({$field}) {$operator} LOWER({$placeholder})", $parameter);
        } else {
            return new QueryPart("{$field} {$operator} {$placeholder}", $parameter);
        }
    }

    /**
     * Generate a searchcondition that checks whether $value matches $field exactly.
     *
     * @param string $field full qualified table column
     * @param mixed $value string/number/decimal expected column value
     * @param string $dbFieldType help determine exact search method
     *
     * @return QueryPart
     */
    public function exactCondition($field, $value, $dbFieldType = null)
    {
        if ($dbFieldType == Db::FT_NUMBER) {
            $value = (int) $value;
        }
        return $this->simpleCondition($field, $value, '=', '!=');
    }

    /**
     * Generate a searchcondition that accepts '*' as wildcard character.
     *
     * @param string $field
     * @param string $value
     *
     * @return string
     */
    public function wildcardCondition($field, $value)
    {
        return $this->simpleCondition($field, str_replace('*', '%', $value), 'LIKE', 'NOT LIKE');
    }

    /**
     * Generate a searchcondition that checks whether $field contains $value .
     *
     * @param string $field The field
     * @param string $value The value
     *
     * @return QueryPart
     */
    public function substringCondition($field, $value)
    {
        return $this->simpleCondition($field, '%'.str_replace('%', '%%', $value).'%', 'LIKE', 'NOT LIKE');
    }

    /**
     * Generate searchcondition with greater than.
     *
     * @param string $field The database field
     * @param string $value The value
     *
     * @return QueryPart
     */
    public function greaterthanCondition($field, $value)
    {
        return simpleCondition($field, $value, '>', '<=');
    }

    /**
     * Generate searchcondition with greater than.
     *
     * @param string $field The database field
     * @param string $value The value
     *
     * @return string
     */
    public function greaterthanequalCondition($field, $value)
    {
        return $this->simpleCondition($field, $value, '>=', '<');
    }

    /**
     * Generate searchcondition with less than.
     *
     * @param string $field The database field
     * @param string $value The value
     *
     * @return QueryPart
     */
    public function lessthanCondition($field, $value)
    {
        return $this->simpleCondition($field, $value, '<=', '>');
    }

    /**
     * Generate searchcondition with less than.
     *
     * @param string $field The database field
     * @param string $value The value
     *
     * @return QueryPart
     */
    public function lessthanequalCondition($field, $value)
    {
        return $this->simpleCondition($field, $value, '<=', '>');
    }

    /**
     * Get the between condition.
     *
     * @param string $field The database field
     * @param mixed $value1 The first value
     * @param mixed $value2 The second value
     * @param int $dbfieldtype from Db::FT_ constants
     *
     * @return QueryPart
     */
    public function betweenCondition($field, $value1, $value2, $dbfieldtype)
    {
        $placeholder = QueryPart::placeholder($field);
        $parameters = [$placeholder.'_min' => [$value1], $placeholder.'_max' => [$value2]];

        return new QueryPart("{$field} BETWEEN {$placeholder}_min AND {$placeholder}_max", $parameters);
    }

    /**
     * Generate an SQL searchcondition for a regular expression match.
     *
     * @param string $field The fieldname on which the regular expression
     *                        match will be performed.
     * @param string $value The regular expression to search for.
     *
     * @return QueryPart
     */
    public function regexpCondition($field, $value)
    {
        return $this->m_db->func_regexp($field, $value); 
    }
}

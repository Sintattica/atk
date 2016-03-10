<?php

namespace Sintattica\Atk\Db;

use Sintattica\Atk\Core\Tools;

/**
 * SQL Builder for MySQL 4.1+ databases.
 *
 * @author Rene van den Ouden <rene@ibuildings.nl>
 */
class MySqliQuery extends Query
{
    /**
     * Reference to the field where the new sequence
     * value should be stored.
     *
     * @var int
     */
    protected $m_seqValue;

    /**
     * Sequence field name.
     *
     * @var string
     */
    protected $m_seqField;

    /**
     * Should we return a sequence value by setting
     * $this->m_seqValue?
     *
     * @var bool
     */
    protected $m_returnSeqValue = false;

    public $m_fieldquote = '`';

    /**
     * Overriding the _addFrom function to support a change that was made in
     * MySQL 5.0.15 to make MySQL more compliant with the standard.
     *
     * See: http://bugs.mysql.com/bug.php?id=13551
     *
     * @param string $query
     */
    public function _addFrom(&$query)
    {
        $query .= ' FROM (';
        for ($i = 0; $i < count($this->m_tables); ++$i) {
            $query .= $this->quoteField($this->m_tables[$i]);
            if ($this->m_aliases[$i] != '') {
                $query .= ' '.$this->m_aliases[$i];
            }
            if ($i < count($this->m_tables) - 1) {
                $query .= ', ';
            }
        }
        $query .= ') ';
    }

    /**
     * Builds the SQL Insert query.
     *
     * @return string a SQL Insert Query
     */
    public function buildInsert()
    {
        $result = 'INSERT INTO '.$this->quoteField($this->m_tables[0]).' (';

        for ($i = 0; $i < count($this->m_fields); ++$i) {
            $result .= $this->quoteField($this->m_fields[$i]);
            if ($i < count($this->m_fields) - 1) {
                $result .= ',';
            }
        }

        $result .= ') VALUES (';

        for ($i = 0; $i < count($this->m_fields); ++$i) {
            if (($this->m_values[$this->m_fields[$i]] === "''") and ($this->m_db->m_tableMeta[$this->m_tables[0]][$this->m_fields[$i]]['type'] == 'int')) {
                Tools::atkdebug("MysqliQuery::buildInsert() : '' transformed in '0' for MySQL5 compatibility in field '".$this->m_fields[$i]."'");
                $result .= "'0'";
            } else {
                $result .= $this->m_values[$this->m_fields[$i]];
            }
            if ($i < count($this->m_fields) - 1) {
                $result .= ',';
            }
        }

        $result .= ')';

        return $result;
    }

    /**
     * Add's a sequence field to the query.
     *
     * @param string $fieldName field name
     * @param int $value field to store the new sequence value in, note certain drivers
     *                          might populate this field only after the insert query has been
     *                          executed
     * @param string $seqName sequence name (optional for certain drivers)
     *
     * @return Query
     */
    public function addSequenceField($fieldName, &$value, $seqName = null)
    {
        $meta = $this->getDb()->tableMeta($this->m_tables[0]);
        if (!Tools::hasFlag($meta[$fieldName]['flags'], Db::MF_AUTO_INCREMENT)) {
            return parent::addSequenceField($fieldName, $value, $seqName);
        }

        $this->m_seqValue = &$value;
        $this->m_seqValue = -1;
        $this->m_seqField = $fieldName;
        $this->m_returnSeqValue = true;

        return $this;
    }

    /**
     * Wrapper function to execute an insert query.
     */
    public function executeInsert()
    {
        $result = parent::executeInsert();

        if ($result && $this->m_returnSeqValue) {
            $this->m_seqValue = $this->getDb()->getInsertId();
            Tools::atkdebug("Value for sequence column {$this->m_tables[0]}.{$this->m_seqField}: {$this->m_seqValue}");
        }

        return $result;
    }

    /**
     * Generate an SQL searchcondition for a regular expression match.
     *
     * @param string $field The fieldname on which the regular expression
     *                        match will be performed.
     * @param string $value The regular expression to search for.
     * @param bool $inverse Set to false (default) to perform a normal
     *                        match. Set to true to generate a SQL string
     *                        that searches for values dat do not match.
     *
     * @return string A SQL regexp expression.
     */
    public function regexpCondition($field, $value, $inverse = false)
    {
        if ($value[0] == '!') {
            return $field." NOT REGEXP '".substr($value, 1, Tools::atk_strlen($value))."'";
        } else {
            return $field." REGEXP '$value'";
        }
    }

    /**
     * Generate an SQL searchcondition for a soundex match.
     *
     * @param string $field The fieldname on which the soundex match will
     *                        be performed.
     * @param string $value The value to search for.
     * @param bool $inverse Set to false (default) to perform a normal
     *                        match. Set to true to generate a SQL string
     *                        that searches for values dat do not match.
     *
     * @return string A SQL soundex expression.
     */
    public function soundexCondition($field, $value, $inverse = false)
    {
        if ($value[0] == '!') {
            return "soundex($field) NOT like concat('%',substring(soundex('".substr($value, 1, Tools::atk_strlen($value))."') from 2),'%')";
        } else {
            return "soundex($field) like concat('%',substring(soundex('$value') from 2),'%')";
        }
    }

    /**
     * Prepare the query for a limit.
     *
     * @param string $query The SQL query that is being constructed.
     */
    public function _addLimiter(&$query)
    {
        if ($this->m_offset >= 0 && $this->m_limit > 0) {
            $query .= ' LIMIT '.$this->m_offset.', '.$this->m_limit;
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
        if (($distinct || $this->m_distinct) && count($this->m_fields) > 0) {
            $result = 'SELECT COUNT(DISTINCT ';
            $fields = $this->quoteFields($this->m_fields);
            for ($i = 0; $i < count($fields); ++$i) {
                $fields[$i] = "COALESCE({$fields[$i]}, '###ATKNULL###')";
            }
            $result .= implode($this->quoteFields($fields), ', ');
            $result .= ') as count FROM ';
        } else {
            $result = 'SELECT COUNT(*) as count FROM ';
        }

        for ($i = 0; $i < count($this->m_tables); ++$i) {
            $result .= $this->quoteField($this->m_tables[$i]);
            if ($this->m_aliases[$i] != '') {
                $result .= ' '.$this->m_aliases[$i];
            }
            if ($i < count($this->m_tables) - 1) {
                $result .= ', ';
            }
        }

        for ($i = 0; $i < count($this->m_joins); ++$i) {
            $result .= $this->m_joins[$i];
        }

        if (count($this->m_conditions) > 0) {
            $result .= ' WHERE ('.implode(') AND (', $this->m_conditions).')';
        }

        if (count($this->m_searchconditions) > 0) {
            $prefix = ' ';
            if (count($this->m_conditions) == 0) {
                $prefix = ' WHERE ';
            } else {
                $prefix = ' AND ';
            };
            if ($this->m_searchmethod == '' || $this->m_searchmethod == 'AND') {
                $result .= $prefix.'('.implode(' AND ', $this->m_searchconditions).')';
            } else {
                $result .= $prefix.'('.implode(' OR ', $this->m_searchconditions).')';
            }
        }

        if (count($this->m_groupbys) > 0) {
            $result .= ' GROUP BY '.implode(', ', $this->m_groupbys);
        }

        return $result;
    }
}

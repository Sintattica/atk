<?php

namespace Sintattica\Atk\Attributes;

use Sintattica\Atk\DataGrid\DataGrid;
use Sintattica\Atk\Db\Query;
use Sintattica\Atk\Db\Db;

/**
 * With the ExpressionAttribute class you can select arbitrary SQL expressions
 * like subqueries etc. It's not possible to save values using this attribute.
 *
 * @author Peter C. Verhage <peter@ibuildings.nl>
 */
class ExpressionAttribute extends Attribute
{
    public $m_searchType = 'string';
    public $m_expression;

    /**
     * No storage and undefined Db type for expressions
     * @access private
     * @var int
     */
    public $m_dbfieldtype = Db::FT_UNSUPPORTED;

    /**
     * Constructor.
     *
     * @param string $name The name of the attribute.
     * @param int $flags The flags for this attribute.
     * @param string $expression The SQL expression.
     * @param string $searchType The search type (string) for this attribute.
     * At the moment only search types "string", "number" and "date" are supported.
     */
    public function __construct($name, $flags = 0, $expression, $searchType = '')
    {
        $flags = $flags | self::AF_HIDE_ADD | self::AF_READONLY_EDIT;
        parent::__construct($name, $flags);

        $this->m_expression = $expression;

        if ($searchType != '') {
            $this->setSearchType($searchType);
        }
    }

    public function storageType($mode = null)
    {
        return self::NOSTORE;
    }

    public function addToQuery($query, $tablename = '', $fieldaliasprefix = '', &$record, $level = 0, $mode = '')
    {
        $expression = str_replace('[table]', Db::quoteIdentifier($tablename), $this->m_expression);
        $query->addExpression($this->fieldName(), $expression, $fieldaliasprefix);
    }

    /**
     * Returns the order by statement for this attribute.
     *
     * @param array $extra A list of attribute names to add to the order by
     *                          statement
     * @param string $table The table name (if not given uses the owner node's table name)
     * @param string $direction Sorting direction (ASC or DESC)
     *
     * @return string order by statement
     */
    public function getOrderByStatement($extra = [], $table = '', $direction = 'ASC')
    {
        if (empty($table)) {
            $table = $this->m_ownerInstance->m_table;
        }

        $expression = str_replace('[table]', Db::quoteIdentifier($table), $this->m_expression);

        $result = "($expression)";

        if ($this->getSearchType() == 'string' && $this->getDb()->getForceCaseInsensitive()) {
            $result = "LOWER({$result})";
        }

        $result .= ($direction ? " {$direction}" : '');

        return $result;
    }

    /**
     * Sets the search type.
     *
     * @param array $type the search type (string, number or date)
     */
    public function setSearchType($type)
    {
        $this->m_searchType = $type;
    }

    /**
     * Returns the search type.
     *
     * @return string the search type (string, number or date)
     */
    public function getSearchType()
    {
        return $this->m_searchType;
    }

    /**
     * Returns the search modes.
     *
     * @return array list of search modes
     */
    public function getSearchModes()
    {
        if ($this->getSearchType() == Db::FT_NUMBER) {
            return NumberAttribute::getStaticSearchModes();
        } else {
            if ($this->getSearchType() == Db::FT_DATE) {
                return DateAttribute::getStaticSearchModes();
            } else {
                return parent::getSearchModes();
            }
        }
    }


    public function search($atksearch, $extended = false, $fieldprefix = '', DataGrid $grid = null)
    {
        if ($this->getSearchType() == Db::FT_NUMBER) {
            $attr = new NumberAttribute($this->fieldName());
            $attr->m_searchsize = $this->m_searchsize;

            return $attr->search($atksearch, $extended, $fieldprefix);
        } else {
            if ($this->getSearchType() == Db::FT_DATE) {
                $attr = new DateAttribute($this->fieldName());
                $attr->m_searchsize = 10;

                return $attr->search($atksearch, $extended, $fieldprefix);
            } else {
                return parent::search($atksearch, $extended, $fieldprefix);
            }
        }
    }

    public function display($record, $mode)
    {
        if ($this->getSearchType() == "number") {
            $attr = new NumberAttribute($this->fieldName());
            return $attr->display($record, $mode);
        } else if ($this->getSearchType() == "date") {
            $attr = new DateAttribute($this->fieldName());
            $record[$this->fieldName()] = $attr->db2value($record);
            return $attr->display($record, $mode);
        }
        return parent::display($record, $mode);
    }

    public function getSearchCondition(Query $query, $table, $value, $searchmode, $fieldname = '')
    {
        // If we are accidentally mistaken for a relation and passed an array
        // we only take our own attribute value from the array
        if ($this->m_searchmode) {
            $searchmode = $this->m_searchmode;
        }

        $expression = '('.str_replace('[table]', Db::quoteIdentifier($table), $this->m_expression).')';

        if ($this->getSearchType() == 'date') {
            $attr = new DateAttribute($this->fieldName());

            return $attr->getSearchCondition($query, $table, $value, $searchmode, $expression);
        }

        if ($this->getSearchType() == 'number') {
            $attr = new NumberAttribute($this->fieldName());
            $value = $attr->processSearchValue($value, $searchmode);

            if ($searchmode == 'between') {
                return $attr->getBetweenCondition($query, $expression, $value);
            }

            if (isset($value['from']) && $value['from'] != '') {
                $value = $value['from'];
            } else {
                if (isset($value['to']) && $value['to'] != '') {
                    $value = $value['to'];
                } else {
                    return null;
                }
            }
        }

        $func = $searchmode.'Condition';
        if (method_exists($query, $func) && $value !== '' && $value !== null) {
            return $query->$func($expression, $value);
        }

        return null;
    }
}

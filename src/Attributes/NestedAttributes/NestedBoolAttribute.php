<?php


namespace Sintattica\Atk\Attributes\NestedAttributes;

use Exception;
use Sintattica\Atk\Attributes\Attribute;
use Sintattica\Atk\Attributes\BoolAttribute;
use Sintattica\Atk\Core\Tools;
use Sintattica\Atk\Db\Query;

class NestedBoolAttribute extends BoolAttribute implements NestedAttributeInterface
{

    /**
     * @throws Exception
     */
    public function __construct($name, $flags, string $nestedAttributeField)
    {
        $this->setNestedAttributeField($nestedAttributeField);
        parent::__construct($name, $flags);
    }

    /**
     * Parent is Attribute
     * Override the basic order by to work with Json fields
     */
    public function getOrderByStatement($extra = [], $table = '', $direction = 'ASC')
    {
        $json_query = "";

        if ($this->getOwnerInstance()->hasNestedAttribute($this->fieldName(), $this->getNestedAttributeField())) {
            $json_query = $this->buildJSONExtractValue($this, $table);

            if ($this->dbFieldType() == 'string' && $this->getDb()->getForceCaseInsensitive()) {
                return "LOWER($json_query) " . ($direction ? " {$direction}" : '');
            }

            return "$json_query " . ($direction ? " {$direction}" : '');
        }

        return !empty($json_query) ? $json_query : parent::getOrderByStatement($extra, $table, $direction);
    }


    /**
     * Overload the base function to work with Json fields.
     *
     * @param Query $query
     * @param string $table
     * @param mixed $value
     * @param string $searchmode
     * @param string $fieldname
     * @return string
     */
    public function getSearchCondition(Query $query, $table, $value, $searchmode, $fieldname = '')
    {
        if (!$this->getOwnerInstance()->hasNestedAttribute($this->fieldName(), $this->getNestedAttributeField())) {
            return parent::getSearchCondition($query, $table, $value, $searchmode, $fieldname);
        }

        if (is_array($value)) {
            $value = $value[$this->fieldName()];
        }

        if ($this->m_searchmode) {
            $searchmode = $this->m_searchmode;
        }

        if (strpos($value, '*') !== false && Tools::atk_strlen($value) > 1) {
            // auto wildcard detection
            $searchmode = 'wildcard';
        }

        $fields_sql = $this->buildJSONExtractValue($this, $table);

        $func = $searchmode . 'Condition';
        if (method_exists($query, $func) && ($value || ($value == 0))) {
            return $query->$func($fields_sql, $this->escapeSQL($value), $this->dbFieldType());
        } elseif (!method_exists($query, $func)) {
            Tools::atkdebug("Database doesn't support searchmode '$searchmode' for " . $this->fieldName() . ', ignoring condition.');
        }

        return '';
    }


    /**
     *
     * Construct the SQL query to get data from a Json field.
     *
     * @param Attribute $attr
     * @param string|null $table
     * @return string
     */
    public function buildJSONExtractValue(Attribute $attr, ?string $table)
    {
        if (empty($table)) {
            $table = $attr->getOwnerInstance()->getTable();
        }

        if (strpos($table, '.') !== false) {
            $identifiers = explode('.', $table);

            $tableName = '';
            foreach ($identifiers as $identifier) {
                $tableName .= $attr->getDb()->quoteIdentifier($identifier) . '.';
            }

        } else {
            $tableName = $attr->getDb()->quoteIdentifier($table);
        }

        $nestedAttributeFieldName = $attr->getDb()->quoteIdentifier($attr->getNestedAttributeField());
        $nestedAttrName = $attr->fieldName();

        return "$tableName.$nestedAttributeFieldName->'$.$nestedAttrName'";
    }

}

<?php


namespace Sintattica\Atk\Attributes\NestedAttributes;


use Exception;
use Sintattica\Atk\Attributes\Attribute;
use Sintattica\Atk\Attributes\NumberAttribute;
use Sintattica\Atk\Core\Tools;
use Sintattica\Atk\Db\Query;

class NestedNumberAttribute extends NumberAttribute implements NestedAttributeInterface
{

    /**
     * @throws Exception
     */
    public function __construct($name, $flags, string $nestedAttributeField, $decimals = null)
    {
        $this->setNestedAttributeField($nestedAttributeField);
        parent::__construct($name, $flags, $decimals);
    }

    public function getOrderByStatement($extra = [], $table = '', $direction = 'ASC')
    {
        $json_query = self::getOrderByStatementStatic($this, $extra, $table, $direction);
        if ($json_query) {
            return $json_query;
        }

        return parent::getOrderByStatement($extra, $table, $direction);
    }

    /**
     * Overload funzione padre per permettere ricerca tramite campo JSON
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

        $fields_sql = self::buildJSONExtractValue($this, $table);

        $func = $searchmode . 'Condition';
        if (method_exists($query, $func) && ($value || ($value === 0))) {
            return $query->$func($fields_sql, $this->escapeSQL($value), $this->dbFieldType());
        } elseif (!method_exists($query, $func)) {
            Tools::atkdebug("Database doesn't support searchmode '$searchmode' for " . $this->fieldName() . ', ignoring condition.');
        }

        return '';
    }

    /**
     * Funzione per costuire il pezzo di query SQL per ottenere i dati da un campo JSON.
     *
     * @param Attribute $attr
     * @param $table
     * @return string
     */
    /**
     * Funzione per costuire il pezzo di query SQL per ottenere i dati da un campo JSON.
     *
     * @param Attribute $attr
     * @param string|null $table
     * @return string
     */
    static public function buildJSONExtractValue(Attribute $attr, ?string $table)
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

    /**
     * Funzione per ottenere la query SQL di ordinamento con un campo preso da un JSON. Static per poter essere chiamata negli altri attributi
     *
     * @param Attribute $attr
     * @param array $extra
     * @param string $table
     * @param string $direction
     * @return string
     */
    static public function getOrderByStatementStatic(Attribute $attr, $extra = [], $table = '', $direction = 'ASC')
    {
        if ($attr->getOwnerInstance()->hasNestedAttribute($attr->fieldName(), $attr->getNestedAttributeField())) {
            $json_query = self::buildJSONExtractValue($attr, $table);

            if ($attr->dbFieldType() == 'string' && $attr->getDb()->getForceCaseInsensitive()) {
                return "LOWER($json_query) " . ($direction ? " {$direction}" : '');
            }

            return "$json_query " . ($direction ? " {$direction}" : '');
        }

        return '';
    }

}

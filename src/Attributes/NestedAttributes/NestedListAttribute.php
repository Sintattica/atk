<?php


namespace Sintattica\Atk\Attributes\NestedAttributes;


use Sintattica\Atk\Attributes\ListAttribute;
use Sintattica\Atk\Core\Tools;
use Sintattica\Atk\Db\Query;

class NestedListAttribute extends ListAttribute
{
    public function __construct($name, $flags, $optionArray, $valueArray = null)
    {
        $this->setIsNestedAttribute(true);
        parent::__construct($name, $flags, $optionArray, $valueArray);
    }

    public function getOrderByStatement($extra = [], $table = '', $direction = 'ASC')
    {
        $json_query = NestedAttribute::getOrderByStatementStatic($this, $extra, $table, $direction);
        if ($json_query) {
            return $json_query;
        }

        return parent::getOrderByStatement($extra, $table, $direction);
    }

    public function getSearchCondition(Query $query, $table, $value, $searchmode, $fieldname = '')
    {
        if (!$this->getOwnerInstance()->isNestedAttribute($this->fieldName())) {
            return parent::getSearchCondition($query, $table, $value, $searchmode, $fieldname);
        }
        // We only support 'exact' matches.
        // But you can select more than one value, which we search using the IN() statement,
        // which should work in any ansi compatible database.
        $searchcondition = '';
        if (is_array($value) && Tools::count($value) > 0 && $value[0] != '') { // This last condition is for when the user selected the 'search all' option, in which case, we don't add conditions at all.

            $fields_sql = NestedAttribute::buildJSONExtractValue($this, $table);

            if (Tools::count($value) == 1 && $value[0] != '') { // exactly one value
                if ($value[0] == '__NONE__') {
                    return $query->nullCondition($fields_sql, true);
                } else {
                    return $query->exactCondition($fields_sql, $this->escapeSQL($value[0]), $this->dbFieldType());
                }
            } elseif (Tools::count($value) > 1) { // search for more values
                if (in_array('__NONE__', $value)) {
                    unset($value[array_search('__NONE__', $value)]);

                    return sprintf('(%s OR %s)', $query->nullCondition($fields_sql, true),
                        $fields_sql . " IN ('" . implode("','", $value) . "')");
                } else {
                    return $fields_sql . " IN ('" . implode("','", $value) . "')";
                }
            }
        }

        return $searchcondition;
    }


}

<?php

namespace Sintattica\Atk\Attributes;

use Sintattica\Atk\Core\Tools;
use Sintattica\Atk\DataGrid\DataGrid;
use Sintattica\Atk\Db\Query;

/**
 * Class ExpressionListAttribute
 *
 * It renders a ListAttribute for the search and display.
 *
 * IMPORTANT: the ListAttribute must have the same name of the ExpressionListAttribute
 */
class ExpressionListAttribute extends ExpressionAttribute
{
    /** @var ListAttribute useful for search and display */
    protected $m_listAttribute;

    function __construct($name, $flags, $expression, $listAttribute, $searchType = '')
    {
        parent::__construct($name, $flags, $expression, $searchType);

        $this->addFlag(self::AF_SEARCHABLE);

        $this->m_listAttribute = $listAttribute;
    }

    function setOwnerInstance($instance)
    {
        parent::setOwnerInstance($instance);

        $this->m_listAttribute->setOwnerInstance($this->m_ownerInstance);
    }

    function getListAttribute()
    {
        return $this->m_listAttribute;
    }

    function setListAttribute($listAttribute)
    {
        $this->m_listAttribute = $listAttribute;
    }

    function getSearchModes()
    {
        // all'utente è sensato visualizzare solo exact (in ricerca avanzata)
        return ['exact'];
    }

    function display($record, $mode)
    {
        // translate the value in the text
        return $this->m_listAttribute->display($record, $mode);
    }

    function search($record, $extended = false, $fieldprefix = '', DataGrid $grid = null): string
    {
        // render the ListAttribute passed
        return $this->m_listAttribute->search($record, $extended, $fieldprefix, $grid);
    }

    // TODO: check if it works
    function getSearchCondition(Query $query, $table, $value, $searchmode, $fieldname = '')
    {
        $expression = "(" . str_replace("[table]", $table, $this->m_expression) . ")";

        // N.B. implementazione di MultiListAttribute/ListAttribute, utilizzando però la expression

        if ($this->m_listAttribute instanceof MultiListAttribute) {
            $searchconditions = [];
            if (is_array($value) && $value[0] != "" && count($value) > 0) {
                if (in_array('__NONE__', $value)) {
                    return $query->nullCondition($expression, true);
                }
                // include i separatori nel valore da ricercare, così da rendere sicura la ricerca (posto che il separatore NON sia usato nei valori)
                $sep = $this->m_listAttribute->getFieldSeparator();
                if (count($value) == 1) {
                    $searchconditions[] = $query->substringCondition($expression, Tools::escapeSQL($sep . $value[0] . $sep));
                } else {
                    foreach ($value as $str) {
                        $searchconditions[] = $query->substringCondition($expression, Tools::escapeSQL($sep . $str . $sep));
                    }
                }
            }
            if (count($searchconditions)) {
                return '(' . implode(' OR ', $searchconditions) . ')';
            }

        } else if ($this->m_listAttribute instanceof ListAttribute) {
            if (is_array($value) && count($value) > 0 && $value[0] != "") { // This last condition is for when the user selected the 'search all' option, in which case, we don't add conditions at all.

                if ($this->m_listAttribute->isMultipleSearch(false) && count($value) == 1 && strpos($value[0], ',') !== false) {
                    // in case of multiple select in simple search, we have the selected values into a single string (csv)
                    $value = explode(',', $value[0]);
                    // "search all" option has precedence (when another options are selected together)
                    if ($value[0] == "") {
                        return '';
                    }
                }

                if (count($value) == 1 && $value[0] != '') { // exactly one value
                    if ($value[0] == "__NONE__") {
                        return $query->nullCondition($expression, true);
                    } else {
                        return $query->exactCondition($expression, Tools::escapeSQL($value[0]));
                    }
                } else if (count($value) > 1) { // search for more values
                    if (in_array('__NONE__', $value)) {
                        unset($value[array_search('__NONE__', $value)]);
                        return sprintf('(%s OR %s)',
                            $query->nullCondition($expression, true),
                            $expression . " IN ('" . implode("','", $value) . "')");
                    } else {
                        return $expression . " IN ('" . implode("','", $value) . "')";
                    }
                }
            }
        }

        return '';
    }
}

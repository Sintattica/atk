<?php

namespace Sintattica\Atk\Core;

use Sintattica\Atk\Attributes\NumberAttribute;

/**
 * The ATK data node can be used to create nodes that don't retrieve their
 * data from the database.
 *
 * The data can either be provided using the setData method or the getData
 * method (and possibly other methods) can be overriden to provide the
 * data dynamically.
 *
 * @author Peter C. Verhage <peter@ibuildings.nl>
 */
class DataNode extends Node
{
    /** @var array data array */
    private $m_data = [];

    /**
     * Constructor.
     *
     * @param string $type node type (by default the class name)
     * @param int $flags node flags
     *
     * @return DataNode
     */
    public function __construct($type = '', $flags = 0)
    {
        parent::__construct($type, $flags | self::NF_NO_ADD | self::NF_NO_EDIT);
        $this->setTable($this->m_type);
    }

    /**
     * Sets the data that this node should use.
     *
     * @param array $data data list
     */
    public function setData($data)
    {
        $this->m_data = $data;
    }

    /**
     * Returns the internal data.
     *
     * @return array data list
     */
    protected function getData()
    {
        return $this->m_data;
    }

    /**
     * Select records using the given criteria.
     *
     * @param string $selector selector string
     * @param string $order order string
     * @param array $limit limit array
     *
     * @return array selected records
     */
    public function select($selector = null, $order = null, $limit = null)
    {
        Tools::atkdebug(get_class($this).'::select('.$selector.')');

        if ($order == null) {
            $order = $this->getOrder();
        }

        $params = array(
            'selector' => $selector,
            'order' => $order,
            'offset' => isset($limit['offset']) ? $limit['offset'] : 0,
            'limit' => isset($limit['limit']) ? $limit['limit'] : -1,
            'search' => isset($this->m_postvars['atksearch']) ? $this->m_postvars['atksearch'] : null,
        );

        $result = $this->findData($params);
        Tools::atkdebug('Result '.get_class($this).'::select('.$selector.') => '.Tools::count($result).' row(s)');

        return $result;
    }

    /**
     * Returns how many records will be returned for the given selector.
     *
     * @param string $selector selector string
     *
     * @return int record count
     */
    public function count($selector = null)
    {
        $params = array(
            'selector' => $selector,
            'search' => isset($this->m_postvars['atksearch']) ? $this->m_postvars['atksearch'] : null,
        );

        return $this->countData($params);
    }

    /**
     * Count "rows".
     *
     * Supported parameters are: selector, limit, offset and order.
     *
     * @param array $params parameters
     *
     * @return int number of "records"
     */
    protected function countData($params = array())
    {
        return Tools::count($this->findData($params));
    }

    /**
     * Find data using the given parameters.
     * Supported parameters are: selector, limit, offset and order.
     *
     * @param array $params parameters
     *
     * @return array found data
     */
    protected function findData($params = array())
    {
        $selector = @$params['selector'] ? $params['selector'] : '';
        $limit = @$params['limit'] ? $params['limit'] : -1;
        $offset = @$params['offset'] ? $params['offset'] : 0;
        $order = @$params['order'] ? $params['order'] : null;
        $search = @$params['search'] ? $params['search'] : [];

        $selector = $this->getSelector($selector);
        $criteria = $this->getCriteria($selector);

        $data = $this->getData();
        $data = $this->filterColumns($data);
        $data = $this->filterData($data, $criteria, $search);
        $data = $this->sortData($data, $order);
        $data = $this->limitData($data, $limit, $offset);

        return $data;
    }

    /**
     * Filter invalid columns.
     *
     * @param array $data data
     *
     * @return array data
     */
    protected function filterColumns($data)
    {
        $result = [];

        foreach ($data as $row) {
            foreach (array_keys($row) as $column) {
                if ($this->getAttribute($column) == null) {
                    unset($row[$column]);
                }
            }

            $result[] = $row;
        }

        return $result;
    }

    /**
     * Returns the full selector including added filters.
     *
     * @param string $selector selector
     *
     * @return string full selector string
     */
    protected function getSelector($selector)
    {
        $result = $selector;

        foreach ($this->m_fuzzyFilters as $filter) {
            if (!empty($result)) {
                $result .= ' AND ';
            }

            $result .= $filter;
        }

        return $result;
    }

    /**
     * Translate the given selector to a criteria array
     * which key/values can be used to filter data.
     *
     * @param string $selector selector string
     *
     * @return array criteria
     */
    protected function getCriteria($selector)
    {
        $criteria = $this->m_filters;

        if (empty($selector)) {
            return $criteria;
        }

        if (!is_array($selectors)) {
            $selectors = [$selectors];
        }
        foreach ($selectors as $selector) {
            $selector = json_decode($selector, true);
            $options = array_merge($this->m_primaryKey, $selector);
            foreach ($options as $column => $value) {
                if (strpos($column, '.') !== false) {
                    list($table, $column) = explode('.', $column);
                    if ($table != $this->getTable()) {
                        continue;
                    }
                }

                if (isset($criteria[$column]) && $criteria[$column] != $value) {
                    $criteria[$column] = array_merge((array)$criteria[$column], (array)$value);
                } else {
                    $criteria[$column] = $value;
                }
            }
        }

        return $criteria;
    }

    /**
     * Filter data using the given selector.
     *
     * @param array $data data list
     * @param array $criteria selector criteria list
     * @param array $search search fields / values
     *
     * @return array filtered data
     */
    protected function filterData($data, $criteria, $search)
    {
        $result = [];

        foreach ($data as $record) {
            if ($this->isValidRecord($record, $criteria, $search)) {
                $result[] = $record;
            }
        }

        return $result;
    }

    /**
     * Check if record is valid using the given selector criteria and search params.
     *
     * @param array $record record
     * @param array $criteria selector criteria list
     * @param array $search search fields / values
     *
     * @return bool is valid?
     */
    protected function isValidRecord($record, $criteria, $search)
    {
        foreach ($criteria as $key => $value) {
            if ($record[$key] != $value) {
                return false;
            }
        }

        foreach ($search as $key => $value) {
            if (!empty($value) && stripos($record[$key], $value) === false) {
                return false;
            }
        }

        return true;
    }

    /**
     * Parse the order to something we can use. If the order
     * is invalid false is returned.
     *
     * @param string $order order string
     *
     * @return array|bool array 1st element column, 2nd element ascending? or false
     */
    protected function translateOrder($order)
    {
        if (empty($order)) {
            return false;
        }

        list($column, $direction) = preg_split('/[ ]+/', $order);
        if (strpos($column, '.') !== false) {
            list($table, $column) = explode('.', $column);
            if ($table != $this->getTable()) {
                return false;
            }
        }

        $column = trim($column);
        $direction = strtolower(trim($direction));

        $asc = $direction != 'desc';

        if ($this->getAttribute($column) != null) {
            return array($column, $asc);
        }

        return false;
    }

    /**
     * Sort data by the given order string.
     *
     * @param array $data data list
     * @param string $order order string
     *
     * @return array data list
     */
    protected function sortData($data, $order)
    {
        list($column, $asc) = $this->translateOrder($order);

        if ($column != false) {
            $attr = $this->getAttribute($column);

            if ($attr instanceof NumberAttribute) {
                usort($data,
                    create_function('$a, $b', 'return $a["'.$column.'"] == $b["'.$column.'"] ? 0 : ($a["'.$column.'"] < $b["'.$column.'"] ? -1 : 1);'));
            } else {
                usort($data, create_function('$a, $b', 'return strcasecmp($a["'.$column.'"], $b["'.$column.'"]);'));
            }

            if (!$asc) {
                $data = array_reverse($data);
            }
        }

        return $data;
    }

    /**
     * Limit data using the given limit and offset.
     *
     * @param array $data data list
     * @param int $limit limit
     * @param int $offset offset
     *
     * @return array limited data
     */
    protected function limitData($data, $limit = -1, $offset = 0)
    {
        if ($limit >= 0) {
            $data = array_slice($data, $offset, $limit);
        } else {
            $data = array_slice($data, $offset);
        }

        return $data;
    }

    /**
     * Add is not supported.
     *
     * @return bool false
     */
    public function addDb()
    {
        return false;
    }

    /**
     * Update is not supported.
     *
     * @return bool false
     */
    public function updateDb()
    {
        return false;
    }

    /**
     * Delete is not supported.
     *
     * @return bool false
     */
    public function deleteDb()
    {
        return false;
    }

    /**
     * Don't fetch meta data.
     */
    public function setAttribSizes()
    {
    }
}

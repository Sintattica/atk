<?php

namespace Sintattica\Atk\Utils;

use Sintattica\Atk\Attributes\Attribute;
use Sintattica\Atk\Core\Config;
use Sintattica\Atk\Core\Node;
use Sintattica\Atk\Core\Tools;
use Sintattica\Atk\Db\Query;
use Sintattica\Atk\Db\Db;
use Exception;

/**
 * Fluent interface helper class for retrieving records from a node.
 *
 * @author Peter C. Verhage <peter@achievo.org>
 */
class Selector implements \ArrayAccess, \Countable, \IteratorAggregate
{
    /**
     * This selector's node.
     *
     * @var Node
     */
    protected $m_node;

    /**
     * Selector parameters.
     */
    protected $m_conditions = [];
    protected $m_distinct = false;
    protected $m_mode = '';
    protected $m_order = '';
    protected $m_limit = -1;
    protected $m_offset = 0;
    protected $m_excludes = null;
    protected $m_includes = null;
    protected $m_ignoreDefaultFilters = false;
    protected $m_ignorePostvars = false;
    protected $m_ignoreForceLoad = false;
    protected $m_ignorePrimaryKey = false;

    /**
     * Rows cache.
     *
     * @var array
     */
    protected $m_rows = null;

    /**
     * Row count cache.
     *
     * @var int
     */
    protected $m_rowCount = null;

    /**
     * Indices cache.
     *
     * @var array
     */
    protected $m_indices = null;

    /**
     * Current iterator instance (if iterator is used).
     *
     * @var SelectorIterator
     */
    private $m_iterator = null;

    /**
     * Current query object (if iterator is used).
     *
     * @var Query
     */
    private $m_query = null;

    /**
     * Current attributes by load type (if iterator is used).
     *
     * @var array
     */
    private $m_attrsByLoadType = null;

    /**
     * Constructor.
     *
     * @param Node $node this selector's node
     */
    public function __construct($node)
    {
        $this->m_node = $node;
    }

    /**
     * Returns the node for this selector.
     *
     * @return Node
     */
    protected function _getNode()
    {
        return $this->m_node;
    }

    /**
     * Returns the node's database.
     *
     * @return Db
     */
    protected function _getDb()
    {
        return $this->_getNode()->getDb();
    }

    /**
     * Adds a condition..
     *
     * @param string $condition where clause
     * @param array $params bind parameters
     *
     * @return Selector
     */
    public function where($condition, $params = array())
    {
        if (strlen(trim($condition)) > 0) {
            $this->m_conditions[] = array('condition' => $condition, 'params' => $params);
        }

        return $this;
    }

    /**
     * Ignore default node filters.
     *
     * @param bool $ignore ignore default node filters?
     *
     * @return Selector
     */
    public function ignoreDefaultFilters($ignore = true)
    {
        $this->m_ignoreDefaultFilters = $ignore;

        return $this;
    }

    /**
     * Ignore criteria set in the postvars, like search criteria etc.
     *
     * @param bool $ignore ignore postvars?
     *
     * @return Selector
     */
    public function ignorePostvars($ignore = true)
    {
        $this->m_ignorePostvars = $ignore;

        return $this;
    }

    /**
     * Ignore force load flags.
     *
     * @param bool $ignore ignore force load flags
     *
     * @return Selector
     */
    public function ignoreForceLoad($ignore = true)
    {
        $this->m_ignoreForceLoad = $ignore;

        return $this;
    }

    /**
     * Don't forcefully load the primary key. The result records also won't
     * contain the special "atkprimkey" entry.
     *
     * @param bool $ignore ignore primary key
     *
     * @return Selector
     */
    public function ignorePrimaryKey($ignore = true)
    {
        $this->m_ignorePrimaryKey = $ignore;

        return $this;
    }

    /**
     * Distinct selection?
     *
     * @param bool $distinct distinct selection?
     *
     * @return Selector
     */
    public function distinct($distinct)
    {
        $this->m_distinct = $distinct;

        return $this;
    }

    /**
     * Set the select mode.
     *
     * @param string $mode select mode
     *
     * @return Selector
     */
    public function mode($mode)
    {
        $this->m_mode = $mode;

        return $this;
    }

    /**
     * Order by the given order by string.
     *
     * @param string $order order by string
     *
     * @return Selector
     */
    public function orderBy($order)
    {
        $this->m_order = $order;

        return $this;
    }

    /**
     * Limit the results bij the given limit (and from the optional offset).
     *
     * @param int $limit limit
     * @param int $offset offset
     *
     * @return Selector
     */
    public function limit($limit, $offset = 0)
    {
        $this->m_limit = $limit;
        $this->m_offset = $offset;

        return $this;
    }

    /**
     * Include only the following list of attributes.
     *
     * @param array|string $includes list of includes
     *
     * @return Selector
     */
    public function includes($includes)
    {
        if ($includes == null) {
            $includes = null;
        } else {
            if (!is_array($includes)) {
                $includes = func_get_args();
            }
        }

        $this->m_includes = $includes;

        return $this;
    }

    /**
     * Exclude the following list of attributes.
     *
     * @param array|string $excludes list of excludes
     *
     * @return Selector
     */
    public function excludes($excludes)
    {
        if ($excludes == null) {
            $excludes = null;
        } else {
            if (!is_array($excludes)) {
                $excludes = func_get_args();
            }
        }

        $this->m_excludes = $excludes;

        return $this;
    }

    /**
     * Are we searching?
     */
    protected function _isSearching()
    {
        if ($this->m_ignorePostvars) {
            return false;
        }

        $searchCriteria = Tools::atkArrayNvl($this->_getNode()->m_postvars, 'atksearch');
        $smartSearchCriteria = Tools::atkArrayNvl($this->_getNode()->m_postvars, 'atksmartsearch');
        $indexValue = $this->_getNode()->m_index != '' ? Tools::atkArrayNvl($this->_getNode()->m_postvars, 'atkindex', '') : '';

        return (is_array($searchCriteria) && Tools::count($searchCriteria) > 0) || (is_array($smartSearchCriteria) && Tools::count($smartSearchCriteria) > 0) || !empty($indexValue);
    }

    /**
     * Apply set conditions to query.
     *
     * @param Query $query query object
     */
    protected function _applyConditionsToQuery($query)
    {
        foreach ($this->m_conditions as $condition) {
            $query->addCondition($condition['condition']);
        }
    }

    /**
     * Apply posted filter to query.
     *
     * @param Query $query query object
     */
    protected function _applyPostedFilterToQuery($query)
    {
        $filter = Tools::atkArrayNvl($this->_getNode()->m_postvars, 'atkfilter', '');
        if (empty($filter)) {
            return;
        }

        $query->addCondition($filter);
    }

    /**
     * Apply posted index value to query.
     *
     * @param Query $query query object
     */
    protected function _applyPostedIndexValueToQuery(Query $query)
    {
        $indexAttrName = $this->_getNode()->m_index;
        $indexValue = Tools::atkArrayNvl($this->_getNode()->m_postvars, 'atkindex', '');
        if (empty($indexAttrName) || empty($indexValue) || !is_object($this->_getNode()->getAttribute($indexAttrName))) {
            return;
        }

        $attr = $this->_getNode()->getAttribute($indexAttrName);
        $attr->searchCondition($query, $this->_getNode()->getTable(), $indexValue, 'wildcard', '');
    }

    /**
     * Set search method for query.
     *
     * @param Query $query query object
     */
    protected function _applyPostedSearchMethodToQuery(Query $query)
    {
        if (isset($this->_getNode()->m_postvars['atksearchmethod'])) {
            $query->setSearchMethod($this->_getNode()->m_postvars['atksearchmethod']);
        }
    }

    /**
     * Apply posted (normal) search criteria to query.
     *
     * @param Query $query query object
     * @param array $attrsByLoadType attributes by load type
     */
    protected function _applyPostedSearchCriteriaToQuery(Query $query, array $attrsByLoadType)
    {
        $searchCriteria = Tools::atkArrayNvl($this->_getNode()->m_postvars, 'atksearch');
        if (!is_array($searchCriteria) || Tools::count($searchCriteria) == 0) {
            return;
        }

        foreach ($searchCriteria as $key => $value) {
            if ($value === null || $value === '' || ($this->m_mode != 'admin' && $this->m_mode != 'export' && !array_key_exists($key,
                        $attrsByLoadType[Attribute::ADDTOQUERY]))
            ) {
                continue;
            }

            $attr = $this->_getNode()->getAttribute($key);
            if (is_object($attr)) {
                if (is_array($value) && isset($value[$key]) && Tools::count($value) == 1) {
                    $value = $value[$key];
                }

                $searchMode = $this->_getNode()->getSearchMode();
                if (is_array($searchMode)) {
                    $searchMode = $searchMode[$key];
                }

                if ($searchMode == null) {
                    $searchMode = Config::getGlobal('search_defaultmode');
                }

                $attr->searchCondition($query, $this->_getNode()->getTable(), $value, $searchMode, '');
            } else {
                Tools::atkdebug("Using default search method for $key");
                //$condition = 'LOWER('.$this->_getNode()->getTable().'.'.$key.") LIKE LOWER('%".$this->_getDb()->escapeSQL($value, true)."%')";
                $condition = $this->_getNode()->getTable().'.'.$key." LIKE '%".$this->_getDb()->escapeSQL($value, true)."%'";
                $query->addSearchCondition($condition);
            }
        }
    }

    /**
     * Apply posted smart search criteria to query.
     *
     * @param Query $query query object
     */
    protected function _applyPostedSmartSearchCriteriaToQuery(Query $query)
    {
        $searchCriteria = Tools::atkArrayNvl($this->_getNode()->m_postvars, 'atksmartsearch');
        if (!is_array($searchCriteria) || Tools::count($searchCriteria) == 0) {
            return;
        }

        foreach ($searchCriteria as $id => $criterium) {
            $path = $criterium['attrs'];
            $value = $criterium['value'];
            $mode = $criterium['mode'];

            $attrName = array_shift($path);
            $attr = $this->_getNode()->getAttribute($attrName);

            if (is_object($attr)) {
                $attr->smartSearchCondition($id, 0, $path, $query, $this->_getNode()->getTable(), $value, $this->m_mode);
            }
        }
    }

    /**
     * Apply criteria that are part of the postvars (e.g. filter, index, search criteria).
     *
     * @param Query $query query
     * @param array $attrsByLoadType attributes by load type
     */
    protected function _applyPostvarsToQuery(Query $query, array $attrsByLoadType)
    {
        if (!$this->m_ignorePostvars) {
            $this->_applyPostedFilterToQuery($query);
            $this->_applyPostedIndexValueToQuery($query);
            $this->_applyPostedSearchMethodToQuery($query);
            $this->_applyPostedSearchCriteriaToQuery($query, $attrsByLoadType);
            $this->_applyPostedSmartSearchCriteriaToQuery($query);
        }
    }

    /**
     * Apply node filters to query.
     *
     * @param Query $query query
     */
    protected function _applyFiltersToQuery(Query $query)
    {
        if ($this->m_ignoreDefaultFilters) {
            return;
        }

        // key/value filters
        foreach ($this->_getNode()->m_filters as $key => $value) {
            $query->addCondition($key."='".$this->_getDb()->escapeSQL($value)."'");
        }

        // fuzzy filters
        foreach ($this->_getNode()->m_fuzzyFilters as $filter) {
            $parser = new StringParser($filter);
            $filter = $parser->parse(array('table' => $this->_getNode()->getTable()));
            $query->addCondition($filter);
        }
    }

    /**
     * Is attribute load required?
     *
     * @param Attribute $attr attribute
     *
     * @return bool load required?
     */
    protected function _isAttributeLoadRequired($attr)
    {
        $attrName = $attr->fieldName();

        return (!$this->m_ignorePrimaryKey && in_array($attrName,
                $this->_getNode()->m_primaryKey)) || (!$this->m_ignoreForceLoad && $attr->hasFlag(Attribute::AF_FORCE_LOAD)) || (($this->m_includes != null && in_array($attrName,
                    $this->m_includes)) || ($this->m_excludes != null && !in_array($attrName,
                    $this->m_excludes))) || ($this->m_excludes == null && $this->m_includes == null);
    }

    /**
     * Returns the attributes for each load type (Attribute::PRELOAD, Attribute::ADDTOQUERY, Attribute::POSTLOAD).
     *
     * @return array attributes by load type
     */
    protected function _getAttributesByLoadType()
    {
        $isSearching = $this->_isSearching();
        $result = array(Attribute::PRELOAD => [], Attribute::ADDTOQUERY => [], Attribute::POSTLOAD => array());

        foreach ($this->_getNode()->getAttributes() as $attr) {
            if (!$this->_isAttributeLoadRequired($attr)) {
                continue;
            }

            $loadType = $attr->loadType($this->m_mode);

            if (Tools::hasFlag($loadType, Attribute::PRELOAD)) {
                $result[Attribute::PRELOAD][$attr->fieldName()] = $attr;
            }

            if (Tools::hasFlag($loadType, Attribute::ADDTOQUERY)) {
                $result[Attribute::ADDTOQUERY][$attr->fieldName()] = $attr;
            }

            if (Tools::hasFlag($loadType, Attribute::POSTLOAD)) {
                $result[Attribute::POSTLOAD][$attr->fieldName()] = $attr;
            }
        }

        return $result;
    }

    /**
     * Apply attributes to query, e.g. add columns etc.
     *
     * @param Query $query query object
     * @param array $attrsByLoadType attributes by load type
     */
    protected function _applyAttributesToQuery(Query $query, array $attrsByLoadType)
    {
        $record = [];
        foreach ($attrsByLoadType[Attribute::PRELOAD] as $attr) {
            $record[$attr->fieldName()] = $attr->load($this->_getDb(), $record, $this->m_mode);
        }

        foreach ($attrsByLoadType[Attribute::ADDTOQUERY] as $attr) {
            $attr->addToQuery($query, $this->_getNode()->getTable(), '', $record, 1, $this->m_mode);
        }
    }

    /**
     * Build base query object.
     *
     * @param array $attrsByLoadType attributes by load type
     *
     * @return Query query object
     */
    protected function _buildQuery(array $attrsByLoadType)
    {
        $query = $this->_getNode()->getDb()->createQuery();
        $query->setDistinct($this->m_distinct);
        $query->setTable($this->_getNode()->getTable());

        $this->_applyConditionsToQuery($query);
        $this->_applyFiltersToQuery($query);
        $this->_applyPostvarsToQuery($query, $attrsByLoadType);
        $this->_applyAttributesToQuery($query, $attrsByLoadType);

        return $query;
    }

    /**
     * Build select query object.
     *
     * @param array $attrsByLoadType attributes by load type
     *
     * @return Query query object
     */
    protected function _buildSelectQuery(array $attrsByLoadType)
    {
        $query = $this->_buildQuery($attrsByLoadType);

        if (!empty($this->m_order)) {
            $query->addOrderBy($this->m_order);
        }

        if ($this->m_limit >= 0) {
            $query->setLimit($this->m_offset, $this->m_limit);
        }

        return $query;
    }

    /**
     * Build count query object.
     *
     * @param array $attrsByLoadType attributes by load type
     *
     * @return Query query object
     */
    protected function _buildCountQuery(array $attrsByLoadType)
    {
        return $this->_buildQuery($attrsByLoadType);
    }

    /**
     * Returns all bind parameters for all conditions.
     *
     * @return array bind parameters
     */
    protected function _getBindParameters()
    {
        $params = [];

        foreach ($this->m_conditions as $condition) {
            $params = array_merge($params, $condition['params']);
        }

        return $params;
    }

    /**
     * Transform raw database row to node compatible row.
     *
     * @param array $row raw database row
     * @param Query $query query object
     * @param array $attrsByLoadType attributes by load type
     *
     * @return array node compatible row
     */
    protected function _transformRow($row, Query $query, array $attrsByLoadType)
    {
        $query->deAlias($row);
        Tools::atkDataDecode($row);

        $result = [];
        foreach ($attrsByLoadType[Attribute::ADDTOQUERY] as $attr) {
            $result[$attr->fieldName()] = $attr->db2value($row);
        }

        if (!$this->m_ignorePrimaryKey) {
            $result['atkprimkey'] = $this->_getNode()->primaryKey($result);
        }

        foreach ($attrsByLoadType[Attribute::POSTLOAD] as $attr) {
            $result[$attr->fieldName()] = $attr->load($this->_getDb(), $result, $this->m_mode);
        }

        return $result;
    }

    /**
     * Transform raw database rows to node compatible rows.
     *
     * @param array $rows raw database rows
     * @param Query $query query object
     * @param array $attrsByLoadType attributes by load type
     *
     * @return array node compatible rows
     */
    protected function _transformRows($rows, Query $query, array $attrsByLoadType)
    {
        foreach ($rows as &$row) {
            $row = $this->_transformRow($row, $query, $attrsByLoadType);
        }

        return $rows;
    }

    /**
     * Transform raw database row to node compatible row for the current iterator.
     *
     * @param array $row raw database row
     *
     * @return array node compatible row
     */
    public function transformRow($row)
    {
        if ($this->m_iterator == null) {
            throw new Exception(__METHOD__.' should only be called by the current atkSelectorIterator instance!');
        }

        return $this->_transformRow($row, $this->m_query, $this->m_attrsByLoadType);
    }

    /**
     * Returns the first found row.
     *
     * @return array first row
     */
    public function getFirstRow()
    {
        $this->limit(1, $this->m_offset);
        $rows = $this->fetchAll();

        return Tools::count($rows) == 1 ? $rows[0] : null;
    }

    /**
     * Return all rows.
     *
     * @return array all rows
     */
    public function fetchAll()
    {
        if ($this->m_rows === null) {
            $attrsByLoadType = $this->_getAttributesByLoadType();
            $query = $this->_buildSelectQuery($attrsByLoadType);
            $stmt = $this->_getDb()->prepare($query->buildSelect());
            $stmt->execute($this->_getBindParameters());
            $rows = $stmt->fetchAll();
            $this->m_rows = $this->_transformRows($rows, $query, $attrsByLoadType);
        }

        return $this->m_rows;
    }


    public function getTotals($fields = [])
    {
        $attrsByLoadType = $this->_getAttributesByLoadType();
        $query = $this->_buildQuery($attrsByLoadType);
        $prefix = '__sum__';

        foreach ($fields as $field) {
            $i = array_search($field, array_column($query->m_expressions, 'name'));
            if ($i !== false) {
                $expr = $query->m_expressions[$i]['expression'];
            } else {
                $expr = Db::quoteIdentifier($this->_getNode()->getTable()) . '.' . Db::quoteIdentifier($field);
            }
            $query->addExpression($field, 'SUM('.$expr.')', $prefix);
        }

        $stmt = $this->_getDb()->prepare($query->buildSelect());
        $stmt->execute($this->_getBindParameters());
        $row = $stmt->fetch();

        $query->deAlias($row);
        $res = [];
        foreach ($fields as $field) {
            $res[$field] = $row[$prefix.$field] ?: 0;
        }

        return $res;
    }


    /**
     * Return row count.
     *
     * @return int row count
     */
    public function getRowCount()
    {
        if ($this->m_rowCount === null) {
            $attrsByLoadType = $this->_getAttributesByLoadType();
            $query = $this->_buildCountQuery($attrsByLoadType);
            $stmt = $this->_getDb()->prepare($query->buildCount());
            $stmt->execute($this->_getBindParameters());
            $rows = $stmt->fetchAll();
            $this->m_rowCount = Tools::count($rows) == 1 ? $rows[0]['count'] : Tools::count($rows); // group by fix
        }

        return $this->m_rowCount;
    }

    /**
     * Returns the available indices for the index field based on the criteria.
     *
     * @return array available indices
     */
    public function getIndices()
    {
        if ($this->_getNode()->m_index == null) {
            return [];
        } else {
            if ($this->m_indices != null) {
                return $this->m_indices;
            }
        }

        $attrsByLoadType = $this->_getAttributesByLoadType();

        $index = $this->_getNode()->m_index;
        $this->_getNode()->m_index = null;
        $query = $this->_buildQuery($attrsByLoadType);
        $this->_getNode()->m_index = $index;

        $query->clearFields();
        $query->clearExpressions();

        $indexColumn = Db::quoteIdentifier($this->_getNode()->getTable()).'.'.Db::quoteIdentifier($index);
        $expression = 'UPPER('.$this->_getDb()->func_substring($indexColumn, 1, 1).')';
        $query->addExpression('index', $expression);
        $query->addGroupBy($expression);
        $query->addOrderBy($expression);

        $stmt = $this->_getDb()->prepare($query->buildSelect());
        $stmt->execute($this->_getBindParameters());
        $this->m_indices = $stmt->getAllValues();

        return $this->m_indices;
    }

    /**
     * Does the given offset exist?
     *
     * @param string|int $key key
     *
     * @return bool offset exists?
     */
    public function offsetExists($key)
    {
        $this->fetchAll();

        return isset($this->m_rows[$key]);
    }

    /**
     * Returns the given offset.
     *
     * @param string|int $key key
     *
     * @return mixed
     */
    public function offsetGet($key)
    {
        $this->fetchAll();

        return $this->m_rows[$key];
    }

    /**
     * Sets the value for the given offset.
     *
     * @param string|int $key
     * @param mixed $value
     *
     * @return mixed
     */
    public function offsetSet($key, $value)
    {
        $this->fetchAll();

        return $this->m_rows[$key] = $value;
    }

    /**
     * Unset the given element.
     *
     * @param string|int $key
     */
    public function offsetUnset($key)
    {
        $this->fetchAll();
        unset($this->m_rows[$key]);
    }

    /**
     * Returns this selector's iterator.
     *
     * NOTE: if you call this method multiple times, the same iterator will
     *       be returned, unless you have closed the selector first
     */
    public function getIterator()
    {
        if ($this->m_iterator == null) {
            $attrsByLoadType = $this->_getAttributesByLoadType();
            $query = $this->_buildSelectQuery($attrsByLoadType);
            $stmt = $this->_getDb()->prepare($query->buildSelect());
            $stmt->execute($this->_getBindParameters());

            $this->m_attrsByLoadType = $attrsByLoadType;
            $this->m_query = $query;

            $this->m_iterator = new SelectorIterator($stmt, $this);
        }

        return $this->m_iterator;
    }

    /**
     * Closes the current statement used for this selector.
     * Also clears the row and row count cache.
     */
    public function close()
    {
        if ($this->m_iterator != null) {
            $this->m_iterator = null;
            $this->m_query = null;
            $this->m_attrsByLoadType = null;
        }

        $this->m_rows = null;
        $this->m_rowCount = null;
        $this->m_indices = null;
    }

    /**
     * Returns the row count (used when calling count on an Selector object,
     * don't use this if you want to efficiently retrieve the row count using
     * a Tools::count() select statement, use getRowCount instead!
     *
     * @return int row count
     */
    public function count()
    {
        $this->fetchAll();

        return Tools::count($this->m_rows);
    }
}

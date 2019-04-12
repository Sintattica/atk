<?php

namespace Sintattica\Atk\Attributes;

use Sintattica\Atk\Core\Atk;
use Sintattica\Atk\Core\Node;
use Sintattica\Atk\Core\Tools;
use Sintattica\Atk\DataGrid\DataGrid;
use Sintattica\Atk\Db\Db;
use Sintattica\Atk\Db\Query;

/**
 * The atkFuzzySearchAttribute class represents an attribute of a node
 * that has a field where you can enter certain keywords to search for
 * on another node.
 *
 * @author Ivo Jansch <ivo@achievo.org>
 */
class FuzzySearchAttribute extends Attribute
{
    /**
     * The node we are searching on
     * @var String
     * @access private
     */
    public $m_searchnode = '';

    /**
     * The function to call back with the record and results
     * @var String
     * @access private
     */
    public $m_callback = '';

    /**
     * The mode of the the fuzzy search
     * @var String
     * @access private
     */
    public $m_mode = 'all';

    /**
     * The matches we got from the search
     * @var array
     * @access private
     */
    public $m_matches = [];

    /**
     * The database fieldtype.
     * @access private
     * @var int
     */
    public $m_dbfieldtype = Db::FT_UNSUPPORTED;


    /** @var Node $m_searchnodeInstance An instance of the node we are searching on */
    public $m_searchnodeInstance;

    /*
     * @var String Filter for destination records.
     */
    public $m_destinationFilter = '';

    /**
     * The fuzzySearchAttribute, with this you can search a node for certain keywords
     * and get a selectable list of records that match the keywords.
     * Possible modes:
     * - all (default)    return everything
     * - first            return only the first result
     * - firstperkeyword  return the first result per keyword
     * - select           make the user select
     * - selectperkeyword make the user select for every keyword
     * - multiselect      ?
     *
     * @param string $name The name of the attribute
     * @param int $flags The flags of the attribute
     * @param string $searchnode The node to search on
     * @param string $callback The function of the owner node to call
     *                           with the record to store and the results of the search
     *                           Has to return a status (true or false)
     * @param string $mode The mode of the search (all(default)|first|firstperkeyword|
     *                           select|selectperkeyword|multiselect)
     */
    public function __construct($name, $flags = 0, $searchnode, $callback, $mode = 'all')
    {
        $flags = $flags | self::AF_HIDE_VIEW | self::AF_HIDE_LIST;
        parent::__construct($name, $flags);
        
        $this->m_searchnode = $searchnode;
        $this->m_callback = $callback;
        $this->m_mode = strtolower($mode);
    }

    /**
     * Creates an instance of the node we are searching on and stores it
     * in a member variable ($this->m_searchnodeInstance).
     *
     * @return bool
     */
    public function createSearchNodeInstance()
    {
        if (!is_object($this->m_searchnodeInstance)) {
            $atk = Atk::getInstance();
            $this->m_searchnodeInstance = $atk->atkGetNode($this->m_searchnode);

            return is_object($this->m_searchnodeInstance);
        }

        return true;
    }


    public function validate(&$rec, $mode)
    {
        if (is_array($rec[$this->fieldName()])) {
            // Coming from selectscreen, no search necessary anymore.
        } else {
            $this->m_matches = $this->getMatches($rec[$this->fieldName()]);

            $mustselect = false;

            if ($this->m_mode == 'multiselect' || $this->m_mode == 'selectperkeyword') {
                // In multiselect and selectperkeyword mode, we present the selector
                // if one or more keywords returned more than one match. If they
                // all returned exactly one match, we pass all records and don't
                // offer selection.
                foreach ($this->m_matches as $keyword => $res) {
                    if (Tools::count($res) > 1) {
                        $mustselect = true;
                        break;
                    }
                }
            } else {
                if ($this->m_mode == 'select') {
                    // In single select mode, we show the selector if they all return
                    // just one match together.
                    $total = 0;
                    foreach ($this->m_matches as $keyword => $res) {
                        $total += Tools::count($res);
                    }
                    $mustselect = ($total > 1);
                }
            }

            if ($mustselect) {
                Tools::triggerError($rec, $this->fieldName(), 'fsa_pleasemakeselection');

                return false;
            }
        }

        return true;
    }

    public function edit($record, $fieldprefix, $mode)
    {
        // There are 2 possibilities. Either we are going to search,
        // in which case we show a searchbox.
        // Or, a search has already been performed but multiple
        // matches have been found and an atkerror was set.
        // In this case, we show the selects.
        $select = false;

        if (isset($record['atkerror'])) {
            foreach ($record['atkerror'] as $error) {
                if ($error['attrib_name'] === $this->fieldName()) {
                    $select = true;
                }
            }
        }

        if ($select && $this->createSearchNodeInstance()) {
            $res = '';
            $notempty = false;

            // First lets get the results, which were lost during the redirect
            $this->m_matches = $this->getMatches($record[$this->fieldName()]);

            // Second check if we actually found anything
            if ($this->m_matches) {
                foreach ($this->m_matches as $match) {
                    if (!empty($match)) {
                        $notempty = true;
                        continue;
                    }
                }
                if (!$notempty) {
                    return Tools::atktext('no_results_found');
                }
            }

            if ($this->m_mode == 'multiselect' && Tools::count($this->m_matches) > 1) {
                $optionArray = $valueArray = [];

                foreach ($this->m_matches as $keyword => $matches) {
                    for ($i = 0, $_i = Tools::count($matches); $i < $_i; ++$i) {
                        $optionArray[] = $this->m_searchnodeInstance->descriptor($matches[$i]);
                        $valueArray[] = $this->m_searchnodeInstance->primaryKeyString($matches[$i]);
                    }
                }

                $attrib = new MultiSelectAttribute($this->m_name, $optionArray, $valueArray, 1,
                    self::AF_NO_LABEL | MultiSelectAttribute::AF_CHECK_ALL | MultiSelectAttribute::AF_LINKS_BOTTOM);
                $res .= $attrib->edit($record, $fieldprefix, $mode);
            } else {
                if ($this->m_mode == 'select' || ($this->m_mode == 'multiselect' && Tools::count($this->m_matches) == 1)) {
                    // Select one record from all matches.
                    $res .= '<SELECT NAME="'.$this->getHtmlName($fieldprefix).'[]" class="form-control select-standard">';
                    $res .= '<OPTION VALUE="">'.Tools::atktext('select_none');
                    $selects = [];
                    foreach ($this->m_matches as $keyword => $matches) {
                        for ($i = 0, $_i = Tools::count($matches); $i < $_i; ++$i) {
                            $item = '<OPTION VALUE="'.htmlspecialchars($this->m_searchnodeInstance->primaryKeyString($matches[$i])).'">'.$this->m_searchnodeInstance->descriptor($matches[$i]);
                            if (!in_array($item, $selects)) {
                                $selects[] = $item;
                            }
                        }
                        $res .= implode("\n", $selects);
                    }
                    $res .= '</SELECT>';
                } else {
                    if ($this->m_mode == 'selectperkeyword') {
                        // Select one record per keyword.
                        $res = '<table border="0">';
                        foreach ($this->m_matches as $keyword => $matches) {
                            if (Tools::count($matches) > 0) {
                                $res .= '<tr><td>\''.$keyword.'\': </td><td><SELECT NAME="'.$this->getHtmlName($fieldprefix).'[]" class="form-control select-standard">';
                                $res .= '<OPTION VALUE="">'.Tools::atktext('select_none');
                                for ($i = 0, $_i = Tools::count($matches); $i < $_i; ++$i) {
                                    $res .= '<OPTION VALUE="'.htmlspecialchars($this->m_searchnodeInstance->primaryKeyString($matches[$i])).'">'.$this->m_searchnodeInstance->descriptor($matches[$i]);
                                }
                                $res .= '</SELECT></td></tr>';
                            }
                        }
                        $res .= '</table>';
                    }
                }
            }

            return $res;
        } else {
            $record = ''; // clear the record so we always start with an empty
            // searchbox.
            return parent::edit($record, $fieldprefix, $mode);
        }
    }

    /**
     * The actual function that does the searching.
     *
     * @param string $searchstring The string to search for
     *
     * @return array The matches
     */
    public function getMatches($searchstring)
    {
        Tools::atkdebug('Performing search');
        $result = [];

        if ($this->createSearchNodeInstance() && $searchstring != '') {
            $this->m_searchnodeInstance->addFilter($this->getDestinationFilter());
            $tokens = explode(',', $searchstring);
            foreach ($tokens as $token) {
                $token = trim($token);
                $result[$token] = $this->m_searchnodeInstance->searchDb($token);
            }
        }

        return $result;
    }

    /**
     * Override the store method of this attribute to search.
     *
     * @param Db $db
     * @param array $rec The record
     * @param string $mode
     *
     * @return bool
     */
    public function store($db, $rec, $mode)
    {
        $resultset = [];

        if (is_array($rec[$this->fieldName()])) {
            // If the value is an array, this means we must have come from a select.
            // The user has selected some options, and we must process those.
            // First, load the records, based on the where clauses.
            $wheres = [];
            $matches = $rec[$this->fieldName()];
            for ($i = 0, $_i = Tools::count($matches); $i < $_i; ++$i) {
                if ($matches[$i] != '') {
                    $wheres[] = $matches[$i];
                }
            }
            if (Tools::count($wheres) && $this->createSearchNodeInstance()) {
                $whereclause = '(('.implode(') OR (', $wheres).'))';

                $resultset = $this->m_searchnodeInstance->select($whereclause)->excludes($this->m_searchnodeInstance->m_listExcludes)->mode('admin')->fetchAll();
            }
        } else {
            if (Tools::count($this->m_matches) > 0) {
                // We didn't come from a select, but we found something anyway.
                // Depending on our mode parameter, we either pass all records to
                // the callback, or the first for every keyword, or the very first.
                if ($this->m_mode == 'all') {
                    // Pass all matches.
                    foreach ($this->m_matches as $keyword => $matches) {
                        for ($i = 0, $_i = Tools::count($matches); $i < $_i; ++$i) {
                            // Make sure there are no duplicates
                            if (!in_array($matches[$i], $resultset)) {
                                $resultset[] = $matches[$i];
                            }
                        }
                    }
                } else {
                    if ($this->m_mode == 'firstperkeyword') {
                        // Pass first matches of all keywords.
                        foreach ($this->m_matches as $keyword => $matches) {
                            if (Tools::count($matches)) {
                                $resultset[] = $matches[0];
                            }
                        }
                    } else {
                        if ($this->m_mode == 'first') {
                            // Pass only the first record of the first match.
                            if (Tools::count($this->m_matches)) {
                                $first = reset($this->m_matches);
                                if (Tools::count($first)) {
                                    $resultset[] = $first[0];
                                }
                            }
                        } else {
                            // We get here if one of the SELECT modes is active, but no
                            // selection was made. Getting here means that the validate()
                            // method above decided that presenting a selector was not
                            // necessary. We trust that judgement, and pass all records
                            // that were found.

                            foreach ($this->m_matches as $keyword => $matches) {
                                for ($i = 0, $_i = Tools::count($matches); $i < $_i; ++$i) {
                                    // Make sure there are no duplicates
                                    if (!in_array($matches[$i], $resultset)) {
                                        $resultset[] = $matches[$i];
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }

        if (Tools::count($resultset)) {
            if (method_exists($this->m_ownerInstance, $this->m_callback)) {
                $funcname = $this->m_callback;

                return $this->m_ownerInstance->$funcname($rec, $resultset);
            }
        }

        return true;
    }


    public function load($db, $record, $mode)
    {
        //noop
    }

    
    public function addToQuery($query, $tablename = '', $fieldaliasprefix = '', &$record, $level = 0, $mode = '')
    {
        //noop
    }

    public function hide($record, $fieldprefix, $mode)
    {
        //noop
    }

    public function search($atksearch, $extended = false, $fieldprefix = '', DataGrid $grid = null)
    {
        //noop
    }

    /**
     * Dummy method to prevent loading/storing of data.
     *
     * @return array empty array
     */
    public function getSearchModes()
    {
        return [];
    }

    public function searchCondition($query, $table, $value, $searchmode, $fieldaliasprefix = '')
    {
        //Dummy method to prevent loading/storing of data.
    }

    public function getSearchCondition(Query $query, $table, $value, $searchmode, $fieldname = '')
    {
        //Dummy method to prevent loading/storing of data.
    }

    public function fetchMeta($metadata)
    {
        //Dummy method to prevent loading/storing of data.
    }

    public function dbFieldSize()
    {
        // Dummy method to prevent loading/storing of data.
    }

    /**
     * Adds a filter on the instance of the searchnode.
     *
     * @param string $filter The fieldname you want to filter OR a SQL where
     *                       clause expression.
     * @param string $value Required value. (Ommit this parameter if you pass
     *                       an SQL expression for $filter.)
     */
    public function addSearchFilter($filter, $value = '')
    {
        if (!$this->m_searchnodeInstance) {
            $this->createSearchNodeInstance();
        }
        $this->m_searchnodeInstance->addFilter($filter, $value);
    }

    /**
     * Returns the destination filter.
     *
     * @return string The destination filter.
     */
    public function getDestinationFilter()
    {
        return $this->m_destinationFilter;
    }

    /**
     * Sets the destination filter.
     *
     * @param string $filter The destination filter.
     */
    public function setDestinationFilter($filter)
    {
        $this->m_destinationFilter = $filter;
    }
}

<?php

namespace Sintattica\Atk\Relations;

use Sintattica\Atk\Core\Atk;
use Sintattica\Atk\Core\Tools;
use Sintattica\Atk\DataGrid\DataGrid;
use Sintattica\Atk\Session\SessionManager;
use Sintattica\Atk\Core\Node;
use Sintattica\Atk\Db\Db;
use Sintattica\Atk\Db\Query;
use Sintattica\Atk\Db\QueryPart;

/**
 * Many to many relation. Should not be used directly.
 *
 * This class is used as base class for special kinds of manytomany
 * relations, like the manyboolrelation. Note that most many-to-many
 * relationships can be normalized to a combination of one-to-many and
 * many-to-one relations.
 *
 * @todo Improve multi-field support. For example setOwnerFields with multiple fields
 *       doesn't work properly at the moment. But it seems more code does not take
 *       multi-field support into account.
 *
 * @abstract
 *
 * @author Ivo Jansch <ivo@achievo.org>
 */
class ManyToManyRelation extends Relation
{
    const AF_MANYTOMANY_DETAILVIEW = 536870912;

    public $m_localKey = '';
    public $m_remoteKey = '';
    public $m_link = '';

    /* @var Node null */
    public $m_linkInstance = null;

    public $m_store_deletion_filter = '';
    public $m_localFilter = null;
    protected $m_ownerFields = null;
    protected $m_limit;
    private $m_selectableRecordsCache = [];
    private $m_selectableRecordCountCache = [];

    /**
     * Constructor.
     *
     * @param string $name The name of the relation
     * @param int $flags Flags for the relation.
     * @param string $link The full name of the node that is used as
     *                            intermediairy node. The intermediairy node is
     *                            assumed to have 2 attributes that are named
     *                            after the nodes at both ends of the relation.
     *                            For example, if node 'project' has a M2M relation
     *                            with 'activity', then the intermediairy node
     *                            'project_activity' is assumed to have an attribute
     *                            named 'project' and one that is named 'activity'.
     *                            You can set your own keys by calling setLocalKey()
     *                            and setRemoteKey()
     * @param string $destination The full name of the node that is the other
     *                            end of the relation.
     * @param string|array $local_key field for localKey
     * @param string $remote_key field for remoteKey
     */
    public function __construct($name, $flags = 0, $link, $destination, $local_key = null, $remote_key = null)
    {
        $flags = $flags | self::AF_CASCADE_DELETE | self::AF_NO_SORT;
        $this->m_link = $link;
        parent::__construct($name, $flags, $destination);

        if ($local_key != null) {
            $this->setLocalKey($local_key);
        }
        if ($remote_key != null) {
            $this->setRemoteKey($remote_key);
        }
    }

    /**
     * Returns the selectable records. Checks for an override in the owner instance
     * with name <attribname>_selection.
     *
     * @param array $record
     * @param string $mode
     * @param bool $force
     *
     * @return array
     */
    public function _getSelectableRecords($record = [], $mode = '', $force = false)
    {
        $method = $this->fieldName().'_selection';
        if (method_exists($this->m_ownerInstance, $method)) {
            return $this->m_ownerInstance->$method($record, $mode);
        } else {
            return $this->getSelectableRecords($record, $mode, $force);
        }
    }

    /**
     * Returns the selectable record count.
     *
     * @param array $record
     * @param string $mode
     *
     * @return int
     */
    protected function _getSelectableRecordCount($record = [], $mode = '')
    {
        $method = $this->fieldName().'_selection';
        if (method_exists($this->m_ownerInstance, $method)) {
            return Tools::count($this->_getSelectableRecords($record, $mode));
        } else {
            return $this->getSelectableRecordCount($record, $mode);
        }
    }

    /**
     * Returns the selectable record count. The count is cached unless the
     * $force parameter is set to true.
     *
     * @param array $record
     * @param string $mode
     * @param bool $force
     *
     * @return int
     */
    public function getSelectableRecordCount($record = [], $mode = '', $force = false)
    {
        if (!$this->createDestination()) {
            return 0;
        }

        $filter = $this->parseFilter($record);

        $cacheKey = md5(serialize($filter));
        if (!array_key_exists($cacheKey, $this->m_selectableRecordCountCache) || $force) {
            $this->m_selectableRecordCountCache[$cacheKey] = $this->getDestination()->select($filter)->getRowCount();
        }

        return $this->m_selectableRecordCountCache[$cacheKey];
    }

    /**
     * Returns the selectable records for this relation. The records are cached
     * unless the $force parameter is set to true.
     *
     * @param array $record
     * @param string $mode
     * @param bool $force
     *
     * @return array selectable records
     */
    public function getSelectableRecords($record = [], $mode = '', $force = false)
    {
        if (!$this->createDestination()) {
            return [];
        }

        $filter = $this->parseFilter($record);

        $cacheKey = md5(serialize($filter));
        if (!array_key_exists($cacheKey, $this->m_selectableRecordsCache) || $force) {
            $this->m_selectableRecordsCache[$cacheKey] = $this->getDestination()->select($filter)->limit(is_numeric($this->m_limit) ? $this->m_limit : -1)->includes(Tools::atk_array_merge($this->m_destInstance->descriptorFields(),
                $this->m_destInstance->m_primaryKey))->fetchAll();
        }

        return $this->m_selectableRecordsCache[$cacheKey];
    }

    /**
     * Clears the selectable record count and records cache.
     */
    public function clearSelectableCache()
    {
        $this->m_selectableRecordCountCache = [];
        $this->m_selectableRecordsCache = [];
    }

    /**
     * Returns the primary keys of the currently selected records retrieved
     * from the given record.
     *
     * @param array $record current record
     *
     * @return array list of selected record keys
     */
    public function getSelectedRecords($record)
    {
        $keys = [];

        if (isset($record[$this->fieldName()])) {
            for ($i = 0; $i < Tools::count($record[$this->fieldName()]); ++$i) {
                if (is_array($record[$this->fieldName()][$i][$this->getRemoteKey()])) {
                    $key = $this->m_destInstance->primaryKeyString($record[$this->fieldName()][$i][$this->getRemoteKey()]);
                } else {
                    $key = $this->m_destInstance->primaryKeyString(array($this->m_destInstance->primaryKeyField() => $record[$this->fieldName()][$i][$this->getRemoteKey()]));
                }

                $keys[] = $key;
            }
        }

        return $keys;
    }

    /**
     * Create instance of the intermediary link node.
     *
     * If succesful, the instance is stored in the m_linkInstance member
     * variable.
     *
     * @return bool True if successful, false if not.
     */
    public function createLink()
    {
        if ($this->m_linkInstance == null) {
            $atk = Atk::getInstance();
            $this->m_linkInstance = $atk->newAtkNode($this->m_link);

            // Validate if destination was created succesfully
            if (!is_object($this->m_linkInstance)) {
                Tools::atkerror("Relation with unknown nodetype '".$this->m_link."' (in node '".$this->m_owner."')");
                $this->m_linkInstance = null;

                return false;
            }
        }

        return true;
    }

    /**
     * Returns the link instance.
     *
     * The link has to be created first for this method to work.
     *
     * @return Node link instance
     */
    public function getLink()
    {
        return $this->m_linkInstance;
    }

    /**
     * Get the name of the attribute of the intermediairy node that points
     * to the master node.
     *
     * @return string The name of the attribute.
     */
    public function getLocalKey()
    {
        if ($this->m_localKey == '') {
            $this->m_localKey = $this->determineKeyName($this->m_owner);
        }

        return $this->m_localKey;
    }

    /**
     * Change the name of the attribute of the intermediairy node that points
     * to the master node.
     *
     * @param string|array $attributename The name of the attribute.
     */
    public function setLocalKey($attributename)
    {
        $this->m_localKey = $attributename;
    }

    /**
     * Get the name of the attribute of the intermediairy node that points
     * to the node on the other side of the relation.
     *
     * @return string The name of the attribute.
     */
    public function getRemoteKey()
    {
        $this->createDestination();

        if ($this->m_remoteKey == '') {
            list(, $nodename) = explode('.', $this->m_destination);
            $this->m_remoteKey = $this->determineKeyName($nodename);
        }

        return $this->m_remoteKey;
    }

    /**
     * Sets the owner fields in the owner instance. The owner fields are
     * the attribute(s) of the owner instance which map to the local key
     * of the link node.
     *
     * @param array $ownerfields
     */
    public function setOwnerFields($ownerfields)
    {
        $this->m_ownerFields = $ownerfields;
    }

    /**
     * Returns the owner fields. The owners fields are the attribute(s)
     * of the owner instance which map to the local key of the link node.
     *
     * @return array owner fields
     */
    public function getOwnerFields()
    {
        if (is_array($this->m_ownerFields) && !empty($this->m_ownerFields)) {
            return $this->m_ownerFields;
        }

        return $this->m_ownerInstance->m_primaryKey;
    }

    /**
     * Determine the name of the foreign key based on the name of the
     *  relation.
     *
     * @param string $name the name of the relation
     *
     * @return string the probable name of the foreign key
     */
    public function determineKeyName($name)
    {
        if ($this->createLink()) {
            if (isset($this->m_linkInstance->m_attribList[$name])) {
                // there's an attribute with the same name as the role.
                return $name;
            } else {
                // find out if there's a field with the same name with _id appended to it
                if (isset($this->m_linkInstance->m_attribList[$name.'_id'])) {
                    return $name.'_id';
                }
            }
        }

        return $name;
    }

    /**
     * Change the name of the attribute of the intermediairy node that points
     * to the node on the other side of the relation.
     *
     * @param string $attributename The name of the attribute.
     */
    public function setRemoteKey($attributename)
    {
        $this->m_remoteKey = $attributename;
    }

    /**
     * Returns a displayable string for this value.
     *
     * @param array $record The record that holds the value for this attribute
     * @param string $mode The display mode ("view" for viewpages, or "list"
     *                       for displaying in recordlists, "edit" for
     *                       displaying in editscreens, "add" for displaying in
     *                       add screens. "csv" for csv files. Applications can
     *                       use additional modes.
     *
     * @return string a displayable string for this value
     */
    public function display($record, $mode)
    {
        $result = '';
        if (!$this->createDestination() || !Tools::atk_value_in_array($record[$this->fieldName()])) {
            return in_array($mode, array('csv', 'plain')) ? '' : $this->text('none');
        }

        $recordset = [];
        $remotekey = $this->getRemoteKey();
        foreach ($record[$this->fieldName()] as $dest) {
            $rec = $dest[$remotekey];
            if (!is_array($rec)) {
                if (empty($rec)) {
                    continue;
                }
                $selector = Query::simpleValueCondition($this->m_destInstance->m_table, $this->m_destInstance->primaryKeyField(), $rec);
                $rec = $this->m_destInstance->select($selector)->includes($this->m_destInstance->descriptorFields())->getFirstRow();
            }
            $descr = $this->m_destInstance->descriptor($rec);
            if (!in_array($mode, array('csv', 'plain'))) {
                $descr = htmlspecialchars($descr);
            }
            if ($this->hasFlag(self::AF_MANYTOMANY_DETAILVIEW) && $this->m_destInstance->allowed('view')) {
                $descr = Tools::href(Tools::dispatch_url($this->m_destination, 'view', array('atkselector' => $this->getDestination()->primaryKeyString($rec))),
                    $descr, SessionManager::SESSION_NESTED);
            }
            $recordset[] = $descr;
        }
        if (!in_array($mode, array('csv', 'plain'))) {
            $result = '<ul><li>'.implode('<li>', $recordset).'</ul>';
        } else {
            $result = implode(', ', $recordset);
        }
        return $result;
    }

    /**
     * Dummy function.
     *
     * @param array $record The record that holds the value for this attribute.
     * @param string $fieldprefix The fieldprefix to put in front of the name
     *                            of any html form element for this attribute.
     * @param string $mode The mode we're in ('add' or 'edit')
     *
     * @return string A piece of htmlcode for editing this attribute
     */
    public function edit($record, $fieldprefix, $mode)
    {
    }

    public function addToQuery($query, $tablename = '', $fieldaliasprefix = '', &$record, $level = 0, $mode = '')
    {
        // we don't add ourselves to the query;
    }

    /**
     * load function.
     *
     * @param Db $db
     * @param array $record
     * @param string $mode
     *
     * @return array
     */
    public function load($db, $record, $mode)
    {
        if ($this->createLink()) {
            $where = $this->_getLoadWhereClause($record);
            $rel = $this->m_linkInstance;

            return $rel->select($where)->fetchAll();
        }

        return [];
    }

    /**
     * Get where clause for loading the record from the linkInstance table
     *
     * @param array $record The record
     *
     * @return string The where clause
     */
    public function _getLoadWhereClause($record)
    {
        $whereelems = [];
        $localkey = $this->getLocalKey();
        if (!is_array($localkey)) {
            $localkey = array($localkey);
        }

        $ownerfields = $this->getOwnerFields();

        for ($i = 0, $_i = Tools::count($localkey); $i < $_i; ++$i) {
            $primkeyattr = $this->m_ownerInstance->m_attribList[$ownerfields[$i]];

            if (!$primkeyattr->isEmpty($record)) {
                $whereelems[] = Query::simpleValueCondition($this->m_linkInstance->m_table, $localkey[$i], $primkeyattr->value2db($record));
            }
        }

        if ($this->m_localFilter != null) {
            $whereelems[] = new QueryPart($this->m_localFilter);
        }

        return QueryPart::implode('AND', $whereelems);
    }

    /**
     * delete relational records..
     *
     * @param $record array $record The record
     *
     * @return bool
     */
    public function delete($record)
    {
        if ($this->createLink()) {
            $rel = $this->m_linkInstance;
            $where = $this->_getLoadWhereClause($record);
            if ($where != '') {
                return $rel->deleteDb($where);
            }
        }

        return false;
    }

    /**
     * Returns an array with the existing records indexed by their
     * primary key selector string.
     *
     * @param Db $db database instance
     * @param array $record record
     * @param string $mode mode
     *
     * @return array
     */
    protected function _getExistingRecordsByKey($db, $record, $mode)
    {
        $existingRecords = $this->load($db, $record, $mode);
        $existingRecordsByKey = [];
        foreach ($existingRecords as $existingRecord) {
            $existingRecordKey = is_array($existingRecord[$this->getRemoteKey()]) ? $existingRecord[$this->getRemoteKey()][$this->getDestination()->primaryKeyField()] : $existingRecord[$this->getRemoteKey()];

            $existingRecordsByKey[$existingRecordKey] = $existingRecord;
        }

        return $existingRecordsByKey;
    }

    /**
     * Extracts the selected records from the owner instance record for
     * this relation and index them by their primary key selector string.
     *
     * @param array $record record
     *
     * @return array
     */
    protected function _extractSelectedRecordsByKey($record)
    {
        $selectedRecordsByKey = [];

        if (isset($record[$this->fieldName()])) {
            foreach ($record[$this->fieldName()] as $selectedRecord) {
                $selectedKey = is_array($selectedRecord[$this->getRemoteKey()]) ? $selectedRecord[$this->getRemoteKey()][$this->getDestination()->primaryKeyField()] : $selectedRecord[$this->getRemoteKey()];
                $selectedRecordsByKey[$selectedKey] = $selectedRecord;
            }
        }

        return $selectedRecordsByKey;
    }

    /**
     * Delete existing link record.
     *
     * @param array $record link record
     *
     * @return bool
     */
    protected function _deleteRecord($record)
    {
        $selector = $this->getLink()->primaryKey($record);

        if (empty($selector)) {
            Tools::atkerror('primaryKey-selector for link node is empty. Did you add an self::AF_PRIMARY flag to the primary key field(s) of the intermediate node? Deleting records aborted to prevent dataloss.');

            return false;
        }

        // append the store deletion filter (if set)
        if (!empty($this->m_store_deletion_filter)) {
            $selector = QueryPart::implode('AND', [$selector, new QueryPart($this->m_store_deletion_filter)]);
        }

        return $this->getLink()->deleteDb($selector);
    }

    /**
     * Update existing link record.
     *
     * @param array $record link record
     * @param int $index (new) index (0-based)
     *
     * @return bool
     */
    protected function _updateRecord($record, $index)
    {
        // don't do anything by default
        return true;
    }

    /**
     * Create new link record.
     *
     * @param string $selectedKey primary key selector string of destination record
     * @param array $selectedRecord selected destination record (might only contain the key attributes)
     * @param array $ownerRecord owner instance record
     * @param int $index (new) index (0-based)
     *
     * @return array new link record (not saved yet!)
     */
    protected function _createRecord($selectedKey, $selectedRecord, $ownerRecord, $index)
    {
        $record = array_merge($this->getLink()->initial_values(), $selectedRecord);
        $record[$this->getRemoteKey()] = $selectedKey;

        $ownerFields = $this->getOwnerFields();
        $localKey = $this->getLocalKey();

        if (is_array($localKey)) {
            for ($j = 0; $j < Tools::count($localKey); ++$j) {
                $record[$localKey[0]][$ownerFields[$j]] = $ownerRecord[$ownerFields[$j]];
            }
        } else {
            $record[$localKey] = $ownerRecord[$ownerFields[$j]];
        }

        return $record;
    }

    /**
     * Add new link record to the database.
     *
     * @param array $record link record
     * @param int $index (new) index (0-based)
     * @param string $mode storage mode
     *
     * @return bool
     */
    protected function _addRecord($record, $index, $mode)
    {
        return $this->getLink()->addDb($record, true, $mode);
    }

    /**
     * Stores the values in the database.
     *
     * @param Db $db database instance
     * @param array $record owner instance record
     * @param string $mode storage mode
     *
     * @return bool
     */
    public function store($db, $record, $mode)
    {
        $this->createLink();
        $this->createDestination();

        $existingRecordsByKey = $this->_getExistingRecordsByKey($db, $record, $mode);
        $existingRecordsKeys = array_keys($existingRecordsByKey);

        $selectedRecordsByKey = $this->_extractSelectedRecordsByKey($record);
        $selectedRecordsKeys = array_keys($selectedRecordsByKey);

        // first delete the existing records that aren't selected anymore
        $deleteKeys = array_diff($existingRecordsKeys, $selectedRecordsKeys);

        foreach ($deleteKeys as $deleteKey) {
            if (!$this->_deleteRecord($existingRecordsByKey[$deleteKey])) {
                return false;
            }
        }

        // then add new or update existing records
        $index = 0;
        foreach ($selectedRecordsByKey as $selectedKey => $selectedRecord) {
            if (isset($existingRecordsByKey[$selectedKey])) {
                if (!$this->_updateRecord($existingRecordsByKey[$selectedKey], $index)) {
                    return false;
                }
            } else {
                $newRecord = $this->_createRecord($selectedKey, $selectedRecord, $record, $index);

                if (!$this->_addRecord($newRecord, $index, $mode)) {
                    return false;
                }
            }

            ++$index;
        }

        return true;
    }

    /**
     * Check if the attribute is empty.
     *
     * @param array $postvars
     *
     * @return true if it's empty
     */
    public function isEmpty($postvars)
    {
        return !is_array($postvars[$this->fieldName()]) || Tools::count($postvars[$this->fieldName()]) == 0;
    }

    /**
     * Returns a piece of html code for hiding this attribute in an HTML form,
     * while still posting its value. (<input type="hidden">).
     *
     * @param array $record
     * @param string $fieldprefix
     * @param string $mode
     *
     * @return string html
     */
    public function hide($record, $fieldprefix, $mode)
    {
        $result = '';
        if (is_array(Tools::atkArrayNvl($record, $this->fieldName())) && $this->createDestination()) {
            $ownerFields = $this->getOwnerFields();
            for ($i = 0, $_i = Tools::count($record[$this->fieldName()]); $i < $_i; ++$i) {

                $localKey = $this->getLocalKey();
                if (!is_array($localKey)) {
                    $localKey = [$localKey];
                }
                foreach ($localKey as $key) {
                    if (Tools::atkArrayNvl($record[$this->fieldName()][$i], $key)) {
                        $result .= '<input type="hidden" name="'.$this->getHtmlName($fieldprefix).'['.$i.']['.$key.']" value="'.$this->checkKeyDimension($record[$this->fieldName()][$i][$key], $ownerFields[0]).'">';
                    }
                }

                if (Tools::atkArrayNvl($record[$this->fieldName()][$i], $this->getRemoteKey())) {
                    $result .= '<input type="hidden" name="'.$this->getHtmlName($fieldprefix).'['.$i.']['.$this->getRemoteKey().']" value="'.$this->checkKeyDimension($record[$this->fieldName()][$i][$this->getRemoteKey()],
                            $this->m_destInstance->primaryKeyField()).'">';
                }
            }
        }

        return $result;
    }

    /**
     * Returns a piece of html code that can be used in a form to search.
     *
     * @param array $atksearch Array with values from POST request
     * @param bool $extended if set to false, a simple search input is
     *                            returned for use in the searchbar of the
     *                            recordlist. If set to true, a more extended
     *                            search may be returned for the 'extended'
     *                            search page. The Attribute does not
     *                            make a difference for $extended is true, but
     *                            derived attributes may reimplement this.
     * @param string $fieldprefix The fieldprefix of this attribute's HTML element.
     * @param DataGrid $grid
     *
     * @return string Piece of html code
     */
    public function search($atksearch, $extended = false, $fieldprefix = '', DataGrid $grid = null)
    {
        $this->createDestination();

        $id = $this->getHtmlId($fieldprefix);
        $name = $this->getSearchFieldName($fieldprefix);

        $selectOptions = [];
        $selectOptions['enable-select2'] = true;
        $selectOptions['dropdown-auto-width'] = true;
        $selectOptions['minimum-results-for-search'] = 10;
        if ($extended) {
            $selectOptions['placeholder'] = Tools::atktext('search_all');
        }

        //width always auto
        $selectOptions['width'] = 'auto';

        $selectOptions = array_merge($selectOptions, $this->m_select2Options['search']);
        $data = '';
        foreach ($selectOptions as $k => $v) {
            $data .= ' data-'.$k.'="'.htmlspecialchars($v).'"';
        }


        // now select all records
        $recordset = $this->m_destInstance->select()->includes(Tools::atk_array_merge($this->m_destInstance->descriptorFields(),
            $this->m_destInstance->m_primaryKey))->fetchAll();
        $result = '<select class="form-control"'.$data;
        if ($extended) {
            $result .= 'multiple="multiple" size="'.min(5, Tools::count($recordset) + 1).'"';
        }

        $result .= 'id="'.$id.'" name="'.$name.'[]">';

        $pkfield = $this->m_destInstance->primaryKeyField();

        if (!$extended) {
            $result .= '<option value="">'.Tools::atktext('search_all', 'atk').'</option>';
        }

        for ($i = 0; $i < Tools::count($recordset); ++$i) {
            $pk = $recordset[$i][$pkfield];
            if (!empty($atksearch[$this->getHtmlName()]) && Tools::atk_in_array($pk, $atksearch[$this->getHtmlName()])) {
                $sel = ' selected="selected"';
            } else {
                $sel = '';
            }
            $result .= '<option value="'.$pk.'"'.$sel.'>'.$this->m_destInstance->descriptor($recordset[$i]).'</option>';
        }
        $result .= '</select>';
        $result .= "<script>ATK.Tools.enableSelect2ForSelect('#$id');</script>";

        return $result;
    }

    /**
     * Creates an search condition for a given search value.
     *
     * @param Query $query  The query to which the condition will be added.
     * @param string $table The name of the table in which this attribute
     *                                 is stored
     * @param mixed $value The value the user has entered in the searchbox
     * @param string $searchmode The searchmode to use. This can be any one
     *                                 of the supported modes, as returned by this
     *                                 attribute's getSearchModes() method.
     * @param string $fieldaliasprefix optional prefix for the fieldalias in the table
     */
    public function getSearchCondition(Query $query, $table, $value, $searchmode, $fieldaliasprefix = '')
    {
        if (!is_array($value) || empty($value) || $value[0] == '') {
            // This last condition is for when the user selected the 'search all' option, in which case, we don't add conditions at all.
            return null;
        }
        // We only support 'exact' matches.
        // But you can select more than one value, which we search using the IN() statement,
        // which should work in any ansi compatible database.
        $ownerFields = $this->getOwnerFields();
        $this->createLink();
        $query->addJoin($this->m_linkInstance->m_table, $this->fieldName(), [$this->getLocalKey() => [$table, $ownerFields[0]]], false);
        $query->setDistinct(true);

        if (Tools::count($value) == 1) { // exactly one value
            return $query->exactCondition(Db::quoteIdentifier($this->fieldName(), $this->getRemoteKey()), $value[0]);
        } else { // search for more values using IN()
            return $query->inCondition(Db::quoteIdentifier($this->fieldName(), $this->getRemoteKey()), $value);
        }
    }

    /**
     * Checks if a key is not an array.
     *
     * @param string $key field containing the key values
     * @param string $field field to return if an array
     *
     * @return string of $field
     */
    public function checkKeyDimension($key, $field = 'id')
    {
        if (is_array($key)) {
            return $key[$field];
        }

        return $key;
    }

    /**
     * Fetch value. If nothing selected, return empty array instead
     * of nothing.
     *
     * @param array $postvars
     *
     * @return mixed
     */
    public function fetchValue($postvars)
    {
        return parent::fetchValue($postvars) ?? [];
    }

    /**
     * Function adds a custom filter that is used when deleting items during the store() function.
     *
     * Example:
     * Normally the delete function would do something like this:
     *
     * DELETE FROM phase WHERE phase.template NOT IN (1,2,3)
     *
     * If the template field is NULL, although it is not specified in the NOT IN (1,2,3), it will not be deleted.
     * An extra check can be added just in case the template value is not NULL but 0 or '' (which would delete the phase).
     *
     * @param string $filter The filter that is used when deleting records in the store function.
     */
    public function setStoreDeletionFilter($filter)
    {
        $this->m_store_deletion_filter = $filter;
    }

    /**
     * Local filter is used to only show values that are once selected
     * that comply with the local filter. A local filter is also automatically
     * set as store deletion filter.
     *
     * @param string $filter filter
     */
    public function setLocalFilter($filter)
    {
        $this->setStoreDeletionFilter($filter);
        $this->m_localFilter = $filter;
    }
}

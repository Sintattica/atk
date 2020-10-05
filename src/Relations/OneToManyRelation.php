<?php

namespace Sintattica\Atk\Relations;

use Sintattica\Atk\Ui\Page;
use Sintattica\Atk\Core\Tools;
use Sintattica\Atk\Core\Config;
use Sintattica\Atk\DataGrid\DataGrid;
use Sintattica\Atk\Db\Db;
use Sintattica\Atk\Session\SessionManager;
use Sintattica\Atk\Session\SessionStore;
use Sintattica\Atk\Core\Atk;
use Sintattica\Atk\Core\Node;
use Sintattica\Atk\Db\Query;
use Sintattica\Atk\Db\QueryPart;
use Exception;

/**
 * Implementation of one-to-many relationships.
 *
 * Can be used to create one to many relations ('1 library has N books').
 * A common term for this type of relation is a master-detail relationship.
 * The detailrecords can be edited inline.
 *
 * @author Ivo Jansch <ivo@achievo.org>
 */
class OneToManyRelation extends Relation
{
    /**
     * Only allow deletion of master item when there are no child records.
     */
    const AF_RESTRICTED_DELETE = 33554432;

    /**
     * Show the OTM in add mode.
     * Warning! Not on by default because this only works in simple cases.
     *
     * What ATK does is, when you are in OTM add mode, it stores everything you add
     * in the session, then when you're actually saving, it persists everything to
     * the database.
     *
     * However, as you may guess, not having an id will lead to strange results for:
     * - Nodes that use the foreign key in their descriptor
     * - Nodes with unique records (self::AF_UNIQUE always just checks the database)
     * - Combined primary keys
     */
    const AF_ONETOMANY_SHOW_ADD = 268435456;

    public $m_recordlist;

    /*
     * Instance of atk.recordlist.atkrecordlistcache
     * @access private
     * @var Object
     */
    public $m_recordlistcache;

    /*
     * List of keys from the destination node that refer to the master record.
     * @access private
     * @var array
     */
    public $m_refKey = [];

    /*
     * The maximum number of detail records. If the number of detail records
     * exceeds this maximum, the link for adding new details disappears.
     * @access private
     * @var int
     */
    public $m_maxRecords = 0;

    /*
     * The load method might build a custom filter. When it does, we might want
     * to use it again in other methods.
     * @access private
     * @var string
     */
    public $m_loadFilter = '';

    /*
     * The field that the foreign key in the destination points to.
     * Is set to the primary key if no value is provided.
     * @access private
     * @var array;
     */
    public $m_ownerFields = [];

    /**
     * Use referential key for load filter?
     *
     * @var bool
     */
    protected $m_useRefKeyForFilter = true;

    /*
     * Function names for recordlist header/footer generation
     *
     * @access private
     * @var string
     */
    public $m_headerName = '';
    public $m_footerName = '';

    /*
     * Fields to exclude in the grid
     *
     * @access private
     * @var array
     */
    public $m_excludes = [];

    /**
     * Default constructor.
     *
     * <b>Example: </b> Suppose a department has many employees. To edit the
     * list of employees in a department, this relationship can be built like
     * this, in the department node:
     * <code>
     * $this->add(new atkOneToManyRelation("employees", "mymod.employee", "department_id"));
     * </code>
     *
     * @param string $name The unique name of this relation within a node.
     *                            In contrast with most other attributes, the name
     *                            does not correspond to a database field. (Because
     *                            in one2many relations, the databasefield that
     *                            stores the link, is in the destination node and not
     *                            in the owner node).
     * @param int $flags Attribute flags that influence this attributes' behavior.
     * @param string $destination The node to which the relationship is made
     *                            (in module.nodename notation).
     * @param string|array $refKey For regular oneToMany relationships, $refKey is
     *                            name of the referential key in the destination
     *                            node. In the case of multi-foreign key
     *                            relationships, $refKey can be an array of fields.
     */
    public function __construct($name, $flags = 0, $destination, $refKey = '')
    {
        $flags = $flags | self::AF_NO_SORT | self::AF_HIDE_ADD;
        parent::__construct($name, $flags, $destination);

        if (is_array($refKey)) {
            $this->m_refKey = $refKey;
        } else {
            if (empty($refKey)) {
                $this->m_refKey = [];
            } else {
                $this->m_refKey[] = $refKey;
            }
        }
        $this->setGridExcludes($this->m_refKey);
    }

    public function addFlag($flag)
    {
        $ret = parent::addFlag($flag);
        if (Tools::hasFlag($this->m_flags, self::AF_ONETOMANY_SHOW_ADD)) {
            $this->removeFlag(self::AF_HIDE_ADD);
        }

        return $ret;
    }

    /**
     * Set the ownerfields.
     *
     * @param array $ownerfields
     */
    public function setOwnerFields($ownerfields)
    {
        $this->m_ownerFields = $ownerfields;
    }

    /**
     * Get the owner fields.
     *
     * @return mixed Array or String with ownerfield(s)
     */
    public function getOwnerFields()
    {
        if (is_array($this->m_ownerFields) && Tools::count($this->m_ownerFields) > 0) {
            return $this->m_ownerFields;
        }

        return $this->m_ownerInstance->m_primaryKey;
    }

    /**
     * Use referential key for filtering the records. If you disable this only the
     * explicitly set destination filter will be used.
     *
     * @param bool $useRefKey
     */
    public function setUseRefKeyForFilter($useRefKey)
    {
        $this->m_useRefKeyForFilter = $useRefKey;
    }

    /**
     * Create the datagrid for the edit and display actions. The datagrid is
     * configured with the correct node filter, excludes etc.
     *
     * The datagrid uses for both the edit and display actions the partial_grid
     * method to update it's view.
     *
     * @param array $record the record
     * @param string $mode the mode
     * @param string $action the action
     * @param bool $useSession use session?
     *
     * @return DataGrid grid
     */
    protected function createGrid($record, $mode, $action, $useSession = true)
    {
        $this->createDestination();

        $grid = DataGrid::create($this->m_destInstance, str_replace('.', '_', $this->getOwnerInstance()->atkNodeUri()).'_'.$this->getHtmlName().'_grid', null,
            true, $useSession);

        $grid->setMode($mode);
        $grid->setMasterNode($this->getOwnerInstance());
        $grid->setMasterRecord($record);

        $grid->removeFlag(DataGrid::EXTENDED_SEARCH);
        if ($action == 'view') {
            $grid->removeFlag(DataGrid::MULTI_RECORD_ACTIONS);
            $grid->removeFlag(DataGrid::MULTI_RECORD_PRIORITY_ACTIONS);
        }

        $grid->setBaseUrl(Tools::partial_url($this->getOwnerInstance()->atkNodeUri(), $action, 'attribute.'.$this->fieldName().'.grid'));

        $grid->setExcludes($this->getGridExcludes());

        $grid->addFilter($this->_getLoadWhereClause($record));
        if (!empty($this->m_destinationFilters)) {
            $grid->addFilter($this->parseFilter($record));
        }

        $this->modifyDataGrid($grid, DataGrid::CREATE);

        return $grid;
    }

    /**
     * Updates the datagrid for the edit and display actions.
     *
     * @return string grid html
     */
    public function partial_grid()
    {
        $this->createDestination();
        $node = $this->getDestination();

        try {
            $grid = DataGrid::resume($node);
            $this->modifyDataGrid($grid, DataGrid::RESUME);
        } catch (Exception $e) {
            $grid = DataGrid::create($node);
            $this->modifyDataGrid($grid, DataGrid::CREATE);
        }

        return $grid->render();
    }

    /**
     * Modify grid.
     *
     * @param DataGrid $grid grid
     * @param int $mode CREATE or RESUME
     */
    protected function modifyDataGrid(DataGrid $grid, $mode)
    {
        $method = 'modifyDataGrid';
        if (method_exists($this->getDestination(), $method)) {
            $this->getDestination()->$method($grid, $mode);
        }

        $method = $this->fieldName().'_modifyDataGrid';
        if (method_exists($this->getOwnerInstance(), $method)) {
            $this->getOwnerInstance()->$method($grid, $mode);
        }
    }

    /**
     * Returns a displayable string for this value, to be used in HTML pages.
     *
     * The atkOneToManyRelation displays a list of detail records in "view"
     * mode, in the form of a read-only data grid. In "list" mode, a plain
     * list of detail record descriptors is displayed.
     *
     * @param array $record The record that holds the value for this attribute
     * @param string $mode The display mode ("view" for viewpages, or "list"
     *                       for displaying in recordlists)
     *
     * @return string HTML String
     */
    public function display($record, $mode)
    {
        // for the view mode we use the datagrid and load the records ourselves
        if ($mode == 'view' || ($mode == 'edit' && $this->hasFlag(self::AF_READONLY_EDIT))) {
            $grid = $this->createGrid($record, 'admin', 'view');
            $grid->loadRecords(); // load records early

            if($mode == 'view') {
                $grid->setEmbedded(false);
            }

            // no records
            if ($grid->getCount() == 0) {
                if (!in_array($mode, array('csv', 'plain'))) {
                    return $this->text('none');
                } else {
                    return '';
                }
            }

            $actions = [];
            if (!$this->m_destInstance->hasFlag(Node::NF_NO_VIEW)) {
                $actions['view'] = Tools::dispatch_url($this->m_destination, 'view', ['atkselector' => '[pk]']);
            }

            $grid->setDefaultActions($actions);

            return $grid->render();
        }

        // records should be loaded inside the load method
        $records = $record[$this->fieldName()];

        // no records
        if (Tools::count($records) == 0) {
            return !in_array($mode, array('csv', 'plain', 'list')) ? $this->text('none') : '';
        }

        if ($mode == 'list') { // list mode
            $result = '<ul '.$this->getCSSClassAttribute().'>';

            foreach ($records as $current) {
                $result .= sprintf('<li>%s</li>', htmlspecialchars($this->m_destInstance->descriptor($current)));
            }

            $result .= '</ul>';

            return $result;
        } else { // csv / plain mode
            $result = '';

            foreach ($records as $i => $current) {
                $result .= ($i > 0 ? ', ' : '').$this->m_destInstance->descriptor($current);
            }

            return $result;
        }
    }

    public function edit($record, $fieldprefix, $mode)
    {
        $page = Page::getInstance();
        $page->register_script(Config::getGlobal('assets_url').'javascript/onetomanyrelation.js');

        $grid = $this->createGrid($record, 'admin', $mode);

        $params = [];
        if ($mode === 'add') {
            //All actions in the grid should be done in session store mode
            $params['atkstore'] = 'session';
            $params['atkstore_key'] = $this->getSessionStoreKey();

            // Make the grid use the OTM Session Grid Handler
            // which makes the grid get it's records from the session.
            $handler = new OneToManyRelationSessionGridHandler($this->getSessionStoreKey());

            $grid->setCountHandler(array($handler, 'countHandlerForAdd'));
            $grid->setSelectHandler(array($handler, 'selectHandlerForAdd'));
            // No searching and sorting on session data... for now...
            $grid->removeFlag(DataGrid::SEARCH);
            $grid->removeFlag(DataGrid::SORT);
            $grid->removeFlag(DataGrid::EXTENDED_SORT);
        }

        $actions = $this->m_destInstance->defaultActions('relation', $params);
        $grid->setDefaultActions($actions);

        $grid->loadRecords(); // force early load of records

        if ($mode === 'edit') {
            $usesIndex = $grid->getIndex() != null;
            $isSearching = is_array($grid->getPostvar('atksearch')) && Tools::count($grid->getPostvar('atksearch')) > 0;
            if ($grid->getCount() == 0 && ($usesIndex || $isSearching)) {
                $grid->setComponentOption('list', 'alwaysShowGrid', true);
            }
        }

        $output = $this->editHeader($record, $grid->getRecords()).$grid->render().$this->editFooter($record, $grid->getRecords());

        if ($this->m_destInstance->allowed('add')) {
            $this->_addAddToEditOutput($output, $grid->getRecords(), $record, $mode, $fieldprefix);
        }

        return $output;
    }

    /**
     * Adds the 'add' option to the onetomany, either integrated or as a link.
     *
     * @param string $output The HTML output of the edit function
     * @param array $myrecords The records that are loaded into the recordlist
     * @param array $record The master record that is being edited.
     * @param string $mode
     * @param string $fieldprefix
     */
    public function _addAddToEditOutput(&$output, $myrecords, $record, $mode = '', $fieldprefix = '')
    {
        $add_link = '';

        if (!$this->getDestination()->hasFlag(Node::NF_NO_ADD)) {
            $add_link = $this->_getAddLink($myrecords, $record, true, $mode, $fieldprefix);
        }

        if (Config::getGlobal('onetomany_addlink_position', 'bottom') == 'top') {
            $output = $add_link.$output;
        } else {
            if (Config::getGlobal('onetomany_addlink_position', 'bottom') == 'bottom') {
                $output .= $add_link;
            }
        }
    }

    /**
     * Get the buttons for the embedded mode of the onetomany relation.
     *
     * @todo Move this to a template
     *
     * @return string The HTML buttons
     */
    public function _getEmbeddedButtons()
    {
        $fname = $this->fieldName();
        $output = '<input type="submit" class="btn btn-default otm_add" name="'.$fname.'_save" value="'.Tools::atktext('add').'">';

        return $output.'<input type="button"
        onClick="ATK.OneToManyRelation.toggleAddForm(\''.$fname."_integrated','".$fname."_integrated_link');\"
        class=\"btn btn-default otm_add\"
        name=\"".$fname.'_cancel"
        value="'.Tools::atktext('cancel').'">';
    }

    /**
     * Internal function to get the add link for a atkOneToManyRelation.
     *
     * @param array $myrecords The load of all attributes (see comment in edit() code)
     * @param array $record The record that holds the value for this attribute.
     * @param bool $saveform Save the form values?
     * @param string $mode
     * @param string $fieldprefix
     *
     * @return string The link to add records to the onetomany
     */
    public function _getAddLink($myrecords, $record, $saveform = true, $mode = '', $fieldprefix = '')
    {
        $params = [];
        if ($mode === 'add') {
            $ownerfields = $this->getOwnerFields();
            foreach ($ownerfields as $ownerfield) {
                $record[$ownerfield] = $this->getSessionAddFakeId();
            }
            $params['atkstore'] = 'session';
            $params['atkstore_key'] = $this->getSessionStoreKey();
        }

        return $this->_getNestedAddLink($myrecords, $record, $saveform, $fieldprefix, $params);
    }

    /**
     * Return a fake ID for adding to the session.
     *
     * We use a high negative number because we have to sneak this in
     * as if it's a REAL id for the owner, tricking MTOs in the destination
     * that point back to us into thinking they already have an id.
     * But we also have to make sure it's recognizable, so when we
     * persist the records from the session to the database, then
     * we can set the proper id.
     *
     * @return string
     */
    public function getSessionAddFakeId()
    {
        return '-999999';
    }

    /**
     * Return the key to use when storing records for the OTM destination
     * in the session if the OTM is used in add mode.
     *
     * @return string
     */
    public function getSessionStoreKey()
    {
        return $this->getOwnerInstance()->atkNodeUri().':'.$this->fieldName();
    }

    /**
     * Internal function to get the add link for a atkOneToManyRelation.
     *
     * @param array $myrecords The load of all attributes (see comment in edit() code)
     * @param array $record The record that holds the value for this attribute.
     * @param bool $saveform Save the values of the form?
     * @param string $fieldprefix
     * @param array $params
     *
     * @return string The link to add records to the onetomany
     */
    public function _getNestedAddLink($myrecords, $record, $saveform = true, $fieldprefix = '', $params = [])
    {
        $url = '';
        if ((int)$this->m_maxRecords !== 0 && $this->m_maxRecords <= Tools::count($myrecords)) {
            return $url;
        }
        if (!$this->createDestination()) {
            return $url;
        }
        if ($this->m_destInstance->hasFlag(Node::NF_NO_ADD)) {
            return $url;
        }

        $forceValues = $this->_getForcedValueString($record);
        if (!empty($forceValues)) {
            $params['atkforce'] = $forceValues;
        }

        $onchange = '';
        if (Tools::count($this->m_onchangecode)) {
            $onchange = 'onChange="'.$this->fieldName().'_onChange(this);"';
            $this->_renderChangeHandler($fieldprefix);
        }

        $add_url = $this->getAddURL($params);
        $label = $this->getAddLabel();

        return Tools::href($add_url, $label, SessionManager::SESSION_NESTED, $saveform, $onchange.' class="atkonetomanyrelation btn btn-default"');
    }

    /**
     * Get a string for destination node with primary key of $record
     *
     * With this string, destination node will set correct values when adding a record.
     *
     * @param array $record
     *
     * @return array Array with filter elements
     */
    private function _getForcedValueString($record) : string
    {
        $forcedValues = [];

        $ownerfields = $this->getOwnerFields();
        foreach ($ownerfields as $index => $keyName) {
            $forcedValues[$this->m_refKey[$index]] = $record[$keyName];
        }
        return json_encode($forcedValues);
    }

    protected function getAddURL($params = array())
    {
        return Tools::dispatch_url($this->m_destination, 'add', $params);
    }

    /**
     * Retrieve header for the recordlist.
     *
     * The regular atkOneToManyRelation has no implementation for this method,
     * but it may be overridden in derived classes to add extra information
     * (text, links, whatever) to the top of the attribute, right before the
     * recordlist. This is similar to the adminHeader() method in Node.
     *
     * @param array $record The master record that is being edited.
     * @param array $childrecords The childrecords in this master/detail
     *                            relationship.
     *
     * @return string a String to be added to the header of the recordlist.
     */
    public function editHeader($record = null, $childrecords = null)
    {
        if (!empty($this->m_headerName)) {
            $methodname = $this->m_headerName;

            return $this->m_ownerInstance->$methodname($record, $childrecords, $this);
        } else {
            return '';
        }
    }

    /**
     * Retrieve footer for the recordlist.
     *
     * The regular atkOneToManyRelation has no implementation for this method,
     * but it may be overridden in derived classes to add extra information
     * (text, links, whatever) to the bottom of the attribute, just after the
     * recordlist. This is similar to the adminFooter() method in Node.
     *
     * @param array $record The master record that is being edited.
     * @param array $childrecords The childrecords in this master/detail
     *                            relationship.
     *
     * @return string a String to be added at the bottom of the recordlist.
     */
    public function editFooter($record = null, $childrecords = null)
    {
        if (!empty($this->m_footerName)) {
            $methodname = $this->m_footerName;

            return $this->m_ownerInstance->$methodname($record, $childrecords, $this);
        } else {
            return '';
        }
    }

    /**
     * Create the where clause for the referential key that is used to
     * retrieve the destination records.
     *
     * @param array $record The master record
     *
     * @return QueryPart SQL where clause
     */
    public function _getLoadWhereClause($record)
    {
        if (!$this->m_useRefKeyForFilter) {
            return '';
        }

        $whereelems = [];

        if (Tools::count($this->m_refKey) == 0 || $this->m_refKey[0] == '') {
            $this->m_refKey[0] = $this->m_owner;
        }
        $ownerfields = $this->getOwnerFields();

        for ($i = 0, $_i = Tools::count($this->m_refKey); $i < $_i; ++$i) {
            $primkeyattr = $this->m_ownerInstance->m_attribList[$ownerfields[$i]];

            if (!$primkeyattr->isEmpty($record)) {
                $whereelems[] = Query::simpleValueCondition($this->m_destInstance->getTable(), $this->m_refKey[$i], $primkeyattr->value2db($record));;
            }
        }

        $result = QueryPart::implode('AND', $whereelems, true);

        return $result == null ? new QueryPart('1=0') : $result;
    }

    /**
     * Define a dummy function to use as a dummy handler function in load() below.
     */
    public function ___dummyCount()
    {
    }

    /**
     * Retrieve detail records from the database.
     *
     * Called by the framework to load the detail records.
     *
     * @param Db $db The database used by the node.
     * @param array $record The master record
     * @param string $mode The mode for loading (admin, select, copy, etc)
     *
     * @return array Recordset containing detailrecords, or NULL if no detail
     *               records are present. Note: when $mode is edit, this
     *               method will always return NULL. This is a framework
     *               optimization because in edit pages, the records are
     *               loaded on the fly.
     */
    public function load($db, $record, $mode)
    {
        $result = null;

        // for edit and view mode we don't load any records unless a display override exists
        // we use the grid to load records because it makes things easier
        if (($mode != 'add' && $mode != 'edit' && $mode != 'view') || ($mode == 'view' && method_exists($this->getOwnerInstance(),
                    $this->fieldName().'_display')) || ($mode == 'edit' && $this->hasFlag(self::AF_READONLY_EDIT) && method_exists($this->getOwnerInstance(),
                    $this->fieldName().'_display'))
        ) {
            $grid = $this->createGrid($record, $mode == 'copy' ? 'copy' : 'admin', $mode, false);
            $grid->setPostvar('atklimit', -1); // all records
            $grid->setCountHandler(array($this, '___dummyCount')); // don't count
            $grid->loadRecords();
            $result = $grid->getRecords();
            $grid->destroy(); // clean-up
        }

        return $result;
    }

    /**
     * Override isEmpty function - in a oneToMany relation we should check if the
     * relation contains any records. When there aren't any, the relation is empty,
     * otherwise it isn't.
     *
     * @param array &$record The record to check
     *
     * @return bool true if a destination record is present. False if not.
     */
    public function isEmpty($record)
    {
        if (!isset($record[$this->fieldName()]) || (is_array($record[$this->fieldName()]) && Tools::count($record[$this->fieldName()]) == 0)) {
            // empty. It might be that the record has not yet been fetched. In this case, we do
            // a forced load to see if it's really empty.
            $recs = $this->load($this->m_ownerInstance->getDb(), $record, null);

            return Tools::count($recs) == 0;
        }

        return false;
    }

    /**
     * The delete method is called by the framework to inform the attribute
     * that the master record is deleted.
     *
     * Note that the framework only calls the method when the
     * self::AF_CASCADE_DELETE flag is set. When calling this method, all detail
     * records belonging to the master record are deleted.
     *
     * @param array $record The record that is deleted.
     *
     * @return bool true if cleanup was successful, false otherwise.
     */
    public function delete($record)
    {
        $atk = Atk::getInstance();
        $classname = $this->m_destination;
        $cache_id = $this->m_owner.'.'.$this->m_name;
        $rel = $atk->atkGetNode($classname, true, $cache_id);
        $ownerfields = $this->getOwnerFields();

        $whereelems = [];
        for ($i = 0, $_i = Tools::count($this->m_refKey); $i < $_i; ++$i) {
            $primkeyattr = $this->m_ownerInstance->m_attribList[$ownerfields[$i]];
            $whereelems[] = $this->_addTablePrefix($this->m_refKey[$i])."='".$primkeyattr->value2db($record)."'";
        }
        $where = implode(' AND ', $whereelems);

        if ($where != '') { // double check, so we never by accident delete the entire db
            return $rel->deleteDb($where);
        }

        return true;
    }

    /**
     * Store detail records in the database.
     *
     * For onetomanyrelation, this function does not have much use, since it
     * stores records using its 'add link'.
     * There are however two modes that use this:
     * - 'copy' mode
     *   The copyDb function, to clone detail records.
     * - 'add' mode
     *   When the OTM was used in add mode, we have to transfer
     *   the records stored in the session to the database.
     *
     * other than those this method does not do anything.
     *
     * @param Db $db The database used by the node.
     * @param array $record The master record which has the detail records
     *                       embedded.
     * @param string $mode The mode we're in ("add", "edit", "copy")
     *
     * @return bool true if store was successful, false otherwise.
     */
    public function store($db, $record, $mode)
    {
        switch ($mode) {
            case 'add' :
                return $this->storeAdd($db, $record, $mode);
            case 'copy':
                return $this->storeCopy($db, $record, $mode);
            default:
                return true;
        }
    }

    /**
     * Persist records from the session (in add mode) to the database.
     *
     * @param Db $db
     * @param array $record
     * @param string $mode
     *
     * @return bool
     */
    private function storeAdd($db, $record, $mode)
    {
        if (!$this->createDestination()) {
            return false;
        }

        $rows = SessionStore::getInstance($this->getSessionStoreKey())->getData();

        foreach ($rows as $row) {
            $pk = $record[$this->getOwnerInstance()->primaryKeyField()];
            $row[$this->m_refKey[0]] = $pk;
            $this->m_destInstance->addDb($row);
        }

        // after saving the rows, we can clear the sessionstore
        SessionStore::getInstance($this->getSessionStoreKey())->setData(null);

        return true;
    }

    /**
     * Copy detail records.
     *
     * @param Db $db Datbase connection to use
     * @param array $record Owner record
     * @param string $mode Mode ('copy')
     *
     * @return bool
     */
    private function storeCopy($db, $record, $mode)
    {
        $onetomanyrecs = $record[$this->fieldName()];
        if (!is_array($onetomanyrecs) || Tools::count($onetomanyrecs) <= 0) {
            return true;
        }

        if (!$this->createDestination()) {
            return true;
        }

        $ownerfields = $this->getOwnerFields();
        for ($i = 0; $i < Tools::count($onetomanyrecs); ++$i) {
            // original record
            $original = $onetomanyrecs[$i];
            $onetomanyrecs[$i]['atkorgrec'] = $original;

            // the referential key of the onetomanyrecs could be wrong, if we
            // are called for example from a copy function. So just in case,
            // we reset the correct key.
            if (!$this->destinationHasRelation()) {
                for ($j = 0, $_j = Tools::count($this->m_refKey); $j < $_j; ++$j) {
                    $onetomanyrecs[$i][$this->m_refKey[$j]] = $record[$ownerfields[$j]];
                }
            } else {
                for ($j = 0, $_j = Tools::count($this->m_refKey); $j < $_j; ++$j) {
                    $onetomanyrecs[$i][$this->m_refKey[0]][$ownerfields[$j]] = $record[$ownerfields[$j]];
                }
            }

            if (!$this->m_destInstance->addDb($onetomanyrecs[$i], true, $mode)) {
                // error
                return false;
            }
        }

        return true;
    }

    /**
     * Returns a piece of html code for hiding this attribute in an HTML form.
     *
     * Because the oneToMany has nothing to hide, we override the default
     * hide() implementation with a dummy method.
     *
     * @param array $record
     * @param string $fieldprefix
     * @param string $mode
     *
     * @return string html
     */
    public function hide($record, $fieldprefix, $mode)
    {
        //Nothing to hide..
        return '';
    }

    /**
     * Retrieve the list of searchmodes supported by the attribute.
     *
     * Note that not all modes may be supported by the database driver.
     * Compare this list to the one returned by the databasedriver, to
     * determine which searchmodes may be used.
     *
     * @return array List of supported searchmodes
     */
    public function getSearchModes()
    {
        return array('substring', 'exact', 'wildcard', 'regexp');
    }

    /**
     * Returns the condition (SQL) that should be used when we want to join an owner
     * node with the destination node of the atkOneToManyRelation.
     *
     * @param string $ownerAlias The owner table alias.
     * @param string $destAlias The destination table alias.
     *
     * @return string SQL string for joining the owner with the destination.
     */
    public function getJoinCondition($ownerAlias = '', $destAlias = '')
    {
        if (!$this->createDestination()) {
            return false;
        }

        if ($ownerAlias == '') {
            $ownerAlias = $this->m_ownerInstance->getTable();
        }
        if ($destAlias == '') {
            $destAlias = $this->m_destInstance->getTable();
        }

        $conditions = [];
        $ownerfields = $this->getOwnerFields();

        for ($i = 0, $_i = Tools::count($this->m_refKey); $i < $_i; ++$i) {
            $conditions[] = Db::quoteIdentifier($destAlias, $this->m_refKey[$i]).'='.
                Db::quoteIdentifier($ownerAlias, $ownerfields[$i]);
        }

        return implode(' AND ', $conditions);
    }

    /**
     * Creates a smart search condition for a given search value, and adds it
     * to the query that will be used for performing the actual search.
     *
     * @param int $id The unique smart search criterium identifier.
     * @param int $nr The element number in the path.
     * @param array $path The remaining attribute path.
     * @param Query $query The query to which the condition will be added.
     * @param string $ownerAlias The owner table alias to use.
     * @param mixed $value The value the user has entered in the searchbox.
     * @param string $mode The searchmode to use.
     */
    public function smartSearchCondition($id, $nr, $path, $query, $ownerAlias, $value, $mode)
    {
        // one-to-many join means we need to perform a distinct select
        $query->setDistinct(true);

        if (Tools::count($path) > 0) {
            $this->createDestination();

            $destAlias = "ss_{$id}_{$nr}_".$this->fieldName();

            $query->addJoin($this->m_destInstance->m_table, $destAlias, $this->getJoinCondition($ownerAlias, $destAlias), false);

            $attrName = array_shift($path);
            $attr = $this->m_destInstance->getAttribute($attrName);

            if (is_object($attr)) {
                $attr->smartSearchCondition($id, $nr + 1, $path, $query, $destAlias, $value, $mode);
            }
        } else {
            $this->searchCondition($query, $ownerAlias, $value, $mode);
        }
    }

    /**
     * Adds a search condition for a given search value.
     *
     * @param Query $query The query to which the condition will be added.
     * @param string $table The name of the table in which this attribute
     *                                 is stored
     * @param mixed $value The value the user has entered in the searchbox
     * @param string $searchmode The searchmode to use. This can be any one
     *                                 of the supported modes, as returned by this
     *                                 attribute's getSearchModes() method.
     * @param string $fieldaliasprefix optional prefix for the fieldalias in the table
     */
    public function searchCondition($query, $table, $value, $searchmode, $fieldaliasprefix = '')
    {
        if ($this->createDestination()) {
            $searchcondition = $this->getSearchCondition($query, $table, $value, $searchmode);

            if (!empty($searchcondition)) {
                $query->addSearchCondition($searchcondition);
                $query->setDistinct(true);
            }
        }
    }

    /**
     * Creates a searchcondition for the field,
     * was once part of searchCondition, however,
     * searchcondition() also immediately adds the search condition.
     *
     * @param Query $query The query object where the search condition should be placed on
     * @param string $table The name of the table in which this attribute
     *                           is stored
     * @param mixed $values The value the user has entered in the searchbox
     * @param string $searchmode The searchmode to use. This can be any one
     *                           of the supported modes, as returned by this
     *                           attribute's getSearchModes() method.
     * @param string $fieldname
     *
     * @return QueryPart The searchcondition to use.
     */
    public function getSearchCondition(Query $query, $table, $values, $searchmode, $fieldname = '')
    {
        if (!$this->createDestination() || empty($values)) {
            return null;
        }
        $alias = $this->fieldName().'_AE_'.$this->m_destInstance->m_table;
        $query->addJoin($this->m_destInstance->m_table, $alias, $this->getJoinCondition($table, $alias), false);
        $searchConditions = [];
        if(!is_array($values)) {
            $values = [$values];
        }
        // Searching each value individually :
        $descTemplate = $this->m_destInstance->getDescriptorTemplate();
        foreach ($values as $value) {
            $searchConditions[] =
                $this->m_destInstance->getTemplateSearchCondition($query, $alias, $descTemplate, $value, $searchmode, $fieldname);
        }

        return QueryPart::implode('OR', $searchConditions, true);
    }

    /**
     * Determine the type of the foreign key on the other side.
     *
     * On the other side of a oneToManyRelation (in the destination node),
     * there may be a regular Attribute for the referential key, or an
     * ManyToOneRelation pointing back at the source. This method discovers
     * which of the 2 cases we are dealing with.
     *
     * @return bool True if the foreign key on the other side is a
     *              relation, false if not.
     */
    public function destinationHasRelation()
    {
        if ($this->createDestination()) {
            // If there's a relation back, it's in the destination node under the name of the first refkey element.
            $attrib = $this->m_destInstance->m_attribList[$this->m_refKey[0]];
            if (is_object($attrib) && strpos(get_class($attrib), 'elation') !== false) {
                return true;
            }
        }

        return false;
    }

    /**
     * Are we allowed to delete a record?
     *
     * @return mixed bool if allowed or string with not allowed message
     */
    public function deleteAllowed()
    {
        if ($this->hasFlag(self::AF_RESTRICTED_DELETE)) {
            $selector = $this->m_ownerInstance->primaryKeyFromString($this->m_ownerInstance->m_postvars['atkselector']);
            $records = $this->m_ownerInstance->select($selector)->fetchAll();
            foreach ($records as $record) {
                if (!$this->isEmpty($record)) {
                    return Tools::atktext('restricted_delete_error');
                }
            }
        }

        return true;
    }

    /**
     * Set header generation function name.
     *
     * @param string $name The header generation function name.
     */
    public function setHeader($name)
    {
        $this->m_headerName = $name;
    }

    /**
     * Set footer generation function name.
     *
     * @param string $name The footder generation function name.
     */
    public function setFooter($name)
    {
        $this->m_footerName = $name;
    }

    /**
     * Set the exclude fields for the grid.
     *
     * @param array $excludes
     */
    public function setGridExcludes($excludes)
    {
        $this->m_excludes = $excludes;
    }

    /**
     * Get the exclude fields for the grid.
     *
     * @return array with exclude fields
     */
    public function getGridExcludes()
    {
        return $this->m_excludes;
    }
}

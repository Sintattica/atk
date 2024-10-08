<?php

namespace Sintattica\Atk\Relations;

use Exception;
use Sintattica\Atk\Core\Config;
use Sintattica\Atk\Core\Node;
use Sintattica\Atk\Core\Tools;
use Sintattica\Atk\Db\Db;
use Sintattica\Atk\Session\SessionManager;

/**
 * Many-to-many select relation.
 *
 * The relation shows allows you to add one record at a time to a many-to-many
 * relation using auto-completion or a select page. If a position attribute has
 * been set (setPositionAttribute) the order of the items can be changed using
 * simple drag & drop.
 *
 *
 * @author Peter C. Verhage <peter@achievo.org>
 */
class ManyToManySelectRelation extends ManyToManyRelation
{
    const AF_MANYTOMANYSELECT_DETAILEDIT = 33554432;
    const AF_MANYTOMANYSELECT_DETAILADD = 67108864;
    const AF_MANYTOMANYSELECT_NO_AUTOCOMPLETE = 134217728;

    /**
     * The many-to-one relation.
     *
     * @var ManyToOneRelation
     */
    private $m_manyToOneRelation = null;

    /**
     * The name of the attribute/column where the position of the item in
     * the set should be stored.
     *
     * @var string
     */
    private $m_positionAttribute;

    /**
     * The html to be output next to a positional attribute label.
     *
     * @var string
     */
    private $m_positionAttributeHtmlModifier;


    public function __construct($name, $flags, $link, $destination, $local_key = null, $remote_key = null)
    {
        parent::__construct($name, $flags, $link, $destination, $local_key, $remote_key);

        $relationFlags = ManyToOneRelation::AF_MANYTOONE_AUTOCOMPLETE | self::AF_HIDE;
        $relation = new ManyToOneRelation($this->fieldName() . '_m2msr_add', $relationFlags, $this->m_destination);
        $relation->setDisabledModes(self::DISABLED_VIEW | self::DISABLED_EDIT);
        $relation->setLoadType(self::NOLOAD);
        $relation->setStorageType(self::NOSTORE);
        $this->m_manyToOneRelation = $relation;
    }

    /**
     * Initialize.
     */
    public function init()
    {
        $this->getOwnerInstance()->add($this->getManyToOneRelation());
    }

    /**
     * Return the many-to-one relation we will use for the selection
     * of new records etc.
     *
     * @return ManyToOneRelation
     */
    public function getManyToOneRelation()
    {
        return $this->m_manyToOneRelation;
    }

    /**
     * Create the instance of the destination and copy the destination to
     * the the many to one relation.
     *
     * If succesful, the instance is stored in the m_destInstance member variable.
     *
     * @return bool true if succesful, false if something went wrong.
     */
    public function createDestination()
    {
        $result = parent::createDestination();
        $this->getManyToOneRelation()->m_destInstance = $this->m_destInstance;

        return $result;
    }

    /**
     * Order selected records in the same way as the selected keys. We only do
     * this if the position attribute has been set.
     *
     * @param array $selectedRecords selected records
     * @param array $selectedKeys selected keys
     */
    protected function orderSelectedRecords(array &$selectedRecords, array $selectedKeys)
    {
        $orderedRecords = [];

        foreach ($selectedKeys as $key) {
            foreach ($selectedRecords as $record) {
                if ($key == $this->getDestination()->primaryKey($record)) {
                    $orderedRecords[] = $record;
                }
            }
        }

        $selectedRecords = $orderedRecords;
    }

    /**
     * Return a piece of html code to edit the attribute.
     *
     * @param array $record The record that holds the value for this attribute.
     * @param string $fieldprefix The fieldprefix to put in front of the name
     *                            of any html form element for this attribute.
     * @param string $mode The mode we're in ('add' or 'edit')
     *
     * @return string piece of html code
     * @throws Exception
     */
    public function edit($record, $fieldprefix, $mode)
    {
        if ($this->hasFlag(self::AF_MANYTOMANYSELECT_NO_AUTOCOMPLETE)) {
            $this->getManyToOneRelation()->removeFlag(ManyToOneRelation::AF_MANYTOONE_AUTOCOMPLETE);
        }

        $this->createDestination();
        $this->createLink();

        $page = $this->getOwnerInstance()->getPage();
        $assetUrl = Config::getGlobal('assets_url');
        $page->register_script($assetUrl . 'lib/jquery-sortable/jquery-sortable-min.js');
        $page->register_script($assetUrl . 'javascript/manytomanyselectrelation.js');
        $id = $this->getHtmlId($fieldprefix);
        $selectId = "{$id}_selection";
        $selectedKeys = $this->getSelectedKeys($record, $id);

        $selectedRecords = [];
        if (Tools::count($selectedKeys) > 0) {
            $selector = '(' . implode(') OR (', $selectedKeys) . ')';
            $selectedRecords = $this->getDestination()->select($selector)->includes($this->getDestination()->descriptorFields())->getAllRows();
            $this->orderSelectedRecords($selectedRecords, $selectedKeys);
        }

        // Post an empty value if none selected (instead of not posting anything)
        $result = '<input type="hidden" name="' . $this->getHtmlName($fieldprefix) . '" value="" />';
        $result .= '<div class="atkmanytomanyselectrelation">';

        // "Add" button
        if (($this->hasFlag(self::AF_MANYTOMANYSELECT_DETAILADD)) && ($this->m_destInstance->allowed('add'))) {
            $result .= ' ' . $this->getAddActionLink($record, $fieldprefix);
        }

        // select to add existing record
        $addField = $this->renderAdditionField($record, $fieldprefix, $mode);
        if ($addField) {
            $result .= '<div style="width: 100%;float:left">';
            $result .= '<div class="atkmanytomanyselectrelation-addition">' . $addField . '</div>';
            $result .= '</div>';
        }

        // selected records
        $selectedRecordsField = '';
        foreach ($selectedRecords as $selectedRecord) {
            $selectedRecordsField .= $this->renderSelectedRecord($selectedRecord, $fieldprefix);
        }

        if ($selectedRecordsField) {
            $result .= '<div id="' . $selectId . '" class="atkmanytomanyselectrelation-selection row justify-content-center">';
            $result .= $selectedRecordsField;
            $result .= '</div>';

        } else {
            $result .= '<i>' . $this->text('none') . '</i>';
        }

        $result .= '</div>'; // close atkmanytomanyselectrelation

        if ($this->hasPositionAttribute()) {
            $this->getOwnerInstance()->getPage()->register_loadscript("ATK.ManyToManySelectRelation.makeItemsSortable('{$selectId}');");
        }

        return $result;
    }

    /**
     * Return the selected keys for a given record.
     *
     * @param array $record The record that holds the value for this attribute.
     * @param string $id is the html id of the relation
     * @param mixed $enforceUnique is the type of array_unique filter to use on the results. Use boolean false to disable
     *
     * @return array of selected keys in the order they were submitted
     */
    public function getSelectedKeys(array $record, string $id, $enforceUnique = true): array
    {
        // Get Existing selected records
        $selectedKeys = $this->getSelectedRecords($record);

        // Get records added this time
        if (isset($record[$this->getManyToOneRelation()->fieldName()]) && is_array($record[$this->getManyToOneRelation()->fieldName()])) {
            $selectedKeys[] = $this->getDestination()->primaryKey($record[$this->getManyToOneRelation()->fieldName()]);
        }

        // Get New Selection records
        if (isset($this->getOwnerInstance()->m_postvars[$id . '_newsel'])) {
            $selectedKeys[] = $this->getOwnerInstance()->m_postvars[$id . '_newsel'];
        }

        // Ensure we're only adding an item once
        if ($enforceUnique && is_array($selectedKeys) && Tools::count($selectedKeys)) {
            $selectedKeys = array_unique($selectedKeys);
        }

        return $selectedKeys;
    }

    /**
     * Load function.
     *
     * @param Db $db database instance.
     * @param array $record record
     * @param string $mode load mode
     *
     * @return array values
     * @throws Exception
     */
    public function load($db, $record, $mode)
    {
        if (!$this->hasPositionAttribute()) {
            return parent::load($db, $record, $mode);
        }

        $this->createLink();
        $where = $this->_getLoadWhereClause($record);
        $link = $this->getLink();

        return $link->select()->where($where)->orderBy($link->getTable() . '.' . $this->getPositionAttribute())->getAllRows();
    }

    /**
     * Perform the "create action" on a record that is new.
     *
     * @param array $selectedKey the selected keys
     * @param array $selectedRecord the selected record
     * @param array $ownerRecord the owner record
     * @param int $index the index of the item in the set
     *
     * @return array the newly created record
     */
    protected function _createRecord($selectedKey, $selectedRecord, $ownerRecord, $index)
    {
        $record = parent::_createRecord($selectedKey, $selectedRecord, $ownerRecord, $index);

        if ($this->hasPositionAttribute()) {
            $record[$this->getPositionAttribute()] = $index + 1;
        }

        return $record;
    }

    /**
     * Perform the "update action" on a record that's been changed.
     *
     * @param array $record the record that has been changed
     * @param int $index the index of the item in the set
     *
     * @return bool true if the update was performed successfuly, false if there were issues
     */
    protected function _updateRecord($record, $index)
    {
        // If the parent class didn't manage to update the record
        // then don't attempt to perform this update
        if (!parent::_updateRecord($record, $index)) {
            return false;
        }

        if ($this->hasPositionAttribute()) {
            $record[$this->getPositionAttribute()] = $index + 1;
            if (!$this->getLink()->updateDb($record, true, '', array($this->getPositionAttribute()))) {
                return false;
            }
        }

        return true;
    }

    /**
     * Render selected record.
     *
     * @param array $record selected record
     * @param string $fieldprefix field prefix
     *
     * @return string
     */
    protected function renderSelectedRecord(array $record, string $fieldprefix): string
    {
        $name = $this->getHtmlName($fieldprefix) . '[][' . $this->getRemoteKey() . ']';
        $key = $record[$this->getDestination()->primaryKeyField()];

        // Get the descriptor and ensure it's presentible
        $descriptor = nl2br(htmlentities($this->getDestination()->descriptor($record)));

        // Build the record
        // atkmanytomanyselectrelation-selected is used as selector when "remove" button is clicked
        return '<div class="atkmanytomanyselectrelation-selected mx-2">
                    <input type="hidden" name="' . $name . '" value="' . htmlentities($key) . '"/>
                    <div class="card pb-2" style="width: 150px">
                        <div class="card-body text-center p-2">' . $descriptor . '</div>
                        <div class="card-actions d-flex justify-content-center">' . $this->renderSelectedRecordActions($record) . '</div>
                    </div>
                </div>';
    }

    /**
     * Renders the action links for a given record
     *
     * @param array $record is the selected record
     * @return string the actions in their html link form
     */
    protected function renderSelectedRecordActions(array $record): string
    {
        $actions = [];

        if ($this->hasFlag(self::AF_MANYTOMANY_DETAILVIEW) && !$this->hasFlag(self::AF_MANYTOMANYSELECT_DETAILEDIT) && $this->getDestination()->allowed('view')) {
            $actions[] = 'view';
        }

        if ($this->hasFlag(self::AF_MANYTOMANYSELECT_DETAILEDIT) && $this->getDestination()->allowed('edit', $record)) {
            $actions[] = 'edit';
        }

        if (!$this->getLink()->hasFlag(Node::NF_NO_DELETE)) {
            $actions[] = 'delete';
        }

        $this->recordActions($record, $actions);

        // Call the renderButton action for those actions
        $actionLinks = [];
        foreach ($actions as $action) {
            $actionLink = $this->getActionLink($action, $record);
            if ($actionLink != null) {
                $actionLinks[] = $actionLink;
            }
        }

        $htmlActionLinks = '';
        if (Tools::count($actionLinks)) {
            $htmlActionLinks = implode(' ', $actionLinks);
        }

        return $htmlActionLinks;
    }

    /**
     * This method returns the HTML for the link of a certain action.
     *
     * @param string $action
     * @param array $record
     *
     * @return string
     */
    protected function getActionLink(string $action, array $record): string
    {
        $actionMethod = "get{$action}ActionLink";

        if (method_exists($this, $actionMethod)) {
            return $this->$actionMethod($record);
        } else {
            Tools::atkwarning('Missing ' . $actionMethod . ' method on manytomanyselectrelation. ');
        }
        return '';
    }

    /**
     * The default add link.
     *
     * @param array $record
     * @param string $fieldprefix
     * @param array $params
     * @return string
     */
    protected function getAddActionLink(array $record, string $fieldprefix, array $params = []): string
    {
        $filter = $this->parseFilter($this->getManyToOneRelation()->m_destinationFilter, $record);
        $params = array_merge($params, ['atkfilter' => $filter, 'atkpkret' => $this->getHtmlId($fieldprefix) . '_newsel']);
        $url = Tools::dispatch_url($this->getDestination()->atkNodeUri(), 'add', $params);
        return Tools::href($url, $this->getAddLabel(), SessionManager::SESSION_NESTED, true, 'class="atkmanytomanyselectrelation-link btn btn-sm btn-default mx-1"');
    }

    /**
     * The default view link.
     *
     * @param array $record
     * @return string
     */
    protected function getViewActionLink(array $record): string
    {
        $url = Tools::dispatch_url($this->getDestination()->atkNodeUri(), 'view', [Node::PARAM_ATKSELECTOR => $this->getDestination()->primaryKey($record)]);
        return Tools::href($url, $this->text('view'), SessionManager::SESSION_NESTED, true, 'class="atkmanytomanyselectrelation-link btn btn-sm btn-default mx-1"');
    }

    /**
     * The default edit link.
     *
     * @param array $record
     * @return string
     */
    protected function getEditActionLink(array $record): string
    {
        $url = Tools::dispatch_url($this->getDestination()->atkNodeUri(), 'edit', [Node::PARAM_ATKSELECTOR => $this->getDestination()->primaryKey($record)]);
        return Tools::href($url, $this->text('edit'), SessionManager::SESSION_NESTED, true, 'class="atkmanytomanyselectrelation-link btn btn-sm btn-warning mx-1"');
    }

    /**
     * The default delete link to remove the relation, not the record itself
     *
     * @param array $record
     * @return string
     */
    protected function getDeleteActionLink(array $record): string
    {
        return '<a href="javascript:void(0)" class="atkmanytomanyselectrelation-link btn btn-sm btn-danger mx-1" onclick="ATK.ManyToManySelectRelation.deleteItem(this); return false;">' . $this->text('remove') . '</a>';
    }

    /**
     * Function that is called for each record in a recordlist, to determine
     * what actions may be performed on the record.
     *
     * @param array $record The record for which the actions need to be
     *                        determined.
     * @param array &$actions Reference to an array with the already defined
     *                        actions.
     */
    public function recordActions($record, &$actions)
    {
    }

    /**
     * Render addition field.
     *
     * @param array $record
     * @param string $fieldprefix field prefix
     * @param string $mode
     *
     * @return string
     * @throws Exception
     */
    protected function renderAdditionField(array $record, string $fieldprefix, string $mode): string
    {
        if ($this->getLink()->hasFlag(Node::NF_NO_ADD)) {
            return '';
        }

        $url = Tools::partial_url($this->getOwnerInstance()->atkNodeUri(), $mode, 'attribute.' . $this->fieldName() . '.selectedrecord', ['fieldprefix' => $fieldprefix]);

        $relation = $this->getManyToOneRelation();

        $hasPositionAttribute = $this->hasPositionAttribute() ? 'true' : 'false';
        $relation->addOnChangeHandler("ATK.ManyToManySelectRelation.add(el, '$url', $hasPositionAttribute);");

        $relation->setNoneLabel($this->text('select_none_obligatory'));
        unset($record[$relation->fieldName()]);
        $result = $relation->edit($record, $fieldprefix, $mode);
        if ($result == $this->text('select_none_obligatory')) {
            $result = '';
        }

        return $result;
    }

    /**
     * Partial selected record.
     */
    public function partial_selectedrecord()
    {
        $this->createDestination();
        $this->createLink();

        $fieldprefix = $this->getOwnerInstance()->m_postvars['fieldprefix'];
        $selector = $this->getOwnerInstance()->m_postvars['selector'];

        if (empty($selector)) {
            return '';
        }

        $record = $this->getDestination()->select($selector)->includes($this->getDestination()->descriptorFields())->getFirstRow();

        return $this->renderSelectedRecord($record, $fieldprefix);
    }

    /**
     * Set the searchfields for the autocompletion. By default, the descriptor fields are used.
     *
     * @param array $searchFields
     */
    public function setAutoCompleteSearchFields($searchFields)
    {
        $this->getManyToOneRelation()->setAutoCompleteSearchFields($searchFields);
    }

    /**
     * Set the searchmode for the autocompletion:
     * exact, startswith(default) or contains.
     *
     * @param array $mode
     */
    public function setAutoCompleteSearchMode($mode)
    {
        $this->getManyToOneRelation()->setAutoCompleteSearchMode($mode);
    }

    /**
     * Sets the minimum number of characters before auto-completion kicks in.
     *
     * @param int $chars
     */
    public function setAutoCompleteMinChars($chars)
    {
        $this->getManyToOneRelation()->setAutoCompleteMinChars($chars);
    }

    /**
     * Adds a filter value to the destination filter.
     *
     * @param string $filter
     *
     * @return ManyToOneRelation
     */
    public function addDestinationFilter($filter)
    {
        return $this->getManyToOneRelation()->addDestinationFilter($filter);
    }

    /**
     * Sets the destination filter.
     *
     * @param string $filter
     */
    public function setDestinationFilter($filter)
    {
        $this->getManyToOneRelation()->setDestinationFilter($filter);
    }

    /**
     * Set the positional attribute/column of the many-to-many join. It is the column
     * in the join table that denotes the position of the item in the set.
     *
     * @param string $attr the position attribute/column name of the join
     * @param string|null $htmlIdentifier is the html string to add to the end of the label.
     *                               Defaults to an up down image.
     */
    public function setPositionAttribute(string $attr, string $htmlIdentifier = null)
    {
        $this->m_positionAttribute = $attr;
        $this->m_positionAttributeHtmlModifier = $htmlIdentifier;
    }

    /**
     * Get the positional attribute of the many-to-many join. It is the column
     * in the join table that denotes the position of the item in the set.
     *
     * @return string the position column name of the join
     */
    public function getPositionAttribute(): ?string
    {
        return $this->m_positionAttribute;
    }

    /**
     * Check if position attribute is set.
     *
     * @return bool true if the position attribute has been set
     */
    public function hasPositionAttribute(): bool
    {
        return $this->getPositionAttribute() != null;
    }

    /**
     * Get the HTML label of the attribute.
     *
     * The difference with the label() method is that the label method always
     * returns the HTML label, while the getLabel() method is 'smart', by
     * taking the self::AF_NOLABEL and self::AF_BLANKLABEL flags into account.
     *
     * @param array $record The record holding the value for this attribute.
     * @param string $mode The mode ("add", "edit" or "view")
     *
     * @return string The HTML compatible label for this attribute, or an
     *                empty string if the label should be blank, or NULL if no
     *                label at all should be displayed.
     */
    public function getLabel($record = [], $mode = '')
    {
        $additional = '';
        if ($this->hasPositionAttribute()) {
            if (is_null($this->m_positionAttributeHtmlModifier)) {
                $additional = ' <i class="fa fa-arrows-v"></i>';
            } else {
                $additional = $this->m_positionAttributeHtmlModifier;
            }
        }

        return parent::getLabel($record, $mode) . $additional;
    }
}

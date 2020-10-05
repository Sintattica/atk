<?php

namespace Sintattica\Atk\Relations;

use Exception;
use Sintattica\Atk\Attributes\Attribute;
use Sintattica\Atk\Attributes\ListAttribute;
use Sintattica\Atk\Core\Config;
use Sintattica\Atk\Core\Node;
use Sintattica\Atk\Core\Tools;
use Sintattica\Atk\DataGrid\DataGrid;
use Sintattica\Atk\Db\Db;
use Sintattica\Atk\Db\Query;
use Sintattica\Atk\RecordList\ColumnConfig;
use Sintattica\Atk\Session\SessionManager;
use Sintattica\Atk\Ui\Page;
use Sintattica\Atk\Utils\StringParser;

/**
 * A N:1 relation between two classes.
 *
 * For example, projects all have one coordinator, but one
 * coordinator can have multiple projects. So in the project
 * class, there's a ManyToOneRelation to a coordinator.
 *
 * This relation essentially creates a dropdown box, from which
 * you can select from a set of records.
 *
 * @author Ivo Jansch <ivo@achievo.org>
 */
class ManyToOneRelation extends Relation
{
    /**
     * Create edit/view links for the items in a manytoonerelation dropdown.
     */
    const AF_RELATION_AUTOLINK = 33554432;

    /**
     * Create edit/view links for the items in a manytoonerelation dropdown.
     */
    const AF_MANYTOONE_AUTOLINK = 33554432;

    /**
     * Do not add null option under any circumstance.
     */
    const AF_RELATION_NO_NULL_ITEM = 67108864;

    /**
     * Do not add null option ever.
     */
    const AF_MANYTOONE_NO_NULL_ITEM = 67108864;

    /**
     * Use auto-completition instead of drop-down / selection page.
     */
    const AF_RELATION_AUTOCOMPLETE = 134217728;

    /**
     * Use auto-completition instead of drop-down / selection page.
     */
    const AF_MANYTOONE_AUTOCOMPLETE = 134217728;

    /**
     * Lazy load.
     */
    const AF_MANYTOONE_LAZY = 268435456;

    /**
     * Add a default null option to obligatory relations.
     */
    const AF_MANYTOONE_OBLIGATORY_NULL_ITEM = 536870912;

    const SEARCH_MODE_EXACT = 'exact';
    const SEARCH_MODE_STARTSWITH = 'startswith';
    const SEARCH_MODE_CONTAINS = 'contains';

    /*
     * By default, we do a left join. this means that records that don't have
     * a record in this relation, will be displayed anyway. NOTE: set  this to
     * false only if you know what you're doing. When in doubt, 'true' is
     * usually the best option.
     * @var boolean
     */
    public $m_leftjoin = true;

    /*
     * The array of referential key fields.
     * @access private
     * @var array
     */
    public $m_refKey = [];

    /*
     * SQL statement with extra filter for the join that retrieves the
     * selected record.
     * @var String
     */
    public $m_joinFilter = '';

    /*
     * Hide the relation when there are no records to select.
     * @access private
     * @var boolean
     */
    public $m_hidewhenempty = false;

    /*
     * List columns.
     * @access private
     * @var array
     */
    public $m_listColumns = [];

    /*
     * Always show list columns?
     * @access private
     * @var boolean
     */
    public $m_alwaysShowListColumns = false;

    /*
     * Label to use for the 'none' option.
     *
     * @access private
     * @var String
     */
    public $m_noneLabel = null;

    /*
     * Minimum number of character a user needs to enter before auto-completion kicks in.
     *
     * @access private
     * @var int
     */
    public $m_autocomplete_minchars;

    /*
     * An array with the fieldnames of the destination node in which the autocompletion must search
     * for results.
     *
     * @access private
     * @var array
     */
    public $m_autocomplete_searchfields = '';

    /*
     * The search mode of the autocomplete fields. Can be 'startswith', 'exact' or 'contains'.
     *
     * @access private
     * @var String
     */
    public $m_autocomplete_searchmode;

    /*
     * Value determines wether the search of the autocompletion is case-sensitive.
     *
     * @var boolean
     */
    public $m_autocomplete_search_case_sensitive;

    /*
     * Value determines if select link for autocomplete should use atkSubmit or not (for use in admin screen for example)
     *
     * @var boolean
     */
    public $m_autocomplete_saveform = true;

    /*
     * Set the minimal number of records for showing the automcomplete. If there are less records the normal dropdown is shown
     * @var integer
     */
    public $m_autocomplete_minrecords;

    /**
     * Set the size attribute of the autocompletion input element.
     *
     * @var int
     */
    protected $m_autocomplete_size;

    /**
     * Destination node for auto links (edit, new).
     *
     * @var string
     */
    protected $m_autolink_destination = '';

    /*
     * Use destination filter for autolink add link?
     *
     * @access private
     * @var boolean
     */
    public $m_useFilterForAddLink = true;

    /*
     * Set a function to use for determining the descriptor in the getConcatFilter function
     *
     * @access private
     * @var string
     */
    public $m_concatDescriptorFunction = '';

    /**
     * When autosearch is set to true, this attribute will automatically submit
     * the search form onchange. This will only happen in the admin action.
     *
     * @var bool
     */
    protected $m_autoSearch = false;

    /**
     * Selectable records for edit mode.
     *
     * @see ManyToOneRelation::preAddToEditArray
     *
     * @var array
     */
    protected $m_selectableRecords = null;


    /**
     * How many items for each ajax call
     * @var int
     */
    protected $m_autocomplete_pagination_limit;


    public $m_onchangehandler_init = "var newvalue = el.value;\n";

    public $m_search_by_pk = false;

    protected $m_multipleSearch;

    /**
     * Constructor.
     *
     * @param string $name The name of the attribute. This is the name of the field that is the referential key to the destination.
     *                     For relations with more than one field in the foreign key, you should pass an array of referential key fields.
     *                     The order of the fields must match the order of the primary key attributes in the destination node.
     * @param int $flags Flags for the relation
     *
     * @param string $destination The node we have a relationship with.
     *
     */
    public function __construct($name, $flags = 0, $destination)
    {
        if (Config::getGlobal('manytoone_autocomplete_default', false)) {
            $flags |= self::AF_RELATION_AUTOCOMPLETE;
        }

        if (Config::getGlobal('manytoone_autocomplete_large', true) && Tools::hasFlag($flags, self::AF_LARGE)) {
            $flags |= self::AF_RELATION_AUTOCOMPLETE;
        }

        $this->m_autocomplete_minchars = Config::getGlobal('manytoone_autocomplete_minchars', 2);
        $this->m_autocomplete_searchmode = Config::getGlobal('manytoone_autocomplete_searchmode', 'contains');
        $this->m_autocomplete_search_case_sensitive = Config::getGlobal('manytoone_autocomplete_search_case_sensitive', false);
        $this->m_autocomplete_size = Config::getGlobal('manytoone_autocomplete_size', 50);
        $this->m_autocomplete_minrecords = Config::getGlobal('manytoone_autocomplete_minrecords', -1);
        $this->m_autocomplete_pagination_limit = Config::getGlobal('manytoone_autocomplete_pagination_limit', 25);

        if (is_array($name)) {
            $this->m_refKey = $name;

            // ATK can't handle an array as name, so we initialize the
            // underlying attribute with the first name of the referential
            // keys.
            // Languagefiles, overrides, etc should use this first name to
            // override the relation.
            parent::__construct($name[0], $flags, $destination);
        } else {
            $this->m_refKey[] = $name;
            parent::__construct($name, $flags, $destination);
        }

        if ($this->hasFlag(self::AF_MANYTOONE_LAZY) && (Tools::count($this->m_refKey) > 1 || $this->m_refKey[0] != $this->fieldName())) {
            Tools::atkerror('self::AF_MANYTOONE_LAZY flag is not supported for multi-column reference key or a reference key that uses another column.');
        }
    }

    public function addFlag($flag)
    {
        parent::addFlag($flag);
        if (Config::getGlobal('manytoone_autocomplete_large', true) && Tools::hasFlag($flag, self::AF_LARGE)) {
            $this->m_flags |= self::AF_RELATION_AUTOCOMPLETE;
        }

        return $this;
    }

    /**
     * When autosearch is set to true, this attribute will automatically submit
     * the search form onchange. This will only happen in the admin action.
     *
     * @param bool $auto
     */
    public function setAutoSearch($auto = false)
    {
        $this->m_autoSearch = $auto;
    }

    /**
     * Set join filter.
     *
     * @param string $filter join filter
     */
    public function setJoinFilter($filter)
    {
        $this->m_joinFilter = $filter;
    }

    /**
     * Set the searchfields for the autocompletion.
     *
     * @param array $searchfields
     */
    public function setAutoCompleteSearchFields($searchfields)
    {
        $this->m_autocomplete_searchfields = $searchfields;
    }

    /**
     * Set the searchmode for the autocompletion:
     * exact, startswith(default) or contains.
     *
     * @param array $mode
     */
    public function setAutoCompleteSearchMode($mode)
    {
        $this->m_autocomplete_searchmode = $mode;
    }

    /**
     * Sets the minimum number of characters before auto-completion kicks in.
     *
     * @param int $chars
     */
    public function setAutoCompleteMinChars($chars)
    {
        $this->m_autocomplete_minchars = $chars;
    }

    /**
     * Set if the select link should save form (atkSubmit) or not (for use in admin screen for example).
     *
     * @param bool $saveform
     */
    public function setAutoCompleteSaveForm($saveform = true)
    {
        $this->m_autocomplete_saveform = $saveform;
    }

    /**
     * Set the minimal number of records for the autocomplete to show
     * If there are less records the normal dropdown is shown.
     *
     * @param int $minrecords
     */
    public function setAutoCompleteMinRecords($minrecords)
    {
        $this->m_autocomplete_minrecords = $minrecords;
    }

    /**
     * Set the size of the rendered autocompletion input element.
     *
     * @param int $size
     */
    public function setAutoCompleteSize($size)
    {
        $this->m_autocomplete_size = $size;
    }

    /**
     * Use destination filter for auto add link?
     *
     * @param bool $useFilter use destnation filter for add link?
     */
    public function setUseFilterForAddLink($useFilter)
    {
        $this->m_useFilterForAddLink = $useFilter;
    }

    /**
     * Set the function for determining the descriptor in the getConcatFilter function
     * This function should be implemented in the destination node.
     *
     * @param string $function
     */
    public function setConcatDescriptorFunction($function)
    {
        $this->m_concatDescriptorFunction = $function;
    }

    /**
     * Return the function for determining the descriptor in the getConcatFilter function.
     *
     * @return string
     */
    public function getConcatDescriptorFunction()
    {
        return $this->m_concatDescriptorFunction;
    }

    /**
     * Add list column. An attribute of the destination node
     * that (only) will be displayed in the recordlist.
     *
     * @param string $attr The attribute to add to the listcolumn
     *
     * @return ManyToOneRelation The instance of this ManyToOneRelation
     */
    public function addListColumn($attr)
    {
        $this->m_listColumns[] = $attr;

        return $this;
    }

    /**
     * Add multiple list columns. Attributes of the destination node
     * that (only) will be displayed in the recordlist.
     *
     * @return ManyToOneRelation The instance of this ManyToOneRelation
     */
    public function addListColumns()
    {
        $attrs = func_get_args();
        foreach ($attrs as $attr) {
            $this->m_listColumns[] = $attr;
        }

        return $this;
    }

    public function getListColumns()
    {
        return $this->m_listColumns;
    }

    /**
     * Reset the list columns and add multiple list columns. Attributes of the
     * destination node that (only) will be displayed in the recordlist.
     *
     * @return ManyToOneRelation The instance of this ManyToOneRelation
     */
    public function setListColumns()
    {
        $this->m_listColumns = [];

        $attrs = func_get_args();
        if (Tools::count($attrs) === 1 && is_array($attrs[0])) {
            $columns = $attrs[0];
        } else {
            $columns = $attrs;
        }

        foreach ($columns as $column) {
            $this->m_listColumns[] = $column;
        }

        return $this;
    }

    /**
     * Always show list columns in list view,
     * even if the attribute itself is hidden?
     *
     * @param bool $value always show list columns?
     *
     * @return ManyToOneRelation The instance of this ManyToOneRelation
     */
    public function setAlwaysShowListColumns($value)
    {
        $this->m_alwaysShowListColumns = $value;
        if ($this->m_alwaysShowListColumns) {
            $this->addFlag(self::AF_FORCE_LOAD);
        }

        return $this;
    }

    /**
     * Set the maximum rows of each ajax call
     * @param int $limit
     */
    public function setPaginationLimit($limit)
    {
        $this->m_autocomplete_pagination_limit = $limit;
    }

    public function value2db($rec)
    {
        if ($this->isEmpty($rec)) {
            Tools::atkdebug($this->fieldName().' IS EMPTY!');

            return;
        } else {
            if ($this->createDestination()) {
                if (is_array($rec[$this->fieldName()])) {
                    $pkfield = $this->m_destInstance->m_primaryKey[0];
                    $pkattr = $this->m_destInstance->getAttribute($pkfield);

                    return $pkattr->value2db($rec[$this->fieldName()]);
                } else {
                    return $rec[$this->fieldName()];
                }
            }
        }

        // This never happens, does it?
        return '';
    }

    public function fetchValue($postvars)
    {
        if ($this->isPosted($postvars)) {
            $result = [];

            // support specifying the value as a single number if the
            // destination's primary key consists of a single field
            if (is_numeric($postvars[$this->fieldName()])) {
                $result[$this->getDestination()->primaryKeyField()] = $postvars[$this->fieldName()];
            } else {
                // Split the primary key of the selected record into its
                // referential key elements.
                $keyelements = Tools::decodeKeyValueSet($postvars[$this->fieldName()]);
                foreach ($keyelements as $key => $value) {
                    // Tablename must be stripped out because it is in the way..
                    if (strpos($key, '.') > 0) {
                        $field = substr($key, strrpos($key, '.') + 1);
                    } else {
                        $field = $key;
                    }
                    $result[$field] = $value;
                }
            }

            if (Tools::count($result) == 0) {
                return;
            }

            // add descriptor fields, this means they can be shown in the title
            // bar etc. when updating failed for example
            $record = array($this->fieldName() => $result);
            $this->populate($record);
            $result = $record[$this->fieldName()];

            return $result;
        }

        return;
    }

    public function db2value($rec)
    {
        $this->createDestination();

        if (isset($rec[$this->fieldName()]) && is_array($rec[$this->fieldName()]) && (!isset($rec[$this->fieldName()][$this->m_destInstance->primaryKeyField()]) || empty($rec[$this->fieldName()][$this->m_destInstance->primaryKeyField()]))) {
            return;
        }

        if (isset($rec[$this->fieldName()])) {
            $myrec = $rec[$this->fieldName()];
            if (is_array($myrec)) {
                $result = [];
                if ($this->createDestination()) {
                    foreach (array_keys($this->m_destInstance->m_attribList) as $attrName) {
                        /** @var Attribute $attr */
                        $attr = &$this->m_destInstance->m_attribList[$attrName];
                        if ($attr) {
                            $result[$attrName] = $attr->db2value($myrec);
                        } else {
                            Tools::atkerror("m_attribList['{$attrName}'] not defined");
                        }
                    }
                }

                return $result;
            } else {
                // if the record is not an array, probably only the value of the primary key was loaded.
                // This workaround only works for single-field primary keys.
                if ($this->createDestination()) {
                    return array($this->m_destInstance->primaryKeyField() => $myrec);
                }
            }
        }
    }

    /**
     * Set none label.
     *
     * @param string $label The label to use for the "none" option
     */
    public function setNoneLabel($label)
    {
        $this->m_noneLabel = $label;
    }

    /**
     * Get none label.
     * @param string $mode
     * @return string The label for the "none" option
     */
    public function getNoneLabel($mode = '')
    {
        if ($this->m_noneLabel !== null) {
            return $this->m_noneLabel;
        }

        $text_key = 'select_none';
        if (in_array($mode, array('add', 'edit')) && $this->hasFlag(self::AF_OBLIGATORY)) {
            if ((($mode == 'add' && !$this->hasFlag(self::AF_READONLY_ADD)) || ($mode == 'edit' && !$this->hasFlag(self::AF_READONLY_EDIT)))) {
                $text_key = 'select_none_obligatory';
            }
        } else {
            if ($mode == 'search') {
                $text_key = 'search_none';
            }
        }

        $nodename = $modulename = $ownermodulename = '';
        if ($this->createDestination()) {
            $nodename = $this->m_destInstance->m_type;
            $modulename = $this->m_destInstance->m_module;
            $ownermodulename = $this->m_ownerInstance->m_module;
        }
        $label = Tools::atktext($this->fieldName().'_'.$text_key, $ownermodulename, $this->m_owner, '', '', true);
        if ($label == '') {
            $label = Tools::atktext($text_key, $modulename, $nodename);
        }

        return $label;
    }

    public function display($record, $mode)
    {
        if ($this->createDestination()) {
            $cnt = isset($record[$this->fieldName()]) ? Tools::count($record[$this->fieldName()]) : null;
            if ($cnt === Tools::count($this->m_refKey)) {
                $this->populate($record);
            }

            if (!$this->isEmpty($record)) {
                $result = $this->m_destInstance->descriptor($record[$this->fieldName()]);
                if ($this->hasFlag(self::AF_RELATION_AUTOLINK) && (!in_array($mode, array('csv', 'plain')))) { // create link to edit/view screen
                    if (($this->m_destInstance->allowed('view')) && !$this->m_destInstance->hasFlag(Node::NF_NO_VIEW) && $result != '') {
                        $saveForm = $mode == 'add' || $mode == 'edit';
                        $url = Tools::dispatch_url($this->m_destination, 'view',
                            ['atkfilter' => 'true', 'atkselector' => $this->m_destInstance->primaryKey($record[$this->fieldName()])]);

                        if ($mode != 'list') {
                            $result .= ' '.Tools::href($url, Tools::atktext('view'), SessionManager::SESSION_NESTED, $saveForm,
                                    'class="atkmanytoonerelation-link"');
                        } else {
                            $result = Tools::href($url, $result, SessionManager::SESSION_NESTED, $saveForm);
                        }

                    }
                }
            } else {
                $result = !in_array($mode, array('csv', 'plain', 'list')) ? $this->getNoneLabel($mode) : ''; // no record
            }

            return $result;
        } else {
            Tools::atkdebug("Can't create destination! ($this -> m_destination");
        }

        return '';
    }

    /**
     * Populate the record with the destination record data.
     *
     * @param array $record record
     * @param mixed $fullOrFields load all data, only the given fields or only the descriptor fields?
     */
    public function populate(&$record, $fullOrFields = false)
    {
        if (!is_array($record) || $record[$this->fieldName()] == '') {
            return;
        }

        Tools::atkdebug('Delayed loading of '.($fullOrFields || is_array($fullOrFields) ? '' : 'descriptor ').'fields for '.$this->m_name);
        $this->createDestination();

        $includes = '';
        if (is_array($fullOrFields)) {
            $includes = array_merge($this->m_destInstance->m_primaryKey, $fullOrFields);
        } else {
            if (!$fullOrFields) {
                $includes = $this->m_destInstance->descriptorFields();
            }
        }

        $result = $this->m_destInstance->select($this->m_destInstance->primaryKey($record[$this->fieldName()]))->orderBy($this->m_destInstance->getColumnConfig()->getOrderByStatement())->includes($includes)->getFirstRow();

        if ($result != null) {
            $record[$this->fieldName()] = $result;
        }
    }

    /**
     * Creates HTML for the selection and auto links.
     *
     * @param string $id attribute id
     * @param array $record record
     *
     * @return string
     */
    public function createSelectAndAutoLinks($id, $record)
    {
        $links = [];
        $filter = $this->parseFilter($this->m_destinationFilter, $record);
        $links[] = $this->_getSelectLink($id, $filter);
        if ($this->hasFlag(self::AF_RELATION_AUTOLINK)) { // auto edit/view link
            $sm = SessionManager::getInstance();

            if ($this->m_destInstance->allowed('add') && !$this->m_destInstance->hasFlag(Node::NF_NO_ADD)) {
                $links[] = Tools::href(Tools::dispatch_url($this->getAutoLinkDestination(), 'add', array(
                    'atkpkret' => $id,
                    'atkfilter' => ($filter ?: 'true'),
                )), Tools::atktext('new'), SessionManager::SESSION_NESTED, true);
            }

            if ($this->m_destInstance->allowed('view') && !$this->m_destInstance->hasFlag(Node::NF_NO_VIEW) && $record[$this->fieldName()] != null) {
                //we laten nu altijd de edit link zien, maar eigenlijk mag dat niet, want
                //de app crasht als er geen waarde is ingevuld.
                $viewUrl = $sm->sessionUrl(Tools::dispatch_url($this->getAutoLinkDestination(), 'view', array('atkselector' => 'REPLACEME', 'atkfilter' => 'true')),
                    SessionManager::SESSION_NESTED);
                $links[] = '<span id="'.$id."_view\" style=\"\"><a href='javascript:ATK.FormSubmit.atkSubmit(ATK.ManyToOneRelation.parse(\"".Tools::atkurlencode($viewUrl).'", document.entryform.'.$id.".value), true)' class=\"atkmanytoonerelation atkmanytoonerelation-link\">".Tools::atktext('view').'</a></span>';
            }
        }

        return implode(' ', $links);
    }

    /**
     * Set destination node for the Autolink links (new/edit).
     *
     * @param string $node
     */
    public function setAutoLinkDestination($node)
    {
        $this->m_autolink_destination = $node;
    }

    /**
     * Get destination node for the Autolink links (new/edit).
     *
     * @return string
     */
    public function getAutoLinkDestination()
    {
        if (!empty($this->m_autolink_destination)) {
            return $this->m_autolink_destination;
        }

        return $this->m_destination;
    }

    public function preAddToEditArray(&$record, $fieldPrefix, $mode)
    {
        if ($mode == 'edit' && ($this->hasFlag(self::AF_READONLY_EDIT) || $this->hasFlag(self::AF_HIDE_EDIT))) {
            // in this case we don't want that the destination filters are activated
            return;
        }

        if ((!$this->hasFlag(self::AF_RELATION_AUTOCOMPLETE) && !$this->hasFlag(self::AF_LARGE)) || $this->m_autocomplete_minrecords > -1) {
            $this->m_selectableRecords = $this->_getSelectableRecords($record, $mode);

            if (Tools::count($this->m_selectableRecords) > 0 && !Config::getGlobal('list_obligatory_null_item') && (($this->hasFlag(self::AF_OBLIGATORY) && !$this->hasFlag(self::AF_MANYTOONE_OBLIGATORY_NULL_ITEM)) || (!$this->hasFlag(self::AF_OBLIGATORY) && $this->hasFlag(self::AF_RELATION_NO_NULL_ITEM)))) {
                if (!isset($record[$this->fieldName()]) || !is_array($record[$this->fieldName()])) {
                    $record[$this->fieldName()] = $this->m_selectableRecords[0];
                } else {
                    if (!$this->_isSelectableRecord($record, $mode)) {
                        $record[$this->fieldName()] = $this->m_selectableRecords[0];
                    } else {
                        $current = $this->getDestination()->primaryKey($record[$this->fieldName()]);
                        $record[$this->fieldName()] = null;
                        foreach ($this->m_selectableRecords as $selectable) {
                            if ($this->getDestination()->primaryKey($selectable) == $current) {
                                $record[$this->fieldName()] = $selectable;
                                break;
                            }
                        }
                    }
                }
            }
        } else {
            if (!isset($record[$this->fieldName()]) || (is_array($record[$this->fieldName()]) && !$this->_isSelectableRecord($record, $mode))) {
                $record[$this->fieldName()] = null;
            } else {
                if (is_array($record[$this->fieldName()])) {
                    $this->populate($record);
                }
            }
        }
    }

    public function edit($record, $fieldprefix, $mode)
    {
        $type = 'edit';

        if (!$this->createDestination()) {
            Tools::atkerror("Could not create destination for destination: $this -> m_destination!");

            return;
        }

        $recordset = $this->m_selectableRecords;

        if ($recordset === null && $this->hasFlag(self::AF_RELATION_AUTOCOMPLETE) && $this->m_autocomplete_minrecords > -1) {
            $recordset = $this->_getSelectableRecords($record, $mode);
        }

        $isAutocomplete = (is_array($recordset) && Tools::count($recordset) > $this->m_autocomplete_minrecords) || $this->m_autocomplete_minrecords == -1;
        if ($this->hasFlag(self::AF_RELATION_AUTOCOMPLETE) && is_object($this->m_ownerInstance) && $isAutocomplete) {


            $result = $this->drawAutoCompleteBox($record, $fieldprefix, $mode);
            return $result;
        }



        $result = '';
        $options = [];
        $id = $this->getHtmlId($fieldprefix);
        $name = $this->getHtmlName($fieldprefix);
        $htmlAttributes = [];
        $linkview = false;
        $style = '';


        if($this->getCssStyle($type, 'width') === null && $this->getCssStyle($type, 'min-width') === null) {
            $this->setCssStyle($type, 'min-width', '220px');
        }

        foreach($this->getCssStyles($type) as $k => $v) {
            $style .= "$k:$v;";
        }
        if($style != ''){
            $htmlAttributes['style'] = $style;
        }

        $value = isset($record[$this->fieldName()]) ? $record[$this->fieldName()] : null;
        $currentPk = ($value != null) ? $this->getDestination()->primaryKey($value) : null;
        $selValues = ($currentPk != null) ? [$currentPk] : [];

        if ($this->hasFlag(self::AF_LARGE)) {
            //no select list, but a link for select
            $result = '';

            $result .= '<span class="atkmanytoonerelation-large-container">';
            $destrecord = $record[$this->fieldName()];
            if (is_array($destrecord)) {
                $result .= '<span id="'.$id.'_current">';

                if ($this->hasFlag(self::AF_RELATION_AUTOLINK) && $this->m_destInstance->allowed('view') && !$this->m_destInstance->hasFlag(Node::NF_NO_VIEW)) {
                    $url = Tools::dispatch_url($this->m_destination, 'view', ['atkselector' => $this->m_destInstance->primaryKey($record[$this->fieldName()])]);
                    $descriptor = $this->m_destInstance->descriptor($destrecord);
                    $result .= $descriptor.' '.Tools::href($url, Tools::atktext('view'), SessionManager::SESSION_NESTED, true,
                            'class="atkmanytoonerelation-link"');
                } else {
                    $result .= $this->m_destInstance->descriptor($destrecord);
                }

                $result .= ' ';

                if (!$this->hasFlag(self::AF_OBLIGATORY)) {
                    $result .= '<a href="#" onClick="jQuery(\'#'.$id.'\').val(\'\');jQuery(\'#'.$id.'_current\').hide();" class="atkmanytoonerelation atkmanytoonerelation-link">'.Tools::atktext('unselect').'</a> ';
                }
                $result .= '</span>';
            }

            $result .= $this->hide($record, $fieldprefix, $mode);
            $result .= $this->_getSelectLink($name, $this->parseFilter($this->m_destinationFilter, $record));
        } else {
            //normal dropdown
            if ($recordset == null) {
                $recordset = $this->_getSelectableRecords($record, $mode);
            }

            // autoselect if there is only one record (if obligatory is not set,
            // we don't autoselect, since user may wist to select 'none' instead
            // of the 1 record.
            if (Tools::count($recordset) == 0) {
                $result = '<span class="form-control-static">' . $this->getNoneLabel();
            } else {
                // relation may be empty, so we must provide an empty selectable..
                $hasNullOption = false;
                $emptyValue = '';
                $linkview = true;

                if (!$this->hasFlag(self::AF_MANYTOONE_NO_NULL_ITEM)) {
                    if (!$this->hasFlag(self::AF_OBLIGATORY) || (
                            $this->hasFlag(self::AF_MANYTOONE_OBLIGATORY_NULL_ITEM) ||
                            (Config::getGlobal("list_obligatory_null_item") && !$this->hasFlag(ListAttribute::AF_LIST_NO_OBLIGATORY_NULL_ITEM))
                        )
                    ) {
                        $hasNullOption = true;
                        $noneLabel = $this->getNoneLabel($mode);
                        $options[$emptyValue] = $noneLabel;
                    }
                }

                foreach ($recordset as $selectable) {
                    $pk = $this->getDestination()->primaryKey($selectable);
                    $options[$pk] = $this->m_destInstance->descriptor($selectable);
                }

                $selectOptions = [];
                $selectOptions['enable-select2'] = true;
                $selectOptions['minimum-results-for-search'] = 10;
                $selectOptions['dropdown-auto-width'] = true;
                if ($hasNullOption) {
                    $selectOptions['with-empty-value'] = $emptyValue;
                }
                $selectOptions = array_merge($selectOptions, $this->m_select2Options['edit']);

                if (Tools::count($this->m_onchangecode)) {
                    $this->_renderChangeHandler($fieldprefix);
                    $htmlAttributes['onchange'] = $this->getHtmlId($fieldprefix).'_onChange(this)';
                }

                $result .= $this->drawSelect($id, $name, $options, $selValues, $selectOptions, $htmlAttributes);
            }
        }

        $autolink = $this->getRelationAutolink($id, $name, $this->parseFilter($this->m_destinationFilter, $record));
        $result .= $linkview && isset($autolink['view']) ? $autolink['view'] : '';
        $result .= isset($autolink['add']) ? $autolink['add'] : '';

        if ($this->hasFlag(self::AF_LARGE) || Tools::count($recordset) == 0) {
            $result .= '</span>'; // atkmanytoonerelation-large-container
        }

        return $result;
    }

    /**
     * Get the select link to select the value using a select action on the destination node.
     *
     * @param string $selname
     * @param string $filter
     *
     * @return string HTML-code with the select link
     */
    public function _getSelectLink($selname, $filter)
    {
        $result = '';
        // we use the current level to automatically return to this page
        // when we come from the select..
        $sm = SessionManager::getInstance();
        $atktarget = Tools::atkurlencode(Config::getGlobal('dispatcher').'?atklevel='.$sm->atkLevel().'&'.$selname.'=[atkprimkey]');
        $linkname = Tools::atktext('link_select_'.Tools::getNodeType($this->m_destination), $this->getOwnerInstance()->getModule(),
            $this->getOwnerInstance()->getType(), '', '', true);
        if (!$linkname) {
            $linkname = Tools::atktext('link_select_'.Tools::getNodeType($this->m_destination), Tools::getNodeModule($this->m_destination),
                Tools::getNodeType($this->m_destination), '', '', true);
        }
        if (!$linkname) {
            $linkname = Tools::atktext('select_a');
        }

        $result .= Tools::href(Tools::dispatch_url($this->m_destination, 'select', ['atkfilter' => $filter ?: 'true', 'atktarget' => $atktarget]),
            $linkname, SessionManager::SESSION_NESTED, $this->m_autocomplete_saveform, 'class="atkmanytoonerelation atkmanytoonerelation-link"');

        return $result;
    }

    /**
     * Creates and returns the auto edit/view links.
     *
     * @param string $id The field html id
     * @param string $name The field html name
     * @param string $filter Filter that we want to apply on the destination node
     *
     * @return array The HTML code for the autolink links
     */
    public function getRelationAutolink($id, $name, $filter)
    {
        $autolink = [];

        if ($this->hasFlag(self::AF_RELATION_AUTOLINK)) { // auto edit/view link
            $page = Page::getInstance();
            $page->register_script(Config::getGlobal('assets_url').'javascript/manytoonerelation.js');
            $sm = SessionManager::getInstance();

            if (!$this->m_destInstance->hasFlag(Node::NF_NO_VIEW) && $this->m_destInstance->allowed('view')) {
                $viewUrl = $sm->sessionUrl(Tools::dispatch_url($this->getAutoLinkDestination(), 'view', array('atkselector' => 'REPLACEME', 'atkfilter' => 'true')),
                    SessionManager::SESSION_NESTED);
                $autolink['view'] = " <a href='javascript:ATK.FormSubmit.atkSubmit(ATK.ManyToOneRelation.parse(\"".Tools::atkurlencode($viewUrl).'", document.entryform.'.$id.".value),true)' class='atkmanytoonerelation atkmanytoonerelation-link'>".Tools::atktext('view').'</a>';
            }
            if (!$this->m_destInstance->hasFlag(Node::NF_NO_ADD) && $this->m_destInstance->allowed('add')) {
                $autolink['add'] = ' '.Tools::href(Tools::dispatch_url($this->getAutoLinkDestination(), 'add', array(
                        'atkpkret' => $name,
                        'atkfilter' => ($this->m_useFilterForAddLink && $filter != '' ? $filter : 'true'),
                    )), Tools::atktext('new'), SessionManager::SESSION_NESTED, true, 'class="atkmanytoonerelation atkmanytoonerelation-link"');
            }
        }

        return $autolink;
    }

    public function hide($record, $fieldprefix, $mode)
    {
        if (!$this->createDestination()) {
            return '';
        }

        $currentPk = '';
        if (isset($record[$this->fieldName()]) && $record[$this->fieldName()] != null) {
            $this->fixDestinationRecord($record);
            $currentPk = $this->m_destInstance->primaryKey($record[$this->fieldName()]);
        }

        $result = '<input type="hidden" id="'.$this->getHtmlId($fieldprefix).'" name="'.$this->getHtmlName($fieldprefix).'" value="'.$currentPk.'">';

        return $result;
    }

    /**
     * Support for destination "records" where only the id is set and the
     * record itself isn't converted to a real record (array) yet.
     *
     * @param array $record The record to fix
     */
    public function fixDestinationRecord(&$record)
    {
        if ($this->createDestination() && isset($record[$this->fieldName()]) && $record[$this->fieldName()] != null && !is_array($record[$this->fieldName()])) {
            $record[$this->fieldName()] = array($this->m_destInstance->primaryKeyField() => $record[$this->fieldName()]);
        }
    }

    public function getEdit($mode, &$record, $fieldprefix)
    {
        $this->fixDestinationRecord($record);

        return parent::getEdit($mode, $record, $fieldprefix);
    }

    /**
     * Converts a record filter to a record array.
     *
     * @param string $filter filter string
     *
     * @return array record
     */
    protected function filterToArray($filter)
    {
        $result = [];

        $values = Tools::decodeKeyValueSet($filter);
        foreach ($values as $field => $value) {
            $parts = explode('.', $field);
            $ref = &$result;

            foreach ($parts as $part) {
                $ref = &$ref[$part];
            }

            $ref = $value;
        }

        return $result;
    }

    public function drawSelect($id, $name, $options = [], $selected = [], $selectOptions = [], $htmlAttributes = [])
    {
        $page = $this->m_ownerInstance->getPage();
        $page->register_script(Config::getGlobal('assets_url').'javascript/manytoonerelation.js');

        $htmlAttrs = '';
        foreach ($selectOptions as $k => $v) {
            $htmlAttrs .= ' data-'.$k.'="'.htmlspecialchars($v).'"';
        }
        foreach ($htmlAttributes as $k => $v) {
            $htmlAttrs .= ' '.$k.'="'.htmlspecialchars($v).'"';
        }

        $result = '<select '.$this->getCSSClassAttribute('form-control').' id="'.$id.'" name="'.$name.'"'.$htmlAttrs.'>';
        foreach ($options as $value => $option) {
            $result .= '<option ';
            $result .= 'value="'.htmlspecialchars($value).'"';
            $result .= in_array($value, $selected) ? ' selected' : '';
            $result .= '>';
            $result .= htmlspecialchars($option);
            $result .= '</option>';
        }
        $result .= '</select>';
        $result .= "<script>ATK.Tools.enableSelect2ForSelect('#$id');</script>";
        $result .= "<script>jQuery('#$id').on('select2:close',function(){jQuery(this).focus();});</script>";

        return $result;
    }

    public function search($record, $extended = false, $fieldprefix = '', DataGrid $grid = null)
    {
        $useautocompletion = Config::getGlobal('manytoone_search_autocomplete', true) && $this->hasFlag(self::AF_RELATION_AUTOCOMPLETE);
        $id = $this->getHtmlId($fieldprefix);
        $name = $this->getSearchFieldName($fieldprefix).'[]';
        $htmlAttributes = [];
        $isMultiple = $this->isMultipleSearch($extended);
        $onchange = '';
        $options = ['' => Tools::atktext('search_all')];
        $selectOptions = [];
        $selectOptions['enable-select2'] = true;
        $selectOptions['dropdown-auto-width'] = true;
        $selectOptions['with-empty-value'] = '';

        $style = '';
        $type = $extended ? 'extended_search' : 'search';

        if ($isMultiple) {
            $htmlAttributes['multiple'] = 'multiple';
        }

        if($this->getCssStyle($type, 'width') === null && $this->getCssStyle($type, 'min-width') === null) {
            $this->setCssStyle($type, 'min-width', '220px');
        }

        if (!$this->hasFlag(self::AF_LARGE) && !$useautocompletion) {

            //Normal dropdown search
            if (!$this->createDestination()) {
                return '';
            }

            $recordset = $this->_getSelectableRecords($record, 'search');

            if (isset($record[$this->fieldName()][$this->fieldName()])) {
                $record[$this->fieldName()] = $record[$this->fieldName()][$this->fieldName()];
            }

            // options and values
            if (!$this->hasFlag(self::AF_OBLIGATORY) && !$this->hasFlag(self::AF_RELATION_NO_NULL_ITEM)) {
                $options['__NONE__'] = $this->getNoneLabel('search');
            }
            $pkfield = $this->m_destInstance->primaryKeyField();
            foreach ($recordset as $option) {
                $pk = $option[$pkfield];
                $options[$pk] = $this->m_destInstance->descriptor($option);
            }

            // selected values
            $selValues = isset($record[$this->fieldName()]) ? $record[$this->fieldName()] : [];
            if($isMultiple && $selValues[0] == ''){
                unset($selValues[0]);
            }

            // if is multiple, replace null selection with empty string
            if($isMultiple) {
                $onchange .= <<<EOF
var s=jQuery(this), v = s.val();
if (v != null && v.length > 0) {
    var nv = jQuery.grep(v, function(value) {
        return value != '';
    });
    s.val(nv);s.trigger('change.select2');
}else if(v === null){
   s.val('');s.trigger('change.select2');
};
EOF;
            }

            if (!is_null($grid) && !$extended && $this->m_autoSearch) {
                $onchange .= $grid->getUpdateCall(array('atkstartat' => 0), [], 'ATK.DataGrid.extractSearchOverrides');
            }

            if($onchange != '') {
                $this->getOwnerInstance()->getPage()->register_loadscript('jQuery("#'.$id.'").on("change", function(el){'.$onchange.'});');
            }


            $selectOptions = array_merge($selectOptions, $this->m_select2Options['search']);

            foreach($this->getCssStyles($type) as $k => $v) {
                $style .= "$k:$v;";
            }
            if($style != ''){
                $htmlAttributes['style'] = $style;
            }

            return $this->drawSelect($id, $name, $options, $selValues, $selectOptions, $htmlAttributes);

        } else {
            //Autocomplete search
            if (isset($record[$this->fieldName()][$this->fieldName()])) {
                $record[$this->fieldName()] = $record[$this->fieldName()][$this->fieldName()];
            }


            if ($useautocompletion) {
                $selValues = isset($record[$this->fieldName()]) ? $record[$this->fieldName()] : null;
                if (!is_array($selValues)) {
                    $selValues = [$selValues];
                }
                foreach ($selValues as $selValue) {
                    $options[$selValue] = $selValue;
                }

                $selectOptions['enable-manytoonereleation-autocomplete'] = true;
                // $selectOptions['tags'] = true;
                $selectOptions['ajax--url'] = Tools::partial_url($this->m_ownerInstance->atkNodeUri(), $this->m_ownerInstance->m_action,
                    'attribute.'.$this->fieldName().'.autocomplete_search');
                $selectOptions['minimum-input-length'] = $this->m_autocomplete_minchars;
                $selectOptions['placeholder'] = $options[''];

                if(!$this->isMultipleSearch($extended)) {
                    $selectOptions['allow-clear'] = true;
                }
                $selectOptions = array_merge($selectOptions, $this->m_select2Options['search']);

                if($isMultiple) {
                    $onchange .= <<<EOF
var s=jQuery(this), v = s.val();
if (v != null && v.length > 0) {
    var nv = jQuery.grep(v, function(value) {
        return value != '';
    });
    s.val(nv);s.trigger('change.select2');
}else if(v === null){
   s.val('');s.trigger('change.select2');
};
EOF;
                }

                if($onchange != '') {
                    $this->getOwnerInstance()->getPage()->register_loadscript('jQuery("#'.$id.'").on("change", function(el){'.$onchange.'});');
                }

                foreach($this->getCssStyles($type) as $k => $v) {
                    $style .= "$k:$v;";
                }
                if($style != ''){
                    $htmlAttributes['style'] = $style;
                }

                $result = '<span class="select-inline">';
                $result .= $this->drawSelect($id, $name, $options, $selValues, $selectOptions, $htmlAttributes);
                $result .= '</span>';

                return $result;

            } else {
                $current = isset($record[$this->fieldName()]) ? $record[$this->fieldName()] : null;
                if(is_array($current)){
                    $current = implode(' ', $current);
                }

                //normal input field
                $result = '<input type="text" id="'.$id.'" name="'.$name.'" '.$this->getCSSClassAttribute('form-control').' value="'.$current.'"'.($this->m_searchsize > 0 ? ' size="'.$this->m_searchsize.'"' : '').'>';
            }

            return $result;
        }
    }

    public function getSearchModes()
    {
        if ($this->hasFlag(self::AF_LARGE) || $this->hasFlag(self::AF_MANYTOONE_AUTOCOMPLETE)) {
            return array('substring', 'exact', 'wildcard', 'regex');
        }

        return array('exact'); // only support exact search when searching with dropdowns
    }

    public function smartSearchCondition($id, $nr, $path, $query, $ownerAlias, $value, $mode)
    {
        if (Tools::count($path) > 0) {
            $this->createDestination();

            $destAlias = "ss_{$id}_{$nr}_".$this->fieldName();

            $query->addJoin($this->m_destInstance->m_table, $destAlias, $this->getJoinCondition($query, $ownerAlias, $destAlias), false);

            $attrName = array_shift($path);
            $attr = $this->m_destInstance->getAttribute($attrName);

            if (is_object($attr)) {
                $attr->smartSearchCondition($id, $nr + 1, $path, $query, $destAlias, $value, $mode);
            }
        } else {
            $this->searchCondition($query, $ownerAlias, $value, $mode);
        }
    }

    public function getSearchCondition(Query $query, $table, $value, $searchmode, $fieldname = '')
    {
        if (!$this->createDestination()) {
            return;
        }

        if (is_array($value)) {
            foreach ($this->m_listColumns as $attr) {
                $attrValue = $value[$attr];
                if (!empty($attrValue)) {
                    /** @var Attribute $p_attrib */
                    $p_attrib = $this->m_destInstance->m_attribList[$attr];
                    if (!$p_attrib == null) {
                        $p_attrib->searchCondition($query, $this->fieldName(), $attrValue, $this->getChildSearchMode($searchmode, $p_attrib->fieldName()));
                    }
                }
            }

            if (isset($value[$this->fieldName()])) {
                $value = $value[$this->fieldName()];
            }
        }


        if (empty($value)) {
            return '';
        } else {
            if (!$this->hasFlag(self::AF_LARGE) && !$this->hasFlag(self::AF_RELATION_AUTOCOMPLETE)) {
                // We only support 'exact' matches.
                // But you can select more than one value, which we search using the IN() statement,
                // which should work in any ansi compatible database.
                if (!is_array($value)) { // This last condition is for when the user selected the 'search all' option, in which case, we don't add conditions at all.
                    $value = array($value);
                }

                if (Tools::count($value) == 1) { // exactly one value

                    if ($value[0] == '__NONE__') {
                        return $query->nullCondition($table.'.'.$this->fieldName(), true);
                    } elseif ($value[0] != '') {
                        return $query->exactCondition($table.'.'.$this->fieldName(), $this->escapeSQL($value[0]), $this->dbFieldType());
                    }
                } else { // search for more values using IN()
                    return $table.'.'.$this->fieldName()." IN ('".implode("','", $value)."')";
                }
            } else { // AF_LARGE || AF_RELATION_AUTOCOMPLETE

                if($value[0] == ''){
                    return '';
                }


                // If we have a descriptor with multiple fields, use CONCAT
                $attribs = $this->m_destInstance->descriptorFields();
                $alias = $fieldname.$this->fieldName();
                if (Tools::count($attribs) > 1) {
                    $searchcondition = $this->getConcatFilter($value, $alias);
                } else {
                    // ask the destination node for it's search condition
                    $searchmode = $this->getChildSearchMode($searchmode, $this->fieldName());
                    $conditions = '';
                    foreach($value as $v) {
                        $sc = $this->m_destInstance->getSearchCondition($query, $alias, $fieldname, $v, $searchmode);

                        if($sc != '') {
                            $conditions[] = '('.$sc.')';
                        }
                    }
                    $searchcondition = implode(' OR ', $conditions);
                }

                return $searchcondition;
            }
        }
    }

    public function addToQuery($query, $tablename = '', $fieldaliasprefix = '', &$record, $level = 0, $mode = '')
    {
        if ($this->hasFlag(self::AF_MANYTOONE_LAZY)) {
            parent::addToQuery($query, $tablename, $fieldaliasprefix, $record, $level, $mode);

            return;
        }

        if ($this->createDestination()) {
            if ($mode != 'update' && $mode != 'add') {
                $alias = $fieldaliasprefix.$this->fieldName();
                $query->addJoin($this->m_destInstance->m_table, $alias, $this->getJoinCondition($query, $tablename, $alias), $this->m_leftjoin);
                $this->m_destInstance->addToQuery($query, $alias, $level + 1, false, $mode, $this->m_listColumns);
            } else {
                for ($i = 0, $_i = Tools::count($this->m_refKey); $i < $_i; ++$i) {
                    if ($record[$this->fieldName()] === null) {
                        $query->addField($this->m_refKey[$i], 'NULL', '', '', false);
                    } else {
                        $value = $record[$this->fieldName()];
                        if (is_array($value)) {
                            $fk = $this->m_destInstance->getAttribute($this->m_destInstance->m_primaryKey[$i]);
                            $value = $fk->value2db($value);
                        }

                        $query->addField($this->m_refKey[$i], $value, '', '', !$this->hasFlag(self::AF_NO_QUOTES));
                    }
                }
            }
        }
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
        return $this->_getSelectedRecord($record, $mode);
    }

    public function loadType($mode)
    {
        if (isset($this->m_loadType[$mode]) && $this->m_loadType[$mode] !== null) {
            return $this->m_loadType[$mode];
        } else {
            if (isset($this->m_loadType[null]) && $this->m_loadType[null] !== null) {
                return $this->m_loadType[null];
            } // Default backwardscompatible behaviour:
            else {
                if ($this->hasFlag(self::AF_MANYTOONE_LAZY)) {
                    return self::POSTLOAD | self::ADDTOQUERY;
                } else {
                    return self::ADDTOQUERY;
                }
            }
        }
    }

    public function validate(&$record, $mode)
    {
        $sessionmanager = SessionManager::getInstance();
        $storetype = null;
        if ($sessionmanager) {
            $storetype = $sessionmanager->stackVar('atkstore');
        }
        if ($storetype !== 'session' && !$this->_isSelectableRecord($record, $mode)) {
            Tools::triggerError($record, $this->fieldName(), 'error_integrity_violation');
        }
    }

    public function equal($recA, $recB)
    {
        if ($this->createDestination()) {
            return ($recA[$this->fieldName()][$this->m_destInstance->primaryKeyField()] == $recB[$this->fieldName()][$this->m_destInstance->primaryKeyField()]) || ($this->isEmpty($recA) && $this->isEmpty($recB));
            // we must also check empty values, because empty values need not necessarily
            // be equal (can be "", NULL or 0.
        }

        return false;
    }

    public function dbFieldType()
    {
        // The type of field that we need to store the foreign key, is equal to
        // the type of field of the primary key of the node we have a
        // relationship with.
        if ($this->createDestination()) {
            if (Tools::count($this->m_refKey) > 1) {
                $keys = [];
                for ($i = 0, $_i = Tools::count($this->m_refKey); $i < $_i; ++$i) {
                    /** @var Attribute $attrib */
                    $attrib = $this->m_destInstance->m_attribList[$this->m_destInstance->m_primaryKey[$i]];
                    $keys [] = $attrib->dbFieldType();
                }

                return $keys;
            } else {
                /** @var Attribute $attrib */
                $attrib = $this->m_destInstance->m_attribList[$this->m_destInstance->primaryKeyField()];

                return $attrib->dbFieldType();
            }
        }

        return '';
    }

    public function dbFieldSize()
    {
        // The size of the field we need to store the foreign key, is equal to
        // the size of the field of the primary key of the node we have a
        // relationship with.
        if ($this->createDestination()) {
            if (Tools::count($this->m_refKey) > 1) {
                $keys = [];
                for ($i = 0, $_i = Tools::count($this->m_refKey); $i < $_i; ++$i) {
                    /** @var Attribute $attrib */
                    $attrib = $this->m_destInstance->m_attribList[$this->m_destInstance->m_primaryKey[$i]];
                    $keys [] = $attrib->dbFieldSize();
                }

                return $keys;
            } else {
                /** @var Attribute $attrib */
                $attrib = $this->m_destInstance->m_attribList[$this->m_destInstance->primaryKeyField()];

                return $attrib->dbFieldSize();
            }
        }

        return 0;
    }

    /**
     * Returns the selected record for this many-to-one relation. Uses
     * the owner instance $this->fieldName()."_selected" method if it exists.
     *
     * @param array $record The record
     * @param string $mode The mode we're in
     *
     * @return array with the selected record
     */
    public function _getSelectedRecord($record = [], $mode = '')
    {
        $method = $this->fieldName().'_selected';
        if (method_exists($this->m_ownerInstance, $method)) {
            return $this->m_ownerInstance->$method($record, $mode);
        } else {
            return $this->getSelectedRecord($record, $mode);
        }
    }

    /**
     * Returns the currently selected record.
     *
     * @param array $record The record
     * @param string $mode The mode we're in
     *
     * @return array with the selected record
     */
    public function getSelectedRecord($record = [], $mode = '')
    {
        $this->createDestination();

        $condition = $this->m_destInstance->m_table.'.'.$this->m_destInstance->primaryKeyField()."='".$record[$this->fieldName()][$this->m_destInstance->primaryKeyField()]."'";

        $filter = $this->createFilter($record, $mode);
        if (!empty($filter)) {
            $condition = $condition.' AND '.$filter;
        }

        $record = $this->m_destInstance->select($condition)->getFirstRow();

        return $record;
    }

    /**
     * Returns the selectable records for this many-to-one relation. Uses
     * the owner instance $this->fieldName()."_selection" method if it exists.
     *
     * @param array $record The record
     * @param string $mode The mode we're in
     *
     * @return array with the selectable records
     */
    public function _getSelectableRecords($record = [], $mode = '')
    {
        $method = $this->fieldName().'_selection';
        if (method_exists($this->m_ownerInstance, $method)) {
            return $this->m_ownerInstance->$method($record, $mode);
        } else {
            return $this->getSelectableRecords($record, $mode);
        }
    }

    public function _getSelectableRecordsSelector($record = [], $mode = '')
    {
        $method = $this->fieldName().'_selectionSelector';
        if (method_exists($this->m_ownerInstance, $method)) {
            return $this->m_ownerInstance->$method($record, $mode);
        } else {
            return $this->getSelectableRecordsSelector($record, $mode);
        }
    }

    /**
     * Is selectable record? Uses the owner instance $this->fieldName()."_selectable"
     * method if it exists.
     *
     * @param array $record The record
     * @param string $mode The mode we're in
     *
     * @return bool to indicate if the record is selectable
     */
    public function _isSelectableRecord($record = [], $mode = '')
    {
        $method = $this->fieldName().'_selectable';
        if (method_exists($this->m_ownerInstance, $method)) {
            return $this->m_ownerInstance->$method($record, $mode);
        } else {
            return $this->isSelectableRecord($record, $mode);
        }
    }

    /**
     * Create the destination filter for the given record.
     *
     * @param array $record
     * @param string $mode (not used here, but usable in derived classes)
     *
     * @return string filter
     */
    public function createFilter($record, $mode)
    {
        if ($this->m_destinationFilter != '') {
            $parser = new StringParser($this->m_destinationFilter);

            return $parser->parse($record);
        } else {
            return '';
        }
    }

    /**
     * Is selectable record?
     *
     * Use this one from your selectable override when needed.
     *
     * @param array $record The record
     * @param string $mode The mode we're in
     *
     * @return bool to indicate if the record is selectable
     */
    public function isSelectableRecord($record = [], $mode = '')
    {
        if (!isset($record[$this->fieldName()]) || $record[$this->fieldName()] == null) {
            return false;
        }

        if (in_array($mode, array(
                'edit',
                'update',
            )) && ($this->hasFlag(self::AF_READONLY_EDIT) || $this->hasFlag(self::AF_HIDE_EDIT))
        ) { // || ($this->hasFlag(AF_LARGE) && !$this->hasFlag(AF_MANYTOONE_AUTOCOMPLETE))
            // in this case we want the current value is selectable, regardless the destination filters
            return true;
        }

        $this->createDestination();

        // if the value is set directly in the record field we first
        // need to convert the value to an array
        if (!is_array($record[$this->fieldName()])) {
            $record[$this->fieldName()] = array(
                $this->m_destInstance->primaryKeyField() => $record[$this->fieldName()],
            );
        }

        $selectedKey = $this->m_destInstance->primaryKey($record[$this->fieldName()]);
        if ($selectedKey == null) {
            return false;
        }

        // If custom selection method exists we use this one, although this is
        // way more inefficient, so if you create a selection override you should
        // also think about creating a selectable override!
        $method = $this->fieldName().'_selection';
        if (method_exists($this->m_ownerInstance, $method)) {
            $rows = $this->m_ownerInstance->$method($record, $mode);
            foreach ($rows as $row) {
                $key = $this->m_destInstance->primaryKey($row);
                if ($key == $selectedKey) {
                    return true;
                }
            }

            return false;
        }

        // No selection override exists, simply add the record key to the selector.
        $filter = $this->createFilter($record, $mode);
        $selector = "($selectedKey)".($filter != null ? " AND ($filter)" : '');

        return $this->m_destInstance->select($selector)->getRowCount() > 0;
    }

    /**
     * Returns the selectable records.
     *
     * Use this one from your selection override when needed.
     *
     * @param array $record The record
     * @param string $mode The mode we're in
     *
     * @return array with the selectable records
     */
    public function getSelectableRecords($record = [], $mode = '')
    {
        return $this->getSelectableRecordsSelector($record, $mode)->getAllRows();
    }

    public function getSelectableRecordsSelector($record = [], $mode = '')
    {
        $this->createDestination();

        $selector = $this->createFilter($record, $mode);
        $result = $this->m_destInstance->select($selector)->orderBy($this->getDestination()->getOrder())->includes(Tools::atk_array_merge($this->m_destInstance->descriptorFields(),
            $this->m_destInstance->m_primaryKey));

        return $result;
    }

    public function getJoinCondition($query, $tablename = '', $fieldalias = '')
    {
        if (!$this->createDestination()) {
            return false;
        }

        if ($tablename != '') {
            $realtablename = $tablename;
        } else {
            $realtablename = $this->m_ownerInstance->m_table;
        }
        $joinconditions = [];

        for ($i = 0, $_i = Tools::count($this->m_refKey); $i < $_i; ++$i) {
            $joinconditions[] = $realtablename.'.'.$this->m_refKey[$i].'='.$fieldalias.'.'.$this->m_destInstance->m_primaryKey[$i];
        }

        if ($this->m_joinFilter != '') {
            $parser = new StringParser($this->m_joinFilter);
            $filter = $parser->parse(array(
                'table' => $realtablename,
                'owner' => $realtablename,
                'destination' => $fieldalias,
            ));
            $joinconditions[] = $filter;
        }

        return implode(' AND ', $joinconditions);
    }

    /**
     * Make this relation hide itself from the form when there are no items to select.
     *
     * @param bool $hidewhenempty true - hide when empty, false - always show
     */
    public function setHideWhenEmpty($hidewhenempty)
    {
        $this->m_hidewhenempty = $hidewhenempty;
    }

    public function addToEditArray($mode, &$arr, &$defaults, &$error, $fieldprefix)
    {
        if ($this->createDestination()) {
            // check if destination table is empty
            // only check if hidewhenempty is set to true
            if ($this->m_hidewhenempty) {
                $recs = $this->_getSelectableRecords($defaults, $mode);
                if (Tools::count($recs) == 0) {
                    return $this->hide($defaults, $fieldprefix, $mode);
                }
            }
        }

        parent::addToEditArray($mode, $arr, $defaults, $error, $fieldprefix);
    }

    public function getOrderByStatement($extra = '', $table = '', $direction = 'ASC')
    {
        if (!$this->createDestination()) {
            return parent::getOrderByStatement();
        }

        if (!empty($table)) {
            $table = $table.'_AE_'.$this->fieldName();
        } else {
            $table = $this->fieldName();
        }

        if (!empty($extra) && in_array($extra, $this->m_listColumns)) {
            return $this->getDestination()->getAttribute($extra)->getOrderByStatement('', $table, $direction);
        }

        $order = $this->m_destInstance->getOrder();

        if (!empty($order)) {
            $newParts = [];
            $parts = explode(',', $order);

            foreach ($parts as $part) {
                $split = preg_split('/\s+/', trim($part));
                $field = isset($split[0]) ? $split[0] : null;
                $fieldDirection = empty($split[1]) ? 'ASC' : strtoupper($split[1]);

                // if our default direction is DESC (the opposite of the default ASC)
                // we always have to switch the given direction to be the opposite, e.g.
                // DESC => ASC and ASC => DESC, this way we respect the default ordering
                // in the destination node even if the default is descending
                if ($fieldDirection == 'DESC') {
                    $fieldDirection = $direction == 'DESC' ? 'ASC' : 'DESC';
                } else {
                    $fieldDirection = $direction;
                }

                if (strpos($field, '.') !== false) {
                    list(, $field) = explode('.', $field);
                }

                $newPart = $this->getDestination()->getAttribute($field)->getOrderByStatement('', $table, $fieldDirection);

                // realias if destination order contains the wrong tablename.
                if (strpos($newPart, $this->m_destInstance->m_table.'.') !== false) {
                    $newPart = str_replace($this->m_destInstance->m_table.'.', $table.'.', $newPart);
                }
                $newParts[] = $newPart;
            }

            return implode(', ', $newParts);
        } else {
            $fields = $this->m_destInstance->descriptorFields();
            if (Tools::count($fields) == 0) {
                $fields = array($this->m_destInstance->primaryKeyField());
            }

            $order = '';
            foreach ($fields as $field) {
                $order .= (empty($order) ? '' : ', ').$table.'.'.$field;
            }

            return $order;
        }
    }


    public function addToListArrayHeader(
        $action,
        &$arr,
        $fieldprefix,
        $flags,
        $atksearch,
        $columnConfig,
        DataGrid $grid = null,
        $column = '*'
    ) {
        if ($column == null || $column == '*') {
            $prefix = $fieldprefix.$this->fieldName().'_AE_';
            parent::addToListArrayHeader($action, $arr, $prefix, $flags, $atksearch[$this->fieldName()], $columnConfig, $grid, null);
        }

        if ($column == '*') {
            // only add extra columns when needed
            if ($this->hasFlag(self::AF_HIDE_LIST) && !$this->m_alwaysShowListColumns) {
                return;
            }
            if (!$this->createDestination() || Tools::count($this->m_listColumns) == 0) {
                return;
            }

            foreach ($this->m_listColumns as $column) {
                $this->_addColumnToListArrayHeader($column, $action, $arr, $fieldprefix, $flags, $atksearch, $columnConfig, $grid);
            }
        } else {
            if ($column != null) {
                $this->_addColumnToListArrayHeader($column, $action, $arr, $fieldprefix, $flags, $atksearch, $columnConfig, $grid);
            }
        }
    }

    /**
     * Adds the child attribute / field to the list row.
     *
     * Framework method. It should not be necessary to call this method directly.
     *
     * @param string $column child column (null for this attribute, * for this attribute and all childs)
     * @param string $action the action that is being performed on the node
     * @param array $arr reference to the the recordlist array
     * @param string $fieldprefix the fieldprefix
     * @param int $flags the recordlist flags
     * @param array $atksearch the current ATK search list (if not empty)
     * @param ColumnConfig $columnConfig order by
     * @param DataGrid $grid The DataGrid this attribute lives on.
     * @throws Exception
     */
    protected function _addColumnToListArrayHeader(
        $column,
        $action,
        &$arr,
        $fieldprefix,
        $flags,
        $atksearch,
        $columnConfig,
        DataGrid $grid = null
    ) {
        $prefix = $fieldprefix.$this->fieldName().'_AE_';

        $p_attrib = $this->m_destInstance->getAttribute($column);
        if ($p_attrib == null) {
            throw new Exception("Invalid list column {$column} for ManyToOneRelation ".$this->getOwnerInstance()->atkNodeUri().'::'.$this->fieldName());
        }

        $p_attrib->m_flags |= self::AF_HIDE_LIST;
        $p_attrib->m_flags ^= self::AF_HIDE_LIST;
        $p_attrib->addToListArrayHeader($action, $arr, $prefix, $flags, $atksearch[$this->fieldName()], $columnConfig, $grid, null);

        // fix order by clause
        $needle = $prefix.$column;
        foreach (array_keys($arr['heading']) as $key) {
            if (strpos($key, $needle) !== 0) {
                continue;
            }

            if (empty($arr['heading'][$key]['order'])) {
                continue;
            }

            $order = $this->fieldName().'.'.$arr['heading'][$key]['order'];

            if (is_object($columnConfig) && isset($columnConfig->m_colcfg[$this->fieldName()]) && isset($columnConfig->m_colcfg[$this->fieldName()]['extra']) && $columnConfig->m_colcfg[$this->fieldName()]['extra'] == $column) {
                $direction = $columnConfig->getDirection($this->fieldName());
                if ($direction == 'asc') {
                    $order .= ' desc';
                }
            }

            $arr['heading'][$key]['order'] = $order;
        }
    }

    public function addToListArrayRow(
        $action,
        &$arr,
        $nr,
        $fieldprefix,
        $flags,
        $edit = false,
        DataGrid $grid = null,
        $column = '*'
    ) {
        if ($column == null || $column == '*') {
            $prefix = $fieldprefix.$this->fieldName().'_AE_';
            parent::addToListArrayRow($action, $arr, $nr, $prefix, $flags, $edit, $grid, null);
        }

        if ($column == '*') {
            // only add extra columns when needed
            if ($this->hasFlag(self::AF_HIDE_LIST) && !$this->m_alwaysShowListColumns) {
                return;
            }
            if (!$this->createDestination() || Tools::count($this->m_listColumns) == 0) {
                return;
            }

            foreach ($this->m_listColumns as $column) {
                $this->_addColumnToListArrayRow($column, $action, $arr, $nr, $fieldprefix, $flags, $edit, $grid);
            }
        } else {
            if ($column != null) {
                $this->_addColumnToListArrayRow($column, $action, $arr, $nr, $fieldprefix, $flags, $edit, $grid);
            }
        }
    }

    /**
     * Adds the child attribute / field to the list row.
     *
     * @param string $column child attribute name
     * @param string $action the action that is being performed on the node
     * @param array $arr reference to the the recordlist array
     * @param int $nr the current row number
     * @param string $fieldprefix the fieldprefix
     * @param int $flags the recordlist flags
     * @param bool $edit editing?
     * @param DataGrid $grid data grid
     * @throws Exception
     */
    protected function _addColumnToListArrayRow(
        $column,
        $action,
        &$arr,
        $nr,
        $fieldprefix,
        $flags,
        $edit = false,
        DataGrid $grid = null
    ) {
        $prefix = $fieldprefix.$this->fieldName().'_AE_';

        // small trick, the destination record is in a subarray. The destination
        // addToListArrayRow will not expect this though, so we have to modify the
        // record a bit before passing it to the detail columns.
        $backup = $arr['rows'][$nr]['record'];
        $arr['rows'][$nr]['record'] = $arr['rows'][$nr]['record'][$this->fieldName()];

        $p_attrib = $this->m_destInstance->getAttribute($column);
        if ($p_attrib == null) {
            throw new Exception("Invalid list column {$column} for ManyToOneRelation ".$this->getOwnerInstance()->atkNodeUri().'::'.$this->fieldName());
        }

        $p_attrib->m_flags |= self::AF_HIDE_LIST;
        $p_attrib->m_flags ^= self::AF_HIDE_LIST;

        $p_attrib->addToListArrayRow($action, $arr, $nr, $prefix, $flags, $edit, $grid, null);

        $arr['rows'][$nr]['record'] = $backup;
    }

    public function addToSearchformFields(&$fields, $node, &$record, $fieldprefix = '', $extended = true)
    {
        $prefix = $fieldprefix.$this->fieldName().'_AE_';

        parent::addToSearchformFields($fields, $node, $record, $prefix, $extended);

        // only add extra columns when needed
        if ($this->hasFlag(self::AF_HIDE_LIST) && !$this->m_alwaysShowListColumns) {
            return;
        }
        if (!$this->createDestination() || Tools::count($this->m_listColumns) == 0) {
            return;
        }

        foreach ($this->m_listColumns as $attribname) {
            /** @var Attribute $p_attrib */
            $p_attrib = $this->m_destInstance->m_attribList[$attribname];
            $p_attrib->m_flags |= self::AF_HIDE_LIST;
            $p_attrib->m_flags ^= self::AF_HIDE_LIST;

            if (!$p_attrib->hasFlag(self::AF_HIDE_SEARCH)) {
                $p_attrib->addToSearchformFields($fields, $node, $record[$this->fieldName()], $prefix, $extended);
            }
        }
    }

    /**
     * Retrieve the sortorder for the listheader based on the
     * ColumnConfig.
     *
     * @param ColumnConfig $columnConfig The config that contains options for
     *                                   extended sorting and grouping to a
     *                                   recordlist.
     *
     * @return string Returns sort order ASC or DESC
     */
    public function listHeaderSortOrder(ColumnConfig $columnConfig)
    {
        $order = $this->fieldName();

        // only add desc if not one of the listColumns is used for the sorting
        if (isset($columnConfig->m_colcfg[$order]) && empty($columnConfig->m_colcfg[$order]['extra'])) {
            $direction = $columnConfig->getDirection($order);
            if ($direction == 'asc') {
                $order .= ' desc';
            }
        }

        return $order;
    }


    /**
     * Draw the auto-complete box for the edit type
     *
     * @param array $record The record
     * @param string $fieldprefix The fieldprefix
     * @param string $mode The mode we're in
     * @return string html
     */
    public function drawAutoCompleteBox($record, $fieldprefix, $mode)
    {
        $this->createDestination();

        $page = $this->m_ownerInstance->getPage();
        $page->register_script(Config::getGlobal('assets_url').'javascript/manytoonerelation.js');
        $id = $this->getHtmlId($fieldprefix);
        $name = $this->getHtmlName($fieldprefix);

        $result = '';
        $label = '';
        $value = '';
        $options = [];
        $selValues = [];
        $currentValue = null;
        $hasNullOption = false;
        $noneLabel = '';
        $emptyValue = '';
        $htmlAttributes = [];

        // validate is this is a selectable record and if so retrieve the display label and hidden value
        if ($this->_isSelectableRecord($record, $mode)) {
            $currentValue = $record[$this->fieldName()];
            $label = $this->m_destInstance->descriptor($record[$this->fieldName()]);
            $value = $this->m_destInstance->primaryKey($record[$this->fieldName()]);
        }

        // create the widget
        if ($this->hasFlag(self::AF_MANYTOONE_OBLIGATORY_NULL_ITEM) || (!$this->hasFlag(self::AF_OBLIGATORY) && !$this->hasFlag(self::AF_RELATION_NO_NULL_ITEM)) || (Config::getGlobal('list_obligatory_null_item') && !is_array($value))) {
            $hasNullOption = true;
            $noneLabel = $this->getNoneLabel($mode);
            $options[$emptyValue] = $noneLabel;
        }

        if ($currentValue) {
            $options[$value] = $label;
            $selValues[] = $value;
        }


        $ajaxUrlParams = [];
        if($this->m_destinationFilter && ($mode == 'edit' || $mode == 'add')) {
            $ajaxUrlParams['atkfilter'] =  $this->parseFilter($this->m_destinationFilter, $record);
        }

        $selectOptions = [];
        $selectOptions['enable-select2'] = true;
        $selectOptions['enable-manytoonereleation-autocomplete'] = true;
        $selectOptions['dropdown-auto-width'] = false;
        $selectOptions['ajax--url'] = Tools::partial_url($this->m_ownerInstance->atkNodeUri(), $mode, 'attribute.'.$this->fieldName().'.autocomplete', $ajaxUrlParams);
        $selectOptions['minimum-input-length'] = $this->m_autocomplete_minchars;
        $selectOptions['width'] = '100%';

        // standard select2 with clear button
        if ($hasNullOption) {
            $selectOptions['allow-clear'] = true;
            $selectOptions['placeholder'] = $noneLabel;
        }

        $selectOptions = array_merge($selectOptions, $this->m_select2Options['edit']);

        if (Tools::count($this->m_onchangecode)) {
            $htmlAttributes['onchange'] = $this->getHtmlId($fieldprefix).'_onChange(this)';
            $this->_renderChangeHandler($fieldprefix);
        }

        $style = '';
        foreach($this->getCssStyles('edit') as $k => $v) {
            $style .= "$k:$v;";
        }
        if($style != ''){
            $htmlAttributes['style'] = $style;
        }

        $result .= $this->drawSelect($id, $name, $options, $selValues, $selectOptions, $htmlAttributes);
        $result .= ' '.$this->createSelectAndAutoLinks($name, $record);


        $result = '<span class="select-inline">'.$result.'</span>';

        return $result;
    }


    /**
     * Auto-complete partial.
     *
     * @param string $mode add/edit mode?
     * @return string html
     */
    public function partial_autocomplete($mode)
    {
        $this->createDestination();

        $searchvalue = $this->escapeSQL($this->m_ownerInstance->m_postvars['value']);
        $filter = $this->createSearchFilter($searchvalue);
        $this->addDestinationFilter($filter);

        // use "request" because "m_postvars" yelds stange results...
        if(isset($_REQUEST['atkfilter'])) {
            $this->addDestinationFilter($_REQUEST['atkfilter']);
        }

        $record = $this->m_ownerInstance->updateRecord();
        $result = "\n";
        $limit = $this->m_autocomplete_pagination_limit;
        $page = 1;
        if (isset($this->m_ownerInstance->m_postvars['page']) && is_numeric($this->m_ownerInstance->m_postvars['page'])) {
            $page = $this->m_ownerInstance->m_postvars['page'];
        }
        $offset = ($page - 1) * $limit;

        $selector = $this->_getSelectableRecordsSelector($record, $mode);
        $selector->limit($limit, $offset);
        $count = $selector->getRowCount();
        $iterator = $selector->getIterator();
        $more = ($offset + $limit > $count) ? 'false' : 'true';

        $result .= '<div id="total">'.$count.'</div>'."\n";
        $result .= '<div id="page">'.$page.'</div>'."\n";
        $result .= '<div id="more">'.$more.'</div>'."\n";

        $result .= '<ul>';
        foreach ($iterator as $rec) {
            $option = $this->m_destInstance->descriptor($rec);
            $value = $this->m_destInstance->primaryKey($rec);
            $result .= '
          <li value="'.htmlentities($value).'">'.htmlentities($option).'</li>';
        }
        $result .= '</ul>';

        return $result;
    }

    /**
     * Auto-complete search partial.
     *
     * @return string HTML code with autocomplete result
     */
    public function partial_autocomplete_search()
    {
        $this->createDestination();
        $searchvalue = $this->escapeSQL($this->m_ownerInstance->m_postvars['value']);
        $filter = $this->createSearchFilter($searchvalue);
        $this->addDestinationFilter($filter);
        $record = [];
        $mode = 'search';

        $result = "\n";
        $limit = $this->m_autocomplete_pagination_limit;
        $page = 1;
        if (isset($this->m_ownerInstance->m_postvars['page']) && is_numeric($this->m_ownerInstance->m_postvars['page'])) {
            $page = $this->m_ownerInstance->m_postvars['page'];
        }
        $offset = ($page - 1) * $limit;

        $selector = $this->_getSelectableRecordsSelector($record, $mode);
        $selector->limit($limit, $offset);
        $count = $selector->getRowCount();
        $iterator = $selector->getIterator();
        $more = ($offset + $limit > $count) ? 'false' : 'true';

        $result .= '<div id="total">'.$count.'</div>'."\n";
        $result .= '<div id="page">'.$page.'</div>'."\n";
        $result .= '<div id="more">'.$more.'</div>'."\n";

        $result .= '<ul>';
        foreach ($iterator as $rec) {
            $option = $value = $this->m_destInstance->descriptor($rec);
            $result .= '
          <li value="'.htmlentities($value).'">'.htmlentities($option).'</li>';
        }
        $result .= '</ul>';

        return $result;
    }


    /**
     * Creates a search filter with the given search value on the given
     * descriptor fields.
     *
     * @param string $searchvalue A searchstring
     *
     * @return string a search string (WHERE clause)
     */
    public function createSearchFilter($searchvalue)
    {
        if ($this->m_autocomplete_searchfields == '') {
            $searchfields = $this->m_destInstance->descriptorFields();
        } else {
            $searchfields = $this->m_autocomplete_searchfields;
        }

        $parts = preg_split('/\s+/', $searchvalue);

        $mainFilter = [];
        foreach ($parts as $part) {
            $filter = [];
            foreach ($searchfields as $attribname) {
                if (strstr($attribname, '.')) {
                    $table = '';
                } else {
                    $table = $this->m_destInstance->m_table.'.';
                }

                if (!$this->m_autocomplete_search_case_sensitive) {
                    $tmp = 'LOWER('.$table.$attribname.')';
                } else {
                    $tmp = $table.$attribname;
                }

                switch ($this->m_autocomplete_searchmode) {
                    case self::SEARCH_MODE_EXACT:
                        if (!$this->m_autocomplete_search_case_sensitive) {
                            $tmp .= " = LOWER('{$part}')";
                        } else {
                            $tmp .= " = '{$part}'";
                        }
                        break;
                    case self::SEARCH_MODE_STARTSWITH:
                        if (!$this->m_autocomplete_search_case_sensitive) {
                            $tmp .= " LIKE LOWER('{$part}%')";
                        } else {
                            $tmp .= " LIKE '{$part}%'";
                        }
                        break;
                    case self::SEARCH_MODE_CONTAINS:
                        if (!$this->m_autocomplete_search_case_sensitive) {
                            $tmp .= " LIKE LOWER('%{$part}%')";
                        } else {
                            $tmp .= " LIKE '%{$part}%'";
                        }
                        break;
                    default:
                        $tmp .= " = LOWER('{$part}')";
                }

                $filter[] = $tmp;
            }

            if (Tools::count($filter) > 0) {
                $mainFilter[] = '('.implode(') OR (', $filter).')';
            }
        }

        if (Tools::count($mainFilter) > 0) {
            $searchFilter = '('.implode(') AND (', $mainFilter).')';
        } else {
            $searchFilter = '';
        }

        // When no searchfields are specified and we use the CONTAINS mode
        // add a concat filter
        if ($this->m_autocomplete_searchmode == self::SEARCH_MODE_CONTAINS && $this->m_autocomplete_searchfields == '') {
            $filter = $this->getConcatFilter($searchvalue);
            if ($filter) {
                if ($searchFilter != '') {
                    $searchFilter .= ' OR ';
                }
                $searchFilter .= $filter;
            }
        }

        return $searchFilter;
    }

    /**
     * Get Concat filter.
     *
     * @param string $searchValue Search value
     * @param string $fieldaliasprefix Field alias prefix
     *
     * @return string|bool
     */
    public function getConcatFilter($searchValue, $fieldaliasprefix = '')
    {
        // If we have a descriptor with multiple fields, use CONCAT
        $attribs = $this->m_destInstance->descriptorFields();
        if (Tools::count($attribs) > 1) {
            $fields = [];
            foreach ($attribs as $attribname) {
                $post = '';
                if (strstr($attribname, '.')) {
                    if ($fieldaliasprefix != '') {
                        $table = $fieldaliasprefix.'_AE_';
                    } else {
                        $table = '';
                    }
                    $post = substr($attribname, strpos($attribname, '.'));
                    $attribname = substr($attribname, 0, strpos($attribname, '.'));
                } elseif ($fieldaliasprefix != '') {
                    $table = $fieldaliasprefix.'.';
                } else {
                    $table = $this->m_destInstance->m_table.'.';
                }

                /** @var Attribute $p_attrib */
                $p_attrib = $this->m_destInstance->m_attribList[$attribname];
                $fields[$p_attrib->fieldName()] = $table.$p_attrib->fieldName().$post;
            }

            if (is_array($searchValue)) {
                // (fix warning trim function)
                $searchValue = $searchValue[0];
            }

            $value = $this->escapeSQL(trim($searchValue));
            $value = str_replace('  ', ' ', $value);
            if (!$value) {
                return false;
            } else {
                $function = $this->getConcatDescriptorFunction();
                if ($function != '' && method_exists($this->m_destInstance, $function)) {
                    $descriptordef = $this->m_destInstance->$function();
                } elseif ($this->m_destInstance->m_descTemplate != null) {
                    $descriptordef = $this->m_destInstance->m_descTemplate;
                } elseif (method_exists($this->m_destInstance, 'descriptor_def')) {
                    $descriptordef = $this->m_destInstance->descriptor_def();
                } else {
                    $descriptordef = $this->m_destInstance->descriptor(null);
                }

                $parser = new StringParser($descriptordef);
                $concatFields = $parser->getAllParsedFieldsAsArray($fields, true);
                $concatTags = $concatFields['tags'];
                $concatSeparators = $concatFields['separators'];
                $concatSeparators[] = ' '; //the query removes all spaces, so let's do the same here [Bjorn]
                // to search independent of characters between tags, like spaces and comma's,
                // we remove all these separators so we can search for just the concatenated tags in concat_ws [Jeroen]
                foreach ($concatSeparators as $separator) {
                    $value = str_replace($separator, '', $value);
                }

                $db = $this->getDb();
                $searchcondition = 'UPPER('.$db->func_concat_ws($concatTags, '', true).") LIKE UPPER('%".$value."%')";
            }

            return $searchcondition;
        }

        return false;
    }

    /**
     * @param bool $normal
     * @param bool $extended
     * @return array $m_multipleSearch
     */
    public function setMultipleSearch($normal = true, $extended = true)
    {
        $this->m_multipleSearch = [
            'normal' => $normal,
            'extended' => $extended,
        ];

        return $this->m_multipleSearch;
    }

    public function getMultipleSearch()
    {
        return $this->m_multipleSearch;
    }

    /**
     * @param bool $extended
     * @return bool
     */
    public function isMultipleSearch($extended)
    {
        $ms = $this->getMultipleSearch();

        return $ms[$extended ? 'extended' : 'normal'];
    }
}

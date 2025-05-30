<?php

namespace Sintattica\Atk\Core;

use Exception;
use Sintattica\Atk\AdminLte\UIStateColors;
use Sintattica\Atk\Attributes\Attribute;
use Sintattica\Atk\Attributes\ButtonAttribute;
use Sintattica\Atk\Attributes\DateAttribute;
use Sintattica\Atk\Attributes\DateTimeAttribute;
use Sintattica\Atk\Attributes\FieldSet;
use Sintattica\Atk\Attributes\FileAttribute;
use Sintattica\Atk\Attributes\JsonAttribute;
use Sintattica\Atk\Attributes\ListAttribute;
use Sintattica\Atk\Attributes\MultiListAttribute;
use Sintattica\Atk\Attributes\StateColorAttribute;
use Sintattica\Atk\Db\Db;
use Sintattica\Atk\Db\Query;
use Sintattica\Atk\Handlers\ActionHandler;
use Sintattica\Atk\Handlers\AddHandler;
use Sintattica\Atk\Handlers\AdminHandler;
use Sintattica\Atk\Handlers\EditHandler;
use Sintattica\Atk\Handlers\SearchHandler;
use Sintattica\Atk\Handlers\ViewHandler;
use Sintattica\Atk\RecordList\ColumnConfig;
use Sintattica\Atk\Relations\ManyToOneRelation;
use Sintattica\Atk\Relations\Relation;
use Sintattica\Atk\Security\SecurityManager;
use Sintattica\Atk\Session\SessionManager;
use Sintattica\Atk\Session\State;
use Sintattica\Atk\Ui\Footer;
use Sintattica\Atk\Ui\Page;
use Sintattica\Atk\Ui\PageBuilder;
use Sintattica\Atk\Ui\Ui;
use Sintattica\Atk\Utils\ActionListener;
use Sintattica\Atk\Utils\EditFormModifier;
use Sintattica\Atk\Utils\Selector;
use Sintattica\Atk\Utils\StringParser;
use Throwable;

/**
 * The Node class represents a piece of information that is part of an
 * application. This class provides standard functionality for adding,
 * editing and deleting nodes.
 * This class must be seen as an abstract base class: For every piece of
 * information in an application, a class must be derived from this class
 * with specific implementations for that type of node.
 *
 * @author Ivo Jansch <ivo@achievo.org>
 */
class Node
{
    /**
     * Define some flags for nodes. Use the constructor of the Node
     * class to set the flags. (concatenate multiple flags with '|').
     */
    /**
     * No new records may be added.
     */
    const NF_NO_ADD = 1;

    /**
     * Records may not be edited.
     */
    const NF_NO_EDIT = 2;

    /**
     * Records may not be deleted.
     */
    const NF_NO_DELETE = 4;

    /**
     * Immediately after you add a new record,
     * you get the editpage for that record.
     */
    const NF_EDITAFTERADD = 8;

    /**
     * Records may not be searched.
     */
    const NF_NO_SEARCH = 16;

    /**
     * Ignore addFilter filters.
     */
    const NF_NO_FILTER = 32;

    /**
     * Doesn't show an add form on the admin page
     * but a link to the form.
     */
    const NF_ADD_LINK = 64;

    /**
     * Records may not be viewed.
     */
    const NF_NO_VIEW = 128;

    /**
     * Records / trees may be copied.
     */
    const NF_COPY = 256;

    /**
     * If this flag is set and only one record is
     * present on a selectpage, atk automagically
     * selects it and moves on to the target.
     */
    const NF_AUTOSELECT = 512;

    /**
     * If set, atk stores the old values of
     * a record as ["atkorgrec"] in the $rec that
     * gets passed to the postUpdate.
     */
    const NF_TRACK_CHANGES = 1024;

    /**
     * Quick way to disable accessright checking
     * for an entire node. (Everybody may access this node).
     */
    const NF_NO_SECURITY = 2048;

    /**
     * Extended search feature is turned off.
     */
    const NF_NO_EXTENDED_SEARCH = 4096;

    /**
     * Multi-selection of records is turned on.
     */
    const NF_MULTI_RECORD_ACTIONS = 8192;

    /**
     * Multi-priority-selection of records is turned on.
     */
    const NF_MRPA = 16384;

    /**
     * Quick way to ensable the csv import feature.
     */
    const NF_IMPORT = 131072;

    /**
     * Add CSV export ability to the node.
     */
    const NF_EXPORT = 262144;

    /**
     * Enable extended sorting (multicolumn sort).
     */
    const NF_EXT_SORT = 524288;

    /**
     * Makes a node cache it's recordlist.
     */
    const NF_CACHE_RECORDLIST = 1048576;

    /**
     * After adding a new record add another one instantaniously.
     */
    const NF_ADDAFTERADD = 2097152;

    /**
     * No sorting possible.
     */
    const NF_NO_SORT = 4194304;

    /**
     * Specific node flag 1.
     */
    const NF_SPECIFIC_1 = 33554432;

    /**
     * Specific node flag 2.
     */
    const NF_SPECIFIC_2 = 67108864;

    /**
     * Specific node flag 3.
     */
    const NF_SPECIFIC_3 = 134217728;

    /**
     * Specific node flag 4.
     */
    const NF_SPECIFIC_4 = 268435456;

    /**
     * Specific node flag 5.
     */
    const NF_SPECIFIC_5 = 536870912;

    /**
     * Records may be copied and open for editing.
     */
    const NF_EDITAFTERCOPY = 1073741824;

    /**
     * Alias for NF_MULTI_RECORD_ACTIONS flag (shortcut).
     */
    const NF_MRA = 8192;

    /**
     * Aggregate flag to quickly create readonly nodes.
     */
    const NF_READONLY = 7;

    /**
     * Multi-record-actions selection modes. These
     * modes are mutually exclusive.
     */
    /**
     * Multiple selections possible.
     */
    const MRA_MULTI_SELECT = 1;

    /**
     * Only one selection possible.
     */
    const MRA_SINGLE_SELECT = 2;

    /**
     * No selection possible (e.g. action is always for all (visible) records!).
     */
    const MRA_NO_SELECT = 3;

    const DEFAULT_RECORDLIST_BG_COLOR = UIStateColors::COLOR_WHITE;

    const ROW_COLOR_MODE_CELL = 'cell';
    const ROW_COLOR_MODE_DEFAULT = 'row';
    const ROW_COLOR_ATTRIBUTE = '_row_color_attribute';

    /**
     * Attribute prefix, useful to show|hide default or special attributes.
     */
    const PREFIX_FIELDSET = 'fieldset';
    const PREFIX_DEFAULT = 'fieldset';
    const PREFIX_TABBED_PANE = 'tabbedPaneAttr';

    const ACTION_DOWNLOAD_FILE_ATTRIBUTE = 'download_file_attribute';
    const PARAM_ATTRIBUTE_NAME = 'attribute_name';
    const PARAM_ATKSELECTOR = 'atkselector';
    const PARAM_ATKMENU = 'atkmenu';
    const ATK_ORG_REC = 'atkorgrec';

    /*
     * reference to the class which is used to validate atknodes
     * the validator is overridable by changing this variable
     *
     * @access private
     * @var String
     */
    public $m_validate_class = NodeValidator::class;

    /*
     * Unique field sets of a certain node.
     *
     * Indicates which field combinations should be unique.
     * It doesn't contain the unique fields which have been set by flag
     * Attribute::AF_UNIQUE.
     *
     * @access private
     * @var array
     */
    public $m_uniqueFieldSets = [];

    /*
     * Nodes must be initialised using the init() function before they can be
     * used. This member indicated whether the node has been initialised.
     * @access private
     * @var boolean
     */
    public $m_initialised = false;

    /**
     * Check to prevent double execution of setAttribSizes on pages with more
     * than one form.
     * @access private
     * @var boolean $m_attribsizesset
     */
    public $m_attribsizesset = false;

    /**
     * The list of attributes of a node. These should be of the class
     * Attribute or one of its derivatives.
     * @access private
     * @var Attribute[] $m_attribList
     */
    public $m_attribList = [];

    /**
     * Name of the custom submit attributes that are different
     * from the ATK built in ones (ex: ButtonAttribute)
     * @var array
     */
    private $submitBtnAttribList = [];

    /*
     * Index list containing the attributes in the order in which they will
     * appear on screen.
     * @access private
     * @var array
     */
    public $m_attribIndexList = [];

    /*
     * Reference to the page on which the node is rendering its output.
     * @access private
     * @var Page
     */
    public $m_page = null;

    /*
     * List of available tabs. Associative array structured like this:
     * array($action=>$arrayOfTabnames)
     * @access private
     * @var array
     */
    public $m_tabList = [];

    /*
     * List of available sections. Associative array structured like this:
     * array($action=>$arrayOfSectionnames)
     * @access private
     * @var array
     */
    public $m_sectionList = [];

    /*
     * Keep track of tabs per attribute.
     * @access private
     * @var array
     */
    public $m_attributeTabs = [];

    /*
     * Keep track if a tab contains attribs (checkEmptyTabs function)
     * @access private
     * @var array
     */
    public $m_filledTabs = [];

    /*
     * The type of the node.
     * @access protected
     * @var String
     */
    public $m_type;

    /*
     * The module of the node.
     * @access protected
     * @var String
     */
    public $m_module;

    /*
     * The database that the node is using for storing and loading its data.
     * @access protected
     * @var mixed
     */
    public $m_db = null;

    /*
     * The table to use for data storage.
     * @access protected
     * @var String
     */
    public $m_table;

    /*
     * The name of the sequence used for autoincrement fields.
     * @access protected
     * @var String
     */
    public $m_seq;

    /*
     * List of names of the attributes that form this node's primary key.
     * @access protected
     * @var array
     */
    public $m_primaryKey = [];

    /*
     * The postvars (or getvars) that are passed to a page will be passed
     * to the class using the dispatch function. We store them in a member
     * variable for easy access.
     * @access protected
     * @var array
     */
    public $m_postvars = [];

    /*
     * The action that the node is currently performing.
     * @access protected
     * @var String
     */
    public $m_action;

    /*
     * Contains the definition of what needs to rendered partially.
     * If set to NULL not in partial rendering mode.
     */
    public $m_partial = null;

    /*
     * The active action handler.
     * @access protected
     * @var ActionHandler
     */
    public $m_handler = null;

    /*
     * Default order by statement.
     * @access protected
     * @var String
     */
    public $m_default_order = '';

    /*
     * Bitwise mask of node flags (self::NF_* flags).
     * @var int
     */
    public $m_flags;

    /*
     * Name of the field that is used for creating an alphabetical index in
     * admin/select pages.
     * @access private
     * @var String
     */
    public $m_index = '';

    /*
     * Default tab being displayed in add/edit mode.
     * @access private
     * @var String
     */
    public $m_default_tab = 'default';

    /*
     * Default sections that are expanded.
     * @access private
     * @var String
     */
    public $m_default_expanded_sections = [];

    /*
     * Record filters, in attributename/required value pairs.
     * @access private
     * @var array
     */
    public $m_filters = [];

    /*
     * Record filters, as a list of sql statements.
     * @access private
     * @var array
     */
    public $m_fuzzyFilters = [];

    /*
     * For speed, we keep track of a list of attributes that we don't have to
     * load in recordlists.
     * @access protected
     * @var array
     */
    public $m_listExcludes = [];

    /*
     * For speed, we keep track of a list of attributes that we don't have to
     * load when in view pages.
     * @todo This can probably be moved to the view handler.
     * @access protected
     * @var array
     */
    public $m_viewExcludes = [];

    /*
     * For speed, we keep track of a list of attributes that have the cascade
     * delete flag set.
     * @todo This should be moved to the delete handler, or should not be
     *       cached at all. (caching this on each load is slower than just
     *       retrieving the list when it's needed)
     * @access private
     * @var array
     */
    public $m_cascadingAttribs = [];

    /*
     * Actions are mapped to security units.
     *
     * For example, both actions "save" and "add" require access "add". If an
     * item is not in this list, it's treated 'as-is'. Derived nodes may add
     * more mappings to tell the systems that some custom actions require the
     * same privilege as others.
     * Structure: array($action=>$requiredPrivilege)
     * @access protected
     * @var array
     */
    public $m_securityMap = [
        'save' => 'add',
        'update' => 'edit',
        'multiupdate' => 'edit',
        'copy' => 'add',
        'import' => 'add',
        'editcopy' => 'add',
        'search' => 'admin',
        'smartsearch' => 'admin',
    ];

    /*
     * The right to execute certain actions can be implied by the fact that you
     * have some other right. For example, if you have the right to access a
     * feature (admin right), you may also view that record, and don't need
     * explicit rights to view it. So the 'view' right is said to be 'implied'
     * by the 'admin' right.
     * This is a subtle difference with m_securityMap.
     * @access protected
     * @var array
     */
    public $m_securityImplied = ['view' => 'admin'];

    /*
     * Name of the node that is used for privilege checking.
     *
     * If a class is named 'project', then by default, if the system needs to
     *  know whether a user may edit a record, the securitymanager searches
     * for 'edit' access on 'project'. However, if an alias is set here, the
     * securitymanger searches for 'edit' on that alias.
     * @access private
     * @var String
     */
    public $m_securityAlias = '';

    /*
     * Nodes can specify actions that require no access level
     * Note: for the moment, the "select" action is always allowed.
     * @todo This may not be correct. We have to find a way to bind the
     * select action to the action that follows after the select.
     * @access private
     * @var array
     */
    public $m_unsecuredActions = ['select', 'multiselect', 'feedback'];

    /*
     * Auto search-actions; action that will be performed if only one record
     * is found.
     * @access private
     * @var array
     */
    public $m_search_action;

    /*
     * Priority actions
     * @access private
     * @todo This, and the priority_min/max members, should be moved
     *       to the recordlist
     * @var array
     */
    public $m_priority_actions = [];

    /*
     * Minimum for the mra priority select
     * @access private
     * @var int
     */
    public $m_priority_min = 1;

    /*
     * Maximum for the mra priority select
     * @access private
     * @var int
     */
    public $m_priority_max = 0;

    /*
     * List of actions that should give success/failure feedback
     * @access private
     * @var array
     */
    public $m_feedback = [];

    /*
     * Number to use with numbering
     * @access protected
     * @var mixed
     */
    public $m_numbering = null;

    /*
     * Descriptor template.
     * @access protected
     * @var String
     */
    public $m_descTemplate = null;

    /*
     * Descriptor handler.
     * @access protected
     * @var Object
     */
    public $m_descHandler = null;

    /*
     * List of action listeners
     * @access protected
     * @var array
     */
    public $m_actionListeners = [];

    /*
     * List of trigger listeners
     * @access protected
     * @var array
     */
    public $m_triggerListeners = [];

    /**
     * List of callback functions to manipulate the record actions.
     *
     * @var array
     */
    protected $m_recordActionsCallbacks = [];

    /**
     * List of callback functions to add css class to row.
     * See details in DGList::getRecordlistData() method.
     *
     * @var array
     */
    protected $m_rowClassCallback = [];

    /*
     * Extended search action. The action which is called if the user
     * wants to perform an extended search.
     *
     * @access private
     * @var String
     */
    public $m_extended_search_action = null;

    /*
     * List of editable list attributes.
     * @access private
     * @var array
     */
    public $m_editableListAttributes = [];

    /*
     * Multi-record actions, selection mode.
     * @access private
     * @var int
     */
    public $m_mraSelectionMode = self::MRA_MULTI_SELECT;

    /*
     * The default edit fieldprefix to use for atk
     * @access private
     * @var String
     */
    public $m_edit_fieldprefix = '';

    /**
     * Default column name (null means across all columns).
     *
     * @var string
     */
    private $m_defaultColumn = null;

    /**
     * Current maximum attribute order value.
     *
     * @var int
     */
    private $m_attribOrder = 0;

    /*
     * parent Attribute flag (treeview)
     */
    public $m_parent;


    public $m_cacheidentifiers;

    private $recordListHover = true;
    private $rowColorConditions = [];

    private $rowColorMode = self::ROW_COLOR_MODE_DEFAULT;

    /**
     * @var array - List of nested attributes stored in the $nestedAttributeField
     */
    private $nestedAttributesList = [];

    /**
     * @var array[] - List of legend item ['text', 'color']
     */
    private $legendItems = [];

    /**
     * @var array[] - List of button filters of admin header ['label', 'values', 'noFilterLabel']
     */
    private $adminHeaderButtonFilters = [];

    /**
     * @var array[] - List of input filters of admin header
     */
    private $adminHeaderInputFilters = [];

    private $hidePageTitle = false;

    private $adminPageBookmarkLink = null;

    private $adminPageNodeHelp = null;

    /** @var int $recordListDropdownStartIndex */
    private $recordListDropdownStartIndex = null;

    /**
     * @param string $nodeUri The nodeuri
     * @param int $flags Bitmask of node flags (self::NF_*).
     */
    public function __construct($nodeUri, $flags = 0)
    {
        list($this->m_module, $this->m_type) = explode('.', $nodeUri);
        $this->m_flags = $flags;

        $this->setEditFieldPrefix(Config::getGlobal('edit_fieldprefix', ''));

        $this->addStateColorAttribute();
        $this->setRowColorMode(self::ROW_COLOR_MODE_DEFAULT);
    }

    /**
     * Create a state color attribute so the client can create set the row colors as
     * a 'row type', where all the row gets colored or show them as a column type, where
     * the row color will be shown automatically in a new column with a specified shape;
     */
    private function addStateColorAttribute()
    {

        $rowColorModeAttribute = new StateColorAttribute(self::ROW_COLOR_ATTRIBUTE);
        $rowColorModeAttribute->setShape(StateColorAttribute::SHAPE_ROUND);
        $rowColorModeAttribute->setSize(StateColorAttribute::SIZE_LG);

        $rowColorModeAttribute->setViewCallback(function ($record, $mode, $attribute) {
            $attribute->setColor($this->recordStateColor($record) ?: self::DEFAULT_RECORDLIST_BG_COLOR);

            $display = $attribute->display($record, $mode);
            if ($mode === 'list') {
                $display = "<div class='text-center w-100'>" . $display . "</div>";
            }

            return $display;

        });

        $this->add($rowColorModeAttribute);
    }


    /**
     * Resolve section. If a section is only prefixed by
     * a dot this means we need to add the default tab
     * before the dot.
     *
     * @param string $section section name
     *
     * @return string resolved section name
     */
    public function resolveSection(string $section): string
    {
        list($part1, $part2) = (strpos($section, '.') !== false) ? explode('.', $section) : array($section, '');
        if ($part2 != null && strlen($part2) > 0 && strlen($part1) == 0) {
            return $this->m_default_tab . '.' . $part2;
        } else {
            if (strlen($part2) == 0 && strlen($part1) == 0) {
                return $this->m_default_tab;
            } else {
                return $section;
            }
        }
    }

    /**
     * Resolve sections.
     *
     * @param array $sections section list
     *
     * @return array resolved section list
     *
     * @see resolveSection
     */
    public function resolveSections(array $sections): array
    {
        $result = [];

        foreach ($sections as $section) {
            $result[] = $this->resolveSection($section);
        }

        return $result;
    }

    /**
     * Returns the default column name.
     *
     * @return string default column name
     */
    public function getDefaultColumn()
    {
        return $this->m_defaultColumn;
    }

    /**
     * Set default column name.
     *
     * @param string $name default column name
     */
    public function setDefaultColumn($name)
    {
        $this->m_defaultColumn = $name;
    }

    /**
     * Resolve column for sections.
     *
     * If one of the sections contains something after a double
     * colon (:) than that's used as column name, else the default
     * column name will be used.
     *
     * @param array $sections sections
     *
     * @return string column name
     */
    protected function resolveColumn(&$sections)
    {
        $column = $this->getDefaultColumn();

        if (!is_array($sections)) {
            return $column;
        }

        foreach ($sections as &$section) {
            if (strpos($section, ':') !== false) {
                list($section, $column) = explode(':', $section);
            }
        }

        return $column;
    }

    /**
     * Resolve sections, tabs and the order based on the given
     * argument to the attribute add method.
     *
     * @param mixed $sections
     * @param mixed $tabs
     * @param mixed $order
     */
    public function resolveSectionsTabsOrder(&$sections, &$tabs, &$column, &$order)
    {
        // Because sections/tabs will probably be used more than the order override option
        // the API for this method now favours the $sections argument. For backwards
        // compatibility we still support the old API ($attribute,$order=0).
        if ($sections !== null && is_int($sections)) {
            $order = $sections;
            $sections = array($this->m_default_tab);
        }

        // If no section/tab is specified or tabs are disabled, we use the current default tab
        // (specified with the setDefaultTab method, or "default" otherwise)
        elseif ($sections === null || (is_string($sections) && strlen($sections) == 0) || !Config::getGlobal('tabs')) {
            $sections = array($this->m_default_tab);
        } // Sections should be an array.
        else {
            if ($sections != '*' && !is_array($sections)) {
                $sections = array($sections);
            }
        }

        $column = $this->resolveColumn($sections);

        if (is_array($sections)) {
            $sections = $this->resolveSections($sections);
        }

        // Filter tabs from section names.
        $tabs = $this->getTabsFromSections($sections);
    }

    /**
     * Add an Attribute (or one of its derivatives) to the node.
     *
     * @param Attribute $attribute The attribute you want to add
     * @param string|array|null $sections The tab(s) on which the attribute should be
     *                             displayed. Can be a tabname (String) or a list of
     *                             tabs (array) or "*" if the attribute should be
     *                             displayed on all tabs.
     * @param int $order The order at which the attribute should be displayed.
     *                             If ommitted, this defaults to 100 for the first
     *                             attribute, and 100 more for each next attribute that
     *                             is added.
     *
     * @return Attribute the attribute just added
     */
    public function add(Attribute $attribute, string|array $sections = null, int $order = 0): Attribute
    {
        // if $attribute is a nested attribute create a fake one and handle the loading/storage through the JsonAttribute
        if ($attribute->isNestedAttribute()) {
            $nestedAttributeFieldName = $attribute->getNestedAttributeField();
            $nestedAttributeField = $this->getAttribute($nestedAttributeFieldName);
            // the first time I add a nested attribute, the related JsonAttribute is also added
            if (!$nestedAttributeField) {
                $nestedAttributeField = $this->add(new JSONAttribute($nestedAttributeFieldName, Attribute::AF_HIDE | Attribute::AF_FORCE_LOAD));
            }
            $nestedAttributeField->setForceUpdate(true);

            // Set the attribute as not stored/loaded on/from the database (the JsonAttribute will handle the storage)
            $attribute->setStorageType(Attribute::NOSTORE)->setLoadType(Attribute::NOLOAD);
            $this->addNestedAttribute($attribute->fieldName(), $nestedAttributeFieldName);
        }

        $tabs = null;
        $column = null;

        $attribute->m_owner = $this->m_type;

        if (!$this->atkReadOptimizer()) {
            $this->resolveSectionsTabsOrder($sections, $tabs, $column, $order);

            // check for parent fieldname (treeview)
            if ($attribute->hasFlag($attribute::AF_PARENT)) {
                $this->m_parent = $attribute->fieldName();
            }

            // check for cascading delete flag
            if ($attribute->hasFlag($attribute::AF_CASCADE_DELETE)) {
                $this->m_cascadingAttribs[] = $attribute->fieldName();
            }

            if ($attribute->hasFlag($attribute::AF_HIDE_LIST) && !$attribute->hasFlag($attribute::AF_PRIMARY)) {
                if (!in_array($attribute->fieldName(), $this->m_listExcludes)) {
                    $this->m_listExcludes[] = $attribute->fieldName();
                }
            }

            if ($attribute->hasFlag($attribute::AF_HIDE_VIEW) && !$attribute->hasFlag($attribute::AF_PRIMARY)) {
                if (!in_array($attribute->fieldName(), $this->m_viewExcludes)) {
                    $this->m_viewExcludes[] = $attribute->fieldName();
                }
            }

        } else {
            // when the read optimizer is enabled there is no active tab
            // we circument this by putting all attributes on all tabs
            if ($sections !== null && is_int($sections)) {
                $order = $sections;
            }
            $tabs = '*';
            $sections = '*';
            $column = $this->getDefaultColumn();
        }

        // NOTE: THIS SHOULD WORK. BUT, since add() is called from inside the $this
        // constructor, m_ownerInstance ends up being a copy of $this, rather than
        // a reference. Don't ask me why, it has something to do with the way PHP
        // handles the constructor.
        // To work around this, we reassign the pointer to the attributes as
        // soon as possible AFTER the constructor. (the dispatcher function)
        $attribute->setOwnerInstance($this);

        if (method_exists($attribute, 'getType') && $attribute->getType() == ButtonAttribute::TYPE_SUBMIT) {
            $this->addSubmitBtnAttrib($attribute->m_name);
        }

        if ($attribute->hasFlag(Attribute::AF_PRIMARY)) {
            if (!in_array($attribute->fieldName(), $this->m_primaryKey)) {
                $this->m_primaryKey[] = $attribute->fieldName();
            }
        }

        $attribute->init();

        $exist = false;
        if (isset($this->m_attribList[$attribute->fieldName()]) && is_object($this->m_attribList[$attribute->fieldName()])) {
            $exist = true;
            // if order is set, overwrite it with new order, last order will count
            if ($order != 0) {
                $this->m_attribIndexList[$this->m_attribList[$attribute->fieldName()]->m_index]['order'] = $order;
            }
            $attribute->m_index = $this->m_attribList[$attribute->fieldName()]->m_index;
        }

        if (!$exist) {
            if ($order == 0) {
                $this->m_attribOrder += 100;
                $order = $this->m_attribOrder;
            }

            if (!$this->atkReadOptimizer()) {
                // add new tab(s) to the tab list ("*" isn't a tab!)
                if ($tabs != '*') {
                    if (!$attribute->hasFlag(Attribute::AF_HIDE_ADD)) {
                        $this->m_tabList['add'] = isset($this->m_tabList['add']) ? Tools::atk_array_merge($this->m_tabList['add'], $tabs) : $tabs;
                    }
                    if (!$attribute->hasFlag(Attribute::AF_HIDE_EDIT)) {
                        $this->m_tabList['edit'] = isset($this->m_tabList['edit']) ? Tools::atk_array_merge($this->m_tabList['edit'], $tabs) : $tabs;
                    }
                    if (!$attribute->hasFlag(Attribute::AF_HIDE_VIEW)) {
                        $this->m_tabList['view'] = isset($this->m_tabList['view']) ? Tools::atk_array_merge($this->m_tabList['view'], $tabs) : $tabs;
                    }
                }

                if ($sections != '*') {
                    if (!$attribute->hasFlag(Attribute::AF_HIDE_ADD)) {
                        $this->m_sectionList['add'] = isset($this->m_sectionList['add']) ? Tools::atk_array_merge($this->m_sectionList['add'],
                            $sections) : $sections;
                    }
                    if (!$attribute->hasFlag(Attribute::AF_HIDE_EDIT)) {
                        $this->m_sectionList['edit'] = isset($this->m_sectionList['edit']) ? Tools::atk_array_merge($this->m_sectionList['edit'],
                            $sections) : $sections;
                    }
                    if (!$attribute->hasFlag(Attribute::AF_HIDE_VIEW)) {
                        $this->m_sectionList['view'] = isset($this->m_sectionList['view']) ? Tools::atk_array_merge($this->m_sectionList['view'],
                            $sections) : $sections;
                    }
                }
            }

            $attribute->m_order = $order;
            $this->m_attribIndexList[] = array(
                'name' => $attribute->fieldName(),
                'tabs' => $tabs,
                'sections' => $sections,
                'order' => $attribute->m_order,
            );
            $attribute->m_index = max(array_keys($this->m_attribIndexList)); // might contain gaps
            $attribute->setTabs((array)$tabs);
            $attribute->setSections($sections);
            $this->m_attributeTabs[$attribute->fieldName()] = $tabs;
        }

        // Order the tablist
        $this->m_attribList[$attribute->fieldName()] = $attribute;
        $attribute->setTabs($this->m_attributeTabs[$attribute->fieldName()]);
        $attribute->setSections($this->m_attribIndexList[$attribute->m_index]['sections']);
        $attribute->setColumn($column);

        return $attribute;
    }

    /**
     * Add fieldset.
     *
     * To include an attribute label use [attribute.label] inside your
     * template. To include an attribute edit/display field use
     * [attribute.field] inside your template.
     *
     * @param string $name name
     * @param string $template template string
     * @param int $flags attribute flags
     * @param mixed|null $sections The sections/tab(s) on which the attribute should be
     *                         displayed. Can be a tabname (String) or a list of
     *                         tabs (array) or "*" if the attribute should be
     *                         displayed on all tabs.
     * @param int $order The order at which the attribute should be displayed.
     *                         If ommitted, this defaults to 100 for the first
     *                         attribute, and 100 more for each next attribute that
     *                         is added.
     */
    public function addFieldSet(string $name, string $template, int $flags = 0, mixed $sections = null, int $order = 0): void
    {
        $this->add(new FieldSet($name, $template, $flags), $sections, $order);
    }

    /**
     * Retrieve the tabnames from the sections string (tab.section).
     *
     * @param mixed $sections An array with sections or a section string
     * @return array|null
     */
    public function getTabsFromSections(mixed $sections): ?array
    {
        if ($sections == '*' || $sections === null) {
            return $sections;
        }

        $tabs = [];

        if (!is_array($sections)) {
            $sections = [$sections];
        }

        foreach ($sections as $section) {
            $tabs[] = $this->getTabFromSection($section);
        }

        // when using the tab.sections notation, we can have duplicate tabs strip them out.
        return array_unique($tabs);
    }

    /**
     * Strip section part from a section and return the tab.
     *
     * If no tab name is provided, the default tab is returned.
     *
     * @param string $section The section to get the tab from
     * @return mixed|string
     */
    public function getTabFromSection(string $section): mixed
    {
        $tab = ($section == null) ? '' : $section;

        if (strstr($tab, '.') !== false) {
            list($tab) = explode('.', $tab);
        }

        return ($tab == '') ? $this->m_default_tab : $tab;
    }

    /**
     * Remove an attribute.
     *
     * Completely removes an attribute from a node.
     * Note: Since other functionality may already depend on the attribute
     * that you are about to remove, it's often better to just hide an
     * attribute if you don't need it.
     *
     * @param string $attribname The name of the attribute to remove.
     * @return void
     */
    public function remove(string $attribname): void
    {
        if (is_object($this->m_attribList[$attribname])) {
            Tools::atkdebug("removing attribute $attribname");

            $listindex = $this->m_attribList[$attribname]->m_index;

            unset($this->m_attribList[$attribname]);
            foreach ($this->m_listExcludes as $i => $name) {
                if ($name == $attribname) {
                    unset($this->m_listExcludes[$i]);
                }
            }
            foreach ($this->m_viewExcludes as $i => $name) {
                if ($name == $attribname) {
                    unset($this->m_viewExcludes[$i]);
                }
            }
            foreach ($this->m_cascadingAttribs as $i => $name) {
                if ($name == $attribname) {
                    unset($this->m_cascadingAttribs[$i]);
                    $this->m_cascadingAttribs = array_values($this->m_cascadingAttribs);
                }
            }

            unset($this->m_attribIndexList[$listindex]);
            unset($this->m_attributeTabs[$attribname]);
        }
    }

    /**
     * Returns the table name for this node.
     *
     * @return string table name
     */
    public function getTable(): string
    {
        return $this->m_table;
    }

    /**
     * Get an attribute by name.
     *
     * @param string $attributeName The name of the attribute to retrieve.
     * @return Relation|Attribute|null The attribute.
     */
    public function getAttribute(string $attributeName): Relation|Attribute|null
    {
        return $this->m_attribList[$attributeName] ?? null;
    }

    /**
     * Check if the node has an attribute with the passed name.
     *
     * @param string $attributeName
     * @return bool
     */
    public function hasAttribute(string $attributeName): bool
    {
        return $this->getAttribute($attributeName) !== null;
    }

    /**
     * @param array $record
     * @param string $attributeName
     * @return mixed
     */
    public function getAttributeValue(array $record, string $attributeName): mixed
    {
        $value = null;
        $attr = $this->getAttribute($attributeName);
        if (isset($record[$attributeName])) {
            $value = $record[$attributeName];
        } elseif ($attr->isNestedAttribute()) {
            $nestedAttributeFieldName = $attr->getNestedAttributeField();
            $nestedAttribute = $record[$nestedAttributeFieldName];
            if (is_string($nestedAttribute)) {
                $nestedAttribute = json_decode($nestedAttribute, true);
            }
            $value = $nestedAttribute[$attributeName];
        }

        return $value;
    }

    /**
     * @param array $record
     * @param string $attributeName
     * @return mixed
     */
    public function getAttributeOldValue(array $record, string $attributeName): mixed
    {
        $attr = $this->getAttribute($attributeName);
        $oldValue = $record[self::ATK_ORG_REC][$attributeName];

        if (!isset($oldValue) && $attr->isNestedAttribute()) {
            $nestedAttributeFieldName = $attr->getNestedAttributeField();
            $nestedAttributeOld = $record[self::ATK_ORG_REC][$nestedAttributeFieldName];
            if (is_string($nestedAttributeOld)) {
                $nestedAttributeOld = json_decode($nestedAttributeOld, true);
            }
            $oldValue = $nestedAttributeOld[$attributeName];
        }

        return $oldValue;
    }

    /**
     * Checks if the user has filled in something:
     * return true if he has, otherwise return false.
     *
     * @param  -
     *
     * @return boolean.
     */
    public function filledInForm(): bool
    {
        if (is_null($this->getAttributes())) {
            return false;
        }

        $postvars = Tools::atkGetPostVar();
        foreach ($this->m_attribList as $name => $value) {
            if (!$value->hasFlag(Attribute::AF_HIDE_LIST)) {
                if (!is_array($value->fetchValue($postvars)) && $value->fetchValue($postvars) !== '') {
                    return true;
                }
            }
        }

        return false;
    }

    /**
     * Gets all the attributes.
     *
     * @return array|null Array with the attributes.
     */
    public function &getAttributes(): ?array
    {
        $result = $this->m_attribList ?? null;
        return $result;
    }

    /**
     * Returns a list of attribute names.
     *
     * @return array attribute names
     */
    public function getAttributeNames(): array
    {
        return array_keys($this->m_attribList);
    }

    /**
     * Gets the attribute order.
     *
     * @param string $name The name of the attribute
     */
    public function getAttributeOrder(string $name)
    {
        return $this->m_attribIndexList[$this->m_attribList[$name]->m_index]['order'];
    }

    /**
     * Sets an attributes order.
     *
     * @param string $name The name of the attribute
     * @param int $order The order of the attribute
     */
    public function setAttributeOrder(string $name, int $order): void
    {
        $this->m_attribList[$name]->m_order = $order;
        $this->m_attribIndexList[$this->m_attribList[$name]->m_index]['order'] = $order;
    }

    /**
     * Checks if the node has a certain flag set.
     *
     * @param int $flag The flag to check.
     * @return bool True if the node has the flag.
     */
    public function hasFlag(int $flag): bool
    {
        return ($this->m_flags & $flag) == $flag;
    }

    /**
     * Add a flag to the node.
     *
     * @param int $flag The flag to add.
     */
    public function addFlag(int $flag): self
    {
        $this->m_flags |= $flag;
        return $this;
    }

    /**
     * Removes a flag from the node.
     *
     * @param int $flag The flag to remove
     */
    public function removeFlag(int $flag): self
    {
        if ($this->hasFlag($flag)) {
            $this->m_flags ^= $flag;
        }
        return $this;
    }

    /**
     * Returns the node flags.
     *
     * @return int node flags
     */
    public function getFlags(): int
    {
        return $this->m_flags;
    }

    /**
     * Set node flags.
     *
     * @param int $flags node flags
     * @return $this
     */
    public function setFlags(int $flags): self
    {
        $this->m_flags = $flags;
        return $this;
    }

    /**
     * Returns the current partial name.
     *
     * @return string|null partial name
     */
    public function getPartial(): ?string
    {
        return $this->m_partial;
    }

    /**
     * Is partial request?
     *
     * @return bool|null is partial
     */
    public function isPartial(): ?bool
    {
        return $this->m_partial;
    }

    /**
     * Sets the editable list attributes. If you supply this method
     * with one or more string arguments, all arguments are collected in
     * an array. Else the first parameter will be used.
     *
     * @param array|null $attrs list of attribute names
     * @return void
     */
    public function setEditableListAttributes(?array $attrs): void
    {
        if (is_array($attrs)) {
            $this->m_editableListAttributes = $attrs;
        } else {
            $this->m_editableListAttributes = func_get_args();
        }
    }

    /**
     * Sets the multi-record-action selection mode. Can either be
     * Node::MRA_MULTI_SELECT (default), Node::MRA_SINGLE_SELECT or
     * Node::MRA_NO_SELECT.
     *
     * @param string $mode selection mode
     * @return $this
     */
    public function setMRASelectionMode(string $mode): self
    {
        $this->m_mraSelectionMode = $mode;
        return $this;
    }

    /**
     * Returns the multi-record-action selection mode.
     *
     * @return int multi-record-action selection mode
     */
    public function getMRASelectionMode(): int
    {
        return $this->m_mraSelectionMode;
    }

    /**
     * Returns the primary key sql expression of a record.
     *
     * @param array|string $record The record or the primary key value for which the primary key is calculated.
     * @return string the primary key of the record.
     */
    public function primaryKey(array|string $record): string
    {
        $primKey = '';

        if ($record) {
            $nrOfElements = Tools::count($this->m_primaryKey);
            for ($i = 0; $i < $nrOfElements; ++$i) {
                $p_attrib = $this->m_attribList[$this->m_primaryKey[$i]];
                $primKeyValue = is_array($record) ? $p_attrib->value2db($record) : $record;
                $primKey .= $this->m_table . '.' . $this->m_primaryKey[$i] . "='$primKeyValue'";
                if ($i < ($nrOfElements - 1)) {
                    $primKey .= ' AND ';
                }
            }
        }

        return $primKey;
    }

    /**
     * Retrieve the name of the primary key attribute.
     *
     * Note: If a node has a primary key that consists of multiple attributes,
     * this method will retrieve only the first attribute!
     *
     * @return string|null First primary key attribute
     */
    public function primaryKeyField(): ?string
    {
        if (Tools::count($this->m_primaryKey) === 0) {
            Tools::atkwarning($this->atkNodeUri() . '::primaryKeyField() called, but there are no primary key fields defined!');
            return null;
        }

        return $this->m_primaryKey[0];
    }

    /**
     * Returns a primary key template.
     *
     * Like primaryKey(), this method returns a sql expression, but in this
     * case, no actual data is used. Instead, template fields are inserted
     * into the expression. This is useful for rendering multiple primary
     * keys later with a record and a template parser.
     *
     * @return string Primary key template
     */
    public function primaryKeyTpl(): string
    {
        $primKey = '';
        $nrOfElements = Tools::count($this->m_primaryKey);
        for ($i = 0; $i < $nrOfElements; ++$i) {
            $primKey .= $this->m_primaryKey[$i] . "='[" . $this->m_primaryKey[$i] . "]'";
            if ($i < ($nrOfElements - 1)) {
                $primKey .= ' AND ';
            }
        }

        return $primKey;
    }

    public function getAtkError(array $record): string
    {
        $errors = [];
        if (isset($record['atkerror']) && count($record['atkerror']) > 0) {
            foreach ($record['atkerror'] as $error) {
                if (is_array($error["attrib_name"])) {
                    $error["attrib_name"] = implode(', ', $error["attrib_name"]);
                }
                $errors[] = $error["attrib_name"] . ": " . $error['msg'];
            }
        }
        return implode("\n", $errors);
    }

    /**
     * Returns the primary key selector of the record.
     *
     * @param array $record
     * @return string
     */
    public function getPrimaryKey(array $record): string
    {
        if (isset($record['atkprimkey']) && $atkPrimKey = $record['atkprimkey']) {
            return $atkPrimKey;
        }
        if ($atkPrimKey = $this->primaryKey($record)) {
            return $atkPrimKey;
        }
        return $this->getTable() . "." . $this->primaryKeyField() . "='" . $record[$this->primaryKeyField()] . "'";
    }

    /**
     * Set default sort order for the node.
     *
     * @param string $orderby Default order by. Can be an attribute name or a SQL expression.
     */
    public function setOrder(string $orderby): void
    {
        $this->m_default_order = $orderby;
    }

    /**
     * Get default sort order for the node.
     *
     * @return string Default order by. Can be an attribute name or a SQL expression.
     */
    public function getOrder(): string
    {
        return str_replace('[table]', $this->getTable(), $this->m_default_order);
    }

    /**
     * Set the table that the node should use.
     *
     * Note: This should be called in the constructor of derived classes,
     * after the base class constructor is called.
     *
     * @param string $tablename The name of the table to use.
     * @param string $seq The name of the sequence to use for autoincrement
     *                          attributes.
     * @param mixed|null $db The database connection to use. If ommitted, this
     *                          defaults to the default database connection.
     *                          So in apps using only one database, it's not necessary
     *                          to pass this parameter.
     *                          You can pass either a connection (Db instance), or
     *                          a string containing the name of the connection to use.
     */
    public function setTable(string $tablename, string $seq = '', mixed $db = null): void
    {
        $this->m_table = $tablename;
        if ($seq == '') {
            $seq = $tablename;
        }
        $this->m_seq = $seq;
        $this->m_db = $db;
    }

    /**
     * Sets the database connection.
     *
     * @param string|Db $db database name or object
     */
    public function setDb($db): void
    {
        $this->m_db = $db;
    }

    /**
     * Get the database connection for this node.
     *
     * @return Db|null Database connection instance
     */
    public function getDb(): ?Db
    {
        if ($this->m_db == null) {
            return Db::getInstance();
        } else {
            if (is_object($this->m_db)) {
                return $this->m_db;
            } else {
                // must be a named connection
                return Db::getInstance($this->m_db);
            }
        }
    }

    /**
     * Create an alphabetical index.
     *
     * Any string- or textbased attribute can be used to create an
     * alphabetical index in admin- and selectpages.
     *
     * @param string $attribname The name of the attribute for which to create
     *                           the alphabetical index.
     * @return void
     */
    public function setIndex(string $attribname): void
    {
        $this->m_index = $attribname;
    }

    /**
     * Set tab index.
     *
     * @param string $tabname Tabname
     * @param int $index Index number
     * @param string $action Action name (add,edit,view)
     */
    public function setTabIndex(string $tabname, int $index, string $action = ''): void
    {
        Tools::atkdebug("self::setTabIndex($tabname,$index,$action)");
        $actionList = ['add', 'edit', 'view'];
        if ($action != '') {
            $actionList = [$action];
        }
        foreach ($actionList as $action) {
            $new_index = $index;
            $list = &$this->m_tabList[$action];
            if ($new_index < 0) {
                $new_index = 0;
            }
            if ($new_index > Tools::count($list)) {
                $new_index = Tools::count($list);
            }
            $current_index = array_search($tabname, $list);
            if ($current_index !== false) {
                $tmp = array_splice($list, $current_index, 1);
                array_splice($list, $new_index, 0, $tmp);
            }
        }
    }

    /**
     * Set default tab being displayed in view/add/edit mode.
     * After calling this method, all attributes which are added after the
     * method call without specification of tab will be placed on the default
     * tab. This means you should use this method before you add any
     * attributes to the node.
     * If you accept the default name for the first tab ("default") you do not
     * need to call this method.
     *
     * @param string $tab the name of the default tab
     */
    public function setDefaultTab(string $tab = 'default'): void
    {
        $this->m_default_tab = $tab;
    }

    /**
     * Get a list of tabs for a certain action.
     *
     * @param string $action The action for which you want to retrieve the
     *                       list of tabs.
     *
     * @return array The list of tabnames.
     */
    public function getTabs(string $action): array
    {
        $list = $this->m_tabList[$action] ?? null;
        $disable = $this->checkTabRights($list);

        if (!is_array($list)) {
            // fallback to view tabs.
            $list = $this->m_tabList['view'];
        }

        // Attributes can also add tabs to the tablist.
        $this->m_filledTabs = [];
        foreach (array_keys($this->m_attribList) as $attribname) {
            $p_attrib = &$this->m_attribList[$attribname];
            if ($p_attrib->hasFlag(Attribute::AF_HIDE)) {
                continue;
            } // attributes to which we don't have access are explicitly hidden

            // Only display the attribute if the attribute
            // resides on at least on visible tab
            for ($i = 0, $_i = sizeof($p_attrib->m_tabs); $i < $_i; ++$i) {
                if ((is_array($list) && in_array($p_attrib->m_tabs[$i], $list)) || (!is_array($disable) || !in_array($p_attrib->m_tabs[$i], $disable))) {
                    break;
                }
            }

            if (is_object($p_attrib)) {
                $additional = $p_attrib->getAdditionalTabs(null);
                if (is_array($additional) && Tools::count($additional) > 0) {
                    $list = Tools::atk_array_merge($list, $additional);
                    $this->m_filledTabs = Tools::atk_array_merge($this->m_filledTabs, $additional);
                }

                // Keep track of the tabs that containg attribs
                // so we only display none-empty tabs
                $tabCode = $this->m_attributeTabs[$attribname][0];
                if (!in_array($tabCode, $this->m_filledTabs)) {
                    $this->m_filledTabs[] = $tabCode;
                }
            } else {
                Tools::atkdebug("node::getTabs() Warning: $attribname is not an object!?");
            }
        }

        // Check if the currently known tabs all containg attributes
        // so we don't end up with empty tabs
        return $this->checkEmptyTabs($list);
    }

    /**
     * Retrieve the sections for the active tab.
     *
     * @param string $action
     * @return array The active sections.
     */
    public function getSections(string $action): array
    {
        $sections = [];

        if (is_array($this->m_sectionList[$action])) {
            foreach ($this->m_sectionList[$action] as $element) {
                list($tab, $sec) = (str_contains($element, '.')) ? explode('.', $element) : [$element, null];

                //if this section is on an active tab, we return it.
                if ($tab == $this->getActiveTab() && $sec !== null) {
                    $sections[] = $sec;
                }
            }
        }

        //we do not want duplicate sections on the same tab.
        return array_unique($sections);
    }

    /**
     * Add sections that must be expanded by default.
     */
    public function addDefaultExpandedSections(): void
    {
        $sections = func_get_args();
        $sections = $this->resolveSections($sections);
        $this->m_default_expanded_sections = array_unique(array_merge($sections, $this->m_default_expanded_sections));
    }

    /**
     * Remove sections that must be expanded by default.
     */
    public function removeDefaultExpandedSections(): void
    {
        $sections = func_get_args();

        $this->m_default_expanded_sections = array_diff($this->m_default_expanded_sections, $sections);
    }

    /**
     * Check if the user has the rights to access existing tabs and
     * removes tabs from the list that may not be accessed.
     *
     * @param array|null $tablist Array containing the current tablist
     * @return array with disable tabs
     */
    public function checkTabRights(?array &$tablist): array
    {
        $atk = Atk::getInstance();
        $disable = [];

        if (empty($this->m_module)) {
            return $disable;
        }

        for ($i = 0, $_i = Tools::count($tablist); $i < $_i; ++$i) {
            if ($tablist[$i] == '' || $tablist[$i] == 'default') {
                continue;
            }
            $secMgr = SecurityManager::getInstance();

            $priv = 'tab_' . $tablist[$i];
            if (isset($atk->g_nodes[$this->m_module][$this->m_type]) && Tools::atk_in_array($priv, $atk->g_nodes[$this->m_module][$this->m_type])) {
                // authorisation is required
                if (!$secMgr->allowed($this->m_module . '.' . $this->m_type, 'tab_' . $tablist[$i])) {
                    Tools::atkdebug('Removing TAB ' . $tablist[$i] . ' because access to this tab was denied');
                    $disable[] = $tablist[$i];
                    unset($tablist[$i]);
                }
            }
        }

        if (is_array($tablist)) {
            // we might have now something like:
            // [0]=>tabA,[3]=>tabD
            // we convert this to a 'normal' array:
            // [0]=>tabA,[1]=>tabD;
            $newarray = [];
            foreach ($tablist as $tab) {
                $newarray[] = $tab;
            }
            $tablist = $newarray;
        }

        return $disable;
    }

    /**
     * Remove tabs without attribs from the tablist.
     *
     * @param array|null $list The list of tabnames
     * @return array The list of tabnames without the empty tabs.
     */
    public function checkEmptyTabs(?array $list): array
    {
        $tabList = [];

        if (is_array($list)) {
            foreach ($list as $tabEntry) {
                if (in_array($tabEntry, $this->m_filledTabs)) {
                    $tabList[] = $tabEntry;
                } else {
                    Tools::atkdebug('Removing TAB ' . $tabEntry . ' because it had no attributes assigned');
                }
            }
        }

        return $tabList;
    }

    /**
     * Returns the currently active tab.
     *
     * Note that in themes which use dhtml tabs (tabs without reloads), this
     * method will always return the name of the first tab.
     *
     * @return string The name of the currently visible tab.
     */
    public function getActiveTab(): string
    {
        global $ATK_VARS;
        $tablist = $this->getTabs($ATK_VARS['atkaction']);

        // Note: we may not read atktab from $this->m_postvars, because $this->m_postvars is not filled if this is
        // a nested node (in a relation for example).
        if (!empty($ATK_VARS['atktab']) && in_array($ATK_VARS['atktab'], $tablist)) {
            $tab = $ATK_VARS['atktab'];
        } elseif (!empty($this->m_default_tab) && in_array($this->m_default_tab, $tablist)) {
            $tab = $this->m_default_tab;
        } else {
            $tab = $tablist[0];
        }

        return $tab;
    }

    /**
     * Get the active sections.
     *
     * @param string $tab The currently active tab
     * @param string $mode The current mode ("edit", "add", etc.)
     * @return array active Sections
     */
    public function getActiveSections(string $tab, string $mode): array
    {
        $activeSections = [];
        if (is_array($this->m_sectionList[$mode])) {
            foreach ($this->m_sectionList[$mode] as $section) {
                if (str_starts_with($section, $tab)) {
                    $sectionName = 'section_' . str_replace('.', '_', $section);
                    $key = [
                        'nodetype' => $this->atkNodeUri(),
                        'section' => $sectionName
                    ];
                    $defaultOpen = in_array($section, $this->m_default_expanded_sections);
                    if (State::get($key, $defaultOpen ? 'opened' : 'closed') != 'closed') {
                        $activeSections[] = $section;
                    }
                }
            }
        }

        return $activeSections;
    }

    /**
     * Add a recordset filter.
     *
     * @param string $filter The fieldname you want to filter OR a SQL where
     *                       clause expression.
     * @param string $value Required value. (Omit this parameter if you pass
     *                       an SQL expression for $filter.)
     */
    public function addFilter(string $filter, string $value = ''): void
    {
        if ($value == '') {
            // $key is a where clause kind of thing
            $this->m_fuzzyFilters[] = $filter;
        } else {
            // $key is a $key, $value is a value
            $this->m_filters[$filter] = $value;
        }
    }

    /**
     * Search and remove a recordset filter.
     *
     * @param string $filter The filter to search for
     * @param string $value The value to search for in case it is not a fuzzy filter
     * @return bool TRUE if the given filter was found and removed, FALSE otherwise.
     */
    public function removeFilter(string $filter, string $value = ''): bool
    {
        if ($value == '') {
            // fuzzy
            $key = array_search($filter, $this->m_fuzzyFilters);
            if (is_numeric($key)) {
                unset($this->m_fuzzyFilters[$key]);
                $this->m_fuzzyFilters = array_values($this->m_fuzzyFilters);

                return true;
            }

        } else {
            // not fuzzy
            foreach (array_keys($this->m_filters) as $key) {
                if ($filter == $key && $value == $this->m_filters[$key]) {
                    unset($this->m_filters[$key]);

                    return true;
                }
            }
        }

        return false;
    }

    /**
     * Returns the form buttons for a certain page.
     *
     * Can be overridden by derived classes to define custom buttons.
     *
     * @param string $mode The action for which the buttons are retrieved.
     * @param array|null $record The record currently displayed/edited in the form.
     *                       This param can be used to define record specific
     *                       buttons.
     * @return array
     */
    public function getFormButtons(string $mode, ?array $record = []): array
    {
        $result = [];
        $sm = SessionManager::getInstance();

        if ($mode == 'edit') {
            if ($sm->atkLevel() > 0 || Tools::hasFlag(Tools::atkArrayNvl($this->m_feedback, 'update', 0), ActionHandler::ACTION_SUCCESS)) {
                $result[] = $this->getButton('saveandclose', true);
            }

            $result[] = $this->getButton('save');

            // if atklevel is 0 or less, we are at the bottom of the session stack,
            // which means that 'saveandclose' doesn't close anyway, so we leave out
            // the 'saveandclose' and 'cancel' button. Unless, a feedback screen is configured.
            if ($sm->atkLevel() > 0 || Tools::hasFlag(Tools::atkArrayNvl($this->m_feedback, 'update', 0), ActionHandler::ACTION_CANCELLED)) {
                $result[] = $this->getButton('cancel');
            }

        } elseif ($mode == 'add') {
            if ($this->hasFlag(self::NF_EDITAFTERADD) === true) {
                if ($this->allowed('edit')) {
                    $result[] = $this->getButton('saveandedit', true);
                } else {
                    Tools::atkwarning("Node::NF_EDITAFTERADD found but no 'edit' privilege.");
                }
            } else {
                $result[] = $this->getButton('saveandclose', true);

                if ($this->hasFlag(self::NF_ADDAFTERADD)) {
                    $result[] = $this->getButton('saveandnext', false);
                }
            }

            if ($sm->atkLevel() > 0 || Tools::hasFlag(Tools::atkArrayNvl($this->m_feedback, 'save', 0), ActionHandler::ACTION_CANCELLED)) {
                $result[] = $this->getButton('cancel');
            }

        } elseif ($mode == 'view') {
            // if appropriate, display an edit button.
            if (!$this->hasFlag(self::NF_NO_EDIT) && $this->allowed('edit', $record)) {
                $result[] = '<input type="hidden" name="atkaction" value="edit">' . '<input type="hidden" name="atknodeuri" value="' . $this->atkNodeUri() . '">' . $this->getButton('edit');
            }

            if ($sm->atkLevel() > 0) {
                $result[] = $this->getButton('back', false, Tools::atktext('cancel'));
            }

        } elseif ($mode == 'delete') {
            $result[] = '<input name="confirm" type="submit" class="btn btn-primary btn_ok" value="' . $this->text('yes') . '">';
            $result[] = '<input name="cancel" type="submit" class="btn btn-default btn_cancel mr-1" value="' . $this->text('no') . '">';

        } elseif ($mode == 'search') {
            // (don't change the order of button)
            $result[] = $this->getButton('search', true);
            $result[] = $this->getButton('cancel');
        }

        return $result;
    }

    /**
     * Returns the form button with passed html name.
     */
    function getFormButton(string $name, string $mode, array $record = []): ?string
    {
        $buttons = self::getFormButtons($mode, $record);

        foreach ($buttons as $i => $button) {
            if (str_contains($button, "name=\"$name\"")) {
                return $button;
            }
        }

        return null;
    }

    /**
     * Remove the form button with passed html name.
     */
    function removeFormButton(array &$buttons, string $name): void
    {
        foreach ($buttons as $i => $button) {
            if (str_contains($button, "name=\"$name\"")) {
                unset($buttons[$i]);
                break;
            }
        }
    }

    /**
     * Find the form button in the passed array of buttons.
     */
    function findFormButton(array $buttons, string $name): bool
    {
        foreach ($buttons as $i => $button) {
            if (str_contains($button, "name=\"$name\"")) {
                return true;
            }
        }

        return false;
    }

    /**
     * Create a button.
     *
     * @param string $action
     * @param bool $default Add the atkdefaultbutton class?
     * @param string|null $label
     * @return string HTML
     */
    public function getButton(string $action, bool $default = false, string $label = null): string
    {
        $valueAttribute = '';

        switch ($action) {
            case 'save':
                $name = 'atknoclose';
                $class = 'btn_save';
                break;
            case 'saveandclose':
                $name = 'atksaveandclose';
                $class = 'btn_saveandclose';
                break;
            case 'cancel':
                $name = 'atkcancel';
                $class = 'btn_cancel';
                break;
            case 'saveandedit':
                $name = 'atksaveandcontinue';
                $class = 'btn_saveandcontinue';
                break;
            case 'saveandnext':
                $name = 'atksaveandnext';
                $class = 'btn_saveandnext';
                break;
            case 'back':
                $name = 'atkback';
                $class = 'btn_cancel';
                $value = '<< ' . Tools::atktext($action, 'atk');
                break;
            case 'edit':
                $name = 'atkedit';
                $class = 'btn_save';
                break;
            case 'search':
                $name = 'atkdosearch';
                $class = 'btn_search';
                break;
            default:
                $name = $action;
                $class = 'atkbutton';
        }

        if (!isset($value)) {
            $value = $this->text($action);
        }
        if (isset($label)) {
            $value = $label;
        }
        $value = htmlentities($value);

        $class = trim('btn ' . $class);

        if ($default) {
            $class .= (!empty($class) ? ' ' : '') . 'atkdefaultbutton btn-primary';
        } else {
            $class .= (!empty($class) ? ' ' : '') . 'btn-default';
        }

        if ($class != '') {
            $class = "class=\"$class\" ";
        }

        if ($value != '') {
            $valueAttribute = "value=\"{$value}\" ";
        }

        if ($name != '') {
            $name = 'name="' . $this->getEditFieldPrefix() . "{$name}\" ";
        }

        return '<button type="submit" ' . $class . $name . $valueAttribute . '>' . $value . '</button>';
    }

    /**
     * Get the ui instance for drawing and templating purposes.
     *
     * @return Ui An Ui instance for drawing and templating.
     */
    public function getUi(): Ui
    {
        return Ui::getInstance();
    }

    /**
     * Generate a title for a certain action on a certain action.
     *
     * The default implementation displayes the action name, and the
     * descriptor of the current record between brackets. This can be
     * overridden by derived classes.
     *
     * @param string $action The action for which the title is generated.
     * @param array $record The record for which the title is generated.
     * @return string The full title of the action.
     */
    public function actionTitle(string $action, array $record = []): string
    {
        $sm = SessionManager::getInstance();
        $ui = $this->getUi();
        $res = '';

        if ($record) {
            $descr = $this->descriptor($record);
            $sm->pageVar('descriptor', $descr);
        }

        // we want to show only the action title of the current record, not the previous in the stack (in the past was 3);
        // in the breadcrumb we now show the descriptor of the previous records as tooltip of hyperlinks (v. actionpage.tpl)
        $maxel = 1;

        $descriptortrace = $sm->descriptorTrace();
        $nomodule = false;
        if (!empty($descriptortrace)) {
            $nomodule = true;
            $descrtrace = '';
            // only show the last 3 elems
            $cnt = Tools::count($descriptortrace);
            if ($maxel > 1 && $cnt > $maxel) {
                $descrtrace = '... - ';
            }
            for ($i = max(0, $cnt - $maxel), $_i = $cnt; $i < $_i; ++$i) {
                $desc = $descriptortrace[$i];
                $descrtrace .= htmlentities($desc, ENT_COMPAT) . ' - ';
            }
            $res = $descrtrace . $res;
        }

        if (is_object($ui)) {
            $res .= $this->nodeTitle($action, $nomodule);
        }

        return $res;
    }

    /**
     * Place a set of tabs around content.
     *
     * @param string $action The action for which the tabs are loaded.
     * @param string $content The content that is to be displayed within the tabset.
     * @return string The complete tabset with content.
     */
    public function tabulate(string $action, string $content): string
    {
        $list = $this->getTabs($action);
        $sections = $this->getSections($action);
        $tabs = Tools::count($list);

        if (Tools::count($sections) > 0 || $tabs > 1) {
            $page = $this->getPage();
            $page->register_script(Config::getGlobal('assets_url') . 'javascript/tabs.js?stateful=' . (Config::getGlobal('dhtml_tabs_stateful') ? '1' : '0'));

            // Load default tab show script.
            $page->register_loadscript('if ( ATK.Tabs.showTab ) {ATK.Tabs.showTab(\'' . ($this->m_postvars['atktab'] ?? '') . '\');}');

            $fulltabs = $this->buildTabs($action);
            $tabscript = "var tabs = new Array();\n";
            foreach ($fulltabs as $tab) {
                $tabscript .= "tabs[tabs.length] = '" . $tab['tab'] . "';\n";
            }
            $page->register_scriptcode($tabscript);
        }

        if ($tabs > 1) {
            $ui = $this->getUi();
            if (is_object($ui)) {
                return $ui->renderTabs(array(
                    'tabs' => $this->buildTabs($action),
                    'content' => $content,
                ));
            }
        }

        return $content;
    }

    /**
     * Determine the default form parameters for an action template.
     *
     * @param bool $locked If the current record is locked, pass true, so
     *                     the lockicon can be placed in the params too.
     * @return array Default form parameters for action forms (assoc. array)
     */
    public function getDefaultActionParams(bool $locked = false): array
    {
        $params = [];
        $params['formend'] = '</form>';

        return $params;
    }

    /**
     * Check attribute security.
     *
     * Makes some attributes read-only, or hides the attribute based
     * on the current mode / record.
     *
     * @param string $mode current mode (add, edit, view etc.)
     * @param array|null $record current record (optional)
     */
    public function checkAttributeSecurity(string $mode, array $record = null): void
    {
        // check if an attribute needs to be read-only or
        // even hidden based on the current record
        $secMgr = SecurityManager::getInstance();
        foreach (array_keys($this->m_attribList) as $attrName) {
            $attr = $this->getAttribute($attrName);

            if (($mode == 'add' || $mode == 'edit') && !$secMgr->attribAllowed($attr, $mode, $record) && $secMgr->attribAllowed($attr, 'view', $record)) {
                $attr->addFlag(Attribute::AF_READONLY);
            } else {
                if (!$secMgr->attribAllowed($attr, $mode, $record)) {
                    $attr->addFlag(Attribute::AF_HIDE);
                }
            }
        }
    }

    /**
     * The preAddToEditArray method is called from within the editArray
     * method prior to letting the attributes add themselves to the edit
     * array, but after the edit record values have been collected (a
     * combination of the current record, initial/edit values and the forced
     * values). This makes it possible to do some last-minute modifications to
     * the record data and possibily add some last-minute attributes etc.
     *
     * @param array $record the edit record
     * @param string $mode edit mode (add or edit)
     */
    public function preAddToEditArray(array &$record, string $mode): void
    {
        // do nothing
    }

    /**
     * The preAddToViewArray method is called from within the viewArray
     * method prior to letting the attributes add themselves to the view
     * array, but after the view record values have been collected This makes
     * it possible to do some last-minute modifications to the record data
     * and possibily add some last-minute attributes etc.
     *
     * @param array $record the edit record
     * @param string $mode view mode
     */
    public function preAddToViewArray(array &$record, string $mode): void
    {
        // do nothing
    }

    /**
     * Function outputs an array with edit fields. For each field the array
     * contains the name, edit HTML code etc. (name, html, obligatory,
     * error, label).
     *
     * @param string $mode The edit mode ("add" or "edit")
     * @param array|null $record The record currently being edited.
     * @param array|string $forceList A key-value array used to preset certain
     *                               fields to a certain value, regardless of the
     *                               value in the record.
     * @param array|string $suppressList List of attributenames that you want to hide
     * @param string $fieldprefix Of set, each form element is prefixed with
     *                               the specified prefix (used in embedded form
     *                               fields)
     * @param bool $ignoreTab Ignore the tabs an attribute should be shown on.
     * @param bool $injectSections Inject sections?
     *
     * @return array List of edit fields (per field ( name, html, obligatory,
     *               error, label })
     * @throws Exception
     * TODO: The editArray method should use a set of classes to build the
     *       form, instead of an array with an overly complex structure.
     *
     */
    public function editArray(
        string       $mode = 'add',
        array        $record = null,
        array|string $forceList = '',
        array|string $suppressList = '',
        string       $fieldprefix = '',
        bool         $ignoreTab = false,
        bool         $injectSections = true
    ): array
    {
        // update visibility of some attributes based on the current record
        $this->checkAttributeSecurity($mode, $record);

        // read metadata
        $this->setAttribSizes();

        // default values
        if (!empty($record)) {
            $defaults = $record;
        } else {
            $defaults = [];
        }

        $result['hide'] = [];
        $result['fields'] = [];

        if ($mode == 'edit') {
            /* nodes can define edit_values */
            $overrides = $this->edit_values($defaults);
            foreach ($overrides as $varname => $value) {
                $defaults[$varname] = $value;
            }

        } else { // add mode
            // nodes can define initial values, if they don't already have values.
            if (!isset($defaults['atkerror'])) { // only load initial values the first time (not after an error occured)
                $overrides = $this->initial_values();

                if (is_array($overrides) && Tools::count($overrides) > 0) {
                    foreach ($overrides as $varname => $value) {
                        if (!isset($defaults[$varname]) || $defaults[$varname] == '') {
                            $defaults[$varname] = $value;
                        }
                    }
                }
            }
        }

        // check for forced values
        if (is_array($forceList)) {
            foreach ($forceList as $forcedvarname => $forcedvalue) {
                $attribname = '';

                if ($forcedvarname != '') {
                    if (strpos($forcedvarname, '.') > 0) {
                        list($firstpart, $field) = explode('.', $forcedvarname);
                        if ($firstpart == $this->m_table) {
                            // this is a filter on the current table.
                            $defaults[$field] = $forcedvalue;
                            $attribname = $field;
                        } else {
                            // this is a filter on a field of another table (something we have a
                            // relationship with.if(is_object($this->m_attribList[$table]))
                            if (is_object($this->m_attribList[$firstpart])) {
                                $defaults[$firstpart][$field] = $forcedvalue;
                                $attribname = $firstpart;
                            } else {
                                // This is not a filter for this node.
                            }
                        }

                    } else {
                        if (!isset($defaults[$forcedvarname]) || $defaults[$forcedvarname] == '') {
                            $defaults[$forcedvarname] = $forcedvalue;
                        }

                        $attribname = $forcedvarname;
                    }

                    if ($attribname != '') {
                        if (isset($this->m_attribList[$attribname])) {
                            $p_attrib = $this->m_attribList[$attribname];
                            if (is_object($p_attrib) && !$p_attrib->hasFlag($p_attrib::AF_NO_FILTER)) {
                                $p_attrib->m_flags |= $p_attrib::AF_READONLY | $p_attrib::AF_HIDE_ADD;
                            }
                        } else {
                            Tools::atkerror("Attribute '$attribname' doesn't exist in the attributelist");
                        }
                    }
                }
            }
        }

        // call preAddToEditArray at the attribute level, allows attribute to do
        // some last minute manipulations on for example the record
        foreach ($this->getAttributes() as $attr) {
            $attr->preAddToEditArray($defaults, $fieldprefix, $mode);
        }

        // call preAddToEditArray for the node itself.
        $this->preAddToEditArray($defaults, $mode);

        // initialize dependencies
        foreach ($this->getAttributes() as $attr) {
            $attr->initDependencies($defaults, $fieldprefix, $mode);
        }

        // extra submission data
        $result['hide'][] = '<input type="hidden" name="atkfieldprefix" value="' . $this->getEditFieldPrefix(false) . '">';
        $result['hide'][] = '<input type="hidden" name="' . $fieldprefix . 'atknodeuri" value="' . $this->atkNodeUri() . '">';
        $result['hide'][] = '<input type="hidden" name="' . $fieldprefix . 'atkprimkey" value="' . Tools::atkArrayNvl($record, 'atkprimkey', '') . '">';

        foreach (array_keys($this->m_attribIndexList) as $r) {
            $attribname = $this->m_attribIndexList[$r]['name'];
            $p_attrib = $this->m_attribList[$attribname];

            if ($p_attrib) {
                if ($p_attrib->hasDisabledMode($p_attrib::DISABLED_EDIT)) {
                    continue;
                }

                // fields that have not yet been initialised may be overriden in the url
                if (!array_key_exists($p_attrib->fieldName(), $defaults) && array_key_exists($p_attrib->fieldName(), $this->m_postvars)) {
                    $defaults[$p_attrib->fieldName()] = $this->m_postvars[$p_attrib->fieldName()];
                }

                if (is_array($suppressList) && Tools::count($suppressList) > 0 && in_array($attribname, $suppressList)) {
                    $p_attrib->m_flags |= ($mode == 'add' ? $p_attrib::AF_HIDE_ADD : $p_attrib::AF_HIDE_EDIT);
                }

                // we let the attribute add itself to the edit array
                $p_attrib->addToEditArray($mode, $result, $defaults, $record['atkerror'], $fieldprefix);
            } else {
                Tools::atkerror("Attribute $attribname not found!");
            }
        }

        if ($injectSections) {
            $this->injectSections($result['fields']);
        }

        // check for errors
        $result['error'] = $record['atkerror'];

        // return the result array
        return $result;
    }

    /**
     * Function outputs an array with view fields. For each field the array
     * contains the name, view HTML code etc.
     *
     * @param string $mode The edit mode ("view")
     * @param array $record The record currently being viewed.
     * @param bool $injectSections Inject sections?
     *
     * @return array List of edit fields (per field ( name, html, obligatory,
     *               error, label })
     * @throws Exception
     * @todo The viewArray method should use a set of classes to build the
     *       form, instead of an array with an overly complex structure.
     *
     */
    public function viewArray(string $mode, array $record, bool $injectSections = true): array
    {
        // update visibility of some attributes based on the current record
        $this->checkAttributeSecurity($mode, $record);

        // call preAddToViewArray at the attribute level, allows attribute to do
        // some last minute manipulations on for example the record
        foreach ($this->getAttributes() as $attr) {
            $attr->preAddToViewArray($record, $mode);
        }

        // call preAddToViewArray for the node itself.
        $this->preAddToViewArray($record, $mode);

        $result = [];

        foreach (array_keys($this->m_attribIndexList) as $r) {
            $attribname = $this->m_attribIndexList[$r]['name'];

            $p_attrib = $this->m_attribList[$attribname];
            if ($p_attrib != null) {
                if ($p_attrib->hasDisabledMode(Attribute::DISABLED_VIEW)) {
                    continue;
                }

                /* we let the attribute add itself to the view array */
                $p_attrib->addToViewArray($mode, $result, $record);
            } else {
                Tools::atkerror("Attribute $attribname not found!");
            }
        }

        /* inject sections */
        if ($injectSections) {
            $this->injectSections($result['fields']);
        }

        /* return the result array */

        return $result;
    }

    /**
     * Add sections to the edit/view fields array.
     *
     * @param array $fields fields array (will be modified in-place)
     */
    public function injectSections(array &$fields): void
    {
        $this->groupFieldsBySection($fields);

        $addedSections = [];
        $result = [];
        foreach ($fields as $field) {
            /// we add the section link before the first attribute that is in it
            $fieldSections = $field['sections'];
            if (!is_array($fieldSections)) {
                $fieldSections = array($fieldSections);
            }

            $newSections = array_diff($fieldSections, $addedSections);
            if (Tools::count($newSections) > 0) {
                foreach ($newSections as $section) {
                    if (str_contains($section, '.')) {
                        $result[] = array(
                            'html' => 'section',
                            'name' => $section,
                            'tabs' => $field['tabs'],
                        );
                        $addedSections[] = $section;
                    }
                }
            }

            $result[] = $field;
        }

        $fields = $result;
    }

    /**
     * Group fields by section.
     *
     * @param array $fields fields array (will be modified in-place)
     */
    public function groupFieldsBySection(array &$fields): void
    {
        $result = [];
        $sections = [];

        // first find sectionless fields and collect all sections
        foreach ($fields as $field) {
            if ($field['sections'] == '*' || (Tools::count($field['sections']) == 1 && $field['sections'][0] == $this->m_default_tab)) {
                $result[] = $field;
            } else {
                if (is_array($field['sections'])) {
                    $sections = array_merge($sections, $field['sections']);
                }
            }
        }

        $sections = array_unique($sections);

        // loop through each section (except the default tab/section) of the mode we are currently in.
        while (Tools::count($sections) > 0) {
            $section = array_shift($sections);

            // find fields for this section
            foreach ($fields as $field) {
                if (is_array($field['sections']) && in_array($section, $field['sections'])) {
                    $result[] = $field;
                }
            }
        }

        $fields = $result;
    }

    /**
     * Retrieve the initial values for a new record.
     *
     * The system calls this method to create a new record. By default
     * this method returns an empty record, but derived nodes may override
     * this method to perform record initialization.
     *
     * @return array Array containing an initial value per attribute.
     *               Only attributes that are initialized appear in the
     *               array.
     */
    public function initial_values(): array
    {
        $record = [];

        foreach (array_keys($this->m_attribList) as $attrName) {
            $attr = $this->getAttribute($attrName);

            if (is_array($this->m_postvars) && isset($this->m_postvars[$attrName])) {
                $value = $attr->fetchValue($this->m_postvars);
            } else {
                $value = $attr->initialValue();
            }

            if ($value !== null) {
                $record[$attr->fieldName()] = $value;
            }
        }

        return $record;
    }

    /**
     * Retrieve new values for an existing record.
     *
     * The system calls this method to override the values of a record
     * before editing the record.
     * The default implementation does not do anything to the record, but
     * derived classes may override this method to make modifications to.
     * the record.
     *
     * @param array $record The record that is about to be edited.
     * @return array The manipulated record.
     */
    public function edit_values(array $record): array
    {
        return $record;
    }

    /**
     * Get the template to use for a certain action.
     *
     * The system calls this method to determine which template to use when
     * rendering a certain screen. The default implementation always returns
     * the same template for the same action (it ignores parameter 2 and 3).
     * You can override this method in derived classes however, to determine
     * on the fly which template to use.
     * The action, the current record (if any) and the tab are passed as
     * parameter. By using these params, you can have custom templates per
     * action, and/or per tab, and even per record.
     *
     * @param string $action The action for which you wnat to retrieve the
     *                       template.
     * @param array|null $record The record for which you want to return the
     *                       template (or NULL if there is no record).
     * @param string $tab The name of the tab for which you want to
     *                       retrieve the template.
     * @return string|null The filename of the template (without path)
     */
    public function getTemplate(string $action, ?array $record = null, string $tab = ''): ?string
    {
        switch ($action) {
            case 'add': // add and edit both use the same form.
            case 'edit':
                return 'editform.tpl';
            case 'view':
                return 'viewform.tpl';
            case 'search':
                return 'searchform.tpl';
            case 'smartsearch':
                return 'smartsearchform.tpl';
            case 'admin':
                return 'recordlist.tpl';
        }

        return null;
    }

    /**
     * Function outputs a form with all values hidden.
     *
     * This is probably only useful for the atkOneToOneRelation's hide method.
     *
     * @param string $mode The edit mode ("add" or "edit")
     * @param array|null $record The record that should be hidden.
     * @param array|string $forceList A key-value array used to preset certain
     *                            fields to a certain value, regardless of the
     *                            value in the record.
     * @param string $fieldprefix Of set, each form element is prefixed with
     *                            the specified prefix (used in embedded form
     *                            fields)
     *
     * @return string HTML fragment containing all hidden elements.
     * @throws Exception
     */
    public function hideForm(string $mode = 'add', array $record = null, array|string $forceList = '', string $fieldprefix = ''): string
    {
        /* suppress all */
        $suppressList = [];
        foreach (array_keys($this->m_attribIndexList) as $r) {
            $suppressList[] = $this->m_attribIndexList[$r]['name'];
        }

        /* get data, transform into "form", return */
        $data = $this->editArray($mode, $record, $forceList, $suppressList, $fieldprefix);
        return implode('', $data['hide']);
    }

    /**
     * Builds a list of tabs.
     *
     * This doesn't generate the actual HTML code, but returns the data for
     * the tabs (title, selected, urls that should be loaded upon click of the
     * tab etc.).
     *
     * @param string $action The action for which the tabs should be generated.
     * @return array List of tabs
     *
     * @todo Make translation of tabs module aware
     */
    public function buildTabs(string $action = ''): array
    {
        if ($action == '') {
            // assume active action
            $action = $this->m_action;
        }

        $result = [];

        // which tab is currently selected
        $tab = $this->getActiveTab();

        // build navigator
        $list = $this->getTabs($action);

        if (is_array($list)) {
            $sm = SessionManager::getInstance();
            $newtab['total'] = Tools::count($list);
            foreach ($list as $t) {
                $newtab['title'] = $this->text(["tab_$t", $t]);
                $newtab['tab'] = $t;
                $url = Tools::dispatch_url($this->atkNodeUri(), $this->m_action, ['atktab' => $t]);
                if ($this->m_action == 'view') {
                    $newtab['link'] = $sm->sessionUrl($url, SessionManager::SESSION_DEFAULT);
                } else {
                    $newtab['link'] = "javascript:ATK.FormSubmit.atkSubmit('" . Tools::atkurlencode($sm->sessionUrl($url, SessionManager::SESSION_DEFAULT)) . "')";
                }
                $newtab['selected'] = ($t == $tab);
                $result[] = $newtab;
            }
        }

        return $result;
    }

    /**
     * Retrieve an array with the default actions for a certain mode.
     *
     * This will return a list of actions that can be performed on records
     * of this node in an admin screen.
     * The actions may contain a [pk] template variable to reference a record,
     * so for each record you should run the stringparser on the action.
     *
     * @param string $mode The mode for which you want a list of actions.
     *                       Currently available modes for this method:
     *                       - "admin" (for actions in adminscreens)
     *                       - "relation" (for the list of actions when
     *                       displaying a recordlist in a onetomany-relation)
     *                       - "view" (for actions when viewing only)
     *                       Note: the default implementation of defaultActions
     *                       makes no difference between "relation" and "admin"
     *                       and will return the same actions for both, but you
     *                       might want to override this behaviour in derived
     *                       classes.
     * @param array $params An array of extra parameters to add to all the
     *                       action urls. You can use this to pass things like
     *                       an atkfilter for example. The array should be
     *                       key/value based.
     * @return array List of actions in the form array($action=>$actionUrl)
     */
    public function defaultActions(string $mode, array $params = []): array
    {
        $actions = [];

        $params[self::PARAM_ATKSELECTOR] = "[pk]";

        if (!$this->hasFlag(self::NF_NO_VIEW) && $this->allowed('view')) {
            $actions['view'] = Tools::dispatch_url($this->atkNodeUri(), 'view', array_merge($params, ['atkaction' => 'view']));
        }

        if ($mode != 'view') {
            if (!$this->hasFlag(self::NF_NO_EDIT) && $this->allowed('edit')) {
                $actions['edit'] = Tools::dispatch_url($this->atkNodeUri(), 'edit', array_merge($params, ['atkaction' => 'edit']));
            }

            if (!$this->hasFlag(self::NF_NO_DELETE) && $this->allowed('delete')) {
                $actions['delete'] = Tools::dispatch_url($this->atkNodeUri(), 'delete', array_merge($params, ['atkaction' => 'delete']));
            }
            if ($this->hasFlag(self::NF_COPY) && $this->allowed('copy')) {
                $actions['copy'] = Tools::dispatch_url($this->atkNodeUri(), 'copy', array_merge($params, ['atkaction' => 'copy']));
            }
            if ($this->hasFlag(self::NF_EDITAFTERCOPY) && $this->allowed('editcopy')) {
                $actions['editcopy'] = Tools::dispatch_url($this->atkNodeUri(), 'editcopy', array_merge($params, ['atkaction' => 'editcopy']));
            }
        }

        return $actions;
    }

    /**
     * Sets the priority range, for multi-record-priority actions.
     *
     * @param int $min the minimum priority
     * @param int $max the maximum priority (0 for auto => min + record count)
     */
    public function setPriorityRange(int $min = 1, int $max = 0): void
    {
        $this->m_priority_min = (int)$min;
        if ($max < $this->m_priority_min) {
            $max = 0;
        } else {
            $this->m_priority_max = $max;
        }
    }

    /**
     * Sets the possible multi-record-priority actions.
     *
     * @param array $actions list of actions
     */
    public function setPriorityActions(array $actions): void
    {
        if (!is_array($actions)) {
            $this->m_priority_actions = [];
        } else {
            $this->m_priority_actions = $actions;
        }
    }

    /**
     * Get extended search action.
     *
     * @return string|null extended search action
     */
    public function getExtendedSearchAction(): ?string
    {
        if (empty($this->m_extended_search_action)) {
            return Config::getGlobal('extended_search_action');
        } else {
            return $this->m_extended_search_action;
        }
    }

    /**
     * Set extended search action.
     *
     * @param string $action extended search action
     */
    public function setExtendedSearchAction(string $action): void
    {
        $this->m_extended_search_action = $action;
    }

    /**
     * Function returns a page in which the user is asked if he really wants
     * to perform a certain action.
     *
     * @param array|string $atkselector Selector of current record on which the
     *                               action will be performed (String), or an
     *                               array of selectors when multiple records are
     *                               processed at once. The method uses the
     *                               selector(s) to display the current record(s)
     *                               in the confirmation page.
     * @param string $action The action for which confirmation is needed.
     * @param bool $checkoverride If set to true, this method will try to
     *                               find a custom method named
     *                               "confirm".$action."()" (e.g.
     *                               confirmDelete() and call that method
     *                               instead.
     * @param bool $mergeSelectors Merge all selectors to one selector string (if more then one)?
     * @param string|null $csrfToken
     *
     * @return string Complete html fragment containing a box with the
     *                confirmation page, or the output of the custom
     *                override if $checkoverride was true.
     * @throws \Smarty\Exception
     */
    public function confirmAction(
        array|string $atkselector,
        string            $action,
        bool              $checkoverride = true,
        bool              $mergeSelectors = true,
        string            $csrfToken = null
    ): string
    {
        $method = 'confirm' . $action;
        if ($checkoverride && method_exists($this, $method)) {
            return $this->$method($atkselector);
        }

        $ui = $this->getUi();

        if (is_array($atkselector)) {
            $atkselectorString = '((' . implode(') OR (', $atkselector) . '))';
        } else {
            $atkselectorString = $atkselector;
        }

        $sm = SessionManager::getInstance();

        $formstart = '<form action="' . Config::getGlobal('dispatcher') . '" method="post">';
        $formstart .= $sm->formState();
        $formstart .= '<input type="hidden" name="atkaction" value="' . $action . '">';
        $formstart .= '<input type="hidden" name="atknodeuri" value="' . $this->atkNodeUri() . '">';

        if (isset($csrfToken)) {
            $this->getHandler($action);
            $formstart .= '<input type="hidden" name="atkcsrftoken" value="' . $csrfToken . '">';
        }

        if ($atkselectorString) {
            if ($mergeSelectors) {
                $formstart .= '<input type="hidden" name="' . self::PARAM_ATKSELECTOR . '" value="' . $atkselectorString . '">';
            } else {
                if (!is_array($atkselector)) {
                    $formstart .= '<input type="hidden" name="' . self::PARAM_ATKSELECTOR . '" value="' . $atkselector . '">';
                } else {
                    foreach ($atkselector as $selector) {
                        $formstart .= '<input type="hidden" name="' . self::PARAM_ATKSELECTOR . '[]" value="' . $selector . '">';
                    }
                }
            }
        }

        $buttons = $this->getFormButtons($action);
        if (Tools::count($buttons) == 0) {
            $buttons[] = '<input name="confirm" type="submit" class="btn btn-primary btn_ok atkdefaultbutton" value="' . $this->text('yes') . '">';
            $buttons[] = '<input name="cancel" type="submit" class="btn btn-default btn_cancel mr-1" value="' . $this->text('no') . '">';
        }

        $record = [];
        $content = $this->confirmActionText($atkselector, $action);

        if ($atkselectorString) {
            $recs = $this->select($atkselectorString)->includes($this->descriptorFields())->getAllRows();
            if (Tools::count($recs) == 1) {
                // 1 record, put it in the page title (with the actionTitle call, a few lines below)
                $record = $recs[0];
                $this->getPage()->setTitle(Tools::atktext('app_shorttitle') . ' - ' . $this->actionTitle($action, $record));
            } else {
                // we are going to perform an action on more than one record
                // show a list of affected records, at least if we can find a
                // descriptor_def method
                if ($this->m_descTemplate != null || method_exists($this, 'descriptor_def')) {
                    $content .= '<div class="mt-2">';
                    $content .= '<div>' . $this->text('confirm_action_title_multi') . '</div>';
                    $content .= '<ul class="mt-1">';
                    for ($i = 0, $_i = Tools::count($recs); $i < $_i; ++$i) {
                        $content .= '<li>' . str_replace(' ', '&nbsp;', htmlentities($this->descriptor($recs[$i])));
                    }
                    $content .= '</ul></div>';
                }
            }

        } else {
            // no record selected
            $this->getPage()->setTitle(Tools::atktext('app_shorttitle') . ' - ' . $this->actionTitle($action));
        }

        $output = $ui->renderAction($action, [
            'content' => $content,
            'formstart' => $formstart,
            'formend' => '</form>',
            'buttons' => $buttons,
        ]);

        return $ui->renderBox([
            'title' => $this->actionTitle($action, $record),
            'content' => $output,
        ]);
    }

    /**
     * Determine the confirmation message.
     *
     * @param string|string[] $atkselector The record(s) on which the action is performed.
     * @param string $action The action being performed.
     * @param bool $checkoverride If true, returns the output of a custom method named "confirm".$action."text()"
     * @return string The confirmation text.
     */
    public function confirmActionText(array|string $atkselector = '', string $action = 'delete', bool $checkoverride = true): string
    {
        $method = 'confirm' . $action . 'text';
        if ($checkoverride && method_exists($this, $method)) {
            return $this->$method($atkselector);
        } else {
            return $this->text("confirm_$action" . (is_array($atkselector) && Tools::count($atkselector) > 1 ? '_multi' : ''));
        }
    }

    /**
     * Small compare function for sorting attribs on order field.
     *
     * @param array $a The first attribute
     * @param array $b The second attribute
     * @return int
     */
    private function attrib_cmp(array $a, array $b): int
    {
        if ($a['order'] == $b['order']) {
            return 0;
        }

        return ($a['order'] < $b['order']) ? -1 : 1;
    }

    /**
     * This function initialises certain elements of the node.
     *
     * This must be called right after the constructor. The function has a
     * check to prevent it from being executed twice.
     */
    public function init(): void
    {
        Tools::atkdebug('init for ' . $this->m_type);

        // Check if initialisation is not already done.
        if ($this->m_initialised) {
            return;
        }

        // We assign the $this reference to the attributes at this stage, since
        // it fails when we do it in the add() function.
        // See also the comments in the add() function.
        foreach (array_keys($this->m_attribList) as $attribname) {
            $p_attrib = $this->m_attribList[$attribname];
            $p_attrib->setOwnerInstance($this);
        }

        $this->_addListeners();

        // We set the tabs for the attributes
        foreach (array_keys($this->m_attribList) as $attribname) {
            $p_attrib = $this->m_attribList[$attribname];
            $p_attrib->setTabs($this->m_attributeTabs[$attribname]);
        }

        $this->attribSort();
        $this->setAttribSizes();
        $this->m_initialised = true;

        // Call the attributes postInit method to do some last time
        // initialization if necessary.
        foreach (array_keys($this->m_attribList) as $attribname) {
            $p_attrib = $this->m_attribList[$attribname];
            $p_attrib->postInit();
        }

        $this->setFilters();
        $this->setRowColors();
        $this->setLegendItems();
        $this->setAdminHeaderFilterButtons();
        $this->setAdminHeaderFilter();
        $this->adminPageNodeHelp = $this->buildAdminPageNodeHelp();

    }

    /**
     * Use it to add filters in child nodes.
     */
    protected function setFilters(): void
    {
    }

    /**
     * Use this to add color conditions for rows in child nodes.
     */
    protected function setRowColors(): void
    {
    }

    /**
     * Use this to add legend items in child nodes.
     */
    public function setLegendItems(): void
    {
    }

    /**
     * @deprecated Use setAdminHeaderFilter() instead
     */
    public function setAdminHeaderFilterButtons(): void
    {
    }

    /**
     * Use this to add button or input filters in child nodes.
     */
    public function setAdminHeaderFilter(): void
    {
    }

    /**
     * Use this to add node help in the right corner of the header of the adminPage
     */
    public function buildAdminPageNodeHelp(): string
    {
        return '';
    }

    public function getAdminPageNodeHelp(): ?string
    {
        return $this->adminPageNodeHelp;
    }

    public function setAdminPageNodeHelp(string $nodeHelper): self
    {
        $this->adminPageNodeHelp = $nodeHelper;
        return $this;
    }

    public function getAdminPageBookmarkLink(): ?string
    {
        return $this->adminPageBookmarkLink;
    }

    public function setAdminPageBookmarkLink(string $adminPageBookmarkLink): self
    {
        $this->adminPageBookmarkLink = $adminPageBookmarkLink;
        return $this;
    }

    public function getLegendItems(): array
    {
        return $this->legendItems;
    }

    public function addLegendItem(string $text, string $uiStateColor = UIStateColors::STATE_WHITE): void
    {
        // TODO: trasform in object?
        $this->legendItems[] = ['text' => $text, 'color' => $uiStateColor];
    }

    /**
     * Build the box of the legend in the adminHeader.
     *
     * @param string $sep Separator between one item and another
     * @param string $title Title of the legend (default: "Legend")
     * @return string String with html to render the legend box
     */
    function buildAdminHeaderLegend(string $sep = ' ', string $title = 'legend'): string
    {
        if (!$this->legendItems) {
            return '';
        }

        $ret = '<div class="row no-gutters legenda-box"><div class="legenda-titolo my-auto pb-1">' . $this->text($title) . ': </div>';

        for ($i = 0; $i < count($this->legendItems); $i++) {
            $item = $this->legendItems[$i];

            $text = $this->text($item['text'] ?? 'n.d.');

            $bgColor = UIStateColors::getHex($item['color'] ?? UIStateColors::STATE_WHITE);
            $borderColor = Tools::dimColorBy($bgColor);
            $txtColor = Tools::isLightTxtUsingBg($bgColor) ? '#F8F9FA' : '#212529';

            $ret .= '<div class="legenda-item-box ml-1 mb-1 p-1 pl-2 pr-2 border rounded" style="background-color: ' . $bgColor . '; border-color: ' . $borderColor . ' !important;">
                        <span class="legenda-item-text" style="color: ' . $txtColor . ' ">' . $text . '</span>
                     </div>';

            if ($i != count($this->legendItems) - 1) {
                $ret .= $sep;
            }
        }

        return $ret . '</div>';
    }

    /**
     * @deprecated Use getAdminHeaderButtonFilters instead
     */
    public function getAdminHeaderFilterButtons(): array
    {
        return $this->getAdminHeaderButtonFilters();
    }

    public function getAdminHeaderButtonFilters(): array
    {
        return $this->adminHeaderButtonFilters;
    }

    /**
     * @deprecated Use addAdminHeaderButtonFilter instead
     */
    public function addAdminHeaderFilterButton(string $label, array $values, ?string $noFilterLabel): void
    {
        $this->addAdminHeaderButtonFilter($label, $values, $noFilterLabel);
    }

    public function addAdminHeaderButtonFilter(string $label, array $values, ?string $noFilterLabel): void
    {
        // TODO: trasform in object?
        $this->adminHeaderButtonFilters[] = ['label' => $label, 'values' => $values, 'noFilterLabel' => $noFilterLabel];
    }

    /**
     * @deprecated Use buildAdminHeaderButtonFilters instead
     */
    function buildAdminHeaderFilterButtons(string $cssClass = '', string $uiStateColorActive = ''): string
    {
        return $this->buildAdminHeaderButtonFilters($cssClass, $uiStateColorActive);
    }

    /**
     * Builds the box of the button filters in the adminHeader.
     *
     * @param string $cssClass Css class of the buttons
     * @param string $uiStateColorActive UIStateColor of the active button
     * @return string
     */
    function buildAdminHeaderButtonFilters(string $cssClass = '', string $uiStateColorActive = ''): string
    {
        if (!$this->adminHeaderButtonFilters) {
            return '';
        }

        $ret = '<div class="mb-3">';
        foreach ($this->adminHeaderButtonFilters as $filter) {
            $ret .= $this->addAdminHeaderFilter($filter['label'], $filter['values'], $filter['noFilterLabel'], '', $cssClass, $uiStateColorActive) . '<div class="mb-1"></div>';
        }
        return $ret . '</div>';
    }

    /**
     * Builds a filter to insert into the filter box in the adminHeader.
     *
     * @param string $label Name of the filter
     * @param array $values Values of filter to show as a link
     * @param string|null $noFilterLabel Text to show when null filter (es. 'all' | 'none')
     * @param string $sepLinks Separator of links TODO: remove
     * @param string $cssClass Css class of the buttons
     * @param string $uiStateColorActive UIStateColor of the active button
     * @return string
     */
    private function addAdminHeaderFilter(string $label, array $values, ?string $noFilterLabel, string $sepLinks = '', string $cssClass = '',
                                          string $uiStateColorActive = ''): string
    {
        $sm = SessionManager::getInstance();
        $cssClass .= ' btn btn-sm btn-default mr-1 mb-1';

        if (!$uiStateColorActive) {
            // default active state
            $uiStateColorActive = UIStateColors::STATE_CYAN_LIGHT;
        }
        $cssBgClassActive = UIStateColors::getBgClassFromState($uiStateColorActive);

        $buttons = [];

        // button to clear filters
        if ($noFilterLabel) {
            if (!$sm or !$sm->pageVar($label) or !in_array($sm->pageVar($label), $values)) {
                $buttons[] = '<button class="' . $cssClass . ' ' . $cssBgClassActive . '" disabled>' . $this->formatTitle($noFilterLabel) . '</button>';
            } else {
                $buttons[] = Tools::actionHref($this->atkNodeUri(), 'admin', [$label => ''], $this->text($noFilterLabel), 'class="' . $cssClass . '"');
            }
        }

        // filters
        foreach ($values as $filter) {
            if ($sm and $sm->pageVar($label) == $filter) {
                $buttons[] = '<button class="' . $cssClass . ' ' . $cssBgClassActive . '" disabled>' . $this->formatTitle($filter) . '</button>';
            } else {
                $buttons[] = Tools::actionHref($this->atkNodeUri(), 'admin', [$label => $filter], $this->text($filter), 'class="' . $cssClass . '"');
            }
        }

        return $this->text($label) . ': ' . implode($sepLinks, $buttons);
    }

    /**
     * Builds the form of the input filters in the adminHeader.
     */
    function buildAdminHeaderInputFilters(string $cssClass = ''): string
    {
        if (!$this->adminHeaderInputFilters) {
            return '';
        }

        $isPrint = $this->m_postvars['print'] ?? false;

        $html = '<form action="' . Config::getGlobal('dispatcher') . '" method="get">';
        $html .= '<input type="hidden" name="atkaction" value="' . $this->getAction() . '">';
        $html .= '<input type="hidden" name="atknodeuri" value="' . $this->atkNodeUri() . '">';
        if ($atkMenu = SessionManager::getInstance()->globalStackVar(Node::PARAM_ATKMENU)) {
            $html .= '<input type="hidden" name="' . Node::PARAM_ATKMENU . '" value="' . $atkMenu . '">';
        }

        $html .= '<div class="filters form-inline ' . $cssClass . '">';

        foreach ($this->adminHeaderInputFilters as $filter) {

            /** @var Attribute $attr */
            $attr = $filter['attribute'];
            $attr->setOwnerInstance($this);

            $attrName = $attr->fieldName();

            if ($filter['newline']) {
                $html .= '</div><div class="filters form-inline ' . $cssClass . '">';
            }

            // check possible initial value
            if ($this->getAdminHeaderInputFilterValue($attrName) === null && $attr->initialValue()) {
                $this->setAdminHeaderInputFilterValue($attrName, $attr->initialValue());
            }

            if (!$isPrint) {
                $class = $filter['class'] ?: '';
                $html .= '<div class="filter form-group ' . $class . '">';
                if (!$attr->hasFlag(Attribute::AF_NO_LABEL)) {
                    $html .= '<span class="mr-1">' . $attr->label() . ':</span>';
                }

                if ($filter['mode'] == 'edit') {
                    $html .= $attr->edit($this->m_postvars, '', 'edit');
                } else if ($filter['mode'] == 'search-ext') {
                    $html .= $attr->search($this->m_postvars['atksearch'], true);
                } else {
                    $html .= $attr->search($this->m_postvars['atksearch']);
                }

                $html .= '</div>';

            } else { // (print)

                if ($filter['mode'] != 'edit') { // search
                    $this->m_postvars[$attrName] = $this->m_postvars['atksearch'][$attrName];
                    // TODO: some attributes may require special handling
                    if ($attr instanceof ListAttribute) {
                        $this->m_postvars[$attrName] = $this->m_postvars[$attrName][0];
                    }
                }

                if ($this->m_postvars[$attrName]) {
                    $value = $attr->display($this->m_postvars, 'view');
                    if (trim($value)) {
                        $html .= '<div class="filter form-group">';
                        if (!$attr->hasFlag(Attribute::AF_NO_LABEL)) {
                            $html .= '<strong class="mr-1">' . $attr->label() . '</strong>: ';
                        }
                        $html .= $value . '</div>';
                    }
                }
            }
        }

        if (!$isPrint) {
            $html .= '<button type="submit" class="btn btn-sm btn-primary ml-1">' . Tools::atktext('admin_header_input_submit') . '</button>';
        }

        $html .= '</div></form>';

        return $html;
    }

    /**
     * @param string $name
     * @return mixed|null
     */
    protected function getAdminHeaderInputFilterValue(string $name): mixed
    {
        $filter = $this->adminHeaderInputFilters[$name] ?? null;
        if ($filter === null) {
            // eventuale filtro passato da un'altra pagina
            return $this->m_postvars[$name] ?? null;
        }
        if ($filter['mode'] === 'edit') {
            $value = $this->m_postvars[$name] ?? null;
        } else if ($filter['mode'] === 'search-ext') {
            $value = $this->m_postvars['atksearch'][$name] ?? null;
        } else {
            $value = $this->m_postvars['atksearch'][$name][0] ?? null;
        }
        // TODO: for the manyToOne the value is like "table.key=value" and should be converted (as well as overwritten in the m_postvars)
        if (is_array($value) && count($value) == 1 && $value[0] == '') {
            return null;
        }
        return $value;
    }

    private function setAdminHeaderInputFilterValue(string $name, $value): void
    {
        if ($filter = $this->adminHeaderInputFilters[$name]) {
            if ($filter['mode'] === 'edit') {
                $this->m_postvars[$name] = $value;
            } else if ($filter['mode'] === 'search-ext') {
                $this->m_postvars['atksearch'][$name] = $value;
            } else {
                $this->m_postvars['atksearch'][$name][0] = $value;
            }
        }
    }

    protected function addAdminHeaderInputFilter(Attribute $attribute, string $mode = 'edit', string $class = '', bool $newline = false): self
    {
        $this->adminHeaderInputFilters[$attribute->fieldName()] = [
            'attribute' => $attribute,
            'mode' => $mode,
            'class' => $class,
            'newline' => $newline
        ];
        return $this;
    }

    /**
     * Formats a string in a title
     *
     * @param string $title
     * @param bool $html True if you want to insert html tags
     * @return string
     */
    protected function formatTitle(string $title, bool $html = true): string
    {
        $ret = $this->text($title);
        if ($html) {
            $ret = '<strong>' . $ret . '</strong>';
        }
        return $ret;
    }

    /**
     * Add the listeners for the current node
     * A listener can be defined either by placing an instantiated object
     * or the full location in Tools::atkimport( style notation, in a global array
     * called $g_nodeListeners (useful for example for adding listeners
     * to nodes from another module's module.inc file. in module.inc files,
     * $listeners can be used to add listeners to a node.
     */
    public function _addListeners(): void
    {
        $atk = Atk::getInstance();
        if (isset($atk->g_nodeListeners[$this->atkNodeUri()])) {
            foreach ($atk->g_nodeListeners[$this->atkNodeUri()] as $listener) {
                if (is_object($listener)) {
                    $this->addListener($listener);
                } else {
                    if (is_string($listener)) {
                        $listenerobj = new $listener();
                        if (is_object($listenerobj)) {
                            $this->addListener($listenerobj);
                        } else {
                            Tools::atkdebug("We couldn't find a classname for listener with supposed nodetype: '$listener'");
                        }
                    } else {
                        Tools::atkdebug("Failed to add listener with supposed nodetype: '$listener'");
                    }
                }
            }
        }
    }

    /**
     * This function reads meta information from the database and initialises
     * its attributes with the metadata.
     *
     * This method should be called before rendering a form, if you want the
     * sizes of all the inputs to match the fieldlengths from the database.
     */
    public function setAttribSizes(): bool
    {
        if ($this->m_attribsizesset) {
            return true;
        }

        if ($this->m_table) {
            $metainfo = $this->getDb()->tableMeta($this->m_table);

            foreach (array_keys($this->m_attribList) as $attribname) {
                $p_attrib = $this->m_attribList[$attribname];
                if (is_object($p_attrib)) {
                    $p_attrib->fetchMeta($metainfo);
                }
            }
        }
        $this->m_attribsizesset = true;
        return true;
    }

    /**
     * Render a generic action.
     *
     * Renders actionpage.tpl for the desired action. This includes the
     * given block(s) and a pagetrial, but not a box.
     *
     * @param string $action The action for which the page is rendered.
     * @param mixed|array $blocks Pieces of html content to be rendered. Can be a
     *                       single string with content, or an array with
     *                       multiple content blocks.
     *
     * @return string Piece of HTML containing the given blocks and a pagetrail.
     */
    public function renderActionPage(string $action, string|array $blocks = []): string
    {
        if (!is_array($blocks)) {
            $blocks = ($blocks == '' ? [] : array($blocks));
        }

        $ui = $this->getUi();

        // todo: overridable action templates
        return $ui->render('actionpage.tpl', [
            'blocks' => $blocks,
            'title' => !$this->hidePageTitle ? $ui->title($this->m_module, $this->m_type) : '',
            'footer' => Footer::getInstance()->render()
        ]);
    }

    function action_admin(AdminHandler $handler): void
    {
        $this->setAttributesFlags(null, 'admin');
        $handler->action_admin();
    }

    function adminPage(AdminHandler $handler, array $actions = []): string
    {
        return $handler->adminPage($actions);
    }

    function addPage(AddHandler $handler, array $record = null): string
    {
        $this->setAttributesFlags($record, 'add');
        return $handler->addPage($record);
    }

    function viewPage(ViewHandler $handler, array $record, Node $node, bool $renderbox = true): string
    {
        $this->setAttributesFlags($record, 'view');
        return $handler->viewPage($record, $node, $renderbox);
    }

    function editPage(EditHandler $handler, array $record): string
    {
        $this->setAttributesFlags($record, 'edit');
        return $handler->editPage($record);
    }

    /**
     * @throws Exception
     */
    function searchPage(SearchHandler $handler, array $record = null): string
    {
        $this->setAttributesFlags($record, 'search');
        return $handler->searchPage($record);
    }

    /**
     * use this function to set flags on the attributes of the node.
     * It is called automatically by action_admin, addPage, viewPage, editPage and searchPage functions.
     *
     * @param array|null $record
     * @param string $mode 'admin', 'add', 'view' and 'edit'
     * @return void
     */
    function setAttributesFlags(array $record = null, string $mode = ''): void
    {

    }

    /**
     * Use this function to enable feedback for one or more actions.
     *
     * When feedback is enabled, the action does not immediately return to the
     * previous screen, but first displays a message to the user. (e.g. 'The
     * record has been saved').
     *
     * @param mixed $action The action for which feedback is enabled. You can
     *                          either pass one action or an array of actions.
     * @param int $statusmask The status(ses) for which feedback is enabled.
     *                          If for example this is set to ActionHandler::ACTION_FAILED,
     *                          feedback is enabled only when the specified
     *                          action failed. It is possible to specify more
     *                          than one status by concatenating with '|'.
     */
    public function setFeedback(mixed $action, int $statusmask): void
    {
        if (is_array($action)) {
            for ($i = 0, $_i = Tools::count($action); $i < $_i; ++$i) {
                $this->m_feedback[$action[$i]] = $statusmask;
            }
        } else {
            $this->m_feedback[$action] = $statusmask;
        }
    }

    /**
     * Get the page instance of the page on which the node can render output.
     *
     * @return Page The page instance.
     */
    public function getPage(): Page
    {
        return Page::getInstance();
    }

    /**
     * Returns a new page builder instance.
     *
     * @return PageBuilder
     */
    public function createPageBuilder(): PageBuilder
    {
        return new PageBuilder($this);
    }

    /**
     * Redirect the browser to a different location.
     *
     * This is usually used at the end of actions that have no output. An
     * example: when the user clicks 'save and close' in an edit screen, the
     * action 'save' is executed. If the save is succesful, this method is
     * called to redirect the user back to the adminpage.
     * When $config_debug is set to 2, redirects are paused and you can click
     * a link to execute the redirect (useful for debugging the action that
     * called the redirect).
     * Note: this method should be called before any output has been sent to
     * the browser, i.e. before any echo or before the call to
     * Output::outputFlush().
     *
     * @static
     *
     * @param string $location The url to which you want to redirect the user.
     *                             If ommitted, the call automatically redirects
     *                             to the previous screen of the user. (one level
     *                             back on the session stack).
     * @param array $recordOrExit If you pass a record here, the record is passed
     *                             as 'atkpkret' to the redirected url. Usually it's
     *                             not necessary to pass this parameter. If you pass a
     *                             boolean here we assume it's value must be used for
     *                             the exit parameter.
     * @param bool $exit Exit script after redirect.
     * @param int $levelskip Number of levels to skip
     */
    public function redirect(string $location = '', array $recordOrExit = [], bool $exit = false, int $levelskip = 1): void
    {
        Tools::atkdebug('node::redirect()');

        $record = $recordOrExit;
        if (is_bool($recordOrExit)) {
            $record = [];
            $exit = $recordOrExit;
        }

        if ($location == '') {
            $sm = SessionManager::getInstance();
            $location = $sm->sessionUrl(Config::getGlobal('dispatcher'), SessionManager::SESSION_BACK, $levelskip);
        }

        if (Tools::count($record)) {
            if (isset($this->m_postvars['atkpkret'])) {
                $location .= '&' . $this->m_postvars['atkpkret'] . '=' . rawurlencode($this->primaryKey($record));
            }
        }

        Tools::redirect($location, $exit);
    }

    /**
     * Parse a set of url vars into a valid record structure.
     *
     * When attributes are posted in a formposting, the values may not be
     * valid yet. After posting, a call to updateRecord should be made to
     * translate the html values into the internal values that the attributes
     * work with.
     *
     * @param array|string $vars The request variables that were posted from a form.
     * @param array|null $includes Only fetch the value for these attributes.
     * @param array|null $excludes Don't fetch the value for these attributes.
     * @param bool|array $postedOnly Only fetch the value for attributes that have really been posted.
     * @return array A valid record.
     */
    public function updateRecord(array|string $vars = '', array $includes = null, array $excludes = null, bool|array $postedOnly = false): array
    {
        if ($vars == '') {
            $vars = $this->m_postvars;
        }
        $record = [];

        foreach (array_keys($this->m_attribList) as $attribname) {
            if ((!is_array($includes) || in_array($attribname, $includes)) && (!is_array($excludes) || !in_array($attribname, $excludes))) {
                $p_attrib = $this->m_attribList[$attribname];
                if (!$postedOnly || $p_attrib->isPosted($vars)) {
                    $record[$p_attrib->fieldName()] = $p_attrib->fetchValue($vars);
                }
            }
        }

        if (isset($vars['atkprimkey'])) {
            $record['atkprimkey'] = $vars['atkprimkey'];
        }

        return $record;
    }

    /**
     * Update a record with variables from a form posting.
     *
     * Similar to updateRecord(), but here you can pass an existing record
     * (for example loaded from the db), and update it with the the variables
     * from the request. Instead of returning a record, the record you pass
     * is modified directly.
     *
     * @param array $record The record to update.
     * @param array $vars The request variables that were posted from a form.
     */
    public function modifyRecord(array &$record, array $vars): void
    {
        foreach (array_keys($this->m_attribList) as $attribname) {
            $p_attrib = $this->m_attribList[$attribname];
            $record[$p_attrib->fieldName()] = $p_attrib->fetchValue($vars);
        }
    }

    /**
     * Get descriptor handler.
     *
     * @return object|null descriptor handler
     */
    public function getDescriptorHandler(): ?object
    {
        return $this->m_descHandler;
    }

    /**
     * Set descriptor handler.
     *
     * @param object $handler The descriptor handler.
     */
    public function setDescriptorHandler(object $handler): void
    {
        $this->m_descHandler = $handler;
    }

    /**
     * Returns the descriptor template for this node.
     *
     * @return string|null The descriptor Template
     */
    public function getDescriptorTemplate(): ?string
    {
        return $this->m_descTemplate;
    }

    /**
     * Sets the descriptor template for this node.
     *
     * @param string $template The descriptor template.
     */
    public function setDescriptorTemplate(string $template): void
    {
        $this->m_descTemplate = $template;
    }

    /**
     * Retrieve the list of attributes that are used in the descriptor
     * definition.
     *
     * @return array The names of the attributes forming the descriptor.
     */
    public function descriptorFields(): array
    {
        $fields = [];

        // See if node has a custom descriptor definition.
        if ($this->m_descTemplate != null || method_exists($this, 'descriptor_def')) {
            if ($this->m_descTemplate != null) {
                $descriptordef = $this->m_descTemplate;
            } else {
                $descriptordef = $this->descriptor_def();
            }

            // parse fields from descriptordef
            $parser = new StringParser($descriptordef);
            $fields = $parser->getFields();

            // There might be fields that have a '.' in them. These fields are
            // a concatenation of an attributename (probably a relation), and a subfield
            // (a field of the destination node).
            // The actual field is the one in front of the '.'.
            for ($i = 0, $_i = Tools::count($fields); $i < $_i; ++$i) {
                $elems = explode('.', $fields[$i]);
                if (Tools::count($elems) > 1) {
                    // dot found. attribute is the first item.
                    $fields[$i] = $elems[0];
                }
            }
        } else {
            // default descriptor.. (default is first attribute of a node)
            $keys = array_keys($this->m_attribList);
            $fields[0] = $keys[0];
        }

        return $fields;
    }

    /**
     * Determine a descriptor of a record.
     *
     * The descriptor is a string that describes a record for the user. For
     * person records, this may be the firstname and the lastname, for
     * companies it may be the company name plus the city etc.
     * The descriptor is used when displaying records in a dropdown for
     * example, or in the title of editpages, delete confirmations etc.
     *
     * The descriptor method calls a method named descriptor_def() on the node
     * to retrieve a template for the descriptor (string with attributenames
     * between blockquotes, for example "[lastname], [firstname]".
     *
     * If the node has no descriptor_def() method, the first attribute of the
     * node is used as descriptor.
     *
     * Derived classes may override this method to implement custom descriptor
     * logic.
     *
     * @param array $record The record for which the descriptor is returned.
     *
     * @return string The descriptor for the record.
     */
    public function descriptor(array $record): string
    {
        // Descriptor handler is set?
        if ($this->m_descHandler != null) {
            return $this->m_descHandler->descriptor($record, $this);
        }

        // Descriptor template is set?
        if ($this->m_descTemplate != null) {
            $parser = new StringParser($this->m_descTemplate);

            return $parser->parse($record);

        } else {
            // See if node has a custom descriptor definition.
            if (method_exists($this, 'descriptor_def')) {
                $parser = new StringParser($this->descriptor_def());
                return $parser->parse($record);

            } else {

                if ($primaryKeyField = $this->primaryKeyField()) {
                    // default descriptor is primary key field
                    $descriptorAttribute = $primaryKeyField;
                } else {
                    // or first attribute
                    $attributesNames = array_keys($this->m_attribList);
                    $descriptorAttribute = $attributesNames[0];
                    if ($descriptorAttribute === self::ROW_COLOR_ATTRIBUTE) {
                        $descriptorAttribute = $attributesNames[1];
                    }
                }

                $ret = $record[$descriptorAttribute];
                if (is_array($ret)) {
                    return '';
                }

                return $ret;
            }
        }
    }

    /**
     * Validates a record.
     *
     * Validates unique fields, required fields, dataformat etc.
     *
     *
     * @param array $record The record to validate
     * @param string $mode The mode for which validation is performed ('add' or 'update')
     * @param array $ignoreList The list of attributes that should not be
     *                           validated
     * @return bool
     */
    public function validate(array &$record, string $mode, array $ignoreList = []): bool
    {
        /** @var NodeValidator $validateObj */
        $validateObj = new $this->m_validate_class();
        $validateObj->setNode($this);
        $validateObj->setRecord($record);
        $validateObj->setIgnoreList($ignoreList);
        $validateObj->setMode($mode);

        return $validateObj->validate();
    }

    /**
     * Add a unique field set.
     *
     * When you add a set of attributes using this method, any combination of
     * values for the attributes should be unique. For example, if you pass
     * array("name", "parent_id"), name does not have to be unique, parent_id
     * does not have to be unique, but the combination should be unique.
     *
     * @param array $fieldArr The list of names of attributes that should be
     *                        unique in combination.
     */
    public function addUniqueFieldset(array $fieldArr): void
    {
        sort($fieldArr);
        if (!in_array($fieldArr, $this->m_uniqueFieldSets)) {
            $this->m_uniqueFieldSets[] = $fieldArr;
        }
    }

    /**
     * Called by updateDb to load the original record inside the record if the
     * self::NF_TRACK_CHANGES flag is set.
     *
     * NOTE: this method is made public because it's called from the update handler
     *
     * @param array $record
     * @param array|string $excludes
     * @param array|string $includes
     */
    public function trackChangesIfNeeded(array &$record, array|string $excludes = '', array|string $includes = ''): void
    {
        if (!$this->hasFlag(self::NF_TRACK_CHANGES) || isset($record[self::ATK_ORG_REC])) {
            return;
        }

        // We need to add the NO_FILTER flag in case the new values would filter the record.
        $flags = $this->m_flags;

        $this->addFlag(self::NF_NO_FILTER);

        $record[self::ATK_ORG_REC] = $this->select()->where($record['atkprimkey'])->excludes($excludes)->includes($includes)->mode('edit')->getFirstRow();

        // Need to restore the NO_FILTER flag back to its original value.
        $this->m_flags = $flags;
    }

    /**
     * Update a record in the database.
     *
     * The record should already exist in the database, or this method will
     * fail.
     *
     * NOTE: Does not commit your transaction! If you are using a database that uses
     * transactions you will need to call 'Db::getInstance()->commit()' manually.
     *
     * @param array $record The record to update in the database.
     * @param bool $exectrigger wether to execute the pre/post update triggers
     * @param array|string $excludes exclude list (these attribute will *not* be updated)
     * @param array|string $includes include list (only these attributes will be updated)
     *
     * @return bool True if succesful, false if not.
     */
    public function updateDb(array &$record, bool $exectrigger = true, array|string $excludes = '', array|string $includes = ''): bool
    {
        $db = $this->getDb();
        $query = $db->createQuery();

        $query->addTable($this->m_table);

        // The record that must be updated is indicated by 'atkprimkey'
        // (not by atkselector, since the primary key might have
        // changed, so we use the atkprimkey, which is the value before
        // any update happened.)
        if ($record['atkprimkey'] != '') {
            $this->trackChangesIfNeeded($record, $excludes, $includes);

            if ($exectrigger) {
                $this->executeTrigger('preUpdate', $record);
            }

            $pk = $record['atkprimkey'];
            $query->addCondition($pk);

            $storelist = array('pre' => [], 'post' => [], 'query' => array());

            foreach (array_keys($this->m_attribList) as $attribname) {
                if ((!is_array($excludes) || !in_array($attribname, $excludes)) && (!is_array($includes) || in_array($attribname, $includes))) {
                    $p_attrib = $this->m_attribList[$attribname];
                    if ($p_attrib->needsUpdate($record) || Tools::atk_in_array($attribname, $includes)) {
                        $storemode = $p_attrib->storageType('update');
                        if (Tools::hasFlag($storemode, Attribute::PRESTORE)) {
                            $storelist['pre'][] = $attribname;
                        }
                        if (Tools::hasFlag($storemode, Attribute::POSTSTORE)) {
                            $storelist['post'][] = $attribname;
                        }
                        if (Tools::hasFlag($storemode, Attribute::ADDTOQUERY)) {
                            $storelist['query'][] = $attribname;
                        }
                    }
                }
            }

            if (!$this->_storeAttributes($storelist['pre'], $record, 'update')) {
                return false;
            }

            for ($i = 0, $_i = Tools::count($storelist['query']); $i < $_i; ++$i) {
                $p_attrib = $this->m_attribList[$storelist['query'][$i]];
                $p_attrib->addToQuery($query, $this->m_table, '', $record, 1, 'update'); // start at level 1
            }

            if (Tools::count($query->m_fields) && !$query->executeUpdate()) {
                return false;
            }

            if (!$this->_storeAttributes($storelist['post'], $record, 'update')) {
                return false;
            }

            // Now we call a postUpdate function, that can be used to do some processing after the record
            // has been saved.
            if ($exectrigger) {
                return $this->executeTrigger('postUpdate', $record);
            } else {
                return true;
            }
        }

        Tools::atkdebug('NOT UPDATING! NO SELECTOR SET!');

        return false;
    }

    /**
     * updateDb() v2
     *
     * @param array $record
     * @param array $includes Associative array ['attribute_name' => 'attribute_value']
     * @param bool $exectrigger
     * @param array $excludes
     * @return bool
     * @throws Exception
     */
    function updateDbIncludes(array $record, array $includes = [], bool $exectrigger = true, array $excludes = []): bool
    {
        // inject value of includes attributes into the record
        foreach ($includes as $k => $v) {
            $record[$k] = $v;
        }

        unset($record['__executedpreUpdate']);
        unset($record['__executedpostUpdate']);
        unset($record[self::ATK_ORG_REC]);

        $this->trackChangesIfNeeded($record);

        try {
            if ($exectrigger) {
                if (!$this->executeTrigger('preUpdate', $record)) {
                    throw new Exception('preUpdate error');
                }
            }

            $this->validate($record, 'update', ['__version']);
            if ($errors = $this->getAtkError($record)) {
                throw new Exception("validate error ($errors)'");
            }

            if (!$this->updateDb($record, $exectrigger, $excludes, array_keys($includes))) {
                throw new Exception('updateDb error');
            }

        } catch (Exception $e) {
            Tools::atkerror($this->atkNodeUri() . ' updateDbIncludes ' . $e->getMessage());
            return false;
        }

        return true;
    }

    /**
     * Call the store() method on a list of attributes.
     *
     * @param array $storelist The list of attributes for which the
     *                          store() method should be called.
     * @param array $record The master record being stored.
     * @param string $mode The storage mode ("add", "copy" or "update")
     * @return bool True if succesful, false if not.
     */
    public function _storeAttributes(array $storelist, array &$record, string $mode): bool
    {
        // store special storage attributes.
        for ($i = 0, $_i = Tools::count($storelist); $i < $_i; ++$i) {
            $p_attrib = $this->m_attribList[$storelist[$i]];
            if (!$p_attrib->store($this->getDb(), $record, $mode)) {
                // something went wrong.
                Tools::atkdebug("Store aborted. Attribute '" . $storelist[$i] . "' reported an error.");

                return false;
            }
        }

        return true;
    }

    /**
     * Copy a record in the database.
     *
     * Primarykeys are automatically regenerated for the copied record. Any
     * detail records (onetomanyrelation) are copied too. Refered records
     * manytoonerelation are not copied.
     *
     * @param array $record The record to copy.
     * @param string $mode The mode we're in (mostly "copy")
     * @return bool True if succesful, false if not.
     * @throws Exception
     */
    public function copyDb(array &$record, string $mode = 'copy'): bool
    {
        // add original record
        $original = $record; // force copy
        $record[self::ATK_ORG_REC] = $original;

        //notify precopy listeners
        $this->preNotify('precopy', $record);

        // remove primarykey (copied record will get a new primary key)
        unset($record['atkprimkey']);

        // remove trigger has been executed references
        foreach (array_keys($record) as $key) {
            if (preg_match('/^__executed.*$/', $key)) {
                unset($record[$key]);
            }
        }

        $this->preCopy($record);

        return $this->addDb($record, true, $mode);
    }

    /**
     * Get the current searchmode.
     *
     * @return mixed If there is one searchmode set for all attributes, this
     *               method returns a string. If there are searchmodes per
     *               attribute, an array of strings is returned.
     */
    public function getSearchMode(): mixed
    {
        //The searchmode of an index should be used only once, therefore it uses
        // atksinglesearchmode instead of atksearchmode.
        if (isset($this->m_postvars['atksinglesearchmode'])) {
            return $this->m_postvars['atksinglesearchmode'];
        } else {
            if (isset($this->m_postvars['atksearchmode'])) {
                return $this->m_postvars['atksearchmode'];
            }
        }

        return Config::getGlobal('search_defaultmode');
    }

    /**
     * Set some default for the selector.
     *
     * @param Selector $selector selector
     * @param string|null $condition condition
     * @param array $params condition bind parameters
     */
    protected function _initSelector(Selector $selector, string $condition = null, array $params = array()): void
    {
        $selector->orderBy($this->getOrder());
        $selector->ignoreDefaultFilters($this->hasFlag(self::NF_NO_FILTER));
        $selector->ignorePostvars($this->atkReadOptimizer());

        if ($condition != null) {
            $selector->where($condition, $params);
        }
    }

    /**
     * Retrieve records from the database using a handy helper class.
     *
     * @param string|null $condition condition
     * @param array $params condition bind parameters
     * @return array|Selector
     */
    public function select(string $condition = null, array $params = array()): array|Selector
    {
        $selector = new Selector($this);
        $this->_initSelector($selector, $condition, $params);

        return $selector;
    }

    /**
     * Returns a record (array) as identified by a primary key (usually an "id" column),
     * including applicable relations.
     *
     * @param int $pk primary key identifying the record
     * @return array the associated record, or null if no such record exists
     */
    public function fetchByPk(int $pk): array
    {
        return $this->select($this->getTable() . '.' . $this->primaryKeyField() . '= ?', array($pk))->getFirstRow();
    }

    /**
     * Add this node to an existing query.
     *
     * Framework method, it should not be necessary to call this method
     * directly.
     * This method is used when adding the entire node to an existing
     * query, as part of a join.
     *
     * @param Query $query The query statement
     * @param string $alias The aliasprefix to use for fields from this node
     * @param int $level The recursion level.
     * @param bool $allfields If set to true, all fields from the node are
     *                          added to the query. If set to false, only
     *                          the primary key and fields from the desriptor
     *                          are added.
     * @param string $mode The mode we're in
     * @param array $includes List of fields that should be included
     * @todo The allfields parameter is too inflexible.
     *
     */
    public function addToQuery(Query $query, string $alias = '', int $level = 0, bool $allfields = false, string $mode = 'select', array $includes = array()): void
    {
        if ($level >= 4) {
            return;
        }

        $usefieldalias = false;

        if ($alias == '') {
            $alias = $this->m_table;
        } else {
            $usefieldalias = true;
        }

        // If allfields is set, we load the entire record.. otherwise, we only
        // load the important fields (descriptor and primary key fields)
        // this is mainly used by onetoonerelation.
        if ($allfields) {
            $usedFields = array_keys($this->m_attribList);
        } else {
            $usedFields = Tools::atk_array_merge($this->descriptorFields(), $this->m_primaryKey, $includes);
            foreach (array_keys($this->m_attribList) as $name) {
                if (is_object($this->m_attribList[$name]) && $this->m_attribList[$name]->hasFlag(Attribute::AF_FORCE_LOAD)) {
                    $usedFields[] = $name;
                }
            }
            $usedFields = array_unique($usedFields);
        }

        foreach ($usedFields as $usedfield) {
            list($attribname) = explode('.', $usedfield);
            $p_attrib = $this->m_attribList[$attribname];
            if (is_object($p_attrib)) {
                $loadmode = $p_attrib->loadType('');

                if ($loadmode && Tools::hasFlag($loadmode, Attribute::ADDTOQUERY)) {
                    $fieldaliasprefix = '';
                    if ($usefieldalias) {
                        $fieldaliasprefix = $alias . '_AE_';
                    }

                    $dummy = [];
                    $p_attrib->addToQuery($query, $alias, $fieldaliasprefix, $dummy, $level, $mode);
                }
            } else {
                Tools::atkdebug("$attribname is not an object?! Check your descriptor_def for non-existant fields");
            }
        }
    }

    /**
     * Get search condition for this node.
     *
     * @param Query $query
     * @param string $table
     * @param string $alias
     * @param string $value
     * @param string $searchmode
     *
     * @return string The search condition
     */
    public function getSearchCondition($query, string $table, string $alias, $value, string $searchmode): string
    {
        $usefieldalias = false;

        if ($alias == '') {
            $alias = $this->m_table;
        } else {
            $usefieldalias = true;
        }

        $searchConditions = [];

        $attribs = $this->descriptorFields();
        $attribs = array_unique($attribs);

        foreach ($attribs as $field) {
            $p_attrib = $this->getAttribute($field);
            if (!is_object($p_attrib)) {
                continue;
            }
            $fieldaliasprefix = '';

            if ($usefieldalias) {
                $fieldaliasprefix = $alias . '_AE_';
            }

            // check if the node has a searchcondition method defined for this attr
            $methodName = $field . '_searchcondition';
            if (method_exists($this, $methodName)) {
                $searchCondition = $this->$methodName($query, $table, $value, $searchmode);
                if ($searchCondition != '') {
                    $searchConditions[] = $searchCondition;
                }
            } else {
                // checking for the getSearchCondition for backwards compatibility
                if (method_exists($p_attrib, 'getSearchCondition')) {

                    Tools::atkdebug("getSearchCondition: $table - $fieldaliasprefix");
                    $searchCondition = $p_attrib->getSearchCondition($query, $table, $value, $searchmode, $fieldaliasprefix);
                    if ($searchCondition != '') {
                        $searchConditions[] = $searchCondition;
                    }
                } else {
                    // if the attrib can't return it's searchcondition, we'll just add it to the query
                    // and hope for the best
                    $p_attrib->searchCondition($query, $table, $value, $searchmode, $fieldaliasprefix);
                }
            }
        }

        if (Tools::count($searchConditions)) {
            return '(' . implode(' OR ', $searchConditions) . ')';
        } else {
            return '';
        }
    }

    /**
     * Save a new record to the database.
     *
     * The record is passed by reference, because any autoincrement field gets
     * its value when stored to the database. The record is updated, so after
     * the call to addDb you can use access the primary key fields.
     *
     * NOTE: Does not commit your transaction! If you are using a database that uses
     * transactions you will need to call 'Db::getInstance()>commit()' manually.
     *
     * @param array $record The record to save.
     * @param bool $exectrigger Indicates whether the postAdd trigger
     *                            should be fired.
     * @param string $mode The mode we're in
     * @param array $excludelist List of attributenames that should be ignored
     *                            and not stored in the database.
     * @return bool True if succesful, false if not.
     * @throws Exception
     */
    public function addDb(array &$record, bool $exectrigger = true, string $mode = 'add', array $excludelist = []): bool
    {
        if ($exectrigger) {
            if (!$this->executeTrigger('preAdd', $record, $mode)) {
                Tools::atkerror('preAdd() failed!');
                return false;
            }
        }

        $db = $this->getDb();
        $query = $db->createQuery();

        $storelist = ['pre' => [], 'post' => [], 'query' => []];

        $query->addTable($this->m_table);

        foreach (array_keys($this->m_attribList) as $attribname) {
            $p_attrib = $this->m_attribList[$attribname];
            if (!Tools::atk_in_array($attribname, $excludelist) && ($mode != 'add' || $p_attrib->needsInsert($record))) {
                $storemode = $p_attrib->storageType($mode);
                if (Tools::hasFlag($storemode, Attribute::PRESTORE)) {
                    $storelist['pre'][] = $attribname;
                }
                if (Tools::hasFlag($storemode, Attribute::POSTSTORE)) {
                    $storelist['post'][] = $attribname;
                }
                if (Tools::hasFlag($storemode, Attribute::ADDTOQUERY)) {
                    $storelist['query'][] = $attribname;
                }
            }
        }

        if (!$this->_storeAttributes($storelist['pre'], $record, $mode)) {
            return false;
        }

        for ($i = 0, $_i = Tools::count($storelist['query']); $i < $_i; ++$i) {
            $p_attrib = $this->m_attribList[$storelist['query'][$i]];
            $p_attrib->addToQuery($query, $this->m_table, '', $record, 1, 'add'); // start at level 1
        }

        if (!$query->executeInsert()) {
            Tools::atkdebug('executeInsert failed..');
            return false;
        }

        // new primary key
        $record['atkprimkey'] = $this->primaryKey($record);

        if (!$this->_storeAttributes($storelist['post'], $record, $mode)) {
            Tools::atkdebug('_storeAttributes failed..');
            return false;
        }

        // Now we call a postAdd function, that can be used to do some processing after the record
        // has been saved.
        if ($exectrigger && !$this->executeTrigger('postAdd', $record, $mode)) {
            return false;
        }

        return true;
    }

    /**
     * Executes a trigger on a add,update or delete action.
     *
     * To prevent triggers from executing twice, the method stores an
     * indication in the record when a trigger is executed.
     * ('__executed<triggername>')
     *
     * @param string $trigger function, such as 'postUpdate'
     * @param array $record record on which action is performed
     * @param string|null $mode mode like add or update
     * @return bool true on case of success or when the trigger isn't returning anything (assumes success)
     */
    public function executeTrigger(string $trigger, array &$record, ?string $mode = null): bool
    {
        if (!isset($record['__executed' . $trigger])) {
            $record['__executed' . $trigger] = true;

            $return = $this->$trigger($record, $mode);

            if ($return === null) {
                Tools::atkdebug('Undefined return: ' . $this->atkNodeUri() . ".$trigger doesn't return anything, it should return a boolean!",
                    Tools::DEBUG_WARNING);
                $return = true;
            }

            if (!$return) {
                Tools::atkdebug($this->atkNodeUri() . ".$trigger failed!");
                return false;
            }

            for ($i = 0, $_i = Tools::count($this->m_triggerListeners); $i < $_i; ++$i) {
                $listener = $this->m_triggerListeners[$i];
                $return = $listener->notify($trigger, $record, $mode);

                if ($return === null) {
                    Tools::atkdebug('Undefined return: ' . $this->atkNodeUri() . ', ' . get_class($listener) . ".notify('$trigger', ...) doesn't return anything, it should return a boolean!",
                        Tools::DEBUG_WARNING);
                    $return = true;
                }

                if (!$return) {
                    Tools::atkdebug($this->atkNodeUri() . ', ' . get_class($listener) . ".notify('$trigger', ...) failed!");
                    return false;
                }
            }
        }

        return true;
    }

    /**
     * Delete record(s) from the database.
     *
     * After deletion, the postDel() trigger in the node method is called, and
     * on any attribute that has the Attribute::AF_CASCADE_DELETE flag set, the delete()
     * method is invoked.
     *
     * NOTE: Does not commit your transaction! If you are using a database that uses
     * transactions you will need to call 'Db::getInstance()->commit()' manually.
     *
     * @todo There's a discrepancy between updateDb, addDb and deleteDb:
     *       There should be a deleteDb which accepts a record, instead
     *       of a selector.
     *
     * @param string $selector SQL expression used as where-clause that
     *                              indicates which records to delete.
     * @param bool $exectrigger wether to execute the pre/post triggers
     * @param bool $failwhenempty determine whether to throw an error if there is nothing to delete
     * @returns boolean True if successful, false if not.
     */
    public function deleteDb(string $selector, bool $exectrigger = true, bool $failwhenempty = false): bool
    {
        $recordset = $this->select($selector)->mode('delete')->getAllRows();

        // nothing to delete, throw an error (determined by $failwhenempty)!
        if (Tools::count($recordset) == 0) {
            Tools::atkwarning($this->atkNodeUri() . "->deleteDb($selector): 0 records found, not deleting anything.");

            return !$failwhenempty;
        }

        if ($exectrigger) {
            for ($i = 0, $_i = Tools::count($recordset); $i < $_i; ++$i) {
                $return = $this->executeTrigger('preDelete', $recordset[$i]);
                if (!$return) {
                    return false;
                }
            }
        }

        // delete on "cascading" attributes (like relations, file attribute) BEFORE the query execution
        if (Tools::count($this->m_cascadingAttribs) > 0) {
            for ($i = 0, $_i = Tools::count($recordset); $i < $_i; ++$i) {
                for ($j = 0, $_j = Tools::count($this->m_cascadingAttribs); $j < $_j; ++$j) {
                    $p_attrib = $this->m_attribList[$this->m_cascadingAttribs[$j]];
                    if (isset($recordset[$i][$this->m_cascadingAttribs[$j]]) && !$p_attrib->isEmpty($recordset[$i])) {
                        if (!$p_attrib->delete($recordset[$i])) {
                            // error
                            return false;
                        }
                    }
                }
            }
        }

        $query = $this->getDb()->createQuery();
        $query->addTable($this->m_table);
        $query->addCondition($selector);
        if ($query->executeDelete()) {

            // postDelete on "cascading" attributes (like relations, file attribute) AFTER the query execution
            if (Tools::count($this->m_cascadingAttribs) > 0) {
                for ($i = 0, $_i = Tools::count($recordset); $i < $_i; ++$i) {
                    for ($j = 0, $_j = Tools::count($this->m_cascadingAttribs); $j < $_j; ++$j) {
                        $p_attrib = $this->m_attribList[$this->m_cascadingAttribs[$j]];
                        if (isset($recordset[$i][$this->m_cascadingAttribs[$j]]) && !$p_attrib->isEmpty($recordset[$i])) {
                            if (!$p_attrib->postDelete($recordset[$i])) {
                                // error
                                return false;
                            }
                        }
                    }
                }
            }

            if ($exectrigger) {
                for ($i = 0, $_i = Tools::count($recordset); $i < $_i; ++$i) {
                    $return = ($this->executeTrigger('postDel', $recordset[$i]) && $this->executeTrigger('postDelete', $recordset[$i]));
                    if (!$return) {
                        return false;
                    }
                }
            }

            return true;
        } else {
            return false;
        }
    }

    /**
     * Function that is called by the framework, right after a new record has
     * been saved to the database.
     *
     * This function does essentially nothing, but it can be overriden in
     * derived classes if you want to do something special after you saved a
     * record.
     *
     * @param array $record The record that has just been saved.
     * @param string $mode The 'mode' indicates whether the added record was a
     *                       completely new record ("add") or a copy ("copy").
     * @return bool True if succesful, false if not.
     */
    public function postAdd(array $record, string $mode = 'add'): bool
    {
        // Do nothing
        return true;
    }

    /**
     * Function that is called by the framework, just before a new record will
     * be saved to the database.
     *
     * This function does essentially nothing, but it can be overriden in
     * derived classes if you want to modify the record just before it will
     * be saved.
     *
     * @param array $record The record that will be saved to the database.
     * @param string $mode The 'mode' indicates whether the added record was a
     *                       completely new record ("add") or a copy ("copy").
     *
     * @return bool
     */
    public function preAdd(array &$record, string $mode = 'add'): bool
    {
        $this->storeNestedAttributesValue($record);
        return true;
    }

    /**
     * Function that is called by the framework, right after an existing
     * record has been updated in the database.
     *
     * This function does essentially nothing, but it can be overriden in
     * derived classes if you want to do something special after the record is
     * updated.
     *
     * If the self::NF_TRACK_CHANGES flag is present for the node, both the new
     * and the original record are passed to this method. The original
     * record is stored in the new record, in $record["atkorgrec"].
     *
     * @param array $record The record that has just been updated in the
     *                      database.
     * @return bool True if succesful, false if not.
     */
    public function postUpdate(array $record): bool
    {
        // Do nothing
        return true;
    }

    /**
     * Function that is called by the framework, just before an existing
     * record will be saved to the database.
     *
     * This function does essentially nothing, but it can be overriden in
     * derived classes if you want to modify the record just before it will
     * be saved.
     *
     * @param array $record The record that will be updated in the database.
     * @return bool Weather or not we succeeded in what we wanted to do.
     */
    public function preUpdate(array &$record): bool
    {
        $this->storeNestedAttributesValue($record);
        return true;
    }

    /**
     * Function that is called by the framework, right before a record will be
     * deleted. Should this method return false the deleting will halt.
     *
     * This function does essentially nothing, but it can be overriden in
     * derived classes if you want to do something special after a record is
     * deleted.
     *
     * If this function returns false the delete action will not continue.
     *
     * @param array $record The record that will be deleted.
     *
     * @return bool True if succesful, false if not.
     */
    public function preDelete(array $record): bool
    {
        return true;
    }

    /**
     * Deprecated function that is called by the framework,
     * right after a record has been deleted.
     * Please use postDelete() instead.
     *
     * @param array $record The record that has just been deleted.
     * @return bool Weather or not we succeeded in what we wanted to do.
     */
    public function postDel(array $record): bool
    {
        // Do nothing
        return true;
    }

    /**
     * Function that is called by the framework, right after a record has been
     * deleted.
     *
     * This function does essentially nothing, but it can be overriden in
     * derived classes if you want to do something special after a record is
     * deleted.
     *
     * @param array $record The record that has just been deleted.
     * @return bool Weather or not we succeeded in what we wanted to do.
     */
    public function postDelete(array $record): bool
    {
        // Do nothing
        return true;
    }

    /**
     * Function that is called by the framework, right before a copied record
     * is stored to the database.
     *
     * This function does nothing, but it can be overriden in derived classes
     * if you want to do some processing on a record before it is
     * being copied.
     * Typical usage would be: Suppose you have a field named 'title' in a
     * record. In the preCopy method, you could change the title field of the
     * record to 'Copy of ..', so the user can distinguish between the
     * original and the copy.
     *
     * @param array $record A reference to the copied record. You can change the
     *                      contents of the record, since it is passed by
     *                      reference.
     */
    public function preCopy(array &$record): void
    {
    }

    /**
     * Function that is called for each record in a recordlist, to determine
     * what actions may be performed on the record.
     *
     * This function does nothing, but it can be overriden in derived classes,
     * to make custom actions for certain records.
     * The array with actions (edit, delete, etc.) is passed to the function
     * and can be modified.
     * To create a new action, just do $actions["new_action"]=$url;
     * in the derived function.
     * To disable existing actions, for example the edit action, for a record,
     * use: unset($actions["edit"]);
     *
     * @param array $record The record for which the actions need to be
     *                           determined.
     * @param array &$actions Reference to an array with the already defined
     *                           actions. This is an associative array with the action
     *                           identifier as key, and an url as value. Actions can be
     *                           removed from it, or added to the array.
     * @param array &$mraactions List of multirecordactions that are supported for
     *                           the passed record.
     */
    public function recordActions(array $record, array &$actions, array &$mraactions): void
    {
        // Do nothing.
    }

    /**
     * Registers a function/method that is called for each record in a recordlist,
     * to determine what actions may be performed on the record.
     *
     * The callback receives the record, a reference to the record actions and
     * a reference to the MRA actions as arguments.
     */
    public function registerRecordActionsCallback($callback): void
    {
        if (is_callable($callback, false, $callableName)) {
            if (is_array($callback)) {
                if (!method_exists($callback[0], $callback[1])) {
                    Tools::atkerror("The registered record actions callback method '$callableName' doesn't exist");

                    return;
                }
            }
            $this->m_recordActionsCallbacks[] = $callback;
        } else {
            Tools::atkerror("The registered record actions callback '$callableName' is not callable");

            return;
        }
    }

    /**
     * Function that is called for each record in a recordlist, to determine
     * what actions may be performed on the record.
     *
     * This function is a framework method and should not be called directly.
     * It should not be overridden either.
     *
     * To change the record actions, either override self::recordActions() in you node,
     * or call self::registerRecordActionsCallback to register a callback.
     *
     * @param array $record The record for which the actions need to be
     *                           determined.
     * @param array &$actions Reference to an array with the already defined
     *                           actions. This is an associative array with the action
     *                           identifier as key, and an url as value. Actions can be
     *                           removed from it, or added to the array.
     * @param array &$mraactions List of multirecordactions that are supported for
     *                           the passed record.
     */
    public function collectRecordActions(array $record, array &$actions, array &$mraactions): void
    {
        $this->recordActions($record, $actions, $mraactions);

        foreach ($this->m_recordActionsCallbacks as $callback) {
            call_user_func_array($callback, array($record, &$actions, &$mraactions));
        }
    }

    /**
     * Retrieve the security key of an action.
     *
     * Returns the privilege required to perform a certain action.
     * Usually, the privilege and the action are equal, but in m_securityMap,
     * aliasses may be defined.
     *
     * @param string $action The action for which you want to determine the
     *                       privilege.
     *
     * @return string The security privilege required to perform the action.
     */
    public function securityKey(string $action): string
    {
        if (!isset($this->m_securityMap[$action])) {
            return $action;
        }

        return $this->m_securityMap[$action];
    }

    /**
     * Returns the type of this node.  (This is *not* the full ATK node type;
     * see atkNodeUri() for the full node type.).
     *
     * @return string type
     */
    public function getType(): string
    {
        return $this->m_type;
    }

    /**
     * Returns the module name for this node.
     *
     * @return string node
     */
    public function getModule(): string
    {
        return $this->m_module;
    }

    /**
     * Returns the current action for this node.
     *
     * @return string|null action
     */
    public function getAction(): ?string
    {
        return $this->m_action;
    }

    /**
     * Get the full node Uri of this node (module.type notation).  This is sometimes
     * referred to as the node name (or nodename) or node string.
     *
     * @return string The nodeUri of the node.
     */
    public function atkNodeUri(): string
    {
        return (empty($this->m_module) ? '' : $this->m_module . '.') . $this->m_type;
    }

    /**
     * This function determines if the user has the privilege to perform a certain
     * action on the node.
     *
     * @param string $action The action to be checked.
     * @param array $record The record on which the action is to be performed.
     *                       The standard implementation ignores this
     *                       parameter, but derived classes may override this
     *                       method to implement their own record based
     *                       security policy. Keep in mind that a record is not
     *                       passed in every occasion. The method is called
     *                       several times without a record, to just see if
     *                       the user has the privilege for the action
     *                       regardless of the record being processed.
     *
     * @return bool True if the action may be performed, false if not.
     */
    public function allowed(string $action, array $record = array()): bool
    {
        $secMgr = SecurityManager::getInstance();

        // shortcut, admins can do everything
        if ($secMgr::isUserAdmin()) {
            return true;
        }

        $alias = $this->atkNodeUri();
        $this->resolveNodeTypeAndAction($alias, $action);

        return $this->hasFlag(self::NF_NO_SECURITY) || in_array($action, $this->m_unsecuredActions) || $secMgr->allowed($alias,
                $action) || (isset($this->m_securityImplied[$action]) && $secMgr->allowed($alias, $this->m_securityImplied[$action]));
    }

    /**
     * Resolves a possible node / action alias for the given node / action.
     * The given node alias and action are updated depending on
     * the found mapping.
     *
     * @param string $alias node type
     * @param string $action action name
     */
    public function resolveNodeTypeAndAction(string &$alias, string &$action): void
    {
        if (!empty($this->m_securityAlias)) {
            $alias = $this->m_securityAlias;
        }

        // Resolve action
        $action = $this->securityKey($action);

        // If action contains a dot, it's a complete nodename.action or modulename.nodename.action alias.
        // Else, it's only an action alias, and we use the default node.

        if (strpos($action, '.') !== false) {
            $complete = explode('.', $action);
            if (Tools::count($complete) == 3) {
                $alias = $complete[0] . '.' . $complete[1];
                $action = $complete[2];
            } else {
                $alias = $this->m_module . '.' . $complete[0];
                $action = $complete[1];
            }
        }
    }

    /**
     * Set the security alias of a node.
     *
     * By default a node has it's own set of privileges. With this method,
     * the privileges of another node can be used. This is useful when you
     * have a master/detail relationship, and people may manipulate details
     * when they have privileges on the master node.
     * Note: When setting an alias for the node, the node no longer has to
     * have a registerNode call in the getNodes method in module.inc.
     *
     * @param string $alias The node (module.nodename) to set as a security
     *                      alias for this node.
     */
    public function setSecurityAlias(string $alias): void
    {
        $this->m_securityAlias = $alias;
    }

    /**
     * Returns the node's security alias (if set).
     *
     * @return string security alias
     */
    public function getSecurityAlias(): string
    {
        return $this->m_securityAlias;
    }

    /**
     * Disable privilege checking for an action.
     *
     * This method disables privilege checks for the specified action, for the
     * duration of the current http request.
     *
     * @param string $action The name of the action for which security is
     *                       disabled.
     */
    public function addAllowedAction(string $action): void
    {
        if (is_array($action)) {
            $this->m_unsecuredActions = Tools::atk_array_merge($this->m_unsecuredActions, $action);
        } else {
            $this->m_unsecuredActions[] = $action;
        }
    }

    /**
     * Invoke the handler for an action.
     *
     * If there is a known registered external handler method for the
     * specified action, this method will call it. If there is no custom
     * external handler, the atkActionHandler object is determined and the
     * actionis invoked on the actionhandler.
     *
     * @param string $action the node action
     */
    public function callHandler(string $action): void
    {
        Tools::atkdebug('self::callHandler(); action: ' . $action);
        $atk = Atk::getInstance();
        $handler = $atk->atkGetNodeHandler($this->atkNodeUri(), $action);

        // handler function
        if ($handler != null && is_string($handler) && function_exists($handler)) {
            Tools::atkdebug("self::callHandler: Calling external handler function for '" . $action . "'");
            $handler($this, $action);
        } // handler object
        elseif ($handler != null && $handler instanceof ActionHandler) {
            Tools::atkdebug('self::callHandler:Using override/existing ActionHandler ' . get_class($handler) . " class for '" . $action . "'");
            $handler->handle($this, $action, $this->m_postvars);
        } // no (valid) handler
        else {
            Tools::atkdebug("Calling default handler function for '" . $action . "'");
            $this->m_handler = $this->getHandler($action);
            $this->m_handler->handle($this, $action, $this->m_postvars);
        }
    }

    /**
     * Get the atkActionHandler object for a certain action.
     *
     * The default implementation returns a default handler for the action,
     * but derived classes may override this to return a custom handler.
     *
     * @param string $action The action for which the handler is retrieved.
     *
     * @return ActionHandler The action handler.
     */
    public function getHandler(string $action): ActionHandler
    {
        Tools::atkdebug('self::getHandler(); action: ' . $action);

        //check if a handler exists registered including the module name
        $atk = Atk::getInstance();
        $handler = $atk->atkGetNodeHandler($this->atkNodeUri(), $action);

        // The node handler might return a class, then we need to instantiate the handler
        if (is_string($handler) && class_exists($handler)) {
            $handler = new $handler();
        }

        // The node handler might return a function as nodehandler. We cannot
        // return a function so we ignore this option.
        //       this would probably only work fine when using PHP5, but's better then nothing?
        //       or why support functions at all?!
        // handler object
        if ($handler != null && is_subclass_of($handler, 'ActionHandler')) {
            Tools::atkdebug('self::getHandler: Using existing ActionHandler ' . get_class($handler) . " class for '" . $action . "'");
            $handler->setNode($this);
            $handler->setAction($action);
        } else {
            $handler = ActionHandler::getDefaultHandler($action);

            $handler->setNode($this);
            $handler->setPostvars($this->m_postvars);
            $handler->setAction($action);

            //If we use a default handler we need to register it to this node
            //because we might call it a second time.
            Tools::atkdebug('self::getHandler: Register default ActionHandler for ' . $this->m_type . " action: '" . $action . "'");
            $atk->atkRegisterNodeHandler($this->m_type, $action, $handler);
        }

        return $handler;
    }

    /**
     * Sets the search action.
     *
     * The search action is the action that will be performed
     * if only a single record is found after doing a certain search query.
     *
     * You can specify more then 1 action. If the user isn't allowed to
     * execute the 1st action, the 2nd action will be used, etc. If you
     * want to pass multiple actions, just pass multiple params (function
     * has a variable number of arguments).
     *
     * @todo Using func_get_args is non-standard. It's cleaner to accept an
     *       array.
     *
     */
    public function setSearchAction(): void
    {
        $this->m_search_action = func_get_args();
    }

    /**
     * This function resorts the attribIndexList and attribList.
     *
     * This is necessary if you add attributes *after* init() is already
     * called, and you set an order for those attributes.
     */
    public function attribSort(): void
    {
        usort($this->m_attribIndexList, [$this, 'attrib_cmp']);

        // after sorting we need to update the attribute indices
        $attrs = [];
        foreach ($this->m_attribIndexList as $index => $info) {
            $attr = $this->getAttribute($info['name']);
            $attr->m_index = $index;
            $attrs[$info['name']] = $attr;
        }

        $this->m_attribList = $attrs;
    }

    /**
     * Search all records for the occurance of a certain expression.
     *
     * This function searches in all fields that are not Attribute::AF_HIDE_SEARCH, for
     * a certain expression (substring match). The search performed is an
     * 'or' search. If any of the fields contains the expression, the record
     * is added to the resultset.\
     *
     * Currently, searchDb only searches those attributes that are of type
     * string or text.
     *
     * @param string $expression The keyword to search for.
     * @param string $searchmethod
     *
     * @return array Set of records matching the keyword.
     */
    public function searchDb(string $expression, string $searchmethod = 'OR'): array
    {
        // Set default searchmethod to OR (put it in m_postvars, because selectDb
        // will use m_postvars to built it's search conditions).
        $this->m_postvars['atksearchmethod'] = $searchmethod;

        // To perform the search, we fill atksearch, so selectDb automatically
        // searches. Because an atksearch variable may have already been set,
        // we save it to restore it after the query.
        $orgsearch = Tools::atkArrayNvl($this->m_postvars, 'atksearch');

        // Built whereclause.
        foreach (array_keys($this->m_attribList) as $attribname) {
            $p_attrib = $this->m_attribList[$attribname];
            // Only search in fields that aren't explicitly hidden from search
            if (!$p_attrib->hasFlag(Attribute::AF_HIDE_SEARCH) && (in_array($p_attrib->dbFieldType(),
                        array('string', 'text')) || $p_attrib->hasFlag(Attribute::AF_SEARCHABLE))
            ) {
                $this->m_postvars['atksearch'][$attribname] = $expression;
            }
        }

        // We load records in admin mode, se we are certain that all fields are added.
        $recs = $this->select()->excludes($this->m_listExcludes)->mode('admin')->getAllRows();

        // Restore original atksearch
        $this->m_postvars['atksearch'] = $orgsearch;

        return $recs;
    }

    /**
     * Determine the url for the feedbackpage.
     *
     * Output is dependent on the feedback configuration. If feedback is not
     * enabled for the action, this method returns an empty string, so the
     * result of this method can be passed directly to the redirect() method
     * after completing the action.
     *
     * The $record parameter is ignored by the default implementation, but
     * derived classes may override this method to perform record-specific
     * feedback.
     *
     * @param string $action The action that was performed
     * @param int $status The status of the action.
     * @param array $record The record on which the action was performed.
     * @param string $message An optional message to pass to the feedbackpage,
     *                          for example to explain the reason why an action
     *                          failed.
     * @param int|null $levelskip Number of levels to skip
     * @return string The feedback url.
     */
    public function feedbackUrl(string $action, int $status, array $record = [], string $message = '', int $levelskip = null): string
    {
        $sm = SessionManager::getInstance();
        $vars = [];
        $atkNodeUri = '';
        $sessionStatus = SessionManager::SESSION_BACK;

        if ((isset($this->m_feedback[$action]) && Tools::hasFlag($this->m_feedback[$action], $status)) || $status == ActionHandler::ACTION_FAILED) {
            $vars = [
                'atkaction' => 'feedback',
                'atkfbaction' => $action,
                'atkactionstatus' => $status,
                'atkfbmessage' => $message,
            ];
            $atkNodeUri = $this->atkNodeUri();
            $sessionStatus = SessionManager::SESSION_REPLACE;

            // The level skip given is based on where we should end up after the
            // feedback action is shown to the user. This means that the feedback
            // action should be shown one level higher in the stack, hence the -1.
            // Default the feedback action is shown on the current level, so in that
            // case we have a simple SessionManager::SESSION_REPLACE with a level skip of null.
            $levelskip = $levelskip == null ? null : $levelskip - 1;
        }

        $dispatch_url = Tools::dispatch_url($atkNodeUri, Tools::atkArrayNvl($vars, 'atkaction', ''), $vars);

        return $sm->sessionUrl($dispatch_url, $sessionStatus, $levelskip);
    }

    /**
     * Sets numbering of the attributes to begin with the number that was passed to it,
     * or defaults to 1.
     *
     * @param mixed|int $number the number that the first attribute begins with
     */
    public function setNumbering(mixed $number = 1): void
    {
        $this->m_numbering = $number;
    }

    /**
     * Gets the numbering of the attributes.
     *
     * @return mixed the number whith which the numbering starts
     */
    public function getNumbering(): mixed
    {
        return $this->m_numbering;
    }

    /**
     * Set the security of one or more actions action the same as other actions.
     * If $mapped is empty $action has to be an array. The key would be used as action and would be mapped to the value.
     * If $mapped is not empty $action kan be a string containing one action of an array with one or more action. In both
     * cases al actions would be mapped to $mappped.
     *
     * @param mixed $action The action that has to be mapped
     * @param string $mapped The action on witch $action has to be mapped
     */
    public function addSecurityMap(mixed $action, string $mapped = ''): void
    {
        if ($mapped != '') {
            if (!is_array($action)) {
                $this->m_securityMap[$action] = $mapped;
                $this->changeMapping($action, $mapped);
            } else {
                foreach ($action as $value) {
                    $this->m_securityMap[$value] = $mapped;
                    $this->changeMapping($value, $mapped);
                }
            }
        } else {
            if (is_array($action)) {
                foreach ($action as $key => $value) {
                    $this->m_securityMap[$key] = $value;
                    $this->changeMapping($key, $value);
                }
            }
        }
    }

    /**
     * change the securitymap that already exist. Where actions are mapped on $oldmapped change it by $newmapped.
     *
     * @param string $oldmapped the old value
     * @param string $newmapped the new value with replace the old one
     */
    public function changeMapping(string $oldmapped, string $newmapped): void
    {
        foreach ($this->m_securityMap as $key => $value) {
            if ($value == $oldmapped) {
                $this->m_securityMap[$key] = $newmapped;
            }
        }
    }

    /**
     * Add an atkActionListener to the node.
     *
     * @param ActionListener $listener
     */
    public function addListener(ActionListener $listener): void
    {
        $listener->setNode($this);

        if (is_a($listener, 'ActionListener')) {
            $this->m_actionListeners[] = $listener;
        } else {
            if (is_a($listener, 'TriggerListener')) {
                $this->m_triggerListeners[] = $listener;
            } else {
                Tools::atkdebug('self::addListener: Unknown listener base class ' . get_class($listener));
            }
        }
    }

    /**
     * Notify all listeners of the occurance of a certain action.
     *
     * @param string $action The action that occurred
     * @param array $record The record on which the action was performed
     */
    public function notify(string $action, array $record): void
    {
        for ($i = 0, $_i = Tools::count($this->m_actionListeners); $i < $_i; ++$i) {
            $this->m_actionListeners[$i]->notify($action, $record);
        }
    }

    /**
     * Notify all listeners in advance of the occurance of a certain action.
     *
     * @param string $action The action that will occur
     * @param array $record The record on which the action will be performed
     */
    public function preNotify(string $action, array &$record): void
    {
        for ($i = 0, $_i = Tools::count($this->m_actionListeners); $i < $_i; ++$i) {
            $this->m_actionListeners[$i]->preNotify($action, $record);
        }
    }

    /**
     * Get the column configuration object.
     *
     * @param string|null $id optional column config id
     * @param bool $forceNew force new instance?
     * @return ColumnConfig
     */
    public function getColumnConfig(string $id = null, bool $forceNew = false): ColumnConfig
    {
        return ColumnConfig::getConfig($this, $id, $forceNew);
    }

    /**
     * Translate using this node's module and type.
     *
     * @param string|string[] $string string or array of strings containing the name(s) of the string to return
     *                              when an array of strings is passed, the second will be the fallback if
     *                              the first one isn't found, and so forth
     * @param string|null $module module in which the language file should be looked for,
     *                              defaults to core module with fallback to ATK
     * @param string $lng ISO 639-1 language code, defaults to config variable
     * @param string $firstfallback the first module to check as part of the fallback
     * @param bool $nodefaulttext if true, then it doesn't return a default text
     *                              when it can't find a translation
     * @return string the string from the languagefile
     */
    public function text(array|string $string, string $module = null, string $lng = '', string $firstfallback = '', bool $nodefaulttext = false): string
    {
        if ($module === null) {
            $module = $this->m_module;
        }

        return Tools::atktext($string, $module, $this->m_type, $lng, $firstfallback, $nodefaulttext);
    }

    /**
     * String representation for this node (PHP5 only).
     *
     * @return string ATK node type
     */
    public function __toString(): string
    {
        return $this->atkNodeUri();
    }

    /**
     * Set the edit fieldprefix to use in atk.
     *
     * @param string $prefix
     */
    public function setEditFieldPrefix(string $prefix): void
    {
        $this->m_edit_fieldprefix = $prefix;
    }

    /**
     * Get the edit fieldprefix to use.
     *
     * @param bool $atk_layout do we want the prefix in atkstyle (with _AE_) or not
     * @return string with edit fieldprefix
     */
    public function getEditFieldPrefix(bool $atk_layout = true): string
    {
        if ($this->m_edit_fieldprefix == '') {
            return '';
        } else {
            return $this->m_edit_fieldprefix . ($atk_layout ? '_AE_' : '');
        }
    }

    /**
     * Escape SQL string, uses the node's database to do the escaping.
     *
     * @param string $string string to escape
     * @return string escaped string
     */
    public function escapeSQL(string $string): string
    {
        return $this->getDb()->escapeSQL($string);
    }

    /**
     * Should always be called from in child methods!
     * The method can return a simple value (which will be used for the normal row color), or can be
     * an array, in which case the first element will be the normal row color, and the second the mouseover
     * row color, example: function rowColor($record) { return ['#f00', '#00f'] };
     *
     * @param array $record
     * @return string
     */
    public function rowColor(array $record): string
    {
        return self::DEFAULT_RECORDLIST_BG_COLOR;
    }


    /**
     * On child nodes should be called before calling the parent constructor
     * @param array $record
     * @param int $index
     * @return string|null
     */
    public function recordStateColor(array $record, int $index = 0): ?string
    {
        if ($this->rowColorConditions) {
            $recordDisabled = false;
            if (isset($record['disabled']) && ($record['disabled'] === 1 || $record['disabled'] === 't')) {
                $recordDisabled = true;
            }
            if (isset($record['_disabled']) && ($record['_disabled'] === 1 || $record['_disabled'] === 't')) {
                $recordDisabled = true;
            }
            if ($recordDisabled) {
                return UIStateColors::STATE_DISABLED;
            }
            foreach ($this->rowColorConditions as $uiState => $callback) {
                if (call_user_func($callback, $record)) {
                    return $uiState;
                }
            }
        }

        return null;
    }

    public function recordHexColor(array $record, int $index = 0): ?string
    {
        if ($state = $this->recordStateColor($record, $index)) {
            return UIStateColors::getHexRList($state);
        }

        return $this->rowColor($record) ?: self::DEFAULT_RECORDLIST_BG_COLOR;
    }

    /**
     * Row CSS class.
     *
     * Used to determine the CSS class(s) for rows in the datagrid list.
     *
     * @param array $record record
     * @param int $nr row number
     * @return string CSS class(es)
     */
    public function rowClass(array $record, int $nr): string
    {
        return $nr % 2 == 0 ? 'row1' : 'row2';
    }

    /**
     * Add callback function for add css class to row.
     *
     * @param mixed $callback name of a function or array with an object
     *                        and the name of a method or closure
     *
     * @return bool
     * @throws Exception
     */
    public function setRowClassCallback(mixed $callback): bool
    {
        $res = false;
        if (is_callable($callback, false, $callableName)) {
            if (is_array($callback) && !method_exists($callback[0], $callback[1])) {
                Tools::atkerror("The registered row class callback method '$callableName' doesn't exist");
            } else {
                $this->m_rowClassCallback[] = $callback;
                $res = true;
            }
        } else {
            if (is_array($callback)) {
                if (!method_exists($callback[0], $callback[1])) {
                    Tools::atkerror("The registered row class callback method '$callableName' doesn't exist");
                }
            }
            Tools::atkerror("The registered row class callback '$callableName' is not callable");
        }

        return $res;
    }

    /**
     * Return array with callback function list, which use for add css class to row.
     *
     * @return array
     */
    public function getRowClassCallback(): array
    {
        return $this->m_rowClassCallback;
    }

    /**
     * Adds a flag to a list of attributes.
     *
     * @param string[] $attrsNames The names of attributes
     * @param int $flag The flag to add to the attributes
     * @param bool $check Check the presence of the attributes
     * @return Node
     */
    public function addAttributesFlag(array $attrsNames, int $flag, bool $check = true): self
    {
        foreach ($attrsNames as $name) {
            $attr = $this->getAttribute($name);
            if (!$check || $attr) {
                $attr->addFlag($flag);
            }
        }
        return $this;
    }

    /**
     * Adds all passed flags to a list of attributes.
     *
     * @param string[] $attrsNames
     * @param int[] $flags
     * @param bool $check
     * @return Node
     */
    protected function addAttributesFlags(array $attrsNames, array $flags, bool $check = true): self
    {
        foreach ($flags as $flag) {
            $this->addAttributesFlag($attrsNames, $flag, $check);
        }
        return $this;
    }

    /**
     * Removes a flag from a list of attributes.
     *
     * @param string[] $attrsNames The names of attributes
     * @param int $flag The flag to remove from the attributes
     * @param bool $check Check the presence of the attributes
     * @return Node
     */
    public function removeAttributesFlag(array $attrsNames, int $flag, bool $check = false): self
    {
        foreach ($attrsNames as $name) {
            $attr = $this->getAttribute($name);
            if (!$check || $attr) {
                $attr->removeFlag($flag);
            }
        }
        return $this;
    }

    /**
     * Removes all passed flags from a list of attributes.
     *
     * @param string[] $attrsNames
     * @param int[] $flags
     * @param bool $check
     * @return Node
     */
    protected function removeAttributesFlags(array $attrsNames, array $flags, bool $check = false): self
    {
        foreach ($flags as $flag) {
            $this->removeAttributesFlag($attrsNames, $flag, $check);
        }
        return $this;
    }

    /**
     * @return bool
     */
    public function isRecordListHover(): bool
    {
        return $this->recordListHover;
    }

    /**
     * @param bool $recordListHover
     * @return Node
     */
    public function setRecordListHover(bool $recordListHover): self
    {
        $this->recordListHover = $recordListHover;
        return $this;
    }

    public function atkReadOptimizer(): bool
    {
        return false;
    }

    /**
     * Adds color for all rows that satisfy the specified condition.
     * Used as a callback function.
     * Example on a node: $this->addColorCondition(UIStateColors::STATE_SUCCESS, function($record){
     *                      return $record['id'] === 1;
     *                      });
     * In PHP >= 7.4 you can use this with an arrow function if you want.
     * @param string $uiState
     * @param $callback
     */
    public function addColorCondition(string $uiState, $callback): void
    {
        $this->rowColorConditions[$uiState] = $callback;
    }


    /**
     * @return string
     */
    public function getRowColorMode(): string
    {
        return $this->rowColorMode;
    }

    /**
     * @param string $rowColorMode
     * @return Node
     */
    public function setRowColorMode(string $rowColorMode): self
    {
        if ($rowColorMode === self::ROW_COLOR_MODE_CELL) {
            $this->getAttribute(self::ROW_COLOR_ATTRIBUTE)->removeFlag(Attribute::AF_HIDE);
            $this->getAttribute(self::ROW_COLOR_ATTRIBUTE)->setInitialHidden(false);
        } else {
            $this->getAttribute(self::ROW_COLOR_ATTRIBUTE)->addFlag(Attribute::AF_HIDE);
            $this->getAttribute(self::ROW_COLOR_ATTRIBUTE)->setInitialHidden(true);
        }

        $this->rowColorMode = $rowColorMode;
        return $this;
    }

    public function getSubmitBtnAttribList(): array
    {
        return $this->submitBtnAttribList;
    }

    public function addSubmitBtnAttrib(string $buttonName): self
    {
        $this->submitBtnAttribList[] = $buttonName;
        return $this;
    }

    public function isSubmitBtnClicked(string $buttonName): bool
    {
        return in_array($buttonName, $this->submitBtnAttribList) and isset($this->m_postvars[$buttonName]) and $this->m_postvars[$buttonName];
    }

    public function showAttribute(EditFormModifier $modifier, string $attrName, string $prefix = ''): void
    {
        $this->setAttributeVisibility($modifier, $attrName, $prefix, 'show');
    }

    public function showAttributes(EditFormModifier $modifier, array $attrNames, $prefix = ''): void
    {
        foreach ($attrNames as $attrName) {
            $this->showAttribute($modifier, $attrName, $prefix);
        }
    }

    public function showAttributeTabbed(EditFormModifier $modifier, string $attrName): void
    {
        $this->showAttribute($modifier, $attrName, self::PREFIX_TABBED_PANE);
    }

    public function showAttributesTabbed(EditFormModifier $modifier, array $attrNames): void
    {
        $this->showAttributes($modifier, $attrNames, self::PREFIX_TABBED_PANE);
    }

    public function showAttributeFieldSet(EditFormModifier $modifier, string $attrName): void
    {
        $this->showAttribute($modifier, $attrName, self::PREFIX_FIELDSET);
    }

    public function showAttributesFieldSet(EditFormModifier $modifier, array $attrNames): void
    {
        $this->showAttributes($modifier, $attrNames, self::PREFIX_FIELDSET);
    }

    public function hideAttribute(EditFormModifier $modifier, string $attrName, $prefix = ''): void
    {
        $this->setAttributeVisibility($modifier, $attrName, $prefix, 'hide');
    }

    public function hideAttributes(EditFormModifier $modifier, array $attrNames, $prefix = ''): void
    {
        foreach ($attrNames as $attrName) {
            $this->hideAttribute($modifier, $attrName, $prefix);
        }
    }

    public function hideAttributeTabbed(EditFormModifier $modifier, string $attrName): void
    {
        $this->hideAttribute($modifier, $attrName, self::PREFIX_TABBED_PANE);
    }

    public function hideAttributesTabbed(EditFormModifier $modifier, array $attrNames): void
    {
        $this->hideAttributes($modifier, $attrNames, self::PREFIX_TABBED_PANE);
    }

    public function hideAttributeFieldSet(EditFormModifier $modifier, string $attrName): void
    {
        $this->hideAttribute($modifier, $attrName, self::PREFIX_FIELDSET);
    }

    public function hideAttributesFieldSet(EditFormModifier $modifier, array $attrNames): void
    {
        $this->hideAttributes($modifier, $attrNames, self::PREFIX_FIELDSET);
    }

    /**
     * Mostra o nasconde un attributo del form.
     *
     * @param EditFormModifier $modifier
     * @param string $attrName Nome dell'attributo
     * @param string $prefix Valori possibili: ar|tabbedPaneAttr|fieldset
     * @param string $visibility Valori possibili: show|hide
     */
    private function setAttributeVisibility(EditFormModifier $modifier, string $attrName, string $prefix = '', string $visibility = 'show'): void
    {
        if (!$attr = $this->getAttribute($attrName)) {
            return;
        }

        if ($prefix === self::PREFIX_FIELDSET) {
            $prefix = str_replace('.', '_', $modifier->getNode()->atkNodeUri());
        }

        $defaultPrefixes = [self::PREFIX_DEFAULT, self::PREFIX_TABBED_PANE];
        $prefixes = $prefix ? [$prefix] : $defaultPrefixes;

        foreach ($prefixes as $prefix) {
            $rowId = $prefix . '_' . $attr->getHtmlId($modifier->getFieldPrefix());

            if (!in_array($prefix, $defaultPrefixes)) {
                // prefisso non di default
                if ($visibility === 'hide') {
                    $code = "jQuery('#$rowId').parent().addClass('atkAttrRowHidden');"; // devo nascondere il padre
                } else {
                    $code = "jQuery('#$rowId').parent().removeClass('atkAttrRowHidden');"; // devo mostrare il padre
                }

            } else {
                if ($visibility === 'hide') {
                    $code = "try {ATK.Tools.hideAttribute('$rowId');} catch (exception) {/*noop*/}";
                } else {
                    $code = "try {ATK.Tools.showAttribute('$rowId');} catch (exception) {/*noop*/}";
                }
            }

            $modifier->scriptCode($code);
        }
    }

//    public function getNestedAttributeField(): string
//    {
//        return $this->nestedAttributeField;
//    }

//    public function setNestedAttributeField(string $nestedAttributeField): self
//    {
//        $this->nestedAttributeField = $nestedAttributeField;
//        return $this;
//    }

    public function getNestedAttributesList(): array
    {
        return $this->nestedAttributesList;
    }

    public function addNestedAttribute(string $attributeName, string $nestedAttributeField): self
    {
        $this->nestedAttributesList[$nestedAttributeField][] = $attributeName;
        return $this;
    }

    public function hasNestedAttributes(): bool
    {
        return count($this->nestedAttributesList) > 0;
    }

    public function hasNestedAttribute(string $attributeName, string $nestedAttributeField): bool
    {
        return in_array($attributeName, $this->nestedAttributesList[$nestedAttributeField]);
    }

    /**
     * Load the value of all nested attributes
     */
    public function loadNestedAttributesValue(array &$record): void
    {
        if (!$this->hasNestedAttributes()) {
            return;
        }

        $nestedAttributeFields = array_keys($this->nestedAttributesList);

        foreach ($nestedAttributeFields as $nestedAttributeField) {
            if (isset($record[$nestedAttributeField]) && !is_array($record[$nestedAttributeField])) {
                // decodifico il contenuto del JSON
                $record[$nestedAttributeField] = json_decode($record[$nestedAttributeField], true);
            }

            $nestedAttributes = $this->nestedAttributesList[$nestedAttributeField];

            foreach ($nestedAttributes as $attribute) {
                // set the value for each nested attribute
                // TODO: check
                $default = null;
                $list = ['ManyToOneAttribute', 'OneToManyAttribute'];

                if (in_array($this->getAttribute($attribute)->get_class_name(), $list)) {
                    $default = []; // necessario per il default di quelle classi che si aspettano un array (Relations, etc)
                }

                $values = [];
                $values[$attribute] = $record[$nestedAttributeField][$attribute] ?? $default;
                $record[$attribute] = $this->getAttribute($attribute)->db2value($values);
            }
        }
    }

    /**
     * Store the value of all the nested attributes
     */
    protected function storeNestedAttributesValue(array &$record): void
    {
        if (!$this->hasNestedAttributes()) {
            return;
        }

        $nestedAttributeFields = array_keys($this->nestedAttributesList);

        foreach ($nestedAttributeFields as $nestedAttributeField) {
            if (!is_array($record[$nestedAttributeField])) {
                $record[$nestedAttributeField] = json_decode($record[$nestedAttributeField], true);
            }

            $nestedAttributes = $this->nestedAttributesList[$nestedAttributeField];

            foreach ($nestedAttributes as $attribute) {
                $record[$nestedAttributeField][$attribute] = $this->getAttribute($attribute)->value2db([$attribute => $record[$attribute]]);
            }
        }
    }

    /**
     * Ask the user a confirmation before proceeding with the action.
     *
     * @param ActionHandler $handler
     * @param string[]|null $atkSelectors
     * @param string|null $cancelUrl It goes to this url if the user has cancelled the action
     * @return bool True if the user has confirmed the action.
     * @throws \Smarty\Exception
     */
    protected function checkConfirmAction(ActionHandler $handler, ?array $atkSelectors = null, ?string $cancelUrl = null): bool
    {
        $confirm = $this->m_postvars['confirm'] ?? false;
        $cancel = $this->m_postvars['cancel'] ?? false;
        $atkCancel = $this->m_postvars['atkcancel'] ?? false;

        if (!$confirm && !$cancel && !$atkCancel) {
            if (!$atkSelectors) {
                $atkSelectors = $this->m_postvars[self::PARAM_ATKSELECTOR] ?? '';
            }
            $this->getPage()->addContent($this->renderActionPage(
                $handler->m_action, [$this->confirmAction($atkSelectors, $handler->m_action)]) // show confirm buttons
            );

            return false;
        }

        if ($cancel) {
            // the user has cancelled the action.
            if ($cancelUrl) {
                $this->redirect($cancelUrl);
            } else {
                $this->redirect();
            }
            return false;
        }

        return true;
    }

    /**
     * Check if at least one of the attribute has been modified before saving.
     *
     * @param array $record
     * @param string[] $attributeNames
     * @return bool
     */
    protected function areAttributesModified(array $record, array $attributeNames): bool
    {
        foreach ($attributeNames as $attribute) {
            if ($this->isAttributeModified($record, $attribute)) {
                // at least one of the attribute has been modified
                return true;
            }
        }

        return false;
    }

    /**
     * Check if the attribute is present on the current node and if its TRACK_CHANGES flag has been set.
     */
    protected function isAttributeTrackChangesAvailable(string $attributeName): bool
    {
        return $this->getAttribute($attributeName) and $this->hasFlag(self::NF_TRACK_CHANGES);
    }

    /**
     * Check if the attribute has been modified before saving it.
     *
     * @param array $record
     * @param string $attributeName
     * @param string[] $checkMtoPrimaryKeys More $keys that can be specified in case of ManyToManyRelations
     * @return bool
     */
    protected function isAttributeModified(array $record, string $attributeName, array $checkMtoPrimaryKeys = ['id']): ?bool
    {
        if (!$this->isAttributeTrackChangesAvailable($attributeName)) {
            // cannot check the changes for the given attribute on this node.
            return null;
        }

        $oldValue = $this->getAttributeOldValue($record, $attributeName);
        $newValue = $this->getAttributeValue($record, $attributeName);

        // 'special' attributes.
        $attr = $this->getAttribute($attributeName);
        if ($attr instanceof FileAttribute) {
            // true if the file has been added or modified.
            return strpos($newValue['tmpfile'], sys_get_temp_dir()) !== false;

        } elseif ($attr instanceof MultiListAttribute || $attr instanceof DateAttribute || $attr instanceof DateTimeAttribute) {
            // check foreach id/key (unordered)

            // convert the null values to empty arrays so the array_diff doesn't complain.
            if ($oldValue === null) {
                $oldValue = [];
            }

            if ($newValue === null) {
                $newValue = [];
            }

            return array_diff($oldValue, $newValue) or array_diff($newValue, $oldValue);

        } elseif (is_array($newValue) || $attr instanceof ManyToOneRelation) {
            // relation attributes

            if (!is_array($newValue)) {
                foreach ($checkMtoPrimaryKeys as $key) {
                    // if newValue is not an array => the attribute must be a ManyToOneRelation
                    // transform the record in an array with 'id' or 'codice' depending on the oldValue structure.
                    if (isset($oldValue[$key])) {
                        $newValue = [$key => $newValue]; // in oldValue we have found a $key field
                    }
                }
            }

            foreach ($checkMtoPrimaryKeys as $key) {
                if ($oldValue[$key] != $newValue[$key]) {
                    return true; // the field value has changed
                }
            }

            return false;
        }

        // default check
        return $newValue != $oldValue;
    }

    /**
     * Verify if the value of the attribute has changed from null or empty to a non-null value.
     */
    protected function attributeGainedValue(array $record, string $attributeName): ?bool
    {
        if (!$this->isAttributeTrackChangesAvailable($attributeName)) {
            // cannot check the changes for the given attribute on this node.
            return null;
        }

        $oldValue = $this->getAttributeOldValue($record, $attributeName);
        $newValue = $this->getAttributeValue($record, $attributeName);

        $attr = $this->getAttribute($attributeName);
        if ($attr instanceof FileAttribute) {
            // True if the file is just added or edited
            return strpos($newValue['tmpfile'], sys_get_temp_dir()) !== false;
        }

        // default check
        return !$oldValue and $newValue;
    }

    /**
     * Get the list of the attribute names for a given tab.
     */
    function getTabAttributeNames(string $tabName): array
    {
        $tabAttributes = $this->getTabAttributes($tabName);
        $attrNames = [];
        foreach ($tabAttributes as $attribute) {
            $attrNames[] = $attribute->fieldName();
        }
        return $attrNames;
    }

    /**
     * Get the list of the attributes for a given tab.
     *
     * @param string $tabName Tab Name
     * @return Attribute[]
     */
    function getTabAttributes(string $tabName): array
    {
        $attrs = [];
        foreach ($this->m_attributeTabs as $attrName => $tabs) {
            if (in_array($tabName, $tabs)) {
                $attrs[] = $this->getAttribute($attrName);
            }
        }
        return $attrs;
    }

    /**
     * True if the node has the passed action
     * @param string $action
     * @return bool
     */
    function hasAction(string $action): bool
    {
        $g_nodes = Atk::getInstance()->g_nodes;
        $nodeActions = $g_nodes[$this->getModule()][$this->getModule()][$this->getType()];
        return Tools::atk_in_array($action, $nodeActions);
    }

    public function isHidePageTitle(): bool
    {
        return $this->hidePageTitle;
    }

    public function setHidePageTitle(bool $hidePageTitle): self
    {
        $this->hidePageTitle = $hidePageTitle;
        return $this;
    }

    // used to retrieve the attribute
    public function getNestedAttribute(string $nestedAttributeField): JsonAttribute
    {
        $attr = new JsonAttribute($nestedAttributeField, Attribute::AF_HIDE | Attribute::AF_FORCE_LOAD);
        $attr->setForceUpdate(true);
        return $attr;
    }

    public function getRecordListDropdownStartIndex(): ?int
    {
        return $this->recordListDropdownStartIndex;
    }

    public function setRecordListDropdownStartIndex(?int $recordListDropdownStartIndex): self
    {
        $this->recordListDropdownStartIndex = $recordListDropdownStartIndex;
        return $this;
    }

    /**
     * This function returns a suitable title text for an action.
     * Example: echo $ui->title("users", "employee", "edit"); might return:
     *          'Edit an existing employee'.
     *
     * @param string|null $action the action that we are trying to find a title for
     * @param bool $actionOnly whether to return a name of the node
     *                           if we couldn't find a specific title
     *
     * @return string the title for the action
     */
    public function nodeTitle(string $action = null, bool $actionOnly = false): string
    {
        $nodeType = $this->m_type;

        if ($action != null) {
            $keys = [
                'title_' . $this->m_module . '_' . $nodeType . '_' . $action,
                'title_' . $nodeType . '_' . $action,
                'title_' . $action
            ];

            $label = $this->text($keys, null, '', '', true);
        } else {
            $label = '';
        }

        if ($label == '') {
            $actionKeys = [
                'action_' . $this->m_module . '_' . $nodeType . '_' . $action,
                'action_' . $nodeType . '_' . $action,
                'action_' . $action,
                $action
            ];

            if ($actionOnly) {
                return $this->text($actionKeys);

            } else {
                $keys = [
                    'title_' . $this->m_module . '_' . $nodeType,
                    'title_' . $nodeType, $nodeType
                ];
                $label = $this->text($keys);
                if ($action != null) {
                    $label .= ' - ' . $this->text($actionKeys);
                }
            }
        }

        return $label;
    }

    /**
     * Override to customize the filename of file using default action export.
     *
     * @return string
     */
    public function exportFileName(): string
    {
        return 'export_' . strtolower(str_replace(' ', '_', $this->nodeTitle('export')));
    }

    /**
     * @throws Exception
     */
    function action_download_file_attribute(ActionHandler $handler, string $downloadName = ''): void
    {
        $record = [];

        try {
            if (!$this->m_postvars[self::PARAM_ATKSELECTOR]) {
                throw new Exception(sprintf($this->text('error_missing_param'), self::PARAM_ATKSELECTOR));
            }

            $attributeName = $this->m_postvars[self::PARAM_ATTRIBUTE_NAME];
            if (!$attributeName || !$this->hasAttribute($attributeName)) {
                throw new Exception(sprintf($this->text('error_missing_param'), self::PARAM_ATTRIBUTE_NAME));
            }

            $record = $this->select($this->m_postvars[self::PARAM_ATKSELECTOR])->getFirstRow();
            if (!$record) {
                throw new Exception($this->text('error_record_not_found'));
            }

            if (!$this->allowed($handler->m_action, $record)) {
                $handler->renderAccessDeniedPage();
                return;
            }

            $fileAttr = $this->getAttribute($attributeName);
            if (!$fileAttr instanceof FileAttribute) {
                throw new Exception(sprintf($this->text('error_attribute_instance_of_fileattribute'), $attributeName));
            }

            $filePath = $fileAttr->getAbsoluteFilePath($record);
            if (!$filePath) {
                throw new Exception(sprintf($this->text('error_fileattribute_path_null'), $attributeName));
            }

            if ($this->m_postvars[FileAttribute::INLINE_PARAM]) {
                $mimeType = mime_content_type($filePath);
                Tools::downloadFile($filePath, $downloadName, $mimeType, true);
            } else {
                Tools::downloadFile($filePath, $downloadName);
            }
            exit;

        } catch (Throwable $e) {
            $this->redirect($this->feedbackUrl($handler->m_action, ActionHandler::ACTION_FAILED, $record, $e->getMessage()));
        }
    }
}

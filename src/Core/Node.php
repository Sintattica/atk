<?php

namespace Sintattica\Atk\Core;

use Sintattica\Atk\Attributes\Attribute;
use Sintattica\Atk\Attributes\FieldSet;
use Sintattica\Atk\Db\Db;
use Sintattica\Atk\Db\Query;
use Sintattica\Atk\Db\QueryPart;
use Sintattica\Atk\Handlers\ActionHandler;
use Sintattica\Atk\RecordList\ColumnConfig;
use Sintattica\Atk\Relations\Relation;
use Sintattica\Atk\Security\SecurityManager;
use Sintattica\Atk\Session\SessionManager;
use Sintattica\Atk\Ui\Page;
use Sintattica\Atk\Ui\PageBuilder;
use Sintattica\Atk\Ui\Ui;
use Sintattica\Atk\Utils\ActionListener;
use Sintattica\Atk\Utils\Selector;
use Sintattica\Atk\Utils\StringParser;

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

    /*
     * Reference to the page on which the node is rendering its output.
     * @access private
     * @var Page
     */
    public $m_page = null;

    /*
     * List of tab orders when defined with setTabOrder.
     * @access private
     * @var array['action']['tab'] = order.
     * Ordered tabs come first.
     */
    private $m_tabOrder = [];

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
     * Record filters, as QueryParts conditions
     * @access private
     * @var array of QueryParts
     */
    public $m_filters = [];

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
    public $m_securityMap = array(
        'save' => 'add',
        'update' => 'edit',
        'multiupdate' => 'edit',
        'copy' => 'add',
        'import' => 'add',
        'editcopy' => 'add',
        'search' => 'admin',
        'smartsearch' => 'admin',
    );

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
    public $m_securityImplied = array('view' => 'admin');

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
    public $m_unsecuredActions = array('select', 'multiselect', 'feedback');

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

    /**
     * @param string $nodeUri The nodeuri
     * @param int $flags Bitmask of node flags (self::NF_*).
     */
    public function __construct($nodeUri, $flags = 0)
    {
        list($this->m_module, $this->m_type) = explode('.', $nodeUri);
        $this->m_flags = $flags;

        $this->setEditFieldPrefix(Config::getGlobal('edit_fieldprefix', ''));
    }

    /**
     * Add an Attribute (or one of its derivatives) to the node.
     *
     * @param Attribute $attribute The attribute you want to add
     * @param mixed $parts The section or tabs on which the attribute should be
     *                     displayed. Can be
     *                      - a tabname (string)
     *                      - a list of tabs (array of string)
     *                      - the string '*' (display on all tabs)
     *                      - a section of a tab (string in the form 'tab.section')
     *                      - a section of the default tab (string in the form '.section')
     * @param int $order The order at which the attribute should be displayed.
     *                   If ommitted, this defaults to 100 for the first
     *                   attribute, and 100 more for each next attribute that
     *                   is added.
     *
     * @return Attribute the attribute just added
     */
    public function add($attribute, $parts = '', $order = null)
    {
        $attribute->m_owner = $this->m_type;

        // Because sections/tabs will probably be used more than the order override option
        // the API for this method now favours the $sections argument. For backwards
        // compatibility we still support the old API ($order as second argument).
        if ($parts !== null && is_int($parts)) {
            $order = $parts;
            $parts = '';
        }

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

        // NOTE: THIS SHOULD WORK. BUT, since add() is called from inside the $this
        // constructor, m_ownerInstance ends up being a copy of $this, rather than
        // a reference. Don't ask me why, it has something to do with the way PHP
        // handles the constructor.
        // To work around this, we reassign the this pointer to the attributes as
        // soon as possible AFTER the constructor. (the dispatcher function)
        $attribute->setOwnerInstance($this);

        if ($attribute->hasFlag(Attribute::AF_PRIMARY)) {
            if (!in_array($attribute->fieldName(), $this->m_primaryKey)) {
                $this->m_primaryKey[] = $attribute->fieldName();
            }
        }

        $attribute->init();

        // If we've already registered an attribute with the same name, we just
        // set the new $order if present
        if (isset($this->m_attribList[$attribute->fieldName()]) && is_object($this->m_attribList[$attribute->fieldName()])) {
            if ($order !== null) {
                $attribute->setOrder($order);
            }
        } else {
            if ($order === null) {
                $this->m_attribOrder += 100;
                $order = $this->m_attribOrder;
                $attribute->setOrder($this->m_attribOrder);
            }
            $attribute->setOrder($order);
        }

        $attribute->setParts($parts);
        $this->m_attribList[$attribute->fieldName()] = $attribute;

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
     * @param mixed $parts see add() description
     * @param int $order see add() description
     */
    public function addFieldSet($name, $template, $flags = 0, $parts = '', $order = null)
    {
        $this->add(new FieldSet($name, $template, $flags), $parts, $order);
    }

    /**
     * Strip section part from a section and return the tab.
     *
     * If no tab name is provided, the default tab is returned.
     *
     * @param string $section The section to get the tab from
     */
    public function getTabFromSection($section)
    {
        $tab = ($section == null) ? '' : $section;

        $dotPos = strpos($tab, '.');
        if ($dotPos !== false) {
            $tab = substr($tab, 0, $dotPos);
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
     */
    public function remove($attribname)
    {
        if (is_object($this->m_attribList[$attribname])) {
            Tools::atkdebug("removing attribute $attribname");

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
        }
    }

    /**
     * Returns the table name for this node.
     *
     * @return string table name
     */
    public function getTable()
    {
        return $this->m_table;
    }

    /**
     * Get an attribute by name.
     *
     * @param string $name The name of the attribute to retrieve.
     *
     * @return Attribute|Relation The attribute.
     */
    public function getAttribute($name)
    {
        $ret = isset($this->m_attribList[$name]) ? $this->m_attribList[$name] : null;

        return $ret;
    }

    /**
     * Checks if the user has filled in something:
     * return true if he has, otherwise return false.
     *
     * @param  -
     *
     * @return boolean.
     */
    public function filledInForm()
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
     * @return Attribute[] Array with the attributes.
     */
    public function &getAttributes()
    {
        if (isset($this->m_attribList)) {
            return $this->m_attribList;
        } else {
            return null;
        }
    }

    /**
     * Returns a list of attribute names.
     *
     * @return array attribute names
     */
    public function getAttributeNames()
    {
        return array_keys($this->m_attribList);
    }

    /**
     * Gets the attribute order.
     *
     * @param string $name The name of the attribute
     */
    public function getAttributeOrder($name)
    {
        return $this->m_attribList[$name]->getOrder();
    }

    /**
     * Sets an attributes order.
     *
     * @param string $name The name of the attribute
     * @param int $order The order of the attribute
     */
    public function setAttributeOrder($name, $order)
    {
        $this->m_attribList[$name]->setOrder($order);
    }

    /**
     * Checks if the node has a certain flag set.
     *
     * @param int $flag The flag to check.
     *
     * @return bool True if the node has the flag.
     */
    public function hasFlag($flag)
    {
        return ($this->m_flags & $flag) == $flag;
    }

    /**
     * Add a flag to the node.
     *
     * @param int $flag The flag to add.
     */
    public function addFlag($flag)
    {
        $this->m_flags |= $flag;
    }

    /**
     * Removes a flag from the node.
     *
     * @param int $flag The flag to remove from the attribute
     */
    public function removeFlag($flag)
    {
        if ($this->hasFlag($flag)) {
            $this->m_flags ^= $flag;
        }
    }

    /**
     * Returns the node flags.
     *
     * @return int node flags
     */
    public function getFlags()
    {
        return $this->m_flags;
    }

    /**
     * Set node flags.
     *
     * @param int $flags node flags
     */
    public function setFlags($flags)
    {
        $this->m_flags = $flags;
    }

    /**
     * Returns the current partial name.
     *
     * @return string partial name
     */
    public function getPartial()
    {
        return $this->m_partial;
    }

    /**
     * Is partial request?
     *
     * @return bool is partial
     */
    public function isPartial()
    {
        return $this->m_partial;
    }

    /**
     * Sets the editable list attributes. If you supply this method
     * with one or more string arguments, all arguments are collected in
     * an array. Else the first parameter will be used.
     *
     * @param array $attrs list of attribute names
     */
    public function setEditableListAttributes($attrs)
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
     */
    public function setMRASelectionMode($mode)
    {
        $this->m_mraSelectionMode = $mode;
    }

    /**
     * Returns the multi-record-action selection mode.
     *
     * @return int multi-record-action selection mode
     */
    public function getMRASelectionMode()
    {
        return $this->m_mraSelectionMode;
    }

    /**
     * Returns the serialization of an encoded array concatenating all primary
     * key values
     *
     * This is the value stored in $record['atkprimkey'] and may appear in HTML
     * pages displayed to clients.
     *
     * @param array $rec The record for which the primary key is calculated.
     *
     * @return string The serialized value
     */
    public function primaryKeyString(array $rec) : string
    {
        $res = [];
        // Case we have only one value : encode it directly.
        if (count($this->m_primaryKey) == 1) {
            if (is_numeric($rec[$this->m_primaryKey[0]])) {
                // casting strings to numeric values when they are numeric :
                return json_encode($rec[$this->m_primaryKey[0]] + 0);
            }
            return json_encode($rec[$this->m_primaryKey[0]]);
        }
        foreach ($this->m_primaryKey as $key) {
            $keyAttr = $this->getAttribute($key);
            if ($keyAttr instanceof Relation) {
                $res[] = $keyAttr->value2db($rec);
            } else {
                $res[] = $rec[$key];
            }
        }
        return json_encode($res);
    }

    /**
     * Compare two primaryKeyStrings and tell if they are equal
     *
     * Strings can't be compared directly because, for example :
     * '{"k1":25,"k2":6}' != '{"k2":6,"k1":25}' but :
     * ["k1" => 25, "k2" => 6] == ["k2" => 6, "k1" => 25]
     *
     * @param string $enc1 first value
     * @param string $enc2 second value
     *
     * @return boolean true if equals, false if not
     */
    public static function primaryKeyStringEqual(string $enc1, string $enc2)
    {
        return json_decode($enc1, true) == json_decode($enc2, true);
    }

    /**
     * Returns Primary Key SQL condition from values encoded with primaryKeyString
     *
     * If several strings are passed as an array, they are joined with OR
     * condition.
     * It is NOT the reverse of primaryKeyString.
     *
     * Examples :
     * '45' or '[45]' => QueryPart('"table"."id"=:id', [':id' => 45])
     * '[2,"txtval"]' => QueryPart('"table"."k1" = :k1 AND "table"."k2" = :k2', [':k1' => 2, ':k2' => "txtval"])
     * ['2', '45'] or ['[2]','[45]'] => QueryPart('"table"."id" = :id1 OR "table"."id" = :id2', [':id1' => 2, ':id2' => 45])
     *
     * @param string|array $selectors The encoded values.
     *
     * @return QueryPart SQL condition
     *                '0=1' (false) if the parameter can't be decoded
     */
    public function primaryKeyFromString($selectors) : QueryPart
    {
        if (is_string($selectors)) {
            $selectors = [$selectors];
        }
        if (!is_array($selectors)) {
            return new QueryPart('0=1');
        }
        $conditions = [];
        foreach ($selectors as $selector) {
            // building the ['k1' => value, 'k2' => value] array for the record :
            $record = json_decode($selector, true);
            if (!is_array($record)) {
                $record = [$record];
            }
            if (count($record) != count($this->m_primaryKey)) {
                continue;
            }
            $record = array_combine($this->m_primaryKey, $record);
            // Computing the primary key SQL condition :
            $conditions[] = $this->primaryKey($record);
        }
        if (empty($conditions)) {
            return new QueryPart('0=1');
        }
        return QueryPart::implode('OR', $conditions, true);
    }

    /**
     * Returns the primary key sql condition for ONE record.
     *
     * @param array $rec The record for which the primary key condition is computed
     * @param boolean $negate return NOT (condition) rather than condition.
     *
     * @return QueryPart the primary key SQL condition
     */
    public function primaryKey($rec, $negate = false)
    {
        $conditions = [];
        foreach ($this->m_primaryKey as $field) {
            $conditions[] = Query::simpleValueCondition($this->m_table, $field, $this->m_attribList[$field]->value2db($rec));
        }

        if (!$negate) {
            return QueryPart::implode('AND', $conditions, true);
        }
        // $negate :
        $condition = new QueryPart('NOT ');
        $condition->append(QueryPart::implode('AND', $conditions, true));
        return $condition;
    }

    /**
     * Retrieve the name of the primary key attribute.
     *
     * Note: If a node has a primary key that consists of multiple attributes,
     * this method will retrieve only the first attribute!
     *
     * @return string First primary key attribute
     */
    public function primaryKeyField()
    {
        if (Tools::count($this->m_primaryKey) === 0) {
            Tools::atkwarning($this->atkNodeUri().'::primaryKeyField() called, but there are no primary key fields defined!');

            return;
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
    public function primaryKeyTpl()
    {
        $primKey = '';
        $nrOfElements = Tools::count($this->m_primaryKey);
        for ($i = 0; $i < $nrOfElements; ++$i) {
            $primKey .= $this->m_primaryKey[$i]."='[".$this->m_primaryKey[$i]."]'";
            if ($i < ($nrOfElements - 1)) {
                $primKey .= ' AND ';
            }
        }

        return $primKey;
    }

    /**
     * Set default sort order for the node.
     *
     * @param string $orderby Default order by. Can be an attribute name or a
     *                        SQL expression.
     */
    public function setOrder($orderby)
    {
        $this->m_default_order = $orderby;
    }

    /**
     * Get default sort order for the node.
     *
     * @return string $orderby Default order by. Can be an attribute name or a
     *                SQL expression.
     */
    public function getOrder()
    {
        return str_replace('[table]', Db::quoteIdentifier($this->getTable()), $this->m_default_order);
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
     * @param mixed $db The database connection to use. If ommitted, this
     *                          defaults to the default database connection.
     *                          So in apps using only one database, it's not necessary
     *                          to pass this parameter.
     *                          You can pass either a connection (Db instance), or
     *                          a string containing the name of the connection to use.
     */
    public function setTable($tablename, $seq = '', $db = null)
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
    public function setDb($db)
    {
        $this->m_db = $db;
    }

    /**
     * Get the database connection for this node.
     *
     * @return Db Database connection instance
     */
    public function getDb()
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
     */
    public function setIndex($attribname)
    {
        $this->m_index = $attribname;
    }

    /**
     * Set tab order
     *
     * Ordered tabs come before unordered tabs. By default, tabs get their order from
     * the order of the attributes
     *
     * @param string $tabname Tabname
     * @param int $order Index number
     * @param string $action Action name (add,edit,view). If not set, the tab order
     *                       is set for all actions.
     */
    public function setTabOrder($tabname, $order, $action = '')
    {
        Tools::atkdebug("self::setTabIndex($tabname,$index,$action)");
        $actionList = array('add', 'edit', 'view');
        if ($action == '') {
            $actionList = array($action);
        }
        foreach ($actionList as $action) {
            if (!isset($this->m_tabOrder[$action])) {
                $this->m_tabOrder[$action] = [];
            }
            $this->m_tabOrder[$action][$tabname] = $order;
        }
    }

    /**
     * Set tab order (alias of setTabOrder, for compatibility reasons)
     */
    public function setTabIndex($tabname, $index, $action = '')
    {
        $this->setTabOrder($tabname, $index, $action);
    }

    /**
     * Set default tab being displayed in view/add/edit mode. All attributes
     * without a tab or a section are added to this tab.
     *
     * If you accept the default name for the first tab ("default") you do not
     * need to call this method.
     *
     * @param string $tab the name of the default tab
     */
    public function setDefaultTab($tab = 'default')
    {
        $this->m_default_tab = $tab;
    }

    /**
     * Resolve tab name
     *
     * @param string $tab name
     *
     * @return $string $tab
     */
    public function resolveTab($tab = '')
    {
        if ($tab == '') {
            return $this->m_default_tab;
        }
        if ($tab == '*') {
            return 'alltabs';
        }
        return $tab;
    }

    /**
     * Sort the list of $tabs according to self::m_tabOrder
     *
     * @param string $action we're in
     * @param array $tabs list of $tabs collected
     *
     * @return array $tabs ordered
     */
    public function sortTabs($action, $tabs)
    {
        if (empty($this->m_tabOrder)) {
            return $tabs;
        }
        // Then we reorder the tab list :
        usort($tabs, function ($t1, $t2) {
                $i1 = $this->m_tabOrder[$action][$t1] ?? PHP_INT_MAX;
                $i2 = $this->m_tabOrder[$action][$t2] ?? PHP_INT_MAX;
                return ($i1 == $i2) ? 0 : ($i1 > $i2 ? 1 : -1);
            });
        return $tabs;
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
    public function resolveSection($section)
    {
        if (substr($section, 0, 1) == '.') {
            return $this->m_default_tab . $section;
        }
        return $section;
    }

    /**
     * Add sections that must be expanded by default.
     */
    public function addDefaultExpandedSections()
    {
        $sections = func_get_args();
        foreach ($sections as $section) {
            $section = $this->resolveSection($section);
            if (!in_array($section, $this->m_default_expanded_sections)) {
                $this->m_default_expanded_sections[] = $section;
            }
        }
    }

    /**
     * Remove sections that must be expanded by default.
     */
    public function removeDefaultExpandedSections()
    {
        $sections = func_get_args();

        $this->m_default_expanded_sections = array_diff($this->m_default_expanded_sections, $sections);
    }

    /**
     * Tell if a section is expanded by default or not
     *
     * @param $section name
     *
     * @return bool
     */
    public function isSectionDefaultExpanded($section)
    {
        return in_array($section, $this->m_default_expanded_sections);
    }

    /**
     * Check if the user has the rights to access existing tabs and
     * removes tabs from the list that may not be accessed.
     *
     * @param array $tablist Array containing the current tablist
     *
     * @return array with disable tabs
     */
    public function checkTabRights(&$tablist)
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

            $priv = 'tab_'.$tablist[$i];
            if (isset($atk->g_nodes[$this->m_module][$this->m_type]) && Tools::atk_in_array($priv, $atk->g_nodes[$this->m_module][$this->m_type])) {
                // authorisation is required
                if (!$secMgr->allowed($this->m_module.'.'.$this->m_type, 'tab_'.$tablist[$i])) {
                    Tools::atkdebug('Removing TAB '.$tablist[$i].' because access to this tab was denied');
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
     * Add a recordset filter.
     *
     * You can add 3 kinds of filters :
     * - field, value filter : will filter on table.field = value
     * - a SQL expression that may contain '[table]' : will filter on this
     *      expression, replacing [table] with table name.
     * - a QueryPart condition (with parameters).
     *
     * @param string $filter a QueryPart expression or the fieldname you want
     *                       to filter OR a SQL where clause expression.
     * @param mixed $value Required value. (Ommit this parameter if you pass
     *                       au QueryPart or an SQL expression for $filter.)
     */
    public function addFilter($filter, $value = '')
    {
        if ($filter instanceof QueryPart) {
            // QueryPart case :
            $this->m_filters[] = $filter;
        }
        if ($value == '') {
            // $filter is a where clause kind of thing
            $this->m_filters[] = new QueryPart(str_replace('[table]', Db::quoteIdentifier($this->m_table), $filter), []);
        } else {
            // $field, $value case :
            $this->m_filters[] = Query::simpleValueCondition($this->m_table, $filter, $value);
        }
    }

    /**
     * Search and remove a recordset filter.
     *
     *  DEPRECATED.
     *
     * @return false
     */
    public function removeFilter($filter, $value = '')
    {
        Tools::atkwarning('Function removeFilter is deprecated and no longer works.');

        return false;
    }

    /**
     * Returns the form buttons for a certain page.
     *
     * Can be overridden by derived classes to define custom buttons.
     *
     * @param string $mode The action for which the buttons are retrieved.
     * @param array $record The record currently displayed/edited in the form.
     *                       This param can be used to define record specific
     *                       buttons.
     *
     * @return array
     */
    public function getFormButtons($mode, $record = array())
    {
        $result = [];
        $sm = SessionManager::getInstance();

        // edit mode
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
                $result[] = '<input type="hidden" name="atkaction" value="edit">'.'<input type="hidden" name="atknodeuri" value="'.$this->atkNodeUri().'">'.$this->getButton('edit');
            }

            if ($sm->atkLevel() > 0) {
                $result[] = $this->getButton('back', false, Tools::atktext('cancel'));
            }
        } elseif ($mode == 'delete') {
            $result[] = '<input name="confirm" type="submit" class="btn btn-default btn_ok" value="'.$this->text('yes').'">';
            $result[] = '<input name="cancel" type="submit" class="btn btn-default btn_cancel" value="'.$this->text('no').'">';
        } elseif ($mode == 'search') {
            // (don't change the order of button)
            $result[] = $this->getButton('search', true);
            $result[] = $this->getButton('cancel');
        }

        return $result;
    }

    /**
     * Create a button.
     *
     * @param string $action
     * @param bool $default Add the atkdefaultbutton class?
     *
     * @return string HTML
     */
    public function getButton($action, $default = false, $label = null)
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
                $value = '<< '.Tools::atktext($action, 'atk');
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

        $class = trim('btn '.$class);

        if ($default) {
            $class .= (!empty($class) ? ' ' : '').'atkdefaultbutton btn-primary';
        } else {
            $class .= (!empty($class) ? ' ' : '').'btn-default';
        }

        if ($class != '') {
            $class = "class=\"$class\" ";
        }

        if ($value != '') {
            $valueAttribute = "value=\"{$value}\" ";
        }

        if ($name != '') {
            $name = 'name="'.$this->getEditFieldPrefix()."{$name}\" ";
        }

        return '<button type="submit" '.$class.$name.$valueAttribute.'>'.$value.'</button>';
    }

    /**
     * Get the ui instance for drawing and templating purposes.
     *
     * @return Ui An Ui instance for drawing and templating.
     */
    public function getUi()
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
     *
     * @return string The full title of the action.
     */
    public function actionTitle($action, $record = [])
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
                $descrtrace .= htmlentities($desc, ENT_COMPAT).' - ';
            }
            $res = $descrtrace.$res;
        }

        if (is_object($ui)) {
            $res .= $ui->nodeTitle($this, $action, $nomodule);
        }

        return $res;
    }

    /**
     * Determine the default form parameters for an action template.
     *
     * @param bool $locked If the current record is locked, pass true, so
     *                     the lockicon can be placed in the params too.
     *
     * @return array Default form parameters for action forms (assoc. array)
     */
    public function getDefaultActionParams($locked = false)
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
     * @param array $record current record (optional)
     */
    public function checkAttributeSecurity($mode, $record = null)
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
    public function preAddToEditArray(&$record, $mode)
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
    public function preAddToViewArray(&$record, $mode)
    {
        // do nothing
    }

    /**
     * Function outputs an array with edit fields. For each field the array
     * contains the name, edit HTML code etc. (name, html, obligatory,
     * error, label).
     *
     * @todo The editArray method should use a set of classes to build the
     *       form, instead of an array with an overly complex structure.
     *
     * @param string $mode The edit mode ("add" or "edit")
     * @param array $record The record currently being edited.
     * @param array $forceList A key-value array used to preset certain
     *                               fields to a certain value, regardless of the
     *                               value in the record.
     * @param array $suppressList List of attributenames that you want to hide
     * @param string $fieldprefix Of set, each form element is prefixed with
     *                               the specified prefix (used in embedded form
     *                               fields)
     *
     * @return array List of edit fields (per field ( name, html, obligatory,
     *               error, label })
     */
    public function editArray(
        $mode = 'add',
        $record = null,
        $forceList = '',
        $suppressList = '',
        $fieldprefix = ''
    ) {
        // update visibility of some attributes based on the current record
        $this->checkAttributeSecurity($mode, $record);

        /* read metadata */
        $this->setAttribSizes();

        /* default values */
        if (!empty($record)) {
            $defaults = $record;
        } else {
            $defaults = [];
        }

        $result['hide'] = [];
        $result['fields'] = [];

        /* edit mode */
        if ($mode == 'edit') {
            /* nodes can define edit_values */
            $overrides = $this->edit_values($defaults);
            foreach ($overrides as $varname => $value) {
                $defaults[$varname] = $value;
            }
        } /* add mode */ else {
            /* nodes can define initial values, if they don't already have values. */
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

        /* check for forced values */
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
                        $defaults[$forcedvarname] = $forcedvalue;
                        $attribname = $forcedvarname;
                    }

                    if ($attribname != '') {
                        if (isset($this->m_attribList[$attribname])) {
                            $p_attrib = $this->m_attribList[$attribname];
                            if (is_object($p_attrib) && (!$p_attrib->hasFlag($p_attrib::AF_NO_FILTER))) {
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
        $result['hide'][] = '<input type="hidden" name="atkfieldprefix" value="'.$this->getEditFieldPrefix(false).'">';
        $result['hide'][] = '<input type="hidden" name="'.$fieldprefix.'atknodeuri" value="'.$this->atkNodeUri().'">';
        $result['hide'][] = '<input type="hidden" name="'.$fieldprefix.'atkprimkey" value="'.htmlspecialchars(Tools::atkArrayNvl($record, 'atkprimkey', '')).'">';

        foreach ($this->m_attribList as $attribname => $p_attrib) {
            if ($p_attrib->hasDisabledMode($p_attrib::DISABLED_EDIT)) {
                continue;
            }

            /* fields that have not yet been initialised may be overriden in the url */
            if (!array_key_exists($attribname, $defaults) && array_key_exists($attribname, $this->m_postvars)) {
                $defaults[$attribname] = $this->m_postvars[$attribname];
            }

            if (is_array($suppressList) && Tools::count($suppressList) > 0 && in_array($attribname, $suppressList)) {
                $p_attrib->m_flags |= ($mode == 'add' ? $p_attrib::AF_HIDE_ADD : $p_attrib::AF_HIDE_EDIT);
            }

            /* we let the attribute add itself to the edit array */
            $p_attrib->addToEditArray($mode, $result, $defaults, $record['atkerror'], $fieldprefix);
        }

        /* check for errors */
        $result['error'] = $record['atkerror'];

        /* return the result array */

        return $result;
    }

    /**
     * Function outputs an array with view fields. For each field the array
     * contains the name, view HTML code etc.
     *
     * @todo The viewArray method should use a set of classes to build the
     *       form, instead of an array with an overly complex structure.
     *
     * @param string $mode The edit mode ("view")
     * @param array $record The record currently being viewed.
     * @param bool $injectSections Inject sections?
     *
     * @return array List of edit fields (per field ( name, html, obligatory,
     *               error, label })
     */
    public function viewArray($mode, $record, $injectSections = true)
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

        foreach ($this->m_attribList as $attribname => $p_attrib) {
            if ($p_attrib->hasDisabledMode(Attribute::DISABLED_VIEW)) {
                continue;
            }

            /* we let the attribute add itself to the view array */
            $p_attrib->addToViewArray($mode, $result, $record);
        }

        /* return the result array */

        return $result;
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
    public function initial_values()
    {
        $record = [];

        foreach ($this->m_attribList as $attr) {
            $value = $attr->fetchValue($this->m_postvars) ?? $attr->initialValue();

            if ($value !== null) {
                $record[$attribname] = $value;
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
     *
     * @return array The manipulated record.
     */
    public function edit_values($record)
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
     * @param array $record The record for which you want to return the
     *                       template (or NULL if there is no record).
     * @param string $tab The name of the tab for which you want to
     *                       retrieve the template.
     *
     * @return string The filename of the template (without path)
     */
    public function getTemplate($action, $record = null, $tab = '')
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
    }

    /**
     * Function outputs a form with all values hidden.
     *
     * This is probably only useful for the atkOneToOneRelation's hide method.
     *
     * @param string $mode The edit mode ("add" or "edit")
     * @param array $record The record that should be hidden.
     * @param array $forceList A key-value array used to preset certain
     *                            fields to a certain value, regardless of the
     *                            value in the record.
     * @param string $fieldprefix Of set, each form element is prefixed with
     *                            the specified prefix (used in embedded form
     *                            fields)
     *
     * @return string HTML fragment containing all hidden elements.
     */
    public function hideForm($mode = 'add', $record = null, $forceList = '', $fieldprefix = '')
    {
        /* suppress all */
        $suppressList = array_keys($this->m_attribList);

        /* get data, transform into "form", return */
        $data = $this->editArray($mode, $record, $forceList, $suppressList, $fieldprefix);
        $form = '';
        foreach ($data['hide'] as $hide) {
            $form .= $hide;
        }

        return $form;
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
     *                       action urls. The array should be key/value based.
     *
     * @return array List of actions in the form array($action=>$actionurl)
     */
    public function defaultActions($mode, $params = array())
    {
        $actions = [];
        $postfix = '';

        if (Tools::count($params) > 0) {
            foreach ($params as $key => $value) {
                $postfix .= "&$key=".rawurlencode($value);
            }
        }

        $actionbase = Config::getGlobal('dispatcher').'?atknodeuri='.$this->atkNodeUri().'&atkselector=[pk]'.$postfix;
        if (!$this->hasFlag(self::NF_NO_VIEW) && $this->allowed('view')) {
            $actions['view'] = $actionbase.'&atkaction=view';
        }

        if ($mode != 'view') {
            if (!$this->hasFlag(self::NF_NO_EDIT) && $this->allowed('edit')) {
                $actions['edit'] = $actionbase.'&atkaction=edit';
            }

            if (!$this->hasFlag(self::NF_NO_DELETE) && $this->allowed('delete')) {
                $actions['delete'] = $actionbase.'&atkaction=delete';
            }
            if ($this->hasFlag(self::NF_COPY) && $this->allowed('copy')) {
                $actions['copy'] = $actionbase.'&atkaction=copy';
            }
            if ($this->hasFlag(self::NF_EDITAFTERCOPY) && $this->allowed('editcopy')) {
                $actions['editcopy'] = $actionbase.'&atkaction=editcopy';
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
    public function setPriorityRange($min = 1, $max = 0)
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
    public function setPriorityActions($actions)
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
     * @return string extended search action
     */
    public function getExtendedSearchAction()
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
    public function setExtendedSearchAction($action)
    {
        $this->m_extended_search_action = $action;
    }

    /**
     * Function returns a page in which the user is asked if he really wants
     * to perform a certain action.
     *
     * @param mixed $atkselector Selector of current record on which the
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
     * @param string $csrfToken
     *
     * @return string Complete html fragment containing a box with the
     *                confirmation page, or the output of the custom
     *                override if $checkoverride was true.
     */
    public function confirmAction(
        $atkselector,
        $action,
        $checkoverride = true,
        $csrfToken = null
    ) {
        $method = 'confirm'.$action;
        if ($checkoverride && method_exists($this, $method)) {
            return $this->$method($atkselector);
        }

        $ui = $this->getUi();

        $sm = SessionManager::getInstance();

        $formstart = '<form action="'.Config::getGlobal('dispatcher').'" method="post">';
        $formstart .= $sm->formState();
        $formstart .= '<input type="hidden" name="atkaction" value="'.$action.'">';
        $formstart .= '<input type="hidden" name="atknodeuri" value="'.$this->atkNodeUri().'">';

        if (isset($csrfToken)) {
            $this->getHandler($action);
            $formstart .= '<input type="hidden" name="atkcsrftoken" value="'.$csrfToken.'">';
        }

        if (!is_array($atkselector)) {
            $atkselector = [$atkselector];
        }
        foreach ($atkselector as $selector) {
            $formstart .= '<input type="hidden" name="atkselector[]" value="'.htmlspecialchars($selector).'">';
        }

        $buttons = $this->getFormButtons($action, array());
        if (Tools::count($buttons) == 0) {
            $buttons[] = '<input name="confirm" type="submit" class="btn btn-default btn_ok atkdefaultbutton" value="'.$this->text('yes').'">';
            $buttons[] = '<input name="cancel" type="submit" class="btn btn-default btn_cancel" value="'.$this->text('no').'">';
        }

        $content = '';
        $record = null;
        $recs = $this->select($this->primaryKeyFromString($atkselector))->includes($this->descriptorFields())->fetchAll();
        if (Tools::count($recs) == 1) {
            // 1 record, put it in the page title (with the actionTitle call, a few lines below)
            $record = $recs[0];
            $this->getPage()->setTitle(Tools::atktext('app_shorttitle').' - '.$this->actionTitle($action, $record));
        } else {
            // we are gonna perform an action on more than one record
            // show a list of affected records, at least if we can find a
            // descriptor_def method
            if ($this->m_descTemplate != null || method_exists($this, 'descriptor_def')) {
                $content .= '<ul>';
                foreach ($recs as $record) {
                    $content .= '<li>'.str_replace(' ', '&nbsp;', htmlentities($this->descriptor($record)));
                }
                $content .= '</ul>';
            }
        }

        $content .= '<br>'.$this->confirmActionText($atkselector, $action, true);

        $output = $ui->renderAction($action, array(
            'content' => $content,
            'formstart' => $formstart,
            'formend' => '</form>',
            'buttons' => $buttons,
        ));

        return $ui->renderBox(array(
            'title' => $this->actionTitle($action, $record),
            'content' => $output,
        ));
    }

    /**
     * Determine the confirmation message.
     *
     * @param string $atkselector The record(s) on which the action is
     *                              performed.
     * @param string $action The action being performed.
     * @param bool $checkoverride If true, returns the output of a custom
     *                              method named "confirm".$action."text()"
     *
     * @return string The confirmation text.
     */
    public function confirmActionText($atkselector = '', $action = 'delete', $checkoverride = true)
    {
        $method = 'confirm'.$action.'text';
        if ($checkoverride && method_exists($this, $method)) {
            return $this->$method($atkselector);
        } else {
            return $this->text("confirm_$action".(is_array($atkselector) && count($atkselector) > 1 ? '_multi' : ''));
        }
    }

    /**
     * Small compare function for sorting attribs on order field.
     *
     * @param array $a The first attribute
     * @param array $b The second attribute
     *
     * @return int
     */
    private static function attrib_cmp($a, $b)
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
    public function init()
    {
        Tools::atkdebug('init for '.$this->m_type);

        // Check if initialisation is not already done.
        if ($this->m_initialised == true) {
            return;
        }

        $this->_addListeners();

        $this->attribSort();
        $this->setAttribSizes();
        $this->m_initialised = true;

        // Call the attributes postInit method to do some last time
        // initialization if necessary.
        foreach ($this->m_attribList as $p_attrib) {
            $p_attrib->postInit();
        }
    }

    /**
     * Add the listeners for the current node
     * A listener can be defined either by placing an instantiated object
     * or the full location in Tools::atkimport( style notation, in a global array
     * called $g_nodeListeners (useful for example for adding listeners
     * to nodes from another module's module.inc file. in module.inc files,
     * $listeners can be used to add listeners to a node.
     */
    public function _addListeners()
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
    public function setAttribSizes()
    {
        if ($this->m_attribsizesset) {
            return true;
        }

        $db = $this->getDb();
        $metainfo = $db->tableMeta($this->m_table);

        foreach ($this->m_attribList as $p_attrib) {
            $p_attrib->fetchMeta($metainfo);
        }
        $this->m_attribsizesset = true;
    }

    /**
     * Render a generic action.
     *
     * Renders actionpage.tpl for the desired action. This includes the
     * given block(s) and a pagetrial, but not a box.
     *
     * @param string $action The action for which the page is rendered.
     * @param mixed $blocks Pieces of html content to be rendered. Can be a
     *                       single string with content, or an array with
     *                       multiple content blocks.
     *
     * @return string Piece of HTML containing the given blocks and a pagetrail.
     */
    public function renderActionPage($action, $blocks = array())
    {
        if (!is_array($blocks)) {
            $blocks = ($blocks == '' ? [] : array($blocks));
        }

        $ui = $this->getUi();

        // todo: overridable action templates
        return $ui->render('actionpage.tpl', array(
            'blocks' => $blocks,
            'title' => $ui->title($this->m_module, $this->m_type),
        ));
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
    public function setFeedback($action, $statusmask)
    {
        if (is_array($action)) {
            foreach ($action as $actionname) {
                $this->m_feedback[$actionname] = $statusmask;
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
    public function getPage()
    {
        $page = Page::getInstance();

        return $page;
    }

    /**
     * Returns a new page builder instance.
     *
     * @return PageBuilder
     */
    public function createPageBuilder()
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
    public function redirect($location = '', $recordOrExit = [], $exit = false, $levelskip = 1)
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
                $location .= '&'.$this->m_postvars['atkpkret'].'='.rawurlencode($this->primaryKeyString($record));
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
     * @param array $vars The request variables that were posted from a form.
     * @param array $includes Only fetch the value for these attributes.
     * @param array $excludes Don't fetch the value for these attributes.
     * @param array|bool $postedOnly Only fetch the value for attributes that have really been posted.
     *
     * @return array A valid record.
     */
    public function updateRecord($vars = '', $includes = null, $excludes = null, $postedOnly = false)
    {
        if ($vars == '') {
            $vars = $this->m_postvars;
        }
        $record = [];

        foreach ($this->m_attribList as $attribname => $p_attrib) {
            if ((!is_array($includes) || in_array($attribname, $includes)) && (!is_array($excludes) || !in_array($attribname, $excludes))) {
                if (!$postedOnly || $p_attrib->isPosted($vars)) {
                    $record[$attribname] = $p_attrib->fetchValue($vars);
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
    public function modifyRecord(&$record, $vars)
    {
        foreach ($this->m_attribList as $attribname => $p_attrib) {
            $record[$attribname] = $p_attrib->fetchValue($vars);
        }
    }

    /**
     * Get descriptor handler.
     *
     * @return object descriptor handler
     */
    public function getDescriptorHandler()
    {
        return $this->m_descHandler;
    }

    /**
     * Set descriptor handler.
     *
     * @param object $handler The descriptor handler.
     */
    public function setDescriptorHandler($handler)
    {
        $this->m_descHandler = $handler;
    }

    /**
     * Returns the descriptor template for this node.
     *
     * It will return (by priority order) :
     * $this->m_descTemplate;
     * $this->descriptor_def();
     * '['.name of the first attribute.']'
     * ''
     *
     * @return string The descriptor Template
     */
    public function getDescriptorTemplate()
    {
        if ($this->m_descTemplate != null) {
            return $this->m_descTemplate;
        }
        if (method_exists($this, 'descriptor_def')) {
            return $this->descriptor_def();
        }
        if (!empty($this->m_attribList)) {
            // default descriptor : first attribute of a node
            return '['.array_keys($this->m_attribList)[0].']';
        }
        return '';
    }

    /**
     * Sets the descriptor template for this node.
     *
     * @param string $template The descriptor template.
     */
    public function setDescriptorTemplate($template)
    {
        $this->m_descTemplate = $template;
    }

    /**
     * Retrieve the list of attributes that are used in the descriptor
     * definition.
     *
     * @return array The names of the attributes forming the descriptor.
     */
    public function descriptorFields()
    {
        $fields = [];

        $parser = new StringParser($this->getDescriptorTemplate());
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
    public function descriptor($record)
    {
        // Descriptor handler is set?
        if ($this->m_descHandler != null) {
            return $this->m_descHandler->descriptor($record, $this);
        }

        $parser = new StringParser($this->getDescriptorTemplate());
        return $parser->parse($record);
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
    public function validate(&$record, $mode, $ignoreList = array())
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
    public function addUniqueFieldset($fieldArr)
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
     * @param array $excludes
     * @param array $includes
     */
    public function trackChangesIfNeeded(&$record, $excludes = '', $includes = '')
    {
        if (!$this->hasFlag(self::NF_TRACK_CHANGES) || isset($record['atkorgrec'])) {
            return;
        }

        // We need to add the NO_FILTER flag in case the new values would filter the record.
        $flags = $this->m_flags;

        $this->addFlag(self::NF_NO_FILTER);

        $record['atkorgrec'] = $this->select()->where($this->primaryKeyFromString($record['atkprimkey']))->excludes($excludes)->includes($includes)->mode('edit')->getFirstRow();

        // Need to restore the NO_FILTER bit back to its original value.
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
     * @param array $excludes exclude list (these attribute will *not* be updated)
     * @param array $includes include list (only these attributes will be updated)
     *
     * @return bool True if succesful, false if not.
     */
    public function updateDb(&$record, $exectrigger = true, $excludes = '', $includes = '')
    {
        $db = $this->getDb();
        $query = $db->createQuery($this->m_table);

        // The record that must be updated is indicated by 'atkprimkey'
        // (not by atkselector, since the primary key might have
        // changed, so we use the atkprimkey, which is the value before
        // any update happened.)
        if ($record['atkprimkey'] != '') {
            $this->trackChangesIfNeeded($record, $excludes, $includes);

            if ($exectrigger) {
                if (!$this->executeTrigger('preUpdate', $record)) {
                    Tools::atkerror('preUpdate() failed!');
                    return false;
                }
            }

            $query->addCondition($this->primaryKeyFromString($record['atkprimkey']));

            $storelist = array('pre' => [], 'post' => [], 'query' => array());

            foreach ($this->m_attribList as $attribname => $p_attrib) {
                if ((!is_array($excludes) || !in_array($attribname, $excludes)) && (!is_array($includes) || in_array($attribname, $includes))) {
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

            foreach ($storelist['query'] as $attribname) {
                $p_attrib = $this->m_attribList[$attribname];
                $p_attrib->addToQuery($query, $this->m_table, '', $record, 1, 'update'); // start at level 1
            }

            if (!$query->executeUpdate()) {
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
     * Call the store() method on a list of attributes.
     *
     * @param array $storelist The list of attributes for which the
     *                          store() method should be called.
     * @param array $record The master record being stored.
     * @param string $mode The storage mode ("add", "copy" or "update")
     *
     * @return bool True if successful, false if not.
     */
    public function _storeAttributes($storelist, &$record, $mode)
    {
        // store special storage attributes.
        foreach ($storelist as $attribname) {
            $p_attrib = $this->m_attribList[$attribname];
            if (!$p_attrib->store($this->getDb(), $record, $mode)) {
                // something went wrong.
                Tools::atkdebug("Store aborted. Attribute '{$attribname}' reported an error.");

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
     * manytoonerelation) are not copied.
     *
     * @param array $record The record to copy.
     * @param string $mode The mode we're in (mostly "copy")
     *
     * @return bool True if succesful, false if not.
     */
    public function copyDb(&$record, $mode = 'copy')
    {
        // add original record
        $original = $record; // force copy
        $record['atkorgrec'] = $original;

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
    public function getSearchMode()
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
     * Retrieve records from the database using a handy helper class.
     *
     * @param QueryPart|string $condition SQL condition
     *
     * @return Selector
     */
    public function select($condition = null)
    {
        $selector = new Selector($this);

        $selector->orderBy($this->getOrder());
        $selector->ignoreDefaultFilters($this->hasFlag(self::NF_NO_FILTER));

        if (is_string($condition) && !empty($condition)) {
            $selector->where(new QueryPart($condition));
        } elseif (!is_null($condition)) {
            $selector->where($condition);
        }

        return $selector;
    }

    /**
     * Returns a record (array) as identified by a primary key (usually an "id" column),
     * including applicable relations.
     *
     * @param int $pk primary key identifying the record
     *
     * @return array the associated record, or null if no such record exists
     */
    public function fetchByPk($pk)
    {
        return $this->select($this->primaryKeyFromString($pk))->getFirstRow();
    }

    /**
     * Add this node to an existing query.
     *
     * Framework method, it should not be necessary to call this method
     * directly.
     * This method is used when adding the entire node to an existing
     * query, as part of a join.
     *
     * @todo The allfields parameter is too inflexible.
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
     */
    public function addToQuery($query, $alias = '', $level = 0, $allfields = false, $mode = 'select', $includes = array())
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
            foreach ($this->m_attribList as $name => $attrib) {
                if ($attrib->hasFlag(Attribute::AF_FORCE_LOAD)) {
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
                        $fieldaliasprefix = $alias.'_AE_';
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
     * Get a search condition from a template string
     *
     * Used for searching by descriptor or by aggregated columns.
     * For an expression like '[name] ([email])', it will perform the search
     * on "table"."name", on "table"."email" and on the expression :
     * CONCAT_WS('', "table".name", ' (', "table"."email", ')')
     * (the use of CONCAT_WS permit that null fields are coalesced to '').
     *
     * For the last part (the expression), only 'exact', 'substring', 'wildcard'
     * and 'regexp' modes' are allowed, and it will work with simple types, not
     * with complex types (such as : relations, flags, list attributes, ...)
     *
     * TODO : handle relations (at least).
     *
     * @param Query $query
     * @param string $table or alias used to identifiy this node's table
     * @param string $expr to use for searching values
     * @param string $value to search for
     * @param string $searchmode
     * @param string $fieldaliasprefix to prepend to joins if needed.
     */
    public function getTemplateSearchCondition($query, $table, $expr, $value, $searchmode, $fieldaliasprefix)
    {
        if(empty($table)) {
            $table = $this->m_table;
        }
        $searchConditions = [];

        $parser = new StringParser($expr);
        // Searching by individual fields :
        foreach ($parser->getFields() as $field) {
            $p_attrib = $this->getAttribute($field);
            if (!is_object($p_attrib)) {
                continue;
            }

            // check if the node has a searchcondition method defined for this attr
            $methodName = $field.'_searchcondition';
            if (method_exists($this, $methodName)) {
                $sc = $this->$methodName($query, $table, $value, $searchmode);
                if (!is_null($sc)) {
                    $searchConditions[] = $sc;
                }
            } else {
                Tools::atkdebug("getSearchCondition: $table - $fieldaliasprefix");
                $sc = $p_attrib->getSearchCondition($query, $table, $value, $searchmode, $fieldaliasprefix);
                if (!is_null($sc)) {
                    $searchConditions[] = $sc;
                }
            }
        }

        if (!in_array($searchmode, ['exact', 'substring', 'wildcard', 'regexp'])) {
            return QueryPart::implode('OR', $searchConditions, true);
        }

        // Searching by the expression concatenating all fields :
        $parts = [];
        $db = $this->getDb();
        foreach($parser->getAllFieldsAsArray() as $field) {
            if($field[0] != '[' || $field[strlen($field)-1] != ']') {
                // If it's a string part, just quote it and append it :
                $parts[] = $db->quote($field);
            } else {
                // If it's an attribute, then add its name to query, after removing the []
                $field = substr($field, 1, strlen($field) - 2);
                if (strpos($field, '.') === false) {
                    // Simple case : attribute from this node.
                    $parts[] = Db::quoteIdentifier($table, $field);
                } else {
                    // Complex case : attribute from a relation :
                    list($relationName, $attributeName) = explode('.', $field);
                    $relation = $this->getAttribute($relationName);
                    if ($relation instanceof Relation) {
                        $alias = $table.'_AE_'.$relationName;
                        $query->addJoin($relation->getDestination()->getTable(), $alias, $relation->getJoinCondition($table, $alias));
                        $parts[] = Db::quoteIdentifier($alias, $attributeName);
                    }
                }
            }
        }
        $expression = $db->func_concat_coalesce($parts);
        $func = $searchmode.'Condition';
        $sc = $query->$func($expression, $value);
        if (!is_null($sc)) {
            $searchConditions[] = $sc;
        }
        return QueryPart::implode('OR', $searchConditions, true);
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
     *
     * @return bool True if succesful, false if not.
     */
    public function addDb(&$record, $exectrigger = true, $mode = 'add', $excludelist = array())
    {
        if ($exectrigger) {
            if (!$this->executeTrigger('preAdd', $record, $mode)) {
                return Tools::atkerror('preAdd() failed!');
            }
        }

        $db = $this->getDb();
        $query = $db->createQuery($this->m_table);

        $storelist = array('pre' => [], 'post' => [], 'query' => array());

        foreach ($this->m_attribList as $attribname => $p_attrib) {
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

        foreach ($storelist['query'] as $attribname) {
            $p_attrib = $this->m_attribList[$attribname];
            $p_attrib->addToQuery($query, $this->m_table, '', $record, 1, 'add'); // start at level 1
        }

        if (!$query->executeInsert()) {
            Tools::atkdebug('executeInsert failed..');

            return false;
        }

        // new primary key
        $record['atkprimkey'] = $this->primaryKeyString($record);

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
     * @param string $mode mode like add or update
     *
     * @return bool true on case of success or when the trigger isn't returning anything (assumes success)
     */
    public function executeTrigger($trigger, &$record, $mode = null)
    {
        if (!isset($record['__executed'.$trigger])) {
            $record['__executed'.$trigger] = true;

            $return = $this->$trigger($record, $mode);

            if ($return === null) {
                Tools::atkdebug('Undefined return: '.$this->atkNodeUri().".$trigger doesn't return anything, it should return a boolean!",
                    Tools::DEBUG_WARNING);
                $return = true;
            }

            if (!$return) {
                Tools::atkdebug($this->atkNodeUri().".$trigger failed!");

                return false;
            }

            foreach ($this->m_triggerListeners as $listener) {
                $return = $listener->notify($trigger, $record, $mode);

                if ($return === null) {
                    Tools::atkdebug('Undefined return: '.$this->atkNodeUri().', '.get_class($listener).".notify('$trigger', ...) doesn't return anything, it should return a boolean!",
                        Tools::DEBUG_WARNING);
                    $return = true;
                }

                if (!$return) {
                    Tools::atkdebug($this->atkNodeUri().', '.get_class($listener).".notify('$trigger', ...) failed!");

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
    public function deleteDb($selector, $exectrigger = true, $failwhenempty = false)
    {
        $recordset = $this->select($selector)->mode('delete')->fetchAll();

        // nothing to delete, throw an error (determined by $failwhenempty)!
        if (Tools::count($recordset) == 0) {
            Tools::atkwarning($this->atkNodeUri()."->deleteDb({$selector->sql}): 0 records found, not deleting anything.");

            return !$failwhenempty;
        }

        if ($exectrigger) {
            foreach ($recordset as $record) {
                $return = $this->executeTrigger('preDelete', $record);
                if (!$return) {
                    return false;
                }
            }
        }

        // delete on "cascading" attributes (like relations, file attribute) BEFORE the query execution
        foreach ($this->m_cascadingAttribs as $attribname) {
            $p_attrib = $this->m_attribList[$attribname];
            foreach ($recordset as $record) {
                if (isset($record[$attribname]) && !$p_attrib->isEmpty($record)) {
                    if (!$p_attrib->delete($record)) {
                        // error
                        return false;
                    }
                }
            }
        }

        $query = $this->getDb()->createQuery($this->m_table);
        $query->addCondition($selector);
        if (!$query->executeDelete()) {
            return false;
        }

        // postDelete on "cascading" attributes (like relations, file attribute) AFTER the query execution
        foreach ($this->m_cascadingAttribs as $attribname) {
            $p_attrib = $this->m_attribList[$attribname];
            foreach ($recordset as $record) {
                if (isset($record[$attribname]) && !$p_attrib->isEmpty($record)) {
                    if (!$p_attrib->postDelete($record)) {
                        // error
                        return false;
                    }
                }
            }
        }

        if ($exectrigger) {
            foreach ($recordset as $record) {
                $return = ($this->executeTrigger('postDel', $record) && $this->executeTrigger('postDelete', $record));
                if (!$return) {
                    return false;
                }
            }
        }

        return true;
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
     *
     * @return bool True if succesful, false if not.
     */
    public function postAdd($record, $mode = 'add')
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
    public function preAdd(&$record, $mode = 'add')
    {
        // Do nothing
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
     *
     * @return bool True if succesful, false if not.
     */
    public function postUpdate($record)
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
     *
     * @return bool Wether or not we succeeded in what we wanted to do.
     */
    public function preUpdate(&$record)
    {
        // Do nothing
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
    public function preDelete($record)
    {
        return true;
    }

    /**
     * Deprecated function that is called by the framework,
     * right after a record has been deleted.
     * Please use postDelete() instead.
     *
     * @param array $record The record that has just been deleted.
     *
     * @return bool Wether or not we succeeded in what we wanted to do.
     */
    public function postDel($record)
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
     *
     * @return bool Wether or not we succeeded in what we wanted to do.
     */
    public function postDelete($record)
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
    public function preCopy(&$record)
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
    public function recordActions($record, &$actions, &$mraactions)
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
    public function registerRecordActionsCallback($callback)
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
    public function collectRecordActions($record, &$actions, &$mraactions)
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
    public function securityKey($action)
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
    public function getType()
    {
        return $this->m_type;
    }

    /**
     * Returns the module name for this node.
     *
     * @return string node
     */
    public function getModule()
    {
        return $this->m_module;
    }

    /**
     * Returns the current action for this node.
     *
     * @return string action
     */
    public function getAction()
    {
        return $this->m_action;
    }

    /**
     * Get the full node Uri of this node (module.type notation).  This is sometimes
     * referred to as the node name (or nodename) or node string.
     *
     * @return string The nodeUri of the node.
     */
    public function atkNodeUri()
    {
        return (empty($this->m_module) ? '' : $this->m_module.'.').$this->m_type;
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
    public function allowed($action, $record = array())
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
    public function resolveNodeTypeAndAction(&$alias, &$action)
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
                $alias = $complete[0].'.'.$complete[1];
                $action = $complete[2];
            } else {
                $alias = $this->m_module.'.'.$complete[0];
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
    public function setSecurityAlias($alias)
    {
        $this->m_securityAlias = $alias;
    }

    /**
     * Returns the node's security alias (if set).
     *
     * @return string security alias
     */
    public function getSecurityAlias()
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
    public function addAllowedAction($action)
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
    public function callHandler($action)
    {
        Tools::atkdebug('self::callHandler(); action: '.$action);
        $atk = Atk::getInstance();
        $handler = $atk->atkGetNodeHandler($this->atkNodeUri(), $action);

        // handler function
        if ($handler != null && is_string($handler) && function_exists($handler)) {
            Tools::atkdebug("self::callHandler: Calling external handler function for '".$action."'");
            $handler($this, $action);
        } // handler object
        elseif ($handler != null && $handler instanceof ActionHandler) {
            Tools::atkdebug('self::callHandler:Using override/existing ActionHandler '.get_class($handler)." class for '".$action."'");
            $handler->handle($this, $action, $this->m_postvars);
        } // no (valid) handler
        else {
            Tools::atkdebug("Calling default handler function for '".$action."'");
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
    public function getHandler($action)
    {
        Tools::atkdebug('self::getHandler(); action: '.$action);

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
            Tools::atkdebug('self::getHandler: Using existing ActionHandler '.get_class($handler)." class for '".$action."'");
            $handler->setNode($this);
            $handler->setAction($action);
        } else {
            $handler = ActionHandler::getDefaultHandler($action);

            $handler->setNode($this);
            $handler->setPostvars($this->m_postvars);
            $handler->setAction($action);

            //If we use a default handler we need to register it to this node
            //because we might call it a second time.
            Tools::atkdebug('self::getHandler: Register default ActionHandler for '.$this->m_type." action: '".$action."'");
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
     * @param string $action The name of the action.
     */
    public function setSearchAction()
    {
        $this->m_search_action = func_get_args();
    }

    /**
     * This function resorts the attribIndexList and attribList.
     *
     * This is necessary if you add attributes *after* init() is already
     * called, and you set an order for those attributes.
     */
    public function attribSort()
    {
        uasort($this->m_attribList, function ($attrib1, $attrib2) {
                return $attrib1->getOrder() <=> $attrib2->getOrder();
            });
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
    public function searchDb($expression, $searchmethod = 'OR')
    {
        // Set default searchmethod to OR (put it in m_postvars, because selectDb
        // will use m_postvars to built it's search conditions).
        $this->m_postvars['atksearchmethod'] = $searchmethod;

        // To perform the search, we fill atksearch, so selectDb automatically
        // searches. Because an atksearch variable may have already been set,
        // we save it to restore it after the query.
        $orgsearch = Tools::atkArrayNvl($this->m_postvars, 'atksearch');

        // Built whereclause.
        foreach ($this->m_attribList as $attribname => $p_attrib) {
            // Only search in fields that aren't explicitly hidden from search
            if (!$p_attrib->hasFlag(Attribute::AF_HIDE_SEARCH) && ($p_attrib->dbFieldType() == Db::FT_STRING || $p_attrib->hasFlag(Attribute::AF_SEARCHABLE))
            ) {
                $this->m_postvars['atksearch'][$attribname] = $expression;
            }
        }

        // We load records in admin mode, se we are certain that all fields are added.
        $recs = $this->select()->excludes($this->m_listExcludes)->mode('admin')->fetchAll();

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
     * @param int $levelskip Number of levels to skip
     *
     * @return string The feedback url.
     */
    public function feedbackUrl($action, $status, $record = [], $message = '', $levelskip = null)
    {
        $sm = SessionManager::getInstance();
        $vars = [];
        $atkNodeUri = '';
        $sessionStatus = SessionManager::SESSION_BACK;

        if ((isset($this->m_feedback[$action]) && Tools::hasFlag($this->m_feedback[$action], $status)) || $status == ActionHandler::ACTION_FAILED) {
            $vars = array(
                'atkaction' => 'feedback',
                'atkfbaction' => $action,
                'atkactionstatus' => $status,
                'atkfbmessage' => $message,
            );
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
     * @param mixed $number the number that the first attribute begins with
     */
    public function setNumbering($number = 1)
    {
        $this->m_numbering = $number;
    }

    /**
     * Gets the numbering of the attributes.
     *
     * @return mixed the number whith which the numbering starts
     */
    public function getNumbering()
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
    public function addSecurityMap($action, $mapped = '')
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
    public function changeMapping($oldmapped, $newmapped)
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
    public function addListener($listener)
    {
        $listener->setNode($this);

        if ($listener instanceof ActionListener) {
            $this->m_actionListeners[] = $listener;
        } else {
            if ($listener instanceof TriggerListener) {
                $this->m_triggerListeners[] = $listener;
            } else {
                Tools::atkdebug('self::addListener: Unknown listener base class '.get_class($listener));
            }
        }
    }

    /**
     * Notify all listeners of the occurance of a certain action.
     *
     * @param string $action The action that occurred
     * @param array $record The record on which the action was performed
     */
    public function notify($action, $record)
    {
        foreach ($this->m_actionListeners as $listener) {
            $listener->notify($action, $record);
        }
    }

    /**
     * Notify all listeners in advance of the occurance of a certain action.
     *
     * @param string $action The action that will occur
     * @param array $record The record on which the action will be performed
     */
    public function preNotify($action, &$record)
    {
        foreach ($this->m_actionListeners as $listener) {
            $listener->preNotify($action, $record);
        }
    }

    /**
     * Get the column configuration object.
     *
     * @param string $id optional column config id
     * @param bool $forceNew force new instance?
     *
     * @return ColumnConfig
     */
    public function getColumnConfig($id = null, $forceNew = false)
    {
        $columnConfig = ColumnConfig::getConfig($this, $id, $forceNew);

        return $columnConfig;
    }

    /**
     * Translate using this node's module and type.
     *
     * @param string|array $string string or array of strings containing the name(s) of the string to return
     *                              when an array of strings is passed, the second will be the fallback if
     *                              the first one isn't found, and so forth
     * @param string $module module in which the language file should be looked for,
     *                              defaults to core module with fallback to ATK
     * @param string $lng ISO 639-1 language code, defaults to config variable
     * @param string $firstfallback the first module to check as part of the fallback
     * @param bool $nodefaulttext if true, then it doesn't return a default text
     *                              when it can't find a translation
     *
     * @return string the string from the languagefile
     */
    public function text($string, $module = null, $lng = '', $firstfallback = '', $nodefaulttext = false)
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
    public function __toString()
    {
        return $this->atkNodeUri();
    }

    /**
     * Set the edit fieldprefix to use in atk.
     *
     * @param string $prefix
     */
    public function setEditFieldPrefix($prefix)
    {
        $this->m_edit_fieldprefix = $prefix;
    }

    /**
     * Get the edit fieldprefix to use.
     *
     * @param bool $atk_layout do we want the prefix in atkstyle (with _AE_) or not
     *
     * @return string with edit fieldprefix
     */
    public function getEditFieldPrefix($atk_layout = true)
    {
        if ($this->m_edit_fieldprefix == '') {
            return '';
        } else {
            return $this->m_edit_fieldprefix.($atk_layout ? '_AE_' : '');
        }
    }

    /**
     * Row CSS class.
     *
     * Used to determine the CSS class(s) for rows in the datagrid list.
     *
     * @param array $record record
     * @param int $nr row number
     *
     * @return string CSS class(es)
     */
    public function rowClass($record, $nr)
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
     */
    public function setRowClassCallback($callback)
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
    public function getRowClassCallback()
    {
        return $this->m_rowClassCallback;
    }

    /**
     * Adds a flag to a list of attributes.
     *
     * @param array $attrsNames The names of attributes
     * @param int $flag The flag to add to the attributes
     * @param bool $check Check the presence of the attributes
     */
    public function addAttributesFlag($attrsNames, $flag, $check = false)
    {
        foreach ($attrsNames as $name) {
            $attr = $this->getAttribute($name);
            if (!$check || $attr) {
                $attr->addFlag($flag);
            }
        }
    }

    /**
     * Removes a flag from a list of attributes.
     *
     * @param array $attrsNames The names of attributes
     * @param int $flag The flag to remove from the attributes
     * @param bool $check Check the presence of the attributes
     */
    public function removeAttributesFlag($attrsNames, $flag, $check = false)
    {
        foreach ($attrsNames as $name) {
            $attr = $this->getAttribute($name);
            if (!$check || $attr) {
                $attr->removeFlag($flag);
            }
        }
    }
}

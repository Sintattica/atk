<?php

namespace Sintattica\Atk\DataGrid;

use Sintattica\Atk\Core\Config;
use Sintattica\Atk\Core\Node;
use Sintattica\Atk\Core\Tools;
use Sintattica\Atk\Session\SessionManager;
use Sintattica\Atk\Utils\Json;
use Sintattica\Atk\Utils\StringParser;

/**
 * The data grid is a component based record list container.
 *
 * It's main purpose is gathering the information needed for rendering the
 * grid. The components, like for example atkDGList, are responsible for
 * rendering the list, the pagination etc.
 *
 * The grid has built-in Ajax support for updating the grid contents. Most of
 * the times updates are triggered by one of the grid components (for example
 * a pagination link). However, the grid also supports external update
 * triggers. For more information see atk/scripts/class.atkdatagrid.js.
 *
 * @author Peter C. Verhage <peter@achievo.org>
 */
class DataGrid
{
    /**
     * Enable sorting for the datagrid.
     */
    const SORT = 1;

    /**
     * Enable extended sort for the datagrid.
     */
    const EXTENDED_SORT = 2;

    /**
     * Enable searching the datagrid.
     */
    const SEARCH = 4;

    /**
     * Enable extended search for the datagrid.
     */
    const EXTENDED_SEARCH = 8;

    /**
     * Enable multi-record actions for the datagrid.
     */
    const MULTI_RECORD_ACTIONS = 16;

    /**
     * Enable multi-record priority actions for the datagrid.
     */
    const MULTI_RECORD_PRIORITY_ACTIONS = 32;

    /**
     * Create mode.
     *
     * @private
     */
    const CREATE = 1;

    /**
     * Resume mode.
     *
     * @private
     */
    const RESUME = 2;

    /**
     * Grid modifiers.
     *
     * @var array
     */
    private static $s_modifiers;

    /**
     * Name.
     *
     * @var string
     */
    private $m_name;

    /**
     * Node.
     *
     * @var Node
     */
    private $m_node;

    /**
     * Master Node.
     *
     * @var Node
     */
    private $m_masterNode;

    /**
     * Flags.
     *
     * @var int
     */
    private $m_flags;

    /**
     * Form name.
     *
     * @var string
     */
    private $m_formName;

    /**
     * Embedded in edit form?
     *
     * @return bool
     */
    private $m_embedded = false;

    /**
     * Base URL for updating the data grid using an Ajax call.
     *
     * @var string
     */
    private $m_baseUrl;

    /**
     * Are we updating the grid?
     *
     * @var bool
     */
    private $m_update;

    /**
     * Postvars for this datagrid.
     *
     * @var array
     */
    private $m_postvars;

    /**
     * List of datagrid component definitions.
     *
     * @var array
     */
    private $m_components = [];

    /**
     * List of datagrid component instances.
     *
     * @var DataGridComponent[]
     */
    private $m_componentInstances = [];

    /**
     * Fields that should be excluded from the list.
     *
     * @var array
     */
    private $m_excludes = [];

    /**
     * Default record actions.
     *
     * @var array
     */
    private $m_defaultActions;

    /**
     * Default limit.
     *
     * @var int
     */
    private $m_defaultLimit;

    /**
     * Default order by.
     *
     * @var string
     */
    private $m_defaultOrderBy;

    /**
     * Main datagrid template.
     *
     * @var string
     */
    private $m_template;

    /**
     * Node which handles the extended search and multi-record actions.
     * Normally this is the same node as the grid node.
     *
     * @var Node
     */
    private $m_actionNode;

    /**
     * Multi-record-action selection mode.
     *
     * @var int
     */
    private $m_mraSelectionMode;

    /**
     * Action session status for record actions.
     * Normally record actions are nested.
     *
     * @var int
     */
    private $m_actionSessionStatus;

    /**
     * Node filters. Only used when the select handler and count
     * handlers are not overridden.
     *
     * @var array
     */
    private $m_filters = [];

    /**
     * Selection mode.
     *
     * @var string
     */
    private $m_mode;

    /**
     * Master record.
     *
     * @var array
     */
    private $m_masterRecord;

    /**
     * Method/function for retrieving the records for the current page.
     *
     * @var mixed
     */
    private $m_selectHandler;

    /**
     * Method/function for retrieving the total record count.
     *
     * @var mixed
     */
    private $m_countHandler;

    /**
     * Records loaded?
     *
     * @var bool
     */
    private $m_recordsLoaded;

    /**
     * Records (for the current page).
     *
     * @var array
     */
    private $m_records;

    /**
     * Total record count.
     *
     * @var int
     */
    private $m_count;

    /**
     * Grid listeners.
     *
     * @var array
     */
    private $m_listeners = [];

    /**
     * The number of times we "tried" to override the node postvars.
     *
     * @see DataGrid::overrideNodePostvars
     *
     * @var int
     */
    private $m_overrideNodePostvarsLevel = 0;

    /**
     * Backup of the original node postvars in case the postvars
     * have been overriden.
     *
     * @see DataGrid::overrideNodePostvars
     *
     * @var array
     */
    private $m_overrideNodePostvarsBackup = null;

    /**
     * Use session to store the properties of this datagrid?
     *
     * @var bool
     */
    private $m_useSession;

    /**
     * Session manager. We store a reference to the session manager in our
     * object instance variable to make sure we have access to it in the
     * destructor.
     *
     * @var SessionManager
     */
    private $m_sessionMgr;

    /**
     * Session data.
     *
     * @var array
     */
    private $m_session;

    /**
     * Destroyed?
     *
     * @var bool
     */
    private $m_destroyed = false;

    /**
     * Default Multi Record Action.
     *
     * @var string
     */
    private $m_mraDefaultAction = null;


    private $m_displayTopInfo;
    private $m_displayBottomInfo;

    /**
     * Create a new DataGrid instance.
     *
     * @param Node $node node
     * @param string $name name (will be auto-generated if left empty)
     * @param string $class class (by default the DataGrid class)
     * @param bool $isEmbedded is embedded?
     * @param bool $useSession use session
     *
     * @return DataGrid datagrid instance
     */
    public static function create(Node $node, $name = null, $class = null, $isEmbedded = false, $useSession = true)
    {
        $useSession = $useSession && SessionManager::getInstance() != null;
        $name = $name == null ? uniqid('atkdatagrid') : $name;
        $class = $class == null ? Config::getGlobal('datagrid_class') : $class;
        $sessions = &$GLOBALS['ATK_VARS']['atkdgsession'];
        $sessions[$name] = array('class' => $class, 'custom' => [], 'system' => array());
        if ($useSession) {
            SessionManager::getInstance()->pageVar('atkdgsession', $sessions);
        }
        $class = substr($class, strrpos($class, '.') + 1);
        $grid = new $class($node, $name, self::CREATE, $isEmbedded, $useSession);
        self::callModifiers($grid, self::CREATE);

        return $grid;
    }

    /**
     * Resume datagrid operations.
     *
     * Most of the datagrid parameters are simply retrieved from the session
     * (including the datagrid class). You can however configure the datagrid
     * even more by adjusting options on the object returned. If the session
     * manager does not exist, this method will fail!
     *
     * @param Node $node datagrid node
     * @throws \Exception
     * @return DataGrid datagrid instance
     */
    public static function resume(Node $node)
    {
        // Cannot resume from session.
        if (!isset($GLOBALS['ATK_VARS']['atkdatagrid'])) {
            throw new \Exception('No last known datagrid!');
        }

        $name = $GLOBALS['ATK_VARS']['atkdatagrid'];

        if (!isset($GLOBALS['ATK_VARS']['atkdgsession'][$name])) {
            throw new \Exception('No session data for grid: '.$name);
        }
        $session = $GLOBALS['ATK_VARS']['atkdgsession'][$name];

        $class = &$session['class'];

        $class = substr($class, strrpos($class, '.') + 1);
        $grid = new $class($node, $name, self::RESUME);
        self::callModifiers($grid, self::RESUME);

        return $grid;
    }

    /**
     * Constructor.
     *
     * @param Node $node datagrid node
     * @param string $name datagrid name
     * @param int $mode creation mode
     * @param bool $isEmbedded is embedded?
     * @param bool $useSession use session?
     */
    public function __construct(Node $node, $name, $mode = self::CREATE, $isEmbedded = false, $useSession = true)
    {
        $this->setName($name);
        $this->setNode($node);
        $this->setActionNode($this->getNode());
        $this->setEmbedded($isEmbedded);

        $this->m_useSession = $useSession;
        $this->m_sessionMgr = $useSession ? SessionManager::getInstance() : null;

        $this->registerGlobalOverrides();
        $this->setUpdate($mode == self::RESUME);

        if (!$this->isEmbedded() && empty($node->m_postvars)) {
            $allVars = $GLOBALS['ATK_VARS'];
        } else {
            $allVars = (array)$node->m_postvars;
        }

        $vars = isset($GLOBALS['ATK_VARS']['atkdg'][$name]) ? $GLOBALS['ATK_VARS']['atkdg'][$name] : null;

        $vars = !is_array($vars) ? [] : $vars;
        $this->setPostvars(array_merge($allVars, $vars));

        $this->loadSession();

        if ($mode == self::RESUME) {
            $this->initOnResume();
        } else {
            $this->initOnCreate();
        }
    }

    /**
     * Initialize when we create the datagrid for the first time.
     */
    protected function initOnCreate()
    {
        $this->setFlags($this->convertNodeFlags($this->getNode()->getFlags()));
        $this->setBaseUrl(Tools::partial_url($this->getNode()->atkNodeUri(), $this->getNode()->m_action, 'datagrid'));

        $this->setDefaultLimit(Config::getGlobal('recordsperpage'));
        $this->setDefaultActions($this->getNode()->defaultActions('admin'));
        $this->setDefaultOrderBy($this->getNode()->getOrder());
        $this->setTemplate('datagrid.tpl');
        $this->setActionSessionStatus(SessionManager::SESSION_NESTED);
        $this->setMode('admin');
        $this->setMRASelectionMode($this->getNode()->getMRASelectionMode());

        $this->setDisplayTopInfo(Config::getGlobal('datagrid_display_top_info'));
        $this->setDisplayBottomInfo(Config::getGlobal('datagrid_display_bottom_info'));

        if (!$this->getNode()->hasFlag(Node::NF_NO_FILTER)) {
            foreach ($this->getNode()->m_filters as $key => $value) {
                $this->addFilter($key."='".$value."'");
            }

            foreach ($this->getNode()->m_fuzzyFilters as $filter) {
                $parser = new StringParser($filter);
                $filter = $parser->parse(array('table' => $this->getNode()->getTable()));
                $this->addFilter($filter);
            }
        }

        $this->addComponent('list', __NAMESPACE__.'\\DataGridList');
        $this->addComponent('summary', __NAMESPACE__.'\\DataGridSummary');
        $this->addComponent('limit', __NAMESPACE__.'\\DataGridLimit', array('showAll' => Config::getGlobal('enable_showall')));
        $this->addComponent('norecordsfound', __NAMESPACE__.'\\DataGridNoRecordsFound');
        $this->addComponent('paginator', __NAMESPACE__.'\\DataGridPaginator');

        if (!empty($this->getNode()->m_index)) {
            $this->addComponent('index', __NAMESPACE__.'\\DataGridIndex');
        }

        if (count($this->getNode()->m_editableListAttributes) > 0) {
            $this->addComponent('editcontrol', __NAMESPACE__.'\\DataGridEditControl');
        }
    }

    /**
     * Initialize when we resume DataGrid operations from a partial request.
     */
    protected function initOnResume()
    {
        foreach ($this->m_session['system'] as $var => $value) {
            $fullVar = "m_{$var}";
            $this->$fullVar = $value;
        }
    }

    /**
     * Destructor.
     */
    public function __destruct()
    {
        if ($this->isDestroyed()) {
            return;
        }
        $this->storePostvars();
        $this->storeSession();
    }

    /**
     * Destroys the datagrid. Will remove all references to it
     * from the session and will make sure it won't be written
     * to the session later on.
     */
    public function destroy()
    {
        $this->m_destroyed = true;

        $sessions = &$GLOBALS['ATK_VARS']['atkdg'];
        unset($sessions[$this->getName()]);
        if ($this->m_useSession) {
            $this->m_sessionMgr->pageVar('atkdg', $sessions);
        }

        $sessions = &$GLOBALS['ATK_VARS']['atkdgsession'];
        unset($sessions[$this->getName()]);
        if ($this->m_useSession) {
            $this->m_sessionMgr->pageVar('atkdgsession', $sessions);
        }

        foreach ($this->m_componentInstances as $comp) {
            $comp->destroy();
        }

        $this->m_componentInstances = [];
        $this->m_listeners = [];
        $this->m_records = null;
    }

    /**
     * Is this grid destroyed?
     *
     * @return bool is destroyed?
     */
    public function isDestroyed()
    {
        return $this->m_destroyed;
    }

    /**
     * It's allowed to use the request variables atkstartat, atklimit, atksearch,
     * atksmartsearch, atksearchmode, atkorderby, atkindex and atkcolcmd directly. If they
     * are used directly we need to store their values in the datagrid session
     * entry and override existing values if needed.
     */
    protected function registerGlobalOverrides()
    {
        if ($this->isEmbedded()) {
            return;
        }

        $request = array_merge($_GET, $_POST);
        Tools::atkDataDecode($request);

        $vars = array(
            'atkstartat',
            'atklimit',
            'atksearch',
            'atksmartsearch',
            'atksearchmode',
            'atkorderby',
            'atkindex',
            'atkcolcmd',
        );

        $sessions = &$GLOBALS['ATK_VARS']['atkdg'];
        if ($sessions == null) {
            $sessions = [];
        }

        foreach ($vars as $var) {
            if (isset($request[$var])) {
                $sessions[$this->getName()][$var] = $request[$var];
            } // fix grid searching problem after extended search
            else {
                if (isset($request['atkdg'][$this->getName()][$var])) {
                    $sessions[$this->getName()][$var] = $request['atkdg'][$this->getName()][$var];
                }
            }
        }

        $this->getNode()->m_postvars['atkdg'] = $sessions;
        if ($this->m_useSession) {
            $this->m_sessionMgr->pageVar('atkdg', $sessions);
        }
    }

    /**
     * The postvars atkstartat, atklimit, atksearch, atksmartsearch,
     * atksearchmode, atkorderby, atkindex and atkcolcmd might be
     * overriden using setPostvar. Save the latest values in the session.
     */
    protected function storePostvars()
    {
        $sessions = &$GLOBALS['ATK_VARS']['atkdg'];
        $vars = array(
            'atkstartat',
            'atklimit',
            'atksearch',
            'atksmartsearch',
            'atksearchmode',
            'atkorderby',
            'atkindex',
            'atkcolcmd',
        );
        foreach ($vars as $var) {
            if (isset($this->m_postvars[$var])) {
                $sessions[$this->getName()][$var] = $this->m_postvars[$var];
            }
        }

        $this->getNode()->m_postvars['atkdg'] = $sessions;
        if ($this->m_useSession) {
            $this->m_sessionMgr->pageVar('atkdg', $sessions);
        }
    }

    /**
     * Load datagrid properties and custom data from the session.
     *
     * @return bool data retrieved from the session?
     */
    protected function loadSession()
    {
        $this->m_session = &$GLOBALS['ATK_VARS']['atkdgsession'][$this->getName()];
    }

    /**
     * Store datagrid properties and custom data in the session.
     */
    protected function storeSession()
    {
        $this->m_session['system'] = [];

        $vars = array(
            'flags',
            'formName',
            'embedded',
            'baseUrl',
            'components',
            'excludes',
            'defaultActions',
            'defaultLimit',
            'defaultOrderBy',
            'template',
            'actionSessionStatus',
            'filters',
            'mode',
            'mraSelectionMode',
            'countHandler',
            'selectHandler',
            'masterRecord',
            'displayTopInfo',
            'displayBottomInfo',
        );

        foreach ($vars as $var) {
            $fullVar = "m_{$var}";
            $this->m_session['system'][$var] = $this->$fullVar;
        }

        $sessions = &$GLOBALS['ATK_VARS']['atkdgsession'];
        $sessions[$this->getName()] = $this->m_session;
        if ($this->m_useSession) {
            $this->m_sessionMgr->pageVar('atkdgsession', $sessions);
        }
    }

    /**
     * Returns the session.
     *
     * @return array session
     */
    public function &getSession()
    {
        return $this->m_session['custom'];
    }

    /**
     * Returns the grid name.
     *
     * @return string grid name
     */
    public function getName()
    {
        return $this->m_name;
    }

    /**
     * Sets the grid name.
     *
     * @param string $name grid name
     */
    protected function setName($name)
    {
        $this->m_name = $name;
    }

    /**
     * Returns the grid node.
     *
     * @return Node grid node
     */
    public function getNode()
    {
        return $this->m_node;
    }

    /**
     * Sets the grid node.
     *
     * @param Node $node grid node
     */
    protected function setNode($node)
    {
        $this->m_node = $node;
    }

    /**
     * Converts node flags to datagrid flags.
     *
     * @param int $nodeFlags The nodeflags to convert
     *
     * @return int flags
     */
    protected function convertNodeFlags($nodeFlags)
    {
        $flags = !Tools::hasFlag($nodeFlags, Node::NF_NO_SORT) ? self::SORT : 0;
        $flags |= Tools::hasFlag($nodeFlags, Node::NF_EXT_SORT) ? self::EXTENDED_SORT : 0;
        $flags |= !Tools::hasFlag($nodeFlags, Node::NF_NO_SEARCH) ? self::SEARCH : 0;
        $flags |= !Tools::hasFlag($nodeFlags, Node::NF_NO_EXTENDED_SEARCH) ? self::EXTENDED_SEARCH : 0;
        $flags |= Tools::hasFlag($nodeFlags, Node::NF_MRA) ? self::MULTI_RECORD_ACTIONS : 0;
        $flags |= Tools::hasFlag($nodeFlags, Node::NF_MRPA) ? self::MULTI_RECORD_PRIORITY_ACTIONS : 0;

        return $flags;
    }

    /**
     * Returns the grid flags.
     *
     * @return int grid flags.
     */
    public function getFlags()
    {
        return $this->m_flags;
    }

    /**
     * Sets the grid flags.
     *
     * @param int $flags grid flags
     */
    public function setFlags($flags)
    {
        $this->m_flags = $flags;
    }

    /**
     * Adds the given grid flag(s).
     *
     * @param int $flag grid flag(s)
     */
    public function addFlag($flag)
    {
        $this->m_flags |= $flag;
    }

    /**
     * Removes the given grid flag(s).
     *
     * @param int $flag grid flag(s)
     */
    public function removeFlag($flag)
    {
        $this->m_flags = ($this->m_flags | $flag) ^ $flag;
    }

    /**
     * Is/are the given flag(s) set for this grid?
     *
     * @param int $flag grid flag(s)
     *
     * @return bool flag(s) is/are set
     */
    public function hasFlag($flag)
    {
        return Tools::hasFlag($this->m_flags, $flag);
    }

    /**
     * Returns the form name.
     *
     * @return string form name
     */
    public function getFormName()
    {
        return $this->m_formName;
    }

    /**
     * Sets the form name.
     *
     * @param string $name form name
     */
    protected function setFormName($name)
    {
        $this->m_formName = $name;
    }

    /**
     * Sets whatever the grid is embedded in an edit form.
     *
     * If set to true and no form name is specified yet a default form name
     * of "entryform" is used.
     *
     * @param bool $embedded embedded in edit form?
     */
    public function setEmbedded($embedded)
    {
        $this->m_embedded = $embedded;
    }

    /**
     * Is embedded in edit form?
     *
     * @return bool embedded in edit form?
     */
    public function isEmbedded()
    {
        return $this->m_embedded;
    }

    /**
     * Returns the base URL for Ajax update requests for this grid.
     *
     * @return string base url
     */
    public function getBaseUrl()
    {
        return $this->m_baseUrl;
    }

    /**
     * Sets the base URL for Ajax update requests for this grid.
     *
     * @param string $url base url
     */
    public function setBaseUrl($url)
    {
        $this->m_baseUrl = $url;
    }

    /**
     * Set whatever we are updating the grid (or are rendering it
     * for the first time).
     *
     * @param bool $update are we updating the grid?
     */
    public function setUpdate($update)
    {
        $this->m_update = $update;
    }

    /**
     * Are we updating the grid (or rendering it for the first time)?
     *
     * @return bool are we updating the grid?
     */
    public function isUpdate()
    {
        return $this->m_update;
    }

    /**
     * Sets the postvars.
     *
     * @param array $postvars postvars
     */
    public function setPostvars($postvars)
    {
        if ($postvars == null) {
            $postvars = [];
        }

        $this->m_postvars = $postvars;
    }

    /**
     * Sets the postvar with the given name to the given value.
     *
     * @param string $name name
     * @param mixed $value value
     */
    public function setPostvar($name, $value)
    {
        $this->m_postvars[$name] = $value;
    }

    /**
     * Returns the postvars.
     *
     * @return array postvars
     */
    public function getPostvars()
    {
        return $this->m_postvars;
    }

    /**
     * Returns the value of the postvar with the given name. If the postvar is
     * not set an optional default value will be returned instead.
     *
     * @param string $name
     * @param mixed $fallback
     *
     * @return mixed
     */
    public function getPostvar($name, $fallback = null)
    {
        return isset($this->m_postvars[$name]) ? $this->m_postvars[$name] : $fallback;
    }

    /**
     * Are we currently in edit mode?
     *
     * @return bool are we in edit mode?
     */
    public function isEditing()
    {
        return $this->getPostvar('atkgridedit', false);
    }

    /**
     * Returns the master node.
     *
     * @see DataGrid::setMasterNode
     *
     * @return Node master node
     */
    public function getMasterNode()
    {
        return $this->m_masterNode;
    }

    /**
     * Sets the master node if the grid is used in a 1:n relationship.
     *
     * This node is not directly used by the datagrid itself, but
     * components might.
     *
     * @param Node $node master node
     */
    public function setMasterNode(Node $node)
    {
        $this->m_masterNode = $node;
    }

    /**
     * Returns the master record.
     *
     * @see DataGrid::setMasterRecord
     *
     * @return array master record
     */
    public function getMasterRecord()
    {
        return $this->m_masterRecord;
    }

    /**
     * Sets the master record if the grid is used in a 1:n relationship.
     *
     * This record is not directly used by the datagrid itself, but
     * components might.
     *
     * @param array $record master record
     */
    public function setMasterRecord(array $record)
    {
        $this->m_masterRecord = $record;
    }

    /**
     * Returns the grid components.
     *
     * The associative array returned contains for each named component
     * the component class and options.
     *
     * @see DataGrid::getComponents
     *
     * @return array components
     */
    public function getComponents()
    {
        return $this->m_components;
    }

    /**
     * Returns the component with the given name.
     *
     * The component information array returned contains an entry
     * 'class' which contains the component's full ATK class name
     * and 'options' containg the component options.
     *
     * @param string $name The name of the component
     *
     * @return array component information
     */
    public function getComponent($name)
    {
        return @$this->m_components[$name];
    }

    /**
     * Add component for this grid. The component must be specified using it's
     * full ATK class name. The constructor of the component must accept a
     * grid instance and an options array.
     *
     * @see DGComponent::__construct
     *
     * @param string $name name
     * @param string $class class name
     * @param array $options component options
     */
    public function addComponent($name, $class, $options = array())
    {
        $this->m_components[$name] = array('class' => $class, 'options' => $options);
    }

    /**
     * Sets a component option for the given component. Only works when the
     * components haven't been instantiated  yet.
     *
     * @param string $component component name
     * @param string $option option name
     * @param string $value value
     */
    public function setComponentOption($component, $option, $value)
    {
        $this->m_components[$component]['options'][$option] = $value;
    }

    /**
     * Remove a component from the grid.
     *
     * @param string $name name
     */
    public function removeComponent($name)
    {
        unset($this->m_components[$name]);
    }

    /**
     * Instantiates the components.
     */
    protected function loadComponentInstances()
    {
        $this->m_componentInstances = [];

        foreach ($this->getComponents() as $name => $info) {
            $comp = new $info['class']($this, $info['options']);
            $this->m_componentInstances[$name] = $comp;
            if ($comp instanceof DataGridListener) {
                $this->addListener($comp);
            }
        }
    }

    /**
     * Returns the component instances as key/component array. If the
     * component are not loaded yet an empty array will be returned.
     *
     * @return array
     */
    public function getComponentInstances()
    {
        return $this->m_componentInstances;
    }

    /**
     * Returns the component with the given name. If the component doesn't
     * exist or hasn't been loaded yet, null will be returned.
     *
     * @param string $name component name
     *
     * @return DataGridComponent
     */
    public function getComponentInstance($name)
    {
        return @$this->m_componentInstances[$name];
    }

    /**
     * Returns the attributes that should be excluded from the list
     * next to the attributes that already have an Attribute::AF_HIDE_LIST flag.
     *
     * @return array excluded attributes
     */
    public function getExcludes()
    {
        return $this->m_excludes;
    }

    /**
     * Sets the attributes that should be excluded from the list
     * next to the attribute that already have an Attribute::AF_HIDE_LIST flag.
     *
     * @param array $excludes excluded attributes
     */
    public function setExcludes($excludes)
    {
        $this->m_excludes = $excludes;
    }

    /**
     * Returns the default record actions.
     *
     * @return array default record actions
     */
    public function getDefaultActions()
    {
        return $this->m_defaultActions;
    }

    /**
     * Sets the default record actions.
     *
     * @param array $actions default record actions
     */
    public function setDefaultActions($actions)
    {
        $this->m_defaultActions = $actions;
    }

    /**
     * Returns the default record limit.
     *
     * @return int default record limit
     */
    public function getDefaultLimit()
    {
        return $this->m_defaultLimit;
    }

    /**
     * Sets the default record limit.
     *
     * @param int $limit default record limit
     */
    public function setDefaultLimit($limit)
    {
        $this->m_defaultLimit = $limit;
    }

    /**
     * Returns the default order by.
     *
     * @return string default order by
     */
    public function getDefaultOrderBy()
    {
        return $this->m_defaultOrderBy;
    }

    /**
     * Sets the default order by.
     *
     * @param string $orderBy default order by
     */
    public function setDefaultOrderBy($orderBy)
    {
        $this->m_defaultOrderBy = $orderBy;
    }

    /**
     * Returns the template name.
     *
     * @return string template name
     */
    public function getTemplate()
    {
        return $this->m_template;
    }

    /**
     * Sets the datagrid template.
     *
     * @param string $template template
     */
    public function setTemplate($template)
    {
        $this->m_template = $template;
    }

    /**
     * Returns the action node.
     *
     * @see DataGrid::setActionNode
     *
     * @return Node action node
     */
    public function getActionNode()
    {
        return $this->m_actionNode;
    }

    /**
     * Sets the action node.
     *
     * The action handles the extended search and multi-record actions.
     * Normally this is the same node as the grid node.
     *
     * @param Node $node
     */
    public function setActionNode($node)
    {
        $this->m_actionNode = $node;
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
     * Sets the multi-record-action selection mode.
     *
     * @param int $mode multi-record-action selection mode
     */
    public function setMRASelectionMode($mode)
    {
        $this->m_mraSelectionMode = $mode;
    }

    /**
     * Returns the default multi-record-action.
     *
     * @return string default multi-record-action
     */
    public function getMRADefaultAction()
    {
        return $this->m_mraDefaultAction;
    }

    /**
     * Sets the default multi-record-action.
     *
     * @param string $action the default action
     */
    public function setMRADefaultAction($action)
    {
        $this->m_mraDefaultAction = $action;
    }

    /**
     * Returns the record action session status.
     *
     * @see DataGrid::setActionSessionStatus
     *
     * @return int action session status
     */
    public function getActionSessionStatus()
    {
        return $this->m_actionSessionStatus;
    }

    /**
     * Sets the record action session status.
     *
     * Normally record actions are nested.
     *
     * @param int $status session status (e.g. SessionManager::SESSION_NESTED etc.)
     */
    public function setActionSessionStatus($status)
    {
        $this->m_actionSessionStatus = $status;
    }

    /**
     * Returns the current node filters.
     *
     * @return array filters
     */
    public function getFilters()
    {
        return $this->m_filters;
    }

    /**
     * Remove filters.
     */
    public function removeFilters()
    {
        $this->m_filters = [];
    }

    /**
     * Remove filter.
     *
     * @param string $filter
     * @param array $params
     */
    public function removeFilter($filter, $params = array())
    {
        $key = array_search(array('filter' => $filter, 'params' => $params), $this->m_filters);

        if ($key !== false) {
            unset($this->m_filters[$key]);
            $this->m_filters = array_values($this->m_filters);
        }
    }

    /**
     * Add node filter (only used if no custom select and
     * count handlers are used!).
     *
     * @param string $filter filter / condition
     * @param array $params bind parameters
     */
    public function addFilter($filter, $params = array())
    {
        if (!empty($filter)) {
            $this->m_filters[] = array('filter' => $filter, 'params' => $params);
        }
    }

    /**
     * Returns the mode.
     *
     * @return string mode
     */
    public function getMode()
    {
        return $this->m_mode;
    }

    /**
     * Sets the mode.
     *
     * @param string $mode
     */
    public function setMode($mode)
    {
        $this->m_mode = $mode;
    }

    /**
     * Returns the current index value.
     *
     * @return string index value
     */
    public function getIndex()
    {
        return $this->getPostvar('atkindex');
    }

    /**
     * Returns the current limit.
     *
     * @return int limit
     */
    public function getLimit()
    {
        return $this->getPostvar('atklimit', $this->getDefaultLimit());
    }

    /**
     * Returns the current offset.
     *
     * @return int offset
     */
    public function getOffset()
    {
        return $this->getPostvar('atkstartat', 0);
    }

    /**
     * Returns the current order by statement.
     *
     * @return string order by
     */
    public function getOrderBy()
    {
        $orderBy = $this->getNode()->getColumnConfig($this->getName())->getOrderByStatement();
        if (empty($orderBy)) {
            $orderBy = $this->getDefaultOrderBy();
        }

        return $orderBy;
    }

    /**
     * Returns the records for the current page of the grid.
     *
     * @param bool $load load the records (if needed)
     *
     * @return array records
     */
    public function getRecords($load = false)
    {
        if ($load) {
            $this->loadRecords();
        }

        return $this->m_records;
    }

    /**
     * Sets the records for the current page.
     *
     * This method is not publicly callable because the grid controls amongst
     * others the postvars (atksearch etc.) used for retrieving the records.
     * If you want to have more control on the records retrieved please register
     * a custom select handler (and probably also a custom count handler).
     *
     * @see DataGrid::setSelectHandler
     * @see DataGrid::setCountHandler
     *
     * @param array $records records
     */
    public function setRecords($records)
    {
        $this->m_records = $records;
    }

    /**
     * Returns the total record count for the grid.
     *
     * @param bool $load load the record count (if needed)
     *
     * @return int record count
     */
    public function getCount($load = false)
    {
        if ($load) {
            $this->loadRecords();
        }

        return $this->m_count;
    }

    /**
     * Sets the record count.
     *
     * This method is not publicly callable because the grid controls amongst
     * others the postvars (atksearch etc.) used for retrieving the record count.
     * If you want to have more control on the record count please register a
     * custom count handler (and probably also a custom select handler).
     *
     * @see DataGrid::setCountHandler
     * @see DataGrid::setSelectHandler
     *
     * @param int $count record count
     */
    protected function setCount($count)
    {
        $this->m_count = $count;
    }

    /**
     * Sets a method/function which handles the record loading.
     *
     * The handler should return an array of records for the current page when
     * called and will receive the grid instance as argument.
     *
     * @param mixed $handler select handler
     */
    public function setSelectHandler($handler)
    {
        $this->m_selectHandler = $handler;
    }

    /**
     * Returns the select handler.
     *
     * @see DataGrid::setSelectHandler
     *
     * @return mixed select handler
     */
    protected function getSelectHandler()
    {
        return $this->m_selectHandler;
    }

    /**
     * Sets a method/function which handles the record count.
     *
     * The handler should return the record count when called and will receive
     * the grid instance as argument.
     *
     * @param mixed $handler count handler
     */
    public function setCountHandler($handler)
    {
        $this->m_countHandler = $handler;
    }

    /**
     * Returns the count handler.
     *
     * @see DataGrid::setCountHandler
     *
     * @return mixed count handler
     */
    protected function getCountHandler()
    {
        return $this->m_countHandler;
    }

    /**
     * Default implementation for selecting the records for the current page.
     *
     * This method uses the grid node to retrieve a list of records for the
     * current page and will take the currently set filter, order by, limit
     * etc. into account.
     *
     * @return array list of records
     */
    protected function selectRecords()
    {
        $excludes = [];

        // Ignore excludes for copy or if we don't now which mode we are in
        $mode = $this->getMode();
        if ($mode != 'copy' && !empty($mode)) {
            $excludes = $this->getNode()->m_listExcludes;
            $excludes = array_merge($excludes, $this->getExcludes());
        }

        $selector = $this->getNode()->select()->excludes($excludes)->orderBy($this->getOrderBy())->limit($this->getLimit(),
            $this->getOffset())->mode($this->getMode())->ignoreDefaultFilters();

        foreach ($this->m_filters as $filter) {
            $selector->where($filter['filter'], $filter['params']);
        }

        return $selector->getAllRows();
    }

    /**
     * Default implementation for counting the records for this grid.
     *
     * This method uses the grid node to retrieve a record count.
     *
     * @return int
     */
    protected function countRecords()
    {
        $excludes = $this->getMode() == 'copy' ? [] : $this->getNode()->m_listExcludes;
        $excludes = array_merge($excludes, $this->getExcludes());

        $selector = $this->getNode()->select()->excludes($excludes)->mode($this->getMode())->ignoreDefaultFilters();

        foreach ($this->m_filters as $filter) {
            $selector->where($filter['filter'], $filter['params']);
        }

        return $selector->getRowCount();
    }

    /**
     * Loads the grid records for the current page and retrieves the total number
     * of rows for the grid. This method is called automatically by the render()
     * method but can be called manually if necessary.
     *
     * If the records are already loaded no loading will occur unless the $force
     * parameter is set to true. If the record count is already known no new
     * record count will be retrieved, unless the $force parameter is set to true.
     *
     * @param bool $force force record and count retrieval?
     */
    public function loadRecords($force = false)
    {
        // records already loaded?
        if ($this->m_recordsLoaded && !$force) {
            return;
        }

        // load component instances, because they might be listeners
        $this->loadComponentInstances();

        // notify listeners
        $this->notify(DataGridEvent::PRE_LOAD);

        // temporarily overwrite the node postvars so that select and count
        // have access to the atksearch, atkfilter, atklimit etc. parameters
        $this->overrideNodePostvars();

        // retrieve records using the default implementation
        if ($force || ($this->getRecords() === null && $this->getSelectHandler() === null)) {
            $records = $this->selectRecords();
            $this->setRecords($records);
        } // retrieve records using a custom select handler
        else {
            if ($force || $this->getRecords() === null) {
                $records = call_user_func_array($this->getSelectHandler(), array($this));
                $this->setRecords($records);
            }
        }

        // retrieve record count using the default implementation
        if ($force || ($this->getCount() === null && $this->getCountHandler() === null)) {
            $count = $this->countRecords();
            $this->setCount($count);
        } // retrieve record count using a custom cont handler
        else {
            if ($force || $this->getCount() === null) {
                $count = call_user_func_array($this->getCountHandler(), array($this));
                $this->setCount($count);
            }
        }

        // restore previous postvars
        $this->restoreNodePostvars();

        // done loading
        $this->m_recordsLoaded = true;

        // notify listeners
        $this->notify(DataGridEvent::POST_LOAD);
    }

    /**
     * Returns a JavaScript call to update the grid using it's current
     * parameters and optionally overwriting some of the parameters with
     * the given overrides.
     *
     * The overrides are split in simple overrides (key/value array) and
     * JavaScript overrides. The simply overrides are used directly, the
     * JavaScript overrides are evaluated at run-time.
     *
     * @param array $overrides key/value overrides
     * @param array $overridesJs key/value run-time overrides
     * @param string $overridesJsCallback JavaScript function which returns an overrides Hash
     *
     * @return string JavaScript call (might need escaping when used in HTML code)
     */
    public function getUpdateCall($overrides = [], $overridesJs = [], $overridesJsCallback = 'null')
    {
        $overridesJsStr = '';

        foreach ($overridesJs as $key => $js) {
            $overridesJsStr .= (!empty($overridesJsStr) ? ', ' : '')."'$key': $js";
        }

        return 'ATK.DataGrid.update('.Json::encode($this->getName()).', '.Json::encode($overrides).', {'.$overridesJsStr.'}, '.$overridesJsCallback.');';
    }

    /**
     * Returns a JavaScript call to save the current grid's contents when in edit mode.
     *
     * @return string JavaScript call (might need escaping when used in HTML code)
     */
    public function getSaveCall()
    {
        $sm = SessionManager::getInstance();
        $url = $sm->sessionUrl(Tools::dispatch_url($this->getNode()->atkNodeUri(), 'multiupdate', array('output' => 'json')), SessionManager::SESSION_PARTIAL);

        return 'ATK.DataGrid.save('.Json::encode($this->getName()).', '.Json::encode($url).');';
    }

    /**
     * Translate the given string using the grid node.
     *
     * The value of $fallback will be returned if no translation can be found.
     * If you want NULL to be returned when no translation can be found then
     * leave the fallback empty and set $useDefault to false.
     *
     * @param string|array $string string to translate
     * @param string $fallback fallback in-case no translation can be found
     * @param bool $useDefault use default ATK translation if no translation can be found?
     *
     * @return string translation
     */
    public function text($string, $fallback = '', $useDefault = true)
    {
        return $this->getNode()->text($string, null, '', $fallback, !$useDefault);
    }

    /**
     * Add the given listener to this grid.
     *
     * @param DataGridListener $listener
     */
    public function addListener(DataGridListener $listener)
    {
        if (!array_key_exists(spl_object_hash($listener), $this->m_listeners)) {
            $this->m_listeners[spl_object_hash($listener)] = $listener;
        }
    }

    /**
     * Removes the given listener from this grid.
     *
     * @param DataGridListener $listener
     */
    public function removeListener(DataGridListener $listener)
    {
        unset($this->m_listeners[spl_object_hash($listener)]);
    }

    /**
     * Returns the listeners for this grid.
     *
     * @return DataGrid
     */
    protected function getListeners()
    {
        return array_values($this->m_listeners);
    }

    /**
     * Notify listeners of the given event.
     *
     * @see DataGridListener
     *
     * @param string $event identifier
     */
    protected function notify($event)
    {
        $event = new DataGridEvent($this, $event);

        /** @var DataGridListener $listener */
        foreach ($this->getListeners() as $listener) {
            $listener->notify($event);
        }
    }

    /**
     * (Temporarily) override the node postvars so we can apply the grid
     * specific search conditions etc.
     */
    protected function overrideNodePostvars()
    {
        $this->m_overrideNodePostvarsLevel += 1;

        // only override once
        if ($this->m_overrideNodePostvarsLevel == 1) {
            $this->m_overrideNodePostvarsBackup = $this->getNode()->m_postvars;
            $this->getNode()->m_postvars = $this->getPostvars();
        }
    }

    /**
     * Restore override node postvars.
     *
     * @see DataGrid::overrideNodePostvars
     */
    protected function restoreNodePostvars()
    {
        $this->m_overrideNodePostvarsLevel -= 1;

        if ($this->m_overrideNodePostvarsLevel == 0) {
            $this->getNode()->m_postvars = $this->m_overrideNodePostvarsBackup;
            $this->m_overrideNodePostvarsBackup = null;
        }
    }

    /**
     * Renders the grid.
     *
     * @return string grid HTML
     */
    public function render()
    {
        // load component instances
        $this->loadComponentInstances();

        // notify listeners
        $this->notify(DataGridEvent::PRE_RENDER);

        // if we are not embedded in an edit form we generate
        // the form name based on the grid name
        if (!$this->isEmbedded()) {
            $this->setFormName($this->getName().'_form');
        }

        // temporarily overwrite the node postvars so that select and count
        // have access to the atksearch, atkfilter, atklimit etc. parameters
        $this->overrideNodePostvars();

        // load records from database
        $this->loadRecords();

        // render the grid
        $renderer = new DataGridRenderer($this);
        $result = $renderer->render();

        // restore previous postvars
        $this->restoreNodePostvars();

        // notify listeners
        $this->notify(DataGridEvent::POST_RENDER);

        return $result;
    }

    /**
     * Call grid modifiers for the given grid.
     *
     * @param DataGrid $grid grid
     * @param int $mode creation mode
     */
    private static function callModifiers(DataGrid $grid, $mode)
    {
        $keys = array('*', $grid->getNode()->atkNodeUri());

        foreach ($keys as $key) {
            if (!isset(self::$s_modifiers[$key])) {
                continue;
            }

            foreach (self::$s_modifiers[$key] as $callback) {
                call_user_func($callback, $grid, $mode);
            }
        }
    }

    /**
     * Unregister datagrid modifier.
     *
     * @param string|null $nodeType node type (e.g. module.node), leave null to match all nodes
     * @param mixed $callback callback method
     */
    public static function unregisterModifier($nodeType, $callback)
    {
        self::$s_modifiers[$nodeType == null ? '*' : $nodeType] = array_diff(self::$s_modifiers[$nodeType == null ? '*' : $nodeType], array($callback));
    }

    /**
     * Register datagrid modifier.
     *
     * The modifier will be called at the end of construction time if the node
     * type matches. The first argument for the callback will be the datagrid
     * instance, the second argument the creation mode (e.g. DataGrid::CREATE
     * or DataGrid::RESUME).
     *
     * @param string|null $nodeType node type (e.g. module.node), leave null to match all nodes
     * @param mixed $callback callback method
     */
    public static function registerModifier($nodeType, $callback)
    {
        self::$s_modifiers[$nodeType == null ? '*' : $nodeType][] = $callback;
    }


    public function setDisplayTopInfo($displayTopInfo = true)
    {
        $this->m_displayTopInfo = $displayTopInfo;
    }

    public function getDisplayTopInfo()
    {
        return $this->m_displayTopInfo;
    }

    public function setDisplayBottomInfo($displayBottomInfo = true)
    {
        $this->m_displayBottomInfo = $displayBottomInfo;
    }

    public function getDisplayBottomInfo()
    {
        return $this->m_displayBottomInfo;
    }
}

<?php
/**
 * This file is part of the Achievo ATK distribution.
 * Detailed copyright and licensing information can be found
 * in the doc/COPYRIGHT and doc/LICENSE files which should be
 * included in the distribution.
 *
 * @package atk
 * @subpackage utils
 *
 * @copyright (c) 2000-2007 Ibuildings.nl BV
 * 
 * @license http://www.achievo.org/atk/licensing ATK Open Source License
 */

atkimport('atk.datagrid.atkdgcomponent');

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
 * @package atk
 * @subpackage datagrid
 */
class atkDataGrid
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
   * Enable record locking for the datagrid.
   */
  const LOCKING = 64;
  
  /**
   * Name.
   *
   * @var string
   */
  private $m_name;
  
  /**
   * Node.
   *
   * @var atkNode
   */
  private $m_node;
  
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
   * @return boolean
   */
  private $m_embedded;
  
  /**
   * Base URL for updating the data grid using an Ajax call.
   * 
   * @var string
   */
  private $m_baseUrl;
  
  /**
   * Are we updating the grid?
   * 
   * @var boolean
   */
  private $m_update;
  
  /**
   * Postvars for this datagrid.
   * 
   * @var array
   */
  private $m_postvars;
  
  /**
   * List of datagrid components.
   *
   * @var array
   */
  private $m_components = array();

  /**
   * Fields that should be excluded from the list.
   *
   * @var array
   */
  private $m_excludes = array();  
  
  /**
   * Default record actions.
   *
   * @var array
   */
  private $m_defaultActions;

  /**
   * Default limit.
   *
   * @var array
   */
  private $m_defaultLimit;  
  
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
   * @var atkNode
   */
  private $m_actionNode;
  
  /**
   * Action session status for record actions.
   * Normally record actions are nested.
   *
   * @var int
   */
  private $m_actionSessionStatus;  
  
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
   * The number of times we "tried" to override the node postvars.
   * 
   * @see atkDataGrid::overrideNodePostvars
   *
   * @var int
   */
  private $m_overrideNodePostvarsLevel = 0;
  
  /**
   * Backup of the original node postvars in case the postvars
   * have been overriden.
   * 
   * @see atkDataGrid::overrideNodePostvars
   *
   * @var array
   */
  private $m_overrideNodePostvarsBackup = null;

  /**
   * Constructor.
   *
   * @param string  $name datagrid name
   * @param atkNode $node datagrid node
   */
  public function __construct($name, $node)
  {
    $this->setName($name);    
    $this->setNode($node);

    $this->registerGlobalOverrides();
    
    $this->setFlags($this->convertNodeFlags($node->getFlags()));
    $this->setBaseUrl(partial_url($node->atkNodeType(), $node->m_action, 'datagrid'));
    $this->setUpdate($this->getNode()->isPartial());

    $vars = @$node->m_postvars['atkdg'][$name];
    $vars = !is_array($vars) ? array() : $vars;
    $this->setPostvars(array_merge($node->m_postvars, $vars));

    $this->setDefaultLimit(atkconfig('recordsperpage')); 
    $this->setDefaultActions($node->defaultActions("admin"));
    $this->setTemplate('datagrid.tpl');
    $this->setActionNode($node);
    $this->setActionSessionStatus(SESSION_NESTED);
    
    $this->addComponent('list', 'atk.datagrid.atkdglist');
    $this->addComponent('summary', 'atk.datagrid.atkdgsummary');
    $this->addComponent('limit', 'atk.datagrid.atkdglimit');    
    
    if (empty($this->getNode()->m_index))
    {
      $this->addComponent('paginator', 'atk.datagrid.atkdgpaginator');
    }
    else 
    {
      $this->addComponent('paginator', 'atk.datagrid.atkdgindex');
    }
  }
  
  /**
   * It's allowed to use the request variables atkstartat, atklimit, atksearch,
   * atksmartsearch, atksearchmode, atkorderby and atkindex directly. If they
   * are used directly we need to store their values in the datagrid session 
   * entry and override existing values if needed.
   */
  protected function registerGlobalOverrides()
  {
    $request = array_merge($_GET, $_POST);
    atkDataDecode($request);
    
    $vars = array('atkstartat', 'atklimit', 'atksearch', 'atksmartsearch', 'atksearchmode', 'atkorderby', 'atkindex');
    
    $sessionData = atkGetSessionManager()->pageVar('atkdg');
    if ($sessionData == null)
    {
      $sessionData = array();
    }
    
    foreach ($vars as $var)
    {
      if (isset($request[$var]))
      {
        $sessionData[$this->getName()][$var] = $request[$var];
      }
    }
    
    $this->getNode()->m_postvars['atkdg'] = $sessionData;
    atkGetSessionManager()->pageVar('atkdg', $sessionData);
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
   * @return atkNode grid node
   */
  public function getNode()
  {
    return $this->m_node;
  }
  
  /**
   * Sets the grid node.
   *
   * @param atkNode $node grid node
   */
  protected function setNode($node)
  {
    $this->m_node = $node;
  }
  
  /**
   * Converts node flags to datagrid flags.
   * 
   * @param int $flags
   */
  protected function convertNodeFlags($nodeFlags)
  {
    $flags  = !hasFlag($nodeFlags, NF_NO_SORT)            ? self::SORT                          : 0;
    $flags |= hasFlag($nodeFlags, NF_EXT_SORT)            ? self::EXTENDED_SORT                 : 0;
    $flags |= !hasFlag($nodeFlags, NF_NO_SEARCH)          ? self::SEARCH                        : 0;
    $flags |= !hasFlag($nodeFlags, NF_NO_EXTENDED_SEARCH) ? self::EXTENDED_SEARCH               : 0;
    $flags |= hasFlag($nodeFlags, NF_MRA)                 ? self::MULTI_RECORD_ACTIONS          : 0;
    $flags |= hasFlag($nodeFlags, NF_MRPA)                ? self::MULTI_RECORD_PRIORITY_ACTIONS : 0;
    $flags |= hasFlag($nodeFlags, NF_LOCK)                ? self::LOCKING                       : 0;
    
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
   * @param int $flags grid flag(s)
   */
  public function addFlag($flag)
  {
    $this->m_flags |= $flag;
  }
  
  /**
   * Removes the given grid flag(s).
   *
   * @param int $flags grid flag(s)
   */
  public function removeFlag($flag)
  {
    $this->m_flags = ($this->m_flags|$flag) ^ $flag;
  }
  
  /**
   * Is/are the given flag(s) set for this grid?
   *
   * @param int $flag grid flag(s)
   * 
   * @return boolean flag(s) is/are set
   */
  public function hasFlag($flag)
  {
    return hasFlag($this->m_flags, $flag);
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
   * @param boolean $embedded embedded in edit form?
   */
  public function setEmbedded($embedded)
  {
    $this->m_embedded = $embedded;
  }  
  
  /**
   * Is embedded in edit form?
   *
   * @return boolean embedded in edit form?
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
   * @param boolean $update are we updating the grid?
   */
  public function setUpdate($update)
  {
    $this->m_update = $update;
  }
  
  /**
   * Are we updating the grid (or rendering it for the first time)?
   *
   * @return boolean are we updating the grid?
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
    if ($postvars == null)
    {
      $postvars = array();  
    }

    $this->m_postvars = $postvars;
  }

  /**
   * Sets the postvar with the given name to the given value.
   *
   * @param string $name  name
   * @param mixed  $value value
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
   * @return mixed
   */
  public function getPostvar($name, $fallback=NULL)
  {
    return isset($this->m_postvars[$name]) ? $this->m_postvars[$name] : $fallback;
  }  
  
  /**
   * Returns the grid components. 
   * 
   * The associative array returned contains for each named component
   * the component class or instance. If you want all components to be
   * instantiated first specify true for the $load parameter.
   *
   * @param boolean $load instantiate all components (if needed)
   * 
   * @return array
   */
  public function getComponents($load=false)
  {
    if ($load) 
    {
      $this->loadComponents();
    }
    
    return $this->m_components;
  }
  
  /**
   * Returns the component with the given name.
   * 
   * If the $load parameter is set to true the component will be
   * instantiated if needed.
   * 
   * @param boolean $load instantiate component (if needed)
   * 
   * @return dgComponent
   */
  public function getComponent($name, $load=false)
  {
    $classOrObject = $this->m_components[$name];
    
    if (is_string($classOrObject) && $load)
    {
      $classOrObject = atknew($classOrObject, $this);
      $this->m_components[$name] = $classOrObject;
    }
    
    return $classOrObject;
  }

  /**
   * Add component for this grid. The component can either be specified
   * as a full ATK class name (including packages) or as a component
   * instance.
   *
   * @param string             $name          name
   * @param string|dgComponent $classOrObject class or instance
   */
  public function addComponent($name, $classOrObject)
  {
    $this->m_components[$name] = $classOrObject;
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
   * Load all components.
   */
  protected function loadComponents()
  {
    foreach ($this->m_components as $name => $classOrObject)
    {
      if (is_string($classOrObject))
      {
        $object = atknew($classOrObject, $this);
        $this->m_components[$name] = $object;
      }
    }
  }  
  
  /**
   * Returns the attributes that should be excluded from the list
   * next to the attributes that already have an AF_HIDE_LIST flag.
   *
   * @return array excluded attributes
   */
  public function getExcludes()
  {
    return $this->m_excludes;
  }
  
  /**
   * Sets the attributes that should be excluded from the list
   * next to the attribute that already have an AF_HIDE_LIST flag.
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
   * @see atkDataGrid::setActionNode
   * 
   * @return atkNode action node
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
   * @param atkNode $node
   */  
  public function setActionNode($node)
  {
    $this->m_actionNode = $node;
  }
  
  /**
   * Returns the record action session status.
   *
   * @see atkDataGrid::setActionSessionStatus
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
   * @param int status session status (e.g. SESSION_NESTED etc.)
   */  
  public function setActionSessionStatus($status)
  {
    $this->m_actionSessionStatus = $status;
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
   * Returns the current filter.
   *
   * @return string filter
   */
  public function getFilter()
  {
    return $this->getPostvar('atkfilter', '');
  }
  
  /**
   * Returns the current order by statement.
   *
   * @return string order by
   */
  public function getOrderBy()
  {
    return $this->getNode()->getColumnConfig()->getOrderByStatement();
  }   
  
  /**
   * Returns the records for the current page of the grid.
   *
   * @param boolean $load load the records (if needed)
   * 
   * @return array records
   */
  public function getRecords($load=false)
  {
    if ($load)
    {
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
   * @see atkDataGrid::setSelectHandler
   * @see atkDataGrid::setCountHandler 
   *
   * @param array $records records
   */
  protected function setRecords($records)
  {
    $this->m_records = $records;
  }
  
  /**
   * Returns the total record count for the grid.
   * 
   * @param boolean $load load the record count (if needed)
   * 
   * @return int record count
   */
  public function getCount($load=false)
  {
    if ($load)
    {
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
   * @see atkDataGrid::setCountHandler
   * @see atkDataGrid::setSelectHandler 
   *
   * @param int $count record count
   */
  protected function setCount($count)
  {
    $this->m_count = $count;;
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
   * Returns the select handler
   *
   * @see atkDataGrid::setSelectHandler
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
   * @see atkDataGrid::setCountHandler
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
    return 
      $this->getNode()
           ->select()
           ->excludes(array_merge($this->getNode()->m_listExcludes, $this->getExcludes()))      
           ->where($this->getFilter())
           ->orderBy($this->getOrderBy())
           ->limit($this->getLimit(), $this->getOffset())
           ->mode('admin')
           ->allRows();
  }
  
  /**
   * Default implementation for counting the records for this grid.
   * 
   * This method uses the grid node to retrieve a record count.
   *
   * @return unknown
   */
  protected function countRecords()
  {
    return
      $this->getNode()
           ->select()
           ->excludes(array_merge($this->getNode()->m_listExcludes, $this->getExcludes()))      
           ->where($this->getFilter())
           ->mode('admin')
           ->rowCount();
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
   * @param boolean $force force record and count retrieval?
   */
  public function loadRecords($force=false)
  {
    // temporarily overwrite the node postvars so that selectDb and countDb
    // have access to the atksearch, atkfilter, atklimit etc. parameters
    $this->overrideNodePostvars();  
    
    // retrieve records using the default implementation
    if ($force || ($this->getRecords() === null && $this->getSelectHandler() === null))
    {
      $records = $this->selectRecords();
      $this->setRecords($records);
    }
    
    // retrieve records using a custom select handler
    else if ($force || $this->getRecords() === null)
    {
      $records = call_user_func_array($this->getSelectHandler(), array($this));
      $this->setRecords($records);
    }
    
    // retrieve record count using the default implementation
    if ($force || ($this->getCount() === null && $this->getCountHandler() === null))
    {
      $count = $this->countRecords();
      $this->setCount($count);
    }
    
    // retrieve record count using a custom cont handler
    else if ($force || $this->getCount() === null)
    {
      $count = call_user_func_array($this->getCountHandler(), array($this));
      $this->setCount($count); 
    }
        
    // restore previous postvars
    $this->restoreNodePostvars();   
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
   * @param array $overrides           key/value overrides
   * @param array $overridesJs         key/value run-time overrides
   * @param array $overridesJsCallback JavaScript function which returns an overrides Hash
   * 
   * @return string JavaScript call (might need escaping when used in HTML code)
   */
  public function getUpdateCall($overrides=array(), $overridesJs=array(), $overridesJsCallback='null')
  {
    $overridesJsStr = '';
    
    foreach ($overridesJs as $key => $js) 
    {
      $overridesJsStr .= (!empty($overridesJsStr) ? ', ' : '')."'$key': $js";  
    }
    
    return 'ATK.DataGrid.update('.atkJSON::encode($this->getName()).', '.atkJSON::encode($overrides).', {'.$overridesJsStr.'}, '.$overridesJsCallback.');';
  }  
  
  /**
   * Translate the given string using the grid node. 
   * 
   * The value of $fallback will be returned if no translation can be found.
   * If you want NULL to be returned when no translation can be found then
   * leave the fallback empty and set $useDefault to false.  
   *
   * @param string $string      string to translate
   * @param string $fallback    fallback in-case no translation can be found
   * @param boolean $useDefault use default ATK translation if no translation can be found?
   * 
   * @return string translation 
   */
  public function text($string, $fallback='', $useDefault=true)
  {
    return $this->getNode()->text($string, null, '', $fallback, !$useDefault);
  } 

  /**
   * (Temporarily) override the node postvars so we can apply the grid
   * specific search conditions etc.
   */
  protected function overrideNodePostvars()
  {
    $this->m_overrideNodePostvarsLevel += 1;    
    
    // only override once
    if ($this->m_overrideNodePostvarsLevel == 1) 
    {
      $this->m_overrideNodePostvarsBackup = $this->getNode()->m_postvars;
      $this->getNode()->m_postvars = $this->getPostvars();
    }
  }

  /**
   * Restore override node postvars.
   * 
   * @see atkDataGrid::overrideNodePostvars
   */
  protected function restoreNodePostvars()
  {
    $this->m_overrideNodePostvarsLevel -= 1;

    if ($this->m_overrideNodePostvarsLevel == 0)
    {  
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
    // if we are not embedded in an edit form we generate 
    // the form name based on the grid name
    if (!$this->isEmbedded())
    {
      $this->setFormName($this->getName().'_form');
    }
    
    // temporarily overwrite the node postvars so that selectDb and countDb
    // have access to the atksearch, atkfilter, atklimit etc. parameters
    $this->overrideNodePostvars();
   
    // load records
    $this->loadRecords();

    // render the grid
    atkimport('atk.datagrid.atkdgrenderer');
    $renderer = new atkDGRenderer($this);
    $result = $renderer->render();
    
    // restore previous postvars
    $this->restoreNodePostvars();
    
    return $result;
  }
}
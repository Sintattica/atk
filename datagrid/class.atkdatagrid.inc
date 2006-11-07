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
 * @copyright (c)2000-2006 Ibuildings.nl BV
 * @copyright (c)2000-2006 Ivo Jansch
 * 
 * @license http://www.achievo.org/atk/licensing ATK Open Source License
 */

/* data grid flags */
define("DG_NO_SORT",                         1); // grid is not sortable
define("DG_NO_SEARCH",                       2); // grid is not searchable
define("DG_NO_EXTENDED_SEARCH",              4); // grid does not support extended search
define("DG_MRA",                            16); // multi-record-actions enabled
define("DG_MRPA",                           32); // multi-record-priority-actions enabled
define("DG_LOCK",                           64); // records can be locked
define("DG_EXT_SORT",                      128); // extended sort feature

/**
 * ATK data grid.
 * 
 * @author Peter C. Verhage <peter@ibuildings.nl>
 * @package atk
 * @subpackage datagrid
 */
class atkDataGrid
{
  /** 
   * @var string 
   */   
  var $m_name     = NULL;
  
  /** 
   * @var string 
   */    
  var $m_formName = NULL;
  
  /** 
   * @var string 
   */   
  var $m_prefix = "";
  
  /** 
   * @var atkNode 
   */  
  var $m_node = NULL;
  
  /** 
   * @var boolean 
   */  
  var $m_partial = false;
  
  /** 
   * @var string 
   */   
  var $m_action = "";
  
  /** 
   * @var array 
   */   
  var $m_postvars = array();
  
  /** 
   * @var int 
   */  
  var $m_flags = 0;
  
  /**
   * @var string
   */
  var $m_index = NULL;
  
  /** 
   * @var int 
   */  
  var $m_selectionMode = MRA_MULTI_SELECT;
  
  /** 
   * @var string 
   */  
  var $m_index = NULL;   
  
  /** 
   * @var array 
   */  
  var $m_suppressList = array();     
  
  var $m_baseUrl = NULL;
  
  var $m_loadHandler = NULL;
  var $m_loadHandlerParams = array();
  
  var $m_records = NULL;
    
  var $m_actions  = NULL;  
  var $m_excludes = array();
  
  var $m_header = "";
  var $m_footer = "";

  var $m_navigator = "atk.utils.atknavigator";
  var $m_recordlist = "atk.recordlist.atkrecordlist";

  var $m_cache = NULL;
  
  /**
   * Returns the data grid name.
   * 
   * @return string
   */
  function getName()
  {
    return $this->m_name;
  }
  
  /**
   * Sets the data grid name.
   *
   * @param string $name
   */
  function setName($name)
  {
    $this->m_name = $name;
  }
  
  /**
   * Returns the form name.
   * 
   * @return string
   */
  function getFormName()
  {
    return $this->m_formName;
  }
  
  /**
   * Sets the form name.
   *
   * @param string $formName
   */
  function setFormName($formName)
  {
    $this->m_formName = $formName;
  }
  
  /**
   * Returns the prefix.
   * 
   * @return string
   */
  function getPrefix()
  {
    return $this->m_prefix;
  }
  
  /**
   * Set prefix for all fields. Can be used to embed the
   * data grid inside other forms.
   * 
   * @param string $prefix
   */
  function setPrefix($prefix)
  {
    return $this->m_prefix;
  }
  
  /**
   * Returns the node for this data grid.
   * 
   * @return atkNode
   */
  function &getNode()
  {
    return $this->m_node;
  }
  
  /**
   * Sets the node for this data grid.
   *
   * @param atkNode $node
   */
  function setNode(&$node)
  {
    $this->m_node = &$node;
  }
  
  /**
   * Set partial.
   * 
   * @param bool
   */
  function setPartial($partial)
  {
    $this->m_partial = $partial;
  }
  
  /**
   * Is partial?
   * 
   * @return bool
   */
  function isPartial()
  {
    return $this->m_partial;
  }

  /**
   * Get the action name.
   * 
   * @return string
   */
  function getAction()
  {
    return $this->m_action;
  }
  
  /**
   * Sets the action name.
   *
   * @param string $action
   */
  function setAction($action)
  {
    $this->m_action = $action;
  }
  
  /**
   * Sets the postvars. If a prefix has been set and the
   * $usePrefix parameter is set to true (the default) only 
   * a subarray based on the prefix will be used. This also
   * means the the prefix must be set before setting the post
   * vars!
   * 
   * @param array   $postvars
   * @param boolean $usePrefix
   */
  function setPostvars($postvars, $usePrefix=true)
  {
    if ($usePrefix && $this->m_prefix != NULL)
    {
      $postvars = isset($postvars[$prefix]) ? $postvars[$prefix] : array();
    }
    
    $this->m_postvars = $postvars;
  }
  
  /**
   * Sets the postvar with the given name to the given value.
   * 
   * @param string $name
   * @param mixed $value
   */
  function setPostvar($name, $value)
  {
    $this->m_postvars[$name] = $value;
  }
  
  /**
   * Returns the postvars. Note, if a prefix has been set this can
   * mean only a subarray of the original set postvars will be returned.
   * 
   * @return array
   */
  function getPostvars()
  {
    return $this->m_postvars;
  }
  
  /**
   * Returns the value of the postvar with the given name. If the postvar is
   * not set an optional default value will be returned instead.
   *
   * @param string $name
   * @param mixed $default
   * @return mixed
   */
  function getPostvar($name, $default=NULL)
  {
    return isset($this->m_postvars[$name]) ? $this->m_postvars[$name] : $default;
  }
  
  /**
   * Returns the current limit.
   *
   * @return int
   */
  function getLimit()
  {
    return $this->getPostvar('atklimit', atkconfig("recordsperpage"));
  }
  
  /**
   * Returns the current offset.
   *
   * @return int
   */
  function getOffset()
  {
    return $this->getPostvar('atkstartat', 0);
  }

  /**
   * Returns the search array.
   * 
   * @return array
   */
  function getSearch()
  {
    return $this->getPostvar('atksearch');
  }
  
  /**
   * Get flags.
   *
   * @return int
   */
  function getFlags()
  {
    return $this->m_flags;
  }  
  
  /**
   * Has flag?
   *
   * @param int $flag
   * @return bool
   */
  function hasFlag($flag)
  {
    return hasFlag($this->m_flags, $flag);
  }
    
  /**
   * Sets the data grid flags.
   *
   * @param int $flags
   */
  function setFlags($flags)
  {
    $this->m_flags = $flags;
  }
  
  /**
   * Add flag(s).
   *
   * @param int $flag
   */
  function addFlag($flag)
  {
    $this->m_flags |= $flag;
  }
  
  /**
   * Remove flag(s).
   *
   * @param int $flag
   */
  function removeFlag($flag)
  {
    if ($this->hasFlag($flag))
      $this->m_flags ^= $flag;
  }
  
  /**
   * Converts the given node flags to data grid flags where possible.
   *
   * @param int $flags
   * @return int
   * 
   * @static
   */
  function convertFlags($flags)
  {
    $result = 0;
    $result |= hasFlag($flags, NF_NO_SORT) ? DG_NO_SORT : 0;    
    $result |= hasFlag($flags, NF_NO_SEARCH) ? DG_NO_SEARCH : 0;
    $result |= hasFlag($flags, NF_NO_EXTENDED_SEARCH) ? DG_NO_EXTENDED_SEARCH : 0;
    $result |= hasFlag($flags, NF_MRA) ? DG_MRA : 0;
    $result |= hasFlag($flags, NF_MRPA) ? DG_MRPA : 0;
    $result |= hasFlag($flags, NF_LOCK) ? DG_LOCK : 0;
    $result |= hasFlag($flags, NF_EXT_SORT) ? DG_EXT_SORT : 0;
    return $result;
  }  
  
  /**
   * Is index attribute set?
   * 
   * @return boolean
   */
  function hasIndex()
  {
    return $this->m_index !== NULL || !empty($this->m_node->m_index);
  }
  
  /**
   * Returns the index attribute name. If no index attribute
   * has been set NULL will be returned.
   * 
   * @return string
   */
  function getIndex()
  {
    if ($this->m_index !== NULL)
    {
      return $this->m_index;
    }
    else if (!empty($this->m_node->m_index))
    {
      return $this->m_node->m_index;
    }
    else 
    {
      return NULL;
    }
  }
  
  /**
   * Sets the index attribute name. This will override the index
   * set in the node.
   *
   * @param string $attrName
   */
  function setIndex($attrName)
  {
    $this->m_index = $attrName;
  }      

  /**
   * Get multi-record-action selection mode.
   * 
   * @return int
   */
  function getSelectionMode()
  {
    return $this->m_selectionMode;
  }
  
  /**
   * Set multi-record-action selection mode.
   *
   * @param int $mode
   */
  function setSelectionMode($mode)
  {
    $this->m_selectionMode = $mode;
  }
  
  /**
   * Returns the suppress list.
   * 
   * @return array
   */
  function getSuppressList()
  {
    return $this->m_suppressList;
  }
  
  /**
   * Set suppress list. A list of attributes that
   * should not be displayed in the grid.
   * 
   * @param array $suppressList 
   */
  function setSuppressList($suppressList)
  {
    $this->m_suppressList = $suppressList;
  }
  
  /**
   * Sets the base Ajax URL for refreshing the recordlist when
   * clicking on one of the navigation or sort links or when 
   * searching.
   *
   * @param string $url
   */
  function setBaseUrl($url)
  {
    $this->m_baseUrl = $url;
  }
  
  /**
   * Sets a method/function which handles the loading of records. You can use
   * this method instead of setRecords to load records dynamically. You can
   * optionally add a list of parameter after the $handler parameter which are
   * forwarded to the handler when called.
   *
   * @param Mixed $handler handler function
   * @param Mixed $param1
   * @param Mixed $param2
   * @param Mixed $param...
   */
  function setLoadHandler($handler)
  {
    $args = func_get_args();
    $this->m_loadHandler = $handler;
    $this->m_loadHandlerParams = array_slice($args, 1);
  }
  
  function setRecords($records)
  {
    $this->m_records = $records;
  }  
  
  function loadRecords()
  {
    $records = array();
    
    if ($this->m_loadHandler !== NULL)
    {
      $records = call_user_func_array($this->m_loadHandler, $this->m_loadHandlerParams);
    }
    
    $this->setRecords($records);
  }
  
  function setActions($actions)
  {
    $this->m_actions = $actions;
  }
  
  function setExcludes($excludes)
  {
    $this->m_excludes = $excludes;
  }
  
  function setHeader($header)
  {
    $this->m_header = $header;
  }
  
  function setFooter($footer)
  {
    $this->m_footer = $footer;
  }
  
  function setNavigator($class)
  {
    $this->m_navigator = $class;
  }
  
  function setRecordList($class)
  {
    $this->m_recordlist = $class;
  }  
    
  function setCache(&$cache)
  {
    $this->m_cache = &$cache;
  }
  
  function &_createNavigator()
  {
    return atknew($this->m_navigator);
  }
  
  function &_createRecordList()
  {
    return atknew($this->m_recordlist);    
  }
    
  function _render()
  {
    $params = array();
    
    /* @var $navigator atkNavigator */
    /* @var $recordlist atkRecordList */
    $navigator = $this->_createNavigator();
    $recordlist = &$this->_createRecordList();
    
    if ($this->m_records == NULL)
      $this->loadRecords();
    $count = count($this->m_records);
    
    $name = $this->m_name;
    if ($name === NULL)
      $name = "rl_".getUniqueId(get_class($recordlist));           
    
    $actions = $this->m_actions;
    if ($actions === NULL)
      $actions = $this->m_node->defaultActions($this->m_action);
      
    $flags = $this->m_flags;
    if ($flags === NULL)
      $flags = atkRecordList::convertFlags($this->m_node->getFlags());
    
    $index = $this->m_index;  
    if ($this->m_index === NULL)
      $index = $this->m_node->m_index;
      
    $mraSelectionMode = $this->m_mraSelectionMode;
    if ($mraSelectionMode === NULL)
      $mraSelectionMode = $this->m_node->m_mraSelectionMode;
      
    $baseUrl = $this->m_baseUrl;
    
    if (empty($baseUrl))
    {
      $params = array();
      if (isset($this->m_node->m_postvars['atkselector']))
        $params['atkselector'] = $this->m_node->m_postvars['atkselector'];
      if (isset($this->m_node->m_postvars['atktarget']))
        $params['atktarget'] = $this->m_node->m_postvars['atktarget'];
      $baseUrl = session_url(partial_url($this->m_node->atkNodeType(), $this->m_action, 'recordlist', $params), SESSION_DEFAULT).'&';
    }
    
    if (!in_array($baseUrl{strlen($baseUrl) - 1}, array('?', '&')))
    {
      $baseUrl .= strpos($baseUrl, '?') === FALSE ? '?' : '&';
    }

    $baseUrl .= 'atkrecordlist='.rawurlencode($name).'&';      
    
    if ($index != FALSE && !empty($index))
      $params['index'] = $navigator->buildIndex($this->m_node, $count > 0 ? $records[0][$index] : '', $name, $baseUrl);
      
    $navigation = $navigator->getNavigationUrls($this->m_node, $this->m_action, '', $name, $baseUrl);
    if ($navigation)
    {
      if (array_key_exists('index', $params))
        $params['index'] .= '<br />';
      
      $params['navbar'] = $navigator->buildNavigation($this->m_node, $this->m_action, $navigation, hasFlag($this->m_flags, RL_EMBED), $name, $baseUrl);
    }
    
    $params['list'] = $recordlist->render($this->m_node, $this->m_records, $actions, $flags, $this->m_excludes, $this->m_form, $navigation, $this->m_embedPrefix, $name, $baseUrl, $mraSelectionMode);
    $params['header'] = $this->m_header;
    $params['footer'] = $this->m_footer;

    $ui = &atkUI::getInstance();
    $output = $ui->renderList($this->m_action, $params);

    return array('name' => $name, 'class' => get_class($recordlist), 'output' => $output, 'script' => $recordlist->m_actionloader);
  }  
  
  function render($partial=false)
  {
    $preAction = $this->m_node->m_action;
    $this->m_node->m_action = $this->m_action;
    
    $output = NULL;
    
    if ($this->m_cache != NULL)
    {
      $data = $this->m_cache->readCache();
    }
    
    if ($data)
    {
      $name = $data['name'];
      $output = $data['output'];
    }
    else
    {
      $data = $this->_render();
      
      $name = $data['name'];
      $output = $data['output'];
      
      if ($this->m_cache != NULL)
      {
        $class = $data['class'];    
        $script = $data['script'];
        $cache->writeCache($name, $class, $output, $script);
      }
    }
        
    if (!$partial)
      $output = "<div id=\"{$name}_container\">{$output}</div>";
      
    $this->m_node->m_action = $preAction;
      
    return $output;
  }
}
?>
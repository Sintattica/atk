<?php
/**
 * @todo recordlist cache
 * @todo actionsessions tatus
 * @todo masternode
 */
class atkDataGrid
{
  private $m_name;
  private $m_node;
  private $m_records;
  private $m_actions;
  private $m_flags;
  private $m_formName;
  private $m_prefix;
  private $m_navigation;
  private $m_suppressList;
  private $m_defaultLimit;
  private $m_count;
  private $m_components = array();
  private $m_update;

  public function __construct($node)
  {
    $this->setName('dg_'.getUniqueId(get_class($this)));    
    $this->setDefaultLimit(atkconfig('recordsperpage')); 
    $this->addComponent('list', 'atk.datagrid.atkdglist');
    $this->addComponent('paginator', 'atk.datagrid.atkdgpaginator');
    $this->addComponent('summary', 'atk.datagrid.atkdgsummary');
    $this->addComponent('limit', 'atk.datagrid.atkdglimit');
    $this->setNode($node);
    $this->setActions($node->defaultActions("admin"));
    $this->setPostvars($node->m_postvars);
    $this->setBaseUrl(partial_url($node->atkNodeType(), $node->m_action, 'recordlist'));
    $this->setFlags(atkRecordList::convertFlags($node->getFlags()));
    $this->setUpdate($this->getNode()->isPartial());
  }
  
  public function getName()
  {
    return $this->m_name;
  }
  
  public function setName($name)
  {
    $this->m_name = $name;
  }
  
  public function getNode()
  {
    return $this->m_node;
  }
  
  protected function setNode($node)
  {
    $this->m_node = $node;
  }
  
  public function getRecords()
  {
    return $this->m_records;
  }
  
  public function setRecords($records)
  {
    $this->m_records = $records;
  }
  
  public function getActions()
  {
    return $this->m_actions;
  }
  
  public function setActions($actions)
  {
    $this->m_actions = $actions;
  }
  
  public function getFlags()
  {
    return $this->m_flags;
  }
  
  public function setFlags($flags)
  {
    $this->m_flags = $flags;
  }
  
  public function getSuppressList()
  {
    return $this->m_suppressList;
  }
  
  public function setSuppressList($list)
  {
    $this->m_suppressList = $list;
  }
  
  public function getFormName()
  {
    return $this->m_formName;
  }
  
  public function setFormName($name)
  {
    $this->m_formName = $name;
  }
  
  public function getPrefix()
  {
    return $this->m_prefix;
  }
  
  public function setPrefix($prefix)
  {
    $this->m_prefix = $prefix;
  }
  
  public function getNavigation()
  {
    return $this->m_navigation;
  }

  public function setNavigation($navigation)
  {
    $this->m_navigation = $navigation;
  }
  
  public function getUpdateCall($overrides=array(), $overridesJs=array())
  {
    $overridesJsStr = '';
    
    foreach ($overridesJs as $key => $js) 
    {
      if (!empty($overridesJsStr)) 
      {
        $overridesJsStr .= ', ';
      }
      
      $overridesJsStr .= "'$key': $js";  
    }
       
    return 'ATK.DataGrid.update('.atkJSON::encode($this->getName()).', Object.extend('.atkJSON::encode($overrides).', {'.$overridesJsStr.'}));';
  }
  
  public function isEmbedded()
  {
    return false;
  }
  
  public function setUpdate($update)
  {
    $this->m_update = $update;
  }
  
  public function isUpdate()
  {
    return $this->m_update;
  }

  /**
   * Sets the postvars.
   *
   * @param array $postvars
   */
  public function setPostvars($postvars)
  {
    $this->m_postvars = $postvars;
  }

  /**
   * Sets the postvar with the given name to the given value.
   *
   * @param string $name
   * @param mixed $value
   */
  public function setPostvar($name, $value)
  {
    $this->m_postvars[$name] = $value;
  }

  /**
   * Returns the postvars. Note, if a prefix has been set this can
   * mean only a subarray of the original set postvars will be returned.
   *
   * @return array
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
  
  public function setBaseUrl($url)
  {
    $this->m_baseUrl = $url;
  }
  
  public function getLimit()
  {
    return $this->getPostvar('atklimit', $this->getDefaultLimit());
  }
  
  public function getBaseUrl()
  {
    return $this->m_baseUrl;
  }
  
  public function getOffset()
  {
    return $this->getPostvar('atkstartat', 0);
  }
  
  public function setCount($count)
  {
    $this->m_count = $count;;
  }
  
  public function getCount()
  {
    return $this->m_count;
  }
  
  public function getDefaultLimit()
  {
    return $this->m_defaultLimit;
  }

  public function setDefaultLimit($limit)
  {
    $this->m_defaultLimit = $limit;
  }  
  
  public function text($string, $fallback='', $useDefault=true)
  {
    return $this->getNode()->text($string, null, '', $fallback, !$useDefault);
  }
  
  /**
   * Sets a method/function which handles the loading of records. You can use
   * this method instead of setRecords to load records dynamically. You can
   * optionally add a list of parameter after the $handler parameter which are
   * forwarded to the handler when called.
   *
   * @param Mixed $handler handler function
   */
  public function setSelectHandler($handler, $params=array())
  {
    $this->m_selectHandler = $handler;
    $this->m_selectHandlerParams = $params;
  }
  
  protected function getSelectHandler()
  {
    return $this->m_selectHandler;
  }
  
  protected function getSelectHandlerParams()
  {
    return $this->m_selectHandlerParams;
  }

  public function getFilter()
  {
    return $this->getPostvar('atkfilter', '');
  }
  
  public function getOrderBy()
  {
    return $this->getNode()->getColumnConfig()->getOrderByStatement();
  }
  
  protected function selectRecords()
  {
    return 
      $this->getNode()
           ->select()
           ->excludes($this->getNode()->m_listExcludes)      
           ->where($this->getFilter())
           ->orderBy($this->getOrderBy())
           ->limit($this->getLimit(), $this->getOffset())
           ->mode('admin')
           ->allRows();
  }
  
  protected function countRecords()
  {
    return
      $this->getNode()
           ->select()
           ->excludes($this->getNode()->m_listExcludes)      
           ->where($this->getFilter())
           ->orderBy($this->getOrderBy())
           ->limit($this->getLimit(), $this->getOffset())
           ->mode('admin')
           ->rowCount();
  }

  public function setCountHandler($handler, $params=array())
  {
    $this->m_countHandler = $handler;
    $this->m_countHandlerParams = $params;
  }
  
  protected function getCountHandler()
  {
    return $this->m_countHandler;
  }
  
  protected function getCountHandlerParams()
  {
    return $this->m_countHandlerParams;
  }
  
  protected function loadRecords()
  {
    if ($this->getRecords() === null && $this->getSelectHandler() === null)
    {
      $records = $this->selectRecords();
      $this->setRecords($records);
    }
    else if ($this->getRecords() === null)
    {
      $records = call_user_func_array($this->getSelectHandler(), $this->getSelectHandlerParams());
      $this->setRecords($records);
    }
    
    if ($this->getCount() === null)
    {
      $count = $this->countRecords();
      $this->setCount($count);
    }
    else if ($this->getCount() === null && $this->getCountHandler() === null)
    {
      $count = call_user_func_array($this->getCountHandler(), $this->getCountHandlerParams());
      $this->setCount($count); 
    }
  }
  
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
  
  public function addComponent($name, $classOrObject)
  {
    $this->m_components[$name] = $classOrObject;
  }

  public function getComponents()
  {
    return $this->m_components;
  }
  
  public function getTemplate()
  {
    return 'datagrid.tpl';
  }
  
  public function render()
  {
    if (!$this->isEmbedded() && $this->getFormName() == null)
    {
      $this->setFormName($this->getName().'_form');
    }
    
    $this->loadRecords();
    $this->loadComponents();

    atkimport('atk.datagrid.atkdgrenderer');
    $renderer = new atkDGRenderer($this);
    return $renderer->render();
  }
}
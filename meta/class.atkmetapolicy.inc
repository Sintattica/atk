<?php
/**
 * The default ATK meta policy.
 *
 * @author Peter C. Verhage <peter@achievo.org>
 * @version $Revision$ 
 */
class atkMetaPolicy
{
  var $m_node = NULL;
  
  var $m_includes = NULL;
  var $m_excludes = NULL;

  var $m_attrs = array();
  
  /**
   * Returns an instance of the default meta policy determined
   * using the $config_meta_policy variable or if not set uses itself.
   *
   * @static
   */
  function defaultInstance(&$node)
  {
    $class = atkconfig("meta_policy");

    if ($class != null)
    {
      atkimport($class);
      $class = strrchr($class, ".") ? substr(strrchr($class, "."), 1) : $class;
      if ($class != "atkmetapolicy")
        return new $class($node);
    }
      
    return new atkMetaPolicy();
  }
  
  /**
   * Get auto-detected flags for the given attribute.
   */
  function _getFlags($name, $meta)
  {
    $flags =
      (hasFlag($meta['flags'], MF_PRIMARY) ? AF_PRIMARY : 0) |
      (hasFlag($meta['flags'], MF_UNIQUE) ? AF_UNIQUE : 0) |
      (hasFlag($meta['flags'], MF_NOT_NULL) ? AF_OBLIGATORY : 0) |
      (hasFlag($meta['flags'], MF_AUTO_INCREMENT|MF_PRIMARY) ? AF_AUTOKEY : 0) |
      ($meta['type'] == "text" ? AF_HIDE_LIST : 0);

    if (hasFlag($flags, AF_PRIMARY) && $meta['num'] == 0 &&
        in_array($name, array("id", $meta['table']."id", $meta['table']."_id")))
      $flags |= AF_AUTOKEY;

    return $flags;
  }
  
  /**
   * Get attribute type and params for the given attribute.
   * Returns an array which looks like the following:
   * array("type" => ..., "params" => array(...))
   */
  function _getTypeAndParams($name, $meta)
  {
    $type = NULL;
    $params = array();
    
    if (in_array($name, array("passw", "password")))
      $type = "atkpasswordattribute";
      
    else
    {
      switch($meta['gentype'])
      {
        // string
        case "string":
          $type = "atkattribute";
          break;
          
        // text
        case "text":
          $type = "atktextattribute";
          break;
          
        // number
        case "number":
        case "decimal":
          $type = "atknumberattribute";
          break;
          
        // date
        case "date":
          $type = "atkdateattribute";
          break;
      }    
    }
    
    return array("type" => $type, "params" => $params);
  }  

  /**
   * Sets the node.
   */
  function setNode(&$node)
  {
    $this->m_node = &$node;
  }
  
  /**
   * Returns the table.
   */
  function getTable()
  {
    // table is already set
    if ($this->m_node->getTable() !== NULL)
      return $this->m_node->getTable();
      
    // type is already set
    if ($this->m_node->m_type !== NULL)
      return $this->m_node->m_type;
      
    // by class
    return get_class($this->m_node);
  }
  
  /**
   * Returns the sequence.
   */
  function getSequence()
  {
    $sequence = NULL;
    
    $cols = $this->m_node->m_db->tableMeta($this->getTable());
    
    foreach ($cols as $name => $meta)
      if (strlen($meta['sequence']) > 0)
        $sequence = $meta['sequence'];
        
    return $sequence;
  }
  
  /**
   * Set includes.
   */
  function setIncludes($attrs)
  {
    if (func_num_args() > 1)
      $attrs = func_get_args();
  
    $this->m_includes = $attrs;
    $this->m_excludes = NULL;
    
    $this->setOrder($attrs);
  }
  
  /**
   * Set excludes.
   */
  function setExcludes($attrs)
  {
    if (func_num_args() > 1)
      $attrs = func_get_args();
  
    $this->m_excludes = $attrs;
    $this->m_includes = NULL;
  }
  
  /**
   * Sets the attribute order. All attributes not mentioned
   * will be put at the bottom using their current order.
   */
  function setOrder($attrs)
  {
    if (func_num_args() > 1)
      $attrs = func_get_args();
    else if (!is_array($attrs))
      $attrs = array($attrs);

    $order = array_merge($attrs, array_diff(array_keys($this->m_attrs), $attrs));
    foreach ($order as $i => $key)
      $this->m_attrs[$key]['order'] = ($i + 1) * 100;
  }

  /**
   * Set flag(s) for the given attribute or list of attributes.
   * If $autoDetect is set to FALSE (defaults to TRUE) no
   * auto-detected flags will be added later on.
   */
  function setFlag($attrs, $flag) { call_user_func_array(array($this, "setFlags"), func_get_args()); }   
  function setFlags($attrs, $flags)
  {
    if (func_num_args() > 2)
    {
      $attrs = func_get_args();
      $flags = array_pop($attrs);
    }
    
    else if (!is_array($attrs))
      $attrs = array($attrs);
    
    foreach ($attrs as $attr)
      if (array_key_exists($attr, $this->m_attrs))
        $this->m_attrs[$attr]["flags"] = $flags;
  }  
  
  /**
   * Add flag(s) to the given attribute or list of attributes.
   */
  function addFlag($attrs, $flag) { $args = func_get_args(); call_user_func_array(array($this, "addFlags"), $args); }
  function addFlags($attrs, $flags)
  {
    if (func_num_args() > 2)
    {
      $attrs = func_get_args();
      $flags = array_pop($attrs);
    }
    
    else if (!is_array($attrs))
      $attrs = array($attrs);

    foreach ($attrs as $attr)
      if (array_key_exists($attr, $this->m_attrs))
        $this->m_attrs[$attr]["flags"] |= $flags;
  }
  
  /**
   * Remove flag(s) from the given attribute or list of attributes.
   */
  function removeFlag($attrs, $flag) { $args = func_get_args(); call_user_func_array(array($this, "removeFlags"), $args); }   
  function removeFlags($attrs, $flags)
  {
    if (func_num_args() > 2)
    {
      $attrs = func_get_args();
      $flags = array_pop($attrs);
    }
    
    else if (!is_array($attrs))
      $attrs = array($attrs);

    foreach ($attrs as $attr)
      if (array_key_exists($attr, $this->m_attrs))
        $this->m_attrs[$attr]["flags"] = ($this->m_attrs[$attr]["flags"] | $flags) ^ $flags;
  }  
  
  /**
   * Set the tab(s) for the given attribute or list of attributes.
   */
  function setTab($attrs, $tab) { $args = func_get_args(); call_user_func_array(array(&$this, "setTabs"), $args); }
  function setTabs($attrs, $tabs)
  {
    if (func_num_args() > 2)
    {
      $attrs = func_get_args();
      $tabs = array_pop($attrs);
    }
    
    else if (!is_array($attrs))
      $attrs = array($attrs);
      
    foreach ($attrs as $attr)
      if (array_key_exists($attr, $this->m_attrs))
        $this->m_attrs[$attr]["tabs"] = $tabs;
  }
  
  /**
   * Returns the attributes data array.
   */
  function &getAttributes()
  {
    return $this->m_attrs;
  }  
  
  /**
   * Intialize attribute for node using
   * the given column meta data.
   */
  function _initAttribute($name, $meta)
  {
    $typeAndParams = $this->_getTypeAndParams($name, $meta);
    if ($typeAndParams["type"] === NULL) return;
    
    $flags = $this->_getFlags($name, $meta);

    $this->m_attrs[$name] = $typeAndParams + array("flags" => $flags, "tabs" => NULL, "order" => ($meta['num'] + 1) * 100);
  }

  /**
   * Initialize attributes using policy.
   */
  function init()
  {
    $cols = $this->m_node->m_db->tableMeta($this->m_node->getTable());
    foreach ($cols as $name => $meta)
      $this->_initAttribute($name, $meta);
  }  
  
  /**
   * Apply attribute.
   */
  function _applyAttribute($name, $type, $params, $flags, $tabs, $order)
  {
    if ($this->m_includes !== NULL && !in_array($name, $this->m_includes)) return;
    if ($this->m_excludes !== NULL && in_array($name, $this->m_excludes)) return;
  
    useattrib($type);
    if (strpos($type, ".") !== FALSE)
      $type = substr($type, strpos($type, ".") + 1);

    $call = "\$attr = new $type(\$name";
    foreach (array_keys($params) as $i)
      $call .= ", \$params[$i]";
    $call .= ", \$flags);";  
    eval($call);
    
    $this->m_node->add($attr, $tabs, $order);
  }  
  
  /**
   * Build / setup node using the collected attributes.
   */
  function apply()
  {
    foreach ($this->m_attrs as $name => $data)
      $this->_applyAttribute($name, $data["type"], $data["params"], $data["flags"], $data["tabs"], $data["order"]);
  }
}
?>
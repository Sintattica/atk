<?php
/**
 * The default ATK meta policy.
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
      (($meta['flags'] & MF_PRIMARY) == MF_PRIMARY ? AF_PRIMARY : 0) |
      (($meta['flags'] & MF_UNIQUE) == MF_UNIQUE ? AF_UNIQUE : 0) |
      (($meta['flags'] & MF_NOT_NULL) == MF_NOT_NULL ? AF_OBLIGATORY : 0) |
      (($meta['flags'] & (MF_AUTO_INCREMENT|MF_PRIMARY)) == (MF_AUTO_INCREMENT|MF_PRIMARY) ? AF_AUTOKEY : 0);

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
   * Set flag(s) for the given attribute or list of attributes.
   * If $autoDetect is set to FALSE (defaults to TRUE) no
   * auto-detected flags will be added later on.
   */
  function setFlags($attrs, $flags)
  {
    if (!is_array($attrs))
      $attrs = array($attrs);
    
    foreach ($attrs as $attr)
      if (array_key_exists($attr, $this->m_attrs))
        $this->m_attrs[$attr]["flags"] = $flags;
  }  
  
  /**
   * Add flag(s) to the given attribute or list of attributes.
   */
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

    $this->m_attrs[$name] = $typeAndParams + array("flags" => $flags);
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
  function _applyAttribute($name, $type, $params, $flags)
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
    
    $this->m_node->add($attr);
  }  
  
  /**
   * Build / setup node using the collected attributes.
   */
  function apply()
  {
    foreach ($this->m_attrs as $name => $data)
      $this->_applyAttribute($name, $data["type"], $data["params"], $data["flags"]);
  }
}
?>
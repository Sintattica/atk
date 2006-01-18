<?php
/**
 * This file is part of the Achievo ATK distribution.
 * Detailed copyright and licensing information can be found
 * in the doc/COPYRIGHT and doc/LICENSE files which should be
 * included in the distribution.
 *
 * @package atk
 * @subpackage meta
 *
 * @copyright (c) 2004-2005 Peter C. Verhage
 * @license http://www.achievo.org/atk/licensing ATK Open Source License
 *
 * @version $Revision$
 * $Id$
 */

atkimport("atk.meta.atkmetatools");

/**
 * The default meta policy.
 *
 * @author Peter C. Verhage <peter@achievo.org>
 *
 * @package atk
 * @subpackage meta
 */
class atkMetaPolicy
{
  var $m_node = NULL;
  var $m_grammar = NULL;
  
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
        return new $class();
    }
      
    return new atkMetaPolicy();
  }
  
  /**
   * Returns the meta grammar.
   * 
   * @return atkMetaGrammar the meta grammar
   */
  function &getGrammar()
  {
    return $this->m_grammar;
  }
    
  /**
   * Sets the meta grammar.
   * 
   * @param atkMetaGrammar $grammar the meta grammar
   */
  function setGrammar(&$grammar)
  {
    $this->m_grammar = &$grammar;
  }
  
  /**
   * Calls a method of this object with the given parameters.
   *
   * @param String $method the method name
   * @param Array $params the method parameters
   * @return unknown
   */
  function _call($method, $params)
  {
    // unfortunately call_user_func_array(array($this, $method), $params) makes
    // a copy of $this, instead of calling the method on the original $this
    // so we use eval instead...
    $call = "\$this->$method(";
    foreach (array_keys($params) as $i)
      $call .= ($i > 0 ? ", " : "")."\$params[$i]";
    $call .= ");";  
    return eval($call);    
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
      $type = "atk.attributes.atkpasswordattribute";
      
    else if (in_array($name, array("email", "e-mail")))
    {
      $type = "atk.attributes.atkemailattribute";    
      $params = array(false);
    }
      
    else
    {
      switch($meta['gentype'])
      {
        // string
        case "string":
          $type = "atk.attributes.atkattribute";
          break;
          
        // text
        case "text":
          $type = "atk.attributes.atktextattribute";
          break;
          
        // number
        case "number":
        case "decimal":
          $type = "atk.attributes.atknumberattribute";
          break;
          
        // date
        case "date":
          $type = "atk.attributes.atkdateattribute";
          break;
          
        // time
        case "time":
          $type = "atk.attributes.atktimeattribute";
          break;
          
        // datetime
        case "datetime":
          $type = "atk.attributes.atkdatetimeattribute";
          break;
      }    
    }
    
    return array("type" => $type, "params" => $params);
  }  
  
  /**
   * Get default value for the given attribute.
   */
  function _getDefaultValue($name, $meta)
  {
    if ($meta['default'] == "NOW" && in_array($meta['gentype'], array('date', 'time', 'datetime')))
    {
      $stamp = getdate();
      $date = array('day' => $stamp['yday'], 'month' => $stamp['mon'], 'year' => $stamp['year']);
      $time = array('hours' => $stamp['hours'], 'minutes' => $stamp['minutes'], 'seconds' => $stamp['seconds']);
      return array_merge($meta['gentype'] == 'time' ? array() : $date, $meta['gentype'] == 'date' ? array() : $time);
    }

    else if (array_key_exists('default', $meta))
      return $meta['default'];

    else return NULL;
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
    
    $type = $this->m_node->m_type;
    if ($type == NULL)  
      $type = get_class($this->m_node);
      
    $db = $this->m_node->getDb();
    if ($db->tableExists($type)) return $type;
    elseif ($db->tableExists($this->m_grammar->singularize($type))) 
      return $this->m_grammar->singularize($type);
    elseif ($db->tableExists($this->m_grammar->pluralize($type))) 
      return $this->m_grammar->pluralize($type);
  }
  
  /**
   * Returns the sequence.
   */
  function getSequence()
  {
    $sequence = NULL;
    
    $db = &$this->m_node->getDb();
    $cols = $db->tableMeta($this->getTable());
    
    foreach ($cols as $name => $meta)
      if (strlen($meta['sequence']) > 0)
        $sequence = $meta['sequence'];

    if ($sequence == NULL)
      $sequence = "seq_".$this->getTable();
    
    return $sequence;
  }
  
  /**
   * Returns the default values for the node's attributes.
   */
  function getDefaultValues()
  {
    $result = array();
    $db = &$this->m_node->getDb();
    $cols = $db->tableMeta($this->m_node->getTable());
    foreach ($cols as $name => $meta)
      if (array_key_exists('default', $meta))
        $result[$name] = $this->_getDefaultValue($name, $meta);
    return $result;
  }
  
  /**
   * Set includes.
   */
  function setIncludes($attrs)
  {
    if (!is_array($attrs))
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
    if (!is_array($attrs))
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

  function setFlag($attrs, $flag) { $params = func_get_args(); $this->_call("setFlags", $params); }   
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
  function addFlag($attrs, $flag) { $params = func_get_args(); $this->_call("addFlags", $params); }
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
  function removeFlag($attrs, $flag) { $params = func_get_args(); $this->_call("removeFlags", $params); }   
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
  function setTab($attrs, $tab) { $params = func_get_args(); $this->_call("setTabs", $params); }
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
   * Sets the attribute type. All extra arguments after the two
   * standard arguments will be threated as parameters for the
   * attribute. If you need to pass arguments by reference you can
   * better use the setTypeAndParams method.
   * 
   * @param $attr  the attribute name
   * @param $type  full ATK attribute class (e.g. atk.attributes.atkboolattribute)
   * @param $...   all other arguments will be threated as parameters
   */
  function setType($attr, $type)
  {
    $params = func_get_args();
    $params = array_slice($params, 2);
    $this->setTypeAndParams($attr, $type, $params);
  }
  
  /**
   * Sets the attribute type and parameters.
   * 
   * @param $attr    the attribute name
   * @param $type    full ATK attribute class (e.g. atk.attributes.atkboolattribute)
   * @param $params  parameters for the attribute (optional)
   */
  function setTypeAndParams($attr, $type, $params=array())
  {
    $this->m_attrs[$attr]["type"] = $type;
    $this->m_attrs[$attr]["params"] = $params;
  }
  
  /**
   * Returns the current maximum order in the attribute list.
   */
  function _getMaxOrder()
  {
    $max = 0;
    
    foreach (array_keys($this->m_attrs) as $key)
      if ($this->m_attrs[$key]["order"] > $max)
        $max = $this->m_attrs[$key]["order"];
        
    return $max;
  }
  
 /**
   * Find destination node for the given meta relation.
   *
   * @param $relation  meta relation
   *
   * @return destination node name for the given relation
   */
  function findDestination(&$relation)
  {
    $module = getNodeModule($relation->m_accessor);
    if ($module == "")
      $module = $this->m_node->m_module;
      
    $node = getNodeType($relation->m_accessor);
    $toMany = is_a($relation, "atkmetaonetomanyrelation");

    if ($module != "")
    {
      if (nodeExists("$module.$node"))
        return "$module.$node";
        
      if ($toMany && nodeExists("$module.".$this->m_grammar->singularize($node)))
        return "$module.".$this->m_grammar->singularize($node);

      if (!$toMany && nodeExists("$module.".$this->m_grammar->pluralize($node)))
        return "$module.".$this->m_grammar->pluralize($node);
    }
    
    if (nodeExists($node))
      return $node;
    
    if ($toMany && nodeExists($this->m_grammar->singularize($node)))
      return $this->m_grammar->singularize($node);

    if (!$toMany && nodeExists($this->m_grammar->pluralize($node)))
      return $this->m_grammar->pluralize($node);
      
    return NULL;
  }

  /**
   * Find destination attribute for the given relation.
   *
   * @param $relation  meta relation
   *
   * @return destination attribute name for the given relation
   */
  function findDestinationAttribute(&$relation)
  {
    $table = $this->getTable();  
    $node = getNodeType($this->m_node->atkNodeType());
    $attrs = array_keys($relation->m_destInstance->m_attribList);

    $attr = NULL;
    if (array_key_exists("dest", $relation->m_options))
      $attr = $relation->m_options["dest"];
    else if (array_key_exists("destination", $relation->m_options))
      $attr = $relation->m_options["destination"];      
    else if (in_array("{$table}_id", $attrs))
      $attr = "{$table}_id";
    else if (in_array($this->m_grammar->singularize($table)."_id", $attrs))
      $attr = $this->m_grammar->singularize($table)."_id";
    else if (in_array("{$node}_id", $attrs))
      $attr = "{$node}_id";
    else if (in_array($this->m_grammar->singularize($node)."_id", $attrs))
      $attr = $this->m_grammar->singularize($node)."_id";      
    else if (in_array("{$table}id", $attrs))
      $attr = "{$table}id";
    else if (in_array($this->m_grammar->singularize($table)."id", $attrs))
      $attr = $this->m_grammar->singularize($table)."id";      
    else if (in_array("{$node}id", $attrs))
      $attr = "{$node}id";
    else if (in_array($this->m_grammar->singularize($node)."id", $attrs))
      $attr = $this->m_grammar->singularize($node)."id";      
    else if (in_array($table, $attrs))
      $attr = $table;
    else if (in_array($this->m_grammar->singularize($table), $attrs))
      $attr = $this->m_grammar->singularize($table);
    else if (in_array($node, $attrs))
      $attr = $node;
    else if (in_array($this->m_grammar->singularize($node), $attrs))
      $attr = $this->m_grammar->singularize($node);
      
    return $attr;
  }
  
  /**
   * One-to-many relation support. You can call the hasMany method to indicate
   * that this node has a one-to-many relation with another node. The meta
   * policy will then try to guess, amongst other things, which fields should
   * be used for this relation.
   *
   * This method uses a smart name guessing scheme for the destination node.
   * If you enter the plural form of the (singular) node name it will still
   * be able to find the node. You can ommit the module name prefix if the
   * destination node resides in the same module as the source node. Ofcourse
   * you can also just use the real module/node name combination.
   *
   * The options list may contain several parameters to make more complex
   * relations work. The supported parameters are as follows:
   *
   * - dest(-ination)  destination attribute name
   * - filter          destination filter 
   *
   * @param $accessor  accessor name (complete description is given above)
   * @param $options   list op options (complete description is given above)
   */
  function hasMany($accessor, $templateOrOptions=array(), $options=array())
  {
    $template = NULL;
    if (is_array($templateOrOptions))
      $options = $templateOrOptions;
    else $template = $templateOrOptions;
    
    $this->m_attrs[getNodeType($accessor)] =
      array(
        "type" => "atk.meta.relations.atkmetaonetomanyrelation",
        "params" => array($accessor, $template, $options),
        "flags" => AF_HIDE_LIST,
        "tabs" => NULL,
        "order" => $this->_getMaxOrder() + 100
      );
  }
  
  /**
   * Many-to-one / one-to-one relation support. You can call the hasOne method
   * to indicate that this node has a many-to-one or a one-to-one relation with
   * another node. The meta policy will then try to guess, amongst other
   * things, which fields should be used for this relation.
   *
   * To determine if a many-to-one or a one-to-one relation should be used
   * the system will check if the source node contains an attribute for 
   * storing the relation. If so the system will use a many-to-one relation,
   * else a one-to-one relation will be used.
   *
   * This method uses a smart name guessing scheme for the destination node.
   * If you enter the singular form of the (plural) node name it will still
   * be able to find the node. You can ommit the module name prefix if the
   * destination node resides in the same module as the source node. Ofcourse
   * you can also just use the real module/node name combination.
   *
   * The options list may contain several parameters to make more complex
   * relations work. The supported parameters are as follows:
   *
   * - source          source attribute name (should only be used for
   *                   many-to-one relations and will act as an indicator
   *                   for whatever this is a many-to-one relation or not)
   * - dest(-ination)  destination attribute name  (should only be used for
   *                   one-to-one relations and will act as an indicator
   *                   for whatever this is a one-to-one relation or not)
   * - filter          destination filter    
   * - large           boolean indicating if there will be lots and lots of
   *                   records in case of a many-to-one relation, same as
   *                   the AF_LARGE flag (defaults to false)
   *
   * @param $accessor  accessor name (complete description is given above)
   * @param $options   list op options (complete description is given above)
   */
  function hasOne($accessor, $templateOrOptions=array(), $options=array())
  {
    $template = NULL;
    if (is_array($templateOrOptions))
      $options = $templateOrOptions;
    else $template = $templateOrOptions;
  
    $node = getNodeType($accessor);
    $attr = NULL;
    
    if (array_key_exists("source", $options))
      $attr = $options["source"];
      
    else if (!array_key_exists("dest", $options) && !array_key_exists("destination", $options))
    {
      foreach (array_keys($this->m_attrs) as $name)
      {
        if ($name == "{$node}_id")
          $attr = "{$node}_id";
        else if ($name == "{$node}id")
          $attr = "{$node}id";
        else if ($name == $this->m_grammar->singularize($node)."_id")
          $attr = $this->m_grammar->singularize($node)."_id";
        else if ($name == $this->m_grammar->singularize($node)."id")
          $attr = $this->m_grammar->singularize($node)."id";
        else if ($name == $node)
          $attr = $node;
        else if ($name == $this->m_grammar->singularize($node))
          $attr = $this->m_grammar->singularize($node);
          
        if ($attr != NULL) break;
      }
    }

    $this->m_attrs[$attr != NULL ? $attr : $node] =
      array(
        "type" => "atk.meta.relations.atkmeta".($attr != NULL ? 'many' : 'one')."toonerelation",
        "params" => array($accessor, $template, $options),
        "flags" => ($attr != NULL ? $this->m_attrs[$attr]["flags"] : 0) | (array_key_exists("large", $options) && $options["large"] ? AF_LARGE : 0),
        "tabs" => $attr != NULL ? $this->m_attrs[$attr]["tabs"] : NULL,
        "order" => $attr != NULL ? $this->m_attrs[$attr]["order"] : $this->_getMaxOrder() + 100        
      );
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
    $db = &$this->m_node->getDb();
    $cols = $db->tableMeta($this->m_node->getTable());
    foreach ($cols as $name => $meta)
      $this->_initAttribute($name, $meta);
  }  
  
  /**
   * Checks if a certain attribute should be included or not.
   * Checks both the include and exclude lists.
   *
   * @param String $name
   * @return Boolean attribute included?
   */
  function _isAttributeIncluded($name)
  {
    if ($this->m_includes !== NULL && in_array($name, $this->m_includes)) return true;
    if ($this->m_excludes !== NULL && in_array($name, $this->m_excludes)) return false;
    
    if ($this->m_includes !== NULL)
    {
      foreach ($this->m_includes as $include)
        if ($include{0} == '/' && preg_match($include, $name)) return true;
      return false;
    }
    
    if ($this->m_excludes !== NULL)
    {
      foreach ($this->m_excludes as $exclude)
        if ($exclude{0} == '/' && preg_match($exclude, $name)) return false;
      return true;
    }
    
    return true;
  }
  
  /**
   * Apply attribute.
   */
  function _applyAttribute($name, $type, $params, $flags, $tabs, $order)
  {
    if (!$this->_isAttributeIncluded($name)) return;
  
    if (atkexists("attribute", $type))
      atkuse("attribute", $type);
    elseif (atkexists("relation", $type))
      atkuse("relation", $type);
    else atkimport($type);
    
    if (strrpos($type, ".") !== FALSE)
      $type = substr($type, strrpos($type, ".") + 1);

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

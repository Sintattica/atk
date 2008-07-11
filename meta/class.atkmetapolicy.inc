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
 * @copyright (c) 2004-2008 Peter C. Verhage
 * @license http://www.achievo.org/atk/licensing ATK Open Source License
 *
 * @version $Revision$
 * $Id$
 */
atkimport("atk.meta.grammar.atkmetagrammar");
atkimport("atk.meta.compiler.atkmetacompiler");

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
  /**
   * No default value.
   */
  const NO_DEFAULT_VALUE = '!@NO_DEFAULT_VALUE@!';

  /**
   * The node which is being constructed by the policy.
   *
   * @var atkMetaNode
   */
  protected $m_node = null;

  /**
   * The meta grammar which is being used by the policy.
   *
   * @var atkMetaGrammar
   */
  protected $m_grammar = null;

  /**
   * The meta compiler which is being used by the policy.
   *
   * @var atkMetaCompiler
   */
  protected $m_compiler = null;

  /**
   * The meta handler which is begin used by the policy. A function name or
   * handler array (class/object and method name). If set to null we will check
   * if the meta node contains a method called meta and will call this instead.
   *
   * @var mixed
   */
  protected $m_handler = null;

  /**
   * Table meta data.
   *
   * @var array
   */
  protected $m_metaData = null;

  /**
   * Node database name.
   *
   * @var string
   */
  protected $m_nodeDatabase = null;

  /**
   * Node table name.
   *
   * @var string
   */
  protected $m_nodeTable = null;

  /**
   * Node sequence name.
   *
   * @var string
   */
  protected $m_nodeSequence = null;

  /**
   * Node flags.
   *
   * @var int
   */
  protected $m_nodeFlags = null;

  /**
   * Node descriptor.
   *
   * @var string
   */
  protected $m_nodeDescriptor = null;

  /**
   * Node (default) order.
   *
   * @var string
   */
  protected $m_nodeOrder = null;

  /**
   * Node index column.
   *
   * @var string
   */
  protected $m_nodeIndex = null;

  /**
   * Node filter.
   *
   * @var string
   */
  protected $m_nodeFilter = null;

  /**
   * Node security alias.
   *
   * @var string
   */
  protected $m_nodeSecurityAlias = null;

  /**
   * Node security map.
   *
   * @var string
   */
  protected $m_nodeSecurityMap = null;

  /**
   * Included attributes.
   *
   * @var array|null
   */
  protected $m_includes = null;

  /**
   * Excluded attributes.
   *
   * @var array|null
   */
  protected $m_excludes = null;

  /**
   * Node attributes.
   *
   * @var array
   */
  protected $m_attrs = array();

  /**
   * Returns an instance of the meta policy using the given class. If no class
   * is specified the default meta policy is used determined using the
   * $config_meta_policy variable.
   *
   * @param atkMetaNode $node  policy node
   * @param string      $class full ATK policy class path
   *
   * @return atkMetaPolicy meta policy
   */
  public static function create(atkMetaNode $node, $class=null)
  {
    if (!is_string($class) || strlen($class) == 0)
    {
      $class = atkconfig("meta_policy", "atk.meta.atkmetapolicy");
    }

    return atknew($class, $node);
  }

  /**
   * Constructor.
   *
   * @param atkMetaNode policy node
   */
  public function __construct(atkMetaNode $node)
  {
    $this->setNode($node);
  }

  /**
   * Returns the node for this policy.
   *
   * @return atkNode policy node
   */
  public function getNode()
  {
    return $this->m_node;
  }

  /**
   * Sets the node for this policy.
   *
   * @param atkNode policy node
   */
  public function setNode(atkMetaNode $node)
  {
    $this->m_node = $node;
  }

  /**
   * Returns the meta grammar.
   *
   * @return atkMetaGrammar the meta grammar
   */
  public function getGrammar()
  {
    return $this->m_grammar;
  }

  /**
   * Sets the meta grammar.
   *
   * @param atkMetaGrammar $grammar the meta grammar
   */
  public function setGrammar(atkMetaGrammar $grammar)
  {
    $this->m_grammar = $grammar;
  }

  /**
   * Returns the meta compiler.
   *
   * @return atkMetaCompiler the meta compiler
   */
  public function getCompiler()
  {
    return $this->m_compiler;
  }

  /**
   * Sets the meta compiler.
   *
   * @param atkMetaCompiler $compiler the meta compiler
   */
  public function setCompiler(atkMetaCompiler $compiler)
  {
    $this->m_compiler = $compiler;
  }

  /**
   * Returns the meta handler.
   *
   * @return mixed the meta handler
   */
  public function getHandler()
  {
    return $this->m_handler;
  }

  /**
   * Sets the meta handler.
   *
   * @param mixed $handler the meta compiler
   */
  public function setHandler($handler)
  {
    $this->m_handler = $handler;
  }

  /**
   * Returns the meta data for this policy's table.
   *
   * @return array meta data
   */
  public function getMetaData()
  {
    return $this->m_metaData;
  }

  /**
   * Sets the meta data for this policy's table.
   *
   * @param array meta data
   */
  protected function _setMetaData($metaData)
  {
    $this->m_metaData = $metaData;
  }

  /**
   * Returns the node database.
   *
   * @return string database name
   */
  public function getNodeDatabase()
  {
    return $this->m_nodeDatabase;
  }

  /**
   * Sets the node database.
   *
   * Only for internal use, because it's too late to set this from inside the meta method.
   *
   * @param string $database database name
   */
  protected function _setNodeDatabase($database)
  {
    $this->m_nodeDatabase = $database;
  }

  /**
   * Returns the node table.
   *
   * @return string table name
   */
  public function getNodeTable()
  {
    return $this->m_nodeTable;
  }

  /**
   * Sets the node table name.
   *
   * Only for internal use, because it's too late to set this from inside the meta method.
   *
   * @param string $table table name
   */
  protected function _setNodeTable($table)
  {
    $this->m_nodeTable = $table;
  }

  /**
   * Returns the node sequence.
   *
   * @return string sequence name
   */
  public function getNodeSequence()
  {
    return $this->m_nodeSequence;
  }

  /**
   * Sets the node sequence name.
   *
   * @param string $sequence sequence name
   */
  public function setNodeSequence($sequence)
  {
    $this->m_nodeSequence = $sequence;
  }

  /**
   * Returns the node flags.
   *
   * @return int node flags
   */
  public function getNodeFlags()
  {
    return $this->m_nodeFlags;
  }

  /**
   * Sets the node flags
   *
   * @param int $flags node flags
   */
  public function setNodeFlags($flags)
  {
    $this->m_nodeFlags = $flags;
  }

  /**
   * Returns the node descriptor.
   *
   * @return string descriptor
   */
  public function getNodeDescriptor()
  {
    return $this->m_nodeDescriptor;
  }

  /**
   * Sets the node descriptor.
   *
   * @param string $descriptor descriptor
   */
  public function setNodeDescriptor($descriptor)
  {
    $this->m_nodeDescriptor = $descriptor;
  }

  /**
   * Returns the (default) node order.
   *
   * @return string node order
   */
  public function getNodeOrder()
  {
    return $this->m_nodeOrder;
  }

  /**
   * Sets the (default) node order.
   *
   * @param string $order node order
   */
  public function setNodeOrder($order)
  {
    $this->m_nodeOrder = $order;
  }

  /**
   * Returns the index column.
   *
   * @return string index column
   */
  public function getNodeIndex()
  {
    return $this->m_nodeIndex;
  }

  /**
   * Sets the node index column.
   *
   * @param string $column index column
   */
  public function setNodeIndex($column)
  {
    $this->m_nodeIndex = $column;
  }

  /**
   * Returns the node filter.
   *
   * @return string filter
   */
  public function getNodeFilter()
  {
    return $this->m_nodeFilter;
  }

  /**
   * Sets the node filter.
   *
   * @param string $filter filter
   */
  public function setNodeFilter($filter)
  {
    $this->m_nodeFilter = $filter;
  }

  /**
   * Returns the node security alias.
   *
   * @return string security alias
   */
  public function getNodeSecurityAlias()
  {
    return $this->m_nodeSecurityAlias;
  }

  /**
   * Sets the node security alias.
   *
   * @param string $alias security alias
   */
  public function setNodeSecurityAlias($alias)
  {
    $this->m_nodeSecurityAlias = $alias;
  }

  /**
   * Returns the additional node security map
   *
   * @return array security map
   */
  public function getNodeSecurityMap()
  {
    return $this->m_nodeSecurityMap;
  }

  /**
   * Sets the node additional security map.
   *
   * @param array $map security map array
   */
  public function setNodeSecurityMap($map)
  {
    $this->m_nodeSecurityMap = $map;
  }

  /**
   * Get attribute type and params for the given attribute/column.
   *
   * Returns an array which looks like the following:
   * array("type" => ..., "params" => array(...))
   *
   * @param string $name attribute/column name
   * @param array  $meta column meta data
   *
   * @return array type and params
   */
  protected function _getTypeAndParams($name, $meta)
  {
    $type = NULL;
    $params = array();

    if (in_array($name, array("passw", "password")))
    {
      $type = "atk.attributes.atkpasswordattribute";
      $params = array(true);
    }
    else if (in_array($name, array("email", "e-mail")))
    {
      $type = "atk.attributes.atkemailattribute";
      $params = array(false);
    }
    else if ($name == 'country')
    {
      $type = 'atk.attributes.atkcountryattribute';
    }
    else if ($name == 'timezone')
    {
      $type = 'atk.attributes.atktimezoneattribute';
    }
    else if ($name == 'created_at' || $name == 'created_on')
    {
      $type = 'atk.attributes.atkcreatestampattribute';
    }
    else if ($name == 'updated_at' || $name == 'updated_on')
    {
      $type = 'atk.attributes.atkupdatestampattribute';
    }
    else if ($name == 'created_by')
    {
      $type = 'atk.attributes.atkcreatedbyattribute';
    }
    else if ($name == 'updated_by')
    {
      $type = 'atk.attributes.atkupdatedbyattribute';
    }
    else if ($meta['gentype'] == 'number' && $meta['len'] == 1 &&
             (substr($name, 0, 3) == 'is_' || substr($name, 0, 4) == 'has_'))
    {
      $type = 'atk.attributes.atkboolattribute';
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
   * Returns the auto-detected flags for the given attribute.
   *
   * @param string $name attribute/column name
   * @param array  $meta column meta data
   *
   * @return int flags
   */
  protected function _getFlags($name, $meta)
  {
    $flags =
      (hasFlag($meta['flags'], MF_PRIMARY) ? AF_PRIMARY : 0) |
      (hasFlag($meta['flags'], MF_UNIQUE) ? AF_UNIQUE : 0) |
      (hasFlag($meta['flags'], MF_NOT_NULL) ? AF_OBLIGATORY : 0) |
      (hasFlag($meta['flags'], MF_AUTO_INCREMENT|MF_PRIMARY) ? AF_AUTOKEY : 0) |
      ($meta['gentype'] == "text" ? AF_HIDE_LIST : 0);

    if (hasFlag($flags, AF_PRIMARY) && $meta['num'] == 0 &&
        in_array($name, array("id", $meta['table']."id", $meta['table']."_id")))
      $flags |= AF_AUTOKEY;

    if (in_array($name, array("passw", "password")))
    {
      $flags |= AF_HIDE_LIST;
    }

    return $flags;
  }

  /**
   * Get auto-detected order for the given attribute.
   *
   * @param string $name column/attribute name
   * @param array  $meta column meta data
   *
   * @return int order
   */
  protected function _getOrder($name, $meta)
  {
    return ($meta['num'] + 1) * 100;
  }


  /**
   * Get auto-detected default value for the given attribute.
   *
   * @param string $name column/attribute name
   * @param array  $meta column meta data
   *
   * @return mixed default value
   */
  protected function _getDefaultValue($name, $meta)
  {
    if (array_key_exists('default', $meta) && $meta['default'] == "NOW" && in_array($meta['gentype'], array('date', 'time', 'datetime')))
    {
      $stamp = getdate();
      $date = array('day' => $stamp['yday'], 'month' => $stamp['mon'], 'year' => $stamp['year']);
      $time = array('hours' => $stamp['hours'], 'minutes' => $stamp['minutes'], 'seconds' => $stamp['seconds']);
      return array_merge($meta['gentype'] == 'time' ? array() : $date, $meta['gentype'] == 'date' ? array() : $time);
    }
    else if (array_key_exists('default', $meta))
    {
      return $meta['default'];
    }
    else
    {
      return self::NO_DEFAULT_VALUE;
    }
  }

  /**
   * Calls a method of this object with the given parameters.
   *
   * @param string $method the method name
   * @param array  $params the method parameters
   *
   * @return mixed result
   */
  protected function _call($method, $params)
  {
    return call_user_func_array(array($this, $method), $params);
  }

  /**
   * Returns the currently set includes (if applicable).
   *
   * @return array includes list
   */
  public function getIncludes()
  {
    return $this->m_includes;
  }

  /**
   * Set includes. Implicitly sets the order!
   *
   * NOTE: Attributes manually added through the policy will always be
   *       included unless they are explicitly removed using the remove
   *       method!
   *
   * This method can also be called with multiple attribute string arguments.
   *
   * @param string|array $attrs attribute name(s)
   */
  public function setIncludes($attrs)
  {
    if (!is_array($attrs))
      $attrs = func_get_args();

    $this->m_includes = $attrs;
    $this->m_excludes = NULL;

    $this->setOrder($attrs);
  }

  /**
   * Returns the currently set excludes (if applicable).
   *
   * @return array excludes list
   */
  public function getExcludes()
  {
    return $this->m_excludes;
  }

  /**
   * Set excludes.
   *
   * NOTE: Attributes manually added through the policy will always be
   *       included unless they are explicitly removed using the remove
   *       method!
   *
   * This method can also be called with multiple attribute string arguments.
   *
   * @param string|array $attrs attribute name(s)
   */
  public function setExcludes($attrs)
  {
    if (!is_array($attrs))
      $attrs = func_get_args();

    $this->m_excludes = $attrs;
    $this->m_includes = NULL;
  }

  /**
   * Sets the attribute order. All attributes not mentioned will be moved to
   * the bottom using their current order.
   *
   * This method can also be called with multiple attribute string arguments.
   *
   * @param string|array $attrs attribute name(s)
   */
  public function setOrder($attrs)
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
   * Set the default value for the given attribute(s).
   *
   * This method can also be called with multiple attribute string arguments
   * as long as the last argument contains the default value.
   *
   * @param array|string $attrs  list of attribute names or single attribute name
   * @param mixed $value         default value
   */
  public function setDefaultValue($attrs, $value)
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
        $this->m_attrs[$attr]["default"] = $value;
  }

  /**
   * Set flag(s) for the given attribute or list of attributes.
   *
   * This method can also be called with multiple attribute string arguments
   * as long as the last argument contains the flag(s).
   *
   * NOTE: this method will overwrite all currently set flags, including
   *       automatically detected flags!
   *
   * @param array|string $attrs  list of attribute names or single attribute name
   * @param int          $flags  flag(s)
   */
  public function setFlag($attrs, $flags)
  {
    $params = func_get_args();
    $this->_call("setFlags", $params);
  }

  /**
   * Set flag(s) for the given attribute or list of attributes.
   *
   * This method can also be called with multiple attribute string arguments
   * as long as the last argument contains the flag(s).
   *
   * NOTE: this method will overwrite all currently set flags, including
   *       automatically detected flags!
   *
   * @param array|string $attrs  list of attribute names or single attribute name
   * @param int          $flags  flag(s)
   */
  public function setFlags($attrs, $flags)
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
   * Add flag(s) for the given attribute or list of attributes.
   *
   * This method can also be called with multiple attribute string arguments
   * as long as the last argument contains the flag(s).
   *
   * @param array|string $attrs  list of attribute names or single attribute name
   * @param int          $flags  flag(s)
   */
  public function addFlag($attrs, $flags)
  {
    $params = func_get_args();
    $this->_call("addFlags", $params);
  }

  /**
   * Add flag(s) for the given attribute or list of attributes.
   *
   * This method can also be called with multiple attribute string arguments
   * as long as the last argument contains the flag(s).
   *
   * @param array|string $attrs  list of attribute names or single attribute name
   * @param int          $flags  flag(s)
   */
  public function addFlags($attrs, $flags)
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
   *
   * This method can also be called with multiple attribute string arguments
   * as long as the last argument contains the flag(s).
   *
   * @param array|string $attrs  list of attribute names or single attribute name
   * @param int          $flags  flag(s)
   */
  public function removeFlag($attrs, $flags)
  {
    $params = func_get_args();
    $this->_call("removeFlags", $params);
  }

  /**
   * Remove flag(s) from the given attribute or list of attributes.
   *
   * This method can also be called with multiple attribute string arguments
   * as long as the last argument contains the flag(s).
   *
   * @param array|string $attrs  list of attribute names or single attribute name
   * @param int          $flags  flag(s)
   */
  public function removeFlags($attrs, $flags)
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
   * Enable force insert for the given attribute or list of attributes.
   *
   * This method can also be called with multiple attribute string arguments.
   *
   * @param string|array $attrs attribute name(s)
   */
  public function addForceInsert($attrs)
  {
    if (!is_array($attrs))
      $attrs = func_get_args();

    foreach ($attrs as $attr)
      if (array_key_exists($attr, $this->m_attrs))
        $this->m_attrs[$attr]["forceInsert"] = true;
  }

  /**
   * Disable force insert for the given attribute or list of attributes.
   *
   * This method can also be called with multiple attribute string arguments.
   *
   * @param string|array $attrs attribute name(s)
   */
  public function removeForceInsert($attrs)
  {
    if (!is_array($attrs))
      $attrs = func_get_args();

    foreach ($attrs as $attr)
      if (array_key_exists($attr, $this->m_attrs))
        $this->m_attrs[$attr]["forceInsert"] = false;
  }

  /**
   * Enable force update for the given attribute or list of attributes.
   *
   * This method can also be called with multiple attribute string arguments.
   *
   * @param string|array $attrs attribute name(s)
   */
  public function addForceUpdate($attrs)
  {
    if (!is_array($attrs))
      $attrs = func_get_args();

    foreach ($attrs as $attr)
      if (array_key_exists($attr, $this->m_attrs))
        $this->m_attrs[$attr]["forceUpdate"] = true;
  }

  /**
   * Disable force update for the given attribute or list of attributes.
   *
   * This method can also be called with multiple attribute string arguments.
   *
   * @param string|array $attrs attribute name(s)
   */
  public function removeForceUpdate($attrs)
  {
    if (!is_array($attrs))
      $attrs = func_get_args();

    foreach ($attrs as $attr)
      if (array_key_exists($attr, $this->m_attrs))
        $this->m_attrs[$attr]["forceUpdate"] = false;
  }

  /**
   * Set the sections/tabs for the given attribute or list of attributes.
   *
   * This method can also be called with multiple attribute string arguments
   * as long as the last argument contains the section name(s).
   *
   * @param array|string $attrs  list of attribute names or single attribute name
   * @param array|string $tabs   tab name(s)
   */
  function setTab($attrs, $tabs)
  {
    $params = func_get_args();
    $this->_call("setTabs", $params);
  }

  /**
   * Set the sections/tabs for the given attribute or list of attributes.
   *
   * This method can also be called with multiple attribute string arguments
   * as long as the last argument contains the section name(s).
   *
   * @param array|string $attrs  list of attribute names or single attribute name
   * @param array|string $tabs   tab name(s)
   */
  public function setTabs($attrs, $tabs)
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
   * Set the sections/tabs for the given attribute or list of attributes.
   *
   * This method can also be called with multiple attribute string arguments
   * as long as the last argument contains the section name(s).
   *
   * @param array|string $attrs    list of attribute names or single attribute name
   * @param array|string $sections section name(s)
   */
  public function setSection($attrs, $sections)
  {
    $params = func_get_args();
    $this->_call("setTabs", $params);
  }

  /**
   * Set the sections/tabs for the given attribute or list of attributes.
   *
   * This method can also be called with multiple attribute string arguments
   * as long as the last argument contains the section name(s).
   *
   * @param array|string $attrs    list of attribute names or single attribute name
   * @param array|string $sections section name(s)
   */
  public function setSections($attrs, $sections)
  {
    $params = func_get_args();
    $this->_call("setTabs", $params);
  }

  /**
   * Sets the attribute type. All extra arguments after the two
   * standard arguments will be threated as parameters for the
   * attribute. If you need to pass arguments by reference you can
   * better use the setTypeAndParams method.
   *
   * @param string|array $attr  the attribute name or a list of attributes
   * @param string $type        full ATK attribute class (e.g. atk.attributes.atkboolattribute)
   * @param mixed ...           all other arguments will be threated as parameters
   */
  public function setType($attr, $type)
  {
    $params = func_get_args();
    $params = array_slice($params, 2);
    $this->setTypeAndParams($attr, $type, $params);
  }

  /**
   * Sets the attribute type and parameters.
   *
   * @param string|array $attr   the attribute name or a list of attributes
   * @param string       $type   full ATK attribute class (e.g. atk.attributes.atkboolattribute)
   * @param array        $params parameters for the attribute (optional)
   */
  public function setTypeAndParams($attr, $type, $params=array())
  {
    $attrs = is_array($attr) ? $attr : array($attr);
    foreach ($attrs as $attr)
    {
      $this->m_attrs[$attr]["type"] = $type;
      $this->m_attrs[$attr]["params"] = $params;
    }
  }

  /**
   * Returns the current maximum order in the attribute list.
   *
   * @return int max order
   */
  protected function _getMaxOrder()
  {
    $max = 0;

    foreach (array_keys($this->m_attrs) as $key)
    {
      if (isset($this->m_attrs[$key]["order"]) && $this->m_attrs[$key]["order"] > $max)
      {
        $max = $this->m_attrs[$key]["order"];
      }
    }

    return $max;
  }

 /**
   * Find destination node for the given meta relation.
   *
   * @param string $accessor  accessor name
   * @param bool   $toMany    accessor name might be in plural form?
   *
   * @return string destination node name for the given relation
   */
  protected function _findDestination($accessor, $toMany)
  {
    $module = getNodeModule($accessor);
    if ($module == "")
      $module = $this->m_node->m_module;

    $node = getNodeType($accessor);

    if ($module != "")
    {
      if (nodeExists("$module.$node"))
        return "$module.$node";

      if ($toMany && nodeExists("$module.".$this->getGrammar()->singularize($node)))
        return "$module.".$this->getGrammar()->singularize($node);

      if (!$toMany && nodeExists("$module.".$this->getGrammar()->pluralize($node)))
        return "$module.".$this->getGrammar()->pluralize($node);

      if (nodeExists("{$module}.{$module}_{$node}"))
        return "{$module}.{$module}_{$node}";

      if ($toMany && nodeExists("{$module}.{$module}_".$this->getGrammar()->singularize($node)))
        return "{$module}.{$module}_".$this->getGrammar()->singularize($node);

      if (!$toMany && nodeExists("{$module}.{$module}_".$this->getGrammar()->pluralize($node)))
        return "{$module}.{$module}_".$this->getGrammar()->pluralize($node);
    }

    if (nodeExists($node))
      return $node;

    if ($toMany && nodeExists($this->getGrammar()->singularize($node)))
      return $this->getGrammar()->singularize($node);

    if (!$toMany && nodeExists($this->getGrammar()->pluralize($node)))
      return $this->getGrammar()->pluralize($node);

    return NULL;
  }

  /**
   * Returns a list of possible attribute name variants for relations
   * which reference this node or the given destination node.
   *
   * @param string $destination destination node
   *
   * @return array list of attribute variants
   */
  protected function _getDestinationAttributeVariants($destination=null)
  {
    $base = array();

    // no destination given, self is assumed, we also add the table name
    // and parent classes as base for the different variants
    if ($destination == null)
    {
      $module = getNodeModule($this->getNode()->atkNodeType());

      $base[] = $this->getNodeTable();
      $base[] = getNodeType($this->getNode()->atkNodeType());

      for ($class = get_class($this->getNode()); stripos($class, 'metanode') === false; $class = get_parent_class($class))
      {
        $base[] = strtolower($class);
      }

      $base = array_unique($base);
    }
    else
    {
      $module = getNodeModule($destination);
      $base[] = getNodeType($destination);
    }

    if ($module != null)
    {
      // add variants for each base with the module as prefix or with the module
      // prefix stripped out (if it was already part of base), we explicitly
      // make a copy of base so that new entries don't mess up the loop
      foreach (array_values($base) as $entry)
      {
        // entry already contains module prefix, strip it
        if (substr($entry, 0, strlen($module) + 1) == $module.'_')
        {
          $base[] = substr($entry, strlen($module) + 1);
        }

        // entry doesn't contain prefix yet, add it
        else
        {
          $base[] = $module."_".$entry;
        }
      }
    }

    $variants = array();

    foreach ($base as $entry)
    {
      $variants[] = "{$entry}_id";
      $variants[] = $this->getGrammar()->singularize($entry)."_id";
      $variants[] = "{$entry}id";
      $variants[] = $this->getGrammar()->singularize($entry)."id";
      $variants[] = $entry;
      $variants[] = $this->getGrammar()->singularize($entry);
    }

    $variants = array_values(array_unique($variants));

    return $variants;
  }

  /**
   * Find source attribute for a many-to-one relation that point to the
   * given destination node.
   *
   * @param string $destination destination node type
   *
   * @return string source attribute name
   */
  protected function _findSourceAttribute($destination)
  {
    $module = getNodeModule($destination);
    $type = getNodeType($destination);

    $prefixes = $module == null ? array('') : array('', "{$module}_");

    foreach ($prefixes as $leftPrefix)
    {
      foreach ($prefixes as $rightPrefix)
      {
        foreach (array_keys($this->m_attrs) as $name)
        {
          switch ($leftPrefix.$name)
          {
            case "{$rightPrefix}{$type}_id":
            case "{$rightPrefix}{$type}id":
            case $rightPrefix.$this->getGrammar()->singularize($type)."_id":
            case $rightPrefix.$this->getGrammar()->singularize($type)."id":
            case $rightPrefix.$type:
            case $rightPrefix.$this->getGrammar()->singularize($type):
              return $name;
          }
        }
      }
    }

    return null;
  }

  /**
   * One-to-many / many-to-many relation support. You can call the hasMany
   * method to indicate that this node has a one-to-many or a many-to-many
   * relationship with another node. The meta policy will then try to guess,
   * amongst other things, which fields should be used for this relation.
   *
   * This method uses a smart name guessing scheme for the (optional
   * intermediate) and destination node. If you enter the plural form of
   * the (singular) node name it will still be able to find the node.
   * You can ommit the module name prefix if the destination node resides
   * in the same module as the source node. Ofcourse you can also just use
   * the real module/node name combination.
   *
   * The options list may contain several parameters to make more complex
   * relations work. The supported parameters are as follows:
   *
   * - dest(-ination)  destination attribute name
   * - filter          destination filter
   * - through         intermediate node name (for many-to-many relations)
   * - type            type of many-to-many relation (shuttle, bool(ean) or list, defaults to shuttle)
   * - cols/columns    number of columns (many-to-many bool relations only)
   * - rows            number of rows (many-to-many list relations only)
   * - name            name for this relation (by default getNodeType($accessor))
   *
   * @param string       $accessor          accessor name (complete description is given above)
   * @param string|array $templateOrOptions template or list of options (complete description is given above)
   * @param array        $options           list op options (complete description is given above)
   * @param int          $flags 	          the flags for the relation
   */
  public function hasMany($accessor, $templateOrOptions=array(), $options=array(), $flags=0)
  {
    $template = NULL;
    if (is_array($templateOrOptions))
      $options = $templateOrOptions;
    else $template = $templateOrOptions;

    if (isset($options['name']))
    {
      $name = $options['name'];
    }
    else
    {
      $name = getNodeType($accessor);
    }

    if (isset($options['source']))
    {
      $options['local'] = $options['source'];
    }

    $destination = $this->_findDestination($accessor, true);

    if (isset($options['through']))
    {
      switch (@$options['type'])
      {
        case 'bool':
        case 'boolean':
          $type = "atk.meta.relations.atkmetamanyboolrelation";
          break;
        case 'list':
          $type = "atk.meta.relations.atkmetamanytomanylistrelation";
          break;
        case 'select':
          $type = "atk.meta.relations.atkmetamanytomanyselectrelation";
          break;
        case 'eshuttle':
        case 'extendableshuttle':
          $type = "atk.meta.relations.atkmetaextendableshuttlerelation";
          break;
        case 'shuttle':
        default:
          $type = "atk.meta.relations.atkmetashuttlerelation";
      }

      $through = $this->_findDestination($options['through'], true);

      if (!isset($options['local']))
      {
        $options['localVariants'] = $this->_getDestinationAttributeVariants();
      }

      if (!isset($options['remote']))
      {
        $remoteVariants = $this->_getDestinationAttributeVariants($destination);
        if (isset($options['name']))
          $remoteVariants = array_merge($remoteVariants, $this->_getDestinationAttributeVariants($options['name']));
        $options['remoteVariants'] = $remoteVariants;
      }

      $params = array($destination, $through, $template, $options);
    }
    else
    {
      $type = "atk.meta.relations.atkmetaonetomanyrelation";

      $variants = $this->_getDestinationAttributeVariants();
      $options['variants'] = $variants;

      $params = array($destination, $template, $options);
    }

    $flags  = AF_HIDE_LIST|AF_HIDE_ADD|$flags;
    $tabs   = NULL;
    $order  = $this->_getMaxOrder() + 100 ;

    $this->add($name, $type, $params, $flags, $tabs, $order);
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
   * @param string       $accessor          accessor name (complete description is given above)
   * @param string|array $templateOrOptions template or list of options (complete description is given above)
   * @param array        $options           list op options (complete description is given above)
   * @param int          $flags 	          the flags for the relation
   */
  public function hasOne($accessor, $templateOrOptions=array(), $options=array(), $flags=0)
  {
    $template = NULL;
    if (is_array($templateOrOptions))
      $options = $templateOrOptions;
    else $template = $templateOrOptions;

    // look-up destination node
    $destination = $this->_findDestination($accessor, false);

    // explicit source given
    if (array_key_exists("source", $options))
    {
      // in case of multi referential key "source" is array
      if (is_array($options["source"]))
      {
        $attr = $options["source"][0]; // we use the first key as name of attribute
      }
      else
      {
        $attr = $options["source"];
      }
    }

    // no source and no destination given, still try to find a source attribute just to be sure
    // note that findSourceAttribute probably returns null for one-to-one relations
    else if (!array_key_exists("dest", $options) && !array_key_exists("destination", $options))
    {
      $attr = $this->_findSourceAttribute($destination);
    }

    // one-to-one relation, lookup possible destination attribute variants
    if ($attr == null && !array_key_exists("dest", $options) && !array_key_exists("destination", $options))
    {
      $options['variants'] = $this->_getDestinationAttributeVariants();
    }

    $name   = $attr != NULL ? $attr : getNodeType($accessor);
    $type   = "atk.meta.relations.atkmeta".($attr != NULL ? 'many' : 'one')."toonerelation";
    $params = array($destination, $template, $options);
    $flags  = ($attr != NULL ? $this->m_attrs[$attr]["flags"] : 0) | (array_key_exists("large", $options) && $options["large"] ? AF_LARGE : 0) | $flags;
    $tabs   = $attr != NULL ? $this->m_attrs[$attr]["tabs"] : NULL;
    $order  = $attr != NULL ? $this->m_attrs[$attr]["order"] : $this->_getMaxOrder() + 100;

    $this->add($name, $type, $params, $flags, $tabs, $order);
  }

  /**
   * Add / replace (custom) attribute.
   *
   * @param string|array $name     attribute name or list of attributes
   * @param string       $type     attribute type
   * @param array        $params   attribute parameters, excluding flags (optional)
   * @param int          $flags    attribute flags (optional)
   * @param string|array $sections sections/tabs to display the attribute on
   * @param int          $order    order of the attribute
   * @param mixed        $default  default value
   */
  public function add($name, $type, $params=array(), $flags=0, $sections=NULL, $order=NULL, $default=self::NO_DEFAULT_VALUE)
  {
    $names = is_array($name) ? $name : array($name);
    foreach ($names as $name)
    {
      if ($order === NULL && isset($this->m_attrs[$name]))
        $order = $this->m_attrs[$name]['order'];
      else if ($order === NULL)
        $order = $this->_getMaxOrder() + 100;

      $this->m_attrs[$name] =
        array(
          "type" => $type,
          "params" => $params,
          "flags" => $flags,
          "tabs" => $sections,
          "order" => $order
        );

      if ($default != self::NO_DEFAULT_VALUE)
      {
        $this->m_attrs[$name]['default'] = $default;
      }
    }
  }
  
  /**
   * Add fieldset.
   * 
   * To include an attribute label use [attribute.label] inside your
   * template. To include an attribute edit/display field use 
   * [attribute.field] inside your template.
   *
   * @param string       $name     name
   * @param string       $template template string
   * @param int          $flags    attribute flags
   * @param string|array $sections sections/tabs to display the attribute on
   * @param int          $order    order of the attribute
   */
  public function addFieldSet($name, $template, $flags=0, $sections=NULL, $order=NULL)
  {
    $this->add($name, 'atk.attributes.atkfieldset', array($template), $flags, $sections, $order);
  }  

  /**
   * Remove attribute.
   *
   * @param string|array $name attribute name
   */
  public function remove($name)
  {
    $names = is_array($name) ? $name : func_get_args();
    
    foreach ($names as $name)
    {
      unset($this->m_attrs[$name]);
    }
  }

  /**
   * Returns a reference to the attributes array.
   *
   * Be very careful when using this array, modifying it might void your warranty!
   *
   * @return array reference to the attributes array
   */
  public function &getAttributes()
  {
    return $this->m_attrs;
  }

  /**
   * Returns the attribute names.
   *
   * @return array string attribute names
   */
  public function getAttributeNames()
  {
    return array_keys($this->m_attrs);
  }

  /**
   * Translate using the node's module and type.
   */
  public function text($string, $module=NULL, $language='', $firstFallback="", $nodeDefaultText=false)
  {
    return $this->getNode()->text($string, $module, $language, $firstFallback, $nodeDefaultText);
  }

  /**
   * Utility method to bit-or two integers.
   *
   * @param int $a integer a
   * @param int $b integer b
   *
   * @return int result of bit-or
   */
  public static function bitOr($a, $b)
  {
    return $a|$b;
  }

  /**
   * Detect node table name.
   *
   * @return string table name
   */
  protected function _detectNodeTable()
  {
    $module = $this->getNode()->getModule();

    $base = array();
    $base[] = $this->getNode()->getType();
    $base[] = $module."_".$this->getNode()->getType();

    for ($class = get_class($this->getNode()); stripos($class, 'metanode') === false; $class = get_parent_class($class))
    {
      $base[] = strtolower($class);
      $base[] = $module."_".strtolower($class);
    }

    $db = atkGetDb($this->getNodeDatabase());
    foreach ($base as $entry)
    {
      if ($db->tableExists($entry))
        return $entry;
      else if ($db->tableExists($this->getGrammar()->singularize($entry)))
        return $this->getGrammar()->singularize($table);
      else if ($db->tableExists($this->getGrammar()->pluralize($entry)))
        return $this->getGrammar()->pluralize($entry);
    }

    return null;
  }

  /**
   * Detect node sequence name.
   *
   * @return string sequence name
   */
  protected function _detectNodeSequence()
  {
    $db = atkGetDb($this->getNodeDatabase());
    $cols = $this->getMetaData();
    $sequence = NULL;

    foreach ($cols as $name => $meta)
    {
      if (isset($meta['sequence']) && strlen($meta['sequence']) > 0)
      {
        $sequence = $meta['sequence'];
      }
    }

    if ($sequence == NULL)
    {
      $sequence = atkConfig("database_sequenceprefix").$this->getNodeTable();
    }

    return $sequence;
  }

  /**
   * Intialize attribute for node using the given column meta data.
   *
   * @param string $name column name
   * @param array  $meta column meta data
   */
  protected function _initAttribute($name, $meta)
  {
    $typeAndParams = $this->_getTypeAndParams($name, $meta);
    if ($typeAndParams["type"] === NULL) return;

    $type = $typeAndParams['type'];
    $params = $typeAndParams['params'];
    $flags = $this->_getFlags($name, $meta);
    $order = $this->_getOrder($name, $meta);
    $default = $this->_getDefaultValue($name, $meta);

    $this->add($name, $type, $params, $flags, null, $order, $default);
  }

  /**
   * Initialize attributes using policy.
   */
  protected function _init()
  {
    $grammar = $this->getNode()->getMetaOption('grammar');
    $grammar = atkMetaGrammar::get($grammar);
    $this->setGrammar($grammar);

    $compiler = $this->getNode()->getMetaOption('compiler');
    $compiler = atkMetaCompiler::get($compiler);
    $this->setCompiler($compiler);

    $handler = $this->getNode()->getMetaOption('handler');
    $this->setHandler($handler);

    $database = $this->getNode()->getMetaOption('database', $this->getNode()->getMetaOption('db', 'default'));
    $this->_setNodeDatabase($database);

    $table = $this->getNode()->getMetaOption('table');
    if ($table == null)
      $table = $this->_detectNodeTable();
    $this->_setNodeTable($table);

    $db = atkGetDb($database);
    $metaData = $db->tableMeta($table);
    $this->_setMetaData($metaData);

    $sequence = $this->getNode()->getMetaOption('sequence');
    if ($sequence == null)
      $sequence = $this->_detectNodeSequence();
    $this->setNodeSequence($sequence);

    $flags = $this->getNode()->getMetaOption('flags', 0);
    if (is_array($flags))
      $flags = array_reduce($flags, array('atkMetaPolicy', 'bitOr'), 0);
    $this->setNodeFlags($flags);

    $descriptor = $this->getNode()->getMetaOption('descriptor');
    $this->setNodeDescriptor($descriptor);

    $order = $this->getNode()->getMetaOption('order');
    $this->setNodeOrder($order);

    $index = $this->getNode()->getMetaOption('index');
    $this->setNodeIndex($index);

    $filter = $this->getNode()->getMetaOption('filter');
    $this->setNodeFilter($filter);

    $securityAlias = $this->getNode()->getMetaOption('securityAlias');
    $this->setNodeSecurityAlias($securityAlias);

    $securityMap = $this->getNode()->getMetaOption('securityMap');
    $this->setNodeSecurityMap($securityMap);

    foreach ($metaData as $name => $meta)
      $this->_initAttribute($name, $meta);
  }

  /**
   * Modify meta policy, by default the meta method of the node is called.
   */
  protected function _meta()
  {
    // handler / callback is set, call the handler
    if ($this->getHandler() != null)
    {
      call_user_func($this->getHandler(), $this);
      return;
    }

    // no handler set, try to call the node's meta method if it exists
    if (!method_exists($this->getNode(), 'meta')) return;

    $method = new ReflectionMethod($this->getNode(), 'meta');

    if ($method->isStatic())
    {
      $method->invoke(get_class($this->getNode()), $this);
    }
    else
    {
      $this->getNode()->meta($this);
    }
  }

  /**
   * Compile policy.
   *
   * @return string code compiled code
   */
  protected function _compile()
  {
    return $this->getCompiler()->compile($this);
  }

  /**
   * Write compiled meta node code to cache.
   *
   * @param string $code compiled code
   *
   * @return string file path
   */
  protected function _cache($code)
  {
    atkimport('atk.utils.atktmpfile');
    $node = $this->getNode();
    $file = new atkTmpFile("meta/".$this->getNode()->getModule()."/".$this->getNode()->getType().".php");
    $file->writeFile("<?php\n$code");
    return $file->getPath();
  }

  /**
   * Build / setup node using the collected attributes.
   */
  public function apply()
  {
    $this->_init();
    $this->_meta();
    $code = $this->_compile();

    atk_var_dump("\n\n$code", "atkMetaPolicy::apply - ".$this->getNode()->atkNodeType());

    $node = $this->getNode();

    if ($this->getNode()->isCacheable())
    {
      $file = $this->_cache($code);
      include($file);
    }
    else
    {
      eval($code);
    }
  }
}
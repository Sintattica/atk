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
 * @version $Revision: 3864 $
 * $Id: class.atkmetagrammar.inc 3864 2006-06-26 14:29:18Z boy $
 */
 
/**
 * The meta policy compiler, transforms a meta policy to node code.
 * 
 * @author Peter C. Verhage <peter@achievo.org>
 *
 * @package atk
 * @subpackage meta
 */
class atkMetaCompiler
{
  /**
   * Compiler instances.
   *
   * @var array instances
   */
  static $instances;
  
  /**
   * Returns an instance of the meta compiler with the given class. If no class
   * is specified the default meta compiler is used determined using the 
   * $config_meta_compiler variable.
   *
   * @param string $class full ATK compiler class path
   * 
   * @return atkMetaCompiler meta compiler
   */
  public static function get($class=null)
  {
    if (!is_string($class) || strlen($class) == 0)
    {
      $class = atkconfig("meta_compiler", "atk.meta.compiler.atkmetacompiler");
    }
    
    if (!isset(self::$instances[$class]))
    {
      self::$instances[$class] = atknew($class);
    }
    
    return self::$instances[$class];
  }   
  
  /**
   * Generates an flags string for a given flags value.
   * 
   * @param int    $flags            flags
   * @param string $prefix           flags prefix
   * @param array  $specificExcludes values of customizable flags which can differ for subclasses
   * @param array  $constExcludes    constant names of constants that should be excluded
   * 
   * @return string flags string
   */
  protected function getFlagsString($flags, $prefix, $specificExcludes=array(), $constExcludes=array())
  {
    static $constants = array();
    
    if (!isset($constants[$prefix])) 
    {
      $allConstants = get_defined_constants();

      $constants[$prefix] = array();
      foreach ($allConstants as $const => $value)
      {
        if (substr($const, 0, strlen($prefix)) == $prefix && 
            (substr($const, 0, strlen($prefix) + 9) == $prefix.'SPECIFIC_' || !in_array($value, $specificExcludes)) && 
            !in_array($const, $constExcludes))
        {
          $constants[$prefix][$value] = $const;
        }
      }
 
      krsort($constants[$prefix]);
    }

    $found = array();
    foreach ($constants[$prefix] as $value => $const)
    {
      if (hasFlag($flags, $value))
      {
        $found[] = $const;
        $flags ^= $value;
      }
    }
    
    return count($found) > 0 ? implode('|', $found) : 0;
  }  

  /**
   * Generates an attributes flags string for a given flags value.
   * 
   * @param int $flags flags
   * 
   * @return string flags string
   */
  protected function getAttributeFlagsString($flags)
  {
    return $this->getFlagsString($flags, 'AF_', array(AF_SPECIFIC_1, AF_SPECIFIC_2, AF_SPECIFIC_3, AF_SPECIFIC_4, AF_SPECIFIC_5), array('AF_UNIX', 'AF_INET', 'AF_INET6'));
  }
  
  /**
   * Generates a node flags string for a given flags value.
   * 
   * @param int $flags flags
   * 
   * @return string flags string
   */
  protected function getNodeFlagsString($flags)
  {
    return $this->getFlagsString($flags, 'NF_', array(NF_SPECIFIC_1, NF_SPECIFIC_2, NF_SPECIFIC_3, NF_SPECIFIC_4, NF_SPECIFIC_5));
  }  
  
  /**
   * Checks if a certain attribute should be included or not. Checks both the
   * include and exclude lists. If the attribute is not part of the table 
   * itself it will always be included (e.g. manually added relations etc.)
   *
   * @param atkMetaPolicy $policy meta policy
   * @param string        $name   attribute name
   * 
   * @return bool attribute included?
   */
  protected function isIncluded(atkMetaPolicy $policy, $name)
  {
    $includes = $policy->getIncludes();
    $excludes = $policy->getExcludes();
    $columns = array_keys($policy->getMetaData());    
    
    if (!in_array($name, $columns)) return true;
    if ($includes !== NULL && in_array($name, $includes)) return true;
    if ($excludes !== NULL && in_array($name, $excludes)) return false;

    return true;
  }    

  /**
   * Compiles the attribute with the given name using the given data.
   *
   * @param atkMetaPolicy $policy meta policy
   * @param string        $name   attribute name
   * @param array         $data   attribute compile data
   * 
   * @param string attribute code
   */
  protected function compileAttribute(atkMetaPolicy $policy, $name, $data)
  {
    $type = $data['type'];
    $params = $data['params'];
    $flags = (int)$data['flags'];
    $tabs = isset($data['tabs']) ? $data['tabs'] : null;
    $order = $data['order'];

    $code = "// attribute: {$name}\n";
    
    if (atkexists("attribute", $type))
      $code .= 'atkuse("attribute", '.var_export($type, true).");\n";
    elseif (atkexists("relation", $type))
      $code .= 'atkuse("relation", '.var_export($type, true).");\n";
    else $code .= 'atkimport('.var_export($type, true).");\n";

    if (strrpos($type, ".") !== FALSE)
      $type = substr($type, strrpos($type, ".") + 1);

    $code .= '$attr = new '.$type.'('.var_export($name, true);
    foreach ($params as $param)
      $code .= ', '.var_export($param, true);
    $code .= ");\n";
    
    if ((atkconfig('debug') > 0 || $policy->getNode()->isCacheable()) && $flags > 0)
    {
      $flags = $this->getAttributeFlagsString($flags);
    }
    
    $code .= "\$attr->addFlag({$flags});\n";
    
    if (isset($data['forceInsert']) && $data['forceInsert'])
    {
      $code .= "\$attr->setForceInsert(true);\n";
    }
    
    if (isset($data['forceUpdate']) && $data['forceUpdate'])
    {
      $code .= "\$attr->setForceUpdate(true);\n";
    }
     
    if (isset($data['default']))
    {
      $code .= '$attr->setInitialValue('.var_export($data['default']).");\n";
    }
    
    $code .= '$node->add($attr, '.var_export($tabs, true).', '.var_export($order, true).");\n\n";
      
    return $code;
  }
  
  /**
   * Compare the order of two attributes.
   *
   * @param array $attr1 attribute data
   * @param array $attr2 attribute data
   * 
   * @return int comparision value
   */
  public static function compareAttributes($attr1, $attr2)
  {
    if ($attr1['order'] < $attr2['order']) return -1;
    else if ($attr1['order'] == $attr2['order']) return 0;
    else return 1;
  }  

  /**
   * Compile attributes.
   *
   * @param atkMetaPolicy $policy meta policy
   * 
   * @return string code
   */
  protected function compileAttributes(atkMetaPolicy $policy)
  {
    $attrs = $policy->getAttributes();
    uasort($attrs, array(__CLASS__, 'compareAttributes'));
    
    $code = '';
    
    if (hasFlag($policy->getNodeFlags(), NF_ML))
    {
      $code .= 
        "useattrib('atkmlselectorattribute');\n".
        "\$node->add(new atkMlSelectorAttribute())\n\n";
    }       
    
    foreach ($attrs as $name => $data) 
    {
      if ($data['type'] == null || !$this->isIncluded($policy, $name))
      {
        continue;
      }
      
      $code .= $this->compileAttribute($policy, $name, $data);
    }

    return $code;    
  }
  
  /**
   * Compile node base.
   *
   * @param atkMetaPolicy $policy meta policy
   * 
   * @return string code
   */
  protected function compileBase(atkMetaPolicy $policy)
  {
    $nodeFlags = (int)$policy->getNodeFlags();
    
    if ((atkconfig('debug') > 0 || $policy->getNode()->isCacheable()) && $nodeFlags > 0)
    {
      $nodeFlags = $this->getNodeFlagsString($nodeFlags);
    }
    
    $code = 
      "// initialize node\n".
      "\$node->setFlags({$nodeFlags});\n".
      '$node->setDb('.var_export($policy->getNodeDatabase(), true).");\n".
      '$node->setTable('.var_export($policy->getNodeTable(), true).', '.var_export($policy->getNodeSequence(), true).");\n";
      
    if ($policy->getNodeDescriptor() != null)
      $code .= '$node->setDescriptorTemplate('.var_export($policy->getNodeDescriptor(), true).");\n";

    if ($policy->getNodeOrder() != null)
      $code .= '$node->setOrder('.var_export($policy->getNodeOrder(), true).");\n";

    if ($policy->getNodeIndex() != null)
      $code .= '$node->setIndex('.var_export($policy->getNodeIndex(), true).");\n";

    if ($policy->getNodeFilter() != null)
      $code .= '$node->addFilter('.var_export($policy->getNodeFilter(), true).");\n";
      
    if ($policy->getNodeSecurityAlias() != null)
      $code .= '$node->setSecurityAlias('.var_export($policy->getNodeSecurityAlias(), true).");\n";

    if ($policy->getNodeSecurityMap() != null)
      $code .= '$node->m_securityMap = array_merge($node->m_securityMap, '.var_export($policy->getNodeSecurityMap(), true).");\n";
    
    $code .= "\n";  
    
    return $code;
  }  
  
  /**
   * Compile meta policy. Returns the node code for the given meta policy.
   *
   * @param atkMetaPolicy $policy meta policy
   * 
   * @return string meta policy
   */
  public function compile(atkMetaPolicy $policy)
  {
    $code = 
      $this->compileBase($policy).
      $this->compileAttributes($policy);
    
    return $code;
  }
}
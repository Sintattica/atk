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
 * @version $Revision: 6320 $
 * $Id: class.atkmetapolicy.inc 6490 2009-09-01 14:14:23Z peter $
 */

/**
 * Fluent interface for modifying the meta policy properties 
 * for attributes.
 * 
 * @author Peter C. Verhage <peter@achievo.org>
 *
 * @package atk
 * @subpackage meta
 */
class atkMetaAttributeModifier
{
	const BEFORE = 1;
	const AFTER  = 2;
	const TOP    = 3;
	const BOTTOM = 4;
	
	/**
	 * Meta policy.
	 * 
	 * @var atkMetaPolicy
	 */
	private $m_policy;
	
	/**
	 * Attribute names.
	 * 
	 * @var array
	 */
	private $m_attrs;
	
	/**
	 * Cosntructor.
	 * 
	 * @param atkMetaPolicy $policy meta policy
	 * @param array         $attrs  attribute names
	 */
	public function __construct($policy, $attrs)
	{
		$this->m_policy = $policy;
		$this->m_attrs = $attrs;
	}

	/**
	 * Returns the meta policy.
	 * 
	 * @return atkMetaPolicy
	 */
	protected function getPolicy()
	{
		return $this->m_policy;
	}
	
	/**
	 * Returns the attribute names.
	 * 
	 * @return array
	 */
	protected function getAttributes()
	{
		return $this->m_attrs;
	}
	
	/**
	 * Add or remove attributes to/from the includes list.
	 * 
	 * The order of the attributes doesn't change!
	 * 
	 * @param boolean $include include?
	 * 
	 * @return atkMetaAttributeModifier
	 */
	public function setIncluded($include=true)
	{
		$order = $this->getPolicy()->getAttributeNames();
		
		if ($include)
		{
      $this->getPolicy()->setIncludes(array_merge($this->getPolicy()->getIncludes(), $this->getAttributes()));
		}
		else
		{
      $this->getPolicy()->setIncludes(array_diff($this->getPolicy()->getIncludes(), $this->getAttributes()));			
		}
		
		$this->setOrder($order);
		
		return $this;
	}
	
  /**
   * Add or remove attributes to/from the excludes list.
   * 
   * @param boolean $exclude exclude?
   * 
   * @return atkMetaAttributeModifier
   */	
	public function setExcluded($exclude)
	{
	  if ($exclude)
    {
      $this->getPolicy()->setExcludes(array_merge($this->getPolicy()->getExcludes(), $this->getAttributes()));
    }
    else
    {
      $this->getPolicy()->setExcludes(array_diff($this->getPolicy()->getExcludes(), $this->getAttributes()));     
    }		
    
    return $this;
	}
	
  /**
   * Set the default value.
   *
   * @param mixed $value default value
   * 
   * @return atkMetaAttributeModifier
   */
  public function setDefaultValue($value)
  {
  	$this->getPolicy()->setDefaultValue($this->getAttributes(), $value);
  	return $this;
  }

  /**
   * Set flag(s).
   * 
   * NOTE: this method will overwrite all currently set flags, including
   *       automatically detected flags!
   *
   * @param int $flags flag(s)
   * 
   * @return atkMetaAttributeModifier
   */
  public function setFlag($flags)
  {
    return $this->setFlags($flags);   
  }  
  
  /**
   * Set flag(s).
   * 
   * NOTE: this method will overwrite all currently set flags, including
   *       automatically detected flags!
   *
   * @param int $flags flag(s)
   * 
   * @return atkMetaAttributeModifier
   */
  public function setFlags($flags)
  {
    $this->getPolicy()->setFlag($this->getAttributes(), $flags);
    return $this;   
  }
  
  /**
   * Add flag(s) .
   *
   * @param int $flags flag(s)
   *  
   * @return atkMetaAttributeModifier
   */
  public function addFlag($flags)
  {
  	return $this->addFlags($flags);
  }  

  /**
   * Add flag(s) .
   *
   * @param int $flags flag(s)
   *  
   * @return atkMetaAttributeModifier
   */
  public function addFlags($flags)
  {
    $this->getPolicy()->addFlags($this->getAttributes(), $flags);
    return $this;   
  }
  
  /**
   * Remove flag(s).

   * @param int $flags flag(s)
   *  
   * @return atkMetaAttributeModifier
   */
  public function removeFlag($flags)
  {
  	return $this->removeFlags($flags);
  }
  
  /**
   * Remove flag(s).

   * @param int $flags flag(s)
   *  
   * @return atkMetaAttributeModifier
   */
  public function removeFlags($flags)
  {
    $this->getPolicy()->removeFlags($this->getAttributes(), $flags);
    return $this;   
  }
  
  /**
   * Enable/disable force insert.
   * 
   * @param boolean $enable enable?
   *  
   * @return atkMetaAttributeModifier
   */
  public function enableForceInsert($enable=true)
  {
    if ($enable)
    {
      $this->getPolicy()->addForceInsert($this->getAttributes());
    }
    else
    {
      $this->getPolicy()->removeForceInsert($this->getAttributes());
    }
    
    return $this;   
  }  

  /**
   * Enable/disable force update.
   * 
   * @param boolean $enable enable?
   *  
   * @return atkMetaAttributeModifier
   */
  public function enableForceUpdate($enable=true)
  {
  	if ($enable)
  	{
      $this->getPolicy()->addForceUpdate($this->getAttributes());
  	}
    else
    {
    	$this->getPolicy()->removeForceUpdate($this->getAttributes());
    }
    
    return $this;   
  }

  /**
   * Set the sections/tabs.
   *
   * @param array|string $tabs tab name(s)
   *  
   * @return atkMetaAttributeModifier
   */
  public function setTab($tabs)
  {
  	$this->getPolicy()->setTab($this->getAttributes(), $tabs);
  	return $this;
  }

  /**
   * Set the sections/tabs.
   *
   * @param array|string $sections section name(s)
   *  
   * @return atkMetaAttributeModifier
   */
  public function setSection($sections)
  {
    $this->getPolicy()->setSection($this->getAttributes(), $sections);
    return $this;
  }
  
  /**
   * Set the column.
   *
   * @param array|string $column colum name
   *  
   * @return atkMetaAttributeModifier
   */
  public function setColumn($column)
  {
  	$this->getPolicy()->setColumn($this->getAttributes(), $column);
  	return $this;
  }
  
  /**
   * Sets the type. All extra arguments after the type argument will be treated 
   * as parameters for the attribute(s). 
   * 
   * If you need to pass arguments by reference you can better use the setTypeAndParams method.
   *
   * @param string $type full ATK attribute class (e.g. atk.attributes.atkboolattribute)
   * @param mixed ...    all other arguments will be treated as parameters
   *  
   * @return atkMetaAttributeModifier
   */
  public function setType($type)
  {
    $params = func_get_args();
    $params = array_slice($params, 1);
    $this->setTypeAndParams($type, $params);
    return $this;
  }
  
  /**
   * Sets the type and parameters.
   *
   * @param string  $type   full ATK attribute class (e.g. atk.attributes.atkboolattribute)
   * @param array   $params parameters for the attribute (optional)
   * 
   * @return atkMetaAttributeModifier
   */
  public function setTypeAndParams($type, $params=array())
  {
  	$this->getPolicy()->setTypeAndParams($this->getAttributes(), $type, $params);
  	return $this;
  }  

  /**
   * Insert.
   * 
   * @param int    $position position
   * @param string $attr     relative to attribute (optional)
   */
  protected function insert($position, $attr=null)
  {
  	// attribute names in their current order
  	$allAttrs = $this->getPolicy()->getAttributeNames();
  	
  	// all attributes, without the attributes we want to insert
  	$attrs = array_values(array_diff($allAttrs, $this->getAttributes()));
  	
  	if ($position == self::TOP)
  	{
  		$attrs = array_merge($this->getAttributes(), $attrs);
  	}
  	else if ($position == self::BOTTOM)
  	{
  		$attrs = array_merge($attrs, $this->getAttributes());
  	}
  	else
  	{
  		$index = array_search($attr, $attrs);
  		$offset = $position == self::BEFORE ? $index : $index + 1;
  		array_splice($attrs, $offset, 0, $this->getAttributes());
  	}
  	
  	$this->getPolicy()->setOrder($attrs);
  	
  	return $this;
  }
  
  /**
   * Insert attribute(s) before the given attribute.
   * 
   * @param string $attr attribute name
   * 
   * @return atkMetaAttributeModifier
   */
  public function insertBefore($attr)
  {
  	return $this->insert(self::BEFORE, $attr);
  }
  
  /**
   * Insert attribute(s) after the given attribute.
   * 
   * @param string $attr attribute name
   * 
   * @return atkMetaAttributeModifier
   */
  public function insertAfter($attr)
  {
    return $this->insert(self::AFTER, $attr);  	
  }
  
  /**
   * Insert attribute(s) at the top.
   * 
   * @return atkMetaAttributeModifier
   */
  public function insertAtTop()
  {
    return $this->insert(self::TOP);  	
  }
  
  /**
   * Insert attribute(s) at the bottom.
   * 
   * @return atkMetaAttributeModifier
   */  
  public function insertAtBottom()
  {
    return $this->insert(self::BOTTOM);  	
  }
}
<?php
/**
 * This file is part of the Achievo ATK distribution.
 * Detailed copyright and licensing information can be found
 * in the doc/COPYRIGHT and doc/LICENSE files which should be 
 * included in the distribution.
 *
 * @package atk
 * @subpackage meta.relations
 *
 * @copyright (c) 2005 Peter C. Verhage <peter@achievo.org>
 * @license http://www.achievo.org/atk/licensing ATK Open Source License
 *
 * @version $Revision$
 * $Id$
 */

/**
 * @internal Include the base class.
 */
userelation("atkmanyboolrelation");
  
/**
 * Implementation of many-to-many relationships using checkboxes for meta nodes.
 *
 * @author Peter C. Verhage <peter@achievo.org>
 * @package atk
 * @subpackage meta.relations
 */        
class atkMetaManyBoolRelation extends atkManyBoolRelation
{
  var $m_accessor;
  var $m_through;
  var $m_options;

  /**
   * Default constructor.    
   */
  function atkMetaManyBoolRelation($name, $accessor, $template, $options, $flags=0)
  {
    $this->atkManyToManyRelation($name, $options['through'], $accessor, $flags);
    $this->m_accessor = $accessor;
    $this->m_through = $options['through'];
    $this->m_options = $options;
    
    $this->setDescriptorTemplate($template);
    if (isset($options["filter"])) 
      $this->setDestinationFilter($options["filter"]);
    if (isset($options["source"]))
      $this->setLocalKey($options["source"]);      
    if (isset($options["remote"]))
      $this->setRemoteKey($options["remote"]);      
    if (isset($options["cols"]))
      $this->setCols($options["cols"]);
    if (isset($options["columns"]))
      $this->setCols($options["columns"]);
  }
  
  /**
   * Create the instance of the destination. 
   *
   * If succesful, the instance is stored in the m_destInstance member variable.
   * 
   * @return boolean true if succesful, false if something went wrong.
   */
  function createDestination()
  {
    if ($this->m_destInstance != "") return TRUE;

    $policy = &$this->m_ownerInstance->getMetaPolicy();

    $this->m_destination = $policy->findDestination($this->m_accessor, true);
    atkdebug("atkMetaShuttleRelation: {$this->m_accessor} destination {$this->m_destination}");
    if ($this->m_destination == NULL) return FALSE;
    else if (!parent::createDestination()) return FALSE;
      
    return TRUE;
  }
  
  /**
   * Create instance of the intermediairy link node.
   *
   * If succesful, the instance is stored in the m_linkInstance member variable.
   * 
   * @return boolean true if succesful, false if something went wrong.
   */
  function createLink()
  {
    if ($this->m_linkInstance != "") return TRUE;

    $policy = &$this->m_ownerInstance->getMetaPolicy();

    $this->m_link = $policy->findDestination($this->m_through, true);
    atkdebug("atkMetaShuttleRelation: {$this->m_through} link {$this->m_destination}");
    if ($this->m_link == NULL) return FALSE;
    else if (!parent::createLink()) return FALSE;
      
    return TRUE;    
  }  
}  
?>
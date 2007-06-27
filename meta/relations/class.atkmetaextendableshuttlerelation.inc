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
userelation("atkextendableshuttlerelation");
  
/**
 * Implementation of many-to-many relationships using a shuttle for meta nodes.
 *
 * @author Peter C. Verhage <peter@achievo.org>
 * @package atk
 * @subpackage meta.relations
 */        
class atkMetaExtendableShuttleRelation extends atkExtendableShuttleRelation
{
  var $m_accessor;
  var $m_through;
  var $m_options;

  /**
   * Default constructor.    
   */
  function __construct($name, $accessor, $template, $options, $flags)
  {
    parent::__construct($name, $options['through'], $accessor, $flags);
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
    atkdebug("atkMetaExtendableShuttleRelation: '{$this->m_accessor}' destination '{$this->m_destination}'");
    
    if ($this->m_destination == NULL) 
    {
      atkerror("No destination found for atkMetaExtendableShuttleRelation with accessor '{$this->m_accessor}'!");
      return FALSE;
    }
    else if (!parent::createDestination()) 
    {
      atkerror("Cannot instantiate destination '{$this->m_destination}' for atkMetaExtendableShuttleRelation with accessor '{$this->m_accessor}'!");        
      return FALSE;
    }      
    
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
    atkdebug("atkMetaExtendableShuttleRelation: {$this->m_through} link {$this->m_destination}");
    
    if ($this->m_link == NULL) 
    {
      atkerror("No link found for atkMetaExtendableShuttleRelation with accessor '{$this->m_through}'!");
      return FALSE;
    }
    else if (!parent::createLink()) 
    {
      atkerror("Cannot instantiate link '{$this->m_link}' for atkMetaExtendableShuttleRelation with accessor '{$this->m_through}'!");        
      return FALSE;
    }  
      
    return TRUE;    
  }  
}  
?>
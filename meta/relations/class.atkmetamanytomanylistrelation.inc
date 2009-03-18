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
userelation("atkmanytomanylistrelation");
  
/**
 * Implementation of many-to-many relationships using checkboxes for meta nodes.
 *
 * @author Peter C. Verhage <peter@achievo.org>
 * @package atk
 * @subpackage meta.relations
 */        
class atkMetaManyToManyListRelation extends atkManyToManyListRelation
{
  private $m_localVariants = null;
  private $m_remoteVariants = null;    
  
  /**
   * Default constructor
   *     
   * @param String $name The name of the relation
   * @param String $destination The full name of the node that is the other
   *                            end of the relation.
   * @param String $through The full name of the node that is used as
   *                     intermediairy node. The intermediairy node is
   *                     assumed to have 2 attributes that are named
   *                     after the nodes at both ends of the relation.
   *                     For example, if node 'project' has a M2M relation
   *                     with 'activity', then the intermediairy node
   *                     'project_activity' is assumed to have an attribute
   *                     named 'project' and one that is named 'activity'.
   *                     You can set your own keys by calling setLocalKey()
   *                     and setRemoteKey()
   * @param string $template The descriptor template
   * @param array $options Array with options
   * @param int $flags Flags for the relation.
   */
  public function __construct($name, $destination, $through, $template, $options, $flags=0)
  {
    parent::__construct($name, $through, $destination, $flags);
    
    $this->setDescriptorTemplate($template);
    if (isset($options["filter"])) 
      $this->setDestinationFilter($options["filter"]);
    if (isset($options["rows"]))
      $this->setRows($options["rows"]);
    if (isset($options["local"]))
      $this->setLocalKey($options["local"]);      
    if (isset($options["localVariants"]))
      $this->m_localVariants = $options['localVariants'];          
    if (isset($options["remote"]))
      $this->setRemoteKey($options["remote"]);      
    if (isset($options["remoteVariants"]))
      $this->m_remoteVariants = $options['remoteVariants'];          
  }
  
  /**
   * Create the instance of the intermedinary link node.
   *
   * If succesful, the instance is stored in the m_linkInstance member variable.
   *
   * @return boolean true if succesful, false if something went wrong.
   */
  public function createLink()
  {
    $result = parent::createLink();
    
    if ($result && is_array($this->m_localVariants))
    {
      foreach ($this->m_localVariants as $variant)
      { 
        if ($this->m_linkInstance->getAttribute($variant) != null)
        {
          $this->setLocalKey($variant);
          break;
        }
      }
      
      $this->m_localVariants = null;
    }
    
    if ($result && is_array($this->m_remoteVariants))
    {
      foreach ($this->m_remoteVariants as $variant)
      { 
        // we *must* use $this->m_linkInstance (not $this->getLink()) else we will create an infinite loop    
        if ($this->m_linkInstance->getAttribute($variant) != null)
        {
          $this->setRemoteKey($variant);
          break;
        }
      }
      
      $this->m_remoteVariants = null;
    }

    return $result;
  }
}
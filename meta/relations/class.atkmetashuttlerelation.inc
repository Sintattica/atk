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
userelation("atkshuttlerelation");
  
/**
 * Implementation of many-to-many relationships using a shuttle for meta nodes.
 *
 * @author Peter C. Verhage <peter@achievo.org>
 * @package atk
 * @subpackage meta.relations
 */        
class atkMetaShuttleRelation extends atkShuttleRelation
{
  /**
   * Default constructor.    
   */
  public function __construct($name, $destination, $through, $template, $options, $flags=0)
  {
    parent::__construct($name, $through, $destination, $flags);
    
    $this->setDescriptorTemplate($template);
    if (isset($options["filter"])) 
      $this->setDestinationFilter($options["filter"]);
    if (isset($options["source"]))
      $this->setLocalKey($options["source"]);      
    if (isset($options["remote"]))
      $this->setRemoteKey($options["remote"]);      
  }
}  
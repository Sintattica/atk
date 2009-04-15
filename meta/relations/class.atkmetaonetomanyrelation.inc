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
 * @version $Revision: 6320 $
 * $Id$
 */

/**
 * @internal Include the base class.
 */
userelation("atkonetomanyrelation");

/**
 * Implementation of one-to-many relationships for meta nodes.
 *
 * @author Peter C. Verhage <peter@achievo.org>
 * @package atk
 * @subpackage meta.relations
 */
class atkMetaOneToManyRelation extends atkOneToManyRelation
{
  private $m_variants = null;
  
  /**
   * Default constructor.
   * 
   * @param String $name The name of the relation
   * @param String $destination The full name of the node that is the other
   *                            end of the relation.
   * @param string $template The descriptor template
   * @param array $options Array with options
   * @param int $flags Flags for the relation.
   */
  public function __construct($name, $destination, $template, $options, $flags=0)
  {
    $refKey = isset($options['dest']) ? $options['dest'] : (isset($options['destination']) ? $options['destination'] : '?');

    parent::__construct($name, $destination, $refKey, $flags);
    
    $this->m_variants = (array)@$options['variants'];

    $this->setDescriptorTemplate($template);

    if (isset($options["filter"])) 
      $this->setDestinationFilter($options["filter"]);
  }

  /**
   * Create the instance of the destination.
   *
   * If succesful, the instance is stored in the m_destInstance member variable.
   *
   * @return boolean true if succesful, false if something went wrong.
   */
  public function createDestination()
  {
    $result = parent::createDestination();
    
    if ($result && is_array($this->m_refKey) && in_array('?', $this->m_refKey))
    {
      foreach ($this->m_variants as $variant)
      { 
        // we *must* use $this->m_destInstance (not $this->getDestination()) else we will create an infinite loop    
        if ($this->m_destInstance->getAttribute($variant) != null)
        {
          $this->m_refKey = array($variant);
          break;
        }
      }
    }
    $this->setGridExcludes($this->m_refKey);

    return $result && $this->m_refKey != NULL;
  }
}
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
userelation("atkmanytoonerelation");

/**
 * Implementation of many-to-one relationships for meta nodes.
 *
 * @author Peter C. Verhage <peter@achievo.org>
 * @package atk
 * @subpackage meta.relations
 */
class atkMetaManyToOneRelation extends atkManyToOneRelation
{
  /**
   * Default constructor.
   */
  public function __construct($name, $destination, $template, $options, $flags=0)
  {
    // we must use $options["source"] (if it is present) as name for the
    // atkManyToOneRelation, because for multi referential keys $name
    // must be array with referential key columns
    if (array_key_exists("source", $options) && is_array($options["source"]))
      $name = $options["source"];

    parent::__construct($name, $destination, $flags);

    $this->setDescriptorTemplate($template);

    if (isset($options["filter"]))
      $this->setDestinationFilter($options["filter"]);

    if (isset($options['join_filter']))
      $this->setJoinFilter($options['join_filter']);

    $cols = array();
    if (array_key_exists('listcols', $options))
      $cols = $options['listcols'];
    else if (array_key_exists('listcolumns', $options))
      $cols = $options['listcolumns'];

    foreach ($cols as $col)
      $this->addListColumn($col);
  }
}
<?php

  usefilter("atkfilter");

/**
 * add a group by clausule to a query
 * use this filter, like you use an attribute, for example:
 * $this->add(new atkGroupByFilter("street_place", "street, place"));
 * 
 * @authors Kees van Dieren <kees@ibuildings.nl>
 *          Ivo Jansch <ivo@ibuildings.nl>
 * @version $Revision$
 * $Id$
 * @copyright Ibuildings.nl BV
 * @package atk
 * @subpackage filters
 */
  class atkGroupByFilter extends atkFilter
  {	
    /**
    * the group by statement
    *
    * @access private
    * @var string groupbystmt
    */
    var $m_groupbystmt;

    /**
    * constructor
    *
    * @param string name
    * @param string groupbystmt
    * @param int flags
    */
    function atkGroupByFilter($name, $groupbystmt, $flags=0)
    {	
      $this->m_groupbystmt = $groupbystmt;
      $this->atkFilter($name, $flags);
    }

    /**
    * add the group by statement to the query
    *
    * @return void
    */
    function addToQuery(&$query)
    {	    
      $query->addGroupBy($this->m_groupbystmt);
    }
  }
?>
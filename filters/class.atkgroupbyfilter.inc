<?php

  /**
   * This file is part of the Achievo ATK distribution.
   * Detailed copyright and licensing information can be found
   * in the doc/COPYRIGHT and doc/LICENSE files which should be 
   * included in the distribution.
   *
   * @package atk
   * @subpackage filters
   *
   * @copyright (c)2000-2004 Ibuildings.nl BV
   * @license http://www.achievo.org/atk/licensing ATK Open Source License
   *
   * @version $Revision$
   * $Id$
   */  

  /** @internal include baseclass */
  usefilter("atkfilter");

  /**
   * Add a group by clausule to a query.
   *
   * Use this filter, like you use an attribute, for example:
   * $this->add(new atkGroupByFilter("street_place", "street, place"));
   * 
   * @author Kees van Dieren <kees@ibuildings.nl>
   * @author Ivo Jansch <ivo@ibuildings.nl>
   * @package atk
   * @subpackage filters
   *
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
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
   * @copyright (c)2007 Ibuildings.nl BV
   * @license http://www.achievo.org/atk/licensing ATK Open Source License
   *
   * @version $Revision: 6320 $
   * $Id$
   */  

  /** @internal include baseclass */
  usefilter("atkfilter");

  /**
   * Add a distinct clause to a query.
   *
   * Use this filter, like you use an attribute, for example:
   * $this->add(new atkDistinctFilter());
   * 
   * @author Ivo Jansch <ivo@ibuildings.nl>
   * @package atk
   * @subpackage filters
   *
   */
  class atkDistinctFilter extends atkFilter
  {	
    /**
    * constructor
    */
    function atkDistinctFilter()
    {	
      $this->atkFilter("distinctfilter");
    }

    /**
    * add the distinct statement to the query
    *
    * @param atkQuery $query The SQL query object
    * @return void
    */
    function addToQuery(&$query)
    {	    
      $query->setDistinct(true);
    }
  }
?>

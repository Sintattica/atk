<?php

  /**
   * This file is part of the Achievo ATK distribution.
   * Detailed copyright and licensing information can be found
   * in the doc/COPYRIGHT and doc/LICENSE files which should be 
   * included in the distribution.
   *
   * @package atk
   * @subpackage db
   *
   * @copyright (c)2000-2004 Ibuildings.nl BV
   * @license http://www.achievo.org/atk/licensing ATK Open Source License
   *
   * @version $Revision$
   * $Id$
   */

  /**   
   * @internal Include baseclass
   */
  require_once(atkconfig("atkroot")."atk/db/class.atkoci8query.inc");
  
  /**
   * SQL Querybuilder for Oracle 8.0.5 databases. 
   *
   * @author Ivo Jansch <ivo@achievo.org>
   * @package atk
   * @subpackage db
   *
   */
  class atkOci805Query extends atkOci8Query
  {

    /**
     * Add the list of order by fields to the SQL query.
     *
     * This oci805 driver overrides the default _addorderby method for the
     * following reason:
     * When doing order by and also setting a limit, the base atkoci8query
     * will perform a subselect to apply the limit. However, Oracle 8.0.5 does
     * not support order by in a subselect.
     * Therefor, if both a limit and order by are set, we do a 'fake' order by
     * by using group by.
     *
     * @access private
     * @param String $query The SQL query that is being constructed.
     */
    function _addOrderBy(&$query)
    {
      if (count($this->m_orderbys)>0 && $this->m_offset >= 0 && $this->m_limit > 0)
      {        
        $groupfields=array();
        
        // first group by fields are the fields we want to sort on.
        for ($i=0, $_i=count($this->m_orderbys); $i<$_i; $i++)
        {
          $fieldalias = $this->m_fieldaliases[$this->m_fields[$i]];          
  
          $groupfields[] = $this->m_orderbys[$i];
        }
        
        for ($i=0;$i<count($this->m_fields);$i++)
        {          
          if (!in_array($this->m_fields[$i], $groupfields)) $groupfields[]=$this->m_fields[$i];
        }
        if (count($groupfields))
        {
          $query.= " GROUP BY ".implode(",", $groupfields);
          $query = str_replace(" DESC", " ", $query); // CENDRIS(ibsticket: 2269) 01-04-2003 Phe:
                                                    // DESC sort doesn't work in the sorting
                                                    // method we use for 805. 
        }
      }    
      else
      {
        // no unsupported condition, just call baseclass version
        parent::_addOrderBy($query);
      }
    }

  }

?>
<?php

 /**
   * Oracle 8.0.5 specific SQL functions.
   * 
   * @author Ivo Jansch (ivo@ibuildings.nl)
   * @version $Revision$
   *
   * $Id$
   *
   */
 
  require_once(atkconfig("atkroot")."atk/db/class.atkoci8query.inc");

  class atkoci805query extends atkoci8query
  {

    /**
      * oci805 overrides the default _addorderby method for the following reason:
      * When doing order by and also setting a limit, atkoci8query will do a 
      * subselect to just select the limit.
      * Oracle 8.0.5 does not support order by in a subselect.
      * Therefor, if both a limit and order by are set, we do a 'fake' order by
      * by using group by.
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
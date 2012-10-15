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
   * @copyright (c)2000-2004 Ivo Jansch
   * @license http://www.achievo.org/atk/licensing ATK Open Source License
   *
   * @version $Revision: 6305 $
   * $Id$
   */
  
  /**
   * Base include
   */
  atkimport("atk.db.atkquery");

  /**
   * SQL Builder for MySQL databases. 
   *
   * @author Ivo Jansch <ivo@achievo.org>
   * @package atk
   * @subpackage db
   *
   */
  class atkMysqlQuery extends atkQuery
  {
    var $m_fieldquote='`';
    
    /**
     * Generate an SQL searchcondition for a regular expression match.
     *
     * @param String $field The fieldname on which the regular expression
     *                      match will be performed.
     * @param String $value The regular expression to search for.
     * @param boolean $inverse Set to false (default) to perform a normal 
     *                         match. Set to true to generate a SQL string 
     *                         that searches for values dat do not match.
     * @return String A SQL regexp expression.
     */ 
    function regexpCondition($field, $value, $inverse=false)
    {
      if($value[0]=='!') 
      {
        return $field." NOT REGEXP '".substr($value,1,atk_strlen($value))."'";
      }
      else
      {
        return $field." REGEXP '$value'";
      }
    }
    
    /**
     * Generate an SQL searchcondition for a soundex match.
     *
     * @param String $field The fieldname on which the soundex match will 
     *                      be performed.
     * @param String $value The value to search for.
     * @param boolean $inverse Set to false (default) to perform a normal 
     *                         match. Set to true to generate a SQL string 
     *                         that searches for values dat do not match.
     * @return String A SQL soundex expression.
     */ 
    function soundexCondition($field, $value, $inverse=false)
    {
      if($value[0]=='!') 
      {
        return "soundex($field) NOT like concat('%',substring(soundex('".substr($value,1,atk_strlen($value))."') from 2),'%')";
      }
      else
      {
        return "soundex($field) like concat('%',substring(soundex('$value') from 2),'%')";
      }
    }
    
    /**
     * Prepare the query for a limit. 
     * @access private
     * @param String $query The SQL query that is being constructed.
     */
    function _addLimiter(&$query)
    {
      if ($this->m_offset >= 0 && $this->m_limit > 0)
      {
        $query .= " LIMIT ".$this->m_offset.", ".$this->m_limit;  
      }      
    }
    
    /**
     * Builds the SQL Select COUNT(*) query. This is different from select,
     * because we do joins, like in a select, but we don't really select the
     * fields.
     *
     * @param boolean $distinct distinct rows?
     *
     * @return String a SQL Select COUNT(*) Query
     */
     function buildCount($distinct = FALSE)
     {
       if (($distinct||$this->m_distinct) && count($this->m_fields) > 0)
       {
        $result = "SELECT COUNT(DISTINCT ";
        $fields = $this->quoteFields($this->m_fields);
        for ($i = 0; $i < count($fields); $i++)
          $fields[$i] = "COALESCE({$fields[$i]}, '###ATKNULL###')";
        $result.=implode($this->quoteFields($fields),", ");
        $result.=") as count FROM ";
       }
       else $result = "SELECT COUNT(*) as count FROM ";

       for ($i=0;$i<count($this->m_tables);$i++)
       {
         $result.= $this->quoteField($this->m_tables[$i]);
         if ($this->m_aliases[$i]!="") $result.=" ".$this->m_aliases[$i];
         if ($i < count($this->m_tables)-1) $result.=", ";
       }

       for ($i=0;$i<count($this->m_joins);$i++)
       {
         $result.=$this->m_joins[$i];
       }

      if (count($this->m_conditions)>0)
      {
         $result.= " WHERE (".implode(") AND (",$this->m_conditions).")";
      }

      if (count($this->m_searchconditions)>0)
      {
        $prefix=" ";
        if(count($this->m_conditions)==0) { $prefix=" WHERE "; } else { $prefix=" AND "; };
        if($this->m_searchmethod==""||$this->m_searchmethod=="AND")
        {
          $result.= $prefix."(".implode(" AND ",$this->m_searchconditions).")";
        }
        else
        {
          $result.= $prefix."(".implode(" OR ",$this->m_searchconditions).")";
        }
      }

       if (count($this->m_groupbys)>0)
       {
         $result.= " GROUP BY ".implode(", ",$this->m_groupbys);
       }
       return $result;
     }    
  }

?>

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
   * @version $Revision: 5.8 $
   * $Id: class.atkmysqlquery.inc,v 5.8 2006/10/10 12:41:55 peter Exp $
   */
  
  /**
   * Base include
   */
  atkimport("atk.db.atkquery");

  /**
   * SQL Builder for IBM db2 databases. 
   *
   * @author Harrie Verveer <harrie@ibuildings.nl>
   * @package atk
   * @subpackage db
   *
   */
  class atkDb2Query extends atkQuery
  {
    var $m_fieldquote='';
    
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
     * 
     * @todo convert to db2
     */ 
    function regexpCondition($field, $value, $inverse=false)
    {
      if($value[0]=='!') 
      {
        return $field." NOT REGEXP '".substr($value,1,strlen($value))."'";
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
     * 
     * @todo convert to db2 (is this ever used anyway???)
     */ 
    function soundexCondition($field, $value, $inverse=false)
    {
      if($value[0]=='!') 
      {
        return "soundex($field) NOT like concat('%',substring(soundex('".substr($value,1,strlen($value))."') from 2),'%')";
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
     * 
     * @todo add  limit statement in db2 style (limit 10,5 will not work)
     */
    function _addLimiter(&$query)
    {
      if ($this->m_offset >= 0 && $this->m_limit > 0)
      {
        // although DB2 doesn't support the following LIMIT syntax, we add it anyway,
        // in class.atkdb2db.inc we remove this line and use it's values to limit
        // the query in a somewhat different way
        $query .= "\nLIMIT ".$this->m_limit." OFFSET ".$this->m_offset;
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
        $result.= " WHERE ".implode(" AND ",$this->m_conditions);
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
     
    function &addField($name, $value="", $table="", $fieldaliasprefix="", $quote=true, $mode="", $fieldType="")
    {
      if ($table!="") $fieldname = $table.".".$name;
      else $fieldname = $name;

      if (strtoupper($fieldType) == 'DATETIME' && $mode != 'add' && $mode != 'update')
      {
        $this->m_aliasLookup["al_".$this->m_generatedAlias] = $fieldaliasprefix.$name;
        $fieldname = "TO_CHAR(".$fieldname.",'YYYY-MM-DD HH24:MI:SS')";

        $this->m_fieldaliases[$fieldname] = "al_".$this->m_generatedAlias;
        $this->m_generatedAlias++;
      }

      $this->m_fields[] = $fieldname;

      if (strtoupper($fieldType) == 'DATETIME' && ($mode == 'add' || $mode == 'update'))
      {
        $value = "TO_TIMESTAMP('".$value."','YYYY-MM-DD HH24:MI:SS')";
      }
      elseif (strtoupper($fieldType) == 'TEXT' && ($mode == 'add' || $mode == 'update'))
      {
        //binding needed for CLOB
        if($value != "")
          $this->m_bind_vars[$name] = $value;
        $value = "EMPTY_CLOB()";
      }
      elseif ($quote||$value=="") $value = "'".$value."'";

      $this->m_values[$fieldname] = $value;

      if ($fieldaliasprefix!="" && !(strtoupper($fieldType) == 'DATETIME' && $mode != 'add' && $mode != 'update'))
      {
        $this->m_aliasLookup["al_".$this->m_generatedAlias] = $fieldaliasprefix.$name;
        $this->m_fieldaliases[$fieldname] = "al_".$this->m_generatedAlias;

        $this->m_generatedAlias++;
      }

      return $this;
    }     
  }

?>
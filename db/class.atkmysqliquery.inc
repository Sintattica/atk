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
   * @copyright (c)2005 Ibuildings.nl BV
   * @license http://www.achievo.org/atk/licensing ATK Open Source License
   *
   * @version $Revision$
   * $Id$
   */

 /**
  * @internal Include baseclass
  */
 require_once(atkconfig("atkroot")."atk/db/class.atkmysqlquery.inc");

  /**
   * SQL Builder for MySQL 4.1+ databases.
   *
   * @author Rene van den Ouden <rene@ibuildings.nl>
   * @package atk
   * @subpackage db
   *
   */
  class atkMysqliQuery extends atkMysqlQuery
  {
    /**
     * Overriding the _addFrom function to support a change that was made in
     * MySQL 5.0.15 to make MySQL more compliant with the standard.
     *
     * See: http://bugs.mysql.com/bug.php?id=13551
     *
     * @param string $query
     */
    function _addFrom(&$query)
    {
      $query.= " FROM (";
      for ($i=0;$i<count($this->m_tables);$i++)
      {
        $query.= $this->quoteField($this->m_tables[$i]);
        if ($this->m_aliases[$i]!="") $query.=" ".$this->m_aliases[$i];
        if ($i < count($this->m_tables)-1) $query.=", ";
      }
      $query.= ") ";
    }

    /**
     * Builds the SQL Insert query
     * @return String a SQL Insert Query
     */
    function buildInsert()
    {
      
      $result = "INSERT INTO ".$this->quoteField($this->m_tables[0])." (";

      for ($i=0;$i<count($this->m_fields);$i++)
      {
        $result.= $this->quoteField($this->m_fields[$i]);
        if ($i < count($this->m_fields)-1) $result.=",";
      }

      $result.=") VALUES (";

      for ($i=0;$i<count($this->m_fields);$i++)
      {
        if (($this->m_values[$this->m_fields[$i]] === "''") and ($this->m_db->m_tableMeta[$this->m_tables[0]][$this->m_fields[$i]]["type"] == "int")){
          atkdebug("atkMysqliQuery::buildInsert() : '' transformed in '0' for MySQL5 compatibility in field '" . $this->m_fields[$i] . "'");
          $result.= "'0'";
        }else{
          $result.= $this->m_values[$this->m_fields[$i]];
        }
        if ($i < count($this->m_fields)-1) $result.=",";
      }

      $result.=")";

      return $result;
    }
    
  }

?>

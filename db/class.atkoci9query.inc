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

  /** @internal include base class */
  require_once(atkconfig("atkroot")."atk/db/class.atkoci8query.inc");

  /**
   * Query builder for Oracle 9i and later databases.
   *
   * @author Sandy Pleyte <sandy@ibuildings.nl>
   * @package atk
   * @subpackage db
   *
   */
  class atkOci9Query extends atkOci8Query
  {

    var $m_bind_vars=array();  // array containing name and value of fields which
                               // need to be binded in update and insert queries
                               // Like CLOB fields

    /**
     * Add's a field to the query
     * @param string $name Field name
     * @param string $value Field value
     * @param string $table Table name
     * @param string $fieldaliasprefix Field alias prefix
     * @param string $quote If this parameter is true, stuff is inserted into the db
     *               using quotes, e.g. SET name = 'piet'. If it is false, it's
     *               done without quotes, e.d. SET number = 4.
     * @param string $mode
     * @param string $fieldType
     * @return atkQuery The query object, for fluent usage.
     */
    function &addField($name, $value="", $table="", $fieldaliasprefix="", $quote=true, $mode="", $fieldType="")
    {
      //$this->m_fields[] = strtr($name,"_",".");
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

    /**
     * Makes a Join SQL query for Oracle
     *
     * @param $table Table name
     * @param $alias Alias for the table
     * @param $condition Condition for the join
     * @param bool $outer Wether to use an outer (left) join or an inner join
     * @return atkQuery The query object (for fluent usage)
     */
    function &addJoin($table, $alias, $condition, $outer)
    {
      // $this->m_joins[]= ", ".$table." ".$alias." WHERE ".$condition." ";

      //Because oracle doesn't accept aliases more than 30 characters
      //we have to replace al aliases with new aliases
      //First save the old alias en create a new one
      //bug 137
      if ($table != $alias)
      {
        $genAlias = "al_".$this->m_generatedAlias;
        //hmm... alias to an alias
        $this->m_aliasLookup[$genAlias] = $alias;
        $this->m_joinaliases[$alias] = $genAlias;
      }
      else $alias = "";

      $this->m_generatedAlias++;

      if ($outer)
      {
        $join="LEFT OUTER JOIN ";
      }
      else
      {
        $join="JOIN ";
        //$this->m_joins[]= " ".$join.$table." ".$alias;
        //$this->addCondition($condition);
      }
      $this->m_joins[]= " ".$join.$table." ".$alias." ON ".$condition." ";

      return $this;
    }

    /**
     * Builds the SQL Select query
     * @param $distinct distinct records?
     * @return String SQL Query
     */
    function buildSelect($distinct = FALSE)
    {
      $query = parent::buildSelect($distinct);

      //Because oracle doesn't accept aliases more than 30 characters
      //we replace al aliases with new aliases
      //bug 137
      $result = $this->convertQuery($query);

      return $result;
    }

     /**
      * Builds the SQL Select COUNT(*) query. This is different from select,
      * because we do joins, like in a select, but we don't really select the
      * fields.
      *
      * @param $distinct distinct rows?
      *
      * @return String a SQL Select COUNT(*) Query
      */
     function buildCount($distinct = FALSE)
     {
      $query = "SELECT COUNT(*) AS count FROM (".$this->buildSelect($distinct).")";
      return $query;
     }

 /**
    * Converts the SQL Select query. All aliases which
    * have been added with addJoin are replaced with
    * generated aliases
    * bug #137
    * @param $query query string
    * @return String SQL query
    */
    function convertQuery($query)
    {
      $query = str_replace("\\'","''",$query);
      $explodedQuery = explode("'",$query);
      for($i=0,$count=count($explodedQuery);$i<$count;$i+=2)
      {
        if ($explodedQuery[$i]!="")
          $explodedQuery[$i] = $this->replaceAliases($explodedQuery[$i]);
      }
      $result = implode("'",$explodedQuery);
      return $result;
    }

 /**
    * Search patterns which are aliases and replace patterns
    * @param $str to search
    * @return $new str
    */
    function replaceAliases($str)
    {
      $pattern = array();
      $replace = array();

      $pattern = array_keys($this->m_joinaliases);
      if (count($pattern))
      {
        //usort($pattern, "sortSearchPattern");
        sort($pattern);

        for($i=0, $count=count($pattern);$i<$count;$i++)
        {
          $replace[$i] = "\\1".$this->m_joinaliases[$pattern[$i]]."\\2";
          //$replace[$i+$count] = "\\1".$this->m_joinaliases[$pattern[$i]]."\\2";
          //$pattern[$i] = "/([\s,\(=])".$pattern[$i]."(([\.])|(\s*[,])|(\s+))/";
          $pattern[$i] = "/([\s,\(=])".$pattern[$i]."(([\.])|(\s*[,])|(\s+((WHERE)|(JOIN)|(ON))))/";
          //$pattern[$i] = "/([,])".$pattern[$i]."(|(\s*[,])|(\s+))/";
          //$pattern[$i+$count] = "/([\s\(=])".$pattern[$i]."(([\.])|(\s+WHERE))/i";
        }
        $str = preg_replace($pattern, $replace, $str);
      }
      return $str;
    }

  /**
    * Wrapper function to execute a query
    */
    function executeUpdate()
    {
      if (!isset($this->m_db)) $this->m_db = &atkGetDb();
      $query = $this->buildUpdate();
      return $this->m_db->query($query, 0, 0, $this->m_bind_vars);
    }

 /**
    * Wrapper function to execute a query
    */
    function executeInsert()
    {
      if (!isset($this->m_db)) $this->m_db = &atkGetDb();
      $query = $this->buildInsert();
      return $this->m_db->query($query, 0, 0, $this->m_bind_vars);
    }


  }

?>
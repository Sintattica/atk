<?php

/**
 * Oracle 9 specific SQL functions.
 * 
 * @author Sandy Pleyte (sandy@ibuildings.net)
 * @version $Revision$
 *
 * $Id$
 *
 */
 
  require_once(atkconfig("atkroot")."atk/db/class.atkoci8query.inc");

  class atkoci9query extends atkoci8query
  {

    var $m_bind_vars=array();  // array containing name and value of fields which 
                               // need to be binded in update and insert queries
                               // Like CLOB fields
    
    /**
    * Add's a field to the query
    * @param $name Field name
    * @param $value Field value
    * @param $table Table name
    * @param $fieldaliasprefix Field alias prefix
    * @param $quote If this parameter is true, stuff is inserted into the db
    *               using quotes, e.g. SET name = 'piet'. If it is false, it's
    *               done without quotes, e.d. SET number = 4.
    */
    function addField($name, $value="", $table="", $fieldaliasprefix="", $quote=true, $mode="", $fieldType="")
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
    }


 /**
    * Makes a Join SQL query for Oracle 
    * 
    * @param $table Table name
    * @param $alias Alias for the table
    * @param $condition Condition for the join
    * @since ATK 0.5.2 
    */ 
    
    function addJoin($table, $alias, $condition, $outer)
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

    }
    
 /**
    * Builds the SQL Select query
    * @result a SQL Select Query
    * @param $distinct distinct records?
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
     * @result a SQL Select COUNT(*) Query
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
    * @result a SQL Select Query
    * @param $query query string
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
      usort($pattern, "sortSearchPattern");
    
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
      return $str;
    }

  /**
    * Wrapper function to execute a query 
    */
    function executeUpdate()
    {
      global $g_db;
      
      $query = $this->buildUpdate();
      return $g_db->query($query, 0, 0, $this->m_bind_vars);      
    }

 /**
    * Wrapper function to execute a query 
    */
    function executeInsert()
    {
      global $g_db;
      
      $query = $this->buildInsert();
      return $g_db->query($query, 0, 0, $this->m_bind_vars);      
    }


  }

?>

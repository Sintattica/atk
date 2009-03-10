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
   * @version $Revision$
   * $Id$
   */

  /**
   * Base include
   */
  atkimport("atk.db.atkmysqlquery");

  /**
   * SQL Builder for MSSQL databases.
   *
   * @author Harrie Verveer <harrie@ibuildings.nl>
   * @package atk
   * @subpackage db
   *
   */
  class atkMssqlQuery extends atkMysqlQuery
  {
    /**
     * Generate an SQL searchcondition for a regular expression match.
     *
     * @param String $field The fieldname on which the regular expression
     *                      match will be performed.
     * @param String $value The regular expression to search for.
     * @param boolean $inverse Set to false (default) to perform a normal
     *                         match. Set to true to generate a SQL string
     *                         that searches for values dat do not match.
     * @todo fixme
     * @return String A SQL regexp expression.
     */
    function regexpCondition($field, $value, $inverse=false)
    {
      // void implementation
      return "";
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
     * @todo fixme
     * @return String A SQL soundex expression.
     */
    function soundexCondition($field, $value, $inverse=false)
    {
      // void implementation
      return "";
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
        // although MSSQL doesn't support the following LIMIT syntax, we add it anyway,
        // in class.atkmssqldb.inc we remove this line and use it's values to limit
        // the query in a somewhat different way
        $query .= "\nLIMIT ".$this->m_limit." OFFSET ".$this->m_offset;
      }
    }

    /**
     * Add's a field to the query
     * @param string $name Field name
     * @param string $value Field value
     * @param string $table Table name
     * @param string $fieldaliasprefix Field alias prefix
     * @param bool $quote If this parameter is true, stuff is inserted into the db
     *               using quotes, e.g. SET name = 'piet'. If it is false, it's
     *               done without quotes, e.d. SET number = 4.
     * @param bool $quotefield Wether or not to quote the fieldname
     * @return atkQuery The query object itself (for fluent usage)
     */
    function &addField($name, $value="", $table="", $fieldaliasprefix="", $quote=true)
    {
      if (strstr($name," "))
        $name = "[$name]";

      if ($table!="") $fieldname = $table.".".$name;
      else $fieldname = $name;
      $this->m_fields[] = $fieldname;

      if ($quote||$value=="") $value = "'".$value."'";

      $this->m_values[$fieldname] = $value;

      if ($fieldaliasprefix!="")
      {
        $this->m_aliasLookup["al_".$this->m_generatedAlias] = $fieldaliasprefix.$name;
        $this->m_fieldaliases[$fieldname] = "al_".$this->m_generatedAlias;

        $this->m_generatedAlias++;
      }

      return $this;
    }
  }

?>

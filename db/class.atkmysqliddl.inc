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
  * @version $Revision: 6323 $
  * $Id$
  *
  */


 /**
  * @internal Include baseclass
  */
 require_once(atkconfig("atkroot")."atk/db/class.atkmysqlddl.inc");

 /**
  * MySQL 4.1+ ddl driver.
  *
  * Implements mysql specific ddl statements.
  *
  * @author Rene van den Ouden <rene@ibuildings.nl>
  * @package atk
  * @subpackage db
  *
  */
 class atkMysqliDDL extends atkMysqlDDL
 {
   /**
    * Convert an database specific type to an ATK generic datatype.
    *
    * @param string $type  The database specific datatype to convert.
    */
   function getGenericType($type)
    {
      $type = strtolower($type);
      switch($type)
      {
        case MYSQLI_TYPE_TINY:
        case MYSQLI_TYPE_SHORT:
        case MYSQLI_TYPE_LONG:
        case MYSQLI_TYPE_LONGLONG:
        case MYSQLI_TYPE_INT24:
        case MYSQLI_TYPE_YEAR:
          return "number";
        case MYSQLI_TYPE_DECIMAL:
        case MYSQLI_TYPE_NEWDECIMAL:
        case MYSQLI_TYPE_FLOAT:
        case MYSQLI_TYPE_DOUBLE:
          return "decimal";        
        case MYSQLI_TYPE_VAR_STRING:
        case MYSQLI_TYPE_STRING:
          return "string";          
        case MYSQLI_TYPE_DATE:
          return "date";
        case MYSQLI_TYPE_TINY_BLOB:
        case MYSQLI_TYPE_MEDIUM_BLOB:
        case MYSQLI_TYPE_LONG_BLOB:
        case MYSQLI_TYPE_BLOB:
          return "text";
        case MYSQLI_TYPE_TIME:
          return "time";
        case MYSQLI_TYPE_TIMESTAMP:
        case MYSQLI_TYPE_DATETIME:
          return "datetime";      
        case MYSQLI_TYPE_NEWDATE:
        case MYSQLI_TYPE_ENUM:
        case MYSQLI_TYPE_SET:         
        case MYSQLI_TYPE_GEOMETRY:
          return ""; // NOT SUPPORTED FIELD TYPES 
      }
      return ""; // in case we have an unsupported type.      
    }
    
    /**
     * Build CREATE VIEW query
     *
     * @param string $name - name of view
     * @param string $select - SQL SELECT statement
     * @param string $with_check_option - use SQL WITH CHECK OPTION
     * @return string CREATE VIEW query string
     */
    function buildView($name, $select, $with_check_option)
    {
      return "CREATE VIEW $name AS ".$select.($with_check_option ? " WITH CHECK OPTION" : "");
    }

    /**
     * Build DROP VIEW query
     *
     * @param string $name - name of view
     * @return string CREATE VIEW query string
     */
    function dropView($name)
    {
      return "DROP VIEW $name";
    }
   
 }
?>

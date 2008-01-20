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
        case MYSQLI_TYPE_YEAR:
        case MYSQLI_TYPE_NEWDATE:
        case MYSQLI_TYPE_ENUM:
        case MYSQLI_TYPE_SET:         
        case MYSQLI_TYPE_GEOMETRY:
          return ""; // NOT SUPPORTED FIELD TYPES 
      }
      return ""; // in case we have an unsupported type.      
    }
    
    function buildView($name, $select)
    {
      return "CREATE VIEW $name AS ".$select;
    }

    function executeCreateView($name, $select)
    {
      if (!isset($this->m_db)) $this->m_db = &atkGetDb();

      $query = $this->buildView($name, $select);
      if ($query!="")
      {
        return $this->m_db->query($query);
      }
      else
      {
        atkdebug("atkddl::executeCreateView: nothing to do!");
      }
      return false;
    }
   
 }
?>

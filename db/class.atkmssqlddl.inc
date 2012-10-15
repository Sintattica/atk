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
  * MSSQL ddl driver.
  *
  * Implements mssql specific ddl statements.
  *
  * @author Harrie Verveer <harrie@ibuildings.nl>
  * @package atk
  * @subpackage db
  *
  */
class atkMsSqlDDL extends atkDDL
{
  /**
   * Constructor
   *
   * @return atkMsSqlDDL
   */
  function atkMsSqlDDL()
  {
  }

  /**
   * Convert an ATK generic datatype to a database specific type.
   *
   * @param string $generictype  The datatype to convert.
   */
  function getType($generictype)
  {
    switch ($generictype)
    {
      case "number":
        return "INT";
      case "decimal":
        return "DECIMAL";
      case "string":
        return "VARCHAR";
      case "date":
        return "DATETIME";
      case "text":
        return "TEXT";
      case "datetime":
        return "DATETIME";
      case "time":
        return "TIMESTAMP";
      case "boolean":
        return "BIT"; // size is added fixed. (because a boolean has no size of its own)
    }
    return ""; // in case we have an unsupported type.
  }

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
      case "int":
      case "number":
        return "number";
      case "float":
      case "decimal":
      case "real":
        return "decimal";
      case "varchar":
      case "char":
      case "string":
        return "string";
      case "date":
        return "date";
      case "text":
      case "blob":
        return "text";
      case "time":
      case "timestamp":
        return "time";
      case "datetime":
        return "datetime";
      case "bit":
        return "boolean";
    }
    return ""; // in case we have an unsupported type.
  }
}
?>

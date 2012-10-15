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
   * @version $Revision: 6323 $
   * $Id$
   */
   
  /**
   * PostgreSQL ddl driver. 
   *
   * @author Peter C. Verhage <peter@ibuildings.nl>
   * @package atk
   * @subpackage db
   *
   */  
  class atkPgsqlDDL extends atkDDL
  {
    /**
     * Constructor
     *
     * @return atkPgsqlDDL
     */
    function atkPgsqlDDL()
    {
      $this->atkDDL();
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
          return "INT4";
        case "decimal":
          return "FLOAT8";
        case "string":
          return "VARCHAR";          
        case "date":
          return "DATE";
        case "text":
          return "TEXT";
        case "datetime":
          return "TIMESTAMP";
        case "time":
          return "TIME";          
        case "boolean":        
          return "BOOLEAN"; 
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
        case "int2":
        case "int4":
        case "int8":
          return "number";
        case "float":
        case "float8":
        case "float16":
        case "numeric":
          return "decimal";        
        case "varchar":
        case "char":
          return "string";          
        case "date":
          return "date";
        case "time":
          return "time";
        case "text":
          return "text";
        case "timestamp":
        case "datetime":
          return "datetime";  
        case "boolean":
          return "boolean";
      }
      return ""; // in case we have an unsupported type.      
    }

    /**
     * Method to determine whether a given generic field type needs
     * to have a size defined.
     *
     * @param string $generictype The type of field.
     * @return bool true  if a size should be specified for the given field type.
     *         false if a size does not have to be specified.
     */
    function needsSize($generictype)
    {
      switch($generictype)
      {
        case "string":
          return true;
          break;
        default:
          return false;
      }
      return false; // in case we have an unsupported type.
    }

    /**
     * Build one or more ALTER TABLE queries and return them as an array of
     * strings.
     *
     * @return An array of ALTER TABLE queries.
     */
    function buildAlter()
    {
      $result = array();

      if ($this->m_table!="")
      {
        // PostgreSQL only supports ALTER TABLE statements which
        // add a single column or constraint.

        $fields = array();
        $notNullFields = array();

        // At this time PostgreSQL does not support NOT NULL constraints
        // as part of the field construct, so a separate ALTER TABLE SET NULL
        // statement is needed.
        foreach ($this->m_fields as $fieldname => $fieldconfig)
        {
          if ($fieldname!="" && $fieldconfig["type"]!="" && $this->getType($fieldconfig["type"])!="")
          {
            $fields[] = $this->buildField($fieldname, $fieldconfig["type"], $fieldconfig["size"], $fieldconfig["flags"]&~DDL_NOTNULL, $fieldconfig["default"]);
            if (hasFlag($fieldconfig["flags"], DDL_NOTNULL))
              $notNullFields[] = $fieldname;
          }
        }

        foreach ($fields as $field)
          $result[] = "ALTER TABLE ".$this->m_table." ADD ".$field;

        foreach ($notNullFields as $field)
          $result[] = "ALTER TABLE ".$this->m_table." ALTER COLUMN ".$field." SET NOT NULL";

        $constraints = $this->_buildConstraintsArray();
        foreach ($constraints as $constraint)
          $result[] = "ALTER TABLE ".$this->m_table." ADD ".$constraint;
      }

      return count($result) > 0 ? $result : "";
    }
  }
?>

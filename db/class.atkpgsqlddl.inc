<?php

  /**
   * PostgreSQL specific implementations of the aktDDL utility class.
   *
   * @author Peter C. Verhge <peter@achievo.org>
   * @version $Revision$
   *
   * $Id$
   */
  class atkpgsqlddl extends atkDDL
  {
    function atkpgsqlddl()
    {
      $this->atkDDL();
    }
    
    function getType($generictype)
    {
      switch ($generictype)
      {
        case "number": 
          return "INTEGER";
        case "decimal":
          return "NUMERIC";
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
    
    function getGenericType($type)
    {
      $type = strtolower($type);
      switch($type)
      {
        case "int": 
          return "number";
        case "float":
          return "decimal";        
        case "varchar":
        case "char":
          return "string";          
        case "date":
          return "date";
        case "text":
          return "text";
        case "datetime":
          return "datetime";                
      }
      return ""; // in case we have an unsupported type.      
    }

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
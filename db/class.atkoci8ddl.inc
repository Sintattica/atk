<?php

  /**
   * Oracle8i specific implementations of the aktDDL utility class.
   *
   * @author Martin Roest <martin@ibuildings.nl>
   *
   * $Id$
   */

  class atkoci8ddl extends atkDDL
  {
    
    function atkoci8ddl()
    {
    }
    
    function getType($generictype)
    {
      switch ($generictype)
      {
        case "number": 
          return "NUMBER";
        case "decimal":
          return "FLOAT";
        case "string":
          return "VARCHAR2";          
        case "date":
          return "DATE";
        case "text":
          return "CLOB";
        case "datetime":
          return "DATE";
        case "time":
          return "DATE";          
        case "boolean":        
          return "NUMBER(1,0)"; // size is added fixed. (because a boolean has no size of its own)          
      }
      return ""; // in case we have an unsupported type.      
    }

    /**
     * Convert an database specific type to an ATK generic datatype.
     * 
     * This function will be overrided by the database specific subclasses of
     * atkDb.     
     * 
     * @param $type  The database specific datatype to convert.
     */
    function getGenericType($type)
    {      
      $type = strtolower($type);
      switch($type)
      {
        case "number": 
          return "number";        
        case "char":
        case "varchar2":
          return "string";          
        case "date":
          return "date";
        case "clob":
          return "text";
        case "date":
          return "date";                
      }
      return ""; // in case we have an unsupported type.      
    }
  }
?>
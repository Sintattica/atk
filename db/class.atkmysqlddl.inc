<?php

  /**
   * MySQL specific implementations of the aktDDL utility class.
   *
   * @author Ivo Jansch <ivo@achievo.org>
   * @version $Revistion$
   *
   * $Id$
   */

  class atkmysqlddl extends atkDDL
  {
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
          return "DATE";
        case "text":
          return "TEXT";
        case "datetime":
          return "DATETIME";
        case "time":
          return "TIME";          
        case "boolean":        
          return "NUMBER(1,0)"; // size is added fixed. (because a boolean has no size of its own)          
      }
      return ""; // in case we have an unsupported type.      
    }
  }

?>
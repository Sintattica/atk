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
   
  /**
   * MySQL ddl driver. 
   *
   * Implements mysql specific ddl statements.
   *
   * @author Ivo Jansch <ivo@achievo.org>
   * @package atk
   * @subpackage db
   *
   */
  class atkMysqlDDL extends atkDDL
  {
    
    function atkMysqlDDL()
    {
    }
    
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
    
    function getGenericType($type)
    {
      $type = strtolower($type);
      switch($type)
      {
        case "int": 
          return "number";
        case "float":
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
          return "time";
        case "datetime":
          return "datetime";                
      }
      return ""; // in case we have an unsupported type.      
    }
    
    /**
     * Generate a string for a field, to be used inside a CREATE TABLE
     * statement. 
     * This function tries to be generic, so it will work in the largest
     * number of databases. Databases that won't work with this syntax,
     * should override this method in the database specific ddl class.
     *
     * @param $name        The name of the field 
     * @param $generictype The datatype of the field (should be one of the 
     *                     generic types supported by ATK).
     * @param $size        The size of the field (if appropriate)
     * @param $flags       The DDL_ flags for this field. 
     * @param $default     The default value to be used when inserting new
     *                     rows.
     */
    function buildField($name, $generictype, $size=0, $flags=0, $default=NULL)
    {
      if ($generictype == "string" && $size > 255)
        $generictype = "text";
        
      return parent::buildField($name, $generictype, $size, $flags, $default);
    }
  }
?>
<?php

  /**
   * Some flags that can be used to configure database fields.
   */
  define("DDL_PRIMARY"       , 1);
  define("DDL_UNIQUE"        , 2);
  define("DDL_NOTNULL"       , 4);
  //define("DDL_AUTO_INCREMENT", 8); not yet implemented..
  //define("DDL_AUTOINCREMENT" , 8);

  /**
   * The Data Definition Language utility class.
   *
   * This class can be used to create and execute CREATE TABLE statements.
   *
   * @todo - support for the other databases (oci, postgresql)
   *       - alter table support. 
   *
   * @author Ivo Jansch <ivo@achievo.org>
   * 
   * @version $Revision$
   *
   * $Id$
   */
  class atkDDL
  {
    var $m_table = array();
    var $m_fields = array();    
    var $m_indexes = array(); // not yet implemented
    var $m_primarykey = array();
  
    /** 
     * Default constructor
     */
    function atkDDL()
    {      
    }
    
    /**
     * Static factory method for creating a new atkDDL instance. This static 
     * method will determine the database type (mysql, oci, etc) and 
     * instantiate the correct DDL class.
     */
    function &create()
    {
      $database = atkconfig("database");
      
      $filename = atkconfig("atkroot")."atk/db/class.atk".$database."ddl.inc";
      if (file_exists($filename))
      {
        include_once($filename);
        $classname = "atk".$database."DDL";
        return new $classname;
      }
      else
      {
        atkerror("atkddl::create: No support for $database!");
      }
      return NULL;
    }
    
    /**
     * Add a field to the table definition.
     *
     * @param $name        The name of the field 
     * @param $generictype The datatype of the field (should be one of the 
     *                     generic types supported by ATK).
     * @param $size        The size of the field (if appropriate)
     * @param $flags       The DDL_ flags for this field. 
     * @param $default     The default value to be used when inserting new
     *                     rows.
     */
    function addField($name, $generictype, $size=0, $flags=0, $default=NULL)
    {
      if (hasFlag($flags, DDL_PRIMARY))
      {
        $this->m_primarykey[] = $name;
        $flags|=DDL_NOTNULL; // primary keys may never be null.
      }
      $this->m_fields[$name] = array("type"=>$generictype, 
                                     "size"=>$size,
                                     "flags"=>$flags,
                                     "default"=>$default);      
    }
    
    /**
     * Convert an ATK generic datatype to a database specific type.
     * 
     * This function will be overrided by the database specific subclasses of
     * atkDDL.
     * Note: in all derived subclasses, the following types *must* be 
     * supported: number, decimal, string, date, text, datetime, time, 
     * boolean.
     * If the database does not have a proper field type, consider using
     * a varchar or number to store the value.
     * 
     * @param $generictype  The datatype to convert.
     */
    function getType($generictype)
    {
      return "";
    }
    
    /**
     * Set the name of the table.
     *
     * @param $tablename  The name of the table
     */
    function setTable($tablename)
    {
      $this->m_table = $tablename;
    }      
    
    /**
     * Build a CREATE TABLE query and return it as a string.
     *
     * @return The CREATE TABLE query.
     */
    function buildCreate()
    {  
      if ($this->m_table!="")
      {
        $fields = $this->buildFields();  
    
        if ($fields!="")
        {
          $q = "CREATE TABLE ".$this->m_table."\n(";
        
          $q.= $fields;
          
          $constraints = $this->buildConstraints();
          
          if ($constraints!="")
          {       
            $q.= ",\n".$constraints;
          }
       
          $q.= ")";
        }
        return $q;
      }
      return "";
    }
    
    /**
     * Build one or more ALTER TABLE queries and return them as an array of
     * strings.
     *
     * The default implementation assumes that multiple fields can be added
     * with one single ALTER TABLE statement. If a database needs to be
     * supported which doesn't have this ability, then an override for this 
     * function should be implemented in the appropriate atk<database>ddl 
     * class.
     *
     * @return An array of ALTER TABLE queries.
     */
    function buildAlter()
    {  
      if ($this->m_table!="")
      {
        $fields = $this->buildFields();  
    
        if ($fields!="")
        {
          $q = "ALTER TABLE ".$this->m_table." ADD\n (";
        
          $q.= $fields;
          
          $constraints = $this->buildConstraints();
          
          if ($constraints!="")
          {       
            $q.= ",\n".$constraints;
          }
       
          $q.= ")";
        }
        return array($q); 
      }
      return "";
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
      $res = $name." ".$this->getType($generictype);
      if ($size>0 && $this->needsSize($generictype))
      {
        $res.="(".$size.")";
      }
      if ($default!==NULL)
      {
        if ($this->needsQuotes($generictype))
        {
          $default = "'".$default."'";
        }
        $res.= " DEFAULT ".$default;
      }
      if (hasFlag($flags, DDL_NOTNULL))
      {
        $res.= " NOT NULL";
      }      
      
      return $res;
    }
    
    /**
     * Generate a string that defines the primary key, for use
     * inside the CREATE TABLE statement.
     * 
     * This function will be overrided by the database specific subclasses of
     * atkDDL.
     *
     * @param $fieldlist  An array of fields that define the primary key.
     */
    function buildPrimaryKey($fieldlist=array())
    {
      if (count($fieldlist)>0)
      {
        return "PRIMARY KEY (".implode(", ",$fieldlist).")";
      }
      return "";
    }
    
    /**
     * Method to determine whether quotes are needed around the values
     * for a given generic datatype.
     *
     * @param $generictype  The type of field.
     * @return true  if quotes should be put around values for the given type
     *               of field.
     *         false if quotes should not be used.
     */
    function needsQuotes($generictype)
    {
      return !($generictype=="number"||$generictype=="decimal");
    }
    
    /**
     * Method to determine whether a given generic field type needs
     * to have a size defined.
     * 
     * @param $generictype The type of field.
     * @return true  if a size should be specified for the given field type.
     *         flase if a size does not have to be specified.
     */
    function needsSize($generictype)
    { 
      switch($generictype)
      {
        case "number": 
        case "decimal":
        case "string":
          return true;
          break;
        case "date":
        case "text":
        case "datetime":
        case "time":
        case "boolean":
          return false; 
          break;
      }
      return false; // in case we have an unsupported type.
    }
    
    /**
     * Convert all fields to a string that can be used in a CREATE TABLE 
     * statement.
     */
    function buildFields()
    {
      $strfields = array();
      foreach ($this->m_fields as $fieldname => $fieldconfig)
      {
        if ($fieldname!="" && $fieldconfig["type"]!="" && $this->getType($fieldconfig["type"])!="")
        {
          $strfields[] = $this->buildField($fieldname, $fieldconfig["type"], $fieldconfig["size"], $fieldconfig["flags"], $fieldconfig["default"]);
        }
      }
      if (count($strfields)>0)
      {
        return implode(",\n", $strfields);
      }
      return "";
    }
    
    /**
     * Convert all constraints to a string that can be used in a CREATE TABLE
     * statement.
     */
    function buildConstraints()
    {
      $res = "";
      
      $res.= $this->buildPrimaryKey($this->m_primarykey);
      
      return $res;
    }
    
    /**
     * Build and execute the CREATE TABLE query. 
     *
     * @return true  if the table was created successfully
     *         false if anything went wrong, or if no table could be created.
     */
    function executeCreate()
    {
      global $g_db; 
      $query = $this->buildCreate();
      if ($query!="")
      {
        $g_db->query($query);        
        return true;
      }
      else
      {
        atkdebug("atkddl::executeCreate: nothing to do!");
      }
      return false;
    }
    
    /**
     * Build and execute ALTER TABLE queries. 
     *
     * Note that more than one query might be performed, depending on the
     * number of fields added, and the database capabilities (some databases
     * are capable of adding several fields in one ALTER TABLE query, others
     * aren't and need to perform multiple queries).
     *
     * @return true  if the table was altered successfully
     *         false if anything went wrong, or if no table could be altered.
     */
    function executeAlter()
    {
      global $g_db;
      $queries = $this->buildAlter();      
      if (count($queries)>0)
      {
        for ($i=0, $_i=count($queries); $i<$_i; $i++)
        {
          $g_db->query($queries[$i]);
        }
        return true;
      }
      else
      {
        atkdebug("atkddl::executeCreate: nothing to do!");
      }
      return false;
    }
      
  }

?>
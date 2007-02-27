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
   * Some flags that can be used to configure database fields.
   */
  define("DDL_PRIMARY"       , 1);
  define("DDL_UNIQUE"        , 2);
  define("DDL_NOTNULL"       , 4);
  //define("DDL_AUTO_INCREMENT", 8); //not yet implemented..
  //define("DDL_AUTOINCREMENT" , 8);

  /**
   * The Data Definition Language abstract base class.
   *
   * Database drivers should derive a class from this base class to implement
   * vendor specific ddl commands.
   *
   * @author Ivo Jansch <ivo@achievo.org>
   * @package atk
   * @subpackage db
   * @abstract
   */
  class atkDDL
  {
    var $m_table = array();
    var $m_fields = array();
    var $m_remove_field;
    var $m_indexes = array(); // not yet implemented
    var $m_primarykey = array();
    var $m_db;

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
     *
     * @return atkDDL instance of db specific DDL driver
     */
    function &create($database=NULL)
    {
      $db = atkconfig("db");
      $database = $database === NULL ? $db["default"]["driver"] : $database;

      $filename = atkconfig("atkroot")."atk/db/class.atk".$database."ddl.inc";
      if (file_exists($filename))
      {
        include_once($filename);
        $classname = "atk".$database."DDL";
        $ddlobject = new $classname;
        return $ddlobject;
      }
      else
      {
        atkerror("atkddl::create: No support for $database!");
      }
      return NULL;
    }

    /**
     * Set all table data at once using the given table meta data,
     * retrieved using the metadata function of the db instance.
     *
     * @param $tablemeta table meta data array
     */
    function loadMetaData($tablemeta)
    {
      $this->setTable($tablemeta[0]["table"]);
      $this->addFields($tablemeta);
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
     * Drop a field from the table definition.
     *
     * @param $name        The name of the field
     */
    function dropField($name)
    {
      $this->m_remove_field=$name;
    }

    /**
     * Add multiple fields at once using the given metadata.
     *
     * NOTE: defaults are not supported yet!
     *
     * @param $meta   The fields meta data.
     */
    function addFields($meta)
    {
      foreach ($meta as $field)
      {
        $flags  = hasFlag($field["flags"], MF_PRIMARY)  ? DDL_PRIMARY : 0;
        $flags |= hasFlag($field["flags"], MF_UNIQUE)   ? DDL_UNIQUE  : 0;
        $flags |= hasFlag($field["flags"], MF_NOT_NULL) ? DDL_NOTNULL : 0;
        // $flags |= hasFlag($field["flags"], MF_AUTO_INCREMENT) ? DDL_AUTO_INCREMENT : 0;

        $this->addField($field["name"], $field["gentype"], $field["len"], $flags);
      }
    }

    /**
     * Convert an ATK generic datatype to a database specific type.
     *
     * This function will be overrided by the database specific subclasses of
     * atkDb.
     * Note: in all derived subclasses, the following types *must* be
     * supported: number, decimal, string, date, text, datetime, time,
     * boolean.
     * If the database does not have a proper field type, consider using
     * a varchar or number to store the value.
     *
     * @param $generictype  The datatype to convert.
     * @abstract
     */
    function getType($generictype)
    {
      return ""; // in case we have an unsupported type.
    }

    /**
     * Convert an database specific type to an ATK generic datatype.
     *
     * This function will be overrided by the database specific subclasses of
     * atkDb.
     *
     * @param $type  The database specific datatype to convert.
     * @abstract
     */
    function getGenericType($type)
    {
      return ""; // in case we have an unsupported type.
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
      atkimport('atk.db.atkquery');
      $query = &atkQuery::create();
      if ($this->m_table!="")
      {
        $fields = $this->buildFields();

        if ($fields!="" || $this->m_remove_field)
        {
          $q = "ALTER TABLE ".$this->m_table;

          if ($this->m_remove_field)
          {
            $q.=" DROP\n {$query->m_fieldquote}$this->m_remove_field{$query->m_fieldquote}";
          }
          else
          {
            $q.=" ADD\n (";


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
      }
      return "";
    }

    /**
     * Build a DROP TABLE query and return it as a string.
     *
     * @return The DROP TABLE query.
     */
    function buildDrop()
    {
      if ($this->m_table!="")
      {
        $q = "DROP TABLE `".$this->m_table."`";
        return $q;
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
     * Convert all fields to string that can be used in a CREATE or ALTER
     * TABLE statement. Fields will be returned in an array. (INTERNAL USE ONLY)
     *
     * @access private
     */
    function _buildFieldsArray()
    {
      $fields = array();

      foreach ($this->m_fields as $fieldname => $fieldconfig)
      {
        if ($fieldname!="" && $fieldconfig["type"]!="" && $this->getType($fieldconfig["type"])!="")
        {
          $fields[] = $this->buildField($fieldname, $fieldconfig["type"], $fieldconfig["size"], $fieldconfig["flags"], $fieldconfig["default"]);
        }
      }

      return $fields;
    }

    /**
     * Convert all fields to a string that can be used in a CREATE or ALTER
     * TABLE statement.
     *
     * @return String containing fields to be used in a CREATE or ALTER TABLE statement
     */
    function buildFields()
    {
      $fields = $this->_buildFieldsArray();
      if (count($fields) > 0)
        return implode(",\n", $fields);
      else return "";
    }

    /**
     * Convert all constraints to an array that can be used in a CREATE or
     * ALTER TABLE statement.
     *
     * @return An array of constraints
     * @access private
     */
    function _buildConstraintsArray()
    {
      $constraints = array();
      $pk = $this->buildPrimaryKey($this->m_primarykey);
      if (!empty($pk)) $constraints[] = $pk;
      return $constraints;
    }

    /**
     * Convert all constraints to a string that can be used in a CREATE TABLE
     * statement.
     *
     * @return String containing constraints to be used in a CREATE or ALTER TABLE statement
     */
    function buildConstraints()
    {
      $constraints = $this->_buildConstraintsArray();
      if (count($constraints) > 0)
        return implode(",\n", $constraints);
      else return "";
    }

    /**
     * Build and execute the CREATE TABLE query.
     *
     * @return true  if the table was created successfully
     *         false if anything went wrong, or if no table could be created.
     */
    function executeCreate()
    {
      if (!isset($this->m_db)) $this->m_db = &atkGetDb();

      $query = $this->buildCreate();
      if ($query!="")
      {
        return $this->m_db->query($query);
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
      if (!isset($this->m_db)) $this->m_db = &atkGetDb();

      $queries = $this->buildAlter();
      if (count($queries)>0)
      {
        for ($i=0, $_i=count($queries); $i<$_i; $i++)
        {
          if ($queries[$i]!="")
          {
            if (!$this->m_db->query($queries[$i]))
              return false;
          }
        }
        return true;
      }
      else
      {
        atkdebug("atkddl::executeCreate: nothing to do!");
      }
      return false;
    }

    /**
     * Build and execute the DROP TABLE query.
     *
     * @return true  if the table was dropped successfully
     *         false if anything went wrong, or if no table could be dropped.
     */
    function executeDrop()
    {
      if (!isset($this->m_db)) $this->m_db = &atkGetDb();

      $query = $this->buildDrop();
      if ($query!="")
      {
        return $this->m_db->query($query);
      }
      else
      {
        atkdebug("atkddl::executeDrop: nothing to do!");
      }
      return false;
    }
  }

?>
<?php

namespace Sintattica\Atk\Db;

use Sintattica\Atk\Core\Config;

/**
 * MySQL 4.1+ ddl driver.
 *
 * Implements mysql specific ddl statements.
 *
 * @author Rene van den Ouden <rene@ibuildings.nl>
 */
class MySqliDdl extends Ddl
{
    public function __construct()
    {
        parent::__construct();
    }

    /**
     * Convert an ATK generic datatype to a database specific type.
     *
     * @param string $generictype The datatype to convert.
     */
    public function getType($generictype)
    {
        $config = Config::getGlobal('db_mysql_default_'.$generictype.'_columntype');
        if ($config) {
            return $config;
        }

        switch ($generictype) {
            case 'number':
                return 'INT';
            case 'decimal':
                return 'DECIMAL';
            case 'string':
                return 'VARCHAR';
            case 'date':
                return 'DATE';
            case 'text':
                return 'TEXT';
            case 'datetime':
                return 'DATETIME';
            case 'time':
                return 'TIME';
            case 'boolean':
                return 'NUMBER(1,0)'; // size is added fixed. (because a boolean has no size of its own)
        }

        return ''; // in case we have an unsupported type.
    }

    /**
     * Convert an database specific type to an ATK generic datatype.
     *
     * @param string $type The database specific datatype to convert.
     *
     * @return string
     */
    public function getGenericType($type)
    {
        $type = strtolower($type);
        switch ($type) {
            case MYSQLI_TYPE_TINY:
            case MYSQLI_TYPE_SHORT:
            case MYSQLI_TYPE_LONG:
            case MYSQLI_TYPE_LONGLONG:
            case MYSQLI_TYPE_INT24:
            case MYSQLI_TYPE_YEAR:
                return 'number';
            case MYSQLI_TYPE_DECIMAL:
            case MYSQLI_TYPE_NEWDECIMAL:
            case MYSQLI_TYPE_FLOAT:
            case MYSQLI_TYPE_DOUBLE:
                return 'decimal';
            case MYSQLI_TYPE_VAR_STRING:
            case MYSQLI_TYPE_STRING:
                return 'string';
            case MYSQLI_TYPE_DATE:
                return 'date';
            case MYSQLI_TYPE_TINY_BLOB:
            case MYSQLI_TYPE_MEDIUM_BLOB:
            case MYSQLI_TYPE_LONG_BLOB:
            case MYSQLI_TYPE_BLOB:
                return 'text';
            case MYSQLI_TYPE_TIME:
                return 'time';
            case MYSQLI_TYPE_TIMESTAMP:
            case MYSQLI_TYPE_DATETIME:
                return 'datetime';
            case MYSQLI_TYPE_NEWDATE:
            case MYSQLI_TYPE_ENUM:
            case MYSQLI_TYPE_SET:
            case MYSQLI_TYPE_GEOMETRY:
                return ''; // NOT SUPPORTED FIELD TYPES 
        }

        return ''; // in case we have an unsupported type.      
    }

    /**
     * Build CREATE VIEW query.
     *
     * @param string $name - name of view
     * @param string $select - SQL SELECT statement
     * @param string $with_check_option - use SQL WITH CHECK OPTION
     *
     * @return string CREATE VIEW query string
     */
    public function buildView($name, $select, $with_check_option)
    {
        return "CREATE VIEW $name AS ".$select.($with_check_option ? ' WITH CHECK OPTION' : '');
    }

    /**
     * Build DROP VIEW query.
     *
     * @param string $name - name of view
     *
     * @return string CREATE VIEW query string
     */
    public function dropView($name)
    {
        return "DROP VIEW $name";
    }

    /**
     * Generate a string for a field, to be used inside a CREATE TABLE
     * statement.
     * This function tries to be generic, so it will work in the largest
     * number of databases. Databases that won't work with this syntax,
     * should override this method in the database specific ddl class.
     *
     * @param string $name The name of the field
     * @param string $generictype The datatype of the field (should be one of the
     *                            generic types supported by ATK).
     * @param int $size The size of the field (if appropriate)
     * @param int $flags The self::DDL_ flags for this field.
     * @param mixed $default The default value to be used when inserting new
     *                            rows.
     */
    public function buildField($name, $generictype, $size = 0, $flags = 0, $default = null)
    {
        if ($generictype == 'string' && $size > 255) {
            $generictype = 'text';
        }

        $result = parent::buildField($name, $generictype, $size, $flags, $default);

        // add binary option after varchar declaration to make sure field
        // values are compared in case-sensitive fashion
        if ($generictype == 'string') {
            $result = preg_replace('/VARCHAR\(([0-9]+)\)/i', 'VARCHAR(\1) BINARY', $result);
        }

        return $result;
    }

    /**
     * Set all table data at once using the given table meta data,
     * retrieved using the metadata function of the db instance.
     *
     * @param array $tablemeta table meta data array
     */
    public function loadMetaData($tablemeta)
    {
        parent::loadMetaData($tablemeta);
        $this->setTableType($tablemeta[0]['table_type']);
    }

    /**
     * Sets the table type (for databases that support different
     * table types).
     *
     * @param string $type
     */
    public function setTableType($type)
    {
        $this->m_table_type = $type;
    }

    /**
     * Build a CREATE TABLE query and return it as a string.
     *
     * @return The CREATE TABLE query.
     */
    public function buildCreate()
    {
        $query = parent::buildCreate();

        if (!empty($this->m_db->m_charset)) {
            $query .= ' DEFAULT CHARACTER SET '.$this->m_db->m_charset;
            if (!empty($this->m_db->m_collate)) {
                $query .= ' COLLATE '.$this->m_db->m_collate;
            }
        }

        if (!empty($query) && !empty($this->m_table_type)) {
            $query .= ' TYPE='.$this->m_table_type;
        }

        return $query;
    }

    /**
     * Drop sequence.
     *
     * @param string $name Sequence name
     *
     * @return bool
     */
    public function dropSequence($name)
    {
        $table = $this->m_db->quoteIdentifier($this->db->m_seq_table);

        return $this->m_db->query("DELETE FROM $table WHERE ".$this->m_db->quoteIdentifier($this->m_db->m_seq_namefield)." = '".$this->escapeSQL($name)."'");
    }

    /**
     * Rename sequence.
     *
     * @param string $name Sequence name
     * @param string $new_name New sequence name
     *
     * @return bool
     */
    public function renameSequence($name, $new_name)
    {
        $name = $this->m_db->escapeSQL($name);
        $new_name = $this->m_db->escapeSQL($new_name);

        return $this->m_db->query("UPDATE db_sequence SET seq_name='$new_name'
                WHERE seq_name='$name'");
    }

    /**
     * Rename table name.
     *
     * @param string $name Table name
     * @param string $new_name New table name
     *
     * @return bool
     */
    public function renameTable($name, $new_name)
    {
        $name = $this->m_db->quoteIdentifier($name);
        $new_name = $this->m_db->quoteIdentifier($new_name);

        return $this->m_db->query("ALTER TABLE $name RENAME $new_name");
    }
}

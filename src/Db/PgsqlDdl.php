<?php

namespace Sintattica\Atk\Db;

use Sintattica\Atk\Core\Tools;

/**
 * PostgreSQL ddl driver.
 *
 * @author Peter C. Verhage <peter@ibuildings.nl>
 */
class PgsqlDdl extends Ddl
{
    /**
     * Constructor.
     */
    public function __construct()
    {
        parent::__construct();
    }

    /**
     * Convert an ATK generic datatype to a database specific type.
     *
     * @param string $generictype The datatype to convert.
     *
     * @return string
     */
    public function getType($generictype)
    {
        switch ($generictype) {
            case Db::FT_NUMBER:
                return 'INT4';
            case Db::FT_DECIMAL:
                return 'FLOAT8';
            case Db::FT_STRING:
                return 'VARCHAR';
            case Db::FT_DATE:
                return 'DATE';
            case Db::FT_DATETIME:
                return 'TIMESTAMP';
            case Db::FT_TIME:
                return 'TIME';
            case Db::FT_BOOLEAN:
                return 'BOOLEAN';
        }

        return ''; // in case we have an unsupported type.      
    }

    /**
     * Method to determine whether a given generic field type needs
     * to have a size defined.
     *
     * @param string $generictype The type of field.
     *
     * @return bool true  if a size should be specified for the given field type.
     *              false if a size does not have to be specified.
     */
    public function needsSize($generictype)
    {
        switch ($generictype) {
            case 'string':
                return true;
                break;
            default:
                return false;
        }
    }

    /**
     * Build one or more ALTER TABLE queries and return them as an array of
     * strings.
     *
     * @return array of ALTER TABLE queries.
     */
    public function buildAlter()
    {
        $result = [];

        if ($this->m_table != '') {
            // PostgreSQL only supports ALTER TABLE statements which
            // add a single column or constraint.

            $fields = [];
            $notNullFields = [];

            // At this time PostgreSQL does not support NOT NULL constraints
            // as part of the field construct, so a separate ALTER TABLE SET NULL
            // statement is needed.
            foreach ($this->m_fields as $fieldname => $fieldconfig) {
                if ($fieldname != '' && $fieldconfig['type'] != '' && $this->getType($fieldconfig['type']) != '') {
                    $fields[] = $this->buildField($fieldname, $fieldconfig['type'], $fieldconfig['size'], $fieldconfig['flags'] & ~self::DDL_NOTNULL,
                        $fieldconfig['default']);
                    if (Tools::hasFlag($fieldconfig['flags'], self::DDL_NOTNULL)) {
                        $notNullFields[] = $fieldname;
                    }
                }
            }

            foreach ($fields as $field) {
                $result[] = 'ALTER TABLE '.$this->m_table.' ADD '.$field;
            }

            foreach ($notNullFields as $field) {
                $result[] = 'ALTER TABLE '.$this->m_table.' ALTER COLUMN '.$field.' SET NOT NULL';
            }

            $constraints = $this->_buildConstraintsArray();
            foreach ($constraints as $constraint) {
                $result[] = 'ALTER TABLE '.$this->m_table.' ADD '.$constraint;
            }
        }

        return Tools::count($result) > 0 ? $result : '';
    }
}
